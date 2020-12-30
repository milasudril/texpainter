//@	{"targets":[{"name":"image.test", "type":"application", "autorun":1}]}

#include "./image.hpp"

#include <cassert>
#include <vector>
#include <functional>
#include <cstring>

namespace Testcases
{
	void texpainterImageCreateFromDataBlock()
	{
		Texpainter::PixelStore::Image img{3u, 2u};
		assert(img.width() == 3);
		assert(img.height() == 2);
		assert(area(img) == 6);

		auto ptr   = img.pixels().begin();
		*(ptr + 0) = Texpainter::PixelStore::red();
		*(ptr + 1) = Texpainter::PixelStore::green();
		*(ptr + 2) = Texpainter::PixelStore::blue();
		*(ptr + 3) = Texpainter::PixelStore::cyan();
		*(ptr + 4) = Texpainter::PixelStore::magenta();
		*(ptr + 5) = Texpainter::PixelStore::yellow();

		assert(distanceSquared(img(0, 0), Texpainter::PixelStore::red()) == 0.0f);
		assert(distanceSquared(img(1, 0), Texpainter::PixelStore::green()) == 0.0f);
		assert(distanceSquared(img(2, 0), Texpainter::PixelStore::blue()) == 0.0f);
		assert(distanceSquared(img(0, 1), Texpainter::PixelStore::cyan()) == 0.0f);
		assert(distanceSquared(img(1, 1), Texpainter::PixelStore::magenta()) == 0.0f);
		assert(distanceSquared(img(2, 1), Texpainter::PixelStore::yellow()) == 0.0f);
	}
	void texpainterImageCreateEmpty()
	{
		Texpainter::PixelStore::Image img{3u, 2u};
		assert(img.width() == 3);
		assert(img.height() == 2);
	}
}

int main()
{
	Testcases::texpainterImageCreateFromDataBlock();
	Testcases::texpainterImageCreateEmpty();
}