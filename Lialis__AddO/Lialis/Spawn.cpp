#include "stdafx.h"
#include "Spawn.h"

Spawn::Spawn()
{

}

Spawn::~Spawn()
{
}

DWORD Spawn::CheckMemoryW(const wchar_t* fileName, const wchar_t* string, int stringSize) {
	if (stringSize) {
		wchar_t wCdir[MAX_PATH + 1];
		GetCurrentDirectory(MAX_PATH, wCdir);
		std::wstringstream wss;
		std::wstring wstr;
		wss << wCdir << L"\\" << fileName;
		wstr = wss.str();

		HANDLE hFile = CreateFile(wstr.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, 3, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return FALSE;
		DWORD fileSize = GetFileSize(hFile, 0);
		char* buffer = (char*)malloc(fileSize);
		DWORD sizeRead;
		if (ReadFile(hFile, buffer, fileSize, &sizeRead, NULL) > 0) {
			for (int i = 0; i <= sizeRead; i++) {
				for (int j = 0; j <= stringSize; j++) {
					if (buffer[i] == string[j]) {
						if (i < sizeRead) {
							if (buffer[i + 1] == '\0') {
								i++;

								if (j == stringSize) {
									CloseHandle(hFile);
									return i;
								}
							}
							else
							{
								break;
							}
							
						}
						
					}
					else {
						//j = NULL;
						break;
					}
				}
			}
		}

		CloseHandle(hFile);
	}

	MessageBox(NULL, L"Failed to Find Memory Location", NULL, NULL);
	return GetLastError();
}

DWORD Spawn::CheckMemory(const wchar_t* fileName, const wchar_t* string, int stringSize) {
	if (stringSize) {
		wchar_t wCdir[MAX_PATH + 1];
		GetCurrentDirectory(MAX_PATH, wCdir);
		std::wstringstream wss;
		std::wstring wstr;
		wss << wCdir << L"\\" << fileName;
		wstr = wss.str();

		HANDLE hFile = CreateFile(wstr.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, 3, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return FALSE;
		DWORD fileSize = GetFileSize(hFile, 0);
		wchar_t* buffer = (wchar_t*)malloc(fileSize);
		DWORD sizeRead;
		if (ReadFile(hFile, buffer, fileSize, &sizeRead, NULL) > 0) {
			fileSize = sizeRead / sizeof(wchar_t);
			for (int i = 0; i <= fileSize; i++) {
				for (int j = 0; j <= stringSize; j++) {
					if (buffer[i] == string[j]) {
						if (i < fileSize) {
							i++;
							if (j == stringSize) {
								CloseHandle(hFile);
								return i;
							}
						}
					}
					else {
						//j = NULL;
						break;
					}
				}
			}
		}

		CloseHandle(hFile);
	}
	
	MessageBox(NULL, L"Failed to Find Memory Location", NULL, NULL);
	return GetLastError();
}

DWORD Spawn::ChangeMemory(const wchar_t* fileName, const wchar_t* string, int offset, int stringSize)
{
	if (stringSize) {
		wchar_t wCdir[MAX_PATH + 1];
		GetCurrentDirectory(MAX_PATH, wCdir);
		std::wstringstream wss;
		std::wstring wstr;
		wss << wCdir << L"\\" << fileName;
		wstr = wss.str();

		HANDLE hFile = CreateFile(wstr.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, 3, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return FALSE;
		DWORD fileSize = GetFileSize(hFile, 0);
		if (fileSize > offset + stringSize) {
			DWORD dwPos = SetFilePointer(hFile, offset, NULL, FILE_BEGIN);
			if (LockFile(hFile, dwPos, NULL, NULL, NULL) != 0) {
				DWORD sizeRead;
				if (WriteFile(hFile, string, stringSize, &sizeRead, NULL) > 0) {
					CloseHandle(hFile);
					return 0;
				}
			}
		}
		CloseHandle(hFile);
	}
	
	MessageBox(NULL, L"Failed to Change Memory Location", NULL, NULL);
	return GetLastError();
}
