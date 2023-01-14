#include "Graphic.h"

#include <memory>

#include "Text.h"

Pad02::Graphic::Graphic()
{
	HRESULT hr = S_OK;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, pId2D1Factory.GetAddressOf());
	ValidateResult(hr, "failed to create d2d1 factory");

	
	
}

auto Pad02::Graphic::AttachToWindow(HWND hwnd) -> bool
{
	RECT rct;
	GetClientRect(hwnd, &rct);
	
	HRESULT hr = S_OK;
	hr = pId2D1Factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(
			hwnd,
			D2D1::SizeU(
				(rct.right - rct.left),
				(rct.bottom - rct.top))),
		pRenderTarget.GetAddressOf());
	ValidateResult(hr, "failed to create hwnd render target");

	hr = pRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(0.0f, 0.0f, 0.0f),
		pBrush.GetAddressOf());
	ValidateResult(hr, "failed to create brush");
	return true;
}

auto Pad02::Graphic::DrawLine(D2D1_POINT_2F xy1, D2D1_POINT_2F xy2, float strokeWidth) const -> void
{
	pRenderTarget->DrawLine(xy1, xy2, pBrush.Get(), strokeWidth);
}

auto Pad02::Graphic::DrawTextLayout(D2D1_POINT_2F origin, ComPtr<IDWriteTextLayout>& textLayout) const -> void
{

	pRenderTarget->DrawTextLayout(
		origin, 
		textLayout.Get(),
		pBrush.Get());
}
