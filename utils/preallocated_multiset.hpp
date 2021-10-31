//@	{
//@	 "targets":[{"name":"preallocated_multiset.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_UTILS_PREALLOCATEDMULTISET_HPP
#define TEXPAINTER_UTILS_PREALLOCATEDMULTISET_HPP

#include "./stack_allocator.hpp"

#include <set>

namespace Texpainter
{
	namespace prealloc_multiset_detail
	{
		template<class T>
		class Allocator: public PreallocStackAllocator<T>
		{
		public:
			template<class CapacityHolder>
			requires requires
			{
				typename CapacityHolder::IsCapacityHolder;
			}
			explicit Allocator(CapacityHolder capacity)
			    : PreallocStackAllocator<T>{capacity.capacity()}
			{
			}
		};

		template<class Type>
		class AllocatorProxy
		{
		public:
			using value_type = Type;

			template<class U>
			requires(!std::same_as<U, Type>) struct rebind
			{
				using other = Allocator<U>;
			};

			explicit AllocatorProxy(size_t capacity): m_capacity{capacity} {}

			size_t capacity() const { return m_capacity; }

			struct IsCapacityHolder
			{
			};

		private:
			size_t m_capacity;
		};
	}

	template<class T, class Compare = std::less<>>
	class PreallocatedMultiset
	    : private std::multiset<T, Compare, prealloc_multiset_detail::AllocatorProxy<T>>
	{
		using Allocator = prealloc_multiset_detail::AllocatorProxy<T>;
		using Base      = std::multiset<T, Compare, Allocator>;

	public:
		explicit PreallocatedMultiset(size_t capacity): Base{Allocator{capacity}} {}

		// Import stuff from base class

		using value_type = typename Base::value_type;
		using key_type   = typename Base::key_type;
		using Base::begin;
		using Base::clear;
		using Base::contains;
		using Base::count;
		using Base::emplace;
		using Base::emplace_hint;
		using Base::end;
		using Base::erase;
		using Base::extract;
		using Base::find;
		using Base::insert;
		using Base::merge;
		using Base::size;
		using Base::swap;

		// Additional functions

		auto capacity() const { return Base::get_allocator().capacity(); }

		auto max_size() const { return capacity(); }

		auto erase_one(key_type const& key)
		{
			auto i = find(key);
			if(i != std::end(key)) [[likely]]
				{
					return erase(i);
				}
			return i;
		}
	};
}

#endif