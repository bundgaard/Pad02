#pragma once

#include <Windows.h>
#include <shellapi.h>

#define CM_NOTIFICATIONICON (WM_USER+1)

namespace Pad02
{
	/** Retrieve the HINSTANCE for the current DLL or EXE using this symbol that
	the linker provides for every module. This avoids the need for a global HINSTANCE variable
	and provides access to this value for static libraries. */
	EXTERN_C IMAGE_DOS_HEADER __ImageBase;
	inline HINSTANCE GetModuleInstanceHandle() noexcept { return reinterpret_cast<HINSTANCE>(&__ImageBase); }

	class NotificationIcon
	{
		HWND m_Owner;
		HWND m_NotificationIcon;
		NOTIFYICONDATA m_notifyicondata;

		void _CreateWindow();
	public:
		NotificationIcon() = delete;
		NotificationIcon(const HWND owner);
		~NotificationIcon();

		void CreateNotificationIcon();
		void RemoveNotificationIcon();
		void ReAddNotificationIcon();

		void NotificationIconPressed();
	};

}