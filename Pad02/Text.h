#pragma once
#include <dwrite.h>

#include <wrl.h>
#include <xstring>

namespace Pad02
{
	using namespace Microsoft::WRL;

	class Text
	{
		ComPtr<IDWriteFactory> pWriteFactory = nullptr;
		ComPtr<IDWriteTextFormat> pTextFormat = nullptr;
		ComPtr<IDWriteTypography> pTypography = nullptr;

	public:
		Text();

		auto CreateTextFormat(const std::wstring fontName, float fontSize) -> void;
		[[nodiscard]] auto CreateTextLayout(const std::wstring& text, float maxWidth, float maxHeight) const -> ComPtr<IDWriteTextLayout>;
	};
}
