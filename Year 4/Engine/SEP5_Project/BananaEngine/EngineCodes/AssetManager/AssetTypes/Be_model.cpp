#include "pch.h"
#include "Be_model.hpp"
#include <Systems/Graphics/Be_buffer.hpp>
#include <Systems/RendererSystem/RendererSystem.h>
#include <Serialisation/BananaSerialisation.h>

#ifndef ENGINE_DIR
#define ENGINE_DIR "../"
#endif

#pragma warning (push)
#pragma warning (disable: 26812) 

RTTR_REGISTRATION
{
    using namespace rttr;
    using namespace BE;
    using namespace BE::Serialisation;
    registration::class_<BeModel>(RegistrationNameCreator<BeModel>())
    .constructor()()
    .method(assetConstructMethodName, &BeModel::ConstructFromPtr)()
    .method(serializeAssetMethodName, &BeModel::Serialise)();
    registration::class_<ModelFileInitialiser>(RegistrationNameCreator<ModelFileInitialiser>())
    .constructor()()
    .property(Serialisation::filePathName, &ModelFileInitialiser::filePath);
}

namespace BE {

    BeModel::BeModel(const std::string& filePath)
    {
        ModelFileInitialiser Model{ filePath };

        try
        {
            this->Construct(Model);
        }
        catch (const std::runtime_error&)
        {
            throw;
        }
    }

    BeModel::BeModel(const std::string& filePath, bool) //this is used in assetmanager, to load for conversion
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filePath.c_str(),
            aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights
            | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality
        );

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            //LogCoreError("Error loading model using assimp from %s", filePath.c_str());
            return;
        }

        if (scene->HasAnimations())
        {
            aiNode* rootBone = findRootBone(const_cast<aiScene*>(scene));

            if (rootBone)
            {
                //recursively add
                loadBones(rootBone, -1);
            }
        }

        //Extract vertex weight and stuff by checking if the bone it has is already detected
        processNode(scene->mRootNode, scene);

        if (!scene->HasAnimations())
        {
            //Rotate first
            //for (size_t i{}; i < allMeshes.size(); ++i)
            //{
            //    for (size_t v{}; v < allMeshes[i].vertices.size(); ++v)
            //    {
            //        allMeshes[i].vertices[v].position = glm::mat3{ 1.0f,0.0f,0.0f,
            //            0.0f,glm::cos(glm::radians(-90.0f)),glm::sin(glm::radians(-90.0f)),0.0f,-1.0f * glm::sin(glm::radians(-90.0f)),glm::cos(glm::radians(-90.0f)) }
            //        *allMeshes[i].vertices[v].position;
            //    }
            //}
            //Normalize the vertices first
            glm::vec3 maxValues{ -FLT_MAX,-FLT_MAX,-FLT_MAX };
            glm::vec3 minValues{ FLT_MAX ,FLT_MAX ,FLT_MAX };

            for (size_t i{}; i < allMeshes.size(); ++i)
            {
                for (size_t v{}; v < allMeshes[i].vertices.size(); ++v)
                {
                    if (maxValues.x < allMeshes[i].vertices[v].position.x)
                        maxValues.x = allMeshes[i].vertices[v].position.x;

                    if (minValues.x > allMeshes[i].vertices[v].position.x)
                        minValues.x = allMeshes[i].vertices[v].position.x;

                    if (maxValues.y < allMeshes[i].vertices[v].position.y)
                        maxValues.y = allMeshes[i].vertices[v].position.y;

                    if (minValues.y > allMeshes[i].vertices[v].position.y)
                        minValues.y = allMeshes[i].vertices[v].position.y;

                    if (maxValues.z < allMeshes[i].vertices[v].position.z)
                        maxValues.z = allMeshes[i].vertices[v].position.z;

                    if (minValues.z > allMeshes[i].vertices[v].position.z)
                        minValues.z = allMeshes[i].vertices[v].position.z;
                }
            }

            glm::vec3 normalizedScale = glm::vec3{ std::max(maxValues.x - minValues.x,std::max(maxValues.y - minValues.y,maxValues.z - minValues.z)) };
            //normalizedScale.x *= -1.0f; // flip horizontally

            for (size_t i{}; i < allMeshes.size(); ++i)
            {
                for (size_t v{}; v < allMeshes[i].vertices.size(); ++v)
                {
                    allMeshes[i].vertices[v].position /= normalizedScale;
                }
            }
        }

        if (scene->HasAnimations())
        {
            createAnimations(scene, filePath);
        }
    }

    BeModel::~BeModel() {
    }

    BeModel::BeModel(BeModel&& _rhs) noexcept
    {
        //this is used
        m_BoneInfoMap = std::move(_rhs.m_BoneInfoMap);
        allMeshes = std::move(_rhs.allMeshes);
        animationDetails = std::move(_rhs.animationDetails);
    }

    //Construct the data for non-templated load asset
    void BeModel::Construct(const ModelFileInitialiser& _data)
    {
        if (_data.filePath.find(".fbx") != std::string::npos)
        {
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(_data.filePath.c_str(),
                aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights
                | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality
            );

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {
                //LogCoreError("Error loading model using assimp from %s", filePath.c_str());
                return;
            }

            if (scene->HasAnimations())
            {
                aiNode* rootBone = findRootBone(const_cast<aiScene*>(scene));

                if (rootBone)
                {
                    //recursively add
                    loadBones(rootBone, -1);
                }
            }

            //Extract vertex weight and stuff by checking if the bone it has is already detected
            processNode(scene->mRootNode, scene);

            if (!scene->HasAnimations())
            {
                //Normalize the vertices first
                glm::vec3 maxValues{ -FLT_MAX,-FLT_MAX,-FLT_MAX };
                glm::vec3 minValues{ FLT_MAX ,FLT_MAX ,FLT_MAX };

                for (size_t i{}; i < allMeshes.size(); ++i)
                {
                    for (size_t v{}; v < allMeshes[i].vertices.size(); ++v)
                    {
                        if (maxValues.x < allMeshes[i].vertices[v].position.x)
                            maxValues.x = allMeshes[i].vertices[v].position.x;

                        if (minValues.x > allMeshes[i].vertices[v].position.x)
                            minValues.x = allMeshes[i].vertices[v].position.x;

                        if (maxValues.y < allMeshes[i].vertices[v].position.y)
                            maxValues.y = allMeshes[i].vertices[v].position.y;

                        if (minValues.y > allMeshes[i].vertices[v].position.y)
                            minValues.y = allMeshes[i].vertices[v].position.y;

                        if (maxValues.z < allMeshes[i].vertices[v].position.z)
                            maxValues.z = allMeshes[i].vertices[v].position.z;

                        if (minValues.z > allMeshes[i].vertices[v].position.z)
                            minValues.z = allMeshes[i].vertices[v].position.z;
                    }
                }

                glm::vec3 normalizedScale = glm::vec3{ std::max(maxValues.x - minValues.x,std::max(maxValues.y - minValues.y,maxValues.z - minValues.z)) };
                //normalizedScale.x *= -1.0f; // flip horizontally

                for (size_t i{}; i < allMeshes.size(); ++i)
                {
                    for (size_t v{}; v < allMeshes[i].vertices.size(); ++v)
                    {
                        allMeshes[i].vertices[v].position /= normalizedScale;
                    }
                }
            }

            for (size_t i{}; i < allMeshes.size(); ++i)
            {
                createVertexBuffers(allMeshes[i]);
                createIndexBuffers(allMeshes[i]);
            }

            if (scene->HasAnimations())
            {
                createAnimations(scene, _data.filePath);
            }
        }
        else if (_data.filePath.find(".bananaModel") != std::string::npos)
        {
            //@donavern
            std::ifstream bananaModel;
            bananaModel.open(_data.filePath, std::ifstream::in | std::ifstream::binary);
            read(bananaModel);
            bananaModel.close();

            for (size_t i{}; i < allMeshes.size(); ++i)
            {
                createVertexBuffers(allMeshes[i]);
                createIndexBuffers(allMeshes[i]);
            }
        }
    }

    void BeModel::ConstructFromPtr(ModelFileInitialiser* _data)
    {
        Construct(*_data);
    }

    BeModel& BeModel::operator=(BeModel&& _rhs) noexcept
    {
        m_BoneInfoMap = _rhs.m_BoneInfoMap;
        allMeshes = std::move(_rhs.allMeshes);
        animationDetails = _rhs.animationDetails;
        return *this;
    }

    void BeModel::createVertexBuffers(Mesh& m) {
        m.vertexCount = static_cast<uint32_t>(m.vertices.size());
        assert(m.vertexCount >= 3 && "Vertex count must be at least 3");
        VkDeviceSize bufferSize = sizeof(m.vertices[0]) * m.vertexCount;
        uint32_t vertexSize = sizeof(m.vertices[0]);

        BeBuffer stagingBuffer{
            BE::RendererSystem::GetInstance()->getDevice(),
            vertexSize,
            m.vertexCount,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        };

        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void*)m.vertices.data());

        m.vertexBuffer = std::make_unique<BeBuffer>(
            BE::RendererSystem::GetInstance()->getDevice(),
            vertexSize,
            m.vertexCount,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        BE::RendererSystem::GetInstance()->getDevice()->copyBuffer(stagingBuffer.getBuffer(), m.vertexBuffer->getBuffer(), bufferSize);
    }

    void BeModel::createIndexBuffers(Mesh& m) {
        m.indexCount = static_cast<uint32_t>(m.indices.size());
        m.hasIndexBuffer = m.indexCount > 0;

        if (!m.hasIndexBuffer) {
            return;
        }

        VkDeviceSize bufferSize = sizeof(m.indices[0]) * m.indexCount;
        uint32_t indexSize = sizeof(m.indices[0]);

        BeBuffer stagingBuffer{
            BE::RendererSystem::GetInstance()->getDevice(),
            indexSize,
            m.indexCount,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        };

        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void*)m.indices.data());

        m.indexBuffer = std::make_unique<BeBuffer>(
            BE::RendererSystem::GetInstance()->getDevice(),
            indexSize,
            m.indexCount,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        BE::RendererSystem::GetInstance()->getDevice()->copyBuffer(stagingBuffer.getBuffer(), m.indexBuffer->getBuffer(), bufferSize);
    }

    void BeModel::processMesh(aiMesh* mesh, const aiScene* scene)
    {
        Mesh newMesh;
        newMesh.meshName = mesh->mName.C_Str();

        Vertex vertex{};

        for (unsigned int i{}; i < mesh->mNumVertices; ++i)
        {
            vertex = Vertex();
            glm::vec3 vector{};

            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.position = vector;

            if (mesh->mColors[0])
            {
                glm::vec3 vec{};
                vec.x = mesh->mColors[0][i].r;
                vec.y = mesh->mColors[0][i].g;
                vec.z = mesh->mColors[0][i].b;
                vertex.color = vec;
            }
            else
            {
                vertex.color = glm::vec3(0.25f, 0.25f, 0.25f);
            }

            if (mesh->mTextureCoords[0])
            {
                glm::vec2 vec{};
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.uv = vec;
            }
            else
            {
                vertex.uv = glm::vec2(0.0f, 0.0f);
            }

            if (mesh->mNormals)
            {
                glm::vec3 vec{};
                vec.x = mesh->mNormals[i].x;
                vec.y = mesh->mNormals[i].y;
                vec.z = mesh->mNormals[i].z;
                vertex.normal = vec;
            }
            else
            {
                vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);
            }

            if (mesh->mTangents)
            {
                glm::vec3 vec{};
                vec.x = mesh->mTangents[i].x;
                vec.y = mesh->mTangents[i].y;
                vec.z = mesh->mTangents[i].z;
                vertex.tangents = vec;

                vec.x = mesh->mBitangents[i].x;
                vec.y = mesh->mBitangents[i].y;
                vec.z = mesh->mBitangents[i].z;
                vertex.bitangents = vec;
            }
            else
            {
                vertex.tangents = glm::vec3(0.0f, 0.0f, 0.0f);
                vertex.bitangents = glm::vec3(0.0f, 0.0f, 0.0f);
            }

            newMesh.vertices.emplace_back(vertex);
        }

        for (unsigned int i{}; i < mesh->mNumFaces; ++i)
        {
            aiFace const& face = mesh->mFaces[i];

            for (unsigned int j{}; j < face.mNumIndices; ++j)
            {
                newMesh.indices.emplace_back(face.mIndices[j]);
            }
        }

        //Bones
        for (unsigned int idxBone{}; idxBone < mesh->mNumBones; ++idxBone)
        {
            aiBone* bone = mesh->mBones[idxBone];
            int idxOfBoneFound{};
            bool foundBone{};

            for (int existingBone{}; existingBone < m_BoneInfoMap.size(); ++existingBone)
            {
                if (std::strcmp(m_BoneInfoMap[existingBone].nodeName.c_str(), bone->mName.C_Str()) == 0)
                {
                    foundBone = true;
                    idxOfBoneFound = existingBone;
                    break;
                }
            }

            if (foundBone)
            {
                m_BoneInfoMap[idxOfBoneFound].offset = ConvertMatrixToGLMFormat(bone->mOffsetMatrix);

                for (unsigned int idxWeight{}; idxWeight < bone->mNumWeights; ++idxWeight)
                {
                    int vertexIdx = bone->mWeights[idxWeight].mVertexId;
                    float weight = bone->mWeights[idxWeight].mWeight;
                    SetVertexBoneData(newMesh.vertices[vertexIdx], idxOfBoneFound, weight);
                }
            }
        }
        balanceOutVertexBoneData(newMesh.vertices);
        allMeshes.emplace_back(std::move(newMesh));
    }

    void BeModel::processNode(aiNode* node, const aiScene* scene)
    {
        for (unsigned int i{}; i < node->mNumMeshes; ++i)
        {
            processMesh(scene->mMeshes[i], scene);
        }
        for (unsigned int i{}; i < node->mNumChildren; ++i)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    void BeModel::createAnimations(const aiScene* scene, const std::string& filePath)
    {
        animationDetails.reserve(scene->mNumAnimations);
        for (unsigned int i{}; i < scene->mNumAnimations; ++i)
        {
            animationDetails.emplace_back(std::move(Animation(scene->mAnimations[i]->mName.data, filePath, i, m_BoneInfoMap)));
        }
    }

    void BeModel::SetVertexBoneData(Vertex& vertex, int boneID, float weight)
    {
        for (int i{}; i < 4; ++i)
        {
            if (vertex.bone_idxs[i] < 0)
            {
                vertex.bone_idxs[i] = boneID;
                vertex.bone_weights[i] = weight;
                break;
            }
        }
    }

    void BeModel::balanceOutVertexBoneData(std::vector<Vertex>& vertex)
    {
        for (int v{}; v < vertex.size(); ++v)
        {
            float total{};

            for (int i{}; i < 4; ++i)
            {
                if (vertex[v].bone_idxs[i] > 0)
                {
                    total += vertex[v].bone_weights[i];
                }
            }

            for (int i{}; i < 4; ++i)
            {
                if (vertex[v].bone_idxs[i] > 0)
                {
                    vertex[v].bone_weights[i] /= total;
                }
            }
        }
    }

    void BeModel::write(std::ofstream& stream)
    {
        size_t size = m_BoneInfoMap.size();
        stream.write((char*)&size, sizeof(size));

        for (size_t counter{}; counter < size; ++counter)
        {
            m_BoneInfoMap[counter].write(stream);
        }

        size = allMeshes.size();
        stream.write((char*)&size, sizeof(size));
        for (size_t counter{}; counter < size; ++counter)
        {
            allMeshes[counter].write(stream);
        }

        size = animationDetails.size();
        stream.write((char*)&size, sizeof(size));
        for (size_t counter{}; counter < size; ++counter)
        {
            animationDetails[counter].write(stream);
        }
    }

    void BeModel::read(std::ifstream& stream)
    {
        size_t size{};
        stream.read((char*)&size, sizeof(size));
        for (size_t counter{}; counter < size; ++counter)
        {
            m_BoneInfoMap.emplace_back(BoneInfo());
            m_BoneInfoMap[counter].read(stream);
        }

        stream.read((char*)&size, sizeof(size)); //size of allmeshes
        for (size_t counter{}; counter < size; ++counter)
        {
            allMeshes.emplace_back(Mesh());
            allMeshes[counter].read(stream);
        }

        stream.read((char*)&size, sizeof(size));
        for (size_t counter{}; counter < size; ++counter)
        {
            animationDetails.emplace_back(Animation());
            animationDetails[counter].read(stream);
        }
    }

    bool BeModel::isBoneOnlyNode(aiNode* node)
    {
        if (node->mNumMeshes > 0)
        {
            return false;
        }
        bool found = true;

        for (unsigned int i{}; i < node->mNumChildren; ++i)
        {
            if (!isBoneOnlyNode(node->mChildren[i]))
            {
                found = false;
            }
        }
        return found;
    }

    aiNode* BeModel::findRootBone(aiScene* scene)
    {
        aiNode* rootNode = scene->mRootNode;

        for (unsigned int i{}; i < rootNode->mNumChildren; ++i)
        {
            aiNode* firstLevelNode = rootNode->mChildren[i];
            if (isBoneOnlyNode(firstLevelNode))
            {
                return firstLevelNode;
            }
        }

        return nullptr;
    }

    void BeModel::loadBones(aiNode* node, int index)
    {
        const std::string temp = node->mName.C_Str();
        if (temp.find("AssimpFbx") != std::string::npos || temp.find("Armature") != std::string::npos || temp.find("RootNode") != std::string::npos)
        {
            for (unsigned int i{}; i < node->mNumChildren; ++i)
            {
                loadBones(node->mChildren[i], -1);
            }
        }
        else
        {
            //Create new bone to store info
            BoneInfo newInfo;
            newInfo.idOfBone = static_cast<int>(m_BoneInfoMap.size());
            newInfo.nodeName = node->mName.C_Str();
            newInfo.nodeTransformation = ConvertMatrixToGLMFormat(node->mTransformation);
            newInfo.idOfParentBone = index;
            newInfo.offset = glm::mat4(1.0f);

            m_BoneInfoMap.emplace_back(newInfo);

            for (unsigned int i{}; i < node->mNumChildren; ++i)
            {
                loadBones(node->mChildren[i], newInfo.idOfBone);
            }
        }
    }

    void BeModel::draw(VkCommandBuffer commandBuffer, int index) {
        if (allMeshes[index].hasIndexBuffer) {
            vkCmdDrawIndexed(commandBuffer, allMeshes[index].indexCount, 1, 0, 0, 0);
        }
        else {
            vkCmdDraw(commandBuffer, allMeshes[index].vertexCount, 1, 0, 0);
        }
    }

    std::vector<BoneInfo>& BeModel::getBoneInfoMap()
    {
        return m_BoneInfoMap;
    }

    std::vector<Mesh>& BeModel::getMeshes()
    {
        return allMeshes;
    }

    Animation& BeModel::GetSpecificAnimation(unsigned id)
    {
        AssertCore(animationDetails.size() > id, "Animation with this index not available");
        return animationDetails[id];
    }

    const std::vector<Animation>& BeModel::GetAnimationDetails()
    {
        return animationDetails;
    }

    void BeModel::bind(VkCommandBuffer commandBuffer, int index) {
        VkBuffer buffers[] = { allMeshes[index].vertexBuffer->getBuffer() };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

        if (allMeshes[index].hasIndexBuffer) {
            vkCmdBindIndexBuffer(commandBuffer, allMeshes[index].indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
        }
    }

    std::vector<VkVertexInputBindingDescription> Vertex::getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> Vertex::getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
        attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
        attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
        attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });
        attributeDescriptions.push_back({ 4, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, tangents) });
        attributeDescriptions.push_back({ 5, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, bitangents) });
        attributeDescriptions.push_back({ 6, 0, VK_FORMAT_R32G32B32A32_SINT, offsetof(Vertex, bone_idxs) });
        attributeDescriptions.push_back({ 7, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, bone_weights) });

        return attributeDescriptions;
    }
    ModelFileInitialiser::ModelFileInitialiser(const std::string& path)
        :filePath{ path }
    {
    }

    void Mesh::write(std::ofstream& stream)
    {
        size_t stringSize = meshName.size();
        stream.write((char*)&stringSize, sizeof(stringSize));
        stream.write((char*)&meshName[0], stringSize);
        vertexCount = static_cast<uint32_t>(vertices.size());
        stream.write((char*)&vertexCount, sizeof(vertexCount));

        for (size_t vcounter{}; vcounter < vertices.size(); ++vcounter)
        {
            stream.write((char*)&vertices[vcounter], sizeof(vertices[vcounter]));
        }

        stream.write((char*)&hasIndexBuffer, sizeof(hasIndexBuffer));
        indexCount = static_cast<uint32_t>(indices.size());
        stream.write((char*)&indexCount, sizeof(indexCount));
        for (size_t vcounter{}; vcounter < indices.size(); ++vcounter)
        {
            stream.write((char*)&indices[vcounter], sizeof(indices[vcounter]));
        }
    }
    void Mesh::read(std::ifstream& stream)
    {
        size_t stringSize{};
        stream.read((char*)&stringSize, sizeof(stringSize));
        meshName.resize(stringSize);
        stream.read((char*)&meshName[0], stringSize);
        stream.read((char*)&vertexCount, sizeof(vertexCount));

        for (size_t vcounter{}; vcounter < vertexCount; ++vcounter)
        {
            vertices.emplace_back(Vertex());
            stream.read((char*)&vertices[vcounter], sizeof(vertices[vcounter]));
        }

        stream.read((char*)&hasIndexBuffer, sizeof(hasIndexBuffer));
        stream.read((char*)&indexCount, sizeof(indexCount));
        for (size_t vcounter{}; vcounter < indexCount; ++vcounter)
        {
            indices.emplace_back(uint32_t());
            stream.read((char*)&indices[vcounter], sizeof(indices[vcounter]));
        }
    }
    std::vector<VkVertexInputBindingDescription> ParticleVertex::getBindingDescriptions()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(ParticleVertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> ParticleVertex::getAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(ParticleVertex, m_Position);
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(ParticleVertex, m_UV);
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].format = VK_FORMAT_R8G8B8A8_UNORM;
        attributeDescriptions[2].offset = offsetof(ParticleVertex, m_Color);
        return attributeDescriptions;
    }

}  // namespace BE
#pragma warning ( pop )
