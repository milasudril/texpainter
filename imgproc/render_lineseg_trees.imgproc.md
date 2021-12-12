# Render line segment trees

Foo

## Input ports

__Input:__ (Line segment tree)

## Output ports

__Output:__ (Grayscale image)

## Parameters

__Line width:__ (= 0.5)

__Growth rate:__ (= 0.5)

__Trunk split:__ (= 0.0)

## Implementation

__Includes:__ 

```c++
#include <algorithm>
#include <chrono>
#include <ranges>
```

__Source code:__ 

```c++
[[nodiscard]] inline double draw(
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

enum class Direction : int
{
	Forward,
	Backward
};

[[nodiscard]] inline double draw(std::span<std::pair<vec2_t, LineSegTree> const> points,
                                 auto const& args,
                                 double line_width,
                                 double growth_rate,
                                 Direction dir)
{
	if(std::size(points) == 0) { return line_width; }

	auto range = std::span{std::begin(points) + 1, std::end(points)};

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
	     growth_factor = std::exp2((dir == Direction::Forward ? growth_rate : -growth_rate)
	                               / length)](auto const& item) mutable {
		    line_width = draw(item.first, start, args, line_width, growth_factor);
		    start      = item.first;
	    });

	std::ranges::for_each(points, [args, line_width, growth_rate](auto const& item) {
		(void)draw(item.second.data, args, line_width, growth_rate, Direction::Forward);
	});

	return line_width;
}

void main(auto const& args, auto const& params)
{
	auto const line_width = sizeFromMin(args.canvasSize(), param<Str{"Line width"}>(params)) / 32.0;
	auto const growth_rate = std::lerp(-4.0f, 4.0f, param<Str{"Growth rate"}>(params).value());

	std::ranges::for_each(
	    input<0>(args).get(),
	    [&args, line_width, growth_rate, trunk_midpoint = param<Str{"Trunk split"}>(params)](
	        auto const& item) {
		    auto const split_at = static_cast<size_t>(static_cast<float>(std::size(item.data))
		                                              * trunk_midpoint.value());
		    if(split_at == 0)
		    { (void)draw(item.data, args, line_width, growth_rate, Direction::Forward); }
		    else if(split_at == std::size(item.data))
		    {
			    (void)draw(item.data, args, line_width, growth_rate, Direction::Backward);
		    }
		    else
		    {
			    auto const res = draw(std::span{std::begin(item.data), split_at},
			                          args,
			                          line_width,
			                          growth_rate,
			                          Direction::Backward);
			    (void)draw(std::span{std::begin(item.data) + (split_at - 1), std::end(item.data)},
			               args,
			               res,
			               growth_rate,
			               Direction::Forward);
		    }
	    });
}
```

## Tags

__Id:__ 916d9f54c7d922f50752901b95e0c22c

__Category:__ Converters

__Release state:__ Experimental