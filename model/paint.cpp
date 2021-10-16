//@	{"targets":[{"name":"paint.o","type":"object"}]}

#include "./paint.hpp"

#include <stack>

void Texpainter::Model::paint(Span2d<PixelStore::RgbaValue> pixels,
                              vec2_t origin,
                              double radius,
                              BrushFunction brush,
                              PixelStore::RgbaValue color)
{
	auto const w = pixels.width();
	auto const h = pixels.height();
	origin += vec2_t{0.5, 0.5};

	auto const r_vec        = vec2_t{0.5, 0.5} + vec2_t{radius, radius};
	auto const begin_coords = origin - r_vec;
	auto const end_coords   = origin + r_vec;

	for(uint32_t row = static_cast<uint32_t>(begin_coords[1]);
	    row != static_cast<uint32_t>(end_coords[1]) + 1u;
	    ++row)
	{
		for(uint32_t col = static_cast<uint32_t>(begin_coords[0]);
		    col != static_cast<uint32_t>(end_coords[0]) + 1u;
		    ++col)
		{
			auto const loc_ret = vec2_t{static_cast<double>(col), static_cast<double>(row)};
			auto d             = loc_ret - origin;
			if(brush(d, radius)) { pixels((col + w) % w, (row + h) % h) = color; }
		}
	}
}

void Texpainter::Model::floodfill(Span2d<PixelStore::RgbaValue> pixels,
                                  vec2_t origin,
                                  PixelStore::RgbaValue color)
{
	auto const w   = pixels.width();
	auto const h   = pixels.height();
	auto const tol = 1e-7;
	auto init      = std::pair{static_cast<uint32_t>(origin[0]), static_cast<uint32_t>(origin[1])};

	auto start_color = pixels((init.first + w) % w, (init.second + h) % h);
	if(distanceSquared(color, start_color) < tol) { return; }

	std::stack<std::pair<uint32_t, uint32_t>> locations;
	locations.push(init);
	while(!locations.empty())
	{
		auto loc = locations.top();
		locations.pop();

		if(distanceSquared(pixels((loc.first + w) % w, (loc.second + h) % h), color) < 1e-7)
		{ continue; }

		pixels((loc.first + w) % w, (loc.second + h) % h) = color;

		if(distanceSquared(pixels((loc.first + w + 1) % w, (loc.second + h) % h), start_color)
		   < tol)
		{ locations.push(std::pair{(loc.first + w + 1) % w, (loc.second + h) % h}); }

		if(distanceSquared(pixels((loc.first + w - 1) % w, (loc.second + h) % h), start_color)
		   < tol)
		{ locations.push(std::pair{(loc.first + w - 1) % w, (loc.second + h) % h}); }

		if(distanceSquared(pixels((loc.first + w) % w, (loc.second + h + 1) % h), start_color)
		   < tol)
		{ locations.push(std::pair{(loc.first + w) % w, (loc.second + h + 1) % h}); }

		if(distanceSquared(pixels((loc.first + w) % w, (loc.second + h - 1) % h), start_color)
		   < tol)
		{ locations.push(std::pair{(loc.first + w) % w, (loc.second + h - 1) % h}); }
	}
}

void Texpainter::Model::drawOutline(Span2d<PixelStore::RgbaValue> pixels)
{
	auto const w = pixels.width();
	auto const h = pixels.height();

	constexpr auto dark  = PixelStore::RgbaValue{0.0f, 0.0f, 0.0f, 1.0f};
	constexpr auto light = PixelStore::RgbaValue{1.0f, 1.0f, 1.0f, 1.0f};

	for(uint32_t k = 0; k < w; ++k)
	{
		auto src     = pixels(k, 0);
		pixels(k, 0) = intensity(src) < 0.5f ? dark : light;
	}

	for(uint32_t k = 0; k < w; ++k)
	{
		auto src         = pixels(k, h - 1);
		pixels(k, h - 1) = intensity(src) < 0.5f ? dark : light;
	}

	for(uint32_t k = 0; k < h; ++k)
	{
		auto src     = pixels(0, k);
		pixels(0, k) = intensity(src) < 0.5f ? dark : light;
	}

	for(uint32_t k = 0; k < h; ++k)
	{
		auto src         = pixels(w - 1, k);
		pixels(w - 1, k) = intensity(src) < 0.5f ? dark : light;
	}

	if(w >= 8 && h >= 8)
	{
		for(int k = -1; k <= 1; ++k)
		{
			for(int l = -1; l <= 1; ++l)
			{
				auto src                     = pixels(w / 2 + l, h / 2 + k);
				pixels(w / 2 + l, h / 2 + k) = intensity(src) < 0.5f ? dark : light;
			}
		}
	}
}
