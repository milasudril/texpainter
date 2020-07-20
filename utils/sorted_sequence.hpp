//@	{"targets":[{"name":"sorted_sequence.hpp", "type":"include"}]}

#ifndef TEXPAINTER_DOUBLEKEYMAP_HPP
#define TEXPAINTER_DOUBLEKEYMAP_HPP

#include "./iter_pair.hpp"
#include "./pair_iterator.hpp"

#include <map>
#include <memory>
#include <type_traits>
#include <vector>
#include <algorithm>

namespace Texpainter
{
	namespace detail
	{
	}

	template<class Key, class Value, class IndexType = size_t>
	class SortedSequence
	{
	public:
		IndexType size() const
		{
			return IndexType{m_map.size()};
		}

		auto insert(std::pair<Key, Value>&& val)
		{
			auto i = m_map.find(val.first);
			if(i != std::end(m_map)) [[unlikely]]
				{
					return std::make_pair(i, false);
				}

			auto ret = m_map.insert(i, std::make_pair(std::move(val.first), std::move(val.second)));
			m_seq.push_back(&ret->second);
			return std::make_pair(ret, true);
		}

		auto rename(Key const& old_name, Key&& new_name)
		{
			auto i_old = m_map.find(old_name);
			auto i_new = m_map.find(new_name);

			if(i_old == std::end(m_map)) [[unlikely]]
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

		auto erase(Key const& name)
		{
			auto i = m_map.find(name);
			if(i == std::end(m_map)) [[unlikely]]
				{
					return false;
				}

			auto j = std::ranges::find(m_seq, &i->second);
			m_seq.erase(j);
			m_map.erase(i);
			return true;
		}

		IndexType moveForward(IndexType index)
		{
			if(index == IndexType{0} || index >= size()) [[unlikely]]
				{
					return index;
				}

			std::swap(m_seq[static_cast<size_t>(index - 1)], m_seq[static_cast<size_t>(index)]);
			return index - 1;
		}

		IndexType moveBackward(IndexType index)
		{
			if(index + 1 >= size()) [[unlikely]]
				{
					return index;
				}

			std::swap(m_seq[static_cast<size_t>(index + 1)], m_seq[static_cast<size_t>(index)]);
			return index + 1;
		}

		void moveFront(IndexType index)
		{
			while(true)
			{
				auto const index_old = index;
				index = moveForward(index);
				if(index == index_old) { return; }
			}
		}

		void moveBack(IndexType index)
		{
			while(true)
			{
				auto const index_old = index;
				index = moveBackward(index);
				if(index == index_old) { return; }
			}
		}

		Value const* operator[](Key const& key) const
		{
			auto i = m_map.find(key);
			return i != std::end(m_map) ? &i->secondś : nullptr;
		}

		Value* operator[](Key const& key)
		{
			return const_cast<Value*>(std::as_const(*this)[key]);
		}

		Value const* operator[](IndexType i) const
		{
			return m_seq[static_cast<size_t>(i)];
		}

		Value* operator[](IndexType i)
		{
			return m_seq[static_cast<size_t>(i)];
		}

		auto valuesByKey() const
		{
		}

		auto valuesByIndex() const
		{
		}

		auto keys() const
		{
		}

	private:
		std::map<Key, Value> m_map;
		std::vector<Value*> m_seq;
	};
}

#endif