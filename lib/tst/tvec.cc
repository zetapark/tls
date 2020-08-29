#include<optional>
#include<algorithm>
#include"tls/tvec.h"
#include"catch.hpp"
using namespace std;

TEST_CASE("thread safe map test") {
	ThreadSafeMap<int, int> m;
	m.push_back({2,3});
	REQUIRE(*m[2] == 3);
}

