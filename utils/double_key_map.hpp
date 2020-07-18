//@	{"targets":[{"name":"double_key_map.hpp", "type":"include"}]}

#ifndef TEXPAINTER_DOUBLEKEYMAP_HPP
#define TEXPAINTER_DOUBLEKEYMAP_HPP

#include "./iter_pair.hpp"
#include "./pair_iterator.hpp"

#include <map>
#include <memory>
#include <type_traits>

namespace Texpainter
{
	namespace detail
	{
		template<class T>
		struct UniquePtrDeepConst
		{
			T const& operator*() const
			{
				return *m_handle;
			}

			T& operator*()
			{
				return *m_handle;
			}

			T* operator->()
			{
				return m_handle.get();
			}

			T const* operator->() const
			{
				return m_handle.get();
			}

			std::unique_ptr<T> m_handle;
		};

		template<class Iter>
		class DerefSecond
		{
		public:
			using difference_type = intptr_t;
			using value_type =
			   std::remove_reference_t<decltype(*(std::declval<typename Iter::value_type>().second))>;
			using iterator_category = std::bidirectional_iterator_tag;

			DerefSecond() = default;

			DerefSecond(DerefSecond const&) = default;

			explicit DerefSecond(Iter i): m_i{i}
			{
			}

			auto operator<=>(DerefSecond const& other) const = default;


			auto& operator*() const
			{
				return *(m_i->second);
			}

			auto operator-> () const
			{
				return &(*(*this));
			}

			DerefSecond& operator++()
			{
				++m_i;
				return *this;
			}

			DerefSecond operator++(int)
			{
				auto tmp = *this;
				++(*this);
				return tmp;
			}

			DerefSecond& operator--()
			{
				--m_i;
				return *this;
			}

			DerefSecond operator--(int)
			{
				auto tmp = *this;
				--(*this);
				return tmp;
			}


			auto rawIterator() const
			{
				return m_i;
			}

		private:
			Iter m_i;
		};
	}

	template<class ValueType, class FirstKey, class SecondKey>
	class DoubleKeyMap
	{
	private:
		using owner = detail::UniquePtrDeepConst<ValueType>;

	public:
		enum class InsertResult : int
		{
			NoError,
			FirstKeyExists,
			SecondKeyExists,
			BothExists
		};

		InsertResult insert(ValueType&& value, FirstKey&& key_a, SecondKey&& key_b)
		{
			auto res = static_cast<int>(InsertResult::NoError);

			if(m_first_index.find(key_a) != std::end(m_first_index)) [[unlikely]]
				{
					res |= static_cast<int>(InsertResult::FirstKeyExists);
				}

			if(m_second_index.find(key_b) != std::end(m_second_index)) [[unlikely]]
				{
					res |= static_cast<int>(InsertResult::SecondKeyExists);
				}

			if(res != 0) [[unlikely]]
				{
					return static_cast<InsertResult>(res);
				}

			auto ptr = std::make_unique<ValueType>(std::move(value));
			auto val_ins = &(*ptr);

			auto ip1 = m_first_index.insert(std::make_pair(std::move(key_a), std::move(ptr)));
			;
			auto ip2 = m_second_index.insert(std::make_pair(std::move(key_b), val_ins));
			m_val_to_first_key.insert(std::make_pair(val_ins, &ip1.first->first)).second;
			m_val_to_second_key.insert(std::make_pair(val_ins, &ip2.first->first)).second;
			return static_cast<InsertResult>(res);
		}

		DoubleKeyMap& remove(FirstKey const& key)
		{
			auto i = m_first_index.find(key);
			if(i == std::end(m_first_index)) [[unlikely]]
				{
					return *this;
				}

			auto& val = &(*(i->second));
			auto const& second_key = *m_val_to_second_key.find(val)->second;
			m_first_index.erase(key);
			m_second_index.erase(second_key);
			m_val_to_first_key.erase(val);
			m_val_to_second_key.erase(val);

			return *this;
		}

		DoubleKeyMap& remove(SecondKey const& key)
		{
			auto i = m_second_index.find(key);
			if(i == std::end(m_second_index)) [[unlikely]]
				{
					return *this;
				}

			auto val = &(*(i->second));
			auto const& first_key = *m_val_to_first_key.find(val)->second;
			m_first_index.erase(first_key);
			m_second_index.erase(key);
			m_val_to_first_key.erase(val);
			m_val_to_second_key.erase(val);

			return *this;
		}


		ValueType const* operator[](FirstKey const& key) const
		{
			auto i = m_first_index.find(key);
			return i != std::end(m_first_index) ? &(*(i->second)) : nullptr;
		}

		ValueType* operator[](FirstKey const& key)
		{
			return const_cast<ValueType*>(std::as_const(*this)[key]);
		}

		ValueType const* operator[](SecondKey const& key) const
		{
			auto i = m_second_index.find(key);
			return i != std::end(m_second_index) ? &(*(i->second)) : nullptr;
		}

		ValueType* operator[](SecondKey const& key)
		{
			return const_cast<ValueType*>(std::as_const(*this)[key]);
		}


		size_t size() const
		{
			return m_first_index.size();
		}


		auto valuesByFirstKey() const
		{
			return IterPair{detail::DerefSecond{std::begin(m_first_index)},
			                detail::DerefSecond{std::end(m_first_index)}};
		}

		auto valuesByFirstKey()
		{
			return IterPair{detail::DerefSecond{std::begin(m_first_index)},
			                detail::DerefSecond{std::end(m_first_index)}};
		}

		auto valuesBySecondKey() const
		{
			return IterPair{detail::DerefSecond{std::begin(m_second_index)},
			                detail::DerefSecond{std::end(m_second_index)}};
		}

		auto valuesBySecondKey()
		{
			return IterPair{detail::DerefSecond{std::begin(m_second_index)},
			                detail::DerefSecond{std::end(m_second_index)}};
		}

		auto firstKey() const
		{
			return IterPair{PairFirstIterator{std::begin(m_first_index)},
			                PairFirstIterator{std::end(m_first_index)}};
		}

		auto secondKey() const
		{
			return IterPair{PairFirstIterator{std::begin(m_second_index)},
			                PairFirstIterator{std::end(m_second_index)}};
		}


	private:
		std::map<FirstKey, owner> m_first_index;
		std::map<SecondKey, ValueType*> m_second_index;
		std::map<ValueType*, FirstKey const*> m_val_to_first_key;
		std::map<ValueType*, SecondKey const*> m_val_to_second_key;
	};
}

#endif