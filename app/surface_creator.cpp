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
			constexpr auto i =
			   Texpainter::itemDistance(begin(Texpainter::Empty<typename Tag::value_type>{}), Tag::value);
			ret[i] = Texpainter::Generators::FilterGraphItemTraits<Tag::value>::name;
		});

		return ret;
	}

	constexpr auto GraphNames = getGraphNames();
}

Texpainter::SurfaceCreator::SurfaceCreator(Ui::Container& owner):
   m_preview{384, 384},
   m_root{owner, Ui::Box::Orientation::Vertical},
   m_filter_graph{m_root, Ui::Box::Orientation::Horizontal, "Filter graph: "},
   m_orientation{m_root, Ui::Box::Orientation::Horizontal, "Orientation: ", false},
   m_horz_cutoff{m_root, Ui::Box::Orientation::Horizontal, "Horz cutoff freq: ", false},
   m_vert_cutoff{m_root, Ui::Box::Orientation::Horizontal, "Vert cutoff freq: ", false},
   m_img_view{m_root.insertMode(Ui::Box::InsertMode{0, Ui::Box::Expand | Ui::Box::Fill})}
{
	std::ranges::for_each(
	   GraphNames, [& filters = m_filter_graph.inputField()](auto item) { filters.append(item); });
	m_filter_graph.inputField()
	   .selected(static_cast<int>(m_generator.filters()))
	   .eventHandler<ControlId::FilterGraph>(*this);
	m_orientation.inputField()
	   .value(m_generator.orientation().turns())
	   .eventHandler<ControlId::Orientation>(*this);
	auto cutoff = m_generator.cutoffFrequency();
	m_horz_cutoff.inputField().value(cutoff.ξ().value()).eventHandler<ControlId::HorzCutoff>(*this);
	m_vert_cutoff.inputField().value(cutoff.η().value()).eventHandler<ControlId::VertCutoff>(*this);
	m_preview = m_generator(m_preview.size());
	m_img_view.image(m_preview);
}

template<>
void Texpainter::SurfaceCreator::onChanged<Texpainter::SurfaceCreator::ControlId::FilterGraph>(
   Ui::Combobox& source)
{
	m_generator.filters(static_cast<Generators::FilterGraph>(source.selected()));
	m_preview = m_generator(m_preview.size());
	m_img_view.update();
}

template<>
void Texpainter::SurfaceCreator::onChanged<Texpainter::SurfaceCreator::ControlId::Orientation>(
   Ui::Slider& source)
{
	m_generator.orientation(Angle{0.5 * source.value(), Angle::Turns{}});
	m_preview = m_generator(m_preview.size());
	m_img_view.update();
}

template<>
void Texpainter::SurfaceCreator::onChanged<Texpainter::SurfaceCreator::ControlId::HorzCutoff>(
   Ui::Slider& source)
{
	auto vert_cutoff = m_vert_cutoff.inputField().value();
	m_generator.cutoffFrequency(SpatialFrequency{vec2_t{source.value(), vert_cutoff}});
	m_preview = m_generator(m_preview.size());
	m_img_view.update();
}

template<>
void Texpainter::SurfaceCreator::onChanged<Texpainter::SurfaceCreator::ControlId::VertCutoff>(
   Ui::Slider& source)
{
	auto horz_cutoff = m_horz_cutoff.inputField().value();
	m_generator.cutoffFrequency(SpatialFrequency{vec2_t{horz_cutoff, source.value()}});
	m_preview = m_generator(m_preview.size());
	m_img_view.update();
}