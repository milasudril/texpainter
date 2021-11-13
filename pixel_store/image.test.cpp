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


	void texpainterImageCropAllZeros()
	{
		constexpr std::array<float, 48> data{
		    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};


		Texpainter::PixelStore::Image const img{Texpainter::Span2d{data.data(), 8, 6}};
		auto img_cropped = Texpainter::PixelStore::crop(
		    img.pixels(), boundingBox(img.pixels(), [](auto val) { return val > 0.0f; }));
		assert(img_cropped.width() == 1);
		assert(img_cropped.height() == 1);
		assert(img_cropped(0, 0) == 0.0f);
	}

	void texpainterImageCropDiagUp()
	{
		// clang-format off
		constexpr std::array<float, 48> data{
		    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		    0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		    0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
		};
		// clang-format on

		Texpainter::PixelStore::Image const img{Texpainter::Span2d{data.data(), 8, 6}};
		auto img_cropped = Texpainter::PixelStore::crop(
		    img.pixels(), boundingBox(img.pixels(), [](auto val) { return val > 0.0f; }));

		assert(img_cropped.width() == 5);
		assert(img_cropped.height() == 4);

		// clang-format off
		constexpr std::array<float, 20> result{
		    0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		    0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		    0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		    1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		};
		// clang-format on

		assert(std::equal(
		    img_cropped.pixels().data(), img_cropped.pixels().data() + 20, std::data(result)));
	}

	void texpainterImageCropDiagUpPadding()
	{
		// clang-format off
		constexpr std::array<float, 48> data{
		    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		    0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		    0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
		};
		// clang-format on

		Texpainter::PixelStore::Image const img{Texpainter::Span2d{data.data(), 8, 6}};
		auto img_cropped = Texpainter::PixelStore::crop(
		    img.pixels(), boundingBox(img.pixels(), [](auto val) { return val > 0.0f; }), 2, 1);

		assert(img_cropped.width() == 7);
		assert(img_cropped.height() == 5);

		// clang-format off
		constexpr std::array<float, 35> result{
		    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		    0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		    0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
		};
		// clang-format on

		assert(std::equal(
		    img_cropped.pixels().data(), img_cropped.pixels().data() + 35, std::data(result)));
	}
}

int main()
{
	Testcases::texpainterImageCreateFromDataBlock();
	Testcases::texpainterImageCropAllZeros();
	Testcases::texpainterImageCropDiagUp();
	return 0;
}
