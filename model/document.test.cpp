//@	{
//@	 "targets":[{"name":"document.test", "type":"application", "autorun":1}]
//@	}

#include "./document.hpp"

#include <cassert>

namespace Testcases
{
	void texpainterModelDocumentRender()
	{
		Texpainter::Model::Document doc{Texpainter::Size2d{3, 2}};
		auto const src_name = Texpainter::Model::ItemName{"Image source"};

		auto input = doc.insert(src_name, Texpainter::PixelStore::RgbaImage{3, 2});
		assert(input != nullptr);
		auto& src = input->source;
		src.modify([](auto& img) noexcept {
			assert((img.size() == Texpainter::Size2d{3, 2}));
			img(0, 0) = Texpainter::PixelStore::RgbaValue{1.0f, 0.0f, 0.0f, 1.0f};
			img(1, 0) = Texpainter::PixelStore::RgbaValue{0.0f, 1.0f, 0.0f, 1.0f};
			img(2, 0) = Texpainter::PixelStore::RgbaValue{0.0f, 0.0f, 1.0f, 1.0f};
			img(0, 1) = Texpainter::PixelStore::RgbaValue{1.0f, 1.0f, 0.0f, 1.0f};
			img(1, 1) = Texpainter::PixelStore::RgbaValue{0.0f, 1.0f, 1.0f, 1.0f};
			img(2, 1) = Texpainter::PixelStore::RgbaValue{1.0f, 0.0f, 1.0f, 1.0f};
			return true;
		});
		src.clearStatus();

		auto node_item = doc.inputNodeItem(src_name);
		assert(node_item != nullptr);
		doc.compositor().connect(Texpainter::Model::Compositor::OutputNodeId,
		                         Texpainter::FilterGraph::InputPortIndex{0},
		                         node_item->first,
		                         Texpainter::FilterGraph::OutputPortIndex{0});

		{
			auto result = render(doc);
			assert(std::ranges::equal(src.get().pixels(), result.pixels(), [](auto a, auto b) {
				a -= b;
				return a.red() == 0.0f && a.green() == 0.0f && a.blue() == 0.0f
				       && a.alpha() == 0.0f;
			}));
		}

		puts("===================");
		src.modify([](auto& img) noexcept {
			assert((img.size() == Texpainter::Size2d{3, 2}));
			img(0, 0) = Texpainter::PixelStore::RgbaValue{1.0f, 0.5f, 0.0f, 1.0f};
			return true;
		});

		assert(src.dirty());

		{
			auto result = render(doc);
			assert(std::ranges::equal(src.get().pixels(), result.pixels(), [](auto a, auto b) {
				a -= b;
				return a.red() == 0.0f && a.green() == 0.0f && a.blue() == 0.0f
				       && a.alpha() == 0.0f;
			}));
		}
	}

	void texpainterModelDocumentInsertAndEraseImage()
	{
		Texpainter::Model::Document doc{Texpainter::Size2d{3, 2}};
		auto const src_name = Texpainter::Model::ItemName{"Image source"};

		auto input = doc.insert(src_name, Texpainter::PixelStore::RgbaImage{3, 2});
		assert(input != nullptr);

		assert(doc.inputNodeItem(src_name) != nullptr);
		assert(doc.eraseImage(src_name));
		assert(doc.inputNodeItem(src_name) == nullptr);
	}

	void texpainterModelDocumentInsertAndErasePalette()
	{
		Texpainter::Model::Document doc{Texpainter::Size2d{3, 2}};
		auto const src_name = Texpainter::Model::ItemName{"Palette source"};

		auto input = doc.insert(src_name, Texpainter::Model::Palette{});
		assert(input != nullptr);

		assert(doc.inputNodeItem(src_name) != nullptr);
		assert(doc.erasePalette(src_name));
		assert(doc.inputNodeItem(src_name) == nullptr);
	}
}

int main()
{
	Testcases::texpainterModelDocumentRender();
	Testcases::texpainterModelDocumentInsertAndEraseImage();
	Testcases::texpainterModelDocumentInsertAndErasePalette();
	return 0;
}