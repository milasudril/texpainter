# Real constant

This image processor generates a real, constant value.

## Output ports

__Output:__ (Grayscale image) A grayscale image whos value is set to `Value`

## Parameters

__Value:__ (= 0.0) The output value

## Implementation

__Source code:__ 

```c++
void main(auto const& args, auto const& params)
{
	auto const size = area(args.canvasSize());
	std::fill(output<0>(args),
	          output<0>(args) + size,
	          static_cast<float>(param<Str{"Value"}>(params).value()));
}
```

## Tags

__Id:__ 85034d41dbd2669203e0b5046faa7174

__Category:__ Generators