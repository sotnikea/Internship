#include "codereviewtask_myvector.hpp"
#include<iostream>

//check for memory liks
#define __CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

int main(void) {

	try {
		//Working with pair by index
		{
			MyVector<int>my;	//One strong ref
			my.push_back(0, "Zero");
			my.push_back(10, "First");
			my.push_back(20, "Second");

			MyVector<int>my2(my);	//Two strong ref
			MyVector<int>my3(my2);	//Three strong ref

			std::cout << my.at(0).first << " " << my.at(0).second << std::endl; //refCount doesn't change becouse only reading used
			//my.at(0).first = 1000;	//such operation denied and data can't be modified

			std::pair<int, std::string> p1 = my[1];	//without proxy class this action copy data and change ref counter
			std::cout << p1.first << " " << p1.second << std::endl;			

			std::pair<int, std::string> p3(90, "xxx");
			my[0] = p3;	//refCount change becouse writing used		

			std::pair<const int, const std::string> p4 = my2[0];
		}
		
		
		//Working with value by name
		{
			MyVector<int>my;
			my.push_back(0, "Zero");
			my.push_back(10, "First");
			my.push_back(20, "Second");

			MyVector<int>my2(my);
			MyVector<int>my3(my2);

			int num1 = my["First"]; //without proxy class this action copy data and change ref counter
			std::cout << num1 << std::endl;

			my2["First"] = 100; //refCount change becouse data changed
			my2[1].second = "Hundred";
			std::cout << my2[1].first<<" "<< my2[1].second<<std::endl;
		}
		
		
		//begin()
		{
			MyVector<int>my;
			my.push_back(0, "Zero");
			my.push_back(10, "First");
			my.push_back(20, "Second");

			MyVector<int>my2(my);

			auto it = my.begin();	//value can be modified using iterator, so refCount change
			std::cout << it->first << " "<<it->second << std::endl;
			it++;	//move iterator
			std::cout << it->first << " " << it->second << std::endl;
			it->first = 100;	//change value by iterator
			it->second = "Handred";
			std::cout << it->first << " " << it->second << std::endl;

			std::cout << (my2.begin() + 2)->first << " "<< (my2.begin() + 2)->second<< std::endl;
			(my2.begin() + 2)->first = 100;	//set new value by iterator
			std::cout << (my2.begin() + 2)->first << " " << (my2.begin() + 2)->second << std::endl;
		}
		
		
		//cbegin()
		{
			MyVector<int>my;
			my.push_back(0, "Zero");
			my.push_back(10, "First");
			my.push_back(20, "Second");

			MyVector<int>my2(my);

			auto it = my.cbegin(); //value can't be modified using const iterator, so refCount doesn't change
			std::cout << it->first << " " << it->second << std::endl;
			it++;
			std::cout << it->first << " " << it->second << std::endl;

			//it->first = 100;	//change value by iterator doesn't work becouse of const iterator
									
			std::cout << (my2.cbegin() + 2)->first << " " << (my2.cbegin() + 2)->second << std::endl;
		}
		
		//end()
		{
			MyVector<int>my;
			my.push_back(0, "Zero");
			my.push_back(10, "First");
			my.push_back(20, "Second");

			MyVector<int>my2(my);

			auto it = my.end() - 1;	//value can be modified using iterator, so refCount change
			std::cout << it->first << " " << it->second << std::endl;
			it--;
			std::cout << it->first << " " << it->second << std::endl;

			it->first = 100;	//change value by iterator
			it->second = "Handred";

			std::cout << it->first << " " << it->second << std::endl;

			std::cout << (my2.end() - 3)->first << " " << (my2.end() - 3)->second << std::endl;
		}
		
		//cend()
		{
			MyVector<int>my;
			my.push_back(0, "Zero");
			my.push_back(10, "First");
			my.push_back(20, "Second");

			MyVector<int>my2(my);

			auto it = my.cend() - 1;	//value can't be modified using const iterator, so refCount doesn't change
			std::cout << it->first << " " << it->second << std::endl;
			it--;
			std::cout << it->first << " " << it->second << std::endl;

			//it->first = 100; //operation denied for const iterator	
			std::cout << (my2.cend() - 3)->first << " " << (my2.cend() - 3)->second << std::endl;
		}
		
		//empty()
		{
			MyVector<int>my;
			my.push_back(0, "Zero");
			my.push_back(10, "First");
			my.push_back(20, "Second");

			MyVector<int>my2(my);

			if (my.empty()) {	//value can't be modified, so refCount doesn't change
				std::cout << "Vector MY is empty" << std::endl;
			}

			if (!my.empty()) {
				std::cout << "Vector MY is not empty" << std::endl;
			}
		}
		
		//size()
		{
			MyVector<int>my;
			my.push_back(0, "Zero");
			my.push_back(10, "First");
			my.push_back(20, "Second");

			MyVector<int>my2(my);

			for (int i = 0; i < my.size(); i++) {	///value can't be modified, so refCount doesn't change
				std::cout << my.at(i).first << " " << my.at(i).second << std::endl;
			}
		}

		
		//reserve and capacity()
		{
			MyVector<int>my;
			my.push_back(0, "Zero");
			my.push_back(10, "First");
			my.push_back(20, "Second");

			MyVector<int>my2(my);

			std::cout << "Current capacity: " << my.capacity() << std::endl;
			std::cout << "Current size: " << my.size() << std::endl;

			my.reserve(10);	//reserve operation allocate extra space in data vector
							//and in fact change it structure
							//so refCount change
			std::cout << "Current capacity: " << my.capacity() << std::endl;
			std::cout << "Current size: " << my.size() << std::endl;

			std::cout << "Current capacity: " << my2.capacity() << std::endl;
			std::cout << "Current size: " << my2.size() << std::endl;
		}

		
		//clear()
		{
			MyVector<int>my;
			my.push_back(0, "Zero");
			my.push_back(10, "First");
			my.push_back(20, "Second");

			MyVector<int>my2(my);

			std::cout << "Current capacity: " << my.capacity() << std::endl;
			std::cout << "Current size: " << my.size() << std::endl;

			my.clear();	//refCount change

			std::cout << "Current capacity: " << my.capacity() << std::endl;
			std::cout << "Current size: " << my.size() << std::endl;

			std::cout << "Current capacity: " << my2.capacity() << std::endl;
			std::cout << "Current size: " << my2.size() << std::endl;
		}
		
	}
	catch (const std::exception& ex) {
		std::cout << "Error: " << ex.what() << std::endl;
	}

	_CrtDumpMemoryLeaks(); //check for memory liks
	return 0;
}