//@	{"targets":[{"name":"paint.o","type":"object"}]}

#include "./paint.hpp"

#include <stack>

void Texpainter::Model::paint(Span2d<PixelStore::Pixel> pixels,
                              vec2_t origin,
                              double radius,
                              BrushFunction brush,
                              PixelStore::Pixel color)
{
	auto const w = pixels.width();
	auto const h = pixels.height();

	auto const r_vec        = vec2_t{radius, radius};
	auto const begin_coords = origin - r_vec;
	auto const end_coords   = origin + r_vec;

	for(int row = static_cast<int>(begin_coords[1]); row <= static_cast<int>(end_coords[1]); ++row)
	{
		for(int col = static_cast<int>(begin_coords[0]); col <= static_cast<int>(end_coords[0]);
		    ++col)
		{
			auto const loc_ret = vec2_t{static_cast<double>(col), static_cast<double>(row)};
			auto d             = loc_ret - origin;
			if(brush(d, radius)) { pixels((col + w) % w, (row + h) % h) = color; }
		}
	}
}

void Texpainter::Model::floodfill(Span2d<PixelStore::Pixel> pixels,
                                  vec2_t origin,
                                  PixelStore::Pixel color)
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
