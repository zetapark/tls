#pragma once
#include<string>
#include<opencv.hpp>
#include<core.hpp>
#include<highgui.hpp>
#include<video/background_segm.hpp>
#include<features2d/features2d.hpp>

class CVMat : public cv::Mat
{
public:
	CVMat(const cv::Mat& r);
	CVMat() = default;
	operator cv::Mat();
	void filter(const Mat& m);//apply kernel
	void show(std::string title="OpenCV");
	void restore();
	void save();
	void read_xpm(const char **xpm);
	
	std::pair<int, int> text(std::string text_to_draw,//return width,height of text
			cv::Point lower_left_of_text = {0,20},
			double scale = 1,//multiply
			cv::Scalar bgra_color = {0,0,0,255},
			int thickness = 2, int fontFace = cv::FONT_HERSHEY_SIMPLEX, 
			int lineType = 8, bool bottomleftorigin = false);

	template<class T> int polyline(const T& x, const T& y, cv::Scalar color = {0,0,0,255},
			int thickness = 2, int shift = 0, bool is_closed = true) {
		const int sz = x.size();
		cv::Point pt[sz];
		const cv::Point* p = pt;
		for(int i=0; i<sz; i++) pt[i] = {x[i], y[i]};
		polylines(*this, &p, &sz, 1, is_closed, color, thickness, 8, shift);
		return sz;
	}

	void gray();
	void scale(float x, float y);
	void noise(int scale);//normal distrubution noise
	void median(int ksize);//median is good for salt&pepper noise
	void normalize(float a, float b);
	void diffx();//after normalize to float
	void diffy();
	void edge(int lowthreshold=30, int thresXratio=100);//Canny
	void corner(float k = 0.04, int block = 3, int aperture = 3);//harris gray->
	void draw_detected_corner(float thres = 0.01);
	void detect_line(int threshold=180, int continuous=50, int hop=10);//edge->Hough
	std::optional<std::vector<cv::Point>> get_rect();
	void detect_circle(int canny_threshold=200, int center_threshold=100,//gray->circ 
			int min_radius=0, int max_radius=0);//gradient를 보므로 edge로 하면 안됨.
	void detect_face();//gray->face
	void detect_contours(int mode = cv::RETR_TREE, int method= cv::CHAIN_APPROX_NONE);
	void draw_detected_contours(int thickness=1,int linetype=8, int maxlevel=INT_MAX);
	void draw_detected_line(cv::Scalar color = {0,0,255});
	void draw_detected_circle(cv::Scalar color = {0,0,255});
	void draw_detected_face();
	void fourier(std::string window = "Fourier");//after gray
	cv::MatND histo(std::string windwo = "Histogram");//after gray
	template<typename T> void feature();
	void draw_feature();
	std::vector<cv::DMatch> match(const CVMat& r, double thres = 0.5) const;
	CVMat background();
	void rotate(double angle, cv::Point center={-1,-1}, double scale=1);
	void transform3(cv::Point2f src[3], cv::Point2f dst[3], cv::Size sz = {0,0});
	void transform4(cv::Point2f src[4], cv::Point2f dst[4], cv::Size sz = {0,0});
	std::vector<cv::Point> get_points(int k);// ^ affine and perspective transform
	void get_businesscard(std::vector<cv::Point> v);
	void clear();
	
protected:
	cv::Mat save_, harris_, descriptor_;
	std::vector<Mat> bgr_;
	std::vector<cv::Vec4i> lines_, hierachy_;//hierachy for contour
	std::vector<cv::Vec3f> circles_;
	std::vector<cv::Rect> faces_;
	std::vector<std::vector<cv::Point>> contours_;

public:
	std::vector<cv::KeyPoint> keypoints_;

private:
	void template_init();
};


static cv::Mat BLUR = (cv::Mat_<float>(3,3) << 1,1,1,1,1,1,1,1,1) / 9;
static cv::Mat GAUSSIAN = (cv::Mat_<float>(3,3) << 1,2,1,2,4,2,1,2,1) / 16;;
static cv::Mat SHARPEN = (cv::Mat_<float>(3,3) << 0,-1,0,-1,5,-1,0,-1,0);
//static cv::Mat GAUS = cv::getGaussianKernel(9, 0.5, CV_32F);
static cv::Mat SOBELX = (cv::Mat_<float>(3,3) << -1,0,1,-2,0,2,-1,0,1);
static cv::Mat SOBELY = (cv::Mat_<float>(3,3) << -1,-2,-1,0,0,0,1,2,1);
static cv::Mat DIM1 = (cv::Mat_<float>(3,3) << -1,1,-1,1,-1,1,-1,1);
