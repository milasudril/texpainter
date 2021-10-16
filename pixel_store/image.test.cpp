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
		Texpainter::PixelStore::Image<int> img{3u, 2u};
		assert(img.width() == 3);
		assert(img.height() == 2);
		assert(area(img) == 6);

		auto ptr   = img.pixels().begin();
		*(ptr + 0) = 0;
		*(ptr + 1) = 1;
		*(ptr + 2) = 2;
		*(ptr + 3) = 3;
		*(ptr + 4) = 4;
		*(ptr + 5) = 5;

		assert(img(0, 0) == 0);
		assert(img(1, 0) == 1);
		assert(img(2, 0) == 2);
		assert(img(0, 1) == 3);
		assert(img(1, 1) == 4);
		assert(img(2, 1) == 5);
	}
}

int main() { Testcases::texpainterImageCreateFromDataBlock(); }