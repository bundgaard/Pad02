#define WIN32_LEAN_AND_MEAN
#include <algorithm>
#include <wrl.h>
#include <Windows.h>
#include <Windowsx.h>
#include <ShlObj.h>

#include <commdlg.h>

#include <dwrite.h>
#include <fstream>


#include <sstream>
#include <vector>

#include "File.h"
#include "Window.h"

#include "Menu.h"
#include "Graphic.h"
#include "Text.h"
#include "NotificationIcon.h"


#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2d1.lib")

using namespace Microsoft::WRL;

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)



extern void ErrorExit();
std::shared_ptr<Pad02::Graphic> g = nullptr;

struct Caret
{
	float X;
	float Y;
	float Height;
	float Width;


	auto DrawCaret() -> void
	{
		g->DrawLine(D2D1::Point2F(X, Y), D2D1::Point2F(Width, Height));
	}
};

struct Mouse
{
	float X1;
	float Y1;
	float X2;
	float Y2;
	bool Moving;
	bool Down;
};


std::unique_ptr<Pad02::Text> text = nullptr;


class D2Window : public Pad::Window
{
	bool isTrailing = false;
	bool isInside = false;
	std::vector<TCHAR> m_buf;
	Mouse position{};

public:
	explicit D2Window(HINSTANCE hInst) : Window(hInst)
	{
	}


	void OnChar(const TCHAR ch, int cRepeat) override
	{
		std::wostringstream debug;
		debug << ch << std::endl;

		OutputDebugString(debug.str().c_str());
		if (ch == VK_BACK)
		{
			m_buf.pop_back();
		}
		else if (ch == VK_TAB)
		{
			for (unsigned i = 0; i < 4; i++)
			{
				m_buf.push_back(L' ');
			}
		}
		else
		{
			m_buf.push_back(ch);
		}
	}

	void OnPaint(HWND hwnd) override
	{
		g->BeginDraw();
		const auto title = std::wstring(m_buf.begin(), m_buf.end());
		auto textlayout = text->CreateTextLayout(title,
		                                         600,
		                                         600);
		g->DrawTextLayout(D2D1::Point2F(0.f, 0.f), textlayout);

		g->DrawLine(D2D1::Point2F(position.X1, position.Y1),
		            D2D1::Point2F(position.X2, position.Y2));

		g->EndDraw();
	}

	void OnMouseMove(int x, int y) override
	{
		std::wostringstream coords;
		if (position.Down)
		{
			position.Moving = true;
			position.X2 = static_cast<float>(x);
			position.Y2 = static_cast<float>(y);
			OutputDebugStringW(L"enable position moving\n");
		}
		if (!position.Down)
		{
			OutputDebugStringW(L"disable position moving\n");
			position.Moving = false;
		}
		coords << L"X=" << x << L",Y=" << y;
		SetTitle(coords.str());
	}

	void OnMouseDown(int x, int y) override
	{
		position.Down = true;
		OutputDebugStringW(L"Mouse Down\n");
		position.X1 = static_cast<float>(x);
		position.Y1 = static_cast<float>(y);

		HitTest(x, y);
	}

	void OnMouseUp(int x, int y) override
	{
		position.Down = false;
		OutputDebugStringW(L"Mouse Up\n");
		position.X2 = static_cast<float>(x);
		position.Y2 = static_cast<float>(y);

		HitTest(x, y);
	}

	void OnSize(int width, int height) override
	{
		g->Resize(width, height);
	}

	void HitTest(const int& x, const int& y)
	{
		BOOL trailing = FALSE;
		BOOL inside = FALSE;
		DWRITE_HIT_TEST_METRICS metrics{};

		text->HitTestPoint(x, y);
		/*HRESULT hr = pTextLayout->HitTestPoint(
			static_cast<FLOAT>(x),
			static_cast<FLOAT>(y),
			&trailing,
			&inside,
			&metrics);
		ValidateResult(hr, "failed to get hit test");*/
	}

	void OnCommand(WPARAM wParam, LPARAM lParam) override
	{
		
		std::wstring filename;
		filename.resize(MAX_PATH);
		OPENFILENAME ofn{};
		ofn.lStructSize = sizeof(OPENFILENAMEW);
		ofn.hInstance = HINST_THISCOMPONENT;
		ofn.hwndOwner = GetInstance();
		ofn.lpstrCustomFilter = nullptr;
		ofn.nMaxCustFilter = 0;
		ofn.nFilterIndex = 0;
		ofn.lpstrFile = filename.data();
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFileTitle = nullptr;
		ofn.nMaxFileTitle = MAX_PATH;
		ofn.lpstrInitialDir = nullptr;
		ofn.lpstrTitle = nullptr;
		ofn.Flags = 0;
		ofn.nFileOffset = 0;
		ofn.nFileExtension = 0;
		ofn.lpstrDefExt = TEXT("txt");
		ofn.lCustData = 0L;
		ofn.lpfnHook = nullptr;
		ofn.lpTemplateName = nullptr;
		if (wParam == Pad02::MainFileSave)
		{
			ofn.Flags = OFN_OVERWRITEPROMPT;

			if (GetSaveFileNameW(&ofn))
			{
				if (!Pad02::File::Create(ofn.lpstrFile).Save(std::wstring(m_buf.begin(), m_buf.end())))
				{
					fwprintf(stdout, L"filename %s\n", ofn.lpstrFile);
					fprintf(stderr, "failed to save file\n");
					return;
				}
			}
		}
		else if (wParam == Pad02::MainFileLoad)
		{
			ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
			if (GetOpenFileNameW(&ofn))
			{
				OutputDebugString(L"Open");
				auto file = Pad02::File::Load(ofn.lpstrFile);
				auto content = file.ReadAll();

				m_buf.assign(content.begin(), content.end());
			}
		}
		if (wParam == Pad02::MainFileClose)
		{
			SendMessage(GetInstance(), WM_DESTROY, 0, 0);
		}
		
	}
};

// TODO: FIX TO NOT THROW BUT COLLECT AND RETURN 1
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	// According to Microsoft SDL
	HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);


	SetConsoleOutputCP(CP_UTF8);

	CoInitialize(nullptr);
	D2Window window(HINST_THISCOMPONENT);

	if (!D2Window::Register(HINST_THISCOMPONENT))
	{
		throw std::runtime_error("failed to register window");
	}


	g = std::make_shared<Pad02::Graphic>();
	if (!window.Create(L"PAD 02"))
	{
		throw std::runtime_error("failed to create window");
	}
	if (!g->AttachToWindow(window.GetInstance()))
	{
		throw std::runtime_error("failed to attach window");
	}

	text = std::make_unique<Pad02::Text>();
	try
	{
		text->CreateTextFormat(L"MonoLisa", 13.0f);
	}
	catch (...)
	{
		OutputDebugStringW(L"Creating font with Arial instead");
		text->CreateTextFormat(L"Arial", 13.0f);
	}
	Pad02::NotificationIcon nicon(window.GetInstance());

	window.Show();
	const auto mainFileBar = Pad02::FileMenu{};
	const auto mainTypographyBar = Pad02::TypographyMenu{};
	const auto mainMenuBar = Pad02::MenuManager{};
	mainMenuBar.AddMenu(L"&File", mainFileBar);
	mainMenuBar.AddMenu(L"&Typography", mainTypographyBar);

	mainMenuBar.SetMenu(window);


	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	CoUninitialize();
	return static_cast<INT>(msg.wParam);
}
