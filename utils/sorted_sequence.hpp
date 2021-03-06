//@	{"targets":[{"name":"sorted_sequence.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_SORTEDSEQUENCE_HPP
#define TEXPAINTER_UTILS_SORTEDSEQUENCE_HPP

#include "./iter_pair.hpp"
#include "./pair_iterator.hpp"
#include "./deref_iterator.hpp"
#include "./to_string.hpp"

#include <map>
#include <memory>
#include <type_traits>
#include <vector>
#include <algorithm>

namespace Texpainter
{
	template<class Key, class Value, class IndexType = size_t>
	class SortedSequence
	{
	public:
		IndexType size() const { return IndexType{m_map.size()}; }

		auto insert(std::pair<Key, Value>&& val)
		{
			auto i = m_map.find(val.first);
			if(i != std::end(m_map)) [[unlikely]]
				{
					return std::make_pair(i, false);
				}

			auto ret = m_map.insert(i, std::move(val));
			m_key_seq.push_back(&ret->first);
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

			if(i_new != std::end(m_map)) [[unlikely]]
				{
					return false;
				}

			auto node  = m_map.extract(i_old);
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

			m_seq.erase(std::ranges::find(m_seq, &i->second));
			m_key_seq.erase(std::ranges::find(m_key_seq, &i->first));
			m_map.erase(i);
			return true;
		}

		IndexType moveForward(IndexType index)
		{
			if(index == IndexType{0} || index >= size()) [[unlikely]]
				{
					return index;
				}

			std::swap(m_seq[static_cast<size_t>(index) - 1], m_seq[static_cast<size_t>(index)]);
			std::swap(m_key_seq[static_cast<size_t>(index) - 1],
			          m_key_seq[static_cast<size_t>(index)]);
			return IndexType{static_cast<size_t>(index) - 1};
		}

		IndexType moveBackward(IndexType index)
		{
			if(static_cast<size_t>(index) + 1 >= static_cast<size_t>(size())) [[unlikely]]
				{
					return index;
				}

			std::swap(m_seq[static_cast<size_t>(index) + 1], m_seq[static_cast<size_t>(index)]);
			std::swap(m_key_seq[static_cast<size_t>(index) + 1],
			          m_key_seq[static_cast<size_t>(index)]);
			return IndexType{static_cast<size_t>(index) + 1};
		}

		void moveFront(IndexType index)
		{
			while(true)
			{
				auto const index_old = index;
				index                = moveForward(index);
				if(index == index_old) { return; }
			}
		}

		void moveBack(IndexType index)
		{
			while(true)
			{
				auto const index_old = index;
				index                = moveBackward(index);
				if(index == index_old) { return; }
			}
		}

		Value const* operator[](Key const& key) const
		{
			auto i = m_map.find(key);
			return i == std::end(m_map) ? nullptr : &i->second;
		}

		Value* operator[](Key const& key) { return const_cast<Value*>(std::as_const(*this)[key]); }

		Value const* operator[](IndexType i) const { return m_seq[static_cast<size_t>(i)]; }

		Value* operator[](IndexType i) { return m_seq[static_cast<size_t>(i)]; }

		IndexType index(Key const& key) const
		{
			auto val_ptr = (*this)[key];
			auto i       = std::ranges::find(m_seq, val_ptr);
			return i == std::end(m_seq) ? IndexType{}
			                            : IndexType{static_cast<size_t>(i - std::begin(m_seq))};
		}

		size_t location(Key const& key) const
		{
			auto i = m_map.find(key);
			return i == std::end(m_map) ? static_cast<size_t>(-1)
			                            : std::distance(std::begin(m_map), i);
		}

		Key const* key(IndexType i) const
		{
			return static_cast<size_t>(i) < m_key_seq.size() ? m_key_seq[static_cast<size_t>(i)]
			                                                 : nullptr;
		}

		auto valuesByIndex() const
		{
			return IterPair{DerefIterator{std::begin(m_seq)}, DerefIterator{std::end(m_seq)}};
		}

		auto keysByIndex() const
		{
			return IterPair{DerefIterator{std::begin(m_key_seq)},
			                DerefIterator{std::end(m_key_seq)}};
		}

		auto valuesByKey() const
		{
			return IterPair{PairSecondIterator{std::begin(m_map)},
			                PairSecondIterator{std::end(m_map)}};
		}

		auto keys() const
		{
			return IterPair{PairFirstIterator{std::begin(m_map)},
			                PairFirstIterator{std::end(m_map)}};
		}

	private:
		std::map<Key, Value> m_map;
		std::vector<Value*> m_seq;
		std::vector<Key const*> m_key_seq;
	};

	template<class Key, class Value, class IndexType = size_t>
	void insertOrThrow(SortedSequence<Key, Value, IndexType>& seq, Key const& key, Value&& value)
	{
		auto ip = seq.insert(std::pair{key, std::move(value)});
		if(!ip.second)
		{ throw std::string{"An item with name "} + toString(key) + " already exists."; }
	}


	template<class Key, class Value, class IndexType = size_t>
	void renameOrThrow(SortedSequence<Key, Value, IndexType>& seq,
	                   Key const& old_name,
	                   Key const& new_name)
	{
		auto tmp = new_name;
		if(!seq.rename(old_name, std::move(tmp)))
		{ throw std::string{"An item with name "} + toString(new_name) + " already exists."; }
	}
}

#endif