//@	{
//@	 "targets":[{"name":"filename_select.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"filename_select.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_FILENAMESELECT_HPP
#define TEXPAINTER_UI_FILENAMESELECT_HPP

#include "./container.hpp"

#include <utility>
#include <filesystem>
#include <memory>

namespace Texpainter::Ui
{
	enum class FilenameSelectMode : int
	{
		Open,
		Save
	};

	using FilenameSelectFilter = bool (*)(void* cb_obj, char const* filename_check);

	bool filenameSelect(Container const& cnt,
	                    std::filesystem::path const& working_dir,
	                    std::filesystem::path& filename_in,
	                    FilenameSelectMode mode,
	                    FilenameSelectFilter cb,
	                    void* cb_obj,
	                    char const* filter_name);

	inline bool filenameSelect(Container const& cnt,
	                           char const* working_dir,
	                           std::filesystem::path& filename_in,
	                           FilenameSelectMode mode)
	{
		auto cb = [](void*, char const*) { return true; };
		return filenameSelect(cnt, working_dir, filename_in, mode, cb, nullptr, "");
	}

	template<class FilterCallback>
	inline bool filenameSelect(Container const& cnt,
	                           std::filesystem::path const& working_dir,
	                           std::filesystem::path& filename_in,
	                           FilenameSelectMode mode,
	                           FilterCallback&& filter,
	                           char const* filter_name)
	{
		auto cb = [](void* cb_obj, char const* filename_check) {
			auto filter = reinterpret_cast<FilterCallback*>(cb_obj);
			return (*filter)(filename_check);
		};
		return filenameSelect(cnt, working_dir, filename_in, mode, cb, &filter, filter_name);
	}
}

#endif