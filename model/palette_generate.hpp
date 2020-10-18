PixelStore::Palette pal{23};
std::ranges::for_each(palette_info.colors, [&pal, k = 0u](auto val) mutable {
	auto hsi = toHsi(val);
	for(int l = 0; l < 5; ++l)
	{
		pal[PixelStore::ColorIndex{k}] = toRgb(hsi);
		++k;
		hsi.intensity /= 2.0f;
	}
});
pal[PixelStore::ColorIndex{20}] = toRgb(PixelStore::Hsi{0.0f, 0.0f, 1.0f / 3.0, 1.0f});
pal[PixelStore::ColorIndex{21}] = toRgb(PixelStore::Hsi{0.0f, 0.0f, 1.0f / (128.0f * 3.0), 1.0f});
pal[PixelStore::ColorIndex{22}] = PixelStore::Pixel{0.0f, 0.0f, 0.0f, 0.0f};