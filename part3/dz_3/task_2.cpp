#include <vector>
#include <iostream>


void foo(std::vector<int>& v) {
	std::vector<int> origin(v);
	try {
		//It's realy strange code, but it simulate error whith adding value into the vector	
		//That needed by the task
		int val, index;
		while (true) {				
			std::cout << "Enter index from 0 to "<<v.size()<<".Enter - 1 for exit: ";
			std::cin >> index;

			if (index < 0) return;			

			std::cout << "Enter new value: ";
			std::cin >> val;	

			v.at(index) = val;	//if index more then vector size
								//method at generate exception
								//invalid vector subscript
		}		
	}	
	catch (std::out_of_range& ex) {
		v.assign(origin.begin(), origin.end());
		throw;
	}
	//for other possible exeption
	catch (...) {		
		v.assign(origin.begin(), origin.end());
		throw;
	}
}

int main(void) {
	std::vector<int> v{ 10,20,30,40,50 };

	try {
		foo(v);
	}
	catch (std::exception& ex) {
		std::cout << "Error: " << ex.what() << std::endl;
	}

	for (const auto& i : v) {
		std::cout << i << " ";
	}
	std::cout << std::endl;

}

