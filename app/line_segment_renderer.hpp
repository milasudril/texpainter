#ifndef TEXPAINTER_UI_LINESEGMENTRENDERER_HPP
#define TEXPAINTER_UI_LINESEGMENTRENDERER_HPP

namespace Texpainter::Ui
{
	class LineSegmentRenderer
	{
	public:
		VertexId insertVertex(ToplevelCoordinates loc)
		{
			m_verts[m_current_id] = loc;
			m_seg_refs[m_current_id];
			auto ret = m_current_id;
			++m_current_id;
			return ret;
		}

		LineSegmentRenderer& updateVertex(VertexId id, ToplevelCoordinates loc)
		{
			m_verts[id] = loc;
		}

		LineSegmentRenderer& removeVertex(VertexId v)
		{
			m_verts.erase(v);
			auto i = m_seg_refs.find(v);
			std::ranges::for_each(i.second, [&segs = m_segs](auto const& item) {
				segs.erase(item.get());
			});
			m_seg_refs.erase
			m_segs->erase(i);
			return *this;
		}

		LineSegmentRenderer& connect(VertexId a, VertexId b)
		{
			if(auto ip = m_segs.insert(std::make_pair(a, b)); ip.second)
			{
				m_seg_refs.push_back(std::ref(*ip.first));
			}
			return *this;
		}

	protected:
	private:
		std::map<VertexId, ToplevelCoordinates> m_verts;
		std::set<std::pair<VertexId, VertexId>> m_segs;
		std::map<VertexId, std::vector<std::reference_wrapper<std::pair<VertexId, VertexId> const>>> m_seg_refs;
	};
}

#endif