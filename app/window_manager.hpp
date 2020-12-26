//@	{"targets":[{"name":"window_manager.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_WINDOWMANAGER_HPP
#define TEXPAINTER_APP_WINDOWMANAGER_HPP

#include "./document_editor.hpp"
#include "./image_editor.hpp"
#include "./filter_graph_editor.hpp"
#include "./image_creator.hpp"
#include "./palette_creator.hpp"
#include "./document_previewer.hpp"

#include "model/document.hpp"
#include "model/palette_generate.hpp"
#include "pixel_store/image_io.hpp"
#include "ui/window.hpp"
#include "ui/image_view.hpp"
#include "ui/dialog.hpp"
#include "ui/filename_select.hpp"
#include "ui/context.hpp"

#include <filesystem>

namespace Texpainter::App
{
	namespace detail
	{
		using FilterGraphEditWindow = DocumentEditor<FilterGraphEditor>;
		using ImageEditWindow       = DocumentEditor<ImageEditor>;
		using DocumentPreviewWindow = DocumentEditor<DocumentPreviewer>;

		template<AppWindowType>
		struct AppWindowTypeTraits;

		template<>
		struct AppWindowTypeTraits<AppWindowType::FilterGraphEditor>
		{
			using type = std::unique_ptr<FilterGraphEditWindow>;
			static constexpr char const* name() { return "Texpainter: Compositor"; }
		};

		template<>
		struct AppWindowTypeTraits<AppWindowType::ImageEditor>
		{
			using type = std::unique_ptr<ImageEditWindow>;
			static constexpr char const* name() { return "Texpainter: Image editor"; }
		};

		template<>
		struct AppWindowTypeTraits<AppWindowType::DocumentPreviewer>
		{
			using type = std::unique_ptr<DocumentPreviewWindow>;
			static constexpr char const* name() { return "Texpainter: Document preview"; }
		};
	}

	class WindowManager
	{
		template<AppWindowType id, class... Args>
		auto createWindow(Args&&... args)
		{
			using T  = typename detail::AppWindowTypeTraits<id>::type::element_type;
			auto ret = std::make_unique<T>(detail::AppWindowTypeTraits<id>::name(),
			                               std::forward<Args>(args)...);
			ret->window().template eventHandler<id>(*this);
			ret->menu().eventHandler(*this);
			ret->widget().template eventHandler<id>(*this);
			return ret;
		}

		using ImageCreatorDlg        = Ui::Dialog<ImageCreator>;
		using EmptyPaletteCreatorDlg = Ui::Dialog<Ui::LabeledInput<Ui::TextEntry>>;
		using PaletteGenerateDlg     = Ui::Dialog<PaletteCreator>;

	public:
		[[nodiscard]] WindowManager(): m_document{Size2d{512, 512}}, m_window_count{3}
		{
			m_windows.get<AppWindowType::FilterGraphEditor>() =
			    createWindow<AppWindowType::FilterGraphEditor>(m_document);
			m_windows.get<AppWindowType::ImageEditor>() =
			    createWindow<AppWindowType::ImageEditor>(m_document);
			m_windows.get<AppWindowType::DocumentPreviewer>() =
			    createWindow<AppWindowType::DocumentPreviewer>(m_document);
		}

		template<AppWindowType>
		void onKeyUp(Ui::Window&, Ui::Scancode)
		{
		}

		template<AppWindowType window>
		void onKeyDown(Ui::Window&, Ui::Scancode)
		{
			if constexpr(window != AppWindowType::DocumentPreviewer)
			{
				m_windows.template get<window>()->widget().onKeyDown(
				    Ui::Context::get().keyboardState());
			}
		}

		template<AppWindowType id>
		void onClose(Ui::Window&)
		{
			--m_window_count;
			if constexpr(id == AppWindowType::FilterGraphEditor)
			{
				m_document.nodeLocations(
				    m_windows.get<AppWindowType::FilterGraphEditor>()->widget().nodeLocations());
			}
			m_windows.get<id>().reset();
			if(m_window_count == 0) { Ui::Context::get().exit(); }
		}

		template<AppWindowType, class Source>
		void handleException(char const* msg, Source&)
		{
			fprintf(stderr, "Error: %s\n", msg);
		}

		template<auto>
		void handleException(char const* msg, Ui::MenuItem&)
		{
			fprintf(stderr, "Error: %s\n", msg);
		}

		template<auto id>
		requires(!std::same_as<decltype(id), AppWindowType>) void onActivated(Ui::MenuItem&)
		{
			fprintf(stderr, "Unimplemented action\n");
		}

		template<AppWindowType item>
		void onActivated(Ui::MenuItem&)
		{
			if(m_windows.get<item>() == nullptr)
			{
				m_windows.get<item>() = createWindow<item>(m_document);
				++m_window_count;
			}
			m_windows.get<item>()->window().show();
		}

		template<auto>
		void handleException(char const* msg, ImageCreatorDlg&)
		{
			fprintf(stderr, "Error: %s\n", msg);
		}

		template<auto>
		void handleException(char const* msg, EmptyPaletteCreatorDlg&)
		{
			fprintf(stderr, "Error: %s\n", msg);
		}

		template<auto>
		void handleException(char const* msg, PaletteGenerateDlg&)
		{
			fprintf(stderr, "Error: %s\n", msg);
		}

		template<auto>
		void dismiss(ImageCreatorDlg&)
		{
			m_img_creator.reset();
		}

		template<auto>
		void confirmPositive(ImageCreatorDlg& src)
		{
			auto result = src.widget().imageInfo();
			insert(std::move(result.name), PixelStore::Image{result.size});
			m_img_creator.reset();
		}

		template<auto>
		void dismiss(EmptyPaletteCreatorDlg&)
		{
			m_empty_pal_creator.reset();
		}

		template<auto>
		void confirmPositive(EmptyPaletteCreatorDlg& src)
		{
			auto result = src.widget().inputField().content();
			insert(Model::ItemName{result}, Model::Palette{});
			m_empty_pal_creator.reset();
		}

		template<auto>
		void dismiss(PaletteGenerateDlg&)
		{
			m_gen_palette.reset();
		}

		template<auto>
		void confirmPositive(PaletteGenerateDlg& src)
		{
			auto result = src.widget().value();
			insert(std::move(result.name), generatePalette(result.params));
			m_gen_palette.reset();
		}

		template<AppWindowType id, class Src>
		void onUpdated(Src&)
		{
			if(auto editor = m_windows.get<AppWindowType::ImageEditor>().get(); editor != nullptr)
				[[likely]] { editor->widget().refresh(); }

			if(auto output = m_windows.get<AppWindowType::DocumentPreviewer>().get();
			   output != nullptr)
				[[likely]] { output->widget().refresh(); }
		}

		template<auto, class T>
		void onMouseDown(T&&...)
		{
		}

		template<auto, class T>
		void onMouseUp(T&&...)
		{
		}

		template<auto, class T>
		void onMouseMove(T&&...)
		{
		}

	private:
		Model::Document m_document;
		Enum::Tuple<AppWindowType, detail::AppWindowTypeTraits> m_windows;

		size_t m_window_count;

		std::unique_ptr<ImageCreatorDlg> m_img_creator;
		std::unique_ptr<EmptyPaletteCreatorDlg> m_empty_pal_creator;
		std::unique_ptr<PaletteGenerateDlg> m_gen_palette;

		void insert(Model::ItemName&& name, PixelStore::Image&& img)
		{
			if(m_document.insert(name, std::move(img)) == nullptr)
			{ throw std::string{"Item already exists"}; }

			auto node = m_document.inputNodeItem(name);
			m_windows.get<AppWindowType::FilterGraphEditor>()->widget().insertNodeEditor(*node);

			m_document.currentImage(std::move(name));

			if(auto img_editor = m_windows.get<AppWindowType::ImageEditor>().get();
			   img_editor != nullptr)
			{
				img_editor->widget().refresh();
				img_editor->window().show();
			}

			if(auto output = m_windows.get<AppWindowType::DocumentPreviewer>().get();
			   output != nullptr)
				[[likely]] { output->widget().refresh(); }
		}

		void insert(Model::ItemName&& name, Model::Palette&& pal)
		{
			if(m_document.insert(name, std::move(pal)) == nullptr)
			{ throw std::string{"Item already exists"}; }

			auto node = m_document.inputNodeItem(name);
			m_windows.get<AppWindowType::FilterGraphEditor>()->widget().insertNodeEditor(*node);

			m_document.currentPalette(std::move(name));

			if(auto img_editor = m_windows.get<AppWindowType::ImageEditor>().get();
			   img_editor != nullptr)
			{
				img_editor->widget().refresh();
				img_editor->window().show();
			}

			if(auto output = m_windows.get<AppWindowType::DocumentPreviewer>().get();
			   output != nullptr)
				[[likely]] { output->widget().refresh(); }
		}
	};

	template<>
	inline void WindowManager::onActivated<AppAction::Quit>(Ui::MenuItem&)
	{
		Ui::Context::get().exit();
	}

	template<>
	inline void WindowManager::onActivated<ImageAction::New>(Ui::MenuItem& item)
	{
		if(m_img_creator == nullptr) [[likely]]
			{
				m_img_creator = std::make_unique<ImageCreatorDlg>(
				    item, "Create new image", Size2d{512, 512}, Size2d{65535, 65535});
				m_img_creator->eventHandler<ImageAction::New>(*this);
			}
		m_img_creator->show();
	}

	template<>
	inline void WindowManager::onActivated<ImageAction::Import>(Ui::MenuItem& item)
	{
		std::filesystem::path filename;
		if(Ui::filenameSelect(
		       item,
		       std::filesystem::current_path(),
		       filename,
		       Ui::FilenameSelectMode::Open,
		       [](char const* filename) { return PixelStore::fileValid(filename); },
		       "Supported image files"))
		{
			insert(Model::createItemNameFromFilename(filename.c_str()),
			       PixelStore::load(filename.c_str()));
		}
	}

	template<>
	inline void WindowManager::onActivated<PaletteAction::New>(Ui::MenuItem& item)
	{
		if(m_empty_pal_creator == nullptr) [[likely]]
			{
				m_empty_pal_creator = std::make_unique<EmptyPaletteCreatorDlg>(
				    item, "Create empty palette", Ui::Box::Orientation::Horizontal, "Name");
				m_empty_pal_creator->eventHandler<PaletteAction::New>(*this);
			}
		m_empty_pal_creator->show();
	}

	template<>
	inline void WindowManager::onActivated<PaletteAction::Generate>(Ui::MenuItem& item)
	{
		if(m_gen_palette == nullptr) [[likely]]
			{
				m_gen_palette = std::make_unique<PaletteGenerateDlg>(item, "Generate palette");
				m_gen_palette->eventHandler<PaletteAction::Generate>(*this);
			}
		m_gen_palette->show();
	}
}

#endif