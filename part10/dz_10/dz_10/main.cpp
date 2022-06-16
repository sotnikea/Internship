#include "SimpleThreadPool.h"
#include <iostream>

//some work function return int
int SomeWork() {
	int dur = rand() % 5 + 1;
	std::this_thread::sleep_for(std::chrono::seconds(dur));
	return dur;
}

//some void work function
void SomeWork2() {
	int dur = rand() % 5 + 1;
	std::this_thread::sleep_for(std::chrono::seconds(dur));
	std::cout << "Task complete in "<< dur << " sec " << std::endl;
}

//some work function that take argument
void SomeWork3(int num) {
	int dur = rand() % num + 1;
	std::this_thread::sleep_for(std::chrono::seconds(dur));
	std::cout << "Task complete in " << dur << " sec " << "with random limits "<< num << std::endl;
}

int main(void) {

	//Test with function return int
	{
		SimpleThreadPool t(5);	//create pool with 5 threads

		int elem = 10;								//amount of tasks
		std::vector<std::future<int>>  fu(elem);	//vector of future results
		{
			for (int i = 0; i < elem; i++) {
				fu[i] = t.Post(SomeWork);
			}

			for (int i = 0; i < elem; i++) {
				int dur = fu[i].get();
				std::cout << "Duration of task " << i << ": " << dur << " sec" << std::endl;
			}
		}

		std::cout << "Thread amount: " << t.GetSize() << std::endl;	//Get current size
		t.Resize(3);	//change amount of thread in pool

		elem = 5;	//change amount of task
		{
			for (int i = 0; i < elem; i++) {
				fu[i] = t.Post(SomeWork);
			}

			int dur = fu[2].get();
			std::cout << "Duration of task " << 2 << ": " << dur << " sec" << std::endl << std::endl;
		}
		std::cout << "Thread amount: " << t.GetSize() << std::endl;		//Get current size
	}

	//Test with void function
	{
		SimpleThreadPool t(5);	//create pool with 5 threads

		int elem = 10;								//amount of tasks
		std::vector<std::future<void>>  fu(elem);	//vector of future results
		{
			for (int i = 0; i < elem; i++) {
				fu[i] = t.Post(SomeWork2);
			}

			for (int i = 0; i < elem; i++) {
				fu[i].get();				
			}
		}

		std::cout << "Thread amount: " << t.GetSize() << std::endl;	//Get current size
		t.Resize(3);	//change amount of thread in pool

		elem = 5;	//change amount of task
		{
			for (int i = 0; i < elem; i++) {
				fu[i] = t.Post(SomeWork2);
			}

			fu[2].get();
		}
		std::cout << "Thread amount: " << t.GetSize() << std::endl;		//Get current size
	}


	//Test with function that take argument
	{
		SimpleThreadPool t(5);	//create pool with 5 threads

		int elem = 10;								//amount of tasks
		std::vector<std::future<void>>  fu(elem);	//vector of future results
		{
			for (int i = 0; i < elem; i++) {
				fu[i] = t.Post(SomeWork3, 5);
			}

			for (int i = 0; i < elem; i++) {
				fu[i].get();
			}
		}

		std::cout << "Thread amount: " << t.GetSize() << std::endl;	//Get current size
		t.Resize(3);	//change amount of thread in pool

		elem = 5;	//change amount of task
		{
			for (int i = 0; i < elem; i++) {
				fu[i] = t.Post(SomeWork3, 7);
			}

			fu[2].get();
		}
		std::cout << "Thread amount: " << t.GetSize() << std::endl;		//Get current size
	}

}