#pragma once
#include <stdexcept>

#include "Edit.h"

namespace Pad02
{
	enum Pad02Menu
	{
		MainFile,
		MainFileClose,
		MainFileSave,
		MainFileLoad,
		MainTypographyStyle02,
	};

	class FileMenu : public Pad::PopupMenu
	{
	public:
		FileMenu() : PopupMenu()
		{
			AppendString(L"&Open", MainFileLoad);
			AppendString(L"S&ave", MainFileSave);
			AppendString(L"&Close", MainFileClose);
		}
	};

	class TypographyMenu : public Pad::PopupMenu
	{
	public:
		TypographyMenu() : PopupMenu()
		{
			AppendString(L"SS 02", MainTypographyStyle02);
		}
	};

	class MenuManager
	{
		HMENU HMenu;

	public:
		MenuManager()
		{
			HMenu = CreateMenu();
		}

		auto AddMenu(const std::wstring& caption, const Pad::Menu& menu) const -> void
		{
			BOOL ok = AppendMenu(HMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(menu.GetInstance()), caption.c_str());
			if (!ok)
			{
				throw std::runtime_error("failed to append menu");
			}
		}

		auto SetMenu(const Pad::Window& window) const -> void
		{
			::SetMenu(window.GetInstance(), HMenu);
		}

		~MenuManager()
		{
			DestroyMenu(HMenu);
		}
	};
}
