#include "ThreadPool.h"

namespace tactical
{
	namespace utils
	{
		ThreadPool::ThreadPool(int numThreads)
		{
			m_threads.reserve(numThreads);

			for (int i = 0; i < numThreads; ++i) {
				m_threads.emplace_back([this] {
					std::function<void(void)> task;
					while (1) {
						{
							// Lock this block scope
							std::unique_lock<std::mutex> lock(m_mutex);

							m_cv.wait(lock, [this] {
								return !m_tasks.empty() || m_terminate;
							});
							// Thread unblocked from here on, now to figure out why

							// If the pool received a shutdown order (and there are no tasks yet to be done)
							if (m_terminate && m_tasks.empty())
								return;

							// If there are tasks to be done
							task = std::move(m_tasks.front());
							m_tasks.pop();
						}
						// outside of block scope, mutex unlocked and now do the task
						task();
					}
				});
			}
		}

		ThreadPool::~ThreadPool()
		{
			m_terminate = true; // no lock necessary since it's atomic
			m_cv.notify_all(); // wake all threads to enter the "return;" statement and exit
			// join
			for (std::thread &t : m_threads) {
				if (t.joinable()) {
					t.join();
				}
				else {
					LOG << LOGTYPE::LOG_WARNING << "Detached thread: " + std::hash<std::thread::id>()(t.get_id());
				}
			}
		}
	}
}