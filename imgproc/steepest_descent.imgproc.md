# Steepest descent

This image processor takes topography data and applies the steepest descent algorithm to each point in `Starget points`.

## Input ports

__Intensity:__ (Grayscale image)

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
#include <stack>
#include <vector>
```

__Source code:__

```c++
struct IntLoc
{
	uint32_t x;
	uint32_t y;
};

struct EscapePoint
{
	vec2_t loc;
	RealValue value;
};

auto get_val(vec2_t loc, auto const& args)
{
	auto const size = args.canvasSize();
	auto const x_0  = (static_cast<uint32_t>(loc[0] + size.width())) % size.width();
	auto const y_0  = (static_cast<uint32_t>(loc[1] + size.height())) % size.height();
	auto const x_1  = (x_0 + size.width() + 1) % size.width();
	auto const y_1  = (y_0 + size.height() + 1) % size.height();

	auto const z_00 = input<0>(args, x_0, y_0);
	auto const z_01 = input<0>(args, x_0, y_1);
	auto const z_10 = input<0>(args, x_1, y_0);
	auto const z_11 = input<0>(args, x_1, y_1);

	auto const xi = loc - vec2_t{static_cast<double>(x_0), static_cast<double>(y_0)};

	auto const z_x0 = (1.0f - static_cast<float>(xi[0])) * z_00 + static_cast<float>(xi[0]) * z_10;
	auto const z_x1 = (1.0f - static_cast<float>(xi[0])) * z_01 + static_cast<float>(xi[0]) * z_11;
	return (1.0f - static_cast<float>(xi[1])) * z_x0 + static_cast<float>(xi[1]) * z_x1;
};

auto grad(vec2_t loc, auto const& args)
{
	auto const dx = vec2_t{1.0, 0.0};
	auto const dy = vec2_t{0.0, 1.0};
	auto const ddx = get_val(loc + dx, args) - get_val(loc - dx, args);
	auto const ddy = get_val(loc + dy, args) - get_val(loc - dy, args);

	auto const ret = vec2_t{ddx, ddy};
	return -ret/Texpainter::length(ret);
}

template<class Filter>
void push_neigbours(std::stack<IntLoc>& nodes, auto const& args, IntLoc start_pos, Filter&& f)
{
	auto const w = args.canvasSize().width();
	auto const h = args.canvasSize().height();

	if(start_pos.y < h - 1 && start_pos.x < w - 1 && f(args, start_pos.x + 1, start_pos.y + 1))
	{ nodes.push(IntLoc{start_pos.x + 1, start_pos.y + 1}); }

	if(start_pos.y < h - 1 && f(args, start_pos.x, start_pos.y + 1))
	{ nodes.push(IntLoc{start_pos.x, start_pos.y + 1}); }

	if(start_pos.y < h - 1 && start_pos.x >= 1 && f(args, start_pos.x - 1, start_pos.y + 1))
	{ nodes.push(IntLoc{start_pos.x - 1, start_pos.y + 1}); }

	if(start_pos.x < w - 1 && f(args, start_pos.x + 1, start_pos.y))
	{ nodes.push(IntLoc{start_pos.x + 1, start_pos.y}); }

	if(start_pos.x >= 1 && f(args, start_pos.x - 1, start_pos.y))
	{ nodes.push(IntLoc{start_pos.x - 1, start_pos.y}); }

	if(start_pos.y >= 1 && start_pos.x < w - 1 && f(args, start_pos.x + 1, start_pos.y - 1))
	{ nodes.push(IntLoc{start_pos.x + 1, start_pos.y - 1}); }

	if(start_pos.y >= 1 && f(args, start_pos.x, start_pos.y - 1))
	{ nodes.push(IntLoc{start_pos.x, start_pos.y - 1}); }

	if(start_pos.y >= 1 && start_pos.x >= 1 && f(args, start_pos.x - 1, start_pos.y - 1))
	{ nodes.push(IntLoc{start_pos.x - 1, start_pos.y - 1}); }
}

int16_t quantize(float val, float factor)
{
	return static_cast<int16_t>(val*factor);
}

std::optional<EscapePoint> find_escape_point(auto const& args, vec2_t start_loc)
{
	std::stack<IntLoc> nodes;
	auto const w = args.canvasSize().width();
	auto const h = args.canvasSize().height();
	Image<int8_t> visited{w, h};

	nodes.push(IntLoc{static_cast<uint32_t>(start_loc[0]), static_cast<uint32_t>(start_loc[1])});
	push_neigbours(nodes,
	               args,
	               IntLoc{static_cast<uint32_t>(start_loc[0]), static_cast<uint32_t>(start_loc[1])},
	               [](auto const&...) { return true; });

	while(!nodes.empty())
	{
		auto const node = nodes.top();
		nodes.pop();

		auto const z0 = quantize(input<0>(args, node.x, node.y), 16384.0f);
		push_neigbours(nodes, args, node, [&visited, z0](auto const& args, uint32_t x, uint32_t y) {
			if(visited(x, y) == 0 && quantize(input<0>(args, x, y), 16384.0f) >= z0)
			{
				visited(x, y) = 1;
				return true;
			}
			return false;
		});
	}

	std::vector<EscapePoint> esc_points;
	for(uint32_t k = 0; k != h - 1; ++k)
	{
		for(uint32_t l = 0; l != w - 1; ++l)
		{
			auto const dx = visited(l + 1, k) - visited(l, k);
			auto const dy = visited(l, k + 1) - visited(l, k);
			if(dx * dx + dy * dy > 0)
			{
				vec2_t loc{static_cast<double>(l) + 0.5, static_cast<double>(k) + 0.5};
				esc_points.push_back(EscapePoint{loc, get_val(loc, args)});
			}
		}
	}

	if(std::size(esc_points) == 0) { return std::optional<EscapePoint>{}; }


	return *std::ranges::min_element(
	    esc_points, [](auto const& a, auto const& b) { return a.value < b.value; });
}

void main(auto const& args)
{
	auto const& points = input<1>(args);
	std::ranges::for_each(
	    points.get(), [&args, size = args.canvasSize()](auto const& item) mutable {
		    auto loc = vec2_t{static_cast<double>(item.loc.x), static_cast<double>(item.loc.y)};
		    std::vector<vec2_t> points;
		    points.reserve(16384);

		    auto min_altitude    = input<0>(args, item.loc.x, item.loc.y);
		    auto travel_distance = 0.0;

		    points.push_back(loc);
		    {
			    auto const loc_next = loc + grad(loc, args);
			    travel_distance += Texpainter::length(loc - loc_next);
			    loc = loc_next;
		    }

		    while(travel_distance <= std::sqrt(area(size)))
		    {
			    auto const z_xy = get_val(loc, args);

			    if(z_xy < min_altitude)
			    {
				    auto const loc_next = loc + grad(loc, args);
				    travel_distance += Texpainter::length(loc - loc_next);
				    loc          = loc_next;
				    min_altitude = z_xy;
			    }
			    else
			    {
				    if(loc[0] < 0.0 || loc[1] < 0.0) { break; }

				    auto const esc = find_escape_point(args, loc);
				    if(!esc.has_value()) { break; }

				    auto const loc_next = esc->loc;

				    printf("%.15g %.15g %.8g-> %.15g %.15g %.8g\n",
				           loc[0],
				           loc[1],
				           z_xy,
				           loc_next[0],
				           loc_next[1],
				           esc->value);

				    break;
				    /*
				    travel_distance += Texpainter::length(loc - loc_next);
				    loc = loc_next;
				    min_altitude = esc->value;
*/
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