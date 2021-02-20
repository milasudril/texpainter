//@	{
//@	 "targets":[{"name":"procinfo.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_UTILS_PROCINFO_HPP
#define TEXPAINTER_UTILS_PROCINFO_HPP

#include <filesystem>

namespace Texpainter::CurrentProcess
{
	inline std::filesystem::path executableFilename()
	{
		return read_symlink(std::filesystem::path{"/proc/self/exe"});
	}
}

#endif