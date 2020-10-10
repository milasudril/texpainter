//@	{"targets":[{"name":"resource_manager.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_RESOUORCEMANAGER_HPP
#define TEXPAINTER_UTILS_RESOUORCEMANAGER_HPP

#include <map>
#include <cstddef>
#include <cstdint>

#include <cassert>

namespace Texpainter
{
	template<class ResourceType>
	class ResourceId
	{
	public:
		constexpr explicit ResourceId(uint64_t val): m_value{val} {}

		constexpr ResourceId(): m_value{0} {}

		constexpr uint64_t value() const { return m_value; }

		constexpr ResourceId& operator++()
		{
			++m_value;
			return *this;
		}

		constexpr ResourceId operator++(int)
		{
			auto current = *this;
			++(*this);
			return current;
		}

		constexpr auto operator<=>(ResourceId const& other) const = default;

	private:
		uint64_t m_value;
	};

	template<class ResourceType>
	class ResourceManager
	{
	public:
		using mapped_type = ResourceType;
		using key_type    = ResourceId<ResourceType>;

		[[nodiscard("Possible leak")]] auto incUsecout(key_type id)
		{
			auto i = m_objects.find(id);
			assert(i != std::end(m_objects));
			++(i->second.second);
			return std::pair{id, std::ref(i->second.first)};
		}

		void decUsecound(key_type id)
		{
			auto i = m_objects.find(id);
			assert(i != std::end(m_objects));
			assert(i->second.second != 0);
			--(i->second.second);
			if(i->second.second == 0) { m_objects.erase(i); }
		}

		size_t usecount(key_type id) const
		{
			auto i = m_objects.find(id);
			assert(i != std::end(m_objects));
			return i->second.second;
		}

		mapped_type& get(key_type id)
		{
			auto i = m_objects.find(id);
			assert(i != std::end(m_objects));
			return i->second;
		}

		template<class... Args>
		[[nodiscard("Possible leak")]] auto create(Args&&... args)
		{
			auto ip = m_objects.insert(m_current_id,
			                           std::pair{ResourceType{std::forward<Args>(args)...}, 1});
			++m_current_id;
			return std::pair{ip->first.first, std::ref(ip->first.second.first)};
		}

	private:
		std::map<key_type, std::pair<ResourceType, size_t>> m_objects;
		key_type m_current_id;
	};
}

#endif
