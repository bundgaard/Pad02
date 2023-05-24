#include "precompiled.h"
#include "Text.h"


#include "Graphic.h"

Pad02::Text::Text()
{
	HRESULT hr = S_OK;
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_ISOLATED,
		_uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(pWriteFactory.GetAddressOf()));
	ValidateResult(hr, "failed to create direct write factory");
	hr = pWriteFactory->CreateTypography(&pTypography);
	ValidateResult(hr, "failed to create typography");
	hr = pTypography->AddFontFeature(DWRITE_FONT_FEATURE{ DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_2, 2 });
	ValidateResult(hr, "failed to add font feature SS02");
	hr = pTypography->AddFontFeature(DWRITE_FONT_FEATURE{ DWRITE_FONT_FEATURE_TAG_SLASHED_ZERO, 1 });
	ValidateResult(hr, "failed to add font feature SLASHED_ZERO");
	hr = pTypography->AddFontFeature(DWRITE_FONT_FEATURE{ DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_7, 1 });
	ValidateResult(hr, "failed to add font feature ROUND PARENTHESIS");
	hr = pTypography->AddFontFeature(DWRITE_FONT_FEATURE{ DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_8, 1 });
	ValidateResult(hr, "failed to add font feature ROUND PARENTHESIS");


	ComPtr<IDWriteTextAnalyzer> pTextAnalyzer = nullptr;
	ComPtr<IDWriteTextAnalyzer2> pTextAnalyzer2 = nullptr;
	hr = pWriteFactory->CreateTextAnalyzer(pTextAnalyzer.GetAddressOf());
	ValidateResult(hr, "failed to create text analyzer");

	ComPtr<IDWriteFontFace> pFontFace = nullptr;
	ComPtr<IDWriteFontFile> pFontFile = nullptr;
	hr = pWriteFactory->CreateFontFileReference(L"C:\\Code\\MonoLisaNormal.ttf", nullptr, &pFontFile);
	ValidateResult(hr, "failed to create reference to font");

	std::array<IDWriteFontFile*, 1> fontFiles{ {pFontFile.Get()} };
	hr = pWriteFactory->CreateFontFace(
		DWRITE_FONT_FACE_TYPE_TRUETYPE,
		gsl::narrow_cast<UINT32>(fontFiles.size()), fontFiles.data()
		, 0, DWRITE_FONT_SIMULATIONS_NONE, &pFontFace);
	ValidateResult(hr, "failed to create font face");

	hr = pTextAnalyzer->QueryInterface(_uuidof(IDWriteTextAnalyzer2), &pTextAnalyzer2);
	ValidateResult(hr, "text analyzer did not support newer version");

	// pTextAnalyzer2->GetTypographicFeatures()
	// Run this before
	// https://learn.microsoft.com/en-us/windows/win32/api/dwrite/nf-dwrite-idwritetextanalyzer-analyzescript
	// pTextAnalyzer2->CheckTypographicFeature(pFontFace, DWRITE_SCRIPT_ANALYSIS{})
}

auto Pad02::Text::CreateTextFormat(const std::wstring fontName, float fontSize) -> void
{
	HRESULT hr = S_OK;
	hr = pWriteFactory->CreateTextFormat(fontName.c_str(),
		nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		fontSize,
		L"",
		pTextFormat.GetAddressOf());
	ValidateResult(hr, "failed to create text format");
}

auto Pad02::Text::CreateTextLayout(const std::wstring& text, float maxWidth,
	float maxHeight) const -> ComPtr<IDWriteTextLayout>
{
	ComPtr<IDWriteTextLayout> pTextLayout = nullptr;
	pWriteFactory->CreateTextLayout(
		text.c_str(), gsl::narrow_cast<UINT32>(text.size()),
		pTextFormat.Get(),
		maxWidth, maxHeight,
		pTextLayout.GetAddressOf());

	return pTextLayout;
}


auto Pad02::Text::HitTestPoint(float x, float y) -> DWRITE_HIT_TEST_METRICS
{
	return DWRITE_HIT_TEST_METRICS{};
}

