//@	{"targets":[{"name":"sorted_sequence.hpp", "type":"include"}]}

#ifndef TEXPAINTER_DOUBLEKEYMAP_HPP
#define TEXPAINTER_DOUBLEKEYMAP_HPP

#include "./iter_pair.hpp"
#include "./pair_iterator.hpp"

#include <map>
#include <memory>
#include <type_traits>
#include <vector>

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
	}

	template<class Key, class Value>
	class SortedSequence
	{
		using ValueHolder = detail::UniquePtrDeepConst<Value>;
	public:

		auto size() const
		{ return m_map.size(); }

		auto insert(std::pair<Key, Value>&& val)
		{
			auto i = m_map.find(val.first);
			if( i != std::end(m_map)) [[unlikely]]
			{
				return std::make_pair(i, false);
			}

			auto ret = m_map.insert(i,
				std::make_pair(std::move(val.first), std::move(val.second)));
			m_seq.push_back(&ret->second);
			return std::make_pair(ret, true);
		}

		auto rename(Key const& old_name, Key&& new_name)
		{
			auto i_old = m_map.find(old_name);
			auto i_new = m_map.find(new_name);

			if( i_old == std::end(m_map)) [[unlikely]]
			{
				return true;
			}

			if(i_new == std::end(m_map)) [[unlikely]]
			{
				return false;
			}

			auto node = extract(i_old);
			node.key() = std::move(new_name);
			m_map.insert(std::move(node));
			return true;
		}
#if 0
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

		auto rename(FirstKey const& key_old, FirstKey&& key_new)
		{
			auto i_new = m_first_index.find(key_new);
			auto i_old = m_first_index.find(key_old);

			if(i_new != std::end(m_first_index)) [[unlikely]]
				{
					return InsertResult::FirstKeyExists;
				}

			if(i_old == std::end(m_first_index)) [[unlikely]]
				{
					return InsertResult::NoError;
				}

			auto ip = m_first_index.insert(std::make_pair(std::move(key_new), std::move(i_old->second)));
			m_second_index.find(*(ip.first->second.second))->second.second = &ip.first->first;
			m_first_index.erase(i_old);
			return InsertResult::NoError;
		}

		auto rename(FirstKey const& key_old, SecondKey&& key_new)
		{
			auto i_new = m_second_index.find(key_new);
			auto i_old = m_first_index.find(key_old);

			if(i_new != std::end(m_second_index)) [[unlikely]]
				{
					return InsertResult::SecondKeyExists;
				}

			if(i_old == std::end(m_first_index)) [[unlikely]]
				{
					return InsertResult::NoError;
				}

			auto ip = m_second_index.insert(
			   std::make_pair(std::move(key_new), std::make_pair(&(*i_old->second.first), &i_old->first)));
			m_second_index.erase(*(i_old->second.second));
			i_old->second.second = &ip.first->first;
			return InsertResult::NoError;
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
#endif

	private:
		std::map<Key, Value> m_map;
		std::vector<Value*> m_seq;
	};
}

#endif