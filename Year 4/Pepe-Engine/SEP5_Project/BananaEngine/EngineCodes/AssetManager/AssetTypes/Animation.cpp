#include "pch.h"
#include "Animation.h"
#include <Systems/RendererSystem/RendererSystem.h>
#include <Serialisation/BananaSerialisation.h>

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	registration::class_<Animation>(BE::RegistrationNameCreator<Animation>())
	.constructor()()
	.method(assetConstructMethodName, &Animation::ConstructFromPointer)()
	.method(serializeAssetMethodName,&Animation::Serialise)();
	registration::class_<AnimationFileInitialiser>(BE::RegistrationNameCreator<AnimationFileInitialiser>())
	.constructor()()
	.property(filePathName, &AnimationFileInitialiser::animationPath)
	.property("animationName", &AnimationFileInitialiser::animationName)
	.property("index", &AnimationFileInitialiser::index);
}


namespace BE 
{
	Animation::Animation(const std::string& animationName, const std::string& animationPath, int index, std::vector<BoneInfo>& bones)
	{
		AnimationFileInitialiser afi{ animationName, animationPath, index,bones };
		
		try
		{
			this->Construct(afi);
		}
		catch (const std::runtime_error&)
		{
			throw;
		}
	}

	void Animation::Construct(const AnimationFileInitialiser& _data)
	{
		//Disabled constructed animations until it works again
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(_data.animationPath,
			aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights
			| aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality);
		auto animation = scene->mAnimations[_data.index];
		m_Duration = static_cast<float>(animation->mDuration);
		m_TicksPerSecond = static_cast<int>(animation->mTicksPerSecond);
		currentAnimationName = _data.animationName;
		globalInverseTransform = glm::inverse(ConvertMatrixToGLMFormat(scene->mRootNode->mTransformation));
		m_BoneInfoMap = _data.bones;

		//Calculate bone matrices
		calculateBoneMatrices(animation,_data.index);
	}

	//Not serialising animation index, update in the future
	AnimationFileInitialiser Animation::Serialise()
	{
		AnimationFileInitialiser temp;
		temp.animationName = currentAnimationName;
		temp.index = 0;
		return temp;
	}

	void Animation::ConstructFromPointer(AnimationFileInitialiser* _data)
	{
		Construct(*_data);
	}

	BeBone* Animation::FindBone(const int id)
	{
		if (id >= m_Bones.size() || id < 0)
		{
			return nullptr;
		}
		return &m_Bones[id];
	}

	void Animation::calculateBoneMatrices(aiAnimation* anim,int index)
	{
		//calculate first
		for (int idxBone{}; idxBone < m_BoneInfoMap.size(); ++idxBone)
		{
			BoneInfo* bone = &m_BoneInfoMap[idxBone];
			int idxOfChannelFound{};
			bool foundChannel{};

			for (unsigned int idxChannel{}; idxChannel<anim->mNumChannels; ++idxChannel)
			{
				aiNodeAnim* aiChannel = anim->mChannels[idxChannel];
				if (std::strcmp(aiChannel->mNodeName.C_Str(), bone->nodeName.c_str()) == 0)
				{
					idxOfChannelFound = idxChannel;
					foundChannel = true;
					break;
				}
			}

			if (foundChannel)
			{
				//make bone matrices for animation bone
				makeBoneMatricesForAnimationBone(anim->mChannels[idxOfChannelFound], index, idxBone);
			}
		}

		//Finalize bone data
		//for (int idxBone{}; idxBone < m_Bones.size(); ++idxBone)
		//{
		//	BeBone* bone = &m_Bones[idxBone];

		//	for (int idxAnimKey{}; idxAnimKey < bone->GetNumKeys(); ++idxAnimKey)
		//	{

		//	}
		//}
	}

	void Animation::makeBoneMatricesForAnimationBone(aiNodeAnim* aiChannel, int idxAnimation, int idxBone)
	{
		m_Bones.emplace_back(BeBone(aiChannel->mNodeName.C_Str(),idxBone,aiChannel));
	}

	void Animation::write(std::ofstream& stream)
	{
		stream.write((char*)&m_Duration, sizeof(m_Duration));
		stream.write((char*)&m_TicksPerSecond, sizeof(m_TicksPerSecond));

		size_t size = m_Bones.size();
		stream.write((char*)&size, sizeof(size));
		for (size_t counter{}; counter < size; ++counter)
		{
			m_Bones[counter].write(stream);
		}

		size = m_BoneInfoMap.size();
		stream.write((char*)&size, sizeof(size));
		for (size_t counter{}; counter < size; ++counter)
		{
			m_BoneInfoMap[counter].write(stream);
		}

		stream.write((char*)&globalInverseTransform, sizeof(globalInverseTransform));
		size = currentAnimationName.size();
		stream.write((char*)&size, sizeof(size));
		stream.write((char*)&currentAnimationName[0], size);
	}

	void Animation::read(std::ifstream& stream)
	{
		stream.read((char*)&m_Duration, sizeof(m_Duration));
		stream.read((char*)&m_TicksPerSecond, sizeof(m_TicksPerSecond));

		size_t size{};
		stream.read((char*)&size, sizeof(size));
		for (size_t counter{}; counter < size; ++counter)
		{
			m_Bones.emplace_back(BeBone());
			m_Bones[counter].read(stream);
		}

		stream.read((char*)&size, sizeof(size));
		for (size_t counter{}; counter < size; ++counter)
		{
			m_BoneInfoMap.emplace_back(BoneInfo());
			m_BoneInfoMap[counter].read(stream);
		}

		stream.read((char*)&globalInverseTransform, sizeof(globalInverseTransform));
		size_t size2{};
		stream.read((char*)&size2, sizeof(size2));
		currentAnimationName.resize(size2);
		stream.read((char*)&currentAnimationName[0], size2);
	}

	int Animation::GetTicksPerSecond() const { return m_TicksPerSecond; }

	float Animation::GetDuration() const { return m_Duration; }

	const std::vector<BoneInfo>& Animation::GetBoneIDMap() const { return m_BoneInfoMap; }

	const std::vector<BeBone>& Animation::getBones() const
	{
		return m_Bones;
	}

	glm::mat4 const& Animation::getGlobalInverseTransform()
	{
		return globalInverseTransform;
	}

	std::string const& Animation::GetCurrentAnimationName() const
	{
		return currentAnimationName;
	}

	AnimationFileInitialiser::AnimationFileInitialiser(const std::string& name, const std::string& path, int idx, std::vector<BoneInfo>& newBones)
		:animationName{name},animationPath{path},index{idx},bones{newBones}
	{
	}
	void BoneInfo::write(std::ofstream& stream)
	{
		size_t stringSize = nodeName.size();
		stream.write((char*)&stringSize, sizeof(stringSize));
		stream.write((char*)&nodeName[0], stringSize);
		stream.write((char*)&idOfBone, sizeof(idOfBone));
		stream.write((char*)&idOfParentBone, sizeof(idOfParentBone));
		stream.write((char*)&offset, sizeof(offset));
		stream.write((char*)&nodeTransformation, sizeof(nodeTransformation));
	}
	void BoneInfo::read(std::ifstream& stream)
	{
		size_t stringSize{};
		stream.read((char*)&stringSize, sizeof(stringSize));
		nodeName.resize(stringSize);
		stream.read((char*)&nodeName[0], stringSize);
		stream.read((char*)&idOfBone, sizeof(idOfBone));
		stream.read((char*)&idOfParentBone, sizeof(idOfParentBone));
		stream.read((char*)&offset, sizeof(offset));
		stream.read((char*)&nodeTransformation, sizeof(nodeTransformation));
	}
}