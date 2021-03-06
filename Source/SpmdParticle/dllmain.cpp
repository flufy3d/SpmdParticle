// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "UnityPluginAPI/IUnityGraphics.h"
#include "UnityPluginAPI/IUnityGraphicsD3D11.h"

#include "GraphicsInterface.h"

static IUnityInterfaces* g_unity_interface;

static void UNITY_INTERFACE_API UnityOnGraphicsInterfaceEvent(UnityGfxDeviceEventType eventType)
{
	if (eventType == kUnityGfxDeviceEventInitialize) {
		auto unity_gfx = g_unity_interface->Get<IUnityGraphics>();
		auto api = unity_gfx->GetRenderer();


		if (api == kUnityGfxRendererD3D11) {
			Log("kUnityGfxRendererD3D11");
			CreateGraphicsInterface(DeviceType::D3D11, g_unity_interface->Get<IUnityGraphicsD3D11>()->GetDevice());
		}
		else
		{
			Log("only support kUnityGfxRendererD3D11! error occur");
		}

	}
	else if (eventType == kUnityGfxDeviceEventShutdown) {
		ReleaseGraphicsInterface();
	}

}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
UnityPluginLoad(IUnityInterfaces* unityInterfaces)
{
	g_unity_interface = unityInterfaces;
	g_unity_interface->Get<IUnityGraphics>()->RegisterDeviceEventCallback(UnityOnGraphicsInterfaceEvent);
	UnityOnGraphicsInterfaceEvent(kUnityGfxDeviceEventInitialize);
}
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
UnityPluginUnload()
{
	auto unity_gfx = g_unity_interface->Get<IUnityGraphics>();
	unity_gfx->UnregisterDeviceEventCallback(UnityOnGraphicsInterfaceEvent);
}




BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

