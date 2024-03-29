# Rivers and lakes

This image processor takes a Grayscale image as a heightmap and generates rivier systems, starting from each point in `Start points`.

## Input ports

__Heightmap:__ (Grayscale image)

__Start points:__ (Point cloud)

## Output ports

__Rivers:__ (Polyline set) The generated rivers

__Heightmap:__ (Grayscale image) The new heightmap (lakes are flat)

__Lakes:__ (Grayscale image) The regions where lakes would appear

## Implementation

The idea behind the implementation is to use steepest descent to generate the rivers. When it gets stuck in a local minumum, a variant of floodfill is used to deduce the location and elevation of the drainage point of the lake that would form around the local minumum. Then, it will continue by using steepest descent, until it reaches the boundary of the heightmap, or when it as traveled for more than the size of the heightmap (size here means the square root of its area).

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

	auto const z_00 = output<1>(args, x_0, y_0);
	auto const z_01 = output<1>(args, x_0, y_1);
	auto const z_10 = output<1>(args, x_1, y_0);
	auto const z_11 = output<1>(args, x_1, y_1);

	auto const xi = loc - vec2_t{static_cast<double>(x_0), static_cast<double>(y_0)};

	auto const z_x0 = (1.0f - static_cast<float>(xi[0])) * z_00 + static_cast<float>(xi[0]) * z_10;
	auto const z_x1 = (1.0f - static_cast<float>(xi[0])) * z_01 + static_cast<float>(xi[0]) * z_11;
	return (1.0f - static_cast<float>(xi[1])) * z_x0 + static_cast<float>(xi[1]) * z_x1;
};

auto grad(vec2_t loc, auto const& args)
{
	auto const dx  = vec2_t{0.5, 0.0};
	auto const dy  = vec2_t{0.0, 0.5};
	auto const ddx = get_val(loc + dx, args) - get_val(loc - dx, args);
	auto const ddy = get_val(loc + dy, args) - get_val(loc - dy, args);

	auto const ret = vec2_t{ddx, ddy};
	auto const l   = Texpainter::length(ret);

	return l > 1.0 / (1024.0 * 1024.0) ? -ret / l : vec2_t{0.0, 0.0};
}

template<class Filter>
void push_neigbours_4(std::stack<IntLoc>& nodes, auto const& args, IntLoc start_pos, Filter&& f)
{
	auto const w = args.canvasSize().width();
	auto const h = args.canvasSize().height();

	if(start_pos.y < h - 1 && f(args, start_pos.x, start_pos.y + 1))
	{ nodes.push(IntLoc{start_pos.x, start_pos.y + 1}); }

	if(start_pos.x < w - 1 && f(args, start_pos.x + 1, start_pos.y))
	{ nodes.push(IntLoc{start_pos.x + 1, start_pos.y}); }

	if(start_pos.x >= 1 && f(args, start_pos.x - 1, start_pos.y))
	{ nodes.push(IntLoc{start_pos.x - 1, start_pos.y}); }

	if(start_pos.y >= 1 && f(args, start_pos.x, start_pos.y - 1))
	{ nodes.push(IntLoc{start_pos.x, start_pos.y - 1}); }
}

Image<int8_t> gen_drainage_basin(auto const& args, vec2_t start_loc)
{
	std::stack<IntLoc> nodes;
	auto const w = args.canvasSize().width();
	auto const h = args.canvasSize().height();
	Image<int8_t> ret{w, h};

	IntLoc const loc{static_cast<uint32_t>(start_loc[0]), static_cast<uint32_t>(start_loc[1])};
	nodes.push(loc);
	while(!nodes.empty())
	{
		auto const node = nodes.top();
		nodes.pop();

		auto const x = node.x;
		auto const y = node.y;
		if(ret(x, y) == 1) { continue; }

		ret(x, y) = 1;

		auto const z0 = output<1>(args, node.x, node.y);
		push_neigbours_4(
		    nodes, args, node, [&ret, z0, start_loc](auto const& args, uint32_t x, uint32_t y) {
			    if(ret(x, y) == 0 && output<1>(args, x, y) >= z0) { return true; }
			    return false;
		    });
	}
	return ret;
}

std::optional<EscapePoint> find_escape_point(auto const& args, Image<int8_t> const& drainage_basin)
{
	auto const w = drainage_basin.width();
	auto const h = drainage_basin.height();
	std::vector<EscapePoint> esc_points;
	for(uint32_t k = 1; k != h - 1; ++k)
	{
		for(uint32_t l = 1; l != w - 1; ++l)
		{
			auto const dx = drainage_basin(l + 1, k) - drainage_basin(l - 1, k);
			auto const dy = drainage_basin(l, k + 1) - drainage_basin(l, k - 1);
			if(drainage_basin(l, k) * (dx * dx + dy * dy) > 0 && output<2>(args, l, k) < 0.5f)
			{
				vec2_t const loc{static_cast<double>(l), static_cast<double>(k)};
				esc_points.push_back(EscapePoint{loc, output<1>(args, l, k)});
			}
		}
	}

	if(std::size(esc_points) == 0) { return std::optional<EscapePoint>{}; }

	return *std::ranges::min_element(
	    esc_points, [](auto const& a, auto const& b) { return a.value < b.value; });
}

void fill_lake(auto const& args,
               Image<int8_t> const& drainage_basin,
               vec2_t start_loc,
               float surface_level)
{
	std::stack<IntLoc> nodes;

	auto const w = args.canvasSize().width();
	auto const h = args.canvasSize().height();
	Image<int8_t> visited{w, h};

	IntLoc const loc{static_cast<uint32_t>(start_loc[0]), static_cast<uint32_t>(start_loc[1])};
	nodes.push(loc);

	while(!nodes.empty())
	{
		auto const node = nodes.top();
		nodes.pop();

		auto const x = node.x;
		auto const y = node.y;

		if(visited(x, y) == 1) { continue; }

		output<1>(args, x, y) = surface_level;
		output<2>(args, x, y) = 1.0f;
		visited(x, y)         = 1;

		push_neigbours_4(nodes,
		                 args,
		                 node,
		                 [&visited, &drainage_basin, surface_level, start_loc](
		                     auto const& args, uint32_t x, uint32_t y) {
			                 if(visited(x, y) == 0 && output<1>(args, x, y) < surface_level
			                    && drainage_basin(x, y) != 0)
			                 { return true; }
			                 return false;
		                 });
	}
}

static constexpr std::array<vec2_t, 9> neighbourhood{vec2_t{0.0, 0.0},

                                                     vec2_t{-1.0, -1.0},
                                                     vec2_t{0.0, -1.0},
                                                     vec2_t{1.0, -1.0},

                                                     vec2_t{-1.0, 0.0},
                                                     vec2_t{1.0, 0.0},

                                                     vec2_t{-1.0, 1.0},
                                                     vec2_t{0.0, 1.0},
                                                     vec2_t{1.0, 1.0}};

auto lowest_in_neigbourhood(auto const& args, vec2_t loc)
{
	return std::ranges::min_element(neighbourhood, [&args, loc](auto a, auto b) {
		a += loc + vec2_t{0.5, 0.5};
		b += loc + vec2_t{0.5, 0.5};
		auto const x_a = static_cast<uint32_t>(a[0]);
		auto const y_a = static_cast<uint32_t>(a[1]);
		auto const x_b = static_cast<uint32_t>(b[0]);
		auto const y_b = static_cast<uint32_t>(b[1]);

		return output<1>(args, x_a, y_a) < output<1>(args, x_b, y_b);
	});
}

void main(auto const& args)
{
	auto const& points = input<1>(args);
	std::copy(input<0>(args), input<0>(args) + area(args.canvasSize()), output<1>(args));
	std::ranges::for_each(points.get(), [&args, size = args.canvasSize()](auto const& item) {
		auto loc = vec2_t{static_cast<double>(item.loc.x), static_cast<double>(item.loc.y)};
		std::vector<vec2_t> points;

		auto min_altitude    = output<1>(args, item.loc.x, item.loc.y);
		auto travel_distance = 0.0;

		points.push_back(loc);

		while(travel_distance <= 4.0 * std::sqrt(area(size)))
		{
			if(loc[0] < 2.0 || loc[1] < 2.0 || loc[0] >= args.canvasSize().width() - 2
			   || loc[1] >= args.canvasSize().height() - 2)
			{ break; }

			auto const loc_next = loc + grad(loc, args);

			auto const z_next = get_val(loc_next, args);

			if(z_next < min_altitude)
			{
				travel_distance += Texpainter::length(loc - loc_next);
				min_altitude = z_next;
				loc          = loc_next;
				points.push_back(loc);
			}
			else
			{
				// Check that we have hit a local minimum. For that to be true, all other points
				// in its neighbourhood must be greater.
				auto const i_lowest = lowest_in_neigbourhood(args, loc);
				auto const lowest   = *i_lowest + loc;
				if(i_lowest != std::begin(neighbourhood))
				{
					// We did not get a local minimum. See if the lowest point found in the
					// neighbourhood is a local minimum
					auto const i_lowest_2 = lowest_in_neigbourhood(args, lowest);
					if(i_lowest_2 != std::begin(neighbourhood))
					{
						// If not, continue without creating a lake
						auto const lowest_offset = lowest + vec2_t{0.5, 0.5};
						auto const x             = static_cast<uint32_t>(lowest_offset[0]);
						auto const y             = static_cast<uint32_t>(lowest_offset[1]);
						auto const loc_next_1 =
						    vec2_t{static_cast<double>(x), static_cast<double>(y)};
						travel_distance += Texpainter::length(loc_next_1 - loc);
						loc          = loc_next_1;
						min_altitude = output<1>(args, x, y);
						points.push_back(loc);
						continue;
					}
				}

				// Go to local minumum
				auto const lowest_offset = lowest + vec2_t{0.5, 0.5};
				auto const loc_next_1 =
				    vec2_t{static_cast<double>(static_cast<uint32_t>(lowest_offset[0])),
				           static_cast<double>(static_cast<uint32_t>(lowest_offset[1]))};
				travel_distance += Texpainter::length(loc_next_1 - loc);
				loc = loc_next_1;
				points.push_back(loc);

				// Generate a lake
				auto const min_val =
				    output<1>(args, static_cast<uint32_t>(loc[0]), static_cast<uint32_t>(loc[1]));
				auto const drainage_basin = gen_drainage_basin(args, loc);
				auto const esc            = find_escape_point(args, drainage_basin);
				if(!esc.has_value()) { break; }

				fill_lake(args, drainage_basin, loc, esc->value);

				auto const loc_next_2 = esc->loc;
				min_altitude          = esc->value;
				loc                   = loc_next_2;
				travel_distance       = 0;
				output<0>(args).get().push_back(std::move(points));
				points.push_back(loc);

				if(esc->value < min_val) { break; }
			}
		}
		output<0>(args).get().push_back(std::move(points));
	});
}
```

## Tags

__Id:__ 788e43cee19047b736ce74d07bcca523

__Category:__ Converters

__Release state:__ Experimental