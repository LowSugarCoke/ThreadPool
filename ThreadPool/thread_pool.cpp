#include "thread_pool.h"

ThreadPool::ThreadPool()
	: start_flag_(false),
	task_end_flag_(false)
{
	if (std::thread::hardware_concurrency() > 3)
	{
		thread_number_ = std::thread::hardware_concurrency() - 2;
	}
	else
	{
		thread_number_ = 1;
	}
}

ThreadPool::~ThreadPool()
{
	if (start_flag_)
	{
		Stop();
	}
}

void ThreadPool::Start()
{
	start_flag_ = true;

	thread_vector_.reserve(thread_number_);
	for (int i = 0; i < thread_number_; ++i)
	{
		thread_vector_.push_back(new std::thread(std::bind(&ThreadPool::ThreadLoop, this)));
	}
}

void ThreadPool::Stop()
{
	start_flag_ = false;
	condition_variable_.notify_all();

	for (auto it = thread_vector_.begin(); it != thread_vector_.end(); ++it)
	{
		(*it)->join();
		delete *it;
	}
}

void ThreadPool::ThreadLoop()
{
	while (start_flag_)
	{
		std::function<void()> task = Take();
		if (task)
		{
			task_end_flag_ = false;
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

bool ThreadPool::IsTaskEnd()
{
	if (task_queue_.empty())
	{
		return true;
	}
	else
	{
		return false;
	}
}

std::function<void()> ThreadPool::Take()
{
	std::unique_lock<std::mutex> lock(mutex_);

	while (task_queue_.empty() && start_flag_)
	{
		condition_variable_.wait(lock);
	}

	std::function<void()> task;
	if (!task_queue_.empty() && start_flag_)
	{
		task = task_queue_.front();
		task_queue_.pop();
	}

	return task;
}