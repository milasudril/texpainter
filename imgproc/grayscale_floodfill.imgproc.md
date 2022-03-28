# Grayscale floodfill

## Input ports

__Image:__ (Grayscale image) The input image

__Paint args:__ (Grayscale paint args)

## Output ports

__Ouptut:__ (Grayscale image) The output image

## Implementation

__Includes:__

```c++
#include <algorithm>
#include <stack>
```

__Source code:__

```c++
template<class Predicate>
void floodfill4(GrayscalePaintArgs const& init, auto const& args, Predicate&& predicate)
{
	std::stack<ImageCoordinates> nodes;

	auto const w = args.canvasSize().width();
	auto const h = args.canvasSize().height();
	Image<int8_t> visited{w, h};

	auto const f = [p = std::forward<Predicate>(predicate), &visited](auto const& args, uint32_t x, uint32_t y) {
		if(visited(x, y) == 1) { return false; }
		return p(args, x, y);
	};

	nodes.push(init.loc);
	while(!nodes.empty())
	{
		auto const start_pos = nodes.top();
		nodes.pop();
		output<0>(args, start_pos.x, start_pos.y) = init.intensity;
		visited(start_pos.x, start_pos.y) = 1;


//		if(start_pos.y < h - 1 && start_pos.x < w - 1 && f(args, start_pos.x + 1, start_pos.y + 1))
//		{ nodes.push(ImageCoordinates{start_pos.x + 1, start_pos.y + 1}); }

		if(start_pos.y < h - 1 && f(args, start_pos.x, start_pos.y + 1))
		{ nodes.push(ImageCoordinates{start_pos.x, start_pos.y + 1}); }

//		if(start_pos.y < h - 1 && start_pos.x >= 1 && f(args, start_pos.x - 1, start_pos.y + 1))
//		{ nodes.push(ImageCoordinates{start_pos.x - 1, start_pos.y + 1}); }

		if(start_pos.x < w - 1 && f(args, start_pos.x + 1, start_pos.y))
		{ nodes.push(ImageCoordinates{start_pos.x + 1, start_pos.y}); }

		if(start_pos.x >= 1 && f(args, start_pos.x - 1, start_pos.y))
		{ nodes.push(ImageCoordinates{start_pos.x - 1, start_pos.y}); }

//		if(start_pos.y >= 1 && start_pos.x < w - 1 && f(args, start_pos.x + 1, start_pos.y - 1))
//		{ nodes.push(ImageCoordinates{start_pos.x + 1, start_pos.y - 1}); }

		if(start_pos.y >= 1 && f(args, start_pos.x, start_pos.y - 1))
		{ nodes.push(ImageCoordinates{start_pos.x, start_pos.y - 1}); }

//		if(start_pos.y >= 1 && start_pos.x >= 1 && f(args, start_pos.x - 1, start_pos.y - 1))
//		{ nodes.push(ImageCoordinates{start_pos.x - 1, start_pos.y - 1}); }
	}
}

void main(auto const& args)
{
	std::ranges::for_each(input<1>(args).get(), [&args](auto const& val) {
		floodfill4(val, args, [z = val.intensity](auto const& args, uint32_t x, uint32_t y) {
			return input<0>(args, x, y) < z;
		});
	});
}
```

## Tags

__Id:__ fc0cc2ffbad331751a46cc4702e8ea52

__Category:__ Painting ops

__Release state:__ Experimental