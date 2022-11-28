# Compute curvature

This image processor takes a Grayscale image, and computes the curvature based on intensity values

## Input ports

__Input:__ (Grayscale image) The input image

## Output ports

__Output:__ (Grayscale image) The curvature

## Parameters

__Max elevation:__ (= 0.5) Max elevation in range $\sqrt{A} \cdot \left[2^{-4}, 2^4\right]$, where $A$ is area of the output image. Default value is therefore $\sqrt{A}$.

## Implementation

__Source code:__

```c++
void main(auto const& args, auto const& params)
{
	auto const size = args.canvasSize();
	auto const scale =
	    std::sqrt(static_cast<RealValue>(area(size)))
	    * std::exp2(std::lerp(-4.0f, 4.0f, param<Str{"Max elevation"}>(params).value()))
	    * args.resolution();

	for(uint32_t row = 1; row != size.height() - 1; ++row)
	{
		for(uint32_t col = 1; col != size.width() - 1; ++col)
		{
			auto const x = col;
			auto const y = row;

			auto const dx = scale*0.5f*(input<0>(args, x + 1u, y) - input<0>(args, x - 1u, y));
			auto const dy = scale*0.5f*(input<0>(args, x, y + 1u) - input<0>(args, x, y - 1u));

			auto const dxx = scale*(input<0>(args, x + 1u, y)
			                  - 2.0f * input<0>(args, x, y)
			                  + input<0>(args, x - 1u, y));

			auto const dyy = scale*(input<0>(args, x, y + 1u)
			                  - 2.0f * input<0>(args, x, y)
			                  + input<0>(args, x , y - 1u));

			auto const dxy = scale*0.25f*(
				 input<0>(args, x - 1u, y - 1u)
				+input<0>(args, x + 1u, y + 1u)
				-input<0>(args, x + 1u, y - 1u)
				-input<0>(args, x - 1u, y + 1u)
			);

			auto const d = 1.0f + dx*dx + dy*dy;
			output<0>(args, col, row) = (dxx*dyy - dxy*dxy)/(d*d);
		}
	}
}
```

## Tags

__Id:__ df2a5a18cc0be6ac363fd09b80d5a5ab

__Category:__ Filters

__Release state:__ Experimental