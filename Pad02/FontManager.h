#pragma once


namespace Pad02
{

	class FontManager
	{
	public:
		FontManager();
		~FontManager() = default;
		auto EnumerateFonts() -> void;

	};
}