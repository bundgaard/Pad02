#pragma once
#include "Window.h"


namespace Pad
{
	class Edit
	{
		HWND m_hwnd;
		HINSTANCE m_hInstance;
		static LRESULT CALLBACK Proc(HWND, UINT, WPARAM, LPARAM)
		{
			
		}
	public:
		Edit() : m_hInstance(nullptr), m_hwnd(nullptr)
		{
			
		}
		~Edit()
		{
			
		}

		
	};
}
