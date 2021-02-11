//@	{"targets":[{"name":"render_job_creator.hpp", "type":"include"}]}

#ifndef TEXPAINTER_APP_RENDERJOBCREATOR_HPP
#define TEXPAINTER_APP_RENDERJOBCREATOR_HPP

#include "./render_options_input.hpp"

#include "ui/button.hpp"
#include "ui/text_entry.hpp"
#include "ui/filename_select.hpp"
#include "ui/error_message_dialog.hpp"

namespace Texpainter::App
{
	class RenderJobCreator
	{
		enum class ControlId : int
		{
			Filename
		};

	public:
		struct Options
		{
			std::filesystem::path filename;
			uint32_t supersampling;
		};

		RenderJobCreator(RenderJobCreator&&) = delete;

		explicit RenderJobCreator(Ui::Container& container)
		    : m_root{container, Ui::Box::Orientation::Vertical}
		    , m_filename{m_root,
		                 Ui::Box::Orientation::Horizontal,
		                 "Filename: ",
		                 Ui::Box::Orientation::Horizontal,
		                 "Browse..."}
		    , m_render_opts{m_root, Ui::Box::Orientation::Vertical, "Render options:"}
		{
			{
				auto& inner = m_filename.inputField();
				inner.label().eventHandler<ControlId::Filename>(*this);
			}
		}

		Options value() const
		{
			return Options{m_filename.inputField().inputField().content(),
			               m_render_opts.inputField().value()};
		}

		template<auto>
		void onClicked(Ui::Button& src)
		{
			std::filesystem::path name;
			if(Ui::filenameSelect(
			       m_root,
			       std::filesystem::path{"."},
			       name,
			       Ui::FilenameSelectMode::Save,
			       [](char const*) { return true; },
			       "Exr files"))
			{
				auto& inner = m_filename.inputField();
				inner.inputField().content(name.c_str());
			}
			src.state(false);
		}

		template<auto, class T>
		void handleException(char const* msg, T&)
		{
			m_err_box.show(m_root, "Texpainter: Exporting image", msg);
		}

	private:
		Ui::Box m_root;
		Ui::LabeledInput<Ui::ReversedLabeledInput<Ui::TextEntry, Ui::Button>> m_filename;
		Ui::LabeledInput<RenderOptionsInput> m_render_opts;
		Ui::ErrorMessageDialog m_err_box;
	};
}

#endif