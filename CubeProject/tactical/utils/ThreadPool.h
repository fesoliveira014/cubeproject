#pragma once
#include "../Common.h"
#include "Logger.h"
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>

namespace tactical
{
	namespace utils
	{
		class ThreadPool
		{
		public:
			ThreadPool(int numThreads);
			~ThreadPool();
			template<class F, class... Args>
			auto AddTask(F&& f, Args&&... args)
				->std::future<typename std::result_of<F(Args...)>::type>;
		private:
			std::mutex m_mutex;
			std::condition_variable m_cv;
			std::atomic<bool> m_terminate;
			std::vector<std::thread> m_threads;
			std::queue <std::function <void(void)> > m_tasks;
		};

		template<class F, class... Args>
		auto ThreadPool::AddTask(F &&func, Args &&... args)
			-> std::future<typename std::result_of<F(Args...)>::type>
		{
			// Create packaged_task for std::future ret
			auto packagedTask = std::make_shared<std::packaged_task<decltype(func(args...))()> >(
				std::bind(std::forward<F>(func), std::forward<Args>(args)...)
				);

			// Emplace generic function call task in m_tasks queue
			m_tasks.emplace([packagedTask] {
				(*packagedTask)();
			});

			std::unique_lock<std::mutex> lock(m_mutex);
			m_cv.notify_one();
			return packagedTask->get_future();
		}
	}
}