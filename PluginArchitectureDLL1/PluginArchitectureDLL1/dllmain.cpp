// dllmain.cpp : Defines the entry point for the DLL application.

#include "pch.h"
#include "Plugin.h"

//#define SET_PLUGIN_NAME(x) extern "C" __declspec(dllexport) \
//std::wstring WINAPI GetPluginName(){ return x;}
//#define SET_LISTVIEW_NAME(x) extern "C"{__declspec(dllexport) \
//std::wstring WINAPI GetListViewName(){ return x;}}
//#define SET_LISTVIEW_ACTIVE(x) extern "C"{__declspec(dllexport) \
//bool WINAPI GetListViewBool(){ return x;}}
//#define SET_IMMEDIATE_ACTIVATION(x) extern "C"{__declspec(dllexport) \
//bool WINAPI GetImmediateActivation(){ return x;}}



extern "C" __declspec(dllexport) const wchar_t* GetPluginName()
{ 
	
	return L"RemoteViewDll";
}

extern "C" __declspec(dllexport) const wchar_t* WINAPI GetListViewName()
{ 
	return L"Remote";

}

extern "C" __declspec(dllexport) bool WINAPI GetListViewBool()
{
	return TRUE;
}

extern "C" __declspec(dllexport) bool WINAPI GetImmediateActivation()
{ 
	return FALSE;
}

extern "C" __declspec(dllexport)  Plugin* WINAPI CreateModule()
{

	return new aPlugin();
}
//SET_PLUGIN_NAME(L"PluginTest")
//SET_LISTVIEW_NAME(L"ListViewPluginTest")
//SET_LISTVIEW_ACTIVE(TRUE);
//SET_IMMEDIATE_ACTIVATION(FALSE);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		break;
    case DLL_THREAD_ATTACH:
		break;
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

