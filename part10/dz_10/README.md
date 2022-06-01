# Условие
Напишите простейший тред пул. Пример необходимого (но не достаточного) интерфейса класса:

~~~C++
class SimpleThreadPool {
  public:

    explicit SimpleThreadPool(std::size_t threadCount);

  ~SimpleThreadPool();

  SimpleThreadPool(const SimpleThreadPool & ) = delete;
  SimpleThreadPool & operator = (const SimpleThreadPool & ) = delete;

  template < typename Fnc_T >
    auto Post(Fnc_T task) -> std::future < decltype(task()) > ;
  void WorkOn();
  void Destroy();
  private:
    size_t m_threadCount;
  std::vector < std::thread > threads;
  std::queue < std:: function < void() >> tasks;
  std::condition_variable condition;
  std::mutex mut;
  bool stop;
};
~~~

# Формат сдачи
Приложить файлы готового проекта

# Обзор решения
Финальная версия класса содержит следующие приватные поля
~~~C++
std::atomic_bool stop;
	std::queue<function_wrapper> tasks;
	std::vector<std::thread> threads;

	std::size_t m_threadCount;
	std::mutex mut;

	void tasksThread(); //distribution of tasks by threads
	void WorkOn();		//associate threads with tasks
	void Destroy();		//destroy current pool

  //safe removal of a task from the queue
	template<typename T>
	bool  try_pop(T& value);
~~~
И следующие публичные поля:
~~~C++
explicit  SimpleThreadPool(std::size_t threadCount);				//constructor	
	SimpleThreadPool(const SimpleThreadPool&) = delete;					//disable copy constructor
	SimpleThreadPool& operator = (const SimpleThreadPool&) = delete;	//prohibit copying by assignment

	//return std::future<>, which will contain the return value of the task and allow the caller to wait for it to complete
	template <typename FunctionType>
	auto Post(FunctionType f)->std::future<decltype(f())>;

  //overload function that can take function object with arguments
	template<typename T, typename... Args>
	auto Post(T&& task, Args&& ... args);
	
	std::size_t GetSize();				//return amount of threads	
	void Resize(std::size_t newSize);	//change amount of threads in pool	
	~SimpleThreadPool();				//Destructor
~~~
Для возможности помещения в очередь задач(функциональных объектов) разных типов создан класс-обертка
~~~C++
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
~~~
Формирование потоков начинается на этапе вызова конструктора
~~~C++
SimpleThreadPool::SimpleThreadPool(std::size_t threadCount) :
	m_threadCount(threadCount) {
	WorkOn();
}
~~~
Метод WorkOn проверяет максимальное количество возможных потоков на текущем устройстве после чего для каждого устройства изымает из очереди задачу и привязывает к ней текущий открытый поток
~~~C++
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
~~~
Естественно на этапе вызова конструктора задач в очереди еще нет и непосредственной привязки какой-либо задачи к потоку не происходит. Данная блокировка реализована с помощью булевого флага stop. Однако проверка появления задач происходит постоянно. В случае их отсутствия текущий поток встает в конец очереди в ожидани ее появления
~~~C++
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
~~~
Добавление задач реализовано отдельным методом. Если задачи появляются, реализовано безопасное их извлечение из очереди с помощью метода try_pop. Это необходимо, чтобы несколько потоков не пытались получить на выполнение одну и ту же задачу
~~~C++
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
~~~
На этапе формирования потоков так же предусмотрено возможное появление исключения. В этом случае вызывается метод прекращающий доступ потоков к задачам и приостанавливающий дальнейшее выполнение программы до окончания работы потоков над их текущими задачами. Этот же метод вызывается деструктором при уничтожении пула
~~~C++
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
~~
Код деструктора
~~~C++
//Destructor
SimpleThreadPool::~SimpleThreadPool()
{
	Destroy();
	std::cout << "All thread free" << std::endl;
}

~~~
Добавление задач в очередь реализовано с помощью метода Post. Метод обрабатывает задачи возвращающие и не возвращающие значения
~~~C++
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
~~~
К сожалению реализация метода не способна обрабатывать задачи принимающие параметры. На просторах интернета найден и внедрен метод, исправляющий этот недочет. Метод внедрен в качестве перегрузки написанного ранее метода Post. Технически этот метод способен полностью выполнять так же и все функции предыдущего метода, при этом не используя класс-обертку. Однако в решении оставлены оба метода с целью в дальнейшем доработать собственный метод Post
~~~C++
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
~~~
Помимо основного функционала реализован ряд сопроводительных методов.
Метод возвращающий размер пула
~~~C++
//return amount of threads
std::size_t SimpleThreadPool::GetSize()
{
	return m_threadCount;
}

~~~
Метод изменяющий размер пула
~~~C++
//change amount of threads in pool
void SimpleThreadPool::Resize(std::size_t newSize)
{
	Destroy();	//Free all thread
	m_threadCount = newSize;
	threads.clear();
	WorkOn();
}
~~~

Пример тестового кейса для написанного класс
~~~C++
//some work function return int
int SomeWork() {
	int dur = rand() % 5 + 1;
	std::this_thread::sleep_for(std::chrono::seconds(dur));
	return dur;
}

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
~~~

# Ссылки
- Параллельное программирование на С++ в действии. Практика разработки многопоточных программ. Энтони Уильямс(глава 9) https://codernet.ru/books/c_plus/parallelnoe_programmirovanie_na_s_v_dejstvii/
- Статья про потоки на хабре http://scrutator.me/post/2012/04/04/parallel-world-p1.aspx
- Livelock, deadlock и starvation https://medium.com/german-gorelkin/deadlocks-livelocks-starvation-ccd22d06f3ae
- Semaphore https://radioprog.ru/post/1411#:~:text=%D0%A1%D0%B5%D0%BC%D0%B0%D1%84%D0%BE%D1%80%20(semaphore)%20%E2%80%93%20%D0%BF%D1%80%D0%B8%D0%BC%D0%B8%D1%82%D0%B8%D0%B2%20%D1%81%D0%B8%D0%BD%D1%85%D1%80%D0%BE%D0%BD%D0%B8%D0%B7%D0%B0%D1%86%D0%B8%D0%B8,%D0%BD%D1%83%D0%BB%D0%B5%D0%B2%D0%BE%D0%B3%D0%BE%20%D0%B7%D0%BD%D0%B0%D1%87%D0%B5%D0%BD%D0%B8%D1%8F%20%D1%81%D1%87%D1%91%D1%82%D1%87%D0%B8%D0%BA%D0%B0%20%D1%8F%D0%B2%D0%BB%D1%8F%D0%B5%D1%82%D1%81%D1%8F%20%D0%B1%D0%BB%D0%BE%D0%BA%D0%B8%D1%80%D1%83%D1%8E%D1%89%D0%B5%D0%B9.
- Thread example on habr https://habr.com/ru/post/656515/

