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
struct Com
{
	Com()
	{
		OutputDebugStringW(L"Com init");
		(void)CoInitialize(nullptr);
	}
	~Com()
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
	
	Com com;
	
	auto g = std::make_shared<Pad02::Graphic>();
	auto text = std::make_shared<Pad02::Text>();

	D2Window window(hInst, g, text);

	if (!D2Window::Register(hInst))
	{
		MessageBox(HWND_DESKTOP, L"Failed to register window", L"Error", MB_OK|MB_ICONERROR);
		return 1;
	}

	if (!window.Create(L"PAD 02"))
	{
		MessageBox(HWND_DESKTOP, L"Failed to create window", L"Error", MB_OK|MB_ICONERROR);
		return 1;
	}
	if (!g->AttachToWindow(window.GetInstance()))
	{
		MessageBox(HWND_DESKTOP, L"failed to attach window", L"Error", MB_OK|MB_ICONERROR);
		return 1;
	}
	
	try
	{
		text->CreateTextFormat(L"MonoLisa", 13.0f);
	}
	catch (...)
	{
		OutputDebugStringW(L"Creating font with Arial instead");
		text->CreateTextFormat(L"Arial", 13.0f);
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
