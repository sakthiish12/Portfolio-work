
#pragma once
#include "Be_device.hpp"
#include "Be_frame_info.hpp"
#pragma warning (push)
#pragma warning (disable:26812)
namespace BE {


    class OffScreen : public Singleton<OffScreen> {

        struct FrameBufferAttachment {
            VkImage image;
            VkDeviceMemory mem;
            VkImageView view;
        };
        struct OffscreenPass {
            uint32_t* width = &tempInfo::viewportSize[0], * height = &tempInfo::viewportSize[1];
            VkFramebuffer frameBuffer;
            FrameBufferAttachment color, depth;
            VkRenderPass renderPass;
            VkSampler sampler;
            VkDescriptorImageInfo descriptor{ VK_NULL_HANDLE };
        };

        std::array<VkClearValue, 2> clearValues{};
        VkDeviceSize offsets[1] = { 0 };
    public:
        OffScreen(BeDevice& device);
        //void Init(Device device);
        ~OffScreen() {
            vkDestroyImageView(m_Device.device(), pass.color.view, nullptr);
            vkDestroyImage(m_Device.device(), pass.color.image, nullptr);
            vkFreeMemory(m_Device.device(), pass.color.mem, nullptr);

            // Depth attachment
            vkDestroyImageView(m_Device.device(), pass.depth.view, nullptr);
            vkDestroyImage(m_Device.device(), pass.depth.image, nullptr);
            vkFreeMemory(m_Device.device(), pass.depth.mem, nullptr);

            vkDestroyRenderPass(m_Device.device(), pass.renderPass, nullptr);
            vkDestroySampler(m_Device.device(), pass.sampler, nullptr);
            vkDestroyFramebuffer(m_Device.device(), pass.frameBuffer, nullptr);
        }

        VkSampler getSampler() { return pass.sampler; }
        VkImageView getImageView() { return pass.color.view; }
        VkRenderPass getRenderPass() { return pass.renderPass; }
        const OffscreenPass getPass() { return pass; }
        OffscreenPass& getPassNonConst() { return pass; }

        void setViewportSize(const glm::vec2& size) {
            *pass.width = (uint32_t)size.x;
            *pass.height = (uint32_t)size.y;

            createImages();
        }

        BeDevice& getDevice() { return m_Device; };

        void start(FrameInfo& frameInfo);
        void end(FrameInfo& frameInfo);
    private:
        void createImages();

        OffscreenPass pass;
        BeDevice& m_Device;

        friend class Singleton<OffScreen>;
    };
#define MainOffScreen OffScreen::GetInstance()
}
#pragma warning (pop)