//@	{
//@	 "targets":[{"name":"unique_function.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UTILS_UNIQUEFUNCTION_HPP
#define TEXPAINTER_UTILS_UNIQUEFUNCTION_HPP

#include <type_traits>
#include <utility>
#include <cassert>

namespace Texpainter
{
	template<class>
	class UniqueFunction;

	template<class R, class... Args>
	class UniqueFunction<R(Args...)>
	{
	public:
		UniqueFunction() = delete;

		template<class F,
		         std::enable_if_t<!std::is_same_v<std::decay_t<F>, UniqueFunction>, int> = 0>
		explicit UniqueFunction(F&& obj)
		    : m_data{new std::decay_t<F>{std::forward<F>(obj)}}
		    , r_callback{callback<std::decay_t<F>>}
		    , r_dtor{dtor<std::decay_t<F>>}
		{
		}

		UniqueFunction(UniqueFunction const& other) = delete;
		UniqueFunction& operator=(UniqueFunction const& other) = delete;

		UniqueFunction(UniqueFunction&& other) noexcept
		    : m_data{other.m_data}
		    , r_callback{other.r_callback}
		    , r_dtor{other.r_dtor}
		{
			other.r_dtor = null_dtor;
		}

		UniqueFunction& operator=(UniqueFunction&& other) noexcept
		{
			r_dtor(m_data);
			m_data       = other.m_data;
			r_callback   = other.r_callback;
			r_dtor       = other.r_dtor;
			other.r_dtor = null_dtor;
			return *this;
		}

		~UniqueFunction() noexcept { r_dtor(m_data); }

		decltype(auto) operator()(Args... args) const
		{
			assert(valid());
			return r_callback(m_data, std::forward<Args>(args)...);
		}

		bool valid() const { return r_dtor != null_dtor; }

	private:
		void* m_data;
		R (*r_callback)(void* data, Args... args);
		void (*r_dtor)(void* data) noexcept;

		static void null_dtor(void*) noexcept {}

		template<class F>
		static void dtor(void* f) noexcept
		{
			delete reinterpret_cast<F*>(f);
		}

		template<class F>
		static R callback(void* f, Args... args)
		{
			auto& self = *reinterpret_cast<F*>(f);
			return self(std::forward<Args>(args)...);
		}
	};
}
#endif