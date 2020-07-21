//@	{
//@	 "targets":[{"name":"function_ref.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UTILS_FUNCTIONREF_HPP
#define TEXPAINTER_UTILS_FUNCTIONREF_HPP

#include <utility>
#include <type_traits>
#include <cstdio>

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
		explicit FunctionRef(Function& f)
		    : r_handle{&f}
		    , r_func{[](void* handle, Args... args) {
			    auto& self = *reinterpret_cast<Function*>(handle);
			    return self(std::forward<Args>(args)...);
		    }}
		{
		}

		decltype(auto) operator()(Args... args) const
		{
			return r_func(r_handle, std::forward<Args>(args)...);
		}

		decltype(auto) handle() const { return r_handle; }

		decltype(auto) function() const { return r_func; }


	private:
		void* r_handle;
		R (*r_func)(void*, Args...);
	};
}

#endif