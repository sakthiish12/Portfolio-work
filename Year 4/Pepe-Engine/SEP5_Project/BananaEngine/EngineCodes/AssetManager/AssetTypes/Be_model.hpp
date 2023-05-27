#pragma once
#ifndef BEMODEL_HPP
#define BEMODEL_HPP

#include "IAsset.h"
#include <Systems/Graphics/Be_buffer.hpp>

namespace BE
{
    struct BoneInfo;
    class Animation;

    constexpr const int maxTextures = 6;
    struct Vertex {
        glm::vec3 position{};
        glm::vec3 color{};
        glm::vec3 normal{};
        glm::vec2 uv{};
        int bone_idxs[4]{-1,-1,-1,-1};
        float bone_weights[4]{ 0.0f,0.0f,0.0f,0.0f };
        glm::vec3 tangents{};
        glm::vec3 bitangents{};

        static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

        bool operator==(const Vertex& other) const {
            return position == other.position && color == other.color && normal == other.normal &&
                uv == other.uv && tangents == other.bitangents && bitangents == other.bitangents;
        }
    };
   

    struct ParticleVertex
    {
        glm::vec3       m_Position;         // World Space
        glm::vec2       m_UV;
        std::uint32_t   m_Color;

        static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
    };



    static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
    {
        glm::mat4 to;
        //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
        to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
        to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
        to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
        to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
        return to;
    }


    struct ModelFileInitialiser
    {
        std::string filePath;
        ModelFileInitialiser(const std::string& in);
        ModelFileInitialiser() = default;
        ModelFileInitialiser(const ModelFileInitialiser& in) = default;
    };

    struct Mesh
    {
        std::string meshName{};
        uint32_t vertexCount{};
        std::vector<Vertex> vertices;
        std::unique_ptr<BeBuffer> vertexBuffer;

        bool hasIndexBuffer = false;
        uint32_t indexCount{};
        std::vector<uint32_t> indices;
        std::unique_ptr<BeBuffer> indexBuffer;

        void write(std::ofstream& stream);
        void read(std::ifstream& stream);
    };

    class BeModel : public IAsset
    {
    public:
        BeModel() = default;
        BeModel(const std::string& filePath);
        //While loading, the model vertices will be extracted, the bone details will be extracted
        //after extraction, loop through the tree and set the correct id of the bone's parent bone
        BeModel(const std::string& filePath, bool conversion);

        ~BeModel();
        BeModel(BeModel&& _rhs) noexcept;
        BeModel& operator=(BeModel&& _rhs) noexcept;

        void bind(VkCommandBuffer commandBuffer, int index);
        void draw(VkCommandBuffer commandBuffer, int index);

        //Read/write
        void write(std::ofstream& stream);
        void read(std::ifstream& stream);

        inline std::vector<BoneInfo>& getBoneInfoMap();
        std::vector<Mesh>& getMeshes();
        Animation& GetSpecificAnimation(unsigned id);
        const std::vector<Animation>& GetAnimationDetails();

        //Construct the data for non-templated load asset
        void Construct(const ModelFileInitialiser& _data);
        void ConstructFromPtr(ModelFileInitialiser* _data);
        ModelFileInitialiser Serialise()
        {
            return ModelFileInitialiser();
        }
        RTTR_ENABLE(IAsset)
    private:
        inline void createVertexBuffers(Mesh& m);
        inline void createIndexBuffers(Mesh& m);

        //Assimp
        inline void processMesh(aiMesh* mesh, const aiScene* scene);
        inline void processNode(aiNode* node, const aiScene* scene);

        inline void createAnimations(const aiScene* scene, const std::string& filePath);

        //v2
        inline bool isBoneOnlyNode(aiNode* node);
        inline aiNode* findRootBone(aiScene* scene);
        inline void loadBones(aiNode* node, int index);
        inline void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
        inline void balanceOutVertexBoneData(std::vector<Vertex>& vertex);

        std::vector<BoneInfo> m_BoneInfoMap; //write
        std::vector<Mesh> allMeshes; //write
        std::vector<Animation> animationDetails;
    };
}  // namespace BE
#endif