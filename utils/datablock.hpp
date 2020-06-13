//@	{"targets":[{"name":"datablock.hpp", "type":"include"}]}

#ifndef TEXPAINTER_DATABLOCK_HPP
#define TEXPAINTER_DATABLOCK_HPP

#include "./trivial.hpp"
#include "./empty.hpp"

#include <type_traits>
#include <cstdlib>
#include <memory>
#include <span>
#include <cstring>

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

			explicit DataBlock(pointer_wrapper<T> ptr, uint32_t size):
				 m_size{size}
				,m_ptr{std::move(ptr)}
			{}

			explicit DataBlock(uint32_t n):
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

			auto data()
			{ return begin(); }

			auto data() const
			{ return begin(); }

		private:
			uint32_t m_size;
			pointer_wrapper<T> m_ptr;
	};


	template<class T, class OutputStream>
	void write(DataBlock<T> const& block, OutputStream stream)
	{
		write(block.size(), stream);
		write(std::span{std::ranges::data(block), std::ranges::size(block)}, stream);
	}

	template<class T, class InputStream>
	DataBlock<T> read(Empty<DataBlock<T>>, InputStream stream)
	{
		auto const n = read(Empty<uint32_t>(), stream);
		DataBlock<T> ret{n};
		read(std::span{std::ranges::data(ret), std::ranges::size(ret)}, stream);
		return ret;
	}

	template<class T>
	void resize(DataBlock<T>& block, uint32_t size_new)
	{
		static_assert(std::is_trivial_v<T>);
		if(size_new > std::size(block))
		{
			DataBlock<T> block_new{size_new};
			memcpy(std::data(block_new), std::data(block), sizeof(T)*std::size(block));
			block = std::move(block_new);
		}
	}
}

#endif