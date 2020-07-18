//@	{"targets":[{"name":"indexed_sequence.hpp", "type":"include"}]}

#ifndef TEXPAINTER_INDEXEDSEQUENCE_HPP
#define TEXPAINTER_INDEXEDSEQUENCE_HPP

#include "./sequence.hpp"
#include "./iter_pair.hpp"
#include "./deref_iterator.hpp"
#include "./pair_iterator.hpp"

#include <map>
#include <memory>

namespace Texpainter
{
	template<class ValueType, class IndexType, class Name>
	class IndexedSequence
	{
	public:
		IndexedSequence(): m_index{std::make_unique<Index>()}
		{
		}

		bool append(Name&& name, ValueType&& val)
		{
			auto i = m_index->m_name_to_index.find(name);
			if(i != std::end(m_index->m_name_to_index)) [[unlikely]]
				{
					return false;
				}

			auto ip =
			   m_index->m_name_to_index.insert(std::make_pair(std::move(name), IndexType{m_seq.size()}));
			auto const& item = *ip.first;
			m_index->m_index_to_name.append(&item.first);
			m_seq.append(std::move(val));
			return true;
		}


		auto valuesInSequence() const
		{
			return IterPair{std::begin(m_seq), std::end(m_seq)};
		}

		auto namesInSequence() const
		{
			auto const& index_to_name = m_index->m_index_to_name;
			return IterPair{DerefIterator{std::begin(index_to_name)},
			                DerefIterator{std::end(index_to_name)}};
		}

		auto namesInOrder() const
		{
			auto const& name_index_map = m_index->m_name_to_index;
			return IterPair{PairFirstIterator{std::begin(name_index_map)},
			                PairFirstIterator{std::end(name_index_map)}};
		}

		auto indicesInOrder() const
		{
			auto const& name_index_map = m_index->m_name_to_index;
			return IterPair{PairSecondIterator{std::begin(name_index_map)},
			                PairSecondIterator{std::end(name_index_map)}};
		}


		auto size() const
		{
			return m_seq.size();
		}


		ValueType const* operator[](IndexType i) const
		{
			return &m_seq[i];
		}

		ValueType const* operator[](Name const& name) const
		{
			auto i = m_index->m_name_to_index.find(name);
			return i != std::end(m_index->m_name_to_index) ? (*this)[i->second] : nullptr;
		}

		ValueType* operator[](IndexType i)
		{
			return const_cast<ValueType*>(std::as_const(*this)[i]);
		}

		ValueType* operator[](Name const& name)
		{
			return const_cast<ValueType*>(std::as_const(*this)[name]);
		}

	private:
		Sequence<ValueType, IndexType> m_seq;

		struct Index
		{
			std::map<Name, IndexType> m_name_to_index;
			Sequence<Name const*, IndexType> m_index_to_name;
		};
		std::unique_ptr<Index> m_index;
	};
}

#endif