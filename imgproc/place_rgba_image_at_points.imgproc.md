# Place RGBA image at points

This image processor places a RGBA image at location deterimed by the input point cloud.

## Input ports

__Locations:__ (Point cloud) The probability distribution.

__Image:__ (RGBA image) the image to place at the points

## Output ports

__Result:__ (RGBA image) The generated image

## Implementation

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

std::pair<ImageCoordinates, ImageCoordinates> findAABB(auto const& args)
{
	auto upper_left  = firstX(args, firstY(args));
	auto lower_right = lastX(args, lastY(args));
	return std::pair{upper_left, lower_right};
}

void main(auto const& args)
{
	std::ranges::for_each(input<0>(args).get(), [&args, aabb = findAABB(args)](auto pos) {
		auto const w = args.canvasSize().width();
		auto const h = args.canvasSize().height();

		auto const Δx = pos.x + w / 2;
		auto const Δy = pos.y + h / 2;

		for(uint32_t row = aabb.first.y; row < aabb.second.y; ++row)
		{
			for(uint32_t col = aabb.first.x; col < aabb.second.x; ++col)
			{
				auto src   = input<1>(args, col, row);
				auto& dest = output<0>(args, ((w + col) + Δx) % w, ((h + row) + Δy) % h);
				dest       = (1.0f - src.alpha()) * dest + src.alpha() * src;
			}
		}
	});
}
```

## Tags

__Id:__ f872b1455c56665160d5faa3e0eae98c

__Category:__ Converters