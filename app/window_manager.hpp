//@	{
//@	 "targets":[{"name":"window_manager.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"window_manager.o","rel":"implementation"}]
//@ }

#ifndef TEXPAINTER_APP_WINDOWMANAGER_HPP
#define TEXPAINTER_APP_WINDOWMANAGER_HPP

#include "./document_editor.hpp"
#include "./image_editor.hpp"
#include "./compositor.hpp"
#include "./image_creator.hpp"
#include "./palette_creator.hpp"
#include "./document_previewer.hpp"
#include "./size_input.hpp"
#include "./render_job_creator.hpp"
#include "./render_to_img_job_creator.hpp"
#include "./start_dialog.hpp"
#include "./show_help.hpp"

#include "model/document.hpp"
#include "model/window_type.hpp"
#include "model/palette_generate.hpp"
#include "pixel_store/image_io.hpp"
#include "ui/window.hpp"
#include "ui/image_view.hpp"
#include "ui/dialog.hpp"
#include "ui/filename_select.hpp"
#include "ui/context.hpp"
#include "ui/error_message_dialog.hpp"

#include <filesystem>

namespace Texpainter::App
{
	namespace detail
	{
		template<class EventHandler>
		using CompositorWindow = DocumentEditor<Compositor, EventHandler>;

		template<class EventHandler>
		using ImageEditWindow = DocumentEditor<ImageEditor, EventHandler>;

		template<class EventHandler>
		using DocumentPreviewWindow = DocumentEditor<DocumentPreviewer, EventHandler>;

		template<Model::WindowType, class EventHandler>
		struct WindowTypeTraits;

		template<class EventHandler>
		struct WindowTypeTraits<Model::WindowType::Compositor, EventHandler>
		{
			using type = std::unique_ptr<CompositorWindow<EventHandler>>;
			static constexpr char const* name() { return "Texpainter: Compositor"; }
		};

		template<class EventHandler>
		struct WindowTypeTraits<Model::WindowType::ImageEditor, EventHandler>
		{
			using type = std::unique_ptr<ImageEditWindow<EventHandler>>;
			static constexpr char const* name() { return "Texpainter: Image editor"; }
		};

		template<class EventHandler>
		struct WindowTypeTraits<Model::WindowType::DocumentPreviewer, EventHandler>
		{
			using type = std::unique_ptr<DocumentPreviewWindow<EventHandler>>;
			static constexpr char const* name() { return "Texpainter: Document preview"; }
		};

		template<WorkspaceAction, class EventHandler>
		struct WindowTypeFromWorkspaceAction
		{
		};

		template<class EventHandler>
		struct WindowTypeFromWorkspaceAction<WorkspaceAction::ShowCompositor, EventHandler>
		{
			static constexpr auto id = Model::WindowType::Compositor;
		};

		template<class EventHandler>
		struct WindowTypeFromWorkspaceAction<WorkspaceAction::ShowImageEditor, EventHandler>
		{
			static constexpr auto id = Model::WindowType::ImageEditor;
		};

		template<class EventHandler>
		struct WindowTypeFromWorkspaceAction<WorkspaceAction::ShowDocumentPreviewer, EventHandler>
		{
			static constexpr auto id = Model::WindowType::DocumentPreviewer;
		};

		struct StartWindow
		{
			Ui::Window owner{"Texpainter startup menu"};
			StartDialog start_dlg{owner};
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
		using WindowType = Model::WindowType;

		template<WorkspaceAction id>
		using WindowTypeFromWorkspaceAction =
		    detail::WindowTypeFromWorkspaceAction<id, WindowManager>;

		template<WindowType id>
		using WindowTypeTraits = detail::WindowTypeTraits<id, WindowManager>;

		template<WindowType id>
		static std::string make_window_title(Model::Document const& doc)
		{
			std::string window_title{WindowTypeTraits<id>::name()};
			window_title += ": ";
			window_title += doc.filename().c_str();
			return window_title;
		}


		template<WindowType id, class... Args>
		auto createWindow(Model::Document& doc, Args&&... args)
		{
			using T  = typename WindowTypeTraits<id>::type::element_type;
			auto ret = std::make_unique<T>(
			    make_window_title<id>(doc).c_str(), *this, doc, std::forward<Args>(args)...);
			ret->window().template eventHandler<id>(*this);
			ret->widget().template eventHandler<id>(*this);
			return ret;
		}

		using DocumentCreatorDlg       = Ui::Dialog<Ui::LabeledInput<SizeInput>>;
		using ExportJobCreatorDlg      = Ui::Dialog<RenderJobCreator>;
		using RenderToImgJobCreatorDlg = Ui::Dialog<RenderToImgJobCreator>;
		using CanvasSizeDlg            = Ui::Dialog<SizeInput>;
		using ImageCreatorDlg          = Ui::Dialog<ImageCreator>;
		using EmptyPaletteCreatorDlg   = Ui::Dialog<Ui::LabeledInput<Ui::TextEntry>>;
		using PaletteGenerateDlg       = Ui::Dialog<PaletteCreator>;

		using Windows = Enum::Tuple<WindowType, WindowTypeTraits>;

	public:
		[[nodiscard]] WindowManager(): m_start_win{std::make_unique<detail::StartWindow>()}
		{
			m_start_win->owner.show().resize(Size2d{280, 200});
			m_start_win->start_dlg.eventHandler(*this);
		}

		[[nodiscard]] WindowManager(char const* filename)
		    : m_document{load(Enum::Empty<Model::Document>{}, filename)}
		    , m_window_count{m_windows.size()}
		{
			Enum::forEachEnumItem<WindowType>([this](auto item) {
				m_windows.get<item.value>() = createWindow<item.value>(*m_document);
			});

			resetWindowPositions();

			m_windows.get<WindowType::ImageEditor>()->window().show();
		}

		void resetWindowPositions();

		template<WindowType>
		void onKeyUp(Ui::Window&, Ui::Scancode)
		{
		}

		template<WindowType window>
		void onKeyDown(Ui::Window&, Ui::Scancode scancode)
		{
			auto& key_state = Ui::Context::get().keyboardState();
			if(isShiftPressed(key_state) && key_state.numberOfPressedKeys() == 2)
			{
				switch(scancode.value())
				{
					case Ui::Scancodes::F1.value():
						createAndShowWindow<WindowType::ImageEditor>();
						return;
					case Ui::Scancodes::F2.value():
						createAndShowWindow<WindowType::Compositor>();
						return;
					case Ui::Scancodes::F3.value():
						createAndShowWindow<WindowType::DocumentPreviewer>();
						return;
				}
			}

			if constexpr(window != WindowType::DocumentPreviewer)
			{
				m_windows.template get<window>()->widget().onKeyDown(
				    scancode, Ui::Context::get().keyboardState());
			}
		}

		template<WindowType id>
		void onClose(Ui::Window&)
		{
			--m_window_count;
			saveNodLocs();
			m_document->windows(windowInfo());
			m_windows.get<id>().reset();
			if(m_window_count == 0) { Ui::Context::get().exit(); }
		}

		template<auto id, class Source>
		void handleException(char const* msg, Source& src)
		{
			m_err_box.show(src.owner(), "Texpainter", msg);
		}

		template<auto id>
		void handleException(char const* msg, Ui::Window& src)
		{
			m_err_box.show(src, "Texpainter", msg);
		}

		template<auto id>
		void handleException(Enum::Tag<id>, char const* msg, Ui::MenuItem&, Ui::Window& src)
		{
			m_err_box.show(src, "Texpainter", msg);
		}

		template<auto id, class Source>
		void handleException(Enum::Tag<id>, char const* msg, Ui::MenuItem&, Source& src)
		{
			m_err_box.show(src.window(), "Texpainter", msg);
		}

		template<auto id, class Source>
		requires(std::same_as<decltype(id), AppAction>) void onActivated(Enum::Tag<id>,
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

		template<WindowType item>
		void createAndShowWindow()
		{
			if(m_windows.get<item>() == nullptr)
			{
				m_windows.get<item>() = createWindow<item>(*m_document);
				auto& window          = m_windows.get<item>()->window();
				window.show();
				auto const& windows_stored = m_document->workspace().m_windows;
				if(windows_stored.get<item>().rect.size != Size2d{0, 0})
				{
					window.resize(windows_stored.get<item>().rect.size)
					    .move(Ui::ScreenCoordinates{windows_stored.get<item>().rect.location})
					    .maximized(windows_stored.get<item>().state.maximized())
					    .minimized(windows_stored.get<item>().state.minimized());
				}
				else
				{
					window.resize(Size2d{800, 600 - 28});
				}

				++m_window_count;
			}
			else
			{
				m_windows.get<item>()->window().show();
			}
		}

		template<WorkspaceAction item, class Source>
		requires(std::same_as<std::decay_t<decltype(WindowTypeFromWorkspaceAction<item>::id)>,
		                      WindowType>) void onActivated(Enum::Tag<item>, Ui::MenuItem&, Source&)
		{
			createAndShowWindow<WindowTypeFromWorkspaceAction<item>::id>();
		}

		template<WorkspaceAction item, class Source>
		void onActivated(Enum::Tag<item>, Ui::MenuItem&, Source&)
		{
			resetWindowPositions();
		}

		template<class Source>
		void onActivated(Enum::Tag<DocumentAction::New>, Ui::MenuItem&, Source& src)
		{
			createNewDoc<DocumentAction::New>(src.window());
		}

		template<class Source>
		void onActivated(Enum::Tag<DocumentAction::Save>, Ui::MenuItem&, Source& src)
		{
			saveDocument(src.window());
		}

		template<class Source>
		void onActivated(Enum::Tag<DocumentAction::SaveAs>, Ui::MenuItem&, Source& src)
		{
			saveDocumentAs(src.window());
		}

		template<class Source>
		void onActivated(Enum::Tag<DocumentAction::SaveCopy>, Ui::MenuItem&, Source& src)
		{
			saveDocumentCopy(src.window());
		}

		template<class Source>
		void onActivated(Enum::Tag<DocumentAction::Open>, Ui::MenuItem&, Source& src)
		{
			openDocument(src.window());
		}

		template<class Source>
		void onActivated(Enum::Tag<DocumentAction::RenderToImage>, Ui::MenuItem&, Source& src)
		{
			if(m_render_to_img_job_creator == nullptr) [[likely]]
				{
					m_render_to_img_job_creator =
					    std::make_unique<RenderToImgJobCreatorDlg>(src.window(), "Render to image");
					m_render_to_img_job_creator->eventHandler<DocumentAction::RenderToImage>(*this);
				}
			m_render_to_img_job_creator->show();
		}

		template<class Source>
		void onActivated(Enum::Tag<DocumentAction::Export>, Ui::MenuItem&, Source& src)
		{
			if(m_export_job_creator == nullptr) [[likely]]
				{
					m_export_job_creator =
					    std::make_unique<ExportJobCreatorDlg>(src.window(), "Export image");
					m_export_job_creator->eventHandler<DocumentAction::Export>(*this);
				}
			m_export_job_creator->show();
		}

		template<class Source>
		void onActivated(Enum::Tag<DocumentAction::SetCanvasSize>, Ui::MenuItem&, Source& src)
		{
			if(m_canvas_modifier == nullptr) [[likely]]
				{
					m_canvas_modifier = std::make_unique<CanvasSizeDlg>(src.window(),
					                                                    "Set canvas size",
					                                                    m_document->canvasSize(),
					                                                    Size2d{65535, 65535});
					m_canvas_modifier->eventHandler<DocumentAction::SetCanvasSize>(*this);
				}
			m_canvas_modifier->show();
		}

		template<class Source>
		void onActivated(Enum::Tag<ImageAction::New>, Ui::MenuItem&, Source& src)
		{
			if(m_img_creator == nullptr) [[likely]]
				{
					m_img_creator = std::make_unique<ImageCreatorDlg>(src.window(),
					                                                  "Create new image",
					                                                  m_document->canvasSize(),
					                                                  Size2d{65535, 65535});
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
		void onActivated(Enum::Tag<ImageAction::Delete>, Ui::MenuItem&, Source&)
		{
			saveNodLocs();
			m_document->eraseImage(m_document->currentImage());
			doUpdateFull();
		}

		template<class Source>
		void onActivated(Enum::Tag<PaletteAction::Delete>, Ui::MenuItem&, Source&)
		{
			saveNodLocs();
			m_document->erasePalette(m_document->currentPalette());
			doUpdateFull();
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
					                                             "Name: ");
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
		void handleException(char const* msg, ImageCreatorDlg& src)
		{
			m_err_box.show(src.owner(), "Texpainter: Creating image", msg);
		}

		template<auto>
		void handleException(char const* msg, EmptyPaletteCreatorDlg& src)
		{
			m_err_box.show(src.owner(), "Texpainter: Creating palette", msg);
		}

		template<auto>
		void handleException(char const* msg, PaletteGenerateDlg& src)
		{
			m_err_box.show(src.owner(), "Texpainter: Creating palette", msg);
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

		template<auto val>
		requires(!std::same_as<decltype(val), StartDialog::ControlId>) void confirmPositive(
		    DocumentCreatorDlg& src)
		{
			createDocument(src);
		}

		template<auto>
		void dismiss(DocumentCreatorDlg&)
		{
			m_doc_creator.reset();
		}

		template<auto>
		void confirmPositive(RenderToImgJobCreatorDlg& src)
		{
			auto opts = src.widget().value();
			insert(std::move(opts.name),
			       render(*m_document, Model::Document::ForceUpdate{true}, opts.supersampling));

			m_render_to_img_job_creator.reset();
		}

		template<auto>
		void dismiss(RenderToImgJobCreatorDlg&)
		{
			m_render_to_img_job_creator.reset();
		}

		template<auto>
		void confirmPositive(ExportJobCreatorDlg& src)
		{
			auto opts = src.widget().value();
			store(render(*m_document, Model::Document::ForceUpdate{true}, opts.supersampling),
			      opts.filename.c_str());

			m_export_job_creator.reset();
		}

		template<auto>
		void dismiss(ExportJobCreatorDlg&)
		{
			m_export_job_creator.reset();
		}


		template<auto>
		void confirmPositive(CanvasSizeDlg& src)
		{
			m_document->canvasSize(src.widget().value());
			m_canvas_modifier.reset();

			if(auto output = m_windows.get<WindowType::DocumentPreviewer>().get();
			   output != nullptr)
			{ output->widget().refresh(Model::Document::ForceUpdate{true}); }
		}

		template<auto>
		void dismiss(CanvasSizeDlg&)
		{
			m_canvas_modifier.reset();
		}

		template<WindowType id, class Src>
		void onUpdated(Src&)
		{
			doUpdate();
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

		Model::Windows windowInfo() const;

		void loadDocument(char const* filename);

		template<StartDialog::ControlId>
		void onClicked(Ui::Button&);

		template<StartDialog::ControlId>
		void handleException(char const* msg, Ui::Button&)
		{
			m_err_box.show(m_start_win->owner, "Texpainter: Creating palette", msg);
		}

		template<StartDialog::ControlId>
		void confirmPositive(DocumentCreatorDlg& src)
		{
			createDocument(src);
			m_start_win.reset();
		}

		template<StartDialog::ControlId id>
		void confirmNegative(DocumentCreatorDlg&)
		{
			m_doc_creator.reset();
			m_start_win->start_dlg.resetButtonState<id>();
		}

	private:
		std::unique_ptr<Model::Document> m_document;

		Windows m_windows;
		size_t m_window_count;

		std::unique_ptr<DocumentCreatorDlg> m_doc_creator;
		std::unique_ptr<RenderToImgJobCreatorDlg> m_render_to_img_job_creator;
		std::unique_ptr<ExportJobCreatorDlg> m_export_job_creator;
		std::unique_ptr<CanvasSizeDlg> m_canvas_modifier;
		std::unique_ptr<ImageCreatorDlg> m_img_creator;
		std::unique_ptr<EmptyPaletteCreatorDlg> m_empty_pal_creator;
		std::unique_ptr<PaletteGenerateDlg> m_gen_palette;

		std::unique_ptr<detail::StartWindow> m_start_win;

		Ui::ErrorMessageDialog m_err_box;

		bool openDocument(Ui::Container& dlg_owner)
		{
			std::filesystem::path filename;
			if(Ui::filenameSelect(
			       dlg_owner,
			       m_document != nullptr ? m_document->workingDirectory() : "",
			       filename,
			       Ui::FilenameSelectMode::Open,
			       [](char const* filename) {
				       return fileValid(Enum::Empty<Model::Document>{}, filename);
			       },
			       "Texpainter documents"))
			{
				loadDocument(filename.c_str());
				return true;
			}
			return false;
		}

		void saveDocumentAs(Ui::Container& dlg_owner)
		{
			std::filesystem::path filename;
			if(Ui::filenameSelect(
			       dlg_owner,
			       m_document->workingDirectory(),
			       filename,
			       Ui::FilenameSelectMode::Save,
			       [](char const* filename) {
				       return fileValid(Enum::Empty<Model::Document>{}, filename);
			       },
			       "Texpainter documents"))
			{
				saveDocument(filename.c_str());
				m_document->filename(std::move(filename));
				updateWindowTitles();
			}
		}

		void saveDocumentCopy(Ui::Container& dlg_owner)
		{
			std::filesystem::path filename;
			if(Ui::filenameSelect(
			       dlg_owner,
			       m_document->workingDirectory(),
			       filename,
			       Ui::FilenameSelectMode::Save,
			       [](char const* filename) {
				       return fileValid(Enum::Empty<Model::Document>{}, filename);
			       },
			       "Texpainter documents"))
			{ saveDocument(filename.c_str()); }
		}

		void updateWindowTitles()
		{
			Enum::forEachEnumItem<WindowType>([&windows = m_windows, &doc = *m_document](auto i) {
				if(auto editor = windows.get<i.value>().get(); editor != nullptr)
				{ editor->window().title(make_window_title<i.value>(doc).c_str()); }
			});
		}

		void saveDocument(Ui::Container& dlg_owner)
		{
			if(m_document->filename().empty())
			{
				saveDocumentAs(dlg_owner);
				return;
			}
			saveDocument(m_document->filename().c_str());
		}

		void saveDocument(char const* filename)
		{
			saveNodLocs();

			m_document->windows(windowInfo());

			store(*m_document, filename);
		}

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

			if(auto compositor = m_windows.get<WindowType::Compositor>().get();
			   compositor != nullptr)
			{
				auto node = std::as_const(*m_document).inputNodeItem(name);
				compositor->widget().insertNodeEditor(*node);
			}

			m_document->currentImage(std::move(name));

			if(auto img_editor = m_windows.get<WindowType::ImageEditor>().get();
			   img_editor != nullptr)
			{
				img_editor->widget().refresh();
				img_editor->window().show();
			}

			if(auto output = m_windows.get<WindowType::DocumentPreviewer>().get();
			   output != nullptr)
			{ output->widget().refresh(); }
		}

		void insert(Model::ItemName&& name, Model::Palette&& pal)
		{
			if(m_document->insert(name, std::move(pal)) == nullptr)
			{ throw std::string{"Item already exists"}; }

			if(auto compositor = m_windows.get<WindowType::Compositor>().get();
			   compositor != nullptr)
			{
				auto node = std::as_const(*m_document).inputNodeItem(name);
				compositor->widget().insertNodeEditor(*node);
			}

			m_document->currentPalette(std::move(name));

			if(auto img_editor = m_windows.get<WindowType::ImageEditor>().get();
			   img_editor != nullptr)
			{
				img_editor->widget().refresh();
				img_editor->window().show();
			}

			if(auto output = m_windows.get<WindowType::DocumentPreviewer>().get();
			   output != nullptr)
			{ output->widget().refresh(); }
		}

		void doUpdate()
		{
			if(auto editor = m_windows.get<WindowType::ImageEditor>().get(); editor != nullptr)
			{ editor->widget().refresh(); }

			if(auto output = m_windows.get<WindowType::DocumentPreviewer>().get();
			   output != nullptr)
			{ output->widget().refresh(); }
		}

		void doUpdateFull()
		{
			Enum::forEachEnumItem<WindowType>([&windows = m_windows](auto item) {
				if(auto editor = windows.get<item.value>().get(); editor != nullptr)
				{ editor->widget().refresh(); }
			});
		}

		void saveNodLocs()
		{
			if(auto compositor = m_windows.get<WindowType::Compositor>().get();
			   compositor != nullptr)
			{ m_document->nodeLocations(compositor->widget().nodeLocations()); }
		}

		template<auto action>
		void createNewDoc(Ui::Window& owner)
		{
			if(m_doc_creator == nullptr) [[likely]]
				{
					m_doc_creator = std::make_unique<DocumentCreatorDlg>(
					    owner,
					    "Create new document",
					    Ui::Box::Orientation::Vertical,
					    "Canvas size:",
					    m_document != nullptr ? m_document->canvasSize() : Size2d{512, 512},
					    Size2d{65535, 65535});
					m_doc_creator->eventHandler<action>(*this);
				}
			m_doc_creator->show();
		}


		void createDocument(DocumentCreatorDlg& src)
		{
			auto doc = std::make_unique<Model::Document>(src.widget().inputField().value());

			Windows tmp;
			Enum::forEachEnumItem<WindowType>(
			    [&tmp, &doc, this](auto i) { tmp.get<i.value>() = createWindow<i.value>(*doc); });

			m_document = std::move(doc);
			m_windows  = std::move(tmp);

			m_doc_creator.reset();
			resetWindowPositions();
			m_windows.get<WindowType::ImageEditor>()->window().show();
		}
	};

	template<>
	inline void WindowManager::onClicked<StartDialog::ControlId::New>(Ui::Button&)
	{
		createNewDoc<StartDialog::ControlId::New>(m_start_win->owner);
	}

	template<>
	inline void WindowManager::onClicked<StartDialog::ControlId::Open>(Ui::Button& src)
	{
		if(openDocument(m_start_win->owner)) { m_start_win.reset(); }
		else
		{
			src.state(false);
		}
	}

	template<>
	inline void WindowManager::onClicked<StartDialog::ControlId::ReadUsrGuide>(Ui::Button& src)
	{
		Help::showHelp("quick-start_guide/index.html");
		src.state(false);
	}

	template<>
	inline void WindowManager::onClicked<StartDialog::ControlId::Quit>(Ui::Button&)
	{
		Ui::Context::get().exit();
	}
}

#endif