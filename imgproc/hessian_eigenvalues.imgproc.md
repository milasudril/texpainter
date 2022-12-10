# Hessian eigenvalues

This image processor takes a Grayscale image, and computes the eigenvalues of the associated Hessian
matrix. It can be used to classify local extrema.

## Input ports

__Input:__ (Grayscale image) The input image

## Output ports

__λ1:__ (Grayscale image) The first eigenvalue

__λ2:__ (Grayscale image) The second eigenvalue

## Implementation

__Source code:__

```c++
void main(auto const& args)
{
	auto const size = args.canvasSize();

	for(uint32_t row = 1; row != size.height() - 1; ++row)
	{
		for(uint32_t col = 1; col != size.width() - 1; ++col)
		{
			auto const x = col;
			auto const y = row;

			std::array<std::array<float, 3>, 3> const vals{
				std::array<float, 3>{input<0>(args, x - 1u, y - 1u), input<0>(args, x, y - 1u), input<0>(args, x + 1u, y - 1u)},
				std::array<float, 3>{input<0>(args, x - 1u, y),      input<0>(args, x, y),      input<0>(args, x + 1u, y)},
				std::array<float, 3>{input<0>(args, x - 1u, y + 1u), input<0>(args, x, y + 1u), input<0>(args, x + 1u, y + 1u)},
			};

			auto const dxx = vals[1][2] - 2.0f*vals[1][1]  + vals[1][0];
			auto const dyy = vals[2][1] - 2.0f*vals[1][1]  + vals[0][1];

			auto const dxy = 0.25f*(vals[0][0] + vals[2][2] - vals[0][2] - vals[2][0]);

			auto const dd_diff = dxx - dyy;

			auto const r = std::sqrt(dd_diff*dd_diff + 4.0f*dxy*dxy);

			output<0>(args, col, row) = 0.5f*(r + dyy + dxx);
			output<1>(args, col, row) = 0.5f*(-r + dyy + dxx);
		}
	}
}
```

## Tags

__Id:__ 9035fab321620fa05dc617f04ef7eb1d

__Category:__ Filters

__Release state:__ Experimental