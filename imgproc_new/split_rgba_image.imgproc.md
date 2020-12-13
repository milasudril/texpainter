# Split RGBA image

This image processor takes an RGBA image, and splits it up in four grayscale images.

## Input ports

__Input:__ (RGBA image) The input image

## Output ports

__Red:__ (Grayscale image) The `red` channel

__Green:__ (Grayscale image) The `green` channel

__Blue:__ (Grayscale image) The `blue` channel

__Alpha:__ (Grayscale image) The `alpha` channel

## Implementation

__Source code:__ 

```c++
void main(auto const& args)
{
	auto size = args.size();

	for(uint32_t row = 0; row < size.height(); ++row)
	{
		for(uint32_t col = 0; col < size.width(); ++col)
		{
			output<0>(args, col, row) = input<0>(args, col, row).red();
			output<1>(args, col, row) = input<0>(args, col, row).green();
			output<2>(args, col, row) = input<0>(args, col, row).blue();
			output<3>(args, col, row) = input<0>(args, col, row).alpha();
		}
	}
}
```

## Tags

__Id:__ 369c906fb7cfe8a1e10f5e8012ab5e60

__Category:__ Converters