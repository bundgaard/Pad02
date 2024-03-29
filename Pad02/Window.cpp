#include "precompiled.h"
#include "Window.h"

#include <sstream>
#include <string>
#include <strsafe.h>
#include "NotificationIcon.h"


HRESULT Pad::Window::Create(const HINSTANCE hInst, const std::wstring& title)
{
	
	HRESULT hr = S_OK;
	if (SUCCEEDED(hr))
	{
		WNDCLASSEX wc{};
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.hIcon = LoadIcon(nullptr, IDI_SHIELD);
		wc.hIconSm = LoadIcon(nullptr, IDI_SHIELD);
		wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(COLOR_WINDOW + 1));
		wc.lpfnWndProc = &Proc;
		m_hInst = wc.hInstance = hInst;
		wc.lpszClassName = app_class;
		wc.lpszMenuName = nullptr;
		wc.style = CS_HREDRAW | CS_VREDRAW;

		hr = RegisterClassEx(&wc) ? S_OK : E_FAIL;
	}
	
	if (SUCCEEDED(hr))
	{
		m_hwnd = CreateWindow(
			app_class,
			title.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			nullptr,
			(HMENU)nullptr,
			m_hInst,
			this);
		hr = m_hwnd ? S_OK : E_FAIL;

	}

	return hr;

	
}
void ErrorExit() 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    const auto dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPTSTR>(&lpMsgBuf),
        0, nullptr);

    // Display the error message and exit the process

    const auto lpDisplayBuf = LocalAlloc(LMEM_ZEROINIT,
                                         (lstrlen(static_cast<LPCTSTR>(lpMsgBuf)) + 40) * sizeof(TCHAR)); 
    StringCchPrintf(static_cast<LPTSTR>(lpDisplayBuf), 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        "a Function", dw, lpMsgBuf); 
    MessageBox(nullptr, static_cast<LPCTSTR>(lpDisplayBuf), TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}

LRESULT CALLBACK
Pad::Window::Proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* self;
	if (msg == WM_NCCREATE)
	{
		const auto lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		self = static_cast<Window*>(lpcs->lpCreateParams);
		self->m_hwnd = hwnd;

		if(SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self)))
		{
			fprintf(stderr, "failed to setWindowLongPtr\n");
			ErrorExit();

			return FALSE;
		}
	}
	else
	{
		self = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}
	if(self)
	{
		return self->OnProc(msg, wParam, lParam);
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}


LRESULT CALLBACK Pad::Window::OnProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		OnCreate(m_hwnd);
		break;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		return 0;
	case WM_SIZE:
		{
			const auto width = LOWORD(lParam);
			const auto height = HIWORD(lParam);
			OnSize(width, height);
		}
		break;
	case WM_CHAR:
		{
			const TCHAR ch = static_cast<TCHAR>(wParam);
			const int cRepeat = LOWORD(lParam);
			OnChar(ch, cRepeat);
		}
		break;
	case WM_LBUTTONDOWN:
		{
			const auto mouseX = GET_X_LPARAM(lParam);
			const auto mouseY = GET_Y_LPARAM(lParam);
			OnMouseUp(mouseX, mouseY);
		}
		return 0;
	case WM_LBUTTONUP:
		{
			
			const auto mouseX = GET_X_LPARAM(lParam);
			const auto mouseY = GET_Y_LPARAM(lParam);
			OnMouseDown(mouseX, mouseY);
		}
		return 0;
	case WM_MOUSEMOVE:
		{
			
			const auto mouseX = GET_X_LPARAM(lParam);
			const auto mouseY = GET_Y_LPARAM(lParam);
			OnMouseMove(mouseX, mouseY);
		}
		break;
	
	case WM_ERASEBKGND:
		return 1;
	case WM_PRINTCLIENT:
	case WM_PAINT:
		OnPaint(m_hwnd);
		break;
	case WM_DESTROY:
		OnDestroy();
		PostQuitMessage(0);
		break;
	default:
		if (msg >= WM_USER) 
		{
			return OnCustom(msg, wParam, lParam);
		}
		
		return DefWindowProc(m_hwnd, msg, wParam, lParam);
	}
}
