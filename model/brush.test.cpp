//@	{
//@	"targets":[{"name":"brush.test","type":"application", "autorun":1}]
//@	}

#include "./brush.hpp"

#include <cassert>

namespace Testcases
{
	void texpainterBrushFunctionCall()
	{
		Texpainter::Model::BrushFunction brush{Texpainter::Model::BrushType::Square};

		assert((brush(1.0f, Texpainter::vec2_t{0.5, 0.5}) == 1.0f));
		assert((brush(1.0f, Texpainter::vec2_t{1.5, 0.5}) == 0.0f));
		assert((brush(1.0f, Texpainter::vec2_t{0.5, 1.5}) == 0.0f));
		assert((brush(1.0f, Texpainter::vec2_t{0.5, -1.5}) == 0.0f));
	}
}

int main()
{
	Testcases::texpainterBrushFunctionCall();
	return 0;
}
