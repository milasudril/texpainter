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
inline auto compute_normal(vec2_t prev, vec2_t next)
{
	auto const v = Texpainter::normalize(next - prev);
	return vec2_t{v[1], -v[0]};
}

inline std::pair<vec2_t, double> closest_seg_dist(
    std::span<std::pair<vec2_t, LineSegTree> const> line_segs, vec2_t loc)
{
	auto r0 = line_segs[0].first;
	auto r  = line_segs[1].first;

	auto n0 = compute_normal(r0, r);
	auto d0 = Texpainter::dot(loc - r0, n0);
	r0      = r;

	for(size_t k = 2; k != std::size(line_segs); ++k)
	{
		r            = line_segs[k].first;
		auto const n = compute_normal(r0, r);
		auto const d = Texpainter::dot(loc - r0, n);
		if(d < d0)
		{
			n0 = n;
			d0 = d;
		}
		r0 = r;
	}

	return std::pair{n0, d0};
}

inline vec2_t eval_sibling_field(std::pair<vec2_t, double> dir_distance, double size)
{
	return dir_distance.first * std::exp2(-dir_distance.second / size);
}

struct ExtFeild
{
	TopographyInfo const* ext_field;
	Size2d domain_size;
};

struct SiblingField
{
	std::span<std::pair<vec2_t, LineSegTree> const> line_segs;
	double size;
};

struct BranchingConstants
{
	double dir_randomization;
	double ext_field_strength;
	double init_field_strength;
	double sibling_field_strength;

	Size2d domain_size;
	TopographyInfo const* ext_potential;
};

struct BranchParams2
{
	double length_tot;
	double seg_length;
	vec2_t loc_init;
	vec2_t v0;
	vec2_t init_field;
	SiblingField sibling_field;
};

inline auto gen_branch(BranchingConstants const& branching_constants,
                       BranchParams2 const& branch_params,
                       Rng& rng)
{
	std::uniform_real_distribution turn{-0.5 * std::numbers::pi, 0.5 * std::numbers::pi};
	std::gamma_distribution seg_length{3.0, branch_params.length_tot * branch_params.seg_length};

	auto const length_squared       = branch_params.length_tot * branch_params.length_tot;
	auto v                          = branch_params.v0;
	auto const w                    = branching_constants.domain_size.width();
	auto const h                    = branching_constants.domain_size.height();
	auto const ext_potential        = branching_constants.ext_potential;
	auto const sibling_field_params = branch_params.sibling_field;

	auto location = branch_params.loc_init;
	std::vector<std::pair<vec2_t, LineSegTree>> ret{std::pair{location, LineSegTree{}}};

	while(Texpainter::lengthSquared(location - branch_params.loc_init) < length_squared)
	{
		auto const random_heading = turn(rng);
		auto const l              = std::max(seg_length(rng), 16.0);
		location += l * v / Texpainter::length(v);

		auto const x              = static_cast<uint32_t>(location[0] + w);
		auto const y              = static_cast<uint32_t>(location[1] + h);
		auto const ext_pot_normal = ext_potential[w * (y % h) + x % w].normal();
		auto const ext_field      = vec2_t{ext_pot_normal[0], ext_pot_normal[1]};

		auto const sibling_seg   = closest_seg_dist(sibling_field_params.line_segs, location);
		auto const sibling_field = eval_sibling_field(sibling_seg, sibling_field_params.size);

		v += branching_constants.dir_randomization
		         * vec2_t{std::cos(random_heading), std::sin(random_heading)}
		     + branching_constants.ext_field_strength * ext_field
		     + branching_constants.init_field_strength * branch_params.init_field
		     + branching_constants.sibling_field_strength * sibling_field;
	}
}

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
	auto const w   = domain_size.width();
	auto const h   = domain_size.height();
	while(Texpainter::lengthSquared(location - l0) < lsq)
	{
		auto const l         = std::max(seg_length(rng), 16.0);
		auto const x         = static_cast<uint32_t>(location[0] + w);
		auto const y         = static_cast<uint32_t>(location[1] + h);
		auto const field     = topo_info[w * (y % h) + x % w].normal();
		auto const grad      = vec2_t{field[0], field[1]};
		auto const t         = Texpainter::length(grad);
		auto const noise_dir = vec2_t{std::cos(heading), std::sin(heading)};
		location += l * ((1.0 - t) * noise_dir + t * grad);
		heading += turn(rng);
		heading += stiffness * (h0 - heading);

		ret.push_back(std::pair{location, LineSegTree{}});
	}

	return ret;
}

struct BranchParams
{
	double segment_length;
	double length_tot;
	vec2_t start_loc;
	double start_heading;
	double side;
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
	branches.push_back(BranchParams{
	    segment_length, length_tot, start_loc, start_heading, -1.0, ret, length_tot, 0});

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

			std::vector<BranchParams> next_set;
			next_set.reserve(std::size(branch));
			for(size_t k = 1; k != std::size(branch); ++k)
			{
				auto const next = branch[k].first;
				if(std::bernoulli_distribution{branch_rate}(rng))
				{
					auto n          = compute_normal(prev, next);
					auto const side = Texpainter::dot(current - prev, n) < 0.0 ? -1.0 : 1.0;
					n *= side;
					auto const theta = std::atan2(n[1], n[0]);
					next_set.push_back(BranchParams{
					    segment_length * seg_scale_factor,
					    std::min(length_tot * branch_scale_factor, node.max_seg_length),
					    current,
					    theta,
					    side,
					    branch[k - 1].second,
					    0.5
					        * std::min(Texpainter::length(prev - current),
					                   Texpainter::length(next - current)),
					    node.depth + 1});
				}
				prev    = current;
				current = next;
			}

			std::ranges::sort(next_set,
			                  [](auto const& a, auto const& b) { return a.side < b.side; });
			std::ranges::copy(next_set, std::back_inserter(branches));
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
	                 branch_rate         = param<Str{"Branch rate"}>(params).value(),
	                 topo_info           = input<1>(args),
	                 domain_size         = args.canvasSize()](auto val) mutable {
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

	std::ranges::transform(
	    input<0>(args).get(), std::back_inserter(output<0>(args).get()), gen_segs);
}
```

## Tags

__Id:__ 1a8d90eee0fdedb3e8af0a0981b20130

__Category:__ Converters

__Release state:__ Experimental