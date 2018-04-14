#include "stdafx.h"

#include "GraphicsInterface.h"

GraphicsInterface::~GraphicsInterface()
{
}

int GraphicsInterface::GetTexelSize(TextureFormat format)
{
    switch (format)
    {
    case TextureFormat::RGBAu8:  return 4;
    case TextureFormat::RGu8:    return 2;
    case TextureFormat::Ru8:     return 1;

    case TextureFormat::RGBAf16:
    case TextureFormat::RGBAi16: return 8;
    case TextureFormat::RGf16:
    case TextureFormat::RGi16:   return 4;
    case TextureFormat::Rf16:
    case TextureFormat::Ri16:    return 2;

    case TextureFormat::RGBAf32:
    case TextureFormat::RGBAi32: return 16;
    case TextureFormat::RGf32:
    case TextureFormat::RGi32:   return 8;
    case TextureFormat::Rf32:
    case TextureFormat::Ri32:    return 4;
    }
    return 0;
}


static GraphicsInterface *g_gfx_device;

GraphicsInterface* CreateGraphicsInterface(DeviceType type, void *device_ptr)
{
    switch (type) {

    case DeviceType::D3D11:
        g_gfx_device = CreateGraphicsInterfaceD3D11(device_ptr);
        break;
    }
    return g_gfx_device;
}

GraphicsInterface* GetGraphicsInterface()
{
    return g_gfx_device;
}

void ReleaseGraphicsInterface()
{
    if (g_gfx_device) {
        delete g_gfx_device;
        g_gfx_device = nullptr;
    }
}

DXGI_FORMAT GetDXGIFormat(TextureFormat fmt)
{
	switch (fmt)
	{
	case TextureFormat::Ru8:     return DXGI_FORMAT_R8_UNORM;
	case TextureFormat::RGu8:    return DXGI_FORMAT_R8G8_UNORM;
	case TextureFormat::RGBAu8:  return DXGI_FORMAT_R8G8B8A8_UNORM;

	case TextureFormat::Rf16:    return DXGI_FORMAT_R16_FLOAT;
	case TextureFormat::RGf16:   return DXGI_FORMAT_R16G16_FLOAT;
	case TextureFormat::RGBAf16: return DXGI_FORMAT_R16G16B16A16_FLOAT;

	case TextureFormat::Ri16:    return DXGI_FORMAT_R16_SNORM;
	case TextureFormat::RGi16:   return DXGI_FORMAT_R16G16_SNORM;
	case TextureFormat::RGBAi16: return DXGI_FORMAT_R16G16B16A16_SNORM;

	case TextureFormat::Rf32:    return DXGI_FORMAT_R32_FLOAT;
	case TextureFormat::RGf32:   return DXGI_FORMAT_R32G32_FLOAT;
	case TextureFormat::RGBAf32: return DXGI_FORMAT_R32G32B32A32_FLOAT;

	case TextureFormat::Ri32:    return DXGI_FORMAT_R32_SINT;
	case TextureFormat::RGi32:   return DXGI_FORMAT_R32G32_SINT;
	case TextureFormat::RGBAi32: return DXGI_FORMAT_R32G32B32A32_SINT;
	}
	return DXGI_FORMAT_UNKNOWN;
}

Result TranslateReturnCode(HRESULT hr)
{
	switch (hr) {
	case S_OK: return Result::OK;
	case E_OUTOFMEMORY: return Result::OutOfMemory;
	case E_INVALIDARG: return Result::InvalidParameter;
	}
	return Result::Unknown;
}