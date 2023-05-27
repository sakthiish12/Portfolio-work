#pragma once
#ifndef BERENDERER_HPP
#define BERENDERER_HPP
#include <Precompiled/pch.h>
#include "Be_device.hpp"
#include "Be_swap_chain.hpp"
#include "../Window/Window.h"

namespace BE {
class BeRenderer {
 public:
  BeRenderer(BeWindow &window, BeDevice &device);
  ~BeRenderer();

  BeRenderer(const BeRenderer &) = delete;
  BeRenderer &operator=(const BeRenderer &) = delete;

  VkRenderPass getSwapChainRenderPass() const { return beSwapChain->getRenderPass(); }
  uint32_t getImageCount() const { return static_cast<uint32_t>(beSwapChain->imageCount()); }
  float getAspectRatio() const { return beSwapChain->extentAspectRatio(); }
  bool isFrameInProgress() const { return isFrameStarted; }

  VkCommandBuffer getCurrentCommandBuffer() const {
    assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
    return commandBuffers[currentFrameIndex];
  }

  int getFrameIndex() const {
    assert(isFrameStarted && "Cannot get frame index when frame not in progress");
    return currentFrameIndex;
  }

  VkCommandBuffer beginFrame();
  void endFrame();
  void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
  void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

  std::unique_ptr<BeSwapChain>& getSwapChain()
  {
      return beSwapChain;
  }
 private:
  void createCommandBuffers();
  void freeCommandBuffers();
  void recreateSwapChain();

  BeWindow &beWindow;
  BeDevice &beDevice;
  std::unique_ptr<BeSwapChain> beSwapChain;
  std::vector<VkCommandBuffer> commandBuffers;

  uint32_t currentImageIndex;
  int currentFrameIndex{0};
  bool isFrameStarted{false};
};
}  // namespace BE
#endif