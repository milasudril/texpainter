//@	{
//@  "targets":[{"name":"hsi_rgb.test","type":"application", "autorun":1}]
//@	}

#include "./hsi_rgb.hpp"

#include <cassert>

namespace Testcases
{
	void texpainterHsiToRgb()
	{
		auto const red_hsi     = Texpainter::PixelStore::Hsi{0.0f, 1.0f, 1.0f / 3.0f, 1.0f};
		auto const yellow_hsi  = Texpainter::PixelStore::Hsi{1.0f / 6.0f, 1.0f, 1.0f / 3.0f, 1.0f};
		auto const green_hsi   = Texpainter::PixelStore::Hsi{2.0f / 6.0f, 1.0f, 1.0f / 3.0f, 1.0f};
		auto const cyan_hsi    = Texpainter::PixelStore::Hsi{3.0f / 6.0f, 1.0f, 1.0f / 3.0f, 1.0f};
		auto const blue_hsi    = Texpainter::PixelStore::Hsi{4.0f / 6.0f, 1.0f, 1.0f / 3.0f, 1.0f};
		auto const magenta_hsi = Texpainter::PixelStore::Hsi{5.0f / 6.0f, 1.0f, 1.0f / 3.0f, 1.0f};

		auto const red_rgb     = toRgb(red_hsi);
		auto const yellow_rgb  = toRgb(yellow_hsi);
		auto const green_rgb   = toRgb(green_hsi);
		auto const cyan_rgb    = toRgb(cyan_hsi);
		auto const blue_rgb    = toRgb(blue_hsi);
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
		auto const red_rgb     = Texpainter::PixelStore::RgbaValue{1.0f, 0.0f, 0.0f, 1.0f};
		auto const magenta_rgb = Texpainter::PixelStore::RgbaValue{0.5f, 0.0f, 0.5f, 1.0f};

		auto const red_hsi     = toHsi(red_rgb);
		auto const magenta_hsi = toHsi(magenta_rgb);


		assert(red_hsi.hue == 0.0f);
		assert(red_hsi.saturation == 1.0f);
		assert(red_hsi.intensity == 1.0f / 3.0f);

		assert(magenta_hsi.hue == 5.0f / 6.0f);
		assert(magenta_hsi.saturation == 1.0f);
		assert(magenta_hsi.intensity == 1.0f / 3.0f);
	}

	void texpainterHsiRgbHsi()
	{
		auto const val_hsi   = Texpainter::PixelStore::Hsi{0.25f, 1.0f, 1.0f / 3.0f, 1.0f};
		auto const val_rgb   = toRgb(val_hsi);
		auto const val_hsi_2 = toHsi(val_rgb);

		assert(std::abs(val_hsi.hue - val_hsi_2.hue) < 1e-5f);
		assert(val_hsi.saturation == val_hsi_2.saturation);
		assert(std::abs(val_hsi.intensity - val_hsi_2.intensity) < 1e-5);
	}

	void texpainterRgbHsiRgb()
	{
		auto const val_rgb   = Texpainter::PixelStore::RgbaValue{0.25f, 0.5f, 1.0f, 1.0f};
		auto const val_hsi   = toHsi(val_rgb);
		auto const val_rgb_2 = toRgb(val_hsi);
		assert(std::abs(val_rgb.red() - val_rgb_2.red()) < 1e-5f);
		assert(std::abs(val_rgb.green() - val_rgb_2.green()) < 1e-5f);
		assert(std::abs(val_rgb.blue() - val_rgb_2.blue() < 1e-5f));
	}
}

int main()
{
#if 0
	//	Testcases::texpainterHsiToRgb();
	//	Testcases::texpainterRgbToHsi();
#endif
	Testcases::texpainterHsiRgbHsi();
	Testcases::texpainterRgbHsiRgb();
	return 0;
}