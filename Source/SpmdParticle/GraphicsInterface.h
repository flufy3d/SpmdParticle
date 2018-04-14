#pragma once


enum class DeviceType
{
    Unknown,
    D3D9,
    D3D11,
    D3D12,
    OpenGL,
    Vulkan,
    PS4,
};

enum class Result
{
    OK,
    Unknown,
    NotAvailable,
    InvalidParameter,
    InvalidOperation,
    OutOfMemory,
};

enum class TextureFormat
{
    Unknown = 0,

    Ru8,
    RGu8,
    RGBAu8,
    Rf16,
    RGf16,
    RGBAf16,
    Ri16,
    RGi16,
    RGBAi16,
    Rf32,
    RGf32,
    RGBAf32,
    Ri32,
    RGi32,
    RGBAi32,
};

enum class BufferType
{
    Index,
    Vertex,
    Constant,
    Compute,
    End,
};

enum class ResourceFlags
{
    None            = 0x0,
    CPU_Write       = 0x1,
    CPU_Read        = 0x2,
    CPU_ReadWrite   = CPU_Read | CPU_Write,
};
inline ResourceFlags operator|(ResourceFlags a, ResourceFlags b) { return ResourceFlags((int)a | (int)b); }
static inline bool operator&(ResourceFlags a, ResourceFlags b) { return ((int)a & (int)b) != 0; }


DXGI_FORMAT GetDXGIFormat(TextureFormat fmt);
Result TranslateReturnCode(HRESULT hr);




inline void CopyRegion(void *dst, int dst_pitch, const void *src, int src_pitch, int num_rows)
{
	if (dst_pitch == src_pitch) {
		memcpy(dst, src, dst_pitch * num_rows);
	}
	else {
		auto *tdst = (char*)dst;
		auto *tsrc = (const char*)src;
		int copy_size = std::min<int>(dst_pitch, src_pitch);
		for (int ri = 0; ri < num_rows; ++ri) {
			memcpy(tdst, tsrc, copy_size);
			tdst += dst_pitch;
			tsrc += src_pitch;
		}
	}
}


class GraphicsInterface
{
protected:
    friend void ReleaseGraphicsInterface();
    virtual ~GraphicsInterface();
    virtual void release() = 0;

public:
    virtual void* getDevicePtr() = 0;
    virtual DeviceType getDeviceType() = 0;

    virtual void    sync() = 0;

    virtual Result  createTexture2D(void **dst_tex, int width, int height, TextureFormat format, const void *data, ResourceFlags flags = ResourceFlags::None) = 0;
    virtual void    releaseTexture2D(void *tex) = 0;
    virtual Result  readTexture2D(void *dst, size_t read_size, void *src_tex, int width, int height, TextureFormat format) = 0;
    virtual Result  writeTexture2D(void *dst_tex, int width, int height, TextureFormat format, const void *src, size_t write_size) = 0;

    virtual Result  createBuffer(void **dst_buf, size_t size, BufferType type, const void *data, ResourceFlags flags = ResourceFlags::None) = 0;
    virtual void    releaseBuffer(void *buf) = 0;
    virtual Result  readBuffer(void *dst, void *src_buf, size_t read_size, BufferType type) = 0;
    virtual Result  writeBuffer(void *dst_buf, const void *src, size_t write_size, BufferType type) = 0;

    static int GetTexelSize(TextureFormat format);
};



GraphicsInterface* CreateGraphicsInterface(DeviceType type, void *device_ptr);

// return instance created by CreateGraphicsInterface()
GraphicsInterface* GetGraphicsInterface();

// release existing instance
void ReleaseGraphicsInterface();


GraphicsInterface* CreateGraphicsInterfaceD3D11(void *device);