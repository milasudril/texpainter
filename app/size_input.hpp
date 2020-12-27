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
		static constexpr auto MinRatio = -1;
		static constexpr auto MaxRatio = 1;

		static constexpr Ui::Slider::TickMark RatioTickMarks[] = {
		    Ui::Slider::TickMark{Ui::logValue(0.5, MinRatio, MaxRatio), "1:2"},
		    Ui::Slider::TickMark{Ui::logValue(1.0 / std::numbers::phi, MinRatio, MaxRatio), "1:Φ"},
		    Ui::Slider::TickMark{Ui::logValue(1.0 / std::numbers::sqrt2, MinRatio, MaxRatio),
		                         "1:√2"},
		    Ui::Slider::TickMark{Ui::logValue(std::numbers::sqrt3 / 2.0, MinRatio, MaxRatio),
		                         "√3:2"},
		    Ui::Slider::TickMark{Ui::logValue(1, MinRatio, MaxRatio), "1:1"},
		    Ui::Slider::TickMark{Ui::logValue(2.0 / std::numbers::sqrt3, MinRatio, MaxRatio),
		                         "2:√3"},
		    Ui::Slider::TickMark{Ui::logValue(std::numbers::sqrt2, MinRatio, MaxRatio), "√2:1"},
		    Ui::Slider::TickMark{Ui::logValue(std::numbers::phi, MinRatio, MaxRatio), "Φ:1"},
		    Ui::Slider::TickMark{Ui::logValue(2.0, MinRatio, MaxRatio), "2:1"}};

		static constexpr Size2d size_from_width(uint32_t width, double r)
		{
			auto const height = static_cast<uint32_t>(width / r + 0.5);
			return Size2d{width, height};
		}

		static constexpr Size2d size_from_height(uint32_t height, double r)
		{
			auto const width = static_cast<uint32_t>(height * r + 0.5);
			return Size2d{width, height};
		}

		static constexpr Size2d size_from_square_size(uint32_t size, double r)
		{
			// A = w*h
			// w/h = r  <=> w = h*r <=> h = w/r
			// A = s*s

			// A = r*h*h
			// s*s = r*h*h
			// s = sqrt(r)*h
			// h = s/sqrt(r)
			// w = h*r = s*r/sqrt(r) = s*sqrt(r)
			auto const w = static_cast<uint32_t>(size * std::sqrt(r) + 0.5);
			auto const h = static_cast<uint32_t>(size / std::sqrt(r) + 0.5);
			return Size2d{w, h};
		}

		using SizeFunc = Size2d (*)(uint32_t, double);

		static constexpr SizeFunc s_size_mode[] = {
		    size_from_width, size_from_height, size_from_square_size};

		static constexpr uint32_t width_from_area(size_t A, double r)
		{
			return static_cast<uint32_t>(std::sqrt(r * A));
		}

		static constexpr uint32_t height_from_area(size_t A, double r)
		{
			return static_cast<uint32_t>(std::sqrt(A / r));
		}

		using LengthFunc = uint32_t (*)(size_t A, double r);

		static constexpr LengthFunc s_size[] = {width_from_area, height_from_area, width_from_area};

	public:
		enum class ControlId : int
		{
			SizeMode,
			SizeValue,
			AspectRatio
		};

		SizeInput(SizeInput&&) = delete;

		explicit SizeInput(Ui::Container& container, Size2d default_size, Size2d max_size)
		    : m_root{container, Ui::Box::Orientation::Vertical}
		    , m_size{m_root, Ui::Box::Orientation::Horizontal, "Width: "}
		    , m_asplect_ratio{m_root, Ui::Box::Orientation::Vertical, "Aspect ratio:", false}
		    , m_comp_size{m_root, Ui::Box::Orientation::Horizontal, "Size: ", "0"}
		    , m_max_size{max_size}
		{
			m_size.label()
			    .append("Height: ")
			    .append("Square size: ")
			    .selected(0)
			    .eventHandler<ControlId::SizeMode>(*this);

			m_size.inputField()
			    .alignment(0.0f)
			    .width(5)
			    .content(std::to_string(default_size.width()).c_str())
			    .eventHandler<ControlId::SizeValue>(*this);

			auto const r = static_cast<double>(default_size.width()) / default_size.height();
			m_asplect_ratio.inputField()
			    .ticks(RatioTickMarks)
			    .value(Ui::logValue(1 / r, MinRatio, MaxRatio))
			    .eventHandler<ControlId::AspectRatio>(*this);

			onChanged<0>(*this);
		}

		Size2d value() const
		{
			auto const size = static_cast<uint32_t>(std::atoi(m_size.inputField().content()));
			auto const r =
			    Ui::logValue(m_asplect_ratio.inputField().value(), MinRatio, MaxRatio, false);
			auto const size_mode = m_size.label().selected();

			return s_size_mode[size_mode](size, r);
		}

		template<auto, class T>
		void onChanged(T&)
		{
			auto size = value();
			if(size.area() > m_max_size.area()) [[unlikely]]
				{
					auto const size_mode = m_size.label().selected();
					auto const r         = Ui::logValue(
                        m_asplect_ratio.inputField().value(), MinRatio, MaxRatio, false);
					auto const length = s_size[size_mode](m_max_size.area(), r);
					m_size.inputField().content(std::to_string(length).c_str());
					size = value();
				}

			m_comp_size.inputField().content(
			    (toString(size) + ", A = " + toString(size.area())).c_str());
		}

		template<ControlId, class... T>
		void handleException(char const* msg, T&...)
		{
			m_err_disp.show(m_root, "Texpainter", msg);
		}

	private:
		Ui::Box m_root;
		Ui::LabeledInput<Ui::TextEntry, Ui::Combobox> m_size;
		Ui::LabeledInput<Ui::Slider> m_asplect_ratio;
		Ui::LabeledInput<Ui::Label> m_comp_size;

		Size2d m_max_size;

		Ui::ErrorMessageDialog m_err_disp;
	};
}

#endif
