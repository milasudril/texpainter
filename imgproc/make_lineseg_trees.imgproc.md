# Make line segment trees

## Input ports

__Input:__ (Point cloud)

__Direction field:__ (Topography data)

## Output ports

__Output:__ (Line segment tree)

## Parameters

__Segment length:__ (= 0.5)

__Stiffness:__ (= 0.5)

__Branch length:__ (= 0.5)

__Max depth:__ (= 0.0) The number of tree levels

__Branch rate:__ (= 0.5)

__Segment scale factor:__ (= 0.5)

__Secondary branch length:__ (= 0.5)

## Implementation

__Includes:__

```c++
#include <random>
#include <deque>
```

__Source code:__

```c++
inline auto gen_branch(double segment_length,
                       double stiffness,
                       double length_tot,
                       Rng& rng,
                       vec2_t location,
                       double heading,
                       TopographyInfo const* topo_info,
                       Size2d domain_size)
{
	std::vector<std::pair<vec2_t, LineSegTree>> ret{std::pair{location, LineSegTree{}}};

	std::uniform_real_distribution turn{-0.5 * std::numbers::pi, 0.5 * std::numbers::pi};
	std::gamma_distribution seg_length{3.0, segment_length * length_tot};
	auto const h0  = heading;
	auto const lsq = length_tot * length_tot;
	auto const l0  = location;
	auto const w = domain_size.width();
	auto const h = domain_size.height();
	while(Texpainter::lengthSquared(location - l0) < lsq)
	{
		auto const l = std::max(seg_length(rng), 16.0);
		location += l * vec2_t{std::cos(heading), std::sin(heading)};

		auto const x = static_cast<uint32_t>(location[0] + w);
		auto const y = static_cast<uint32_t>(location[1] + h);
		auto const field = topo_info[w*(y%h) + x%w].normal();
		auto const h_grad = std::atan2(-field[1], -field[0]);

		heading += turn(rng);
		heading += stiffness * (h0*0.75 + (1.0 - 0.75)*h_grad - heading);

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

struct BranchParams
{
	double segment_length;
	double length_tot;
	vec2_t start_loc;
	double start_heading;
	std::reference_wrapper<LineSegTree> ret;
	double max_seg_length;
	size_t depth;
};

inline LineSegTree gen_line_segment_tree(double segment_length,
                                         double stiffness,
                                         double length_tot,
                                         Rng& rng,
                                         vec2_t start_loc,
                                         double start_heading,
                                         size_t max_depth,
                                         double branch_rate,
                                         double seg_scale_factor,
                                         double branch_scale_factor,
                                         TopographyInfo const* topo_info,
                                         Size2d domain_size)
{
	std::deque<BranchParams> branches;
	LineSegTree ret;
	branches.push_back(
	    BranchParams{segment_length, length_tot, start_loc, start_heading, ret, length_tot, 0});

	while(!branches.empty())
	{
		auto node = branches.front();
		branches.pop_front();
		node.ret.get().data = gen_branch(node.segment_length,
		                                 stiffness,
		                                 node.length_tot,
		                                 rng,
		                                 node.start_loc,
		                                 node.start_heading,
		                                 topo_info,
		                                 domain_size);

		if(node.depth != max_depth)
		{
			auto& branch = node.ret.get().data;
			auto prev    = branch.back().first;
			auto current = branch.front().first;
			for(size_t k = 1; k != std::size(branch); ++k)
			{
				if(std::bernoulli_distribution{branch_rate}(rng))
				{
					auto const n     = compute_normal(prev, current, branch[k].first);
					auto const theta = std::atan2(n[1], n[0]);
					branches.push_back(BranchParams{
					    segment_length * seg_scale_factor,
					    std::min(length_tot * branch_scale_factor, node.max_seg_length),
					    current,
					    theta,
					    branch[k - 1].second,
					    0.5
					        * std::min(Texpainter::length(prev - current),
					                   Texpainter::length(branch[k].first - current)),
					    node.depth + 1});
				}
				prev    = current;
				current = branch[k].first;
			}
		}
	}
	return ret;
}

void main(auto const& args, auto const& params)
{
	auto const domain_length = std::sqrt(area(args.canvasSize()));

	auto gen_segs = [segment_length = Texpainter::ScalingFactors::sizeScaleFactor(
	                     static_cast<double>(param<Str{"Segment length"}>(params).value())),
	                 stiffness  = static_cast<double>(param<Str{"Stiffness"}>(params).value()),
	                 length_tot = static_cast<double>(param<Str{"Branch length"}>(params).value()
	                                                  * domain_length),
	                 rng        = Rng{args.rngSeed()},
	                 max_depth =
	                     static_cast<size_t>(std::lerp(0.0f,
	                                                   std::nextafter(4.0f, 0.0f),
	                                                   param<Str{"Max depth"}>(params).value())),
	                 seg_scale_factor    = param<Str{"Segment scale factor"}>(params).value(),
	                 branch_scale_factor = param<Str{"Secondary branch length"}>(params).value(),
	                 branch_rate = param<Str{"Branch rate"}>(params).value(),
	                 topo_info = input<1>(args),
	                 domain_size = args.canvasSize()](auto val) mutable {
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
		                             branch_rate,
		                             seg_scale_factor,
		                             branch_scale_factor,
		                             topo_info,
		                             domain_size);
	};

	std::ranges::transform(input<0>(args).get(), std::back_inserter(output<0>(args).get()), gen_segs);
}
```

## Tags

__Id:__ 1a8d90eee0fdedb3e8af0a0981b20130

__Category:__ Converters

__Release state:__ Experimental