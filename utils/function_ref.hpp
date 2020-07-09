//@	{
//@	 "targets":[{"name":"function_ref.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UTILS_FUNCTIONREF_HPP
#define TEXPAINTER_UTILS_FUNCTIONREF_HPP

#include <utility>

namespace Texpainter
{
	template<class>
	class FunctionRef;

	template<class R, class... Args>
	class FunctionRef<R(Args...)>
	{
	public:
		template<class Function>
		explicit FunctionRef(Function& f):
		   r_handle{&f},
		   r_func{[](void* handle, Args... args) {
			   auto& self = *reinterpret_cast<Function*>(handle);
			   return self(std::forward<Args>(args)...);
		   }}
		{
		}

		decltype(auto) operator()(Args... args) const
		{
			return r_func(r_handle, std::forward<Args>(args)...);
		}


	private:
		void* r_handle;
		R (*r_func)(void*, Args...);
	};

}

#endif