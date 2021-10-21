# Apply laplace operator

This image processor applies the laplace operator on its input

## Input ports

__Input:__ (Grayscale image) Input

## Output ports

__Output:__ (Grayscale image) The laplace operator of Input

## Implementation

__Includes:__ 

```c++
#include <algorithm>
```

__Source code:__ 

```c++
void main(auto const& arg)
{
	auto const size = arg.canvasSize();

	for(uint32_t row = 0; row < size.height(); ++row)
	{
		for(uint32_t col = 0; col < size.width(); ++col)
		{
			auto const ddfx = input<0>(arg, (col + 1 + size.width()) % size.width(), row)
			                  - 2.0f * input<0>(arg, col, row)
			                  + input<0>(arg, (col - 1 + size.width()) % size.width(), row);
			auto const ddfy = input<0>(arg, col, (row + 1 + size.height()) % size.height())
			                  - 2.0f * input<0>(arg, col, row)
			                  + input<0>(arg, col, (row - 1 + size.height()) % size.height());

			output<0>(arg, col, row) = ddfx + ddfy;
		}
	}
}
```

## Tags

__Id:__ 0e3bee3a6cfbaa65abbf5e22ba36658b

__Category:__ Filters

__Release state:__ Stable