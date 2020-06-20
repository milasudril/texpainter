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
	};

	namespace detail
	{
		template<class ActionId, class Function, class... Args>
		using dispatch_ret_type = std::result_of_t<Function(Tag<begin(Empty<ActionId>{})>, Args&&...)>;

		template<class ActionId, class Function, class... Args>
		using DispatchFptr = dispatch_ret_type<ActionId, Function, Args...> (*)(Function&& f, Args...);


		template<class ActionId>
		constexpr int itemDistance(ActionId a, ActionId b)
		{
			return static_cast<int>(b) - static_cast<int>(a);
		}

		template<class ActionId>
		constexpr ActionId previous(ActionId a)
		{
			return ActionId{static_cast<int>(a) - 1};
		}

		template<class ActionId>
		constexpr auto itemCount(Empty<ActionId> x)
		{
			return itemDistance(begin(x), end(x));
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
				constexpr auto current_id = previous(action_id);
				pointers[itemDistance(begin(Empty<ActionId>{}), current_id)] = []<class... A>(auto f, A... args)
				{
					return f(Tag<current_id>{}, std::forward<Args>(args)...);
				};

				if constexpr(current_id != begin(Empty<ActionId>{}))
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
		return vtable[detail::itemDistance(begin(Empty<ActionId>{}), id)](std::forward<Function>(f),
		                                                                  std::forward<Args>(args)...);
	}
}

#endif