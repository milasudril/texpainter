//@	{"targets":[{"name":"with_status.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_WITHSTATUS_HPP
#define TEXPAINTER_UTILS_WITHSTATUS_HPP

#include "./inplace_mutator.hpp"

namespace Texpainter
{
	template<class T>
	class WithStatus
	{
	public:
		WithStatus(): m_dirty{false} {}

		explicit WithStatus(T&& obj): m_content{obj}, m_dirty{false} {}

		T const& get() const { return m_content; }

		template<InplaceMutator<T> Mutator>
		auto modify(Mutator&& mut)
		{
			auto result = mut(m_content);
			m_dirty     = m_dirty || result;
			return result;
		}

		bool dirty() const { return m_dirty; }

		void clearStatus() { m_dirty = false; }

	private:
		T m_content;
		bool m_dirty;
	};
}

#endif
