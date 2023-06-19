#pragma once
#include "precompiled.h"


namespace Pad
{
	using namespace Microsoft::WRL;

	constexpr wchar_t app_class[] = L"PAD_WINDOW";

	class Window
	{
		HWND m_hwnd;
		HINSTANCE m_hInst;

		static LRESULT CALLBACK Proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK OnProc(UINT msg, WPARAM wParam, LPARAM lParam);

	public:
		Window(const HINSTANCE hInst) : m_hwnd(nullptr), m_hInst(hInst)
		{
		};
		~Window() = default;

		
		auto Create(const HINSTANCE hInst, const std::wstring& title) -> HRESULT;

		virtual auto OnPaint(HWND) -> void
		{
		}

		virtual auto OnCreate(HWND) -> bool
		{
			return false;
		}

		virtual auto OnSize(int cx, int cy) -> void
		{
		}

		virtual auto OnChar(TCHAR ch, int cRepeat) -> void
		{
		}

		virtual auto OnCommand(WPARAM wParam, LPARAM lParam) -> void
		{
		}

		virtual auto OnMouseUp(int x, int y) -> void
		{
		}

		virtual auto OnMouseDown(int x, int y) -> void
		{
		}

		virtual auto OnMouseMove(int x, int y) -> void
		{
		}

		virtual auto OnDestroy() -> void
		{
		}

		virtual auto OnCustom(UINT, WPARAM, LPARAM) -> LRESULT
		{
			return 0;
		}

		auto SetTitle(const std::wstring& title)
		{
			SetWindowText(m_hwnd, title.c_str());
		}

		auto GetTitle() const -> std::wstring
		{
			auto len = GetWindowTextLength(m_hwnd)+1; // plus the zero 
			std::wstring title;
			title.resize(len);
			GetWindowText(m_hwnd, title.data(), gsl::narrow_cast<int>(title.size()));
			return title;
		}

		auto GetInstance() const -> HWND
		{
			return m_hwnd;
		}

		auto Show() const
		{
			ShowWindow(m_hwnd, SW_SHOW);
			UpdateWindow(m_hwnd);
		}
	};


	class Menu
	{
		HMENU HMenu;

	public:
		Menu()
		{
			HMenu = CreateMenu();
		}

		virtual ~Menu()
		{
			DestroyMenu(HMenu);
		}


		auto AppendString(const std::wstring caption, UINT_PTR id) const
		{
			AppendMenuW(HMenu, MF_STRING, id, caption.c_str());
		}

		auto Append(const std::wstring caption, UINT_PTR id) const
		{
			AppendMenuW(HMenu, MF_POPUP, id, caption.c_str());
		}

		[[nodiscard]] auto GetInstance() const -> HMENU
		{
			return HMenu;
		}
	};

	class PopupMenu : public Menu
	{
		HMENU HMenu;

	public:
		PopupMenu()
		{
			HMenu = CreatePopupMenu();
		}
	};
}
