# Make line segment trees

## Input ports

__Input:__ (Point cloud)

## Output ports

__Output:__ (Line segment tree)

## Parameters

__Segment length:__ (= 0.5)

__Stiffness:__ (= 0.5)

__Integrated length:__ (= 0.5)

__Max depth:__ (= 0.0) The number of tree levels __Branch scale factor: (= 0.5) The scaling factor between tree levels

## Implementation

__Includes:__

```c++
#include <random>
```

__Source code:__

```c++
inline auto gen_branch(double const segment_length,
                       double const stiffness,
                       double const length_tot,
                       Rng& rng,
                       vec2_t location,
                       double heading)
{
	std::vector<std::pair<vec2_t, LineSegTree>> ret{std::pair{location, LineSegTree{}}};

	std::uniform_real_distribution turn{-0.5 * std::numbers::pi, 0.5 * std::numbers::pi};
	std::gamma_distribution seg_length{2.0, std::max(segment_length, 1.0)};
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

inline LineSegTree gen_line_segment_tree(double segment_length,
                                         double const stiffness,
                                         double const length_tot,
                                         Rng& rng,
                                         vec2_t const start_loc,
                                         double start_heading,
                                         size_t const)
{
	return LineSegTree{
	    gen_branch(segment_length, stiffness, length_tot, rng, start_loc, start_heading)};
}

void main(auto const& args, auto const& params)
{
	auto const domain_length = std::sqrt(area(args.canvasSize()));

	std::ranges::transform(
	    input<0>(args).get(),
	    std::back_inserter(output<0>(args).get()),
	    [segment_length =
	         static_cast<double>(param<Str{"Segment length"}>(params).value() * domain_length),
	     stiffness = static_cast<double>(param<Str{"Stiffness"}>(params).value()),
	     length_tot =
	         static_cast<double>(param<Str{"Integrated length"}>(params).value() * domain_length),
	     rng = Rng{args.rngSeed()},
	     max_depth =
	         static_cast<size_t>(1)
	         + static_cast<size_t>(std::lerp(
	             0.0f, std::nextafter(3.0f, 0.0f), param<Str{"Max depth"}>(params).value()))](
	        auto val) mutable {
		    auto const loc_vec =
		        vec2_t{static_cast<double>(val.loc.x), static_cast<double>(val.loc.y)};
		    auto const start_heading = val.rot.radians();
		    auto const length_scale = static_cast<double>(val.scale);
		    return gen_line_segment_tree(
		        segment_length*length_scale, stiffness, length_tot*length_scale, rng, loc_vec, start_heading, max_depth);
	    });
}
```

## Tags

__Id:__ 1a8d90eee0fdedb3e8af0a0981b20130

__Category:__ Converters

__Release state:__ Experimental