# Make line segment trees

## Input ports

__Input:__ (Point cloud)

## Output ports

__Output:__ (Line segment tree)

## Parameters

__Segment length:__ (= 0.5)

__Stiffness:__ (= 0.5)

__Branch length:__ (= 0.5)

__Max depth:__ (= 0.0) The number of tree levels __Branch scale factor: (= 0.5) The scaling factor between tree levels

__Segment scale factor:__ (= 0.5)

__Branch scale factor:__ (= 0.5)

## Implementation

__Includes:__

```c++
#include <random>
```

__Source code:__

```c++
inline auto gen_branch(double segment_length,
                       double stiffness,
                       double length_tot,
                       Rng& rng,
                       vec2_t location,
                       double heading)
{
	std::vector<std::pair<vec2_t, LineSegTree>> ret{std::pair{location, LineSegTree{}}};

	std::uniform_real_distribution turn{-0.5 * std::numbers::pi, 0.5 * std::numbers::pi};
	std::gamma_distribution seg_length{2.0, std::max(segment_length, 1.0)};
	auto const h0  = heading;
	auto const lsq = length_tot * length_tot;
	auto const l0  = location;
	while(Texpainter::lengthSquared(location - l0) < lsq)
	{
		auto l = seg_length(rng);
		heading += turn(rng);
		heading += stiffness * (h0 - heading);
		location += l * vec2_t{std::cos(heading), std::sin(heading)};
		ret.push_back(std::pair{location, LineSegTree{}});
	}

	return ret;
}

inline auto compute_normal(vec2_t prev, vec2_t current, vec2_t next)
{
	auto const v      = Texpainter::normalize(next - prev);
	auto const delta  = current - prev;
	auto const n_temp = vec2_t{v[1], -v[0]};
	if(auto const side = Texpainter::dot(delta, n_temp); side < 0.0) { return -n_temp; }
	else
	{
		return n_temp;
	}
}

inline LineSegTree gen_line_segment_tree(double segment_length,
                                         double stiffness,
                                         double length_tot,
                                         Rng& rng,
                                         vec2_t start_loc,
                                         double start_heading,
                                         size_t max_depth,
                                         double seg_scale_factor,
                                         double branch_scale_factor)
{
	auto branch = gen_branch(segment_length, stiffness, length_tot, rng, start_loc, start_heading);
	if(max_depth != 0)
	{
		auto prev    = branch.back().first;
		auto current = branch.front().first;
		for(size_t k = 1; k != std::size(branch); ++k)
		{
			auto const n              = compute_normal(prev, current, branch[k].first);
			auto const theta          = std::atan2(n[1], n[0]);
			branch[k - 1].second = gen_line_segment_tree(segment_length*seg_scale_factor,
			                                                  stiffness,
			                                                  length_tot*branch_scale_factor,
			                                                  rng,
			                                                  branch[k - 1].first,
			                                                  theta,
			                                                  max_depth - 1,
			                                                  seg_scale_factor,
			                                                  branch_scale_factor);
			prev                      = current;
			current                   = branch[k].first;
		}
	}

	return LineSegTree{std::move(branch)};
}

void main(auto const& args, auto const& params)
{
	auto const domain_length = std::sqrt(area(args.canvasSize()));

	auto gen_segs = [segment_length = static_cast<double>(
	                     param<Str{"Segment length"}>(params).value() * domain_length),
	                 stiffness  = static_cast<double>(param<Str{"Stiffness"}>(params).value()),
	                 length_tot = static_cast<double>(param<Str{"Branch length"}>(params).value()
	                                                  * domain_length),
	                 rng        = Rng{args.rngSeed()},
	                 max_depth =
	                     static_cast<size_t>(std::lerp(0.0f,
	                                                   std::nextafter(4.0f, 0.0f),
	                                                   param<Str{"Max depth"}>(params).value())),
	                 seg_scale_factor = param<Str{"Segment scale factor"}>(params).value(),
	                 branch_scale_factor =
	                     param<Str{"Branch scale factor"}>(params).value()](auto val) mutable {
		auto const loc_vec = vec2_t{static_cast<double>(val.loc.x), static_cast<double>(val.loc.y)};
		auto const start_heading = val.rot.radians();
		auto const length_scale  = static_cast<double>(val.scale);
		return gen_line_segment_tree(segment_length * length_scale,
		                             stiffness,
		                             length_tot * length_scale,
		                             rng,
		                             loc_vec,
		                             start_heading,
		                             max_depth,
		                             seg_scale_factor,
		                             branch_scale_factor);
	};

	std::ranges::transform(
	    input<0>(args).get(), std::back_inserter(output<0>(args).get()), gen_segs);
}
```

## Tags

__Id:__ 1a8d90eee0fdedb3e8af0a0981b20130

__Category:__ Converters

__Release state:__ Experimental