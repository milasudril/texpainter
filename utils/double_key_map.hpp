//@	{"targets":[{"name":"double_key_map.hpp", "type":"include"}]}

#ifndef TEXPAINTER_DOUBLEKEYMAP_HPP
#define TEXPAINTER_DOUBLEKEYMAP_HPP

#include <map>
#include <memory>

namespace Texpainter
{
	template<class ValueType, class FirstKey, class SecondKey>
	class DoubleKeyMap
	{
	public:
		enum class InsertResult: int{NoError, FirstKeyExists, SecondKeyExists, BothExists};

		InsertResult insert(ValueType&& value, FirstKey&& key_a, SecondKey&& key_b)
		{
			auto res = static_cast<int>(InsertResult::NoError);

			if(m_first_index.find(key_a) != std::end(m_first_index)) [[unlikely]]
			{ res |= static_cast<int>(InsertResult::FirstKeyExists); }

			if(m_second_index.find(key_b) != std::end(m_second_index)) [[unlikely ]]
			{ res |= static_cast<int>(InsertResult::SecondKeyExists); }

			if(res != 0) [[unlikely]]
			{ return static_cast<InsertResult>(res); }

			auto ptr = std::make_unique<ValueType>(std::move(value));
			auto val_ins = &(*ptr);

			auto ip1 = m_first_index.insert(std::make_pair(std::move(key_a), std::move(ptr)));;
			auto ip2 = m_second_index.insert(std::make_pair(std::move(key_b), val_ins));
			m_val_to_first_key.insert(std::make_pair(val_ins, &ip1.first->first)).second;
			m_val_to_second_key.insert(std::make_pair(val_ins, &ip2.first->first)).second;
			return static_cast<InsertResult>(res);
		}

		DoubleKeyMap& remove(FirstKey const& key)
		{
			auto i = m_first_index.find(key);
			if (i == std::end(m_first_index)) [[unlikely]]
			{ return *this; }

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
			if (i == std::end(m_second_index)) [[unlikely]]
			{ return *this; }

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
			return i != std::end(m_first_index)? &(*(i->second)) : nullptr;
		}

		ValueType* operator[](FirstKey const& key)
		{
			return const_cast<ValueType*>(std::as_const(*this)[key]);
		}

		ValueType const* operator[](SecondKey const& key) const
		{
			auto i = m_second_index.find(key);
			return i != std::end(m_second_index)? &(*(i->second)) : nullptr;
		}

		ValueType* operator[](SecondKey const& key)
		{
			return const_cast<ValueType*>(std::as_const(*this)[key]);
		}


		size_t size() const
		{
			return m_first_index.size();
		}

		// TODO:
		auto itemsByFirstKey() const;

		// TODO:
		auto itemsBySecondKey() const;

	private:
		std::map<FirstKey, std::unique_ptr<ValueType>> m_first_index;
		std::map<SecondKey, ValueType*> m_second_index;
		std::map<ValueType*, FirstKey const*> m_val_to_first_key;
		std::map<ValueType*, SecondKey const*> m_val_to_second_key;
	};
}

#endif