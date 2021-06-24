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
```

__Source code:__

```c++
void main(auto const& args)
{
	auto const& points = input<1>(args);
	std::ranges::for_each(points.get(), [&args, size = args.canvasSize()](auto const& item) {
		auto loc = vec2_t{static_cast<double>(item.loc.x), static_cast<double>(item.loc.y)};
		std::vector<vec2_t> points;
		points.reserve(16384);

		auto const z      = input<0>(args, item.loc.x, item.loc.y).value();
		auto min_altitude = z[3];
		points.push_back(loc);
		{
			auto const grad = vec2_t{z[0], z[1]};
			auto const dir  = grad / Texpainter::length(grad);
			loc += dir;
		}

		for(size_t k = 1; k < 16384; ++k)
		{
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
			auto const z_xy =
			    (1.0f - static_cast<float>(xi[1])) * z_x0 + static_cast<float>(xi[1]) * z_x1;

			if(z_xy[3] >= min_altitude) { break; }

			points.push_back(loc);

			min_altitude    = z_xy[3];
			auto const grad = vec2_t{z_xy[0], z_xy[1]};
			auto const dir  = grad / Texpainter::length(grad);
			loc += dir;
		}
		output<0>(args).get().push_back(std::move(points));
	});
}
```

## Tags

__Id:__ 788e43cee19047b736ce74d07bcca523

__Category:__ Converters

__Release state:__ Experimental