#pragma once
#include <d2d1.h>
#include <d2d1_1.h>

#include <d2d1helper.h>
#include <dwrite.h>
#include <memory>
#include <stdexcept>
#include <wrl.h>

#include "Text.h"

namespace Pad02
{
	using namespace Microsoft::WRL;

	inline auto ValidateResult(HRESULT hr, std::string const& errorMessage)
	{
		if (FAILED(hr))
		{
			throw std::runtime_error(errorMessage);
		}
	}

	class Rectangle
	{
		long left, top, right, bottom;
		RECT Rct;

	public:
		Rectangle(RECT rct) : Rct(rct)
		{
			left = rct.left;
			top = rct.top;
			right = rct.right;
			bottom = rct.bottom;
		}

		[[nodiscard]] auto Rect() const -> RECT
		{
			return Rct;
		}

		template <typename Out>
		auto Width() const -> Out
		{
			return static_cast<Out>(right - left);
		}

		template <typename Out>
		auto Height() const -> Out
		{
			return static_cast<Out>(bottom - top);
		}
	};

	class Graphic
	{
		ComPtr<ID2D1Factory> pId2D1Factory = nullptr;
		ComPtr<ID2D1HwndRenderTarget> pRenderTarget = nullptr;
		ComPtr<ID2D1SolidColorBrush> pBrush = nullptr;
		ComPtr<ID2D1DeviceContext> pDeviceContext = nullptr;

	public:
		Graphic();

		auto Resize(int width, int height) const -> void;
		auto AttachToWindow(HWND hwnd) -> bool;

		auto BeginDraw(D2D1::ColorF clearColor = D2D1::ColorF::White) const -> void;

		auto EndDraw() const -> void;

		auto DrawLine(D2D1_POINT_2F xy1, D2D1_POINT_2F xy2, float strokeWidth = 1) const -> void;
		auto DrawTextLayout(D2D1_POINT_2F origin, ComPtr<IDWriteTextLayout>& textLayout) const -> void;
	};
}
