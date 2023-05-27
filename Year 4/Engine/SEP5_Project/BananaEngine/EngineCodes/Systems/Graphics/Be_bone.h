#pragma once

#include <Precompiled/pch.h>
#include <assimp/scene.h>
#include <list>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace BE
{
	class BeBone
	{
	public:
		BeBone();
		BeBone(const std::string& name, int ID, const aiNodeAnim* channel);
		void Update(float animationTime);

		glm::mat4& GetLocalTransform();
		std::string& GetBoneName();
		int GetBoneID();
		int GetPositionIndex(float animationTime);
		int GetRotationIndex(float animationTime);
		int GetScaleIndex(float animationTime);
		int GetNumKeys();
		void SetChannel(const aiNodeAnim* channel);
		void write(std::ofstream&);
		void read(std::ifstream&);
	private:

		struct KeyPosition
		{
			glm::vec3 position;
			float timeStamp;
		};

		struct KeyRotation
		{
			glm::quat orientation;
			float timeStamp;
		};

		struct KeyScale
		{
			glm::vec3 scale;
			float timeStamp;
		};

		float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
		void InterpolatePosition(float animationTime);
		void InterpolateRotation(float animationTime);
		void InterpolateScaling(float animationTime);

		int m_NumPositions{};
		int m_NumRotations{};
		int m_NumScalings{};
		std::vector<KeyPosition> m_Positions;
		std::vector<KeyRotation> m_Rotations;
		std::vector<KeyScale> m_Scales;

		glm::mat4 m_LocalTransform{};
		glm::mat4 interpolatedPos{}, interpolatedRot{}, interpolatedScale{};
		std::string m_Name{};
		int m_ID{};

		int prevPosIndex{};
		int prevRotIndex{};
		int prevScaleIndex{};
	};
}