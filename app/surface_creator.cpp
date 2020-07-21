//@	{
//@	  "targets":[{"name":"surface_creator.o", "type":"object"}]
//@	 }

#include "./surface_creator.hpp"

namespace
{
	constexpr auto getGraphNames()
	{
		std::array<char const*,
		           static_cast<int>(end(Texpainter::Empty<Texpainter::Generators::FilterGraph>{}))>
		    ret{};

		Texpainter::forEachEnumItem<Texpainter::Generators::FilterGraph>([&ret]<class Tag>(Tag) {
			constexpr auto i = static_cast<int>(Tag::value);
			ret[i] = Texpainter::Generators::FilterGraphItemTraits<Tag::value>::displayName();
		});

		return ret;
	}

	constexpr auto GraphNames = getGraphNames();

	auto toFrequency(Texpainter::Ui::SliderValue val)
	{
		return Texpainter::Frequency{std::exp2(10 * (val.value() - 1.0))};
	}

	auto toSliderValue(Texpainter::Frequency f)
	{
		return Texpainter::Ui::SliderValue{std::log2(f.value()) / 10.0 + 1.0};
	}

	auto toAngle(Texpainter::Ui::SliderValue val, double max_range = 0.5)
	{
		return Texpainter::Angle{max_range * val.value(), Texpainter::Angle::Turns{}};
	}
}

Texpainter::SurfaceCreator::SurfaceCreator(Ui::Container& owner)
    : m_preview{384, 384}
    , m_root{owner, Ui::Box::Orientation::Vertical}
    , m_filter_graph{m_root, Ui::Box::Orientation::Horizontal, "Filter graph: "}
    , m_orientation{m_root, Ui::Box::Orientation::Horizontal, "Orientation: ", false}
    , m_cutoff{m_root, Ui::Box::Orientation::Horizontal}
    , m_cutoff_sliders{m_cutoff.insertMode(Ui::Box::InsertMode{0, Ui::Box::Expand | Ui::Box::Fill}),
                       Ui::Box::Orientation::Vertical}
    , m_horz_cutoff{m_cutoff_sliders, Ui::Box::Orientation::Horizontal, "Horz cutoff freq: ", false}
    , m_vert_cutoff{m_cutoff_sliders, Ui::Box::Orientation::Horizontal, "Vert cutoff freq: ", false}
    , m_cutoff_sliders_lock{m_cutoff.insertMode(Ui::Box::InsertMode{0, 0}), "Lock\nv/h"}
    , m_img_view{m_root.insertMode(Ui::Box::InsertMode{0, Ui::Box::Expand | Ui::Box::Fill})}
{
	std::ranges::for_each(
	    GraphNames, [&filters = m_filter_graph.inputField()](auto item) { filters.append(item); });
	m_filter_graph.inputField()
	    .selected(static_cast<int>(m_generator.filters()))
	    .eventHandler<ControlId::FilterGraph>(*this);
	m_orientation.inputField()
	    .value(Ui::SliderValue{m_generator.orientation().turns()})
	    .eventHandler<ControlId::Orientation>(*this);
	auto cutoff = m_generator.cutoffFrequency();
	m_horz_cutoff.inputField()
	    .value(toSliderValue(cutoff.ξ()))
	    .eventHandler<ControlId::HorzCutoff>(*this);
	m_vert_cutoff.inputField()
	    .value(toSliderValue(cutoff.η()))
	    .eventHandler<ControlId::VertCutoff>(*this);
	m_cutoff_sliders_lock.eventHandler<ControlId::LockHvCutoff>(*this);
	m_preview = m_generator(m_preview.size());
	m_img_view.minSize(Size2d{385, 384}).image(m_preview);
}

template<>
void Texpainter::SurfaceCreator::onChanged<Texpainter::SurfaceCreator::ControlId::FilterGraph>(
    Ui::Combobox& source)
{
	m_generator.filters(static_cast<Generators::FilterGraph>(source.selected()));
	m_preview = m_generator(m_preview.size());
	m_img_view.image(m_preview);
}

template<>
void Texpainter::SurfaceCreator::onChanged<Texpainter::SurfaceCreator::ControlId::Orientation>(
    Ui::Slider& source)
{
	m_generator.orientation(toAngle(source.value()));
	m_preview = m_generator(m_preview.size());
	m_img_view.image(m_preview);
}

template<>
void Texpainter::SurfaceCreator::onChanged<Texpainter::SurfaceCreator::ControlId::HorzCutoff>(
    Ui::Slider& source)
{
	auto const horz_cutoff = toFrequency(source.value());
	auto const vert_cutoff = toFrequency(m_vert_cutoff.inputField().value());

	if(m_cutoff_sliders_lock.state())
	{
		auto freq_new = mean(horz_cutoff, vert_cutoff);
		auto val_new  = toSliderValue(freq_new);
		m_horz_cutoff.inputField().value(val_new);
		m_vert_cutoff.inputField().value(val_new);
		m_generator.cutoffFrequency(SpatialFrequency{freq_new, freq_new});
	}
	else
	{
		m_generator.cutoffFrequency(SpatialFrequency{horz_cutoff, vert_cutoff});
	}

	m_preview = m_generator(m_preview.size());
	m_img_view.image(m_preview);
}

template<>
void Texpainter::SurfaceCreator::onChanged<Texpainter::SurfaceCreator::ControlId::VertCutoff>(
    Ui::Slider& source)
{
	auto const horz_cutoff = toFrequency(m_horz_cutoff.inputField().value());
	auto const vert_cutoff = toFrequency(source.value());

	if(m_cutoff_sliders_lock.state())
	{
		auto freq_new = mean(horz_cutoff, vert_cutoff);
		auto val_new  = toSliderValue(freq_new);
		m_horz_cutoff.inputField().value(val_new);
		m_vert_cutoff.inputField().value(val_new);
		m_generator.cutoffFrequency(SpatialFrequency{freq_new, freq_new});
	}
	else
	{
		m_generator.cutoffFrequency(SpatialFrequency{horz_cutoff, vert_cutoff});
	}

	m_preview = m_generator(m_preview.size());
	m_img_view.image(m_preview);
}

template<>
void Texpainter::SurfaceCreator::onClicked<Texpainter::SurfaceCreator::ControlId::LockHvCutoff>(
    Ui::Button& btn)
{
	if(btn.state())
	{
		auto horz_cutoff = toFrequency(m_horz_cutoff.inputField().value());
		auto vert_cutoff = toFrequency(m_vert_cutoff.inputField().value());

		auto freq_new = mean(horz_cutoff, vert_cutoff);
		auto val_new  = toSliderValue(freq_new);
		m_horz_cutoff.inputField().value(val_new);
		m_vert_cutoff.inputField().value(val_new);
		m_generator.cutoffFrequency(SpatialFrequency{freq_new, freq_new});

		m_preview = m_generator(m_preview.size());
		m_img_view.image(m_preview);
	}
}