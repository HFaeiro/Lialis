#pragma once
#include "pch.h"
struct ClientData {

	SOCKET s;
	struct ClientDataStrings {
		std::wstring name, IP, OS, Country;
	}CDS;
};