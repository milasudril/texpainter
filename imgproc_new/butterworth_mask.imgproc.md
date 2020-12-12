# Butterworth mask

This image processor generates a Butterworth mask, that can be used for lowpass-filtering image data. The mask is computed usinig the formula

$$ f\left(\vec{r}\right) = \frac{1}{\sqrt{1 + \left(\vec{r}\cdot\vec{r}\right)^n}} $$

where

$$\vec{r} = \begin{bmatrix} \frac{1}{s} & 0 \\ 0 & \frac{1}{s \sigma} \end{bmatrix}R(\theta)((x, y) - O)$$

## Input ports

This image processor has no input ports.

## Output ports

__Intensity:__ (Grayscale image) The generated intensity function

## Parameters

__Size:__ (= 0.5) The size of the mask, along the nominal x axis.

__Aspect ratio:__ (= 1.0) The aspect ratio of mask. 1.0 means that the mask is circular.

__Order:__ (= 0.0) The order $n$. 0.0 maps to 1 and 1.0 maps to 4.

__Orientation:__ (= 0.0) Orientation of the mask. 1.0 maps to π.

## Implementation

__Includes:__ 

```c++
#include "utils/angle.hpp"

#include <cmath>
```

__Source code:__ 

```c++
inline auto sizeFromParam(size_t size, ParamValue val)
{
	return 0.5 * std::exp2(std::lerp(-std::log2(size), 0.0, val.value()));
}

inline auto orderFromParam(ParamValue val)
{
	return static_cast<int>(std::nextafter(4.0, 0.0) * val.value()) + 1;
}

void main(auto const& args, auto const& params)
{
	using Texpainter::Angle;
	using Texpainter::vec2_t;

	auto const size = args.size();

	auto const r_x =
	    sizeFromParam(static_cast<size_t>(sqrt(size.area())), param<Str{"Size"}>(params));
	auto const r_y = r_x * param<Str{"Aspect ratio"}>(params).value();
	auto const n   = orderFromParam(param<Str{"Order"}>(params));
	auto const θ   = Angle{0.5 * param<Str{"Orientation"}>(params).value(), Angle::Turns{}};

	auto const rot_vec_x = vec2_t{cos(θ), -sin(θ)};
	auto const rot_vec_y = vec2_t{sin(θ), cos(θ)};

	auto const O =
	    0.5 * vec2_t{static_cast<double>(size.width()), static_cast<double>(size.height())};
	auto const r_0 = sqrt(size.area()) * vec2_t{r_x, r_y};

	for(uint32_t row = 0; row < size.height(); ++row)
	{
		for(uint32_t col = 0; col < size.width(); ++col)
		{
			auto P                    = vec2_t{static_cast<double>(col), static_cast<double>(row)};
			auto r                    = Texpainter::transform(P - O, rot_vec_x, rot_vec_y) / r_0;
			output<0>(args, col, row) = 1.0 / sqrt(1 + std::pow(Texpainter::dot(r, r), n));
		}
	}
}
```

## Tags

__Id:__ c0627175652da7b51518e7bf454e5205

__Category:__ Generators