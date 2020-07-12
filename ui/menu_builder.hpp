//@	{
//@  "targets":[{"name":"menu_builder.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UI_MENUBUILDER_HPP
#define TEXPAINTER_UI_MENUBUILDER_HPP

#include "./menu_bar.hpp"
#include "./menu_item.hpp"
#include "./menu.hpp"

#include "utils/empty.hpp"

#include <type_traits>
#include <memory>

namespace Texpainter::Ui
{
	namespace detial
	{
		template<class T, size_t N>
		class InitArray
		{
		public:
			template<class... CtorArgs>
			explicit InitArray(CtorArgs&&... args)
			{
				for(size_t k = 0; k < N; ++k)
				{
					new(&m_data[k]) T(std::forward<CtorArgs>(args)...);
				}
			}

			static constexpr size_t size()
			{
				return N;
			}

			T& operator[](size_t n)
			{
				return *reinterpret_cast<T*>(&m_data[n]);
			}

		private:
			std::aligned_storage_t<sizeof(T), alignof(T)> m_data[N];
		};
	}

	template<class EnumType, template<EnumType> class EnumTypeTraits>
	class SubmenuBuilder
	{
	public:
		using ControlId = EnumType;

		explicit SubmenuBuilder(Container& owner): m_root{owner}, m_items{m_root}
		{

		}

	private:
		Menu m_root;
		detial::InitArray<MenuItem, static_cast<size_t>(end(Empty<EnumType>{}))> m_items;
	};

	template<class EnumType, template<EnumType> class EnumTypeTraits>
	class MenuBuilder
	{
	public:
		using ControlId = EnumType;

		explicit MenuBuilder(Container& owner): m_root{owner}, m_items{m_root}
		{
			forEachEnumItem<EnumType>([this](auto tag) {
				get<tag.value>(m_items).label(EnumTypeTraits<tag.value>::displayName());
			});
		}

	private:
		MenuBar m_root;
		TupleFromEnum<EnumType, EnumTypeTraits> m_items;
	};
}

#endif