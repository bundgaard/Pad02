#pragma once
#define WIN32_LEAN_AND_MEAN
#include <memory>
#include <string>
#include <Windows.h>
#include <Windowsx.h>


namespace Pad
{
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

		static ATOM Register(HINSTANCE hInst);
		bool Create(const std::wstring& title);

		virtual void OnPaint(HWND)
		{
		}

		virtual bool OnCreate(HWND)
		{
			return false;
		}

		virtual void OnSize(int cx, int cy)
		{
		}
		virtual void OnChar(TCHAR ch, int cRepeat)
		{
			
		}
		virtual void OnCommand(WPARAM wParam, LPARAM lParam)
		{
			
		}

		virtual void OnMouseUp(int x, int y)
		{
		}

		virtual void OnMouseDown(int x, int y)
		{
		}

		virtual void OnMouseMove(int x, int y)
		{
		}

		virtual void OnDestroy()
		{
			
		}
		void SetTitle(const std::wstring& title)
		{
			SetWindowText(m_hwnd, title.c_str());
		}

		HWND GetWindow() const
		{
			return m_hwnd;
		}
	};
}
