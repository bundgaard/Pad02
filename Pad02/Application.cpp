#define WIN32_LEAN_AND_MEAN
#include <wrl.h>
#include <Windows.h>
#include <Windowsx.h>
#include <ShlObj.h>

#include <commdlg.h>

#include <dwrite.h>


#include <sstream>
#include <vector>

#include "File.h"
#include "Window.h"



#include "Graphic.h"
#include "Text.h"


#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2d1.lib")

using namespace Microsoft::WRL;

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)


enum Pad02Menu
{
	MainFile,
	MainFileClose,
	MainFileSave,
	MainFileLoad,
	MainTypographyStyle02,
};

extern void ErrorExit();

std::unique_ptr<Pad02::Graphic> g = nullptr;
std::unique_ptr<Pad02::Text> text = nullptr;

class D2Window : public Pad::Window
{
	bool mouseDown = false;

	bool isTrailing = false;
	bool isInside = false;
	std::vector<TCHAR> m_buf;

public:
	D2Window(HINSTANCE hInst) : Pad::Window(hInst)
	{
	}

	bool OnCreate(HWND hwnd) override
	{
		HRESULT hr = S_OK;
		RECT clientRect;
		GetClientRect(hwnd, &clientRect);


		
		return true;
	}

	void OnChar(const TCHAR ch, int cRepeat) override
	{
		std::wostringstream debug;
		debug << ch << std::endl;

		OutputDebugString(debug.str().c_str());
		m_buf.push_back(ch);
	}

	void OnPaint(HWND hwnd) override
	{
		g->BeginDraw();
		const auto title = std::wstring(m_buf.begin(), m_buf.end());
		auto textlayout = text->CreateTextLayout(title.c_str(), 600, 600);
		g->DrawTextLayout(D2D1::Point2F(0.f, 0.f), textlayout);
		g->EndDraw();
	
	}

	void OnMouseMove(int x, int y) override
	{
		std::wostringstream coords;
		coords << L"X=" << x << L",Y=" << y;
		SetTitle(coords.str());
	}

	void OnMouseDown(int x, int y) override
	{
		mouseDown = true;

		HitTest(x, y);
	}

	void OnMouseUp(int x, int y) override
	{
		mouseDown = false;
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


		/*HRESULT hr = pTextLayout->HitTestPoint(
			static_cast<FLOAT>(x),
			static_cast<FLOAT>(y),
			&trailing,
			&inside,
			&metrics);
		ValidateResult(hr, "failed to get hit test");*/

		fprintf(stderr, "metrics\ninside %s, trailing %s\n", inside ? "true" : "false", trailing ? "true" : "false");
	}

	void OnDestroy() override
	{
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
		if (wParam == MainFileSave)
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
		else if (wParam == MainFileLoad)
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
		if (wParam == MainFileClose)
		{
			SendMessage(GetInstance(), WM_DESTROY, 0, 0);
		}
	}
};

bool CreateFileMenu(HMENU hParentMenu)
{
	if (!AppendMenu(hParentMenu, MF_STRING, MainFileLoad, L"&Open"))
	{
		fprintf(stderr, "failed to append menu item\n");
		return false;
	}

	if (!AppendMenu(hParentMenu, MF_STRING, MainFileSave, L"S&ave"))
	{
		fprintf(stderr, "failed to append menu item\n");
		return false;
	}

	if (!AppendMenu(hParentMenu, MF_STRING, MainFileClose, L"&Close"))
	{
		fprintf(stderr, "failed to append menu item\n");
		return false;
	}
	return true;
}

bool CreateTypographyMenu(HMENU hParentMenu)
{
	if (!AppendMenu(hParentMenu, MF_STRING, MainTypographyStyle02, L"SS 02"))
	{
		fprintf(stderr, "failed to append menu item\n");
		return false;
	}

	return true;
}

int main(int argc, char** argv)
{
	SetConsoleOutputCP(CP_UTF8);

	CoInitialize(nullptr);
	D2Window window(HINST_THISCOMPONENT);

	if (!D2Window::Register(HINST_THISCOMPONENT))
	{
		fprintf(stderr, "failed to register window\n");
		return 1;
	}


	g = std::make_unique<Pad02::Graphic>();
	if (!window.Create(L"PAD 02"))
	{
		fprintf(stderr, "failed to create window");
		const auto dwError = GetLastError();

		return 1;
	}
	if (!g->AttachToWindow(window.GetInstance()))
	{
		fprintf(stderr, "failed to attach to window");
		return false;
	}
	text = std::make_unique<Pad02::Text>();

	window.Show();
	
	const auto mainMenuBar = Pad::Menu();
	const auto mainFileBar = Pad::PopupMenu();
	const auto mainTypographyBar = CreatePopupMenu();


	if (!CreateFileMenu(mainFileBar.GetInstance()))
	{
		fprintf(stderr, "failed to create File menu\n");
		return 1;
	}
	if (!AppendMenu(mainMenuBar.GetInstance(), MF_POPUP, reinterpret_cast<UINT_PTR>(mainFileBar.GetInstance()), L"&File"))
	{
		fprintf(stderr, "failed to append menu item");
		return 1;
	}

	if (!CreateTypographyMenu(mainTypographyBar))
	{
		return 1;
	}


	if (!AppendMenuW(mainMenuBar.GetInstance(), MF_POPUP, reinterpret_cast<UINT_PTR>(mainTypographyBar), L"&Typygraphy"))
	{
		fprintf(stderr, "failed to append menu item");
		return false;
	}

	if (!SetMenu(window.GetInstance(), mainMenuBar.GetInstance()))
	{
		fprintf(stderr, "failed to create menu");
		return false;
	}


	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	DestroyMenu(mainTypographyBar);

	CoUninitialize();
	return static_cast<INT>(msg.wParam);
}
