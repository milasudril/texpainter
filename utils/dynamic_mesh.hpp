//@	{
//@	 "targets":[{"name":"dynamic_mesh.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UTILS_DYNAMICMESH_HPP
#define TEXPAINTER_UTILS_DYNAMICMESH_HPP

#include "./iter_pair.hpp"

#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <cassert>

namespace Texpainter
{
	template<class VertexId, class Point>
	class DynamicMesh
	{
		using SegRef      = std::reference_wrapper<std::pair<VertexId, VertexId> const>;
		using SegRefArray = std::vector<SegRef>;

	public:
		auto insert(std::pair<VertexId, Point>&& val)
		{
			auto i = m_verts.find(val.first);
			if(i != std::end(m_verts)) [[unlikely]]
				{
					return std::make_pair(i, false);
				}

			auto ret = m_verts.insert(i, std::move(val));
			m_seg_refs.insert(std::make_pair(ret.first->first, SegRefArray{}));
			return std::make_pair(ret, true);
		}

		DynamicMesh& moveTo(VertexId id, Point loc)
		{
			auto i = m_verts.find(id);
			assert(i != std::end(m_verts));
			i->second = loc;
			return *this;
		}

		DynamicMesh& remove(VertexId v)
		{
			m_verts.erase(v);
			if(auto i = m_seg_refs.find(v); i != std::end(m_seg_refs))
			{
				std::ranges::for_each(
				    i.second, [&segs = m_segs](auto const& item) { segs.erase(item.get()); });
				m_seg_refs.erase(i);
			}
			return *this;
		}

		DynamicMesh& connect(VertexId a, VertexId b)
		{
			assert(m_verts.find(a) != std::end(m_verts));
			assert(m_verts.find(b) != std::end(m_verts));

			if(auto ip = m_segs.insert(std::make_pair(a, b)); ip.second)
			{ m_seg_refs.push_back(std::ref(*ip.first)); }
			return *this;
		}

		decltype(auto) lineSegs() const { return IterPair{std::begin(m_segs), std::end(m_segs)}; }

		Point location(VertexId id) const
		{
			auto i = m_verts.find(id);
			assert(i != std::end(m_verts));
			return i->second;
		}

		bool exists(VertexId id) const { return m_verts.find(id) != std::end(m_verts); }

	private:
		std::map<VertexId, Point> m_verts;
		std::set<std::pair<VertexId, VertexId>> m_segs;
		std::map<VertexId, SegRefArray> m_seg_refs;
	};

	template<class VertexId, class Point>
	std::vector<std::pair<Point, Point>> resolveLineSegs(DynamicMesh<VertexId, Point> const& mesh)
	{
		std::vector<std::pair<Point, Point>> ret;
		std::ranges::transform(mesh.lineSegs(), std::back_inserter(ret), [&mesh](auto seg){
			return std::make_pair(mesh.location(seg.first), mesh.location(seg.second));
		});
		return ret;
	}
}

#endif