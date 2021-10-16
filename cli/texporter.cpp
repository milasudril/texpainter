//@	 {"targets":[{"name":"texporter", "type":"application"}]}

#include "model/document.hpp"
#include "pixel_store/rgba_image.hpp"
#include "log/logger.hpp"

#include <cstdio>
#include <string_view>
#include <algorithm>
#include <vector>

namespace
{
	constexpr char const* ToFewCmdArgs     = "Too few command line arguments";
	constexpr char const* BadSupersampling = "Please specify a value between 1 and 8, inclusive";
	constexpr char const* TryHelp          = "Try: texporter --help";
}

int main(int argc, char** argv)
{
	try
	{
		Texpainter::Logger::logger(stderr);
		if(argc < 2)
		{
			puts("Try texporter --help");
			return -1;
		}
		std::vector<std::string_view> args{};
		std::ranges::copy(std::span{argv + 1, static_cast<size_t>(argc - 1)},
		                  std::back_inserter(args));

		if(args[0] == "--help")
		{
			puts("Usage: texporter [--supersampling=n] <input> <output>");
			return 0;
		}

		if(std::size(args) < 2) { throw std::runtime_error{ToFewCmdArgs}; }

		uint32_t supersampling = 1u;
		std::string src{args[0]};
		std::string dest{args[1]};
		if(args[0].starts_with("--supersampling="))
		{
			if(std::size(args) < 3) { throw std::runtime_error{ToFewCmdArgs}; }

			if(auto i = std::ranges::find(args[0], '='); i != std::end(args[0]))
			{
				auto const range = std::string{i + 1, std::end(args[0])};
				auto const val   = std::stoll(range);
				if(val < 1 || val > 8) { throw std::runtime_error{BadSupersampling}; }
				supersampling = static_cast<uint32_t>(val);
			}
			else
			{
				throw std::runtime_error{BadSupersampling};
			}

			src  = args[1];
			dest = args[2];
		}

		auto doc = load(Enum::Empty<Texpainter::Model::Document>{}, src.c_str());
		auto img = render(*doc, Texpainter::Model::Document::ForceUpdate{true}, supersampling);
		store(img, dest.c_str());
		return 0;
	}
	catch(char const* e)
	{
		fprintf(stderr, "Texpainter: %s\n", e);
		return -1;
	}
	catch(std::string const& e)
	{
		fprintf(stderr, "Texpainter: %s\n", e.c_str());
		return -1;
	}
	catch(std::exception const& e)
	{
		fprintf(stderr, "Texpainter: %s\n", e.what());
		return -1;
	}
}