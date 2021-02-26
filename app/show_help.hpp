//@	{"targets":[{"name":"open_help.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_SHOWHELP_HPP
#define TEXPAINTER_APP_SHOWHELP_HPP

#include "utils/procinfo.hpp"

#include <cstdlib>
#include <string>

namespace Texpainter::Help
{
	inline void showHelp(char const* help_file)
	{
		auto exe = CurrentProcess::executableFilename();
		std::string cmd{"xdg-open "};
		auto helpfile = exe.parent_path().parent_path() / "share/help/C/texpainter";
		helpfile /= help_file;
		cmd += helpfile;
		[[maybe_unused]] auto dummy = system(cmd.c_str());
	}
}

#endif