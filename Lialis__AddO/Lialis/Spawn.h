#pragma once


class Spawn
{
public:
	Spawn();
	~Spawn();


	DWORD CheckMemoryW(const wchar_t*, const wchar_t*, int);
	DWORD CheckMemory(const wchar_t*, const wchar_t*, int);
	DWORD ChangeMemory(const wchar_t*, const wchar_t*, int, int);

};

