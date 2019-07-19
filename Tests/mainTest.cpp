#include "gtest/gtest.h"

int main(int argc, const char** argv)
{
	testing::InitGoogleTest(&argc, const_cast<char**>(argv));
	int testReturn  = RUN_ALL_TESTS();
	system("pause");
	return testReturn;
}