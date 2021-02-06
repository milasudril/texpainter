# Place RGBA image at points

This image processor places a RGBA image at location deterimed by the input point cloud.

## Input ports

__Locations:__ (Point cloud) The probability distribution.

## Output ports

__Result:__ (RGBA image) The generated image

## Implementation

__Source code:__ 

```c++
void main(auto const& args)
{
	std::ranges::for_each(input<0>(args).get(), [&args](auto pos) {
		output<0>(args, pos.x, pos.y) = RgbaValue{1.0f, 1.0f, 1.0f, 1.0f};
	});
}
```

## Tags

__Id:__ f872b1455c56665160d5faa3e0eae98c

__Category:__ Converters