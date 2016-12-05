#include "ThreadPool.h"

namespace tactical
{
	namespace utils
	{
		ThreadPool::ThreadPool(int numThreads) : m_terminate(false), m_doingTask(0)
		{
			m_threads.reserve(numThreads);

			for (int i = 0; i < numThreads; ++i) {
				m_threads.emplace_back([this] {
					std::function<void(void)> task;
					while (1) {
						{
							// Lock this block scope
							std::unique_lock<std::mutex> lock(m_mutex);

							m_cvTask.wait(lock, [this] {
								return !m_tasks.empty() || m_terminate;
							});
							// Thread unblocked from here on, now to figure out why

							// If the pool received a shutdown order (and there are no tasks yet to be done)
							if (m_terminate && m_tasks.empty())
								return;

							// If there are tasks to be done
							++m_doingTask;
							task = std::move(m_tasks.front());
							m_tasks.pop();
						}
						// outside of block scope, mutex unlocked and now do the task
						auto id = std::this_thread::get_id();

						if (DEBUGPOOL) {
							std::stringstream ss;
							std::unique_lock<std::mutex> lock(m_mutex);
							ss << "Thread ID " << id << " Started Processing Task\n";
							LOG << LOGTYPE::LOG_INFO << ss.str();
							ss.clear();							
						}

						task();

						if (DEBUGPOOL) {
							std::stringstream ss;
							std::unique_lock<std::mutex> lock(m_mutex);
							ss << "Thread ID " << id << " Finished Processing Task\n";
							LOG << LOGTYPE::LOG_INFO << ss.str();
							ss.clear();
						}
						--m_doingTask;
						m_cvFinished.notify_one();
					}
				});
			}
		}

		ThreadPool::~ThreadPool()
		{
			m_terminate = true; // no lock necessary since it's atomic
			m_cvTask.notify_all(); // wake all threads to enter the "return;" statement and exit
			// join
			for (std::thread &t : m_threads) {
				t.join();
				/*
				else {
					LOG << LOGTYPE::LOG_WARNING << "Detached thread: " + std::hash<std::thread::id>()(t.get_id());
				}
				*/
			}
		}

		void ThreadPool::waitFinished()
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_cvFinished.wait(lock, [this]() { return m_tasks.empty() && (m_doingTask == 0); });
		}
	}
}