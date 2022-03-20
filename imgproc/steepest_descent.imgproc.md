# Steepest descent

This image processor takes topography data and applies the steepest descent algorithm to each point in `Starget points`.

## Input ports

__Topgraphy:__ (Topography data)

__Start points:__ (Point cloud)

## Output ports

__Output:__ (Polyline set)

## Implementation

__Includes:__

```c++
#include <algorithm>
#include <cmath>
#include <numbers>
#include <random>
#include <numbers>
```

__Source code:__

```c++
/*
auto closest_point_less_than(auto const& args, RealValue threshold, vec2_t loc, double r)
{
	using RetValue = std::tuple<double, vec2_t, RealValue>;
	std::vector<RetValue> temp;

	auto const w = args.canvasSize().width();
	auto const h = args.canvasSize().height();

	auto const size = vec2_t{static_cast<double>(w),
	                         static_cast<double>(h)};
	auto const min  = loc - vec2_t{r, r} + size;
	auto const max  = loc + vec2_t{r, r} + size;

	for(uint32_t y = static_cast<uint32_t>(min[1]); y <= static_cast<uint32_t>(max[1]); ++y)
	{
		for(uint32_t x = static_cast<uint32_t>(min[0]); x <= static_cast<uint32_t>(max[0]); ++x)
		{
			auto const loc_current = vec2_t{static_cast<double>(x), static_cast<double>(y)} - size;
			auto const d2  = Texpainter::lengthSquared(loc_current - loc);
			auto const z = input<0>(args, x % w, y % h).elevation();
			if(z < threshold && d2>= 4.0 && d2 <= r*r)
			{ temp.push_back(std::tuple{d2, loc, z}); }
		}
	}

	std::ranges::sort(temp, [](auto const& a, auto const& b)
	{
		if(std::get<0>(a) < std::get<0>(b))
		{
			return true;
		}

		if(std::get<0>(a) > std::get<0>(b))
		{
			return false;
		}

		return std::get<2>(a) > std::get<2>(b);
	});

	return std::size(temp) != 0 ? temp[0] : std::optional<RetValue>{};
}
*/

void main(auto const& args)
{
	auto rng = Rng{args.rngSeed()};

	auto const& points = input<1>(args);
	std::ranges::for_each(points.get(), [&args, size = args.canvasSize(), rng](auto const& item) mutable {
		auto loc = vec2_t{static_cast<double>(item.loc.x), static_cast<double>(item.loc.y)};
		std::vector<vec2_t> points;
		points.reserve(16384);

		auto const z      = input<0>(args, item.loc.x, item.loc.y).value();
		auto min_altitude = z[3];
		auto travel_distance = 0.0;

		points.push_back(loc);
		{
			auto const grad = vec2_t{z[0], z[1]};
			auto const dir  = grad / Texpainter::length(grad);
			travel_distance += 1.0;
			loc += dir;
		}

		for(size_t k = 1; k < 16384 && travel_distance <= std::sqrt(area(size)); ++k)
		{
			auto get_val = [](vec2_t loc, Size2d size, auto const& args){
			auto const x_0 = (static_cast<uint32_t>(loc[0] + size.width())) % size.width();
			auto const y_0 = (static_cast<uint32_t>(loc[1] + size.height())) % size.height();
			auto const x_1 = (x_0 + size.width() + 1) % size.width();
			auto const y_1 = (y_0 + size.height() + 1) % size.height();

			auto const z_00 = input<0>(args, x_0, y_0).value();
			auto const z_01 = input<0>(args, x_0, y_1).value();
			auto const z_10 = input<0>(args, x_1, y_0).value();
			auto const z_11 = input<0>(args, x_1, y_1).value();

			auto const xi = loc - vec2_t{static_cast<double>(x_0), static_cast<double>(y_0)};

			auto const z_x0 =
			    (1.0f - static_cast<float>(xi[0])) * z_00 + static_cast<float>(xi[0]) * z_10;
			auto const z_x1 =
			    (1.0f - static_cast<float>(xi[0])) * z_01 + static_cast<float>(xi[0]) * z_11;
			return (1.0f - static_cast<float>(xi[1])) * z_x0 + static_cast<float>(xi[1]) * z_x1;
			};

			auto const z_xy = get_val(loc, size, args);

			if(z_xy[3] < min_altitude)
			{
				min_altitude    = z_xy[3];
				auto const grad = vec2_t{z_xy[0], z_xy[1]};
				auto const dir  = grad / Texpainter::length(grad);
				travel_distance += 1.0;
				loc += dir;
			}
			else
			{
				std::uniform_real_distribution theta_dist{-std::numbers::pi, std::numbers::pi};
				auto const theta = theta_dist(rng);
				loc += vec2_t{std::cos(theta), std::sin(theta)};
				travel_distance += 1.0;
				min_altitude = std::nextafter(get_val(loc, size, args)[3], INFINITY);
			}

			points.push_back(loc);

		}
		output<0>(args).get().push_back(std::move(points));
	});
}
```

## Tags

__Id:__ 788e43cee19047b736ce74d07bcca523

__Category:__ Converters

__Release state:__ Experimental