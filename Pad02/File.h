#pragma once
#include "precompiled.h"


// Idea for this File class is to handle input and output between widechar and UTF8 multibyte.

namespace Pad02
{
	class File
	{
		std::wstring Content;
		HANDLE HFile = nullptr;
		DWORD CbFileSizeHigh = 0;
		DWORD CbFileSizeLow = 0;

		std::string ToMultiByte(const std::wstring& wcData)
		{

			const auto toReserve = WideCharToMultiByte(
				CP_UTF8,
				0,
				wcData.c_str(),
				gsl::narrow_cast<int>(wcData.size()),
				nullptr,
				0,
				nullptr,
				nullptr);

			std::string mbData;
			mbData.resize(toReserve);
			WideCharToMultiByte(
				CP_UTF8,
				0,
				wcData.c_str(),
				gsl::narrow_cast<int>(wcData.size()),
				mbData.data(),
				gsl::narrow_cast<int>(mbData.size()),
				nullptr,
				nullptr);
			return mbData;
		}

		std::wstring ToWideChar(std::string& mbData)
		{
			const auto toReserve = MultiByteToWideChar(CP_UTF8, 0, mbData.c_str(), gsl::narrow_cast<int>(mbData.size()), nullptr, 0);
			std::wstring wcData;
			wcData.resize(toReserve);

			MultiByteToWideChar(CP_UTF8, 0, mbData.c_str(), gsl::narrow_cast<int>(mbData.size()), wcData.data(), gsl::narrow_cast<int>(wcData.size()));
			return wcData;
		}
		File(const wchar_t* szFileName, const char mode)
		{
			if (mode == 'w')
			{
				HFile = CreateFileW(
					szFileName,
					GENERIC_WRITE,
					0,
					nullptr,
					CREATE_NEW,
					FILE_ATTRIBUTE_NORMAL,
					nullptr);
			}
			else if (mode == 'r')
			{
				HFile = CreateFileW(
					szFileName,
					GENERIC_READ,
					0,
					nullptr,
					OPEN_ALWAYS,
					FILE_ATTRIBUTE_NORMAL,
					nullptr);
				if (HFile != nullptr)
				{
					CbFileSizeLow = GetFileSize(HFile, &CbFileSizeHigh);
				}
			}
		}

		
	public:
		~File()
		{
			if (HFile != nullptr)
			{
				CloseHandle(HFile);
			}
		}

		static File Create(const wchar_t *szFilename)
		{
			return {szFilename, 'w'};
		}
		static File Load(const wchar_t *szFilename)
		{
			return {szFilename, 'r'};
		}

		std::wstring ReadAll()
		{

			std::string data;
			data.resize(CbFileSizeLow);

			ReadFile(HFile, data.data(), CbFileSizeLow, nullptr, nullptr);
			std::wstring wcData = ToWideChar(data);
			return wcData;
			
		}
		bool Save(const std::wstring& data)
		{
			
			const auto mbData = ToMultiByte(data);

			if (mbData.empty())
			{
				return false;
			}

			DWORD wroteBytes;
			if (!WriteFile(
				HFile,
				mbData.data(),
				gsl::narrow_cast<DWORD>(mbData.size()),
				&wroteBytes,
				nullptr))
			{
				return false;
			}
			return true;
		}
	};
}
