//@	{"targets":[{"name":"render_job_creator.hpp", "type":"include"}]}

#ifndef TEXPAINTER_APP_RENDERJOBCREATOR_HPP
#define TEXPAINTER_APP_RENDERJOBCREATOR_HPP

#include "ui/box.hpp"
#include "ui/slider.hpp"
#include "ui/labeled_input.hpp"
#include "utils/to_string.hpp"

namespace Texpainter::App
{
	class RenderJobCreator
	{
	private:
		enum class ControlId : int
		{
			Supersampling
		};

	public:
		RenderJobCreator(RenderJobCreator&&) = delete;

		explicit RenderJobCreator(Ui::Container& container)
		    : m_root{container, Ui::Box::Orientation::Vertical}
		    , m_supersampling{m_root,
		                      Ui::Box::Orientation::Horizontal,
		                      "Supersampling:",
		                      Ui::Box::Orientation::Vertical,
		                      "Foo",
		                      false}
		{
			{
				auto& inner = m_supersampling.inputField();
				inner.label().content("1").alignment(1.0f);
				inner.inputField().eventHandler<ControlId::Supersampling>(*this);
			}
		}

		template<auto>
		void onChanged(Ui::Slider&)
		{
			m_supersampling.inputField().label().content(
			    toString(value())
			        .c_str());
		}

		uint32_t value() const
		{
			auto val = m_supersampling.inputField().inputField().value().value();
			return static_cast<uint32_t>(std::nextafter(8.0f, 0.0f) * val) + 1;
		}


	private:
		Ui::Box m_root;
		Ui::LabeledInput<Ui::ReversedLabeledInput<Ui::Slider>> m_supersampling;
	};
}

#endif