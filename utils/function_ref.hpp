//@	{
//@	 "targets":[{"name":"function_ref.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UTILS_FUNCTIONREF_HPP
#define TEXPAINTER_UTILS_FUNCTIONREF_HPP

#include <utility>
#include <type_traits>
namespace Texpainter
{
	template<class>
	class FunctionRef;

	template<class R, class... Args>
	class FunctionRef<R(Args...)>
	{
	public:
		template<class Function,
		         std::enable_if_t<!std::is_same_v<std::decay_t<Function>, FunctionRef>, int> = 0>
		constexpr explicit FunctionRef(Function& f)
		    : r_handle{&f}
		    , r_func{[](void* handle, Args... args) {
			    auto& self = *static_cast<Function*>(handle);
			    return self(std::forward<Args>(args)...);
		    }}
		{
		}

		template<class Function, class Tag>
		constexpr explicit FunctionRef(Function& f, Tag)
		    : r_handle{&f}
		    , r_func{[](void* handle, Args... args) {
			    auto& self = *static_cast<Function*>(handle);
			    return self(Tag{}, std::forward<Args>(args)...);
		    }}
		{
		}

		constexpr decltype(auto) operator()(Args... args) const
		{
			return r_func(r_handle, std::forward<Args>(args)...);
		}

		constexpr decltype(auto) handle() const { return r_handle; }

		constexpr decltype(auto) function() const { return r_func; }


	private:
		void* r_handle;
		R (*r_func)(void*, Args...);
	};

	using SimpleCallback = FunctionRef<void()>;
}

#endif