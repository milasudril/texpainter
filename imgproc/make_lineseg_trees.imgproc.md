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

__Trunk length:__ (= 0.5)

__Segment length:__ (= 0.5)

__Tree depth:__ (= 0.0)

__Branch rate:__ (= 0.5)

__Level 1 length:__ (= 0.5)

__Level 2 length:__ (= 0.5)

__Level 3 length:__ (= 0.5)

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
	ret.dir_noise             = param<Str{"Direction noise"}>(params).value();
	ret.ext_field_strength    = param<Str{"Ext. field strength"}>(params).value();
	ret.parent_field_strength = param<Str{"Parent field strength"}>(params).value();
	ret.ext_potential         = input<1>(args);

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
	std::array<double, 3> branch_lengths;
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
	return ret;
}

struct BranchParams
{
	SizeParameters size_params;
	vec2_t loc_init;
	vec2_t v0;
	vec2_t parent_field;
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

	return (t_a > 0.0 && t_a < 1.0) && (t_b > 0.0 && t_b < 1.0);
}

inline bool intersect(LineSeg a, std::vector<std::pair<vec2_t, LineSegTree>> const& segs)
{
	if(std::size(segs) <= 1) { return false; }

	auto p0 = segs.front().first;
	for(size_t k = 1; k != std::size(segs); ++k)
	{
		if(intersect(a, LineSeg{p0, segs[k].first})) { return true; }
		p0 = segs[k].first;
	}

	return false;
}

inline bool intersect(LineSeg a, LineSegTree const& tree)
{
	if(intersect(a, tree.data)) { return true; }

	return std::ranges::any_of(tree.data,
	                           [a](auto const& item) { return intersect(a, item.second); });
}

inline auto gen_branch(BranchConstants const& branch_constants,
                       BranchParams const& branch_params,
                       LineSegTree const& tree,
                       Rng& rng)
{
	std::uniform_real_distribution turn{-0.5 * std::numbers::pi, 0.5 * std::numbers::pi};
	auto const length_tot = branch_params.size_params.length_tot;
	auto const seg_length = branch_params.size_params.seg_length;
	std::gamma_distribution seg_length_dist{4.0, length_tot * seg_length};

	auto const length_squared = length_tot * length_tot;
	auto v                    = branch_params.v0;
	auto const w              = branch_constants.domain_size.width();
	auto const h              = branch_constants.domain_size.height();
	auto const ext_potential  = branch_constants.ext_potential;

	auto location = branch_params.loc_init;
	std::vector<std::pair<vec2_t, LineSegTree>> ret{std::pair{location, LineSegTree{}}};

	while(Texpainter::lengthSquared(location - branch_params.loc_init) < length_squared)
	{
		auto const l = std::max(seg_length_dist(rng), 16.0);

		auto const loc_next = location + l * v;
		if(intersect(LineSeg{location, loc_next}, ret)) [[unlikely]]
			{
				return ret;
			}

		if(intersect(LineSeg{location, loc_next}, tree)) { return ret; }

		location = loc_next;
		ret.push_back(std::pair{location, LineSegTree{}});

		auto const random_heading = turn(rng) + std::atan2(v[1], v[0]);
		auto const x              = static_cast<uint32_t>(location[0] + w);
		auto const y              = static_cast<uint32_t>(location[1] + h);
		auto const ext_pot_normal = ext_potential[w * (y % h) + x % w].normal();
		auto const ext_field      = vec2_t{ext_pot_normal[0], ext_pot_normal[1]};

		v = branch_constants.dir_noise * vec2_t{std::cos(random_heading), std::sin(random_heading)}
		    + branch_constants.ext_field_strength * ext_field
		    + branch_constants.parent_field_strength * branch_params.parent_field;
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
				if(std::bernoulli_distribution{branching_params.branch_rate}(rng))
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
					                               * node.branch_params.size_params.length_tot,
					                           .seg_length = branch_params.size_params.seg_length},
					        .loc_init     = current,
					        .v0           = n,
					        .parent_field = n}};
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
		trunk_params.size_params.seg_length = size_params.seg_length;
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

__Release state:__ Experimental