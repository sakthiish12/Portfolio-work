#pragma once
#ifndef ANIMATION_H
#define ANIMATION_H

#include "IAsset.h"
#include <Systems/Graphics/Be_bone.h>


namespace BE 
{
	

	struct BoneInfo
	{
		std::string nodeName{};
		int idOfBone{};
		int idOfParentBone{};

		/*offset matrix transforms vertex from model space to bone space*/
		glm::mat4 offset{};
		glm::mat4 nodeTransformation{};

		void write(std::ofstream& stream);
		void read(std::ifstream& stream);
	};

	struct AnimationFileInitialiser
	{
		std::string animationName;
		std::string animationPath;
		int index;
		std::vector<BoneInfo> bones;
		AnimationFileInitialiser() = default;
		AnimationFileInitialiser(const AnimationFileInitialiser& in) = default;
		AnimationFileInitialiser(const std::string& name, const std::string& path, int idx, std::vector<BoneInfo>& bones);
		RTTR_ENABLE()	
	};

	class Animation : public IAsset
	{
	public:
		Animation() = default;
		Animation(const std::string& animationName, const std::string& animationPath, int index,std::vector<BoneInfo>& bones);
		BeBone* FindBone(const int id);

		int GetTicksPerSecond() const;
		float GetDuration() const;
		const std::vector<BoneInfo>& GetBoneIDMap() const;
		inline const std::vector<BeBone>& getBones() const;
		glm::mat4 const& getGlobalInverseTransform();
		std::string const& GetCurrentAnimationName() const;

		//read-write
		void write(std::ofstream& stream);
		void read(std::ifstream& stream);

		//Construct the data for non-templated load asset
		void Construct(const AnimationFileInitialiser& _data);
		void ConstructFromPointer(AnimationFileInitialiser* _data);
		AnimationFileInitialiser Serialise();
		RTTR_ENABLE(IAsset)
	private:
		//v2
		inline void calculateBoneMatrices(aiAnimation* anim, int index);
		inline void makeBoneMatricesForAnimationBone(aiNodeAnim* aiChannel, int idxAnimation, int idxBone);


		float m_Duration{ 0 };
		int m_TicksPerSecond{ 0 };
		std::vector<BeBone> m_Bones;
		std::vector<BoneInfo> m_BoneInfoMap;
		glm::mat4 globalInverseTransform{1.0f};
		std::string currentAnimationName{};
	};
}
#endif