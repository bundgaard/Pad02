#include "precompiled.h"

#include "File.h"
#include "Window.h"

#include "Menu.h"
#include "Graphic.h"
#include "Text.h"
#include "NotificationIcon.h"
#include "D2Window.h"

#include <memory>
#include <functional>

#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2d1.lib")

using namespace Microsoft::WRL;
using namespace Pad02;


extern void ErrorExit();
struct ComInitializer
{
	ComInitializer()
	{
		OutputDebugStringW(L"Com init");
		(void)CoInitialize(nullptr);
	}
	~ComInitializer()
	{
		OutputDebugStringW(L"Com dtor\n");
		CoUninitialize();
	}
};
// TODO: FIX TO NOT THROW BUT COLLECT AND RETURN 1
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	// According to Microsoft SDL
	HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);
	SetConsoleOutputCP(CP_UTF8);
	
	ComInitializer comInit;
	
	
	D2Window window(hInst);
	HRESULT hr = S_OK;
	if (SUCCEEDED(hr))
	{
		hr = window.Create(hInst, L"PAD 02");
	}
	

	Pad02::NotificationIcon nicon(window.GetInstance(), window.GetTitle());

	window.Show();

	const Pad02::FileMenu mainFileBar{};
	const Pad02::TypographyMenu mainTypographyBar{};
	const Pad02::MenuManager mainMenuBar{};

	mainMenuBar.AddMenu(L"&File", mainFileBar);
	mainMenuBar.AddMenu(L"&Typography", mainTypographyBar);
	mainMenuBar.SetMenu(window);

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return static_cast<INT>(msg.wParam);
}
