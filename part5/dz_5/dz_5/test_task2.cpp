#include "pch.h"
#include "../dz5_2/IsLeapYear.cpp"

TEST(IsLeapYearTest, YearThatIsEvenlyDivisibleBy4) {
	//Correct
	ASSERT_TRUE(IsLeapYear(4));
	ASSERT_TRUE(IsLeapYear(1008));
	ASSERT_TRUE(IsLeapYear(2004));

	//Wrong
	ASSERT_FALSE(IsLeapYear(1));
	ASSERT_FALSE(IsLeapYear(1002));
	ASSERT_FALSE(IsLeapYear(2003));
}

TEST(IsLeapYearTest, ExceptEveryYearThatIsEvenlyDivisibleBy100) {
	//Correct
	ASSERT_TRUE(IsLeapYear(104));
	ASSERT_TRUE(IsLeapYear(1108));
	ASSERT_TRUE(IsLeapYear(2012));

	//Wrong
	ASSERT_FALSE(IsLeapYear(100));
	ASSERT_FALSE(IsLeapYear(1000));
	ASSERT_FALSE(IsLeapYear(2200));
}

TEST(IsLeapYearTest, UnlessTheYearIsAlsoEvenlyDivisibleBy400) {
	//Correct
	ASSERT_TRUE(IsLeapYear(400));
	ASSERT_TRUE(IsLeapYear(1600));
	ASSERT_TRUE(IsLeapYear(2000));

	//Wrong
	ASSERT_FALSE(IsLeapYear(100));
	ASSERT_FALSE(IsLeapYear(1100));
	ASSERT_FALSE(IsLeapYear(2300));	
}


