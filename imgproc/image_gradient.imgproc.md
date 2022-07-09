# Image gradient

This image processor takes a grayscale image and computes its gradient

## Input ports

__Input:__ (Grayscale image) The input image

## Output ports

__Output:__ (Vector field) The gradient (as a vector field)

## Parameters

__Mode:__ (= 0.0)

## Implementation

__Source code:__

```c++
void periodic(auto const& args)
{
	auto size = args.canvasSize();
	auto const h = size.height();
	auto const w = size.width();
	for(uint32_t row = 0; row != h; ++row)
	{
		for(uint32_t col = 0; col != w; ++col)
		{
			auto const dx = input<0>(args, (col + 1 + w)%w,  row)
				- input<0>(args, (col - 1 + w)%w,  row);
			auto const dy = input<0>(args, col,  (row + 1 + h)%h)
				- input<0>(args, col, (row - 1 + h)%h);
			output<0>(args, col, row) = 0.5*vec2_t{dx, dy};
		}
	}
}

void zero(auto const& args)
{
	auto size = args.canvasSize();
	auto const h = size.height();
	auto const w = size.width();
	for(uint32_t row = 1; row != h - 1; ++row)
	{
		for(uint32_t col = 1; col != w - 1; ++col)
		{
			auto const dx = input<0>(args, col + 1,  row)
				- input<0>(args, col - 1,  row);
			auto const dy = input<0>(args, col,  row + 1)
				- input<0>(args, col, row - 1);
			output<0>(args, col, row) = 0.5*vec2_t{dx, dy};
		}
	}
}

void extend(auto const& args)
{
	auto size = args.canvasSize();
	auto const h = size.height();
	auto const w = size.width();
	for(uint32_t row = 0; row != h; ++row)
	{
		for(uint32_t col = 0; col != w; ++col)
		{
			auto const dx = input<0>(args, std::min(col + 1, w - 1),  row)
				- input<0>(args, std::max(static_cast<int>(col - 1), 0),  row);
			auto const dy = input<0>(args, col,  std::min(row + 1, h - 1))
				- input<0>(args, col, std::max(static_cast<int>(row - 1), 0));
			output<0>(args, col, row) = 0.5*vec2_t{dx, dy};
		}
	}
}

void main(auto const& args, auto const& params)
{
	auto const mode =
	    static_cast<int>(std::lerp(0.0f,
	                               std::nextafter(static_cast<RealValue>(3), 0.0f),
	                               param<Str{"Mode"}>(params).value()));
	switch(mode)
	{
		case 0:
			periodic(args);
			break;
		case 1:
			zero(args);
			break;
		case 2:
			extend(args);
			break;
	}
}
```

## Tags

__Id:__ f95cba837f25f1937364b50b75b18b5d

__Category:__ Converters

__Release state:__ Stable