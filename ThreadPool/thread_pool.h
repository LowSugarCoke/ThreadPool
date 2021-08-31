//ThreadPool is a Data-Parallel Execution Model.
//ThreadPool thread_pool;
//thread_pool.Start();
//thread_pool.AddTask(your function());
//thread_pool.Stop();

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <iostream>

class ThreadPool {
public:

	ThreadPool();
	~ThreadPool();

	/// <summary> Start thread pool</summary>
	/// <returns> void type, start thread pool </returns>
	void Start();

	/// <summary> Stop thread pool</summary>
	/// <returns> void type, stop thread pool </returns>
	void Stop();

	/// <summary> Add task </summary>
	/// <param name="task">const std::function&lt; void()&lt; &amp; type, input:your function </param>
	/// <returns> void type </returns>
	void AddTask(const std::function<void()>& task);

private:
	void ThreadLoop();
	std::function<void()> Take();
	std::vector<std::thread*> thread_vector_;
	std::queue<std::function<void()>> task_queue_;
	std::mutex mutex_;
	std::condition_variable condition_variable_;
	bool flag_start_;
	int thread_number_;
};
#endif