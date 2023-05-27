#include "pch.h"
#include "Texture.h"

#define TINYDDSLOADER_IMPLEMENTATION
#include <tinyddsloader.h>

#include <Systems/RendererSystem/RendererSystem.h>
#include <Serialisation/BananaSerialisation.h>

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	registration::class_<Texture>(BE::RegistrationNameCreator<Texture>())
	.constructor()()
	.method(assetConstructMethodName, &Texture::ConstructFromPointer)()
	.method(serializeAssetMethodName, &Texture::Serialise)();
	registration::class_<TextureInitialiser>(BE::RegistrationNameCreator<TextureInitialiser>())
		.constructor()()
		.property(filePathName, &TextureInitialiser::filePath);
}
namespace BE
{
	std::tuple<format, color_space, bool> ConvertFormat(tinyddsloader::DDSFile::DXGIFormat Format)
	{
		switch (Format)
		{
		case tinyddsloader::DDSFile::DXGIFormat::BC1_UNorm:
			return std::tuple<format, color_space, bool>{ format::BC1_4RGBA1, color_space::LINEAR, false };
		case tinyddsloader::DDSFile::DXGIFormat::BC1_UNorm_SRGB:
			return std::tuple<format, color_space, bool>{ format::BC1_4RGBA1, color_space::SRGB, false };
		case  tinyddsloader::DDSFile::DXGIFormat::BC2_UNorm:
			return std::tuple<format, color_space, bool>{format::BC2_8RGBA, color_space::LINEAR, false };
		case tinyddsloader::DDSFile::DXGIFormat::BC2_UNorm_SRGB:
			return std::tuple<format, color_space, bool>{format::BC2_8RGBA, color_space::SRGB, false };
		case  tinyddsloader::DDSFile::DXGIFormat::BC3_UNorm:
			return std::tuple<format, color_space, bool>{format::BC3_8RGBA, color_space::LINEAR, false };
		case  tinyddsloader::DDSFile::DXGIFormat::BC3_UNorm_SRGB:
			return std::tuple<format, color_space, bool>{format::BC3_8RGBA, color_space::SRGB, false };
		case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_UNorm:
			return std::tuple<format, color_space, bool>{ format::R8G8B8A8, color_space::LINEAR, false };
		case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_UNorm_SRGB:
			return std::tuple<format, color_space, bool>{ format::R8G8B8A8, color_space::SRGB, false };
		case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_SNorm:
			return std::tuple<format, color_space, bool>{ format::R8G8B8A8, color_space::LINEAR, true };
		case tinyddsloader::DDSFile::DXGIFormat::B8G8R8A8_UNorm:
			return std::tuple<format, color_space, bool>{ format::B8G8R8A8, color_space::LINEAR, false };
		case tinyddsloader::DDSFile::DXGIFormat::B8G8R8A8_UNorm_SRGB:
			return std::tuple<format, color_space, bool>{ format::B8G8R8A8, color_space::SRGB, false };
		case tinyddsloader::DDSFile::DXGIFormat::B8G8R8A8_Typeless:
			return std::tuple<format, color_space, bool>{ format::B8G8R8A8, color_space::LINEAR, true };
		case tinyddsloader::DDSFile::DXGIFormat::B8G8R8X8_UNorm:
			return std::tuple<format, color_space, bool>{ format::B8G8R8U8, color_space::LINEAR, false };
		case tinyddsloader::DDSFile::DXGIFormat::B8G8R8X8_UNorm_SRGB:
			return std::tuple<format, color_space, bool>{ format::B8G8R8U8, color_space::SRGB, false };
		case tinyddsloader::DDSFile::DXGIFormat::B8G8R8X8_Typeless:
			return std::tuple<format, color_space, bool>{ format::B8G8R8U8, color_space::LINEAR, true };
		case  tinyddsloader::DDSFile::DXGIFormat::BC5_UNorm:
			return std::tuple<format, color_space, bool>{ format::BC5_8RG, color_space::LINEAR, true };
		}
		return std::tuple<format, color_space, bool>{ format::INVALID, color_space::SRGB, false };
	}

	Texture::Texture(const std::string& filePath)
	{
		TextureInitialiser TextIni{ filePath };

		try
		{
			this->Construct(TextIni);
		}
		catch (const std::runtime_error&)
		{
			throw;
		}
	}

	Texture::~Texture()
	{
		if (textureImages != VK_NULL_HANDLE)
		{
			vkDestroyImage(BE::RendererSystem::GetInstance()->getDevice()->device(), textureImages, nullptr);
		}

		if (textureImageView != VK_NULL_HANDLE)
		{
			vkDestroyImageView(BE::RendererSystem::GetInstance()->getDevice()->device(), textureImageView, nullptr);
		}

		if (textureDeviceMemory != VK_NULL_HANDLE)
		{
			vkFreeMemory(BE::RendererSystem::GetInstance()->getDevice()->device(), textureDeviceMemory, nullptr);
		}

		if (textureSampler != VK_NULL_HANDLE)
		{
			vkDestroySampler(BE::RendererSystem::GetInstance()->getDevice()->device(), textureSampler, nullptr);
		}

		if (stagingBuffers != VK_NULL_HANDLE)
		{
			vkDestroyBuffer(BE::RendererSystem::GetInstance()->getDevice()->device(), stagingBuffers, nullptr);
		}

		if (stagingBufferMemories != VK_NULL_HANDLE)
		{
			vkFreeMemory(BE::RendererSystem::GetInstance()->getDevice()->device(), stagingBufferMemories, nullptr);
		}
	}

	Texture::Texture(Texture&& _rhs) noexcept
	{
		//create image
		if (_rhs.sizeOfImage == 0)
		{
			//LogError("Attempted to create a copy of a empty texture, stopped vulkan error");
			return;
		}
		BE::AssetManager::GetInstance()->getDevice()->createImageWithInfo(_rhs.imageInfos, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImages, textureDeviceMemory);

		transitionImageLayout(textureImages, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		copyBufferToImage(_rhs.stagingBuffers, textureImages, static_cast<uint32_t>(_rhs.imageInfos.extent.width), static_cast<uint32_t>(_rhs.imageInfos.extent.height));
		transitionImageLayout(textureImages, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		createImageView(textureImages, textureImageView, _rhs.imageInfos.format);

		textureImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		//create sampler
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.anisotropyEnable = VK_TRUE;

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(BE::RendererSystem::GetInstance()->getDevice()->getPhysicalDevice(), &properties);

		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		if (vkCreateSampler(BE::RendererSystem::GetInstance()->getDevice()->device(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
		{
			AssertCore("failed to create texture sampler!\n","");
		}

		imageDescriptorInfo.imageLayout = textureImageLayout;
		imageDescriptorInfo.imageView = textureImageView;
		imageDescriptorInfo.sampler = textureSampler;
	}

	void Texture::Construct(const TextureInitialiser& _data)
	{
		//Double check if file is .dds extension
		if (_data.filePath.find(".dds") == std::string::npos)
		{
			AssertCore("Texture being loaded is not of .dds type: %s", _data.filePath.c_str());
		}

		using namespace tinyddsloader;

		tinyddsloader::DDSFile dds;
		auto result = dds.Load(_data.filePath.c_str());

		if (tinyddsloader::Result::Success != result)
		{
			AssertCore("Failed to load dds: %s", _data.filePath.c_str());
		}

		if (dds.GetTextureDimension() == tinyddsloader::DDSFile::TextureDimension::Unknown
			|| dds.GetTextureDimension() == tinyddsloader::DDSFile::TextureDimension::Texture3D)
		{
			AssertCore("Unsupported dimension of texture: %s", _data.filePath.c_str());
		}

		format Format{};
		color_space colorSpace{};
		bool isSigned{};

		std::tuple<format, color_space, bool> f = ConvertFormat(dds.GetFormat());

		if (std::get<format>(f) == format::INVALID)
		{
			AssertCore("Unsupported Texture format of texture: %s", _data.filePath.c_str());
		}
		else
		{
			Format = std::get<format>(f);
			colorSpace = std::get<color_space>(f);
			isSigned = std::get<bool>(f);
		}

		const auto MipTableBytes = dds.GetMipCount() * sizeof(myMip);
		const auto FaceByteSize = [&]
		{
			auto FaceByteSize = 0;

			// Mips should be organized from biggest to smallest
			std::uint32_t PrevW = 0xffffffff;
			for (std::uint32_t i = 0; i < dds.GetMipCount(); i++)
			{
				auto View = dds.GetImageData(i, 0);
				FaceByteSize += View->m_memSlicePitch;
				if (View->m_width >= PrevW)
				{
					return -1;
				}
				PrevW = View->m_width;
			}

			return FaceByteSize;
		}();

		if (FaceByteSize == -1)
		{
			AssertCore("Mips organized incorrectly of texture: %s", _data.filePath.c_str());
		}

		const auto nSubFaces = dds.IsCubemap() ? 6u : 1u;
		const auto FrameByteSize = FaceByteSize * nSubFaces;
		const auto nFrames = dds.GetArraySize();
		const auto TotalByteSize = MipTableBytes + static_cast<unsigned long long>(FrameByteSize) * static_cast<unsigned long long>(nFrames);
		auto Memory = std::make_unique<uint8_t[]>(TotalByteSize);
		auto pMipOffset = reinterpret_cast<myMip*>(Memory.get());
		auto pFrame = reinterpret_cast<uint8_t*>(&pMipOffset[dds.GetMipCount()]);

		// Set the very first offset
		pMipOffset[0].m_Offset = 0;

		for (std::uint32_t iFrame = 0; iFrame < nFrames; ++iFrame)
		{
			for (std::uint32_t iSubFace = 0; iSubFace < nSubFaces; ++iSubFace)
			{
				auto TopMipView = dds.GetImageData(0, iFrame * iSubFace);
				for (std::uint32_t iMip = 0; iMip < dds.GetMipCount(); ++iMip)
				{
					auto View = dds.GetImageData(iMip, iFrame * iSubFace);
					auto ByteSize = View->m_memSlicePitch;

					// Set the offset of the next mip
					if (iFrame == 0 && iSubFace == 0)
					{
						if ((iMip + 1) < dds.GetMipCount()) pMipOffset[iMip + 1].m_Offset = pMipOffset[iMip].m_Offset + static_cast<int>(ByteSize);
					}
					else
					{
						if (pMipOffset[iMip].m_Offset != static_cast<int>(ByteSize))
						{
							AssertCore("Unexcepted mipmap offset of file: %s", _data.filePath.c_str());
						}
					}

					// Make sure that the size formula follows what we expect
					if (std::max(1u, (TopMipView->m_height >> iMip)) != View->m_height
						|| std::max(1u, (TopMipView->m_width >> iMip)) != View->m_width)
					{
						AssertCore("Unexcepted mipmap size formulation of file: %s", _data.filePath.c_str());
					}

					// Copy the mip data
					std::memcpy(&pFrame[pMipOffset[iMip].m_Offset + iSubFace * FaceByteSize + FrameByteSize * iFrame], View->m_mem, ByteSize);
				}
			}
		}

		VkFormat textureFormat = getVKTextureFormat(Format, colorSpace == color_space::LINEAR || colorSpace == color_space::SRGB, isSigned);

		if (textureFormat == VK_FORMAT_BC1_RGBA_UNORM_BLOCK)
		{
			textureFormat = VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
		}
		else if (textureFormat == VK_FORMAT_BC3_UNORM_BLOCK)
		{
			textureFormat = VK_FORMAT_BC3_SRGB_BLOCK;
		}

		auto View = dds.GetImageData(0, 0);

		//store image size to use later
		sizeOfImage = TotalByteSize;

		//temporary buffer to transfer data to be used later
		BE::AssetManager::GetInstance()->getDevice()->createBuffer(TotalByteSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffers, stagingBufferMemories);

		void* data;
		vkMapMemory(BE::AssetManager::GetInstance()->getDevice()->device(), stagingBufferMemories, 0, TotalByteSize, 0, &data);
		memcpy(data, View->m_mem, static_cast<size_t>(TotalByteSize));
		vkUnmapMemory(BE::AssetManager::GetInstance()->getDevice()->device(), stagingBufferMemories);

		//save image info to be used later
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = View->m_width;
		imageInfo.extent.height = View->m_height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = textureFormat;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		imageInfos = imageInfo;
	}

	void Texture::ConstructFromPointer(TextureInitialiser* _data)
	{
		Construct(*_data);
	}


	Texture& Texture::operator=(Texture&& _rhs) noexcept
	{
		return *this;
	}

	VkDescriptorImageInfo & Texture::getDescriptorImageInfo()
	{
		return imageDescriptorInfo;
	}

	const VkFormat Texture::getVKTextureFormat(format currFormat, bool linear, bool currBool)
	{
		// https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html 
						  // VK Interpretation of Numeric Format
						  // UNORM   - The components are unsigned normalized values in the range [0,1]
						  // SNORM   - The components are signed normalized values in the range [-1,1]
						  // USCALED - The components are unsigned integer values that get converted to floating-point in the range [0,2n-1]
						  // SSCALED - The components are signed integer values that get converted to floating-point in the range [-2n-1,2n-1-1]
						  // UINT    - The components are unsigned integer values in the range [0,2n-1]
						  // SINT    - The components are signed integer values in the range [-2n-1,2n-1-1]
						  // UFLOAT  - The components are unsigned floating-point numbers (used by packed, shared exponent, and some compressed formats)
						  // SFLOAT  - The components are signed floating-point numbers
						  // SRGB    - The R, G, and B components are unsigned normalized values that represent values using sRGB nonlinear encoding, 
						  //           while the A component (if one exists) is a regular unsigned normalized value
		static auto FormatTable = []() noexcept -> auto
		{
			auto _FormatTable = std::array<texture_format, (int)format::ENUM_COUNT>{ texture_format{ VK_FORMAT_UNDEFINED,VK_FORMAT_UNDEFINED,VK_FORMAT_UNDEFINED } };

			//             format                                            VKFormat_LinearUnsigned                VKFormat_LinearSigned                   VKFormat_SRGB
			//           ---------------------------------                                  ----------------------------------    -----------------------------------    ------------------------------------
			_FormatTable[(int)formatTexture::B8G8R8A8] = texture_format{ VK_FORMAT_B8G8R8A8_UNORM            , VK_FORMAT_B8G8R8A8_SNORM             , VK_FORMAT_B8G8R8A8_SRGB };
			_FormatTable[(int)formatTexture::B8G8R8U8] = texture_format{ VK_FORMAT_B8G8R8A8_UNORM            , VK_FORMAT_B8G8R8A8_SNORM             , VK_FORMAT_B8G8R8A8_SRGB };
			_FormatTable[(int)formatTexture::A8R8G8B8] = texture_format{ VK_FORMAT_UNDEFINED                 , VK_FORMAT_UNDEFINED                  , VK_FORMAT_UNDEFINED };
			_FormatTable[(int)formatTexture::U8R8G8B8] = texture_format{ VK_FORMAT_UNDEFINED                 , VK_FORMAT_UNDEFINED                  , VK_FORMAT_UNDEFINED };
			_FormatTable[(int)formatTexture::R8G8B8U8] = texture_format{ VK_FORMAT_R8G8B8A8_UNORM            , VK_FORMAT_R8G8B8A8_SNORM             , VK_FORMAT_R8G8B8A8_SRGB };
			_FormatTable[(int)formatTexture::R8G8B8A8] = texture_format{ VK_FORMAT_R8G8B8A8_UNORM            , VK_FORMAT_R8G8B8A8_SNORM             , VK_FORMAT_R8G8B8A8_SRGB };
			_FormatTable[(int)formatTexture::R8G8B8] = texture_format{ VK_FORMAT_R8G8B8_UNORM              , VK_FORMAT_R8G8B8_SNORM               , VK_FORMAT_R8G8B8_SRGB };
			_FormatTable[(int)formatTexture::R4G4B4A4] = texture_format{ VK_FORMAT_R4G4B4A4_UNORM_PACK16     , VK_FORMAT_UNDEFINED                  , VK_FORMAT_R4G4B4A4_UNORM_PACK16 };

			_FormatTable[(int)formatTexture::R5G6B5] = texture_format{ VK_FORMAT_R5G6B5_UNORM_PACK16       , VK_FORMAT_UNDEFINED                  , VK_FORMAT_R5G6B5_UNORM_PACK16 };
			_FormatTable[(int)formatTexture::B5G5R5A1] = texture_format{ VK_FORMAT_B5G5R5A1_UNORM_PACK16     , VK_FORMAT_UNDEFINED                  , VK_FORMAT_B5G5R5A1_UNORM_PACK16 };

			_FormatTable[(int)formatTexture::BC1_4RGB] = texture_format{ VK_FORMAT_BC1_RGB_UNORM_BLOCK       , VK_FORMAT_UNDEFINED                  , VK_FORMAT_BC1_RGB_SRGB_BLOCK };
			_FormatTable[(int)formatTexture::BC1_4RGBA1] = texture_format{ VK_FORMAT_BC1_RGBA_UNORM_BLOCK      , VK_FORMAT_UNDEFINED                  , VK_FORMAT_BC1_RGBA_SRGB_BLOCK };
			_FormatTable[(int)formatTexture::BC2_8RGBA] = texture_format{ VK_FORMAT_BC2_UNORM_BLOCK           , VK_FORMAT_UNDEFINED                  , VK_FORMAT_BC2_SRGB_BLOCK };
			_FormatTable[(int)formatTexture::BC3_8RGBA] = texture_format{ VK_FORMAT_BC3_UNORM_BLOCK           , VK_FORMAT_UNDEFINED                  , VK_FORMAT_BC3_SRGB_BLOCK };
			_FormatTable[(int)formatTexture::BC4_4R] = texture_format{ VK_FORMAT_BC4_UNORM_BLOCK           , VK_FORMAT_BC4_SNORM_BLOCK            , VK_FORMAT_UNDEFINED };
			_FormatTable[(int)formatTexture::BC5_8RG] = texture_format{ VK_FORMAT_BC5_UNORM_BLOCK           , VK_FORMAT_BC5_SNORM_BLOCK            , VK_FORMAT_UNDEFINED };
			_FormatTable[(int)formatTexture::BC6H_8RGB_FLOAT] = texture_format{ VK_FORMAT_BC6H_UFLOAT_BLOCK         , VK_FORMAT_BC6H_SFLOAT_BLOCK          , VK_FORMAT_UNDEFINED };
			_FormatTable[(int)formatTexture::BC7_8RGBA] = texture_format{ VK_FORMAT_BC7_UNORM_BLOCK           , VK_FORMAT_UNDEFINED                  , VK_FORMAT_BC7_SRGB_BLOCK };

			_FormatTable[(int)formatTexture::ETC2_4RGB] = texture_format{ VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK   , VK_FORMAT_UNDEFINED                  , VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK };
			_FormatTable[(int)formatTexture::ETC2_4RGBA1] = texture_format{ VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK , VK_FORMAT_UNDEFINED                  , VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK };
			_FormatTable[(int)formatTexture::ETC2_8RGBA] = texture_format{ VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK , VK_FORMAT_UNDEFINED                  , VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK };

			return _FormatTable;
		}();

		const auto& Entry = FormatTable[(int)currFormat];
		if (linear)
		{
			if (currBool) return Entry.m_VKFormatLinearSigned;
			return Entry.m_VKFormatLinearUnsigned;
		}

		return Entry.m_VKFormatSRGB;
	}

	void Texture::transitionImageLayout(VkImage& image, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		VkCommandBuffer newCommandBuffer = BE::AssetManager::GetInstance()->getDevice()->beginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else {
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(
			newCommandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		BE::AssetManager::GetInstance()->getDevice()->endSingleTimeCommands(newCommandBuffer);
	}

	void Texture::copyBufferToImage(VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height)
	{
		VkCommandBuffer newCommandBuffer = BE::AssetManager::GetInstance()->getDevice()->beginSingleTimeCommands();

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { width,height,1 };

		vkCmdCopyBufferToImage(newCommandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		BE::AssetManager::GetInstance()->getDevice()->endSingleTimeCommands(newCommandBuffer);
	}

	void Texture::createImageView(VkImage& image, VkImageView& imageView,VkFormat format, VkImageAspectFlags aspectFlags)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkResult result = vkCreateImageView(BE::AssetManager::GetInstance()->getDevice()->device(), &viewInfo, nullptr, &imageView);
		AssertCore(result == VK_SUCCESS, "failed to create image view in createImageView()");
	}
	TextureInitialiser::TextureInitialiser(const std::string& in)
		:filePath{in}
	{
	}
};
//#pragma warning (pop)