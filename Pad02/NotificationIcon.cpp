#include "NotificationIcon.h"

void Pad02::NotificationIcon::_CreateWindow()
{
	WNDCLASS wc{};
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hInstance = GetModuleInstanceHandle();
	wc.lpszClassName = L"NOTIFICATION_ICON_PAD02";
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = DefWindowProcW;
	wc.hIcon = static_cast<HICON>(LoadIcon(nullptr, IDI_APPLICATION));
	RegisterClass(&wc);

	m_NotificationIcon = CreateWindow(
		wc.lpszClassName, 
		wc.lpszClassName, 
		WS_DISABLED, 
		CW_USEDEFAULT, CW_USEDEFAULT, 
		CW_USEDEFAULT, CW_USEDEFAULT, 
		HWND_MESSAGE, 
		nullptr, 
		wc.hInstance, 
		nullptr);

}

Pad02::NotificationIcon::NotificationIcon(const HWND owner, const std::wstring& applicationName) : m_Owner(owner), m_ApplicationName(applicationName)
{
	CreateNotificationIcon();
}

Pad02::NotificationIcon::~NotificationIcon()
{
	// Remove the icon from tray
	RemoveNotificationIcon();
	DestroyWindow(m_NotificationIcon);
}

void Pad02::NotificationIcon::CreateNotificationIcon()
{
	if(!m_NotificationIcon)
	{
		_CreateWindow();
	}
	
	NOTIFYICONDATA nid{};
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = m_Owner;
	nid.uID = 1;

	nid.uCallbackMessage = CM_NOTIFICATIONICON;
	nid.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	nid.uFlags = NIF_MESSAGE | NIF_SHOWTIP | NIF_TIP | NIF_ICON;
	StringCchCopy(nid.szTip,ARRAYSIZE(nid.szTip), m_ApplicationName.c_str());
	Shell_NotifyIcon(NIM_ADD, &nid);
	nid.uVersion = NOTIFYICON_VERSION_4;
	Shell_NotifyIconW(NIM_SETVERSION, &nid);
	m_notifyicondata = nid;


}

void Pad02::NotificationIcon::RemoveNotificationIcon()
{
	Shell_NotifyIcon(NIM_DELETE, &m_notifyicondata);
}

void Pad02::NotificationIcon::ReAddNotificationIcon()
{
	Shell_NotifyIcon(NIM_ADD, &m_notifyicondata);
	Shell_NotifyIcon(NIM_SETVERSION, &m_notifyicondata);
}

void Pad02::NotificationIcon::NotificationIconPressed()
{

}
