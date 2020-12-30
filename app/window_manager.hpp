//@	{"targets":[{"name":"window_manager.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_WINDOWMANAGER_HPP
#define TEXPAINTER_APP_WINDOWMANAGER_HPP

#include "./document_editor.hpp"
#include "./image_editor.hpp"
#include "./filter_graph_editor.hpp"
#include "./image_creator.hpp"
#include "./palette_creator.hpp"
#include "./document_previewer.hpp"
#include "./size_input.hpp"
#include "./render_job_creator.hpp"

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
		template<class EventHandler>
		using FilterGraphEditWindow = DocumentEditor<FilterGraphEditor, EventHandler>;

		template<class EventHandler>
		using ImageEditWindow = DocumentEditor<ImageEditor, EventHandler>;

		template<class EventHandler>
		using DocumentPreviewWindow = DocumentEditor<DocumentPreviewer, EventHandler>;

		template<AppWindowType, class EventHandler>
		struct AppWindowTypeTraits;

		template<class EventHandler>
		struct AppWindowTypeTraits<AppWindowType::FilterGraphEditor, EventHandler>
		{
			using type = std::unique_ptr<FilterGraphEditWindow<EventHandler>>;
			static constexpr char const* name() { return "Texpainter: Compositor"; }
		};

		template<class EventHandler>
		struct AppWindowTypeTraits<AppWindowType::ImageEditor, EventHandler>
		{
			using type = std::unique_ptr<ImageEditWindow<EventHandler>>;
			static constexpr char const* name() { return "Texpainter: Image editor"; }
		};

		template<class EventHandler>
		struct AppWindowTypeTraits<AppWindowType::DocumentPreviewer, EventHandler>
		{
			using type = std::unique_ptr<DocumentPreviewWindow<EventHandler>>;
			static constexpr char const* name() { return "Texpainter: Document preview"; }
		};
	}

	template<class T>
	void exportItem(char const* filter_name,
	                Ui::Container& dlg_owner,
	                std::filesystem::path const& current_dir,
	                std::filesystem::path&& default_name,
	                T const& item)
	{
		if(Ui::filenameSelect(
		       dlg_owner,
		       current_dir,
		       default_name,
		       Ui::FilenameSelectMode::Save,
		       [](char const*) { return true; },
		       filter_name))
		{ store(item, default_name.c_str()); }
	}

	class WindowManager
	{
		template<auto id>
		using AppWindowTypeTraits = detail::AppWindowTypeTraits<id, WindowManager>;

		template<AppWindowType id, class... Args>
		auto createWindow(Model::Document& doc, Args&&... args)
		{
			using T  = typename AppWindowTypeTraits<id>::type::element_type;
			auto ret = std::make_unique<T>(
			    AppWindowTypeTraits<id>::name(), *this, doc, std::forward<Args>(args)...);
			ret->window().template eventHandler<id>(*this);
			ret->widget().template eventHandler<id>(*this);
			return ret;
		}

		using DocumentCreatorDlg     = Ui::Dialog<Ui::LabeledInput<SizeInput>>;
		using CanvasSizeDlg          = Ui::Dialog<SizeInput>;
		using ImageCreatorDlg        = Ui::Dialog<ImageCreator>;
		using EmptyPaletteCreatorDlg = Ui::Dialog<Ui::LabeledInput<Ui::TextEntry>>;
		using PaletteGenerateDlg     = Ui::Dialog<PaletteCreator>;

	public:
		[[nodiscard]] WindowManager()
		    : m_document{std::make_unique<Model::Document>(Size2d{512, 512})}
		    , m_window_count{3}
		{
			m_windows.get<AppWindowType::FilterGraphEditor>() =
			    createWindow<AppWindowType::FilterGraphEditor>(*m_document);
			m_windows.get<AppWindowType::ImageEditor>() =
			    createWindow<AppWindowType::ImageEditor>(*m_document);
			m_windows.get<AppWindowType::DocumentPreviewer>() =
			    createWindow<AppWindowType::DocumentPreviewer>(*m_document);
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
				m_document->nodeLocations(
				    m_windows.get<AppWindowType::FilterGraphEditor>()->widget().nodeLocations());
			}
			m_windows.get<id>().reset();
			if(m_window_count == 0) { Ui::Context::get().exit(); }
		}

		template<auto id, class Source>
		void handleException(char const* msg, Source&)
		{
			fprintf(stderr, "Error: %s\n", msg);
		}

		template<auto id, class Source>
		void handleException(Enum::Tag<id>, char const* msg, Ui::MenuItem&, Source&)
		{
			fprintf(stderr, "Error: %s\n", msg);
		}

		template<auto id, class Source>
		requires(!std::same_as<decltype(id), AppWindowType>) void onActivated(Enum::Tag<id>,
		                                                                      Ui::MenuItem&,
		                                                                      Source&)
		{
			throw "Unimplemented action";
		}

		template<class Source>
		void onActivated(Enum::Tag<AppAction::Quit>, Ui::MenuItem&, Source&)
		{
			Ui::Context::get().exit();
		}

		template<AppWindowType item, class Source>
		void onActivated(Enum::Tag<item>, Ui::MenuItem&, Source&)
		{
			if(m_windows.get<item>() == nullptr)
			{
				m_windows.get<item>() = createWindow<item>(*m_document);
				++m_window_count;
			}
			m_windows.get<item>()->window().show();
		}

		template<class Source>
		void onActivated(Enum::Tag<DocumentAction::New>, Ui::MenuItem&, Source& src)
		{
			if(m_doc_creator == nullptr) [[likely]]
				{
					m_doc_creator =
					    std::make_unique<DocumentCreatorDlg>(src.window(),
					                                         "Create new document",
					                                         Ui::Box::Orientation::Vertical,
					                                         "Canvas size:",
					                                         Size2d{512, 512},
					                                         Size2d{65535, 65535});
					m_doc_creator->eventHandler<DocumentAction::New>(*this);
				}
			m_doc_creator->show();
		}

		template<class Source>
		void onActivated(Enum::Tag<DocumentAction::Save>, Ui::MenuItem&, Source&)
		{
			if(auto compositor = m_windows.get<AppWindowType::FilterGraphEditor>().get();
			   compositor != nullptr)
			{ m_document->nodeLocations(compositor->widget().nodeLocations()); }
			store(*m_document, "/dev/stdout");
		}

		template<class Source>
		void onActivated(Enum::Tag<DocumentAction::Export>, Ui::MenuItem&, Source& src)
		{
			std::filesystem::path name;
			if(Ui::filenameSelect(
			       src.window(),
			       m_document->workingDirectory(),
			       name,
			       Ui::FilenameSelectMode::Save,
			       [](char const*) { return true; },
			       "Exr files"))
			{ store(render(*m_document, Model::Document::ForceUpdate{true}, 4.0), name.c_str()); }
		}

		template<class Source>
		void onActivated(Enum::Tag<DocumentAction::SetCanvasSize>, Ui::MenuItem&, Source& src)
		{
			if(m_canvas_modifier == nullptr) [[likely]]
				{
					m_canvas_modifier = std::make_unique<CanvasSizeDlg>(
					    src.window(), "Set canvas size", Size2d{512, 512}, Size2d{65535, 65535});
					m_canvas_modifier->eventHandler<DocumentAction::SetCanvasSize>(*this);
				}
			m_canvas_modifier->show();
		}

		template<class Source>
		void onActivated(Enum::Tag<ImageAction::New>, Ui::MenuItem&, Source& src)
		{
			if(m_img_creator == nullptr) [[likely]]
				{
					m_img_creator = std::make_unique<ImageCreatorDlg>(
					    src.window(), "Create new image", Size2d{512, 512}, Size2d{65535, 65535});
					m_img_creator->eventHandler<ImageAction::New>(*this);
				}
			m_img_creator->show();
		}

		template<class Source>
		void onActivated(Enum::Tag<ImageAction::Import>, Ui::MenuItem&, Source& src)
		{
			loadAndInsert<PixelStore::Image>("Image files", src.window());
		}

		template<class Source>
		void onActivated(Enum::Tag<ImageAction::Export>, Ui::MenuItem&, Source& src)
		{
			if(auto img = m_document->image(m_document->currentImage()); img != nullptr)
			{
				exportItem("Image files",
				           src.window(),
				           m_document->workingDirectory(),
				           std::filesystem::path{m_document->currentPalette().c_str()},
				           img->source.get());
			}
		}

		template<class Source>
		void onActivated(Enum::Tag<PaletteAction::New>, Ui::MenuItem&, Source& src)
		{
			if(m_empty_pal_creator == nullptr) [[likely]]
				{
					m_empty_pal_creator =
					    std::make_unique<EmptyPaletteCreatorDlg>(src.window(),
					                                             "Create empty palette",
					                                             Ui::Box::Orientation::Horizontal,
					                                             "Name");
					m_empty_pal_creator->eventHandler<PaletteAction::New>(*this);
				}
			m_empty_pal_creator->show();
		}

		template<class Source>
		void onActivated(Enum::Tag<PaletteAction::Generate>, Ui::MenuItem&, Source& src)
		{
			if(m_gen_palette == nullptr) [[likely]]
				{
					m_gen_palette =
					    std::make_unique<PaletteGenerateDlg>(src.window(), "Generate palette");
					m_gen_palette->eventHandler<PaletteAction::Generate>(*this);
				}
			m_gen_palette->show();
		}

		template<class Source>
		void onActivated(Enum::Tag<PaletteAction::Export>, Ui::MenuItem&, Source& src)
		{
			if(auto pal = m_document->palette(m_document->currentPalette()); pal != nullptr)
			{
				exportItem("Palette files",
				           src.window(),
				           m_document->workingDirectory(),
				           std::filesystem::path{m_document->currentPalette().c_str()},
				           pal->source.get());
			}
		}

		template<class Source>
		void onActivated(Enum::Tag<PaletteAction::Import>, Ui::MenuItem&, Source& src)
		{
			loadAndInsert<Model::Palette>("Palette files", src.window());
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

		template<auto>
		void confirmPositive(DocumentCreatorDlg& src)
		{
			auto doc = std::make_unique<Model::Document>(src.widget().inputField().value());

			auto filter_edit   = createWindow<AppWindowType::FilterGraphEditor>(*doc);
			auto img_edit      = createWindow<AppWindowType::ImageEditor>(*doc);
			auto doc_previewer = createWindow<AppWindowType::DocumentPreviewer>(*doc);

			m_document                                        = std::move(doc);
			m_windows.get<AppWindowType::FilterGraphEditor>() = std::move(filter_edit);
			m_windows.get<AppWindowType::ImageEditor>()       = std::move(img_edit);
			m_windows.get<AppWindowType::DocumentPreviewer>() = std::move(doc_previewer);

			m_doc_creator.reset();
		}

		template<auto>
		void dismiss(DocumentCreatorDlg&)
		{
			m_doc_creator.reset();
		}

		template<auto>
		void confirmPositive(CanvasSizeDlg& src)
		{
			m_document->canvasSize(src.widget().value());
			m_canvas_modifier.reset();

			if(auto output = m_windows.get<AppWindowType::DocumentPreviewer>().get();
			   output != nullptr)
				[[likely]] { output->widget().refresh(Model::Document::ForceUpdate{true}); }
		}

		template<auto>
		void dismiss(CanvasSizeDlg&)
		{
			m_canvas_modifier.reset();
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
		std::unique_ptr<Model::Document> m_document;

		Enum::Tuple<AppWindowType, AppWindowTypeTraits> m_windows;

		size_t m_window_count;

		std::unique_ptr<DocumentCreatorDlg> m_doc_creator;
		std::unique_ptr<CanvasSizeDlg> m_canvas_modifier;
		std::unique_ptr<ImageCreatorDlg> m_img_creator;
		std::unique_ptr<EmptyPaletteCreatorDlg> m_empty_pal_creator;
		std::unique_ptr<PaletteGenerateDlg> m_gen_palette;


		template<class T>
		void loadAndInsert(char const* filter_name, Ui::Container& dlg_owner)
		{
			std::filesystem::path filename;
			if(Ui::filenameSelect(
			       dlg_owner,
			       m_document->workingDirectory(),
			       filename,
			       Ui::FilenameSelectMode::Open,
			       [](char const* filename) { return fileValid(Enum::Empty<T>{}, filename); },
			       filter_name))
			{
				insert(Model::createItemNameFromFilename(filename.c_str()),
				       load(Enum::Empty<T>{}, filename.c_str()));
			}
		}

		void insert(Model::ItemName&& name, PixelStore::Image&& img)
		{
			if(m_document->insert(name, std::move(img)) == nullptr)
			{ throw std::string{"Item already exists"}; }

			auto node = m_document->inputNodeItem(name);
			m_windows.get<AppWindowType::FilterGraphEditor>()->widget().insertNodeEditor(*node);

			m_document->currentImage(std::move(name));

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
			if(m_document->insert(name, std::move(pal)) == nullptr)
			{ throw std::string{"Item already exists"}; }

			auto node = m_document->inputNodeItem(name);
			m_windows.get<AppWindowType::FilterGraphEditor>()->widget().insertNodeEditor(*node);

			m_document->currentPalette(std::move(name));

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
}

#endif