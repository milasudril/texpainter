//@	{
//@	 "targets":[{"name":"thread_pool.o","type":"object"}]
//@	}

#include "./thread_pool.hpp"

#include "./utils/default_rng.hpp"

namespace
{
	struct Cookie
	{
		uint64_t rng_seed;
		std::reference_wrapper<Texpainter::Sched::ThreadPool> threads;
	};
}

Texpainter::Sched::ThreadPool::ThreadPool(ThreadCount n_threads)
    : m_terminate{false}
    , m_n_threads{n_threads.value()}
    , m_threads{std::make_unique<pthread_t[]>(m_n_threads)}
{
	std::for_each(m_threads.get(), m_threads.get() + m_n_threads, [this](auto& item) mutable {
		auto cookie = new Cookie{Texpainter::DefaultRng::engine()(), *this};
		pthread_create(
		    &item,
		    nullptr,
		    [](void* cookie) {
			    auto obj = reinterpret_cast<Cookie*>(cookie);
			    Texpainter::DefaultRng::seed(obj->rng_seed);
			    auto& self = obj->threads.get();
			    delete obj;
			    self.performTasks();
			    return static_cast<void*>(nullptr);
		    },
		    cookie);
	});
}

Texpainter::Sched::ThreadPool::~ThreadPool()
{
	{
		std::lock_guard lock{m_mtx};
		m_terminate = true;
		m_cv.notify_all();
	}

	std::for_each(m_threads.get(), m_threads.get() + m_n_threads, [](auto item) {
		void* ret = nullptr;
		pthread_join(item, &ret);
	});
}

void Texpainter::Sched::ThreadPool::performTasks()
{
	while(true)
	{
		std::unique_lock<std::mutex> lock{m_mtx};
		m_cv.wait(lock, [this]() { return !m_tasks.empty() || m_terminate; });
		if(!m_tasks.empty())
		{
			auto task = std::move(m_tasks.front());
			m_tasks.pop();
			lock.unlock();
			task();
		}
		else
		{
			return;
		}
	}
}