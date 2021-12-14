# Render line segment trees

Renders line segment trees as a grayscale image

## Input ports

__Input:__ (Line segment tree)

## Output ports

__Output:__ (Grayscale image)

## Parameters

__Line width:__ (= 0.5) The initial line thickness

__Line width growth rate:__ (= 0.5)

__Intensity:__ (= 1.0) The initial intensity

__Intensity growth rate:__ (= 0.5)

__Trunk split point:__ (= 0.0) The point where to split the trunk. The growth rate will be inverted for all vertices that are "to the left" of the trunk split point.

## Implementation

__Includes:__ 

```c++
#include <algorithm>
#include <chrono>
```

__Source code:__ 

```c++
struct ModulationState
{
	double line_width;
	double intensity;
};

struct ModulationParams
{
	double width_growth;
	double intensity_growth;
};

[[nodiscard]] inline ModulationState draw(vec2_t to,
                                          vec2_t from,
                                          auto const& args,
                                          ModulationState mod_state,
                                          ModulationParams mod_params)
{
	auto const dir  = to - from;
	auto const l    = Texpainter::length(dir);
	auto const v    = dir / l;
	auto const n    = vec2_t{-v[1], v[0]};
	auto const size = args.canvasSize();

	for(int t = 0; t < static_cast<int>(l + 0.5); ++t)
	{
		for(int s = -static_cast<int>(mod_state.line_width);
		    s < static_cast<int>(mod_state.line_width + 0.5);
		    ++s)
		{
			auto const pos = from + static_cast<double>(t) * v + static_cast<double>(s) * 0.5 * n;
			auto pos_x     = static_cast<int>(pos[0]);
			auto pos_y     = static_cast<int>(pos[1]);
			pos_x          = (pos_x + size.width()) % size.width();
			pos_y          = (pos_y + size.height()) % size.height();
			output<0>(args, pos_x, pos_y) = static_cast<float>(mod_state.intensity);
		}
		mod_state.line_width *= mod_params.width_growth;
		mod_state.intensity *= mod_params.intensity_growth;
	}
	return mod_state;
}

enum class Direction : int
{
	Forward,
	Backward
};

[[nodiscard]] inline ModulationState draw(std::span<std::pair<vec2_t, LineSegTree> const> points,
                                          auto const& args,
                                          ModulationState mod_state,
                                          ModulationParams mod_params,
                                          Direction dir)
{
	if(std::size(points) == 0) { return mod_state; }

	auto range = std::span{std::begin(points) + 1, std::end(points)};

	auto length = 0.0;
	std::ranges::for_each(range,
	                      [start = points.front().first, &sum = length](auto const& item) mutable {
		                      sum += Texpainter::length(item.first - start);
		                      start = item.first;
	                      });

	auto const growth_factors_singed =
	    dir == Direction::Forward
	        ? ModulationParams{mod_params.width_growth, mod_params.intensity_growth}
	        : ModulationParams{-mod_params.width_growth, -mod_params.intensity_growth};

	std::ranges::for_each(
	    range,
	    [start = points.front().first,
	     &args,
	     &mod_state,
	     growth_factor = ModulationParams{std::exp2(growth_factors_singed.width_growth / length),
	                                      std::exp2(growth_factors_singed.intensity_growth
	                                                / length)}](auto const& item) mutable {
		    mod_state = draw(item.first, start, args, mod_state, growth_factor);
		    start     = item.first;
	    });

	std::ranges::for_each(points, [args, mod_state, mod_params](auto const& item) {
		(void)draw(item.second.data, args, mod_state, mod_params, Direction::Forward);
	});

	return mod_state;
}

void main(auto const& args, auto const& params)
{
	ModulationParams const mod_params{
	    std::lerp(-4.0f, 4.0f, param<Str{"Line width growth rate"}>(params).value()),
	    std::lerp(-4.0f, 4.0f, param<Str{"Intensity growth rate"}>(params).value())};

	ModulationState state{
	    sizeFromMin(args.canvasSize(), param<Str{"Line width"}>(params)) / 32.0,
	    std::exp2(std::lerp(-14.0f, 0.0f, param<Str{"Intensity"}>(params).value()))};

	std::ranges::for_each(
	    input<0>(args).get(),
	    [&args,
	     state,
	     mod_params,
	     trunk_midpoint = param<Str{"Trunk split point"}>(params).value()](auto const& item) {
		    auto const split_at = static_cast<size_t>(
		        std::lerp(1.0f, static_cast<float>(std::size(item.data)), trunk_midpoint));
		    auto const res = draw(std::span{std::begin(item.data), split_at},
		                          args,
		                          state,
		                          mod_params,
		                          Direction::Backward);
		    (void)draw(std::span{std::begin(item.data) + (split_at - 1), std::end(item.data)},
		               args,
		               res,
		               mod_params,
		               Direction::Forward);
	    });
}
```

## Tags

__Id:__ 916d9f54c7d922f50752901b95e0c22c

__Category:__ Converters

__Release state:__ Stable