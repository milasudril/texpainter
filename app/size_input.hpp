//@	{"targets":[{"name":"size_input.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_SIZEINPUT_HPP
#define TEXPAINTER_APP_SIZEINPUT_HPP

#include "ui/box.hpp"
#include "ui/labeled_input.hpp"
#include "ui/text_entry.hpp"
#include "ui/error_message_dialog.hpp"
#include "ui/combobox.hpp"
#include "ui/slider.hpp"
#include "utils/numconv.hpp"

#include <numbers>
#include <stdexcept>

namespace Texpainter
{
	class SizeInput
	{
		static constexpr auto MinRatio = 0.5;
		static constexpr auto MaxRatio = 2.0;

		static constexpr Ui::Slider::TickMark RatioTickMarks[] = {
		    Ui::Slider::TickMark{Ui::linValue(0.5, MinRatio, MaxRatio), "1:2"},
		    Ui::Slider::TickMark{Ui::linValue(1.0 / std::numbers::phi, MinRatio, MaxRatio), "1:Φ"},
		    Ui::Slider::TickMark{Ui::linValue(1.0 / std::numbers::sqrt2, MinRatio, MaxRatio),
		                         "1:√2"},
		    Ui::Slider::TickMark{Ui::linValue(std::numbers::sqrt3 / 2.0, MinRatio, MaxRatio),
		                         "√3:2"},
		    Ui::Slider::TickMark{Ui::linValue(1, MinRatio, MaxRatio), "1:1"},
		    Ui::Slider::TickMark{Ui::linValue(2.0 / std::numbers::sqrt3, MinRatio, MaxRatio),
		                         "2:√3"},
		    Ui::Slider::TickMark{Ui::linValue(std::numbers::sqrt2, MinRatio, MaxRatio), "√2:1"},
		    Ui::Slider::TickMark{Ui::linValue(std::numbers::phi, MinRatio, MaxRatio), "Φ:1"},
		    Ui::Slider::TickMark{Ui::linValue(2.0, MinRatio, MaxRatio), "2:1"}};

	public:
		enum class ControlId : int
		{
			Width,
			AspectRatio
		};

		explicit SizeInput(Ui::Container& container, Size2d default_size, Size2d max_size)
		    : m_root{container, Ui::Box::Orientation::Vertical}
		    , m_size{m_root, Ui::Box::Orientation::Horizontal, "Width: "}
		    , m_asplect_ratio{m_root, Ui::Box::Orientation::Horizontal, "Aspect ratio: ", false}
		    , m_size_str{std::to_string(default_size.width())}
		    , m_max_size{max_size}
		{
			m_size.label().append("Height: ").append("Square size: ").selected(0);

			m_size.inputField()
			    .content(m_size_str.c_str())
			    .alignment(0.0f)
			    .width(5)
			    .eventHandler<ControlId::Width>(*this);


			auto const r = static_cast<double>(default_size.width()) / default_size.height();

			m_asplect_ratio.inputField()
			    .ticks(RatioTickMarks)
			    .value(Ui::linValue(1 / r, MinRatio, MaxRatio));
		}

		Size2d value() const
		{
			auto const width  = static_cast<uint32_t>(std::atoi(m_size.inputField().content()));
			auto const height = static_cast<uint32_t>(
			    width / Ui::linValue(m_asplect_ratio.inputField().value(), MinRatio, MaxRatio)
			    + 0.5);

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
		Ui::LabeledInput<Ui::TextEntry, Ui::Combobox> m_size;
		Ui::LabeledInput<Ui::Slider> m_asplect_ratio;

		std::string m_size_str;
		Size2d m_max_size;

		Ui::ErrorMessageDialog m_err_disp;
	};

	template<>
	inline void SizeInput::onChanged<SizeInput::ControlId::Width>(Ui::TextEntry& entry)
	{
		auto val = toInt(entry.content());
		if(!val.has_value())
		{
			entry.content(m_size_str.c_str());
			return;
		}

		entry.content(toArray(std::clamp(*val, 1u, m_max_size.width())).data());
	}
}

#endif
