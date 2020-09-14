#pragma once
#if defined(UNITY_WIN) || defined(UNITY_LINUX)
#include <cuda.h>
#include "GraphicsDevice/Vulkan/VulkanGraphicsDevice.h"
#endif 
#include <shared_mutex>
#include "GraphicsDevice/IGraphicsDevice.h"

using namespace webrtc;

namespace unity {
namespace webrtc {

class VideoFrameBufferCreatorInterface
{
public:
    VideoFrameBufferCreatorInterface(
        IGraphicsDevice* device, void* frame, uint32_t destMemoryType);
    virtual ~VideoFrameBufferCreatorInterface();
    virtual void Init() = 0;
    virtual rtc::scoped_refptr<VideoFrameBuffer> CreateBuffer(
        std::shared_mutex& mutex) = 0;
public:
    static VideoFrameBufferCreatorInterface* Create(
        IGraphicsDevice* device, NativeTexPtr ptr, UnityGfxRenderer renderer, uint32_t destMemoryType);
protected:
    int m_width;
    int m_height;
    IGraphicsDevice* m_device;
    void* m_frame;
    bool m_useCpu;
    bool m_useGpu;
};

#if defined(UNITY_WIN)
class VideoFrameBufferCreatorD3D11 : public VideoFrameBufferCreatorInterface
{
public:
    VideoFrameBufferCreatorD3D11(
        IGraphicsDevice* m_device, NativeTexPtr ptr, uint32_t destMemoryType);
    virtual ~VideoFrameBufferCreatorD3D11();

    void Init() override;
    rtc::scoped_refptr<VideoFrameBuffer> CreateBuffer(
        std::shared_mutex& mutex) override;
private:
    rtc::scoped_refptr<I420Buffer> CreateI420Buffer(ITexture2D* tex);

    CUgraphicsResource m_resource;
    CUarray m_mappedArray;
    std::unique_ptr<ITexture2D> m_gpuReadTexture;
    std::unique_ptr<ITexture2D> m_cpuReadTexture;
    rtc::scoped_refptr<I420Buffer> m_dummyBuffer;
};

class VideoFrameBufferCreatorD3D12 : public VideoFrameBufferCreatorInterface
{
public:
    VideoFrameBufferCreatorD3D12(
        IGraphicsDevice* m_device, NativeTexPtr ptr, uint32_t destMemoryType);
    virtual ~VideoFrameBufferCreatorD3D12();

    void Init() override;
    rtc::scoped_refptr<VideoFrameBuffer> CreateBuffer(
        std::shared_mutex& mutex) override;
private:
    rtc::scoped_refptr<I420Buffer> CreateI420Buffer(ITexture2D* tex);

    CUgraphicsResource m_resource;
    CUarray m_mappedArray;
    std::unique_ptr<ITexture2D> m_gpuReadTexture;
    std::unique_ptr<ITexture2D> m_cpuReadTexture;
    rtc::scoped_refptr<I420Buffer> m_dummyBuffer;
};
#endif

class VideoFrameBufferCreatorVulkan : public VideoFrameBufferCreatorInterface
{
public:
    VideoFrameBufferCreatorVulkan(
        IGraphicsDevice* m_device, NativeTexPtr ptr, uint32_t destMemoryType);
    virtual ~VideoFrameBufferCreatorVulkan();

    void Init() override;
    rtc::scoped_refptr<VideoFrameBuffer> CreateBuffer(
        std::shared_mutex& mutex) override;
private:
    rtc::scoped_refptr<I420Buffer> CreateI420Buffer(ITexture2D* tex);

    UnityVulkanImage* m_image;
    std::unique_ptr<ITexture2D> m_gpuReadTexture;
    std::unique_ptr<ITexture2D> m_cpuReadTexture;
    VulkanGraphicsDevice* m_vulkanDevice;
    rtc::scoped_refptr<I420Buffer> m_dummyBuffer;
};

#if defined(UNITY_MAC)
class VideoFrameBufferCreatorMetal : VideoFrameBufferCreatorInterface
{
public:
    VideoFrameBufferCreatorMetal(
        IGraphicsDevice* m_device, NativeTexPtr ptr, uint32_t destMemoryType);
    virtual ~VideoFrameBufferCreatorMetal();

    void Init() override;
    rtc::scoped_refptr<VideoFrameBuffer> CreateBuffer() override;
}
#endif

#if defined(UNITY_LINUX)
class VideoFrameBufferCreatorOpenGL : public VideoFrameBufferCreatorInterface
{
public:
    VideoFrameBufferCreatorOpenGL(
        IGraphicsDevice* m_device, NativeTexPtr ptr, uint32_t destMemoryType);
    virtual ~VideoFrameBufferCreatorOpenGL();

    void Init() override;
    rtc::scoped_refptr<VideoFrameBuffer> CreateBuffer(
        std::shared_mutex& mutex) override;
private:
    CUgraphicsResource m_resource;
    CUarray m_mappedArray;
    std::unique_ptr<ITexture2D> m_gpuReadTexture;
};
#endif
} // end namespace webrtc
} // end namespace unity