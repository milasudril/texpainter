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
	class PreallocStackAllocator
	{
	public:
		using Chunk      = std::aligned_storage_t<sizeof(T), alignof(T)>;
		using ChunkPointer = Chunk*;
		static_assert(sizeof(Chunk) >= sizeof(T));

		using value_type = T;

		explicit PreallocStackAllocator(size_t capacity):
		      m_freelist{std::make_unique<ChunkPointer[]>(capacity)}
		    , m_freelist_end{capacity}
		    , m_storage{std::make_unique<Chunk[]>(capacity)}
		    , m_capacity{capacity}
		{
			std::generate_n(m_freelist.get(), capacity, [
			base_address = m_storage.get(),
			k = static_cast<size_t>(0)]() mutable {
				auto ret = base_address + k;
				++k;
				return ret;
			});
		}

		[[nodiscard]] T* allocate(size_t n)
		{
			if(n != 1) [[unlikely]]
				{
					return m_default_allocator.allocate(n);
				}

			assert(m_freelist_end != 0);
			--m_freelist_end;
			return reinterpret_cast<T*>(m_freelist[m_freelist_end]);
		}

		void deallocate(T* ptr, size_t n)
		{
			if(n != 1) [[unlikely]]
				{
					return m_default_allocator.deallocate(ptr, n);
				}

			if(ptr == nullptr)
			{ return; }

			m_freelist[m_freelist_end] = reinterpret_cast<Chunk*>(ptr);
			++m_freelist_end;
		}

		std::span<size_t const> freelist() const
		{
			return std::span{m_freelist.get(), m_freelist_end};
		}

		size_t capacity() const { return m_capacity; }

	private:
		std::unique_ptr<ChunkPointer[]> m_freelist;
		size_t m_freelist_end;
		std::unique_ptr<Chunk[]> m_storage;
		size_t m_capacity;
		[[no_unique_address]] std::allocator<T> m_default_allocator;
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