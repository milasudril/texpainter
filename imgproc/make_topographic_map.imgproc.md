# Make topographic map

This image processor generates a topographic map from a grayscale image. The pixels in a topographic map contains the normal vector, and the elevation $z$. The normal vector, $\vec{n}$, is computed by using the formula

$$ \vec{n}(x, y) = \frac{1}{\sqrt{\left(\partial_x z(x, y)\right)^2 + \left(\partial_y z(x, y)\right)^2 + 1}}\begin{bmatrix}-\partial_x z(x, y)\\ -\partial_y z(x, y)\\ 1.0\end{bmatrix} $$

The elevation $z$ is given by the parameter `Max elevantion`, multiplied by the current pixel value. The derivatives are approximated with the central difference quotient, and boundary conditions are periodic.

## Input ports

__Input:__ (Grayscale image) Grayscale image to be used as input data

## Output ports

__Output:__ (Topography data) Normal vector + elevation as described above

## Parameters

__Max elevation:__ (= 0.5) Max elevation in range $\sqrt{A} \cdot \left[2^{-4}, 2^4\right]$, where $A$ is area of the output image. Default value is therefore $\sqrt{A}$.

## Implementation

__Source code:__ 

```c++
void main(auto const& arg, auto const& params)
{
	auto size    = arg.canvasSize();
	auto const A = area(size);
	auto height  = std::sqrt(static_cast<double>(A))
	              * exp2(std::lerp(-4.0, 4.0, param<Str{"Max elevation"}>(params).value()));
	for(uint32_t row = 0; row < size.height(); ++row)
	{
		for(uint32_t col = 0; col < size.width(); ++col)
		{
			auto const dx = input<0>(arg, (col + 1 + size.width()) % size.width(), row)
			                - input<0>(arg, (col - 1 + size.width()) % size.width(), row);
			auto const dy = input<0>(arg, col, (row + 1 + size.height()) % size.height())
			                - input<0>(arg, col, (row - 1 + size.height()) % size.height());

			output<0>(arg, col, row) = TopographyInfo{
			    static_cast<float>(height * 0.5 * dx),
			    static_cast<float>(height * 0.5 * dy),
			    static_cast<float>(height * input<0>(arg, col, row) / arg.resolution())};
		}
	}
}
```

## Tags

__Id:__ 177ece2ccf47903b91eacaf01b0ebc51

__Category:__ Generators

__Release state:__ Stable