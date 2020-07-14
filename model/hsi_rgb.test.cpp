//@	{
//@  "targets":[{"name":"hsi_rgb.test","type":"application", "autorun":1}]
//@	}

#include "./hsi_rgb.hpp"

#include <cassert>

namespace Testcases
{
	void texpainterHsiToRgb()
	{
		auto const red_hsi = Texpainter::Model::Hsi{0.0f, 1.0f, 1.0f / 3.0f, 1.0f};
		auto const yellow_hsi = Texpainter::Model::Hsi{1.0f / 6.0f, 1.0f, 1.0f / 3.0f, 1.0f};
		auto const green_hsi = Texpainter::Model::Hsi{2.0f / 6.0f, 1.0f, 1.0f / 3.0f, 1.0f};
		auto const cyan_hsi = Texpainter::Model::Hsi{3.0f / 6.0f, 1.0f, 1.0f / 3.0f, 1.0f};
		auto const blue_hsi = Texpainter::Model::Hsi{4.0f / 6.0f, 1.0f, 1.0f / 3.0f, 1.0f};
		auto const magenta_hsi = Texpainter::Model::Hsi{5.0f / 6.0f, 1.0f, 1.0f / 3.0f, 1.0f};

		auto const red_rgb = toRgb(red_hsi);
		auto const yellow_rgb = toRgb(yellow_hsi);
		auto const green_rgb = toRgb(green_hsi);
		auto const cyan_rgb = toRgb(cyan_hsi);
		auto const blue_rgb = toRgb(blue_hsi);
		auto const magenta_rgb = toRgb(magenta_hsi);

		assert(red_rgb.red() == 1.0f);
		assert(red_rgb.green() == 0.0f);
		assert(red_rgb.blue() == 0.0f);

		assert(yellow_rgb.red() == 0.5f);
		assert(yellow_rgb.green() == 0.5f);
		assert(yellow_rgb.blue() == 0.0f);

		assert(green_rgb.red() == 0.0f);
		assert(green_rgb.green() == 1.0f);
		assert(green_rgb.blue() == 0.0f);

		assert(cyan_rgb.red() == 0.0f);
		assert(cyan_rgb.green() == 0.5f);
		assert(cyan_rgb.blue() == 0.5f);

		assert(blue_rgb.red() == 0.0f);
		assert(blue_rgb.green() == 0.0f);
		assert(blue_rgb.blue() == 1.0f);

		assert(magenta_rgb.red() == 0.5f);
		assert(magenta_rgb.green() == 0.0f);
		assert(magenta_rgb.blue() == 0.5f);
	}

	void texpainterRgbToHsi()
	{
		auto const red_rgb = Texpainter::Model::Pixel{1.0f, 0.0f, 0.0f, 1.0f};
		auto const magenta_rgb = Texpainter::Model::Pixel{0.5f, 0.0f, 0.5f, 1.0f};

		auto const red_hsi = toHsi(red_rgb);
		auto const magenta_hsi = toHsi(magenta_rgb);


		assert(red_hsi.hue == 0.0f);
		assert(red_hsi.saturation == 1.0f);
		assert(red_hsi.intensity == 1.0f / 3.0f);

		assert(magenta_hsi.hue == 5.0f / 6.0f);
		assert(magenta_hsi.saturation == 1.0f);
		assert(magenta_hsi.intensity == 1.0f / 3.0f);
	}
}

int main()
{
	Testcases::texpainterHsiToRgb();
	Testcases::texpainterRgbToHsi();
	return 0;
	Testcases::texpainterHsiToRgb();
	return 0;
}