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
		using Chunk      = std::aligned_storage<sizeof(T), alignof(T)>;
		using value_type = T;

		explicit StackAllocator(size_t capacity)
		    : m_storage{std::make_unique<Chunk[]>(capacity)}
		    , m_current_index{-1}
		    , m_capacity{capacity}
		    , m_freelist{std::make_unique<size_t[]>(capacity)}
		    , m_freelist_end{0}
		{
		}

		T* allocate(size_t n)
		{
			if(n != 1) [[unlikely]]
				{
					return std::allocator<T>::allocatre(n);
				}

			if(m_freelist_end == 0) { ++m_current_index; }
			else
			{
				--m_freelist_end;
				m_current_index = m_freelist_end;
			}
			assert(m_current_index != m_capacity);
			return reinterpret_cast<T*>(m_storage.get()) + m_current_index;
		}

		void deallocate(T* ptr, size_t n)
		{
			if(n != 1) [[unlikely]]
				{
					return std::allocator<T>::deallocate(ptr, n);
				}

			m_freelist[m_freelist_end] = reinterpret_cast<Chunk*>(ptr) - m_storage.get();
			++m_freelist_end;
		}

		std::span<T const> localContent() const
		{
			return std::span{reinterpret_cast<T const>(m_storage.get()), m_current_index + 1};
		}

		std::span<size_t const> freelist() const
		{
			return std::span{m_freelist.get(), m_freelist_end};
		}

		size_t capacity() const { return m_capacity; }

	private:
		std::unique_ptr<Chunk> m_storage;
		size_t m_current_index;
		size_t m_capacity;

		std::unique_ptr<size_t[]> m_freelist;
		size_t m_freelist_end;
	};
}

#endif