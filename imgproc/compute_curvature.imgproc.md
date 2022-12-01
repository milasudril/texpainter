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
	auto const scale = static_cast<double>(
	    std::sqrt(static_cast<RealValue>(area(size)))
	    * std::exp2(std::lerp(-4.0f, 4.0f, param<Str{"Max elevation"}>(params).value()))
	    * args.resolution());

	auto const w = size.width();
	auto const h = size.height();

	for(uint32_t row = 0; row != h; ++row)
	{
		for(uint32_t col = 0; col != w; ++col)
		{
			auto const x = col;
			auto const y = row;

			std::array<std::array<double, 3>, 3> const vals{
				std::array<double, 3>{input<0>(args, (x - 1 + w)%w, (y - 1 + h)%h), input<0>(args, x, (y - 1 + h)%h), input<0>(args, (x + 1 + w)%w, (y - 1 + h)%h)},
				std::array<double, 3>{input<0>(args, (x - 1 + w)%w, y),      input<0>(args, x, y),      input<0>(args, (x + 1 + w)%w, y)},
				std::array<double, 3>{input<0>(args, (x - 1 + w)%w, (y + 1 + h)%h), input<0>(args, x, (y + 1 + h)%h), input<0>(args, (x + 1 + w)%w, (y + 1 + h)%h)},
			};

			auto const dx = scale*0.5*(vals[1][2] - vals[1][0]);
			auto const dy = scale*0.5*(vals[2][1] - vals[0][1]);

			auto const dxx = scale*(vals[1][2] - 2.0*vals[1][1] + vals[1][0]);
			auto const dyy = scale*(vals[2][1] - 2.0*vals[1][1] + vals[0][1]);

			auto const dxy = scale*0.25*(vals[0][0] + vals[2][2] - vals[0][2] - vals[2][0]);

			auto const d = 1.0 + dx*dx + dy*dy;
			output<0>(args, col, row) = static_cast<float>( (dxx*dyy - dxy*dxy)/(d*d) );
		}
	}
}
```

## Tags

__Id:__ df2a5a18cc0be6ac363fd09b80d5a5ab

__Category:__ Edge detection

__Release state:__ Stable