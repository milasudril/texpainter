//@	{
//@	 "targets":[{"name":"memblock.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_MEMBLOCK_HPP
#define TEXPAINTER_MEMBLOCK_HPP

#include <cstdlib>

namespace Texpainter
{
	class Memblock
	{
	public:
		explicit Memblock(size_t size): m_block{malloc(size)} {}

		~Memblock()
		{
			if(m_block != nullptr) { free(m_block); }
		}

		Memblock(Memblock&& other) noexcept: m_block{other.release()} {}

		Memblock& operator=(Memblock&& other) noexcept
		{
			free(m_block);
			m_block = other.release();
			return *this;
		}

		void* get() const { return m_block; }

		void* release()
		{
			auto ret = m_block;
			m_block  = nullptr;
			return ret;
		}

	private:
		void* m_block;
	};

}

#endif
