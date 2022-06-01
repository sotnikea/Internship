#ifndef SIMPLE_THREAD_POOL_H
#define SIMPLE_THREAD_POOL_H

#include <atomic>
#include <mutex>
#include <thread>
#include <functional>
#include <condition_variable>
#include <queue>
#include <iostream>
#include <future>

//For packaged_task implementation
class function_wrapper {
	struct  impl_base {
		virtual  void  call() = 0;
		virtual  ~impl_base() {}
	};

	std::unique_ptr<impl_base>  impl;

	template<typename  F>
	struct  impl_type : impl_base {
		F  f;
		impl_type(F&& f_) : f(std::move(f_)) {}
		void call() { f(); }
	};

public:
	template<typename  F>
	function_wrapper(F&& f) : impl(new  impl_type<F>(std::move(f))) {}

	void operator()() { impl->call(); }
	function_wrapper() = default;
	function_wrapper(function_wrapper&& other) noexcept : impl(std::move(other.impl)) {}
	function_wrapper& operator=(function_wrapper&& other) noexcept {
		impl = std::move(other.impl);
		return  *this;
	}

	function_wrapper(const function_wrapper&) = delete;
	function_wrapper(function_wrapper&) = delete;
	function_wrapper& operator=(const  function_wrapper&) = delete;
};

class SimpleThreadPool {
public:	
	explicit  SimpleThreadPool(std::size_t threadCount);				//constructor	
	SimpleThreadPool(const SimpleThreadPool&) = delete;					//disable copy constructor
	SimpleThreadPool& operator = (const SimpleThreadPool&) = delete;	//prohibit copying by assignment

	//return std::future<>, which will contain the return value of the task and allow the caller to wait for it to complete
	template <typename FunctionType>
	auto Post(FunctionType f)->std::future<decltype(f())>;

	// !!!
	// overload function that can take function object with arguments
	// !!! I have a problems with understandig some part of this methods
	// but I managed to implement it in my solution and it' work
	// even without function_wrapper class.
	// Also it can completely replace the previous method Post
	// but I left both, since the previous one was written entirely by myself
	template<typename T, typename... Args>
	auto Post(T&& task, Args&& ... args);
	
	std::size_t GetSize();				//return amount of threads	
	void Resize(std::size_t newSize);	//change amount of threads in pool	
	~SimpleThreadPool();				//Destructor

private:
	//safe removal of a task from the queue
	template<typename T>
	bool  try_pop(T& value);

private:
	std::atomic_bool stop;
	std::queue<function_wrapper> tasks;
	std::vector<std::thread> threads;

	std::size_t m_threadCount;
	std::mutex mut;

	void tasksThread(); //distribution of tasks by threads
	void WorkOn();		//associate threads with tasks
	void Destroy();		//destroy current pool
};

//return std::future<>, which will contain the return value of the task and allow the caller to wait for it to complete
template<typename FunctionType>
inline auto SimpleThreadPool::Post(FunctionType f) -> std::future<decltype(f())>
{
	typedef decltype(f()) result_type;						//define a type alias
	std::packaged_task<result_type()>  task(std::move(f));	//wrapping the function with a packaged_task object
															//previously defined alias
	std::future<result_type> res(task.get_future());		//now res is the future result of executing the task function
	tasks.push(std::move(task));							//move the task to the queue
															//copying packaged_task does not support
	return res;
}

// overload function that can take function object with arguments
template<typename T, typename ...Args>
inline auto SimpleThreadPool::Post(T&& task, Args && ...args)
{
	using result_type = std::invoke_result_t<T&&, Args&& ...>;

	auto wrapper = std::make_shared<std::packaged_task<result_type()>>(
		std::bind(std::forward<T>(task), std::forward<Args>(args)...)
		);

	{
		std::unique_lock<std::mutex> lock{ this->mut };


		this->tasks.emplace([=]
			{
				(*wrapper)();
			});

	}

	return wrapper->get_future();
}

//safe removal of a task from the queue
template<typename T>
inline bool SimpleThreadPool::try_pop(T& value)
{
	std::lock_guard<std::mutex> lk(mut);	//occupied mutex
	if (tasks.empty())						//if the queue is empty, the extraction failed.
		return  false;
	value = std::move(tasks.front());		//move the first task from the queue to the functional object received by reference
	tasks.pop();							//removing the first element of the queue
	return  true;							//return bool that the task was successfully received
}


#endif //SIMPLE_THREAD_POOL_H

