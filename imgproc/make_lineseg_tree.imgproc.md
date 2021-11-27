# Make line segment trees

## Input ports

__Input:__ (Point cloud) Input

## Output ports

__Output:__ (Line segment tree) Output image

## Parameters

__Max depth:__ (= 0.0) The number of tree levels

## Implementation

__Source code:__ 

```c++
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