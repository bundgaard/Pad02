#pragma once
#define WIN32_LEAN_AND_MEAN
#include <memory>
#include <string>
#include <Windows.h>
#include <Windowsx.h>
#include <wrl.h>


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

		static auto Register(HINSTANCE hInst) -> ATOM;
		auto Create(const std::wstring& title) -> bool;

		virtual auto OnPaint(HWND)-> void
		{
		}

		virtual auto OnCreate(HWND) -> bool
		{
			return false;
		}

		virtual auto OnSize(int cx, int cy)-> void
		{
		}
		virtual auto OnChar(TCHAR ch, int cRepeat)-> void
		{
			
		}
		virtual auto OnCommand(WPARAM wParam, LPARAM lParam)-> void
		{
			
		}

		virtual auto OnMouseUp(int x, int y)-> void
		{
		}

		virtual auto OnMouseDown(int x, int y)-> void
		{
		}

		virtual auto OnMouseMove(int x, int y)-> void
		{
		}

		virtual auto OnDestroy()-> void
		{
			
		}
		auto SetTitle(const std::wstring& title)
		{
			SetWindowText(m_hwnd, title.c_str());
		}

		auto GetWindow() const
		{
			return m_hwnd;
		}
	};
}
