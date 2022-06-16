#include "ForwardList.h"

#include <iostream>
#include <vector>

//check memory liks
#define __CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW


int main(void) {
	try {
		//string test		
		{
			ForwardList<std::string> A;

			std::string textToAdd("Line 1");
			A.push_front(textToAdd);				//add lvalue

			A.push_front(std::string("Line 2"));	//add rvalue

			std::string textToAdd2("Line 3");
			A.push_front(std::move(textToAdd2));	//add lvalue like rvalue
			std::cout << textToAdd2 << std::endl;	//old string deleted

			ForwardList<std::string> B;
			B = std::move(A);						//move operator
			std::cout << A.size() << std::endl;		//size of old list is zero
			std::cout << B.size() << std::endl;		//size of new list is not zero

			ForwardList<std::string> C{ "New 1","New 2", "New 3", "New 4", "New 5" }; //Use initializer_list
			std::cout << C.size() << std::endl;

			auto val = C.front(); //get first element in list
			std::cout << val << std::endl;

			auto it = C.begin();	//create iterator to the first element
			std::cout << *it << std::endl;	//print it
			it++;	//next element postfix form
			std::cout << *it << std::endl;	//print it
			++it;  //next element prefix form
			std::cout << *it << std::endl;	//print it
			*it = "Some new text";	//change text
			std::cout << *it << std::endl;	//print it

			it = C.end();	//relocate iterator to the element after end of list
			//std::cout << *it << std::endl;	//this action throw exception


			//print all list
			for (auto elem : C) {
				std::cout << elem << " ";
			}
			std::cout<<std::endl;

			auto it2 = C.cbegin();	//create constant iterator to the first element
			std::cout << *it2 << std::endl;	//print it
			it2++;	//next element postfix form
			std::cout << *it2 << std::endl;	//print it
			//*it2 = "One more new text";	//It's doesn't work becouse of constant iterator		

			if (C.empty()) {	//not empty
				std::cout << "List C is empty" << std::endl;
			}
			if (A.empty()) {	//empty
				std::cout << "List A is empty" << std::endl;
			}

			C.clear();	//clear list
			if (C.empty()) {	//now it's empty
				std::cout << "List C is empty" << std::endl;
			}

			ForwardList<std::string> D{ "One","Two", "Three", "Four", "Five" };
		}
		

		//string insert		
		{
			ForwardList<std::string> A{ "One","Two", "Three", "Four", "Five" };
			auto it = A.begin();	//get iterator to girst element

			int index = 2;	
			for (int i = 0; i < index; i++, it++);	//move iterator to position [index]

			A.insert_after(it, "New text");	//Insert new data to this position
			
			for (auto elem : A) {	//check result
				std::cout << elem << " ";
			}
			std::cout << std::endl;

			auto it2 = A.begin();
			it2++;	//erase element added on previous step
			A.erase_after(it2);
			for (auto elem : A) {	//check result
				std::cout << elem << " ";
			}
			std::cout << std::endl;
		}
		

		//int test
		{
			ForwardList<int> A{ 10,20,30 };	//use initializer list
			A.push_front(0);	//add new element in front

			for (auto num : A) {	//print it
				std::cout << num << " ";
			}
			std::cout << std::endl;

			ForwardList<int> B;	
			B = std::move(A);	//move list to another list use rvalue

			if (A.empty()) {	//A now is empty
				std::cout << "A become empty" << std::endl;
			}

			auto it = B.begin();	//create iterator
			it++;	//move iterator
			*it = 1000;	//change value by iterator

			for (auto num : B) {	//print list
				std::cout << num << " ";
			}
			std::cout << std::endl;
		}
	}
	catch (std::exception& ex) {
		std::cout << "Error: " << ex.what() << std::endl;
	}
	

	//check memory liks
	_CrtDumpMemoryLeaks();

	return 0;
}