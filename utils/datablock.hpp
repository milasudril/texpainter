//@	{"targets":[{"name":"datablock.hpp", "type":"include"}]}

#ifndef TEXPAINTER_DATABLOCK_HPP
#define TEXPAINTER_DATABLOCK_HPP

#include "./trivial.hpp"
#include "./empty.hpp"

#include <type_traits>
#include <cstdlib>
#include <memory>
#include <span>

namespace Texpainter
{
	namespace detail
	{
		struct CallFree
		{
			void operator()(void* buffer)
			{ free(buffer);}
		};
	}

	template<Trivial T>
	class DataBlock
	{
		public:
			using value_type = T;

			template<class U>
			using pointer_wrapper = std::unique_ptr<U, detail::CallFree>;

			explicit DataBlock(pointer_wrapper<T> ptr, size_t size):
				 m_size{size}
				,m_ptr{std::move(ptr)}
			{}

			explicit DataBlock(size_t n):
				m_size{n},
				m_ptr{reinterpret_cast<T*>(malloc(sizeof(T) * n))}
			{}

			auto size() const
			{ return m_size;}

			auto begin() const
			{ return m_ptr.get(); }

			auto end() const
			{ return begin() + size(); }

			auto begin()
			{ return m_ptr.get(); }

			auto end()
			{ return begin() + size(); }

		private:
			size_t m_size;
			pointer_wrapper<T> m_ptr;
	};


	template<class T, class OutputStream>
	void write(DataBlock<T> const& block, OutputStream stream)
	{
		write(static_cast<uint64_t>(block.size()), stream);
		write(std::span{std::ranges::data(block), std::ranges::size(block)}, stream);
	}

	template<class T, class InputStream>
	DataBlock<T> read(Empty<DataBlock<T>>, InputStream stream)
	{
		auto const n = read(Empty<uint64_t>(), stream);
		DataBlock<T> ret{n};
		read(std::span{std::ranges::data(ret), std::ranges::size(ret)}, stream);
		return ret;
	}
}

#endif