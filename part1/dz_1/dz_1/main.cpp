#include <iostream>
#include <exception>

#include "FileWriter.h"
#include "Encryption.h"

//Test if there some memory liks
#define __CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW


int main(void) {
	try
	{
		/*null pointer for test
		{
			auto t = std::make_unique<XorEncriptionAdapter>(nullptr);
			t->Encrypt(199);
		}
		*/

		//Test read/write
		FileWriter test("text1.txt", "text2.txt");
		test.CopyTextFromFile();
		std::cout << "Copy from file finished" << std::endl;

		//Test encryption
		FileWriter test2("text1.txt", "text3.txt");
		auto xorEncrypt = std::make_unique<XorEncriptionAdapter>(&test2);
		xorEncrypt->Encrypt(199);
		std::cout << "Encrypt finished" << std::endl;


		//Test decipher
		FileWriter test3("text3.txt", "text4.txt");
		auto xorDecipher = std::make_unique<XorEncriptionAdapter>(&test3);
		xorDecipher->Encrypt(199);
		std::cout << "Decipher finished" << std::endl;

	}
	catch (std::bad_alloc& exception)
	{
		std::cerr << "You ran out of memory!" << std::endl;
	}
	catch (const std::runtime_error& ex) {
		std::cerr << "Runtime error: " << ex.what() << std::endl;
	}
	catch (const std::logic_error& ex) {
		std::cerr << "Logic error: " << ex.what() << std::endl;
	}
	catch (const std::exception& exception) {
		std::cerr << "Standard exception: " << exception.what() << std::endl;
	}	
	catch (...) {
		std::cerr << "Unknown error" << std::endl;
	}


	//Show if there memory liks
	_CrtDumpMemoryLeaks();
	return 0;
}

