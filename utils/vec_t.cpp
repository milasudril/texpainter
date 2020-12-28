//@	{"targets":[{"name":"vec_t.o", "type":"object"}]}

#include "./vec_t.hpp"

Texpainter::vec4_t Texpainter::fromString(Enum::Empty<vec4_t>, std::string const& str)
{
	vec4_t vals{0.0f, 0.0f, 0.0f, 1.0f};
	auto i = str.c_str();
	for(int k = 0; k < 4; ++k)
	{
		char* next;
		auto const val = strtof(i, &next);
		if(i == next) { throw std::string{"String contains non-numerical data"}; }

		if(!isspace(*next) && *next != '\0')
		{ throw std::string{"String contains non-numerical data"}; }

		vals[k] = val;

		while(*next > 0 && *next <= ' ')
		{
			++next;
		}
		if(*next == '\0') { return vals; }

		i = next;
	}
	throw std::string{"String contains too many values"};
}