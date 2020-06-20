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
   m_root{owner, Ui::Box::Orientation::Vertical},
   m_filter_graph{m_root, Ui::Box::Orientation::Horizontal, "Filter graph: "},
   m_orientation{m_root, Ui::Box::Orientation::Horizontal, "Orientation: ", false},
   m_img_view{m_root}
{
	std::ranges::for_each(
	   GraphNames, [& filters = m_filter_graph.inputField()](auto item) { filters.append(item); });
}
