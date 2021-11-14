//@	{
//@	 "targets":[{"name":"preallocated_set.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_UTILS_PREALLOCATEDSET_HPP
#define TEXPAINTER_UTILS_PREALLOCATEDSET_HPP

#include "./stack_allocator.hpp"

#include <set>

namespace Texpainter
{
	namespace prealloc_set_detail
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
	    : private std::multiset<T, Compare, prealloc_set_detail::AllocatorProxy<T>>
	{
		using Allocator = prealloc_set_detail::AllocatorProxy<T>;
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

		void erase_one(key_type const& key)
		{
			auto i = find(key);
			if(i != std::end(*this)) [[likely]]
				{
					erase(i);
				}
		}
	};


	template<class T, class Compare = std::less<>>
	class PreallocatedSet: private std::set<T, Compare, prealloc_set_detail::AllocatorProxy<T>>
	{
		using Allocator = prealloc_set_detail::AllocatorProxy<T>;
		using Base      = std::set<T, Compare, Allocator>;

	public:
		explicit PreallocatedSet(size_t capacity): Base{Allocator{capacity}} {}

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

		auto erase_one(key_type const& key) { return erase(key); }
	};
}

#endif