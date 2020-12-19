//@	{"targets":[{"name":"size_input.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_SIZEINPUT_HPP
#define TEXPAINTER_APP_SIZEINPUT_HPP

#include "ui/box.hpp"
#include "ui/labeled_input.hpp"
#include "ui/text_entry.hpp"
#include "ui/error_message_dialog.hpp"
#include "utils/numconv.hpp"

#include <stdexcept>

namespace Texpainter
{
	class SizeInput
	{
	public:
		enum class ControlId : int
		{
			Width,
			Height
		};

		explicit SizeInput(Ui::Container& container, Size2d default_size, Size2d max_size)
		    : m_root{container, Ui::Box::Orientation::Horizontal}
		    , m_width{m_root, Ui::Box::Orientation::Horizontal, "Width: "}
		    , m_height{m_root, Ui::Box::Orientation::Horizontal, "Height: "}
		    , m_width_str{std::to_string(default_size.width())}
		    , m_height_str{std::to_string(default_size.height())}
		    , m_max_size{max_size}
		{
			m_width.inputField()
			    .content(m_width_str.c_str())
			    .alignment(1.0f)
			    .width(5)
			    .eventHandler<ControlId::Width>(*this);

			m_height.inputField()
			    .content(m_height_str.c_str())
			    .alignment(1.0f)
			    .width(5)
			    .eventHandler<ControlId::Height>(*this);
		}

		Size2d value() const
		{
			auto const width  = static_cast<uint32_t>(std::atoi(m_width.inputField().content()));
			auto const height = static_cast<uint32_t>(std::atoi(m_height.inputField().content()));

			return Size2d{width, height};
		}

		template<ControlId>
		void onChanged(Ui::TextEntry& entry);

		template<ControlId, class... T>
		void handleException(char const* msg, T&...)
		{
			m_err_disp.show(m_root, "Texpainter", msg);
		}

	private:
		Ui::Box m_root;
		Ui::LabeledInput<Ui::TextEntry> m_width;
		Ui::LabeledInput<Ui::TextEntry> m_height;

		std::string m_width_str;
		std::string m_height_str;
		Size2d m_max_size;

		Ui::ErrorMessageDialog m_err_disp;
	};

	template<>
	inline void SizeInput::onChanged<SizeInput::ControlId::Width>(Ui::TextEntry& entry)
	{
		auto val = toInt(entry.content());
		if(!val.has_value())
		{
			entry.content(m_width_str.c_str());
			return;
		}

		entry.content(toArray(std::clamp(*val, 1u, m_max_size.width())).data());
	}

	template<>
	inline void SizeInput::onChanged<SizeInput::ControlId::Height>(Ui::TextEntry& entry)
	{
		auto val = toInt(entry.content());
		if(!val.has_value())
		{
			entry.content(m_height_str.c_str());
			return;
		}

		entry.content(toArray(std::clamp(*val, 1u, m_max_size.height())).data());
	}
}

#endif
