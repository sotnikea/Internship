#include "SimpleThreadPool.h"

//constructor
SimpleThreadPool::SimpleThreadPool(std::size_t threadCount) :
	m_threadCount(threadCount) {
	WorkOn();
}

//return amount of threads
std::size_t SimpleThreadPool::GetSize()
{
	return m_threadCount;
}

//change amount of threads in pool
void SimpleThreadPool::Resize(std::size_t newSize)
{
	Destroy();	//Free all thread
	m_threadCount = newSize;
	threads.clear();
	WorkOn();
}

//Destructor
SimpleThreadPool::~SimpleThreadPool()
{
	Destroy();
	std::cout << "All thread free" << std::endl;
}

//distribution of tasks by threads
void SimpleThreadPool::tasksThread()
{	
	while (!stop) {						//move through the cycle until the stop flag is raised
		function_wrapper task;
		if (try_pop(task)) {			//retrieving tasks from the queue
			task();						//starting the task
		}
		else {
			std::this_thread::yield();	//if there are no tasks in the queue, stop the execution, allowing other threads to work
		}
	}	
}

//associate threads with tasks
void SimpleThreadPool::WorkOn()
{
	stop = false;

	//make sure it has enough threads
	if (m_threadCount > std::thread::hardware_concurrency()) {
		m_threadCount = std::thread::hardware_concurrency();
	}

	try {
		for (unsigned i = 0; i < m_threadCount; ++i) {
			threads.push_back(
				std::thread(&SimpleThreadPool::tasksThread, this));		//add a thread to the pool and bind an available task to it
																		//using the tasksThread function
																		//tasksThread works until the stop flag is raised
																		//thus the search for tasks continues until the pool is destroyed
		}
	}
	//Starting a thread may result in an exception,
	//therefore, must be ensure that the threads already running at this point are gracefully terminated.
	catch (...) {
		Destroy();
		throw;
	}
}

//destroy current pool
void SimpleThreadPool::Destroy()
{
	stop = true;	//terminates the task fetch cycle
	for (unsigned long i = 0; i < threads.size(); ++i) {	
		if (threads[i].joinable()) {						//if the thread is not associated with any process, applying join or detach to it will result in an exception
															//jonable checks that the process has a binding
			threads[i].join();								
		}
	}
}
