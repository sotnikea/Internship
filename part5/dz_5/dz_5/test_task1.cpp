#include "pch.h"
#include "../dz5_1/GetProductsAmount.cpp"

//using patern AAA

TEST(GetProductsAmountTest, VectorOfPriceIsEmpty) {
	//Arrange
	std::vector<int> prices{};
	int amountOfMoney = 0;

	//Act
	size_t res = GetProductsAmount(prices, amountOfMoney);

	//Assert
	ASSERT_EQ(res, 0);
}

TEST(GetProductsAmountTest, WeHaveNoMoney) {
	//Arrange
	std::vector<int> prices{1,2,3};
	int amountOfMoney = 0;

	//Act
	size_t res = GetProductsAmount(prices, amountOfMoney);

	//Assert
	ASSERT_EQ(res, 0);
}

TEST(GetProductsAmountTest, MoneyEnoughForEverithing) {
	//Arrange
	std::vector<int> prices{ 10,1,20,4 };
	int amountOfMoney = 40;

	//Act
	size_t res = GetProductsAmount(prices, amountOfMoney);

	//Assert
	ASSERT_EQ(res, 4);
}

TEST(GetProductsAmountTest, MoneyNOTEnoughForEverithing) {
	//Arrange
	std::vector<int> prices{ 10,5,20,3 };
	int amountOfMoney = 1;

	//Act
	size_t res = GetProductsAmount(prices, amountOfMoney);

	//Assert
	ASSERT_EQ(res, 0);
}

TEST(GetProductsAmountTest, ProductWithSamePrice) {
	//Arrange
	std::vector<int> prices{ 1,1,1,1 };
	int amountOfMoney = 3;

	//Act
	size_t res = GetProductsAmount(prices, amountOfMoney);

	//Assert
	ASSERT_EQ(res, 3);
}
TEST(GetProductsAmountTest, MoneyEnoughForOneProduct) {
	//Arrange
	std::vector<int> prices{ 10,5,20,3 };
	int amountOfMoney = 4;

	//Act
	size_t res = GetProductsAmount(prices, amountOfMoney);

	//Assert
	ASSERT_EQ(res, 1);
}

TEST(GetProductsAmountTest, MoneyEnoughForFewProducts) {
	//Arrange
	std::vector<int> prices{ 6,2,7,3,4,5,1 };
	int amountOfMoney1 = 4;
	int amountOfMoney2 = 5;
	int amountOfMoney3 = 6;
	int amountOfMoney4 = 9;
	int amountOfMoney5 = 10;

	//Act
	size_t res1 = GetProductsAmount(prices, amountOfMoney1);
	size_t res2 = GetProductsAmount(prices, amountOfMoney2);
	size_t res3 = GetProductsAmount(prices, amountOfMoney3);
	size_t res4 = GetProductsAmount(prices, amountOfMoney4);
	size_t res5 = GetProductsAmount(prices, amountOfMoney5);

	//Assert
	ASSERT_EQ(res1, 2);
	ASSERT_EQ(res2, 2);
	ASSERT_EQ(res3, 3);
	ASSERT_EQ(res4, 3);
	ASSERT_EQ(res5, 4);

}

TEST(GetProductsAmountTest, TwoProductsWithLowerPriceOrOneMoreExpensive) {
	//Arrange
	std::vector<int> prices{ 2,1,1 };
	int amountOfMoney = 2;

	//Act
	size_t res = GetProductsAmount(prices, amountOfMoney);

	//Assert
	ASSERT_EQ(res, 2);
}