//@	{
//@	 "targets":[{"name":"procinfo.hpp", "type":"include"}]
//@	}

#include <filesystem>

namespace Texpainter::CurrentProcess
{
	inline std::filesystem::path executableFilename()
	{
		return read_symlink(std::filesystem::path{"/proc/self/exe"});
	}
}