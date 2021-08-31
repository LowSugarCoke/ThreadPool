#include "thread_pool.h"

ThreadPool::ThreadPool()
	: flag_start_(false),
	thread_number_(std::thread::hardware_concurrency() - 2)
{
}

ThreadPool::~ThreadPool()
{
	if (flag_start_)
	{
		Stop();
	}
}

void ThreadPool::Start()
{
	flag_start_ = true;

	thread_vector_.reserve(thread_number_);
	for (int i = 0; i < thread_number_; ++i)
	{
		thread_vector_.push_back(new std::thread(std::bind(&ThreadPool::ThreadLoop, this)));
	}
}

void ThreadPool::Stop()
{
	flag_start_ = false;
	condition_variable_.notify_all();

	for (auto it = thread_vector_.begin(); it != thread_vector_.end(); ++it)
	{
		(*it)->join();
		delete *it;
	}
}

void ThreadPool::ThreadLoop()
{
	while (flag_start_)
	{
		std::function<void()> task = Take();
		if (task)
		{
			task();
		}
	}
}

void ThreadPool::AddTask(const std::function<void()>& task)
{
	std::unique_lock<std::mutex> lock(mutex_);

	task_queue_.push(task);
	condition_variable_.notify_one();
}

std::function<void()> ThreadPool::Take()
{
	std::unique_lock<std::mutex> lock(mutex_);

	while (task_queue_.empty() && flag_start_)
	{
		condition_variable_.wait(lock);
	}

	std::function<void()> task;
	auto size = task_queue_.size();
	if (!task_queue_.empty() && flag_start_)
	{
		task = task_queue_.front();
		task_queue_.pop();
	}

	return task;
}