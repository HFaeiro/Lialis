#pragma once
#include "GlobalStructs.h"
#include <string>




class __declspec(dllimport)Plugin
{
public:
	Plugin() {};
	virtual ~Plugin() {};
	virtual void Initialize() = 0;
	virtual void Initialize(SOCKET s) = 0;

	std::wstring PluginName;
	std::wstring ListViewName;
	bool usingListView;
	bool activateOnLoad;
	HMODULE PluginHandle;

};


extern std::vector<Plugin*> vPlugins;