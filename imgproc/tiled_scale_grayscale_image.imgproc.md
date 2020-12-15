# Tiled scale grayscale image

## Input ports

__Input:__ (Grayscale image)

## Output ports

__Output:__ (Grayscale image)

## Parameters

__Scale factor:__ (= 1.0)

__Horz scale factor:__ (= 1.0)

__Vert scale factor:__ (= 1.0)

## Implementation

__Source code:__ 

```c++
inline double mapParameter(ParamValue value) { return std::lerp(-1.0, 1.0, value.value()); }

void main(auto const& args, auto const& params)
{
	using Texpainter::vec2_t;

	auto const w = args.size().width();
	auto const h = args.size().height();

	auto const O = 0.5 * vec2_t{static_cast<double>(w), static_cast<double>(h)};

	auto const r   = mapParameter(param<Str{"Scale factor"}>(params));
	auto const r_x = r * mapParameter(param<Str{"Horz scale factor"}>(params));
	auto const r_y = r * mapParameter(param<Str{"Vert scale factor"}>(params));

	auto const s = vec2_t{r_x, r_y};

	for(uint32_t row = 0; row < h; ++row)
	{
		for(uint32_t col = 0; col < w; ++col)
		{
			auto const loc            = vec2_t{static_cast<double>(col), static_cast<double>(row)};
			auto const src            = O + (loc - O) / s;
			auto const src_x          = static_cast<int32_t>(src[0]);
			auto const src_y          = static_cast<int32_t>(src[1]);
			output<0>(args, col, row) = input<0>(args, (w + src_x) % w, (h + src_y) % h);
		}
	}
}
```

## Tags

__Id:__ ceb75e245816af25b72f454a35aab490

__Category:__ Spatial transformations