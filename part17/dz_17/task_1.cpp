#include <iostream>
#include <utility>
#include <string>

std::pair<char, char> RandomPairDigit(size_t sum) {
	//find minimal value for the first digit
	//so that each digit individually is not greater than 126 (ASCII Table size).. 127 is DEL
	size_t min = sum - 126;

	//rand first digit in range from min to 127
	size_t digit_1 = rand() % (126 - min + 1) + min;

	//find second digit so both digit in sum would equal to sum
	size_t digit_2 = sum - digit_1;

	return std::pair<char, char>(static_cast<char>(digit_1), static_cast<char>(digit_2));
}


void PrintCode() {
	std::string code = "XXXXXX";

	//Sum of digit_0 and digit_5 is 0DE or 222 in Decimal
	auto pair_0_5 = RandomPairDigit(222);
	code[0] = pair_0_5.first;
	code[5] = pair_0_5.second;

	//Sum of digit_2 and digit_3 is 0D4 or 212 in Decimal
	auto pair_2_3 = RandomPairDigit(212);
	code[2] = pair_2_3.first;
	code[3] = pair_2_3.second;

	//digit_1 + digit_4 = 2F2 - 0DE - 0D4 - 6F = D1
	//digit_1 + digit_4 = 754 - 222 - 212 - 111 = 209
	auto pair_1_4 = RandomPairDigit(209);
	code[1] = pair_1_4.first;
	code[4] = pair_1_4.second;

	std::cout <<"Password: "<< code << std::endl;
}

int main()
{
	size_t password_amount = 10;

	for (size_t i = 0; i < password_amount; i++)
		PrintCode();
}

