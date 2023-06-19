#pragma once
#include "precompiled.h"
#include "Window.h"
#include "WinStruct.h"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase);

namespace Pad02
{

	struct TextNode
	{
		int TokenType;
		int StartPosition;
		int Length;
		D2D1::ColorF Color;

	};

	class Caret
	{
		float mCaretX;
		float mCaretY;
	public:
		Caret() : mCaretX(0), mCaretY(0)
		{

		}

		~Caret() = default;
		float GetHeight() const 
		{
			return 14.0f;
		}
		void IncrementY()
		{
			mCaretY++;
		}
		void IncrementX()
		{
			mCaretX++;
		}
		void DecrementY()
		{
			--mCaretY;
		}
		void DecrementX()
		{
			--mCaretX;
		}
		void ResetY()
		{
			mCaretY = 0;
		}
		void ResetX()
		{
			mCaretX = 0;
		}
		void Render(std::unique_ptr< Graphic>& g)
		{
			g->DrawLine(D2D1::Point2F(mCaretX, mCaretY), D2D1::Point2F(mCaretX, mCaretY + GetHeight()));
		}
	};
	/*
	auto g = std::make_shared<Pad02::Graphic>();
	auto text = std::make_shared<Pad02::Text>();

	*/
	class D2Window : public Pad::Window
	{
		Caret mCaret;
		bool isTrailing = false;
		bool isInside = false;

		Mouse position{};
		std::unique_ptr<Pad02::Text> m_text = nullptr;
		std::unique_ptr<Pad02::Graphic> m_g = nullptr;

		std::vector<TCHAR> m_buf;

	public:
		explicit D2Window(HINSTANCE hInst) : Window(hInst), mCaret(Caret())
		{
		}

		bool OnCreate(HWND hwnd) override
		{
			m_g = std::make_unique<Pad02::Graphic>();
			m_text = std::make_unique<Pad02::Text>();

			try
			{
				m_text->CreateTextFormat(L"MonoLisa", 13.0f);
			}
			catch (...)
			{
				OutputDebugStringW(L"Creating font with Arial instead");
				m_text->CreateTextFormat(L"Arial", 13.0f);
			}
			m_g->AttachToWindow(hwnd);
			return true;
		}

		void OnChar(const TCHAR ch, int cRepeat) override
		{
			std::wstringstream debug;
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
				if (ch == L'\n')
				{
					mCaret.ResetX();
					mCaret.IncrementY();
				}
				else
				{
					mCaret.IncrementX();
				}
			}
		}

		void OnPaint(HWND hwnd) override
		{
			m_g->BeginDraw();
			const auto title = std::wstring(m_buf.begin(), m_buf.end());
			auto textlayout = m_text->CreateTextLayout(title,
				600,
				600);
			if (position.Down)
			{
				OutputDebugString(L"Mouse is down WM_PAINT");
			}
			mCaret.Render(m_g);
			m_g->DrawTextLayout(D2D1::Point2F(0.f, 0.f), textlayout);

			m_g->DrawLine(D2D1::Point2F(position.X1, position.Y1),
				D2D1::Point2F(position.X2, position.Y2));

			m_g->EndDraw();
		}

		void OnMouseMove(int x, int y) override
		{
			std::wstringstream coords;
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
			m_g->Resize(width, height);
		}

		void HitTest(const int& x, const int& y)
		{
			BOOL trailing = FALSE;
			BOOL inside = FALSE;
			DWRITE_HIT_TEST_METRICS metrics{};

			m_text->HitTestPoint(static_cast<float>(x), static_cast<float>(y));
			/*HRESULT hr = pTextLayout->HitTestPoint(
				static_cast<FLOAT>(x),
				static_cast<FLOAT>(y),
				&trailing,
				&inside,
				&metrics);
			ValidateResult(hr, "failed to get hit test");*/
		}

		LRESULT OnCustom(UINT msg, WPARAM wParam, LPARAM lParam) override
		{
			switch (msg)
			{
			case CM_NOTIFICATIONICON:
			{

				switch (LOWORD(lParam))
				{
				case NIN_SELECT:
				case NIN_KEYSELECT:
					return 0;
				case WM_CONTEXTMENU:
				{

					std::wstringstream text;
					text << L"Clicked on notification area" << std::endl;
					auto x = GET_X_LPARAM(lParam);
					auto y = GET_Y_LPARAM(lParam);
					text << L"x=" << x << std::endl;
					text << L"y=" << y << std::endl;
					OutputDebugString(text.str().c_str());

				}
				return 0;
				}


			}
			break;
			}
		}

		void OnCommand(WPARAM wParam, LPARAM lParam) override
		{

			std::wstring filename;
			filename.resize(MAX_PATH);
			OPENFILENAME ofn{};
			ofn.lStructSize = sizeof(OPENFILENAMEW);
			ofn.hInstance = HINST_THISCOMPONENT;
			ofn.hwndOwner = GetInstance();
			ofn.lpstrFile = filename.data();
			ofn.nMaxFile = MAX_PATH;			
			ofn.nMaxFileTitle = MAX_PATH;
			ofn.lpstrDefExt = TEXT("txt");
			ofn.lpstrFilter = L"Text files\0*.txt";
			HRESULT hr = S_OK;
			if (wParam == Pad02::MainFileSave)
			{
				ofn.Flags = OFN_OVERWRITEPROMPT;

				hr = GetSaveFileNameW(&ofn) ? S_OK : E_FAIL; 

				if (SUCCEEDED(hr) && (!Pad02::File::Create(ofn.lpstrFile).Save(std::wstring(m_buf.begin(), m_buf.end()))))
				{
					fwprintf(stdout, L"filename %s\n", ofn.lpstrFile);
					fprintf(stderr, "failed to save file\n");
					return;
				}
			}
			
			if (wParam == Pad02::MainFileLoad)
			{
				ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
				hr = GetOpenFileNameW(&ofn) ? S_OK : E_FAIL;

				if (SUCCEEDED(hr))
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
}