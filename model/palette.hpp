//@	{"targets":[{"name":"palette.hpp", "type":"include"}]}

#ifndef TEXPAINTER_PALETTE_HPP
#define TEXPAINTER_PALETTE_HPP

#include "./pixel.hpp"
#include "utils/datablock.hpp"

#include <algorithm>

namespace Texpainter::Model
{
	class Palette
	{
	public:
		auto begin() const
		{
			return std::begin(m_block);
		}

		auto end() const
		{
			return std::end(m_block);
		}

		auto size() const
		{
			return std::size(m_block);
		}

		explicit Palette(DataBlock<Pixel>&& block): m_block{std::move(block)}
		{
		}

		explicit Palette(std::span<Pixel const> src): m_block{static_cast<uint32_t>(std::size(src))}
		{
			std::copy_n(std::begin(src), std::size(m_block), std::begin(m_block));
		}


		explicit Palette(uint32_t n, Pixel color_init = black()): m_block{n}
		{
			std::ranges::fill(m_block, color_init);
		}

		auto operator[](size_t index) const
		{
			return *(begin() + index);
		}

		auto& operator[](size_t index)
		{
			return *(std::begin(m_block) + index);
		}

		void resize(uint32_t size_new)
		{
			auto offset_end = size();
			Texpainter::resize(m_block, size_new);
			std::fill(begin() + offset_end, end(), black());
		}

		operator DataBlock<Pixel> const&() const
		{
			return m_block;
		}

	private:
		DataBlock<Pixel> m_block;
	};

	inline void resize(Palette& p, uint32_t size_new)
	{
		p.resize(size_new);
	}

	template<class OutputStream>
	void write(Palette const& pal, OutputStream stream)
	{
		write(static_cast<Texpainter::DataBlock<Texpainter::Model::Pixel> const&>(pal), stream);
	}

	template<class InputStream>
	Palette read(Empty<Palette>, InputStream stream)
	{
		return Palette{read(Empty<DataBlock<Pixel>>{}, stream)};
	}
}

#endif