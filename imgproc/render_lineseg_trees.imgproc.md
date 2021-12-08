# Render line segment trees

Foo

## Input ports

__Input:__ (Line segment tree)

## Output ports

__Output:__ (Grayscale image)

## Parameters

__Line width:__ (= 0.5)

__Growth rate:__ (= 0.5)

## Implementation

__Includes:__ 

```c++
#include <algorithm>
#include <chrono>
```

__Source code:__ 

```c++
[[nodiscard]] double draw(
    vec2_t to, vec2_t from, auto const& args, double line_width, double growth_factor)
{
	auto const dir  = to - from;
	auto const l    = Texpainter::length(dir);
	auto const v    = dir / l;
	auto const n    = vec2_t{-v[1], v[0]};
	auto const size = args.canvasSize();

	for(int t = 0; t < static_cast<int>(l + 0.5); ++t)
	{
		for(int s = -static_cast<int>(line_width); s < static_cast<int>(line_width + 0.5); ++s)
		{
			auto const pos = from + static_cast<double>(t) * v + static_cast<double>(s) * 0.5 * n;
			auto pos_x     = static_cast<int>(pos[0]);
			auto pos_y     = static_cast<int>(pos[1]);
			pos_x          = (pos_x + size.width()) % size.width();
			pos_y          = (pos_y + size.height()) % size.height();
			output<0>(args, pos_x, pos_y) = 1.0;
		}
		line_width *= growth_factor;
	}
	return line_width;
}

void draw(std::vector<std::pair<vec2_t, LineSegTree>> const& points,
          auto const& args,
          double line_width,
          double growth_rate)
{
	if(std::size(points) == 0) { return; }

	auto range = std::span{std::data(points) + 1, std::size(points) - 1};

	auto length = 0.0;
	std::ranges::for_each(range,
	                      [start = points.front().first, &sum = length](auto const& item) mutable {
		                      sum += Texpainter::length(item.first - start);
		                      start = item.first;
	                      });

	std::ranges::for_each(
	    range,
	    [start = points.front().first,
	     &args,
	     &line_width,
	     growth_factor = std::exp2(growth_rate / length)](auto const& item) mutable {
		    line_width = draw(item.first, start, args, line_width, growth_factor);
		    start      = item.first;
	    });
	std::ranges::for_each(range, [args, line_width, growth_rate](auto const& item) {
		draw(item.second.data, args, line_width, growth_rate);
	});
}

void main(auto const& args, auto const& params)
{
	auto const line_width = sizeFromMin(args.canvasSize(), param<Str{"Line width"}>(params)) / 32.0;
	auto const grow_rate  = std::lerp(-4.0f, 4.0f, param<Str{"Growth rate"}>(params).value());

	auto now = std::chrono::steady_clock::now();
	std::ranges::for_each(input<0>(args).get(), [&args, line_width, grow_rate](auto const& item) {
		draw(item.data, args, line_width, grow_rate);
	});
	printf("RenderTrees: %zu\n", (std::chrono::steady_clock::now() - now).count());
}
```

## Tags

__Id:__ 916d9f54c7d922f50752901b95e0c22c

__Category:__ Converters

__Release state:__ Experimental