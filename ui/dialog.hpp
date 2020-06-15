//@	{"targets":[{"name":"dialog.hpp","type":"include"}]}

#ifndef TEXPAINTER_UI_DIALOG_HPP
#define TEXPAINTER_UI_DIALOG_HPP

#include "./window.hpp"
#include "./button.hpp"
#include "./box.hpp"
#include "./filler.hpp"
#include "utils/add_member_if.hpp"

#include <array>
#include <cassert>

namespace Texpainter::Ui
{
	struct DialogOkCancel
	{
		static constexpr const char* dismiss() noexcept
		{
			return "Cancel";
		}

		static constexpr const char* confirmPositive() noexcept
		{
			return "OK";
		}

		static constexpr const char* confirmNegative() noexcept
		{
			return nullptr;
		}

		static constexpr const char* user1() noexcept
		{
			return nullptr;
		}

		static constexpr const char* user2() noexcept
		{
			return nullptr;
		}
	};

	struct DialogCancel
	{
		static constexpr const char* dismiss() noexcept
		{
			return "Cancel";
		}

		static constexpr const char* confirmPositive() noexcept
		{
			return nullptr;
		}

		static constexpr const char* confirmNegative() noexcept
		{
			return nullptr;
		}

		static constexpr const char* user1() noexcept
		{
			return nullptr;
		}

		static constexpr const char* user2() noexcept
		{
			return nullptr;
		}
	};


	struct DialogNull
	{
		static constexpr const char* dismiss() noexcept
		{
			return nullptr;
		}

		static constexpr const char* confirmPositive() noexcept
		{
			return nullptr;
		}

		static constexpr const char* confirmNegative() noexcept
		{
			return nullptr;
		}

		static constexpr const char* user1() noexcept
		{
			return nullptr;
		}

		static constexpr const char* user2() noexcept
		{
			return nullptr;
		}
	};

	struct DialogOk
	{
		static constexpr const char* dismiss() noexcept
		{
			return nullptr;
		}

		static constexpr const char* confirmPositive() noexcept
		{
			return "OK";
		}

		static constexpr const char* confirmNegative() noexcept
		{
			return nullptr;
		}

		static constexpr const char* user1() noexcept
		{
			return nullptr;
		}

		static constexpr const char* user2() noexcept
		{
			return nullptr;
		}
	};

	template<size_t N>
	std::array<Button, N> buttons_create(Container& cnt);

	template<>
	inline std::array<Button, 0> buttons_create<0>(Container& cnt)
	{
		return std::array<Button, 0>{};
	}

	template<>
	inline std::array<Button, 1> buttons_create<1>(Container& cnt)
	{
		return std::array<Button, 1>{Button(cnt, "")};
	}

	template<>
	inline std::array<Button, 2> buttons_create<2>(Container& cnt)
	{
		return std::array<Button, 2>{Button(cnt, ""), Button(cnt, "")};
	}

	template<>
	inline std::array<Button, 3> buttons_create<3>(Container& cnt)
	{
		return std::array<Button, 3>{Button(cnt, ""), Button(cnt, ""), Button(cnt, "")};
	}

	template<>
	inline std::array<Button, 4> buttons_create<4>(Container& cnt)
	{
		return std::array<Button, 4>{Button(cnt, ""), Button(cnt, ""), Button(cnt, ""), Button(cnt, "")};
	}

	template<>
	inline std::array<Button, 5> buttons_create<5>(Container& cnt)
	{
		return std::array<Button, 5>{
		   Button(cnt, ""), Button(cnt, ""), Button(cnt, ""), Button(cnt, ""), Button(cnt, "")};
	}

	template<class Widget, class DialogTraits = DialogOkCancel>
	class Dialog
	{
	public:
		enum class ButtonId : int
		{
			DISMISS,
			CONFIRM_NEGATIVE,
			CONFIRM_POSITIVE,
			USER_1,
			USER_2
		};


		Dialog& operator=(Dialog&&) = delete;
		Dialog(Dialog&&) = delete;

		template<class... WidgetParams>
		Dialog(Container& owner, const char* title, const WidgetParams&... params):
		   m_window(title, &owner),
		   m_content(m_window, Box::Orientation::Vertical),
		   m_widget(m_content.insertMode({2, Box::Fill | Box::Expand}), params...),
		   m_buttons_outer(m_content.insertMode(Box::InsertMode{0, 0}), Box::Orientation::Horizontal),
		   m_filler_l(m_buttons_outer.insertMode({0, Box::Fill | Box::Expand})),
		   m_buttons_box(m_buttons_outer.insertMode({0, 0}), Box::Orientation::Horizontal),
		   m_buttons(buttons_create<button_count()>(
		      m_buttons_box.homogenous(true).insertMode(Box::InsertMode{2, Box::Fill | Box::Expand}))),
		   m_filler_r(m_buttons_outer.insertMode({0, Box::Fill | Box::Expand}))
		{
			if(has_dismiss())
			{
				assert(ButtonIndex::dismiss() != -1);
				m_buttons[ButtonIndex::dismiss()].label(DialogTraits::dismiss());
			}
			if(has_confirm_neg())
			{
				assert(ButtonIndex::confirmNegative() != -1);
				m_buttons[ButtonIndex::confirmNegative()].label(DialogTraits::confirmNegative());
			}
			if(has_confirm_pos())
			{
				assert(ButtonIndex::confirmPositive() != -1);
				m_buttons[ButtonIndex::confirmPositive()].label(DialogTraits::confirmPositive());
			}
			if(has_user_1())
			{
				assert(ButtonIndex::user1() != -1);
				m_buttons[ButtonIndex::user1()].label(DialogTraits::user1());
			}
			if(has_user_2())
			{
				assert(ButtonIndex::user2() != -1);
				m_buttons[ButtonIndex::user2()].label(DialogTraits::user2());
			}

			focus_select();


			m_window.modal(true).show();
		}

		template<auto id, class EventHandler>
		Dialog& eventHandler(EventHandler& cb_obj)
		{
			static_assert(button_count() != 0, "");
			m_vtable = Vtable{cb_obj, id};
			r_cb_obj = &cb_obj;
			m_id = static_cast<int>(id);
			button_callback_assign<ButtonIndex::dismiss()>(ButtonId::DISMISS);
			button_callback_assign<ButtonIndex::confirmNegative()>(ButtonId::CONFIRM_NEGATIVE);
			button_callback_assign<ButtonIndex::confirmPositive()>(ButtonId::CONFIRM_POSITIVE);
			button_callback_assign<ButtonIndex::user1()>(ButtonId::USER_1);
			button_callback_assign<ButtonIndex::user2()>(ButtonId::USER_2);
			m_window.eventHandler<id>(*this, 0);
			return *this;
		}

		Widget& widget() noexcept
		{
			return m_widget;
		}

		const Widget& widget() const noexcept
		{
			return m_widget;
		}

		void clicked(Button& button, ButtonId id)
		{
			button.state(0);
			switch(id)
			{
				case ButtonId::DISMISS: m_vtable.dismiss(r_cb_obj, *this, m_id); break;
				case ButtonId::CONFIRM_NEGATIVE: m_vtable.confirm_negative(r_cb_obj, *this, m_id); break;
				case ButtonId::CONFIRM_POSITIVE: m_vtable.confirm_positive(r_cb_obj, *this, m_id); break;
				case ButtonId::USER_1: m_vtable.user_1(r_cb_obj, *this, m_id); break;
				case ButtonId::USER_2: m_vtable.user_2(r_cb_obj, *this, m_id); break;
			}
			//	We are potentially dead now. Therfore, do not touch button.
		}

		void onClose(Window& win, int id)
		{
			if(has_dismiss()) { m_vtable.dismiss(r_cb_obj, *this, m_id); }
			else
			{
				m_vtable.confirm_positive(r_cb_obj, *this, m_id);
			}
		}

		void onKeyDown(Window& win, int scancode)
		{
			switch(scancode)
			{
				case 1: //	ESC
					if(has_dismiss()) { m_vtable.dismiss(r_cb_obj, *this, m_id); }
					else
					{
						m_vtable.confirm_positive(r_cb_obj, *this, m_id);
					}
					break;
				case 28: //	ENTER
					m_vtable.confirm_positive(r_cb_obj, *this, m_id);
					break;
				default: break;
			}
		}

		void onKeyUp(Window& win, int scancode)
		{
		}

		void show()
		{
			m_window.show();
		}

	private:
		static constexpr bool has_dismiss() noexcept
		{
			return DialogTraits::dismiss() != nullptr;
		}

		static constexpr bool has_confirm_neg() noexcept
		{
			return DialogTraits::confirmNegative() != nullptr;
		}

		static constexpr bool has_confirm_pos() noexcept
		{
			return DialogTraits::confirmPositive() != nullptr;
		}

		static constexpr bool has_user_1() noexcept
		{
			return DialogTraits::user1() != nullptr;
		}

		static constexpr bool has_user_2() noexcept
		{
			return DialogTraits::user2() != nullptr;
		}

		static constexpr int button_count() noexcept
		{
			return static_cast<int>(has_dismiss()) + static_cast<int>(has_confirm_neg())
			       + static_cast<int>(has_confirm_pos()) + static_cast<int>(has_user_1())
			       + static_cast<int>(has_user_2());
		}

		static constexpr bool has_button(ButtonId id)
		{
			switch(id)
			{
				case ButtonId::DISMISS: return has_dismiss();
				case ButtonId::CONFIRM_NEGATIVE: return has_confirm_neg();
				case ButtonId::CONFIRM_POSITIVE: return has_confirm_pos();
				case ButtonId::USER_1: return has_user_1();
				case ButtonId::USER_2: return has_user_2();
			}
			return 0;
		}

		class ButtonIndex
		{
		public:
			static constexpr int dismiss() noexcept
			{
				return s_dismiss;
			}

			static constexpr int confirmPositive() noexcept
			{
				return s_confirm_positive;
			}

			static constexpr int confirmNegative() noexcept
			{
				return s_confirm_negative;
			}

			static constexpr int user1() noexcept
			{
				return s_user_1;
			}

			static constexpr int user2() noexcept
			{
				return s_user_2;
			}

		private:
			template<ButtonId button>
			static constexpr int get() noexcept
			{
				int l = 0;
				for(int k = 0; k < 5; ++k)
				{
					if(has_button(s_button_order[k]))
					{
						if(button == s_button_order[k]) { return l; }
						++l;
					}
				}
				return -1;
			}

			static constexpr int s_dismiss = get<ButtonId::DISMISS>();
			static constexpr int s_confirm_positive = get<ButtonId::CONFIRM_POSITIVE>();
			static constexpr int s_confirm_negative = get<ButtonId::CONFIRM_NEGATIVE>();
			static constexpr int s_user_1 = get<ButtonId::USER_1>();
			static constexpr int s_user_2 = get<ButtonId::USER_2>();

#ifdef _WIN32
			static constexpr ButtonId s_button_order[] = {ButtonId::USER_1,
			                                              ButtonId::USER_2,
			                                              ButtonId::CONFIRM_POSITIVE,
			                                              ButtonId::CONFIRM_NEGATIVE,
			                                              ButtonId::DISMISS};
#else
			static constexpr ButtonId s_button_order[] = {ButtonId::CONFIRM_NEGATIVE,
			                                              ButtonId::DISMISS,
			                                              ButtonId::USER_1,
			                                              ButtonId::USER_2,
			                                              ButtonId::CONFIRM_POSITIVE};
#endif
		};


		template<int index, bool dummy = true>
		struct button_callback_assign_do
		{
			static void assign(Dialog& dlg, ButtonId id)
			{
				dlg.m_buttons[index].callback(dlg, id);
			}
		};

		template<bool dummy>
		struct button_callback_assign_do<-1, dummy>
		{
			static void assign(Dialog& dlg, ButtonId id)
			{
			}
		};

		template<int index>
		void button_callback_assign(ButtonId id)
		{
			button_callback_assign_do<index>::assign(*this, id);
		}

		typedef void (*DialogCallback)(void* cb_obj, Dialog& self, int id);

		typedef AddMemberIf<has_dismiss(), DialogCallback, 0> Dismiss;
		typedef AddMemberIf<has_confirm_neg(), DialogCallback, 1> ConfirmNeg;
		typedef AddMemberIf<has_confirm_pos(), DialogCallback, 2> ConfirmPos;
		typedef AddMemberIf<has_user_1(), DialogCallback, 3> User1;
		typedef AddMemberIf<has_user_2(), DialogCallback, 4> User2;

		class Vtable: Dismiss, ConfirmNeg, ConfirmPos, User1, User2
		{
		public:
			Vtable()
			{
			}
			template<class Callback, class IdType>
			explicit Vtable(Callback& cb, IdType) noexcept
			{
				Dismiss::value(call<Dismiss, Callback, IdType, has_dismiss()>::callback);
				ConfirmNeg::value(call<ConfirmNeg, Callback, IdType, has_confirm_neg()>::callback);
				ConfirmPos::value(call<ConfirmPos, Callback, IdType, has_confirm_pos()>::callback);
				User1::value(call<User1, Callback, IdType, has_user_1()>::callback);
				User2::value(call<User2, Callback, IdType, has_user_2()>::callback);
			}

			void dismiss(void* cb_obj, Dialog& dlg, int id)
			{
				Dismiss::value()(cb_obj, dlg, id);
			}

			void confirm_negative(void* cb_obj, Dialog& dlg, int id)
			{
				ConfirmNeg::value()(cb_obj, dlg, id);
			}

			void confirm_positive(void* cb_obj, Dialog& dlg, int id)
			{
				ConfirmPos::value()(cb_obj, dlg, id);
			}

			void user_1(void* cb_obj, Dialog& dlg, int id)
			{
				User1::value()(cb_obj, dlg, id);
			}

			void user_2(void* cb_obj, Dialog& dlg, int id)
			{
				User2::value()(cb_obj, dlg, id);
			}

		private:
			template<class Action, class Callback, class IdType, bool enable>
			struct call
			{
				static void callback(void* cb_obj, Dialog& self, int id)
				{
				}
			};

			template<class Callback, class IdType>
			struct call<Dismiss, Callback, IdType, true>
			{
				static void callback(void* cb_obj, Dialog& self, int id)
				{
					reinterpret_cast<Callback*>(cb_obj)->dismiss(self, static_cast<IdType>(id));
				}
			};

			template<class Callback, class IdType>
			struct call<ConfirmNeg, Callback, IdType, true>
			{
				static void callback(void* cb_obj, Dialog& self, int id)
				{
					reinterpret_cast<Callback*>(cb_obj)->confirmNegative(self, static_cast<IdType>(id));
				}
			};

			template<class Callback, class IdType>
			struct call<ConfirmPos, Callback, IdType, true>
			{
				static void callback(void* cb_obj, Dialog& self, int id)
				{
					reinterpret_cast<Callback*>(cb_obj)->confirmPositive(self, static_cast<IdType>(id));
				}
			};

			template<class Callback, class IdType>
			struct call<User1, Callback, IdType, true>
			{
				static void callback(void* cb_obj, Dialog& self, int id)
				{
					reinterpret_cast<Callback*>(cb_obj)->user1(self, static_cast<IdType>(id));
				}
			};

			template<class Callback, class IdType>
			struct call<User2, Callback, IdType, true>
			{
				static void callback(void* cb_obj, Dialog& self, int id)
				{
					reinterpret_cast<Callback*>(cb_obj)->user2(self, static_cast<IdType>(id));
				}
			};
		};

		void focus_select()
		{
			if(has_confirm_pos())
			{
				m_buttons[ButtonIndex::confirmPositive()].focus();
				return;
			}
			if(has_dismiss())
			{
				m_buttons[ButtonIndex::dismiss()].focus();
				return;
			}
			if(has_user_1())
			{
				m_buttons[ButtonIndex::user1()].focus();
				return;
			}
			if(has_user_2())
			{
				m_buttons[ButtonIndex::user2()].focus();
				return;
			}
			if(has_confirm_neg())
			{
				m_buttons[ButtonIndex::confirmNegative()].focus();
				return;
			}
		}

		int m_id;
		void* r_cb_obj;
		Vtable m_vtable;

		Window m_window;
		Box m_content;
		Widget m_widget;
		Box m_buttons_outer;
		Filler m_filler_l;
		Box m_buttons_box;
		std::array<Button, button_count()> m_buttons;
		Filler m_filler_r;
	};

}

#endif