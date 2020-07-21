//@	{"targets":[{"name":"empty.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_EMPTY_HPP
#define TEXPAINTER_UTILS_EMPTY_HPP

#include <utility>
#include <type_traits>
#include <array>

namespace Texpainter
{
	template<class T>
	struct Empty
	{
		using type = T;
	};

	template<auto action_id>
	struct Tag
	{
		static constexpr auto value = action_id;
		using value_type            = decltype(action_id);
	};

	// TODO: Add concept
	template<class ActionId>
	constexpr int itemDistance(ActionId a, ActionId b)
	{
		return static_cast<int>(b) - static_cast<int>(a);
	}


	namespace detail
	{
		template<class ActionId>
		constexpr ActionId previous(ActionId a)
		{
			return ActionId{static_cast<int>(a) - 1};
		}

		template<class ActionId, class Function, class... Args>
		using dispatch_ret_type =
		    std::result_of_t<Function(Tag<previous(end(Empty<ActionId>{}))>, Args&&...)>;

		template<class ActionId, class Function, class... Args>
		using DispatchFptr = dispatch_ret_type<ActionId, Function, Args...> (*)(Function&& f,
		                                                                        Args...);


		template<class ActionId>
		constexpr auto itemCount(Empty<ActionId> x)
		{
			return static_cast<int>(end(x));
		}

		template<class ActionId, class Function, class... Args>
		using FptrArray =
		    std::array<DispatchFptr<ActionId, Function, Args...>, itemCount(Empty<ActionId>{})>;

		template<auto action_id>
		struct GenVtable
		{
			using ActionId = decltype(action_id);

			template<class Function, class... Args>
			constexpr static void assign(FptrArray<ActionId, Function, Args...>& pointers)
			{
				constexpr auto current_id              = previous(action_id);
				pointers[static_cast<int>(current_id)] = []<class... A>(auto f, A... args)
				{
					return f(Tag<current_id>{}, std::forward<Args>(args)...);
				};

				if constexpr(static_cast<int>(current_id) != 0)
				{ GenVtable<current_id>::template assign<Function, Args...>(pointers); }
			}
		};

		template<class ActionId, class Function, class... Args>
		constexpr auto genVtable()
		{
			FptrArray<ActionId, Function, Args...> ret{};
			GenVtable<end(Empty<ActionId>{})>::template assign<Function, Args...>(ret);
			return ret;
		}
	}

	template<class ActionId, class Function, class... Args>
	auto select(ActionId id, Function&& f, Args... args)
	{
		constexpr auto vtable = detail::genVtable<ActionId, Function, Args...>();
		return vtable[static_cast<int>(id)](std::forward<Function>(f), std::forward<Args>(args)...);
	}

	namespace detail
	{
		template<auto enum_item>
		struct VisitEnumItem
		{
			using EnumType = decltype(enum_item);

			template<class Function>
			constexpr static void process(Function&& f)
			{
				constexpr auto current_id = previous(enum_item);
				f(Tag<current_id>{});
				if constexpr(static_cast<int>(current_id) != 0)
				{ VisitEnumItem<current_id>::process(f); }
			}
		};
	}

	template<class EnumType, class Function>
	constexpr void forEachEnumItem(Function&& f)
	{
		detail::VisitEnumItem<end(Empty<EnumType>{})>::process(f);
	}


	namespace detail
	{
		template<class EnumType, int enum_item, template<auto> class EnumItemTraits>
		struct MakeTupleFromEnum
		    : public MakeTupleFromEnum<EnumType, previous(enum_item), EnumItemTraits>
		{
		private:
			using Base = MakeTupleFromEnum<EnumType, previous(enum_item), EnumItemTraits>;
			static constexpr auto CurrentIndex = previous(static_cast<EnumType>(enum_item));

		public:
			typename EnumItemTraits<CurrentIndex>::type m_value;

			template<class... CtorArgs>
			explicit MakeTupleFromEnum(CtorArgs&&... args): Base{args...}
			                                              , m_value{args...}
			{
			}
		};

		template<class EnumType, template<auto> class EnumItemTraits>
		struct MakeTupleFromEnum<EnumType, 0, EnumItemTraits>
		{
			using type = EnumType;

			template<class... CtorArgs>
			explicit MakeTupleFromEnum(CtorArgs&&...)
			{
			}

			static constexpr auto size() { return static_cast<size_t>(end(Empty<EnumType>{})); }
		};
	}

	template<class EnumType, template<auto> class EnumItemTraits>
	using TupleFromEnum = detail::
	    MakeTupleFromEnum<EnumType, static_cast<int>(end(Empty<EnumType>{})), EnumItemTraits>;

	template<auto EnumType, template<auto> class EnumItemTraits>
	constexpr auto const& get(TupleFromEnum<decltype(EnumType), EnumItemTraits> const& x)
	{
		return static_cast<detail::MakeTupleFromEnum<decltype(EnumType),
		                                             static_cast<int>(EnumType) + 1,
		                                             EnumItemTraits> const&>(x)
		    .m_value;
	}

	template<auto EnumType, template<auto> class EnumItemTraits>
	constexpr auto& get(TupleFromEnum<decltype(EnumType), EnumItemTraits>& x)
	{
		return static_cast<detail::MakeTupleFromEnum<decltype(EnumType),
		                                             static_cast<int>(EnumType) + 1,
		                                             EnumItemTraits>&>(x)
		    .m_value;
	}
}

#endif