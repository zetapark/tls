#include<tuple>
#include<cmath>
#include<complex>
#include<random>
#include<cassert>
#include<valarray>
#include"cvmatrix.h"
#include"combi.h"
using namespace std;
using namespace cv;

static string haar_dir = "/usr/share/opencv/haarcascades/";
static string lbp_dir = "/usr/share/opencv/lbpcascades/";
static const char* haar_cascade[] = {
	"haarcascade_mcs_mouth.xml",
	"haarcascade_eye_tree_eyeglasses.xml",
	"haarcascade_mcs_eyepair_small.xml",
	"haarcascade_lefteye_2splits.xml",
	"haarcascade_frontalface_alt_tree.xml",
	"haarcascade_frontalface_default.xml",
	"haarcascade_mcs_eyepair_big.xml",
	"haarcascade_mcs_upperbody.xml",
	"haarcascade_mcs_righteye.xml",
	"haarcascade_smile.xml",
	"haarcascade_righteye_2splits.xml",
	"haarcascade_mcs_nose.xml",
	"haarcascade_upperbody.xml",
	"haarcascade_mcs_leftear.xml",
	"haarcascade_profileface.xml",
	"haarcascade_mcs_lefteye.xml",
	"haarcascade_frontalface_alt2.xml",
	"haarcascade_eye.xml",
	"haarcascade_fullbody.xml",
	"haarcascade_mcs_rightear.xml",
	"haarcascade_lowerbody.xml",
	"haarcascade_frontalface_alt.xml"
};
static const char* lbp_cascade[] = {
	"lbpcascade_frontalface.xml", 
	"lbpcascade_profileface.xml", 
	"lbpcascade_silverware.xml"
};

CVMat::CVMat(const cv::Mat& r) : cv::Mat{r} 
{
	save_ = clone();
	split(*this, bgr_);
}

void CVMat::scale(float x, float y) {
	cv::resize(*this, *this, Size{x * cols, y * rows});
}

void CVMat::rotate(double angle, Point center, double scale) {
	if(center == Point{-1,-1}) center = {cols/2, rows/2};
	warpAffine(*this, *this, getRotationMatrix2D(center, angle, scale), size());
}

void CVMat::transform3(Point2f src[3], Point2f dst[3], Size sz) {
	warpAffine(*this, *this, getAffineTransform(src, dst), (sz == Size{0,0}) ? size() : sz);
}
void CVMat::detect_contours(int mode, int method) 
{
	findContours(*this, contours_, hierachy_, mode, method);
}
void CVMat::draw_detected_contours(int thickness, int linetype, int maxlevel) 
{
	static uniform_int_distribution<> di{0, 255};
	static random_device rd;
	for(int i=0; i<contours_.size(); i++)
		drawContours(*this, contours_, i, {di(rd), di(rd), di(rd)}, thickness,
				linetype, hierachy_, maxlevel);
}
void CVMat::transform4(Point2f src[4], Point2f dst[4], Size sz)
{
	warpPerspective(*this, *this, getPerspectiveTransform(src, dst), (sz == Size{0,0}) ? size() : sz);
}

vector<Point> CVMat::get_points(int k)
{//gray -> edge -> get_points
	Mat tmp;
	copyTo(tmp);
	gray();
	filter(GAUSSIAN);
	auto med = cv::mean(*this);
	edge(std::max(0., med[0]) * 0.7, std::min(255., med[0] * 1.33));
	
	detect_contours(RETR_EXTERNAL);
	
	vector<vector<Point>> vapprox;
	for(auto a : contours_) {
		vector<Point> approx;
		approxPolyDP(Mat(a), approx, arcLength(Mat(a), true)*0.01, true);
		if(approx.size() == k && isContourConvex(approx) && 
				fabs(contourArea(Mat(approx))) > 10000) vapprox.push_back(approx);
	}
	tmp.copyTo(*this);
	return vapprox.empty() ? vector<Point>{{cols/3,rows/3}, {cols/3,2*rows/3},
		{2*cols/3,2*rows/3}, {2*cols/3,rows/3}} : 
		*max_element( vapprox.begin(), vapprox.end(), [](vector<Point> a, vector<Point> b) {
			return fabs(contourArea(Mat(a))) < fabs(contourArea(Mat(b))); });
}

void CVMat::get_businesscard(vector<Point> v)
{
	Point2f xy[4] = {{9000,9000}, {0, 1000}, {1000, 0}, {0,0}};
	for(auto &[x, y] : v) {
		if(x + y < xy[0].x + xy[0].y) xy[0] = {x, y};
		if(x - y > xy[1].x - xy[1].y) xy[1] = {x, y};
		if(y - x > xy[2].y - xy[2].x) xy[2] = {x, y};
		if(x + y > xy[3].x + xy[3].y) xy[3] = {x, y};
	}
	const int width = 630, height = 360;
	Point2f dst[4] = {{0,0}, {width-1, 0}, {0,height-1}, {width-1, height-1}};
	transform4(xy, dst, {width, height});//{420, 240});
}

vector<DMatch> CVMat::match(const CVMat& r, double thres) const
{
//	cout << "desc: " << descriptor_.size() << ", " << r.descriptor_.size() << endl;
//	BFMatcher matcher;
//	vector<DMatch> matches;
//	matcher.match(descriptor_, r.descriptor_, matches);
//	cout << "match size " << matches.size() << endl;
//
//	double max_dist = 0; double min_dist = 1000000;
//
//	//-- Quick calculation of max and min distances between keypoints
//	for( int i = 0; i < matches.size(); i++ ) { 
//		double dist = matches[i].distance;
//		if( dist < min_dist ) min_dist = dist;
//		if( dist > max_dist ) max_dist = dist;
//	}
//
//	printf("-- Max dist : %f \n", max_dist );
//	printf("-- Min dist : %f \n", min_dist );
//	double threshold = min_dist + (max_dist - min_dist) * thres;
//
//	//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
//	std::vector< DMatch > good_matches;
//
//	for( int i = 0; i < matches.size(); i++ ) 
//		if(matches[i].distance < threshold) good_matches.push_back( matches[i]);
//	cout << "good matches : " << good_matches.size() << endl;
//	Mat img_matches;
//	drawMatches( *this, keypoints_, r, r.keypoints_, good_matches, img_matches,
//			Scalar::all(-1), Scalar::all(-1), vector<char>(), 2 );
//	//-- Localize the object
//	std::vector<Point2f> obj;
//	std::vector<Point2f> scene;
//
//	for( int i = 0; i < good_matches.size(); i++ ) {
//		//-- Get the keypoints from the good matches
//		obj.push_back( keypoints_[ good_matches[i].queryIdx ].pt );
//		scene.push_back( r.keypoints_[ good_matches[i].trainIdx ].pt );
//	}
//	Mat H = findHomography( obj, scene, CV_RANSAC );
//
//	//-- Get the corners from the image_1 ( the object to be "detected" )
//	std::vector<Point2f> obj_corners(4);
//	obj_corners[0] = cvPoint(0,0); 
//	obj_corners[1] = cvPoint( cols, 0 );
//	obj_corners[2] = cvPoint( cols, rows ); 
//	obj_corners[3] = cvPoint( 0, rows );
//
//	std::vector<Point2f> scene_corners(4);
//	perspectiveTransform( obj_corners, scene_corners, H);
//
//	//-- Draw lines between the corners (the mapped object in the scene - image_2 )
//	line( img_matches, scene_corners[0] + Point2f(cols, 0), scene_corners[1] + Point2f(cols, 0), Scalar(0, 255, 0), 4 );
//	line( img_matches, scene_corners[1] + Point2f(cols, 0), scene_corners[2] + Point2f(cols, 0), Scalar( 0, 255, 0), 4 );
//	line( img_matches, scene_corners[2] + Point2f(cols, 0), scene_corners[3] + Point2f(cols, 0), Scalar( 0, 255, 0), 4 );
//	line( img_matches, scene_corners[3] + Point2f(cols, 0), scene_corners[0] + Point2f(cols, 0), Scalar( 0, 255, 0), 4 );
//	imshow("match", img_matches);
//	return good_matches;
}

template<typename T> void CVMat::feature()
{
//	T module;//T : BRISK or ORB
//	module(*this, noArray(), keypoints_, descriptor_);
//	KeyPointsFilter::removeDuplicated(keypoints_);
//	KeyPointsFilter::retainBest(keypoints_, 100);
	cout << keypoints_.size() << " keypoints found" << endl;
}

void CVMat::draw_feature()
{
	drawKeypoints(*this, keypoints_, *this);
}

void CVMat::corner(float k, int b, int a)
{
	cornerHarris(*this, harris_, b, a, k, BORDER_DEFAULT);
}

void CVMat::draw_detected_corner(float thresh) {
	int n = 0;
	for(int j=0; j<harris_.rows ; j++ ) for(int i=0; i<harris_.cols; i++ ) {
		if(harris_.at<float>(j,i) > thresh) {
			n++;
			circle(*this, Point( i, j ), 5,  {0,0,255}, 2, 8, 0 );
		}
	}
	cout << n << " corners detected" << endl;
}

pair<int, int> CVMat::text(string s, Point p, double scale, Scalar color, int thickness, int fontFace, int lintType, bool bottomleftorigin)
{
	putText(*this, s, p, fontFace, scale,color, thickness, lintType, bottomleftorigin);
	int baseline = 0;
	Size sz = getTextSize(s, fontFace, scale, thickness, &baseline);
	return {sz.width, sz.height};
}

CVMat CVMat::background()
{
//	static cv::BackgroundSubtractorMOG2 mog2{10, 16, false}; 
//	CVMat mat{Mat()};
//	mog2(*this, mat);
//	return mat;
}

void CVMat::detect_face()
{
	equalizeHist(*this, *this);
	cv::CascadeClassifier cas;
	cas.load(haar_dir + haar_cascade[16]);//alt2
	cas.detectMultiScale(*this, faces_, 1.1, 3, cv::CASCADE_SCALE_IMAGE, {30,30});
	cout << faces_.size() << " faces detected" << endl;
}

void CVMat::draw_detected_face()
{
	for(auto& a : faces_) rectangle(*this, a, {0,0,255});
}

CVMat::operator cv::Mat() 
{
	Mat r;
	r = *this;
	return r;
}

MatND CVMat::histo(string window)
{
	int histSize[] = {256}; // hue varies from 0 to 179, see cvtColor 
	float hranges[] = { 0, 255 }; 
	const float* ranges[] = { hranges }; 
	MatND hist; // we compute the histogram from the 0-th and 1-st channels 
	int channels[] = {0}; 
	calcHist( this, 1, channels, Mat(), // do not use mask 
			hist, 1, histSize, ranges);//, true, false ); 
	double maxVal=0, minVal=0; 
	minMaxLoc(hist, &minVal, &maxVal, 0, 0);
	Mat histImg{histSize[0], histSize[0], CV_8U, Scalar(255)}; 
	int hpt = static_cast<int>(0.9 * histSize[0]);
	for( int h = 0; h < histSize[0]; h++ ) {
		float binVal = hist.at<float>(h);
		int intensity = static_cast<int>(binVal*hpt/maxVal);
	
		line(histImg, {h, histSize[0]}, {h, histSize[0] - intensity}, Scalar::all(0));
	} 
	imshow(window, histImg);
	return hist;
}

void CVMat::fourier(string window)
{
//	using namespace cv;
//    Mat padded;                            //expand input image to optimal size
//    int m = getOptimalDFTSize( rows );
//    int n = getOptimalDFTSize( cols ); // on the border add zero values
//    copyMakeBorder(*this, padded, 0,m-rows,0,n-cols, BORDER_CONSTANT, Scalar::all(0));
//
//    Mat planes[] = {Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F)};
//    Mat complexI;
//    merge(planes, 2, complexI);  // Add to the expanded another plane with zeros
//
//    dft(complexI, complexI);     // this way the result may fit in the source matrix
//
//    // compute the magnitude and switch to logarithmic scale
//    // => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
//    split(complexI, planes);       // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
//    magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
//    Mat magI = planes[0];
//
//    magI += Scalar::all(1);                    // switch to logarithmic scale
//    log(magI, magI);
//
//    // crop the spectrum, if it has an odd number of rows or columns
//    magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));
//
//    // rearrange the quadrants of Fourier image  so that the origin is at the image center
//    int cx = magI.cols/2;
//    int cy = magI.rows/2;
//
//    Mat q0(magI, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
//    Mat q1(magI, Rect(cx, 0, cx, cy));  // Top-Right
//    Mat q2(magI, Rect(0, cy, cx, cy));  // Bottom-Left
//    Mat q3(magI, Rect(cx, cy, cx, cy)); // Bottom-Right
//
//    Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
//    q0.copyTo(tmp);
//    q3.copyTo(q0);
//    tmp.copyTo(q3);
//
//    q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
//    q2.copyTo(q1);
//    tmp.copyTo(q2);
//
//	cv::normalize(magI, magI, 0, 1, CV_MINMAX);//Transform the matrix with float values into a
//	imshow(window, magI);
}

void CVMat::noise(int scale)
{///gaussian noise 
	normal_distribution<float> nd;
	random_device rd;
	for(int i=0; i<rows*cols*channels(); i++) *(data+i) += scale * nd(rd);
}

void CVMat::diffx()
{
	assert(depth() == CV_32F);
	for(int y=0; y<rows; y++) for(int x=0; x<cols-1; x++)
		at<float>(y, x) = at<float>(y, x+1) - at<float>(y, x);
}
void CVMat::diffy()
{
	assert(depth() == CV_32F);
	for(int y=0; y<rows-1; y++) for(int x=0; x<cols; x++)
		at<float>(y, x) = at<float>(y+1, x) - at<float>(y, x);
}

void CVMat::normalize(float a, float b)
{
	cv::normalize(*this, *this, a, b, cv::NORM_MINMAX, CV_32FC1);
}

void CVMat::restore()
{
	save_.copyTo(*this);
}

void CVMat::save()
{
	copyTo(save_);
}

void CVMat::detect_line(int th, int c, int h) 
{
	HoughLinesP(*this, lines_, 1, M_PI/180, th, c, h);
	cout << lines_.size() << " lines detected\n";
}

static optional<complex<double>> intersect(complex<double> a, complex<double> b,
		complex<double> c, complex<double> d) {//cross point of vector a + kb and c + ld
	if(arg(b) == arg(d) || arg(b) + M_PI == arg(d)) return {};//-pie < arg() <pie
	if(b == 0i || d == 0i) return {};
	if(a == c) return a;
	double argA = abs(arg(b) - arg(c - a));
	double argC = abs(arg(d) - arg(a - c));
	if(argA > M_PI) argA = argA - M_PI;
	if(argC > M_PI) argC = argC - M_PI;//l canb minus
	double l = abs(a - c) * sin(argA) / sin(M_PI - argA - argC);//a/sinA=b/sinB=2R
	return c + l / abs(d) * d;
}

static optional<complex<double>> intersect(Vec4i a, Vec4i b)
{
	complex<double> p1{a[0], a[1]}, p2{a[2], a[3]}, p3{b[0], b[1]}, p4{b[2], b[3]};
	return intersect(p1, p2 - p1, p3, p4 - p3);
}

static double distance(Vec4i a, Vec4i b)
{//approx distance between two parallel line
	complex<double> p1{a[0], a[1]}, p2{a[2], a[3]}, p3{b[0], b[1]}, p4{b[2], b[3]};
	complex<double> p5 = p1 + 0.5 * (p2 - p1);
	double argp = arg(p2 - p1) + M_PI * 0.5;
	if(auto a = intersect(p5, polar(1., argp), p3, p4 - p3)) return abs(p5 - *a);
	else return 0;
}

bool is_similar_angle(double a, double b)
{
	return (abs(a - b) < M_PI/32 || abs(a - b - M_PI) < M_PI/32) ? true : false;
}

optional<vector<Point>> CVMat::get_rect()
{
	if(lines_.size() < 4) return {};
	vector<double> args;
	for(auto a : lines_) {
		complex<double> p1{a[0], a[1]}, p2{a[2], a[3]};
		args.push_back(arg(p2 - p1));
	}
	vector<tuple<int, int, double>> idxes;//line idx and distance between line
	nCr ncr{args.size(), 2};
	while(ncr.next()) {
		int i=ncr[0]-1, j=ncr[1]-1;
		if(is_similar_angle(args[i], args[j]))
			if(double dist = distance(lines_[i], lines_[j]); dist > rows / 3)
				idxes.push_back(make_tuple(i, j, dist));
	}
	if(idxes.size() < 2) return {};
	nCr ncr2{idxes.size(), 2};
	while(ncr2.next()) {
		int i=ncr2[0]-1, j=ncr2[1]-1;
		if(is_similar_angle(args[get<0>(idxes[i])] - args[get<0>(idxes[j])], M_PI/2)) {
			double l1 = get<2>(idxes[i]), l2 = get<2>(idxes[j]);
			double width = max(l1, l2), height = min(l1, l2);
			if(abs(height/width - 4./7) < 1./20) {
				int aa[] = {get<0>(idxes[i]), get<1>(idxes[i]), get<0>(idxes[j]), get<1>(idxes[j])};
				auto a = intersect(lines_[aa[0]], lines_[aa[2]]);
				auto b = intersect(lines_[aa[0]], lines_[aa[3]]);
				auto c = intersect(lines_[aa[1]], lines_[aa[2]]);
				auto d = intersect(lines_[aa[1]], lines_[aa[3]]);
//				sort(aa, aa+4);
//				for(int i=0; i<4; i++) lines_[i] = lines_[aa[i]];
//				lines_.erase(lines_.begin() + 4, lines_.end());
				return vector<Point>{{a->real(), a->imag()}, {b->real(), b->imag()},
					{c->real(), c->imag()}, {d->real(), d->imag()}};
			}
		}
	}
	return {};
}

void CVMat::detect_circle(int can, int ct, int min, int max)
{
	//HoughCircles(*this, circles_, CV_HOUGH_GRADIENT, 1, rows/8, can, ct, min, max);
	cout << circles_.size() << " circles detected\n";
}

void CVMat::draw_detected_circle(cv::Scalar color)
{
	for(auto& a : circles_) circle(*this, {a[0], a[1]}, a[2], color, 1, 8, 0);
}

void CVMat::edge(int th, int tXr)
{
	Canny(*this, *this, th, tXr);
}

void CVMat::draw_detected_line(cv::Scalar color)
{
//	for(auto& a : lines_) line(*this, {a[0], a[1]}, {a[2], a[3]}, color, 1, CV_AA);
}

void CVMat::filter(const Mat& ft)
{
	filter2D(*this, *this, depth(), ft);
}

void CVMat::show(std::string title)
{
	imshow(title, *this);
}

void CVMat::median(int k) 
{
	medianBlur(*this, *this, k);
}

void CVMat::gray()
{
//	cvtColor(*this, *this, CV_BGR2GRAY);
}

void CVMat::template_init()
{
	feature<ORB>();
	feature<BRISK>();
}

void CVMat::clear()
{
	release();
	save_.release();
	harris_.release();
	descriptor_.release();
	bgr_.clear();
	lines_.clear();
	hierachy_.clear();//hierachy for contour
	circles_.clear();
	faces_.clear();
	contours_.clear();
	keypoints_.clear();
}

