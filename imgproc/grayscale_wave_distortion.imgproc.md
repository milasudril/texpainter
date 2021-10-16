# Grayscale wave distortion

This image processor takes a grayscale image, and transforms its pixel values so they are wrapped periodically. The pixel-wise transformation rule is

$$ g(z, y) = f\left(\nu \left( z(x, y) + \phi\right)\right) $$

where $f$ is periodic.

## Input ports

__Input:__ (Grayscale image) The input image

## Output ports

__Ouptut:__ (Grayscale image) The output image

## Parameters

__Waveform:__ (= 0.0) Sine, Sawtooth, Square, Triangle

__Wavelength:__ (= 0.86666667) Wavelength. The default value is set such that it results in a wavelength of $1/8$ of the image size.

__Phase:__ (= 0.0) Phase

__Add DC offset:__ (= 1.0) Whether or not to add a DC offset to make sure that output values are between 0 and 1

## Implementation

__Includes:__ 

```c++
#include <numbers>
```

__Source code:__ 

```c++
inline RealValue sin(RealValue val) { return std::sin(val * std::numbers::pi_v<RealValue>); }

inline RealValue sawtooth(RealValue val)
{
	return 2.0f * (0.5f * val - std::floor(0.5f * val + 0.5f));
}

inline RealValue sgn(RealValue val) { return val < 0.0f ? -1.0f : 1.0f; }

inline RealValue square(RealValue val) { return sgn(sawtooth(val)); }

inline RealValue triangle(RealValue val) { return 2.0f * std::abs(sawtooth(val)) - 1.0f; }

using Function = RealValue (*)(RealValue val);

constexpr Function functions[] = {sin, sawtooth, square, triangle};

inline int waveformIndex(ParamValue val)
{
	return static_cast<int>(std::lerp(
	    0.0f, std::nextafter(static_cast<float>(std::size(functions)), 0.0f), val.value()));
}

inline auto wavelength(Size2d canvas_size, ParamValue val)
{
	return std::max(2.0f / static_cast<float>(std::min(canvas_size.width(), canvas_size.height())),
	                sizeScaleFactor(val));
}

void main(auto const& args, auto const& params)
{
	auto const waveform  = waveformIndex(param<Str{"Waveform"}>(params));
	auto const f         = 1.0f / wavelength(args.canvasSize(), param<Str{"Wavelength"}>(params));
	auto const phase     = param<Str{"Phase"}>(params).value();
	auto const dc_offset = param<Str{"Add DC offset"}>(params).value();

	std::transform(input<0>(args),
	               input<0>(args) + area(args.canvasSize()),
	               output<0>(args),
	               [func = functions[waveform], f, phase, dc_offset](auto val) {
		               auto z = func(f * (val - phase));
		               return std::lerp(z, 0.5f * (z + 1.0f), dc_offset);
	               });
}
```

## Tags

__Id:__ 59606c2e602c9b1c6e2d732b9c5d7bec

__Category:__ Dynamic range adjustments

__Release state:__ Stable