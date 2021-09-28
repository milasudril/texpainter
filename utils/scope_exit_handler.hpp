//@	{
//@	  "targets":[{"name":"scope_exit_handler.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_UTILS_SCOPEEXITHANDLER_HPP
#define TEXPAINTER_UTILS_SCOPEEXITHANDLER_HPP

#include <optional>

namespace Texpainter
{
	template<class Callable>
	class ScopeExitHandler
	{
	public:
		explicit ScopeExitHandler(Callable&& f) noexcept: m_obj{std::move(f)} {}

		ScopeExitHandler(ScopeExitHandler&& other) noexcept: m_obj{std::move(other.m_obj)}
		{
			other.m_obj.reset();
		}

		ScopeExitHandler& operator=(ScopeExitHandler&& other) noexcept
		{
			m_obj = std::move(other.m_obj);
			other.m_obj.reset();
		}

		~ScopeExitHandler()
		{
			if(m_obj) { (*m_obj)(); }
		}

	private:
		std::optional<Callable> m_obj;
	};
}

#endif
