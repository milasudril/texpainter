# Make line segment trees

## Input ports

__Input:__ (Point cloud)

__Direction field:__ (Topography data)

## Output ports

__Output:__ (Line segment tree)

## Parameters

__Direction noise:__ (= 0.5)

__Ext. field strength:__ (= 0.0)

__Parent field strength:__ (= 0.5)

__Sibling field strength:__ (= 0.0)

__Trunk length:__ (= 0.5)

__Segment length:__ (= 0.5)

__Tree depth:__ (= 0.0)

__Branch rate:__ (= 0.5)

__Level 1 length:__ (= 0.5)

__Level 2 length:__ (= 0.5)

__Level 3 length:__ (= 0.5)

__Level 4 length:__ (= 0.5)

## Implementation

__Includes:__ 

```c++
#include <random>
#include <deque>
```

__Source code:__ 

```c++
struct BranchConstants
{
	BranchConstants(Size2d size): domain_size{size} {}

	double dir_noise;
	double ext_field_strength;
	double parent_field_strength;
	double sibling_field_strength;

	Size2d domain_size;
	TopographyInfo const* ext_potential;
};

inline auto get_branch_constants(auto const& args, auto const& params)
{
	BranchConstants ret{args.canvasSize()};
	ret.dir_noise              = param<Str{"Direction noise"}>(params).value();
	ret.ext_field_strength     = param<Str{"Ext. field strength"}>(params).value();
	ret.parent_field_strength  = param<Str{"Parent field strength"}>(params).value();
	ret.sibling_field_strength = param<Str{"Sibling field strength"}>(params).value();
	ret.ext_potential          = input<1>(args);

	return ret;
}


struct SizeParameters
{
	double length_tot;
	double seg_length;
};

inline auto get_size_params(auto const& params, double domain_length)
{
	SizeParameters ret;

	ret.length_tot = param<Str{"Trunk length"}>(params).value() * domain_length;
	ret.seg_length =
	    Texpainter::ScalingFactors::sizeScaleFactor(param<Str{"Segment length"}>(params).value());
	return ret;
}

struct BranchingParameters
{
	size_t max_depth;
	double branch_rate;
	std::array<double, 4> branch_lengths;
};

inline auto get_branching_parameters(auto const& params)
{
	BranchingParameters ret;
	ret.max_depth = static_cast<size_t>(
	    std::lerp(0.0f, std::nextafter(4.0f, 0.0f), param<Str{"Tree depth"}>(params).value()));
	ret.branch_rate       = param<Str{"Branch rate"}>(params).value();
	ret.branch_lengths[0] = param<Str{"Level 1 length"}>(params).value();
	ret.branch_lengths[1] = param<Str{"Level 2 length"}>(params).value();
	ret.branch_lengths[2] = param<Str{"Level 3 length"}>(params).value();
	ret.branch_lengths[3] = param<Str{"Level 4 length"}>(params).value();

	return ret;
}

struct SiblingField
{
	std::span<std::pair<vec2_t, LineSegTree> const> line_segs;
	double size;
};

struct BranchParams
{
	SizeParameters size_params;
	vec2_t loc_init;
	vec2_t v0;
	vec2_t parent_field;
	SiblingField sibling_field;
};


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


inline auto gen_branch(BranchConstants const& branch_constants,
                       BranchParams const& branch_params,
                       Rng& rng)
{
	std::uniform_real_distribution turn{-0.5 * std::numbers::pi, 0.5 * std::numbers::pi};
	auto const length_tot = branch_params.size_params.length_tot;
	auto const seg_length = branch_params.size_params.seg_length;
	std::gamma_distribution seg_length_dist{3.0, length_tot * seg_length};

	auto const length_squared       = length_tot * length_tot;
	auto v                          = branch_params.v0;
	auto const w                    = branch_constants.domain_size.width();
	auto const h                    = branch_constants.domain_size.height();
	auto const ext_potential        = branch_constants.ext_potential;
	auto const sibling_field_params = branch_params.sibling_field;

	auto location = branch_params.loc_init;
	std::vector<std::pair<vec2_t, LineSegTree>> ret{std::pair{location, LineSegTree{}}};

	while(Texpainter::lengthSquared(location - branch_params.loc_init) < length_squared)
	{
		auto const random_heading = turn(rng);
		auto const l              = std::max(seg_length_dist(rng), 16.0);
		location += l * v / Texpainter::length(v);

		auto const x              = static_cast<uint32_t>(location[0] + w);
		auto const y              = static_cast<uint32_t>(location[1] + h);
		auto const ext_pot_normal = ext_potential[w * (y % h) + x % w].normal();
		auto const ext_field      = vec2_t{ext_pot_normal[0], ext_pot_normal[1]};

		auto const sibling_seg = std::size(sibling_field_params.line_segs) >= 2
		                             ? closest_seg_dist(sibling_field_params.line_segs, location)
		                             : std::pair{vec2_t{0.0, 0.0}, 0.0};
		auto const sibling_field = eval_sibling_field(sibling_seg, sibling_field_params.size);

		v += branch_constants.dir_noise * vec2_t{std::cos(random_heading), std::sin(random_heading)}
		     + branch_constants.ext_field_strength * ext_field
		     + branch_constants.parent_field_strength * branch_params.parent_field
		     + branch_constants.sibling_field_strength * sibling_field;
	}

	return ret;
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

struct Node
{
	double side;
	size_t index;
	std::reference_wrapper<LineSegTree> ret;
	size_t depth;
	BranchParams branch_params;
};

inline LineSegTree gen_line_segment_tree(BranchConstants const& branch_constants,
                                         BranchingParameters const& branching_params,
                                         BranchParams const& branch_params,
                                         Rng& rng)
{
	LineSegTree ret;
	std::deque<Node> pending_branches;
	pending_branches.push_back(Node{-1.0, 0, ret, 0, branch_params});

	while(!pending_branches.empty())
	{
		auto const node = pending_branches.front();
		pending_branches.pop_front();
		node.ret.get().data = gen_branch(branch_constants, node.branch_params, rng);

		if(node.depth != branching_params.max_depth) {}
	}

	/*
	std::deque<BranchParams> branches;
	branches.push_back(BranchParams{
	    segment_length, length_tot, start_loc, start_heading, -1.0, 0, ret, length_tot, 0});

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
					    k,
					    branch[k - 1].second,
					    0.5
					        * std::min(Texpainter::length(prev - current),
					                   Texpainter::length(next - current)),
					    node.depth + 1});
				}
				prev    = current;
				current = next;
			}

			std::ranges::sort(next_set, [](auto const& a, auto const& b) {
				if(a.side == b.side) { return a.index < b.index; }
				return a.side < b.side;
			});
			std::ranges::copy(next_set, std::back_inserter(branches));
		}
	}
	*/
	return ret;
}

void main(auto const& args, auto const& params)
{
	auto const branch_constants = get_branch_constants(args, params);
	auto const domain_length    = std::sqrt(area(args.canvasSize()));
	auto const size_params      = get_size_params(params, domain_length);
	auto const branching_params = get_branching_parameters(params);

	auto gen_segs = [branch_constants, size_params, branching_params, rng = Rng{args.rngSeed()}](
	                    auto const& item) mutable {
		auto const loc_vec =
		    vec2_t{static_cast<double>(item.loc.x), static_cast<double>(item.loc.y)};
		auto const start_heading = item.rot.radians();
		auto const length_scale  = static_cast<double>(item.scale);

		BranchParams trunk_params{};
		trunk_params.size_params.length_tot = length_scale * size_params.length_tot;
		trunk_params.size_params.seg_length = size_params.seg_length;
		trunk_params.loc_init               = loc_vec;
		trunk_params.v0           = vec2_t{std::cos(start_heading), std::sin(start_heading)};
		trunk_params.parent_field = trunk_params.v0;

		return gen_line_segment_tree(branch_constants, branching_params, trunk_params, rng);
	};

	std::ranges::transform(
	    input<0>(args).get(), std::back_inserter(output<0>(args).get()), gen_segs);


	/*

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
*/
}
```

## Tags

__Id:__ 1a8d90eee0fdedb3e8af0a0981b20130

__Category:__ Converters

__Release state:__ Experimental