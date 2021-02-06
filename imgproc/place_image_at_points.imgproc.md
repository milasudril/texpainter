# Place image at points

This image processor places an image at location deterimed by the input point cloud.

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

__Id:__ 5ecd7079ced4a7aefdc26d174bf3aa42

__Category:__ Converters