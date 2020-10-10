//@	{"targets":[{"name":"resource_manager.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_RESOUORCEMANAGER_HPP
#define TEXPAINTER_UTILS_RESOUORCEMANAGER_HPP

#include <map>
#include <cstddef>
#include <cstdint>
#include <algorithm>

#include <cassert>

#include <memory>

namespace Texpainter
{
	template<class T>
	class TaggedId
	{
	public:
		constexpr explicit TaggedId(uint64_t val): m_value{val} {}

		constexpr TaggedId(): m_value{0} {}

		constexpr uint64_t value() const { return m_value; }

		constexpr TaggedId& operator++()
		{
			++m_value;
			return *this;
		}

		constexpr TaggedId operator++(int)
		{
			auto current = *this;
			++(*this);
			return current;
		}

		constexpr auto operator<=>(TaggedId const& other) const = default;

	private:
		uint64_t m_value;
	};

	template<class ResourceType>
	class ResourceManager
	{
	public:
		using ResourceId = TaggedId<ResourceType>;

		class ResourceHandle
		{
		public:
			ResourceHandle(): r_manager{nullptr} {}

			explicit ResourceHandle(ResourceId id, ResourceManager& manager)
			    : m_id{id}
			    , r_manager{&manager}
			{
			}

			ResourceHandle(ResourceHandle&& other) noexcept
			    : m_id{other.m_id}
			    , r_manager{other.r_manager}
			{
				other.r_manager = nullptr;
			}

			ResourceHandle& operator=(ResourceHandle&& other) noexcept
			{
				m_id            = other.m_id;
				r_manager       = other.r_manager;
				other.r_manager = nullptr;
				return *this;
			}

			~ResourceHandle()
			{
				if(r_manager != nullptr) { r_manager->decUsecount(m_id); }
			}

			bool valid() const { return r_manager != nullptr; }

			std::pair<ResourceId, std::reference_wrapper<ResourceManager>> release()
			{
				assert(valid());
				auto ret  = std::pair{m_id, std::ref(*r_manager)};
				r_manager = nullptr;
				return ret;
			}

		private:
			ResourceId m_id;
			ResourceManager* r_manager;
		};

		class Resource
		{
		public:
			explicit Resource(ResourceType&& res): m_resource{std::move(res)}, m_use_count{0} {}
			Resource(Resource const&) = delete;
			Resource(Resource&&)      = default;

			bool inUse() const { return m_use_count != 0; }

			ResourceType& get() { return m_resource; }

			ResourceType const& get() const { return m_resource; }

		private:
			friend class ResourceManager;

			ResourceType m_resource;
			size_t m_use_count;
		};

		[[nodiscard("Possible leak")]] auto& create(ResourceType&& resource)
		{
			auto ip = m_objects.insert(std::pair{m_current_id, Resource{std::move(resource)}});
			++(ip.first->second.m_use_count);
			++m_current_id;
			return *ip.first;
		}

		bool hasResource(ResourceId id) const { return m_objects.contains(id); }

		Resource& get(ResourceId id)
		{
			auto i = m_objects.find(id);
			assert(i != std::end(m_objects));
			return i->second;
		}

		auto objectCount() const { return m_objects.size(); }

		auto const& objects() const { return m_objects; }

		auto& objects() { return m_objects; }

		[[nodiscard("Possible leak")]] auto& incUsecount(ResourceId id)
		{
			auto i = m_objects.find(id);
			assert(i != std::end(m_objects));
			++(i->second.m_use_count);
			return *i;
		}

		void decUsecount(ResourceId id)
		{
			auto i = m_objects.find(id);
			assert(i != std::end(m_objects));
			assert(i->second.m_use_count != 0);
			--(i->second.m_use_count);
			if(i->second.m_use_count == 0) { m_objects.erase(i); }
		}

#if 0
		void purge()
		{
			auto pred = [](auto const& item){return !item.second.inUse();};
			auto i = std::ranges::find_if(m_objects, pred);
			while(i != std::end(m_objects))
			{ i = std::find_if(m_objects.erase(i), std::end(m_objects), pred);}
		}
#endif

	private:
		std::map<ResourceId, Resource> m_objects;
		ResourceId m_current_id;
	};
}
#endif