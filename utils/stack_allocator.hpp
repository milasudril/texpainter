//@	{
//@	 "targets":[{"name":"stack_allocator.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_UTILS_STACKALLOCATOR_HPP
#define TEXPAINTER_UTILS_STACKALLOCATOR_HPP

#include <memory>
#include <type_traits>
#include <cstddef>
#include <span>
#include <cassert>

namespace Texpainter
{
	template<class T>
	class StackAllocator
	{
	public:
		using Chunk      = std::aligned_storage_t<sizeof(T), alignof(T)>;
		static_assert(sizeof(Chunk) >= sizeof(T));

		using value_type = T;

		explicit StackAllocator(size_t capacity)
		    : m_storage{std::make_unique<Chunk[]>(capacity)}
		    , m_current_index{static_cast<size_t>(0)}
		    , m_capacity{capacity}
		    , m_freelist{std::make_unique<size_t[]>(capacity)}
		    , m_freelist_end{0}
		{
		}

		[[nodiscard]] T* allocate(size_t n)
		{
			if(n != 1) [[unlikely]]
				{
					return m_default_allocator.allocate(n);
				}

			if(m_freelist_end == 0)
			{
				auto ret = m_current_index;
				assert(ret != m_capacity);
				++m_current_index;
				return reinterpret_cast<T*>(m_storage.get()) + ret;
			}
			else
			{
				--m_freelist_end;
				return reinterpret_cast<T*>(m_storage.get()) + m_freelist[m_freelist_end];
			}
		}

		void deallocate(T* ptr, size_t n)
		{
			if(n != 1) [[unlikely]]
				{
					return m_default_allocator.deallocate(ptr, n);
				}

			m_freelist[m_freelist_end] = reinterpret_cast<Chunk*>(ptr) - m_storage.get();
			++m_freelist_end;
		}

		std::span<T const> localContent() const
		{
			return std::span{reinterpret_cast<T const*>(m_storage.get()), m_current_index};
		}

		std::span<size_t const> freelist() const
		{
			return std::span{m_freelist.get(), m_freelist_end};
		}

		size_t capacity() const { return m_capacity; }

	private:
		std::unique_ptr<Chunk[]> m_storage;
		size_t m_current_index;
		size_t m_capacity;

		std::unique_ptr<size_t[]> m_freelist;
		size_t m_freelist_end;

		std::allocator<T> m_default_allocator;
	};

	template<class Allocator, class ... Args>
	auto create(Allocator& alloc, Args ... args)
	{
		using T = typename Allocator::value_type;
		return new(alloc.allocate(1))T{std::forward<Args>(args)...};
	}

	template<class Allocator, class Pointer>
	void destroy(Allocator& alloc, Pointer ptr)
	{
		using T = typename Allocator::value_type;
		ptr->~T();
		alloc.deallocate(ptr, 1);
	}
}

#endif