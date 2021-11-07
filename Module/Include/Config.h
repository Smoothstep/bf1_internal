#pragma once

#include <fstream>
#include <sstream>
#include <filesystem>
#include <string_view>
#include <string>
#include <variant>

#include "FnTraits.h"

namespace fs
{
	using namespace std::filesystem;
}

class ConfigLoader
{
	fs::path Path;

public:
	ConfigLoader(const fs::path& Path)
		: Path(Path)
	{}

	void Save(const std::string_view Content)
	{
		std::ofstream Out(Path);
		std::stringstream Strm;

		Out << Strm.rdbuf();
	}

	std::string Load() const
	{
		std::ifstream In(Path);
		std::stringstream Strm;

		In >> Strm.rdbuf();

		return Strm.str();
	}
};

struct TStringEqual : public std::equal_to<std::wstring>
{
	using is_transparent = void;

	bool operator()(const std::wstring& Str, const std::wstring_view View) const
	{
		return Str == View;
	}

	using std::equal_to<std::wstring>::operator();
};

struct TStringHash : public std::hash<std::wstring>
{
	size_t operator()(const std::wstring_view View) const
	{
		return std::hash<std::wstring_view>()(View);
	}
};

struct TStringLess : public std::less<>
{
	using is_transparent = void;
	using std::less<>::operator();
};