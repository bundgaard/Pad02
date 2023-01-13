#include "Graphic.h"

Pad02::Graphic::Graphic()
{
	HRESULT hr = S_OK;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, pId2D1Factory.GetAddressOf());
	ValidateResult(hr, "failed to create d2d1 factory");

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_ISOLATED, _uuidof(IDWriteFactory),
	                         reinterpret_cast<IUnknown**>(pWriteFactory.GetAddressOf()));
	ValidateResult(hr, "failed to create direct write factory");
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
