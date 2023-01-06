#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <Windows.h>
#include <Windowsx.h>
#include <ShlObj.h>

#include <commdlg.h>

#include <dwrite.h>
#include <dwrite_1.h>
#include <dwrite_2.h>
#include <dwrite_3.h>
#pragma comment(lib, "dwrite.lib")
#include <d2d1.h>
#pragma comment(lib, "d2d1.lib")

#include <array>
#include <sstream>
#include <vector>

#include "File.h"
#include "Window.h"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)


static IDWriteFactory* pWriteFactory = nullptr;
static IDWriteTextLayout* pTextLayout = nullptr;
static IDWriteTextFormat* pTextFormat = nullptr;
static IDWriteTypography* pTypography = nullptr;

static ID2D1Factory* pId2D1Factory = nullptr;
static ID2D1HwndRenderTarget* pRenderTarget = nullptr;
static ID2D1SolidColorBrush* pBrush = nullptr;

enum Pad02Menu
{
	MainFile,
	MainFileClose,
	MainFileSave,
	MainFileLoad,
	MainTypographyStyle02,
};

extern void ErrorExit();


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

		hr = pId2D1Factory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(
				hwnd,
				D2D1::SizeU(
					clientRect.right - clientRect.left,
					clientRect.bottom - clientRect.top)),
			&pRenderTarget);
		if (FAILED(hr))
		{
			fprintf(stderr, "failed to create hwnd render target\n");
			return false;
		}


		hr = pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(0.0f, 0.0f, 0.0f),
			&pBrush);
		if (FAILED(hr))
		{
			fprintf(stderr, "failed to create brush\n");
			return false;
		}


		hr = pWriteFactory->CreateTextFormat(L"MonoLisa",
		                                     nullptr,
		                                     DWRITE_FONT_WEIGHT_NORMAL,
		                                     DWRITE_FONT_STYLE_NORMAL,
		                                     DWRITE_FONT_STRETCH_NORMAL,
		                                     13.f,
		                                     L"",
		                                     &pTextFormat);

		if (FAILED(hr))
		{
			fprintf(stderr, "failed to create text format\n");
			return false;
		}

		hr = pWriteFactory->CreateTypography(&pTypography);
		if (FAILED(hr))
		{
			fprintf(stderr, "failed to create typography\n");
			return false;
		}

		hr = pTypography->AddFontFeature(DWRITE_FONT_FEATURE{DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_2, 2});
		if (FAILED(hr))
		{
			fprintf(stderr, "failed to add font feature SS02\n");
			return false;
		}

		hr = pTypography->AddFontFeature(DWRITE_FONT_FEATURE{DWRITE_FONT_FEATURE_TAG_SLASHED_ZERO, 1});
		if (FAILED(hr))
		{
			fprintf(stderr, "failed to add font feature SLASHED_ZERO\n");
			return false;
		}

		hr = pTypography->AddFontFeature(DWRITE_FONT_FEATURE{DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_7, 1});
		if (FAILED(hr))
		{
			fprintf(stderr, "failed to add font feature ROUND PARENTHESIS");
			return false;
		}
		hr = pTypography->AddFontFeature(DWRITE_FONT_FEATURE{DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_8, 1});
		if (FAILED(hr))
		{
			fprintf(stderr, "failed to add font feature ROUND PARENTHESIS");
			return false;
		}

		IDWriteTextAnalyzer *pTextAnalyzer = nullptr;
		IDWriteTextAnalyzer2 *pTextAnalyzer2 = nullptr;
		hr = pWriteFactory->CreateTextAnalyzer(&pTextAnalyzer);
		if (FAILED(hr))
		{
			fprintf(stderr, "failed to create text analyzer");
			return false;
		}

		IDWriteFontFace *pFontFace = nullptr;
		IDWriteFontFile *pFontFile = nullptr;
		hr = pWriteFactory->CreateFontFileReference(L"C:\\Code\\MonoLisa\\ttf\\MonoLisaNormal.ttf", nullptr, &pFontFile);
		if (FAILED(hr))
		{
			fprintf(stderr, "failed to create reference to font");
			return false;
		}
		std::array< IDWriteFontFile*, 1> fontFiles{{pFontFile}};
		hr = pWriteFactory->CreateFontFace(
			DWRITE_FONT_FACE_TYPE_TRUETYPE, 
			fontFiles.size(),fontFiles.data()
			, 0, DWRITE_FONT_SIMULATIONS_NONE, &pFontFace);
		if(FAILED(hr))
		{
			fprintf(stderr, "failed to create font face");
			return false;
		}

		if(pFontFile) pFontFile->Release(); // keep its own reference
		
		hr = pTextAnalyzer->QueryInterface(_uuidof(IDWriteTextAnalyzer2),reinterpret_cast<void**>(&pTextAnalyzer2));
		if (FAILED(hr))
		{
			fprintf(stderr, "text analyzer did not support newer version");
			return false;
		}
		// pTextAnalyzer2->GetTypographicFeatures()
		// Run this before
		// https://learn.microsoft.com/en-us/windows/win32/api/dwrite/nf-dwrite-idwritetextanalyzer-analyzescript
		 // pTextAnalyzer2->CheckTypographicFeature(pFontFace, DWRITE_SCRIPT_ANALYSIS{})
		pFontFace->Release();
		if(pTextAnalyzer2) pTextAnalyzer2->Release();
		pTextAnalyzer->Release();

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
		pRenderTarget->BeginDraw();
		pRenderTarget->SetTransform(D2D1::IdentityMatrix());
		pRenderTarget->Clear(D2D1::ColorF(1.f, 1.0f, 1.0f, 1.0f));

		const auto title = std::wstring(m_buf.begin(), m_buf.end());

		if (pTextLayout != nullptr)
			pTextLayout->Release();
		RECT clientRect;
		GetClientRect(hwnd, &clientRect);

		HRESULT hr = pWriteFactory->CreateTextLayout(
			title.c_str(),
			static_cast<UINT32>(title.size()),
			pTextFormat,
			static_cast<FLOAT>(clientRect.right - clientRect.left),
			static_cast<FLOAT>(clientRect.bottom - clientRect.top),
			&pTextLayout);
		if (FAILED(hr))
		{
			fprintf(stderr, "failed to create text layout\n");
			return;
		}

		hr = pTextLayout->SetTypography(pTypography, DWRITE_TEXT_RANGE{0, static_cast<UINT32>(title.size())});
		if (FAILED(hr))
		{
			fprintf(stderr, "failed to set typography");
			return;
		}

		auto oldColor = pBrush->GetColor();
		pBrush->SetColor(D2D1::ColorF(0.0f, 0.0f, 0.0f));
		pRenderTarget->DrawTextLayout(
			D2D1::Point2F(0.f, 0.f),
			pTextLayout,
			pBrush);
		pBrush->SetColor(oldColor);

		oldColor = pBrush->GetColor();
		DWRITE_TEXT_METRICS textMetrics{};
		pTextLayout->GetMetrics(&textMetrics);

		pRenderTarget->FillRectangle(D2D1::RectF(textMetrics.left, 0.0f, 2.0f, textMetrics.height), pBrush);
		pBrush->SetColor(oldColor);

		pRenderTarget->EndDraw();
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
		pRenderTarget->Resize(D2D1::SizeU(width, height));
	}

	void HitTest(const int& x, const int& y)
	{
		BOOL trailing;
		BOOL inside;
		DWRITE_HIT_TEST_METRICS metrics{};


		HRESULT hr = pTextLayout->HitTestPoint(
			static_cast<FLOAT>(x),
			static_cast<FLOAT>(y),
			&trailing,
			&inside,
			&metrics);
		if (FAILED(hr))
		{
			const auto dwError = GetLastError();
			fprintf(stderr, "failed to get hit test with %ld\n", dwError);
		}

		fprintf(stderr, "metrics\ninside %s, trailing %s\n", inside ? "true" : "false", trailing ? "true" : "false");
	}

	void OnDestroy() override
	{
		if (pTextLayout != nullptr)
		{
			pTextLayout->Release();
		}
	}

	void OnCommand(WPARAM wParam, LPARAM lParam) override
	{
		std::wstring filename;
		filename.resize(MAX_PATH);
		OPENFILENAME ofn{};
		ofn.lStructSize = sizeof(OPENFILENAMEW);
		ofn.hInstance = HINST_THISCOMPONENT;
		ofn.hwndOwner = GetWindow();
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
		} else if (wParam == MainFileLoad)
		{
			ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
			if(GetOpenFileNameW(&ofn))
			{
				OutputDebugString(L"Open");
				auto file = Pad02::File::Load(ofn.lpstrFile);
				auto content = file.ReadAll();
				
				m_buf.assign(content.begin(), content.end());
			}
		}
		if (wParam == MainFileClose)
		{
			SendMessage(GetWindow(), WM_DESTROY, 0, 0);
		}
	}
};

bool CreateFileMenu(HMENU hParentMenu)
{

	if(!AppendMenu(hParentMenu, MF_STRING, MainFileLoad, L"&Open"))
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

	HRESULT hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		_uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&pWriteFactory));
	if (FAILED(hr))
	{
		fprintf(stderr, "failed to create write factory\n");
		return 1;
	}

	hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		_uuidof(ID2D1Factory),
		reinterpret_cast<void**>(&pId2D1Factory));
	if (FAILED(hr))
	{
		fprintf(stderr, "failed to create d2d1 factory\n");
		return 1;
	}

	if (!window.Create(L"PAD 02"))
	{
		fprintf(stderr, "failed to create window");
		const auto dwError = GetLastError();

		return 1;
	}
	const auto mainMenuBar = CreateMenu();
	const auto mainFileBar = CreatePopupMenu();
	const auto mainTypographyBar = CreatePopupMenu();


	if (!CreateFileMenu(mainFileBar))
	{
		fprintf(stderr, "failed to create File menu\n");
		return 1;
	}
	if (!AppendMenu(mainMenuBar, MF_POPUP, reinterpret_cast<UINT_PTR>(mainFileBar), L"&File"))
	{
		fprintf(stderr, "failed to append menu item");
		return 1;
	}

	if (!CreateTypographyMenu(mainTypographyBar))
	{
		return 1;
	}


	if (!AppendMenuW(mainMenuBar, MF_POPUP, reinterpret_cast<UINT_PTR>(mainTypographyBar), L"&Typygraphy"))
	{
		fprintf(stderr, "failed to append menu item");
		return false;
	}

	if (!SetMenu(window.GetWindow(), mainMenuBar))
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
	DestroyMenu(mainFileBar);
	DestroyMenu(mainMenuBar);
	DestroyMenu(mainTypographyBar);

	pTypography->Release();
	pTextFormat->Release();
	pWriteFactory->Release();
	pRenderTarget->Release();
	pId2D1Factory->Release();
	CoUninitialize();
	return static_cast<INT>(msg.wParam);
}
