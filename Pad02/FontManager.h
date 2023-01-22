#pragma once
#include "precompiled.h"

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