# Make line segment trees

## Input ports

__Input:__ (Point cloud) Input

## Output ports

__Output:__ (Line segment tree) Output image

## Parameters

__Segment length:__ (= 0.5)

__Stiffness:__ (= 0.5)

__Integrated length:__ (= 0.5)

__Max depth:__ (= 0.0) The number of tree levels
__Branch scale factor: (= 0.5) The scaling factor between tree levels

## Implementation

__Includes:__ 

```c++
#include <random>
```

__Source code:__ 

```c++
auto gen_branch(double const segment_length,
                double const stiffness,
                double const length_tot,
                vec2_t location,
                double heading,
                Rng& rng)
{
	std::vector<std::pair<vec2_t, LineSegTree>> ret{std::pair{location, LineSegTree{}}};

	std::uniform_real_distribution turn{-0.5 * std::numbers::pi, 0.5 * std::numbers::pi};
	std::gamma_distribution seg_length{2.0, segment_length};
	auto const h0 = heading;
	auto l_tot    = 0.0;
	while(l_tot < length_tot)
	{
		auto l = seg_length(rng);
		heading += turn(rng);
		heading += stiffness * (h0 - heading);
		location += l * vec2_t{std::cos(heading), std::sin(heading)};
		ret.push_back(std::pair{location, LineSegTree{}});
		l_tot += l;
	}

	return ret;
}

LineSegTree gen_line_segment_tree(vec2_t, size_t)
{
	LineSegTree ret{};

	return ret;
}

void main(auto const& args, auto const& params)
{
	size_t const max_depth =
	    1
	    + static_cast<size_t>(
	        std::lerp(0.0f, std::nextafter(3.0f, 0.0f), param<Str{"Max depth"}>(params).value()));
	std::ranges::transform(
	    input<0>(args).get(), std::back_inserter(output<0>(args).get()), [max_depth](auto val) {
		    auto const loc_vec =
		        vec2_t{static_cast<double>(val.loc.x), static_cast<double>(val.loc.y)};
		    return gen_line_segment_tree(loc_vec, max_depth);
	    });
}
```

## Tags

__Id:__ 1a8d90eee0fdedb3e8af0a0981b20130

__Category:__ Converters

__Release state:__ Experimental