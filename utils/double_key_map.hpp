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
			   std::remove_reference_t<decltype(*(std::declval<typename Iter::value_type>().second.first))>;
			using iterator_category = std::bidirectional_iterator_tag;

			DerefSecond() = default;

			DerefSecond(DerefSecond const&) = default;

			explicit DerefSecond(Iter i): m_i{i}
			{
			}

			auto operator<=>(DerefSecond const& other) const = default;


			auto& operator*() const
			{
				return *(m_i->second.first);
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
			BothKeysExist
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

			auto ip1 = m_first_index.insert(
			   std::make_pair(std::move(key_a), std::make_pair(std::move(ptr), nullptr)));
			auto ip2 =
			   m_second_index.insert(std::make_pair(std::move(key_b), std::make_pair(val_ins, nullptr)));
			ip1.first->second.second = &ip2.first->first;
			ip2.first->second.second = &ip1.first->first;

			return static_cast<InsertResult>(res);
		}

		DoubleKeyMap& remove(FirstKey const& key)
		{
			auto i = m_first_index.find(key);
			if(i == std::end(m_first_index)) [[unlikely]]
				{
					return *this;
				}

			m_second_index.erase(*(i->second.second));
			m_first_index.erase(i);

			return *this;
		}

		DoubleKeyMap& remove(SecondKey const& key)
		{
			auto i = m_second_index.find(key);
			if(i == std::end(m_second_index)) [[unlikely]]
				{
					return *this;
				}

			m_first_index.erase(*(i->second.second));
			m_second_index.erase(i);

			return *this;
		}


		auto& moveForward(SecondKey const& key)
		{
			auto i = m_second_index.find(key);
			if(i != std::end(m_second_index) && i != std::begin(m_second_index)) [[likely]]
				{
					auto i_prev = i;
					--i_prev;
					m_first_index.find(*(i->second.second))->second.second = &i_prev->first;
					m_first_index.find(*(i_prev->second.second))->second.second = &i->first;
					std::swap(i_prev->second, i->second);
				}
			return *this;
		}

		auto& moveBackward(SecondKey const& key)
		{
			auto i = m_second_index.find(key);
			if(i == std::end(m_second_index)) [[unlikely]]
				{
					return *this;
				}

			auto i_next = i;
			++i_next;
			if(i_next == std::end(m_second_index)) [[unlikely]]
				{
					return *this;
				}

			m_first_index.find(*(i->second.second))->second.second = &i_next->first;
			m_first_index.find(*(i_next->second.second))->second.second = &i->first;
			std::swap(i_next->second, i->second);
			return *this;
		}


		ValueType const* operator[](FirstKey const& key) const
		{
			auto i = m_first_index.find(key);
			return i != std::end(m_first_index) ? &(*(i->second.first)) : nullptr;
		}

		ValueType* operator[](FirstKey const& key)
		{
			return const_cast<ValueType*>(std::as_const(*this)[key]);
		}

		ValueType const* operator[](SecondKey const& key) const
		{
			auto i = m_second_index.find(key);
			return i != std::end(m_second_index) ? &(*(i->second.first)) : nullptr;
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
		std::map<FirstKey, std::pair<owner, SecondKey const*>> m_first_index;
		std::map<SecondKey, std::pair<ValueType*, FirstKey const*>> m_second_index;
	};
}

#endif