# Place RGBA image at points

This image processor places a RGBA image at location deterimed by the input point cloud.

## Input ports

__Locations:__ (Point cloud) The probability distribution.

__Image:__ (RGBA image) the image to place at the points

## Output ports

__Result:__ (RGBA image) The generated image

## Implementation

__Includes:__

```c++
#include "utils/rect.hpp"
```

__Source code:__

```c++
ImageCoordinates firstY(auto const& args)
{
	auto const w = args.canvasSize().width();
	auto const h = args.canvasSize().height();
	for(uint32_t row = 0; row < h; ++row)
	{
		for(uint32_t col = 0; col < w; ++col)
		{
			if(input<1>(args, col, row).alpha() > 0.0f) { return ImageCoordinates{col, row}; }
		}
	}
	return ImageCoordinates{w, h};
}

ImageCoordinates firstX(auto const& args, ImageCoordinates first_y)
{
	auto const h = args.canvasSize().height();
	for(uint32_t row = first_y.y; row < h; ++row)
	{
		for(uint32_t col = 0; col < first_y.x; ++col)
		{
			if(input<1>(args, col, row).alpha() > 0.0f) { first_y.x = col; }
		}
	}
	return first_y;
}

ImageCoordinates lastY(auto const& args)
{
	auto const w = args.canvasSize().width();
	auto const h = args.canvasSize().height();
	for(uint32_t row = h; row > 0; --row)
	{
		for(uint32_t col = w; col > 0; --col)
		{
			if(input<1>(args, col - 1, row - 1).alpha() > 0.0f)
			{ return ImageCoordinates{col, row}; }
		}
	}
	return ImageCoordinates{0, 0};
}

ImageCoordinates lastX(auto const& args, ImageCoordinates last_y)
{
	auto const w = args.canvasSize().width();
	for(uint32_t row = last_y.y; row > 0; --row)
	{
		for(uint32_t col = w; col > last_y.x; --col)
		{
			if(input<1>(args, col - 1, row - 1).alpha() > 0.0f) { last_y.x = col; }
		}
	}
	return last_y;
}

std::pair<vec2_t, vec2_t> findAABB(auto const& args)
{
	auto upper_left  = firstX(args, firstY(args));
	auto lower_right = lastX(args, lastY(args));
	return std::pair{vec2_t{static_cast<double>(upper_left.x), static_cast<double>(upper_left.y)},
		vec2_t{static_cast<double>(lower_right.x), static_cast<double>(lower_right.y)}};
}

void main(auto const& args)
{
	std::ranges::for_each(input<0>(args).get(), [&args, aabb = findAABB(args)](auto spot) {
		auto const w = args.canvasSize().width();
		auto const h = args.canvasSize().height();

		auto const O = vec2_t{0.5 * w, 0.5 * h};
		auto pos     = spot.loc;
		auto const v = vec2_t{static_cast<double>(pos.x), static_cast<double>(pos.y)};

		auto const offset = v + O;
		auto const ϴ = spot.rot;
		auto const s      = 1.0f / spot.scale;

		auto const rot_x    = s*vec2_t{cos(ϴ), -sin(ϴ)};
		auto const rot_y    = s*vec2_t{sin(ϴ), cos(ϴ)};
		/*
		auto const radius = 0.5*(aabb.second - aabb.first);
		auto rot_bb = Texpainter::axisAlignedBoundingBox(radius, ϴ);
		auto const min = -2.0*rot_bb + radius + aabb.first;
		auto const max = 2.0*rot_bb + radius + aabb.first;
*/

		for(uint32_t row = 0; row < h; ++row)
		{
			for(uint32_t col = 0; col < w; ++col)
			{
				auto const src_pos =
				    Texpainter::transform(vec2_t{static_cast<double>(col), static_cast<double>(row)} - O, rot_x, rot_y) + O;

				if(src_pos[0] >= 0 && src_pos[0] < w && src_pos[1] >= 0 && src_pos[1] < h)
				{
					auto const src_x = static_cast<uint32_t>(src_pos[0]);
					auto const src_y = static_cast<uint32_t>(src_pos[1]);
					auto src         = input<1>(args, src_x, src_y);
					auto const Δx    = static_cast<uint32_t>(offset[0]);
					auto const Δy    = static_cast<uint32_t>(offset[1]);
					auto& dest = output<0>(args, ((w + col) + Δx) % w, ((h + row) + Δy) % h);
					dest       = (1.0f - src.alpha()) * dest + src.alpha() * src;
				}
			}
		}
	});
}
```

## Tags

__Id:__ f872b1455c56665160d5faa3e0eae98c

__Category:__ Converters