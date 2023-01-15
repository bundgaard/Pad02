#include "Graphic.h"

#include <memory>

#include "Text.h"

Pad02::Graphic::Graphic()
{
	HRESULT hr = S_OK;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, pId2D1Factory.GetAddressOf());
	ValidateResult(hr, "failed to create d2d1 factory");
}

auto Pad02::Graphic::Resize(int width, int height) const -> void
{
	HRESULT hr = pRenderTarget->Resize(D2D1::SizeU(width, height));
	ValidateResult(hr, "failed to resize render target");
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


	hr = pRenderTarget->QueryInterface(__uuidof(ID2D1DeviceContext), &pDeviceContext);
	ValidateResult(hr, "failed to convert to device context");
	return true;
}

auto Pad02::Graphic::BeginDraw(D2D1::ColorF clearColor) const->void
{
	pDeviceContext->BeginDraw();
	pDeviceContext->SetTransform(D2D1::IdentityMatrix());
	pDeviceContext->Clear(clearColor);
}

auto Pad02::Graphic::EndDraw() const->void
{
	pDeviceContext->EndDraw();
}

auto Pad02::Graphic::DrawLine(D2D1_POINT_2F xy1, D2D1_POINT_2F xy2, float strokeWidth) const -> void
{
	pDeviceContext->DrawLine(xy1, xy2, pBrush.Get(), strokeWidth);
}

auto Pad02::Graphic::DrawTextLayout(D2D1_POINT_2F origin, ComPtr<IDWriteTextLayout>& textLayout) const -> void
{
	pDeviceContext->DrawTextLayout(
		origin,
		textLayout.Get(),
		pBrush.Get(),
		D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
}
