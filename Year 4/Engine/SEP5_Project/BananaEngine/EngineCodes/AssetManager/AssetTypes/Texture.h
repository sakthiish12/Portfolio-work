#pragma once

#include "IAsset.h"

namespace BE
{
    enum textureType : std::uint8_t
    {
        INVALID
        , UINT_32_BGRA_8888
        , UINT_32_BGRU_8888
        , UINT_32_ARGB_8888
        , UINT_32_URGB_8888
        , UINT_32_RGBU_8888
        , UINT_32_RGBA_8888
        , UINT_32_ABGR_8888
        , UINT_24_RGB_888
        , UINT_24_ARGB_8565
        , UINT_16_RGBA_4444
        , UINT_16_RGB_565
        , UINT_16_BGRA_5551
        , UINT_16_ABGR_4444
        , UINT_16_ARGB_4444
        , UINT_16_BGR_565
        , UINT_16_ARGB_1555
        , UINT_16_RGBA_5551
        , UINT_16_URGB_1555
        , UINT_16_RGBU_5551
        , UINT_16_ABGR_1555
        , UINT_16_UBGR_1555
        , ENUM_COUNT
        , DEFAULT = UINT_32_RGBA_8888
    };

    enum class format : std::uint8_t
    {
        INVALID

        //
        // Uncompressed formats
        //
        , B8G8R8A8 = textureType::UINT_32_BGRA_8888
        , B8G8R8U8 = textureType::UINT_32_BGRU_8888
        , A8R8G8B8 = textureType::UINT_32_ARGB_8888
        , U8R8G8B8 = textureType::UINT_32_URGB_8888
        , R8G8B8U8 = textureType::UINT_32_RGBU_8888
        , R8G8B8A8 = textureType::UINT_32_RGBA_8888     // PRIMARY FORMAT (xcolor)
        , XCOLOR = R8G8B8A8
        , R8G8B8 = textureType::UINT_24_RGB_888
        , R4G4B4A4 = textureType::UINT_16_RGBA_4444
        , R5G6B5 = textureType::UINT_16_RGB_565
        , B5G5R5A1 = textureType::UINT_16_BGRA_5551

        , XCOLOR_END                                      // end of the range of xcolor

            // S3TC Compressed Texture Image Formats 
        , BC1_4RGB                                        // DXT1_RGB 
        , BC1_4RGBA1                                      // DXT1_RGBA
        , BC2_8RGBA                                       // DXT3_RGBA
        , BC3_8RGBA                                       // DXT5_RGBA

            // RGTC Compressed Texture Image Formats
        , BC4_4R                                          // High quality R  (good for suplemental alpha)
        , BC5_8RG                                         // High Quality RG (good for normal maps)

            // BPTC Compressed Texture Image Formats 
        , BC6H_8RGB_FLOAT                                 // Floating point compression    (good for normal maps)
        , BC7_8RGBA                                       // High quality RGBA compression (good for normal maps) 

            // Ericsson Texture Compression (ETC)
        , ETC2_4RGB
        , ETC2_4RGBA1
        , ETC2_8RGBA

        // ASTC stands for Adaptive Scalable Texture Compression
        , ASTC_4x4_8RGB                                   // 8.00bpp
        , ASTC_5x4_6RGB                                   // 6.40bpp
        , ASTC_5x5_5RGB                                   // 5.12bpp (good for normal maps)
        , ASTC_6x5_4RGB                                   // 4.27bpp
        , ASTC_6x6_4RGB                                   // 3.56bpp
        , ASTC_8x5_3RGB                                   // 3.20bpp
        , ASTC_8x6_3RGB                                   // 2.67bpp
        , ASTC_8x8_2RGB                                   // 2.00bpp
        , ASTC_10x5_3RGB                                  // 2.56bpp
        , ASTC_10x6_2RGB                                  // 2.13bpp
        , ASTC_10x8_2RGB                                  // 1.60bpp
        , ASTC_10x10_1RGB                                 // 1.28bpp
        , ASTC_12x10_1RGB                                 // 1.07bpp
        , ASTC_12x12_1RGB                                 // 0.89bpp

            //
            // Compression formats
            //
        , PAL4_R8G8B8A8                                  // 4 bpp Index + 16  RGBA8888 palette
        , PAL8_R8G8B8A8                                  // 8 bpp Index + 256 RGBA8888 palette

            // PVR compression modes
        , PVR1_2RGB
        , PVR1_2RGBA
        , PVR1_4RGB
        , PVR1_4RGBA
        , PVR2_2RGBA
        , PVR2_4RGBA

        //
        // Extra Frame buffer Formats
        //
        , D24S8_FLOAT                                     // Floating point depth and 8bit stencil
        , D24S8                                           // Depth 24 bits and 8 bit Stencil    
        , R8
        , R32
        , R8G8
        , R16G16B16A16
        , R16G16B16A16_FLOAT
        , A2R10G10B10
        , B11G11R11_FLOAT

        //
        // End
        //
        , ENUM_COUNT
    };

    enum class formatTexture : std::uint8_t
    {
        INVALID
        , B8G8R8A8
        , B8G8R8U8
        , A8R8G8B8
        , U8R8G8B8
        , R8G8B8U8
        , R8G8B8A8
        , XCOLOR = R8G8B8A8
        , R8G8B8
        , R4G4B4A4 = 10
        , R5G6B5
        , B5G5R5A1
        , XCOLOR_END              // end of the range of xcolor
        , BC1_4RGB
        , BC1_4RGBA1
        , BC2_8RGBA
        , BC3_8RGBA
        , BC4_4R
        , BC5_8RG
        , BC6H_8RGB_FLOAT
        , BC7_8RGBA
        , ETC2_4RGB
        , ETC2_4RGBA1
        , ETC2_8RGBA
        , ENUM_COUNT
        , DEFAULT = R8G8B8A8
    };

    enum class color_space : std::uint8_t
    {
        SRGB
        , LINEAR
    };

    struct myMip
    {
        std::int32_t m_Offset;
    };

    struct texture_format
    {
        VkFormat            m_VKFormatLinearUnsigned;
        VkFormat            m_VKFormatLinearSigned;
        VkFormat            m_VKFormatSRGB;
    };

    struct sampler
    {
        enum class address_mode : std::uint8_t
        {
            TILE
            , CLAMP
            , CLAMP_BORDER
            , MIRROR
            , MIRROR_CLAMP
            , ENUM_COUNT
        };

        enum class mipmap_sampler : std::uint8_t
        {
            LINEAR
            , NEAREST
            , ENUM_COUNT
        };

        enum class mipmap_mode : std::uint8_t
        {
            LINEAR
            , NEAREST
            , ENUM_COUNT
        };

        float                       m_MaxAnisotropy{ 8.0f };
        bool                        m_bDisableAnisotropy{ false };
        std::array<address_mode, 3>  m_AddressMode{ address_mode::TILE, address_mode::TILE, address_mode::TILE };
        mipmap_sampler              m_MipmapMag{ mipmap_sampler::LINEAR };
        mipmap_sampler              m_MipmapMin{ mipmap_sampler::LINEAR };
        mipmap_mode                 m_MipmapMode{ mipmap_mode::LINEAR };
    };

    struct TextureInitialiser
    {
        std::string filePath;
        TextureInitialiser() = default;
        TextureInitialiser(const TextureInitialiser& in) = default;
        TextureInitialiser(const std::string& in);
        RTTR_ENABLE()
    };


    class Texture : public IAsset
    {
    public:
        //In the order of the texturetypes
        Texture(const std::string& filePath); 
        ~Texture();
        Texture(Texture&& _rhs) noexcept;
        Texture() = default;
        Texture& operator=(Texture&& _rhs) noexcept;
        
        VkDescriptorImageInfo & getDescriptorImageInfo();

        //Construct the data for non-templated load asset
        void Construct(const TextureInitialiser& _data);
        void ConstructFromPointer(TextureInitialiser* _data);
        TextureInitialiser Serialise()
        {
            return TextureInitialiser();
        }
    private:
        const VkFormat getVKTextureFormat(format currFormat, bool linear, bool currBool);
        void transitionImageLayout(VkImage& image, VkImageLayout oldLayout, VkImageLayout newLayout);
        void copyBufferToImage(VkBuffer&, VkImage&, uint32_t width, uint32_t height);
        void createImageView(VkImage&, VkImageView&,VkFormat, VkImageAspectFlags flags= VK_IMAGE_ASPECT_COLOR_BIT);

        VkDescriptorImageInfo imageDescriptorInfo{};
        VkImage textureImages{ VK_NULL_HANDLE };
        VkImageView textureImageView{ VK_NULL_HANDLE };
        VkDeviceMemory textureDeviceMemory{ VK_NULL_HANDLE };
        VkSampler textureSampler{ VK_NULL_HANDLE };
        VkImageLayout textureImageLayout;


        //temp
        unsigned long long sizeOfImage = 0;
        VkBuffer stagingBuffers{ VK_NULL_HANDLE };
        VkDeviceMemory stagingBufferMemories{ VK_NULL_HANDLE };
        VkImageCreateInfo imageInfos;
        RTTR_ENABLE(IAsset)
    };

    struct TextureSet
    {
        std::vector<Texture> textures;
    };
}  // namespace BE
