//@	{"targets":[{"name":"image.test", "type":"application", "autorun":1}]}

#include "./image.hpp"

#include <cassert>
#include <vector>
#include <functional>
#include <cstring>

namespace
{
	struct Buffer
	{
		std::vector<std::byte> m_data;
		uint32_t m_width{};
		uint32_t m_height{};
	};

	void write(uint32_t val, Buffer& buff)
	{
		if(buff.m_width == 0) { buff.m_width = val; }
		else
		{
			buff.m_height = val;
		}
	}

	uint32_t read(Texpainter::Empty<uint32_t>, Buffer& buff)
	{
		if(buff.m_width != 0)
		{
			auto ret     = buff.m_width;
			buff.m_width = 0;
			return ret;
		}
		return buff.m_height;
	}

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
	void texpainterImageCreateFromDataBlock()
	{
		Texpainter::Model::Image img{3u, 2u};
		assert(img.width() == 3);
		assert(img.height() == 2);
		assert(img.area() == 6);

		auto ptr   = img.pixels().begin();
		*(ptr + 0) = Texpainter::Model::red();
		*(ptr + 1) = Texpainter::Model::green();
		*(ptr + 2) = Texpainter::Model::blue();
		*(ptr + 3) = Texpainter::Model::cyan();
		*(ptr + 4) = Texpainter::Model::magenta();
		*(ptr + 5) = Texpainter::Model::yellow();

		assert(distanceSquared(img(0, 0), Texpainter::Model::red()) == 0.0f);
		assert(distanceSquared(img(1, 0), Texpainter::Model::green()) == 0.0f);
		assert(distanceSquared(img(2, 0), Texpainter::Model::blue()) == 0.0f);
		assert(distanceSquared(img(0, 1), Texpainter::Model::cyan()) == 0.0f);
		assert(distanceSquared(img(1, 1), Texpainter::Model::magenta()) == 0.0f);
		assert(distanceSquared(img(2, 1), Texpainter::Model::yellow()) == 0.0f);
	}
	void texpainterImageCreateEmpty()
	{
		Texpainter::Model::Image img{3u, 2u};
		assert(img.width() == 3);
		assert(img.height() == 2);
	}


	void texpainterImageWriteRead()
	{
		Texpainter::Model::Image img{3u, 2u};

		auto ptr   = img.pixels().begin();
		*(ptr + 0) = Texpainter::Model::red();
		*(ptr + 1) = Texpainter::Model::green();
		*(ptr + 2) = Texpainter::Model::blue();
		*(ptr + 3) = Texpainter::Model::cyan();
		*(ptr + 4) = Texpainter::Model::magenta();
		*(ptr + 5) = Texpainter::Model::yellow();

		Buffer buffer{};
		write(std::as_const(img), std::ref(buffer));
		assert(buffer.m_width == 3);
		assert(buffer.m_height == 2);

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