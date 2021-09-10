#include"thread_pool.h"
#include<iostream>
#include<time.h>
#include<windows.h>

static void SleepTask();
void MainThreadSleepTime(int n);
void ThreadPoolSleepTime(int n);

int main()
{
	int sleep_time = 1000;

	MainThreadSleepTime(sleep_time);
	ThreadPoolSleepTime(sleep_time);

	system("pause");
	return 0;
}

static void SleepTask()
{
	Sleep(1);
}

void MainThreadSleepTime(int n)
{
	clock_t start_main_thread, stop_main_thread;
	start_main_thread = clock();

	for (int i = 1; i < n; ++i)
	{
		SleepTask();
	}
	stop_main_thread = clock();

	std::cout << "Main thread time = " << double(stop_main_thread - start_main_thread) / CLOCKS_PER_SEC << std::endl;
}

void ThreadPoolSleepTime(int n)
{
	clock_t start_thread_pool, stop_thread_pool;
	start_thread_pool = clock();

	ThreadPool thread_pool;
	thread_pool.Start();
	for (int i = 1; i < n; ++i)
	{
		thread_pool.AddTask(SleepTask);
	}
	if (thread_pool.IsTaskEnd())
	{
		thread_pool.Stop();
	}

	stop_thread_pool = clock();
	std::cout << "Thread pool time = " << double(stop_thread_pool - start_thread_pool) / CLOCKS_PER_SEC << std::endl;
}