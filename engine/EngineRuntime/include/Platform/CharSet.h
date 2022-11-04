#pragma once

namespace Engine
{
	size_t AnsiToUnicodeLengthBuffer(const char* chs);

	size_t AnsiToUnicode(const char* chs, wchar_t* wchs);

	size_t UnicodeToAnsiLengthBuffer(const wchar_t* wchs);

	size_t UnicodeToAnsi(const wchar_t* wchs, char* chs);
}
