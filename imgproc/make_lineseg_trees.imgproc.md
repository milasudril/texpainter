# Make line segment trees

## Input ports

__Starting points:__ (Point cloud) The points where to start drawing a new tree

__Direction field:__ (Topography data) This input controls the direction of trajectories. The trajectories will approach the  projection of the normal vector on the xy-plane.

## Output ports

__Output:__ (Line segment tree)

## Parameters

__Direction noise:__ (= 0.5) The amout of randomness in the trajectory direction

__Ext. field strength:__ (= 0.0) The influence of `Direction field` on trajectory directions

__Parent field strength:__ (= 0.5) The influence of the parent normal on trajectory directions

__Smoothness:__ (= 0.5) How smooth trajectories are

__Trunk length:__ (= 0.5) The length of trunk or branch level 0

__Segment length:__ (= 0.5) The length of segments relative to the current branch. The scale is logarithmic between 1/64 and 1, with 0.5 being mapped to 1/8.

__Collision margin:__ (= 0.0) How mutch space there has to be between a line segment endpoint and any other line segment

__Tree depth:__ (= 0.0) The maximum tree depth. 0.0 means that only trunks are created. 1.0 will result in a total of four levels, including the trunk.

__Branch rate:__ (= 0.5) The probablity to spawn a new branch, measured per line segment. Thus, reducing the segment length will increase the number of branches.

__Allow branch on first:__ (= 0.0) Set to greater than 0.5 to allow first vertex to have a branch

__Level 1 scale:__ (= 0.5) How mutch to scale the first branch relative to the length of the trunk

__Level 2 scale:__ (= 0.5) How mutch to scale the first branch relative to the length of level 1

__Level 3 scale:__ (= 0.5) How mutch to scale the first branch relative to the length of level 2

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
	double smoothness;
	double seg_length;
	double line_seg_margin;

	Size2d domain_size;
	TopographyInfo const* ext_potential;
	bool allow_branch_on_first;
};

inline auto get_branch_constants(auto const& args, auto const& params)
{
	BranchConstants ret{args.canvasSize()};
	ret.dir_noise             = param<Str{"Direction noise"}>(params).value();
	ret.ext_field_strength    = param<Str{"Ext. field strength"}>(params).value();
	ret.parent_field_strength = param<Str{"Parent field strength"}>(params).value();
	ret.smoothness            = param<Str{"Smoothness"}>(params).value();
	ret.seg_length =
	    std::exp2(std::lerp(-6.0f, 0.0f, param<Str{"Segment length"}>(params).value()));
	ret.line_seg_margin       = param<Str{"Collision margin"}>(params).value();
	ret.ext_potential         = input<1>(args);
	ret.allow_branch_on_first = param<Str{"Allow branch on first"}>(params).value() >= 0.5;

	return ret;
}


struct SizeParameters
{
	double length_tot;
};

inline auto get_size_params(auto const& params, double domain_length)
{
	SizeParameters ret;
	ret.length_tot = param<Str{"Trunk length"}>(params).value() * domain_length;
	return ret;
}

struct BranchingParameters
{
	size_t max_depth;
	double branch_rate;
	std::array<double, 3> branch_lengths;
};

inline auto get_branching_parameters(auto const& params)
{
	BranchingParameters ret;
	ret.max_depth = static_cast<size_t>(
	    std::lerp(0.0f, std::nextafter(4.0f, 0.0f), param<Str{"Tree depth"}>(params).value()));
	ret.branch_rate       = param<Str{"Branch rate"}>(params).value();
	ret.branch_lengths[0] = param<Str{"Level 1 scale"}>(params).value();
	ret.branch_lengths[1] = param<Str{"Level 2 scale"}>(params).value();
	ret.branch_lengths[2] = param<Str{"Level 3 scale"}>(params).value();
	return ret;
}

struct BranchParams
{
	SizeParameters size_params;
	vec2_t loc_init;
	vec2_t v0;
	vec2_t parent_field;
	std::optional<std::pair<vec2_t, vec2_t>> neighbours;
};


inline auto compute_normal(vec2_t prev, vec2_t next)
{
	auto const v = Texpainter::normalize(next - prev);
	return vec2_t{v[1], -v[0]};
}

struct LineSeg
{
	vec2_t p1;
	vec2_t p2;
};

bool operator==(LineSeg a, LineSeg b)
{
	auto const var_a =
	    a.p1[0] == b.p1[0] && a.p1[1] == b.p1[1] && a.p2[0] == b.p2[0] && a.p2[1] == b.p2[1];

	auto const var_b =
	    a.p1[0] == b.p2[0] && a.p1[1] == b.p2[1] && a.p2[0] == b.p1[0] && a.p2[1] == b.p1[1];

	return var_a || var_b;
}

inline bool intersect(LineSeg a, LineSeg b)
{
	auto const dir_a = a.p2 - a.p1;
	auto const dir_b = b.p2 - b.p1;

	auto const det = dir_a[0] * dir_b[1] - dir_a[1] * dir_b[0];

	if(det == 0.0) [[unlikely]]
		{
			return false;
		}

	auto const t_a = ((b.p1[0] - a.p1[0]) * dir_b[1] + (a.p1[1] - b.p1[1]) * dir_b[0]) / det;
	auto const t_b = ((b.p1[0] - a.p1[0]) * dir_a[1] + (a.p1[1] - b.p1[1]) * dir_a[0]) / det;

	return (t_a >= 0 && t_a <= 1.0) && (t_b >= 0.0 && t_b <= 1.0);
}

template<class Filter>
inline bool intersect(LineSeg a, std::span<std::pair<vec2_t, LineSegTree> const> segs, Filter f)
{
	if(std::size(segs) <= 1) { return false; }

	auto p0 = segs.front().first;
	for(size_t k = 1; k != std::size(segs); ++k)
	{
		auto const seg = LineSeg{p0, segs[k].first};
		if(f(seg) && intersect(a, seg)) { return true; }
		p0 = segs[k].first;
	}

	return false;
}

template<class Filter>
inline bool intersect(LineSeg a, LineSegTree const& tree, Filter filter)
{
	if(intersect(a, tree.data, filter)) { return true; }

	return std::ranges::any_of(tree.data, [a, f = std::forward<Filter>(filter)](auto const& item) {
		return intersect(a, item.second, f);
	});
}

inline bool intersect(LineSeg seg,
                      std::span<std::pair<vec2_t, LineSegTree> const> current_branch,
                      LineSegTree const& tree,
                      std::optional<std::pair<vec2_t, vec2_t>> neighbours)
{
	if(intersect(seg, current_branch, [](auto const&) { return true; })) [[unlikely]]
		{
			return true;
		}

	if(neighbours.has_value() && std::size(current_branch) == 0)
	{
		auto const location = seg.p1;
		return intersect(
		    seg,
		    tree,
		    [linesegs = std::pair{LineSeg{neighbours->first, location},
		                          LineSeg{location, neighbours->second}}](auto const& seg) {
			    return seg != linesegs.first && seg != linesegs.second;
		    });
	}

	return intersect(seg, tree, [](auto const&) { return true; });
}

inline auto gen_branch(BranchConstants const& branch_constants,
                       BranchParams const& branch_params,
                       LineSegTree const& tree,
                       Rng& rng)
{
	std::uniform_real_distribution turn{-0.5 * std::numbers::pi, 0.5 * std::numbers::pi};
	auto const length_tot = branch_params.size_params.length_tot;

	auto const length_squared  = length_tot * length_tot;
	auto v                     = branch_params.v0;
	auto const w               = branch_constants.domain_size.width();
	auto const h               = branch_constants.domain_size.height();
	auto const ext_potential   = branch_constants.ext_potential;
	auto const line_seg_margin = branch_constants.line_seg_margin;
	auto const smoothness      = branch_constants.smoothness;

	auto location = branch_params.loc_init;
	std::vector<std::pair<vec2_t, LineSegTree>> ret{std::pair{location, LineSegTree{}}};
	auto const l = length_tot * branch_constants.seg_length;

	while(Texpainter::lengthSquared(location - branch_params.loc_init) < length_squared)
	{
		auto const loc_next      = location + l * v;
		auto const loc_next_test = loc_next + length_tot * v * line_seg_margin;

		if(intersect(LineSeg{location, loc_next_test},
		             std::span{std::data(ret), std::size(ret) - 1},
		             tree,
		             branch_params.neighbours))
		{ return ret; }

		location = loc_next;
		ret.push_back(std::pair{location, LineSegTree{}});

		auto const random_heading = turn(rng) + std::atan2(v[1], v[0]);
		auto const x              = static_cast<uint32_t>(location[0] + w);
		auto const y              = static_cast<uint32_t>(location[1] + h);
		auto const ext_pot_normal = ext_potential[w * (y % h) + x % w].normal();
		auto const ext_field      = vec2_t{ext_pot_normal[0], ext_pot_normal[1]};

		auto const v_new =
		    branch_constants.dir_noise * vec2_t{std::cos(random_heading), std::sin(random_heading)}
		    + branch_constants.ext_field_strength * ext_field
		    + branch_constants.parent_field_strength * branch_params.parent_field;

		v = (1.0 - smoothness) * v_new + v * smoothness;

		v /= Texpainter::length(v);
	}

	return ret;
}

struct Node
{
	double side;
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
	pending_branches.push_back(Node{-1.0, ret, 0, branch_params});

	while(!pending_branches.empty())
	{
		auto const node = pending_branches.front();
		pending_branches.pop_front();
		node.ret.get().data = gen_branch(branch_constants, node.branch_params, ret, rng);

		if(node.depth != branching_params.max_depth)
		{
			auto& branch = node.ret.get().data;
			auto prev    = branch.back().first;
			auto current = branch.front().first;

			for(size_t k = 1; k != std::size(branch); ++k)
			{
				auto const next = branch[k].first;
				auto const branch_res =
				    std::bernoulli_distribution{branching_params.branch_rate}(rng);
				if(branch_res
				   && ((branch_constants.allow_branch_on_first && node.depth == 0) || k != 1))
				{
					auto n = compute_normal(prev, next);

					auto const side = [](vec2_t a, vec2_t b, Rng& rng) {
						auto const proj = Texpainter::dot(a, b);
						if(proj < 0.0) { return -1.0; }
						if(proj > 0.0) { return 1.0; }
						return std::bernoulli_distribution{0.5}(rng) ? -1.0 : 1.0;
					}(current - prev, n, rng);

					n *= side;

					Node new_node{
					    .side          = side,
					    .ret           = branch[k - 1].second,
					    .depth         = node.depth + 1,
					    .branch_params = BranchParams{
					        .size_params =
					            SizeParameters{.length_tot =
					                               branching_params.branch_lengths[node.depth]
					                               * node.branch_params.size_params.length_tot},
					        .loc_init     = current,
					        .v0           = n,
					        .parent_field = n,
					        .neighbours   = std::pair{prev, next}}};
					pending_branches.push_back(new_node);
				}
				prev    = current;
				current = next;
			}
		}
	}
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
		trunk_params.loc_init               = loc_vec;
		trunk_params.v0           = vec2_t{std::cos(start_heading), std::sin(start_heading)};
		trunk_params.parent_field = trunk_params.v0;

		return gen_line_segment_tree(branch_constants, branching_params, trunk_params, rng);
	};

	std::ranges::transform(
	    input<0>(args).get(), std::back_inserter(output<0>(args).get()), gen_segs);
}
```

## Tags

__Id:__ 1a8d90eee0fdedb3e8af0a0981b20130

__Category:__ Converters

__Release state:__ Stable