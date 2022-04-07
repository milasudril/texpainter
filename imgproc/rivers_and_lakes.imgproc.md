# Rivers and lakes

This image processor takes a Grayscale image as a heightmap and generates rivier systems, starting from each point in `Start points`.

## Input ports

__Heightmap:__ (Grayscale image)

__Start points:__ (Point cloud)

## Output ports

__Rivers:__ (Polyline set) The generated rivers

__Lakes:__ (Grayscale paint args) The points where lakes would appear

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
	vec2_t dir;
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
	auto const dx  = vec2_t{1.0, 0.0};
	auto const dy  = vec2_t{0.0, 1.0};
	auto const ddx = get_val(loc + dx, args) - get_val(loc - dx, args);
	auto const ddy = get_val(loc + dy, args) - get_val(loc - dy, args);

	auto const ret = vec2_t{ddx, ddy};
	auto const l   = Texpainter::length(ret);

	return l > 1.0 / (1024.0 * 1024.0) ? -ret / l : vec2_t{0.0, 0.0};
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

int16_t quantize(float val, float factor) { return static_cast<int16_t>(val * factor); }

std::optional<EscapePoint> find_escape_point(auto const& args,
                                             vec2_t start_loc_a,
                                             vec2_t start_loc_b)
{
	std::stack<IntLoc> nodes;
	auto const w = args.canvasSize().width();
	auto const h = args.canvasSize().height();
	Image<int8_t> visited{w, h};

	const auto loc_a = start_loc_a + vec2_t{0.5, 0.5};
	const auto loc_b = start_loc_b + vec2_t{0.5, 0.5};

	IntLoc const a{static_cast<uint32_t>(loc_a[0]), static_cast<uint32_t>(loc_a[1])};
	IntLoc const b{static_cast<uint32_t>(loc_b[0]), static_cast<uint32_t>(loc_b[1])};

	nodes.push(a);
	nodes.push(b);

	push_neigbours(nodes, args, a, [&visited](auto const&, uint32_t x, uint32_t y) {
		visited(x, y) = 1;
		return true;
	});

	push_neigbours(nodes, args, b, [&visited](auto const&, uint32_t x, uint32_t y) {
		visited(x, y) = 1;
		return true;
	});

	const auto start_loc = 0.5 * (start_loc_a + start_loc_b);
	while(!nodes.empty())
	{
		auto const node = nodes.top();
		nodes.pop();

		auto const z0 = quantize(input<0>(args, node.x, node.y), 32767.5f);
		push_neigbours(
		    nodes, args, node, [&visited, z0, start_loc](auto const& args, uint32_t x, uint32_t y) {
			    if(visited(x, y) == 0
			       && Texpainter::lengthSquared(
			              start_loc - vec2_t{static_cast<double>(x), static_cast<double>(y)})
			              <= 256.0 * 256.0
			       && quantize(input<0>(args, x, y), 32767.5f) >= z0)
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
				vec2_t const loc{static_cast<double>(l) + 0.5, static_cast<double>(k) + 0.5};
				vec2_t const dir{static_cast<double>(dx), static_cast<double>(dy)};
				esc_points.push_back(
				    EscapePoint{loc, -dir / Texpainter::length(dir), get_val(loc, args)});
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
	Image<int8_t> visited{args.canvasSize()};
	std::ranges::for_each(
	    points.get(), [&args, size = args.canvasSize(), &visited](auto const& item) mutable {
		    auto loc = vec2_t{static_cast<double>(item.loc.x), static_cast<double>(item.loc.y)};
		    std::vector<vec2_t> points;
		    auto& lakes = output<1>(args).get();
		    points.reserve(16384);

		    auto min_altitude    = input<0>(args, item.loc.x, item.loc.y);
		    auto travel_distance = 0.0;

		    points.push_back(loc);

		    while(travel_distance <= std::sqrt(area(size)))
		    {
			    if(loc[0] < 1.0 || loc[1] < 1.0 || loc[0] >= args.canvasSize().width() - 1
			       || loc[1] >= args.canvasSize().height() - 1)
			    { break; }

			    if(visited(static_cast<uint32_t>(loc[0] + 0.5),
			               static_cast<uint32_t>(loc[1] + 0.5)))
			    { break; }
			    visited(static_cast<uint32_t>(loc[0] + 0.5), static_cast<uint32_t>(loc[1] + 0.5)) =
			        1;

			    auto const loc_next = loc + grad(loc, args);
			    if(loc_next[0] < 0.0 || loc_next[1] < 0.0
			       || loc_next[0] >= args.canvasSize().width()
			       || loc_next[1] >= args.canvasSize().height())
			    { break; }

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
				    auto const esc = find_escape_point(args, loc, loc_next);
				    if(!esc.has_value()) { break; }
				    loc = loc_next;

				    auto const loc_next = esc->loc;
				    auto const d        = Texpainter::length(loc - loc_next);
				    if(d < 4.0) { break; }

				    if(esc->value > z_next)
				    {
						lakes.push_back(
							GrayscalePaintArgs{ImageCoordinates{static_cast<uint32_t>(loc[0]),
																static_cast<uint32_t>(loc[1])},
											esc->value});
					}
				    travel_distance += Texpainter::length(loc_next - loc);
				    min_altitude = esc->value;
				    loc          = loc_next;
				    points.push_back(loc);
			    }
		    }
		    output<0>(args).get().push_back(std::move(points));
	    });
	//	printf("%zu\n\n", std::size(output<1>(args).get()));
}
```

## Tags

__Id:__ 788e43cee19047b736ce74d07bcca523

__Category:__ Converters

__Release state:__ Experimental