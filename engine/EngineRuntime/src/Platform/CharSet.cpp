#include "EngineRuntime/include/Platform/CharSet.h"
#ifdef _WIN64
#include <Windows.h>
#else
#include <cstring>
#include <cwchar>
#include <cstdlib>
#endif

namespace Engine
{
	size_t AnsiToUnicodeLengthBuffer(const char* chs)
	{
		size_t size;
#ifdef _WIN64
		size = MultiByteToWideChar(CP_ACP, 0, chs, -1, nullptr, 0) * 2;
#else
		size = (strlen(chs) + 1) * 2;
#endif
		return size;
	}

	size_t AnsiToUnicode(const char* chs, wchar_t* wchs)
	{
		int size;
#ifdef _WIN64
		size = MultiByteToWideChar(CP_ACP, 0, chs, -1, nullptr, 0);
		MultiByteToWideChar(CP_ACP, 0, chs, -1, wchs, size);
#else
		size = strlen(chs) + 1;
		std::mbstowcs(wchs, chs, strlen(chs) + 1);
#endif
		return size;
	}

	size_t UnicodeToAnsiLengthBuffer(const wchar_t* wchs)
	{
		size_t size;
#ifdef _WIN64
		size = WideCharToMultiByte(CP_ACP, 0, wchs, -1, nullptr, 0, NULL, NULL);
#else
		size = wcslen(wchs) + 1;
#endif
		return size;
	}

	size_t UnicodeToAnsi(const wchar_t* wchs, char* chs)
	{
		int size;
#ifdef _WIN64
		size = WideCharToMultiByte(CP_ACP, 0, wchs, -1, nullptr, 0, NULL, NULL);
		WideCharToMultiByte(CP_ACP, 0, wchs, -1, chs, size, NULL, NULL);
#else
		return std::wcstombs(chs, wchs, wcslen(wchs) * 2 + 2);
#endif
		return size;
	}

}
