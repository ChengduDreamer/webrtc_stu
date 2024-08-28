#include "common.h"
#include <memory>
#include <codecvt>
#include <locale>
#include <shlwapi.h>
#include <shellapi.h>

#pragma comment(lib, "Shlwapi.lib")

std::string ToUTF8(const std::wstring& src) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.to_bytes(src);
}

std::wstring ToWString(const std::string& src) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(src);
}

std::wstring GetModulePathW(HMODULE hModule) {
	const int maxPath = 4096;
	std::unique_ptr<wchar_t[]> sz_path = std::make_unique<wchar_t[]>(maxPath);
	std::unique_ptr<wchar_t[]> sz_full_path = std::make_unique<wchar_t[]>(maxPath);
	memset(sz_path.get(), 0, sizeof(wchar_t) * maxPath);
	memset(sz_full_path.get(), 0, sizeof(wchar_t) * maxPath);
	::GetModuleFileNameW(hModule, sz_path.get(), maxPath);
	::GetLongPathNameW(sz_path.get(), sz_full_path.get(), maxPath);
	::PathRemoveFileSpecW(sz_full_path.get());
	return std::wstring(sz_full_path.get());
}

