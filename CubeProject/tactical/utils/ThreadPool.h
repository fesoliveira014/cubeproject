#pragma once
#include "../Common.h"
#include "Logger.h"
#include <mutex>
#include <condition_variable>
#include <atomic>


namespace tactical
{
	namespace utils
	{
		class ThreadPool
		{
		public:
			ThreadPool(int numThreads);			
			~ThreadPool();
			void ThreadLoop();			
			void AddTask(std::function<void(void)> func);
		private:
			std::mutex m_mutex;
			std::condition_variable m_cv;
			std::atomic<bool> m_terminate;
			std::vector<std::thread> m_threads;
			std::queue <std::function <void(void)> > m_tasks;
		};
	}
}