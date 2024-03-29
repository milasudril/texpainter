//	{"targets":[{"name":"palette.test", "type":"application", "autorun":1}]}

#include "./palette.hpp"

#include <cassert>
#include <vector>
#include <functional>

namespace
{
	struct Buffer
	{
		std::vector<std::byte> m_data;
		uint32_t m_n_elems;
	};

	void write(uint32_t val, Buffer& buff) { buff.m_n_elems = val; }

	uint32_t read(Texpainter::Empty<uint32_t>, Buffer const& buff) { return buff.m_n_elems; }

	template<Texpainter::Trivial T>
	void write(std::span<T> data, Buffer& buff)
	{
		buff.m_data.resize(sizeof(T) * std::size(data));
		memcpy(static_cast<void*>(std::data(buff.m_data)),
		       std::data(data),
		       sizeof(T) * std::size(data));
	}

	template<Texpainter::Trivial T>
	void read(std::span<T> data, Buffer const& buff)
	{
		memcpy(static_cast<void*>(std::data(data)),
		       std::data(buff.m_data),
		       sizeof(T) * std::size(data));
	}
}

namespace Testcases
{
	void texpainterPaletteCreateFromDataBlock()
	{
		Texpainter::DataBlock<Texpainter::PixelStore::RgbaValue> dblock{3};
		*(std::data(dblock) + 0) = Texpainter::PixelStore::red();
		*(std::data(dblock) + 1) = Texpainter::PixelStore::green();
		*(std::data(dblock) + 2) = Texpainter::PixelStore::blue();

		Texpainter::PixelStore::Palette pal{std::move(dblock)};
		assert(std::size(pal) == 3);
		assert(pal[0].red() == 1.0f);
		assert(pal[0].green() == 0.0f);
		assert(pal[0].blue() == 0.0f);
		assert(pal[1].red() == 0.0f);
		assert(pal[1].green() == 1.0f);
		assert(pal[1].blue() == 0.0f);
		assert(pal[2].red() == 0.0f);
		assert(pal[2].green() == 0.0f);
		assert(pal[2].blue() == 1.0f);
	}

	void texpainterPaletteCreateEmpty()
	{
		Texpainter::PixelStore::Palette pal{3};
		assert(std::size(pal) == 3);
		assert(pal[0].red() == 0.0f);
		assert(pal[0].green() == 0.0f);
		assert(pal[0].blue() == 0.0f);
		assert(pal[0].alpha() == 1.0f);
		assert(pal[1].red() == 0.0f);
		assert(pal[1].green() == 0.0f);
		assert(pal[1].blue() == 0.0f);
		assert(pal[1].alpha() == 1.0f);
		assert(pal[2].red() == 0.0f);
		assert(pal[2].green() == 0.0f);
		assert(pal[2].blue() == 0.0f);
		assert(pal[2].alpha() == 1.0f);
	}

	void texpainterPaletteWriteRead()
	{
		Texpainter::PixelStore::Palette pal{2};
		pal[0] = Texpainter::PixelStore::red();
		pal[1] = Texpainter::PixelStore::green();

		Buffer buffer{};
		write(std::as_const(pal), std::ref(buffer));
		assert(buffer.m_n_elems == std::size(pal));

		auto pal_2 = read(Texpainter::Empty<decltype(pal)>{}, std::ref(buffer));

		assert(pal_2.size() == pal.size());
		assert(memcmp(std::begin(pal),
		              std::begin(pal),
		              std::size(pal) * sizeof(Texpainter::PixelStore::RgbaValue))
		       == 0);
	}
}

int main()
{
	Testcases::texpainterPaletteCreateFromDataBlock();
	Testcases::texpainterPaletteCreateEmpty();
	Testcases::texpainterPaletteWriteRead();
}