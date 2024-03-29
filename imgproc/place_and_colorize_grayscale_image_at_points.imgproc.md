# Place and colorize grayscale image at points

This image processor places a RGBA image at location deterimed by the input point cloud.

## Input ports

__Locations:__ (Point cloud) The points where `image` should be placed

__Image:__ (Grayscale image) The image to place at the points

__Colormap:__ (RGBA image) A RGBA image to be used for color lookup. The color will be sampled from the location where the iamge should be placed.

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
			if(input<1>(args, col, row) > 0.0f) { return ImageCoordinates{col, row}; }
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
			if(input<1>(args, col, row) > 0.0f) { first_y.x = col; }
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
			if(input<1>(args, col - 1, row - 1) > 0.0f) { return ImageCoordinates{col, row}; }
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
			if(input<1>(args, col - 1, row - 1) > 0.0f) { last_y.x = col; }
		}
	}
	return last_y;
}

AxisAlignedBoundingBox findAABB(auto const& args)
{
	auto upper_left  = firstX(args, firstY(args));
	auto lower_right = lastX(args, lastY(args));

	return AxisAlignedBoundingBox::fromLimits(
	    vec2_t{static_cast<double>(upper_left.x), static_cast<double>(upper_left.y)},
	    vec2_t{static_cast<double>(lower_right.x), static_cast<double>(lower_right.y)});
}

void main(auto const& args)
{
	std::ranges::for_each(input<0>(args).get(), [&args, aabb = findAABB(args)](auto spot) {
		auto const w = args.canvasSize().width();
		auto const h = args.canvasSize().height();

		auto const O     = vec2_t{0.5 * w, 0.5 * h};
		auto const pos   = spot.loc;
		auto const color = input<2>(args, pos.x, pos.y);
		auto const v     = vec2_t{static_cast<double>(pos.x), static_cast<double>(pos.y)};

		auto const offset = v + O;
		auto const ϴ      = spot.rot;
		auto const s      = 1.0f / spot.scale;

		auto const rot_x = s * vec2_t{cos(ϴ), -sin(ϴ)};
		auto const rot_y = s * vec2_t{sin(ϴ), cos(ϴ)};

		auto const aabb_rot = rotate(aabb, ϴ, O);

		auto const min = Texpainter::clamp(lowerLimit(aabb_rot), vec2_t{0.0, 0.0}, 2 * O);
		auto const max = Texpainter::clamp(upperLimit(aabb_rot), vec2_t{0.0, 0.0}, 2 * O);

		auto x_min = static_cast<uint32_t>(min[0]);
		auto y_min = static_cast<uint32_t>(min[1]);
		auto x_max = static_cast<uint32_t>(max[0]);
		auto y_max = static_cast<uint32_t>(max[1]);

		for(uint32_t row = y_min; row < y_max; ++row)
		{
			for(uint32_t col = x_min; col < x_max; ++col)
			{
				auto const src_pos =
				    Texpainter::transform(vec2_t{static_cast<double>(col), static_cast<double>(row)}
				                              - O,
				                          rot_x,
				                          rot_y)
				    + O;

				if(src_pos[0] >= 0 && src_pos[0] < w && src_pos[1] >= 0 && src_pos[1] < h)
				{
					auto const src_x = static_cast<uint32_t>(src_pos[0]);
					auto const src_y = static_cast<uint32_t>(src_pos[1]);
					auto src         = input<1>(args, src_x, src_y) * color;
					auto const Δx    = static_cast<uint32_t>(offset[0]);
					auto const Δy    = static_cast<uint32_t>(offset[1]);
					auto& dest = output<0>(args, ((w + col) + Δx) % w, ((h + row) + Δy) % h);
					dest += src;
				}
			}
		}
	});
}
```

## Tags

__Id:__ 38cb4eca5bd70ecd2b5c3abbd043b0f7

__Category:__ Converters

__Release state:__ Stable