//@	{"targets":[{"name":"image.test", "type":"application", "autorun":1}]}

#include "./image.hpp"

#include <cassert>
#include <vector>
#include <functional>

namespace
{
	struct Buffer
	{
		std::vector<std::byte> m_data;
		uint32_t m_width{};
		uint32_t m_n_elems{};
	};

	void write(uint32_t val, Buffer& buff)
	{
		if(buff.m_width == 0) { buff.m_width = val; }
		else
		{
			buff.m_n_elems = val;
		}
	}

	uint32_t read(Texpainter::Empty<uint32_t>, Buffer& buff)
	{
		if(buff.m_width != 0)
		{
			auto ret = buff.m_width;
			buff.m_width = 0;
			return ret;
		}
		return buff.m_n_elems;
	}

	template<Texpainter::Trivial T>
	void write(std::span<T> data, Buffer& buff)
	{
		buff.m_data.resize(sizeof(T) * std::size(data));
		memcpy(static_cast<void*>(std::data(buff.m_data)), std::data(data), sizeof(T) * std::size(data));
	}

	template<Texpainter::Trivial T>
	void read(std::span<T> data, Buffer const& buff)
	{
		memcpy(static_cast<void*>(std::data(data)), std::data(buff.m_data), sizeof(T) * std::size(data));
	}
}

namespace Testcases
{
	void texpainterImageCreateFromDataBlock()
	{
		Texpainter::DataBlock<Texpainter::Model::Pixel> dblock{6};
		*(std::data(dblock) + 0) = Texpainter::Model::red();
		*(std::data(dblock) + 1) = Texpainter::Model::green();
		*(std::data(dblock) + 2) = Texpainter::Model::blue();
		*(std::data(dblock) + 3) = Texpainter::Model::cyan();
		*(std::data(dblock) + 4) = Texpainter::Model::magenta();
		*(std::data(dblock) + 5) = Texpainter::Model::yellow();

		Texpainter::Model::Image img{3u, std::move(dblock)};
		assert(img.width() == 3);
		assert(img.height() == 2);

		assert(distanceSquared(img.get(0, 0), Texpainter::Model::red()) == 0.0f);
		assert(distanceSquared(img.get(1, 0), Texpainter::Model::green()) == 0.0f);
		assert(distanceSquared(img.get(2, 0), Texpainter::Model::blue()) == 0.0f);
		assert(distanceSquared(img.get(0, 1), Texpainter::Model::cyan()) == 0.0f);
		assert(distanceSquared(img.get(1, 1), Texpainter::Model::magenta()) == 0.0f);
		assert(distanceSquared(img.get(2, 1), Texpainter::Model::yellow()) == 0.0f);
	}
	void texpainterImageCreateEmpty()
	{
		Texpainter::Model::Image img{3u, 2u};
		assert(img.width() == 3);
		assert(img.height() == 2);
	}


	void texpainterImageWriteRead()
	{
		Texpainter::DataBlock<Texpainter::Model::Pixel> dblock{6};
		*(std::data(dblock) + 0) = Texpainter::Model::red();
		*(std::data(dblock) + 1) = Texpainter::Model::green();
		*(std::data(dblock) + 2) = Texpainter::Model::blue();
		*(std::data(dblock) + 3) = Texpainter::Model::cyan();
		*(std::data(dblock) + 4) = Texpainter::Model::magenta();
		*(std::data(dblock) + 5) = Texpainter::Model::yellow();

		Texpainter::Model::Image img{3u, std::move(dblock)};

		Buffer buffer{};
		write(std::as_const(img), std::ref(buffer));
		assert(buffer.m_n_elems == 6);
		assert(buffer.m_width == 3);

		auto img_2 = read(Texpainter::Empty<decltype(img)>{}, std::ref(buffer));

		assert(img_2.width() == img.width());
		assert(img_2.height() == img.height());
	}
}

int main()
{
	Testcases::texpainterImageCreateFromDataBlock();
	Testcases::texpainterImageCreateEmpty();
	Testcases::texpainterImageWriteRead();
}