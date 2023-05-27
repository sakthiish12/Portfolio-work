#pragma once
#include "pch.h"
#include "Be_bone.h"

namespace BE
{
	BeBone::BeBone()
	{
	}
	BeBone::BeBone(const std::string& name, int ID, const aiNodeAnim* channel)
		: m_Name(name), m_ID(ID), m_LocalTransform(1.0f)
	{
		if (channel == nullptr)
			return;

		m_NumPositions = channel->mNumPositionKeys;

		aiVector3D aiPosition;
		KeyPosition posData;

		for (int positionIndex{}; positionIndex < m_NumPositions; ++positionIndex)
		{
			aiPosition = channel->mPositionKeys[positionIndex].mValue;
			posData.position = glm::vec3(aiPosition.x, aiPosition.y, aiPosition.z);
			posData.timeStamp = static_cast<float>(channel->mPositionKeys[positionIndex].mTime);
			m_Positions.emplace_back(posData);
		}

		m_NumRotations = channel->mNumRotationKeys;

		aiQuaternion aiOrientation;
		KeyRotation rotData;

		for (int rotationIndex{}; rotationIndex < m_NumRotations; ++rotationIndex)
		{
			aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
			rotData.orientation = glm::quat(aiOrientation.w, aiOrientation.x, aiOrientation.y, aiOrientation.z);
			rotData.timeStamp = static_cast<float>(channel->mRotationKeys[rotationIndex].mTime);
			m_Rotations.emplace_back(rotData);
		}

		m_NumScalings = channel->mNumScalingKeys;

		aiVector3D scale;
		KeyScale scaleData;

		for (int keyIndex{}; keyIndex < m_NumScalings; ++keyIndex)
		{
			scale = channel->mScalingKeys[keyIndex].mValue;
			scaleData.scale = glm::vec3(scale.x, scale.y, scale.z);
			scaleData.timeStamp = static_cast<float>(channel->mScalingKeys[keyIndex].mTime);
			m_Scales.emplace_back(scaleData);
		}
	}

	void BeBone::Update(float animationTime)
	{
		InterpolatePosition(animationTime);
		InterpolateRotation(animationTime);
		InterpolateScaling(animationTime);
		m_LocalTransform = interpolatedPos * interpolatedRot * interpolatedScale;
	}

	glm::mat4& BeBone::GetLocalTransform()
	{
		return m_LocalTransform;
	}

	std::string& BeBone::GetBoneName()
	{
		return m_Name;
	}

	int BeBone::GetBoneID()
	{
		return m_ID;
	}

	int BeBone::GetPositionIndex(float animationTime)
	{
		int idx = prevPosIndex;

		do {
			if (animationTime > m_Positions[idx].timeStamp &&
				animationTime < m_Positions[idx + static_cast<size_t>( 1)].timeStamp)
			{
				prevPosIndex = idx;
				return idx;
			}

			++idx;
			if (idx == static_cast<int>(m_Positions.size()) - 1)
			{
				idx = 0;
			}
		} while (idx != prevPosIndex);

		return 0;
	}

	int BeBone::GetRotationIndex(float animationTime)
	{
		int idx = prevRotIndex;

		do {
			if (animationTime > m_Rotations[idx].timeStamp &&
				animationTime < m_Rotations[idx + static_cast<size_t>( 1)].timeStamp)
			{
				prevRotIndex = idx;
				return idx;
			}

			++idx;
			if (idx == static_cast<int>(m_Rotations.size()) - 1)
			{
				idx = 0;
			}
		} while (idx != prevRotIndex);

		return 0;
	}

	int BeBone::GetScaleIndex(float animationTime)
	{
		int idx = prevScaleIndex;

		do {
			if (animationTime > m_Scales[idx].timeStamp &&
				animationTime < m_Scales[idx + static_cast<size_t>( 1)].timeStamp)
			{
				prevScaleIndex = idx;
				return idx;
			}

			++idx;
			if (idx == static_cast<int>(m_Scales.size()) - 1)
			{
				idx = 0;
			}
		} while (idx != prevScaleIndex);

		return 0;
	}

	int BeBone::GetNumKeys()
	{
		return m_NumPositions;
	}

	void BeBone::SetChannel(const aiNodeAnim* channel)
	{
		m_NumPositions = channel->mNumPositionKeys;

		aiVector3D aiPosition;
		KeyPosition posData;

		for (int positionIndex{}; positionIndex < m_NumPositions; ++positionIndex)
		{
			aiPosition = channel->mPositionKeys[positionIndex].mValue;
			posData.position = glm::vec3(aiPosition.x, aiPosition.y, aiPosition.z);
			posData.timeStamp = static_cast<float>(channel->mPositionKeys[positionIndex].mTime);
			m_Positions.emplace_back(posData);
		}

		m_NumRotations = channel->mNumRotationKeys;

		aiQuaternion aiOrientation;
		KeyRotation rotData;

		for (int rotationIndex{}; rotationIndex < m_NumRotations; ++rotationIndex)
		{
			aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
			rotData.orientation = glm::quat(aiOrientation.w, aiOrientation.x, aiOrientation.y, aiOrientation.z);
			rotData.timeStamp = static_cast<float>(channel->mRotationKeys[rotationIndex].mTime);
			m_Rotations.emplace_back(rotData);
		}

		m_NumScalings = channel->mNumScalingKeys;

		aiVector3D scale;
		KeyScale scaleData;

		for (int keyIndex{}; keyIndex < m_NumScalings; ++keyIndex)
		{
			scale = channel->mScalingKeys[keyIndex].mValue;
			scaleData.scale = glm::vec3(scale.x, scale.y, scale.z);
			scaleData.timeStamp = static_cast<float>(channel->mScalingKeys[keyIndex].mTime);
			m_Scales.emplace_back(scaleData);
		}
	}

	void BeBone::write(std::ofstream& stream)
	{
		stream.write((char*)&m_NumPositions, sizeof(m_NumPositions));
		for (size_t counter{}; counter < m_Positions.size(); ++counter)
		{
			stream.write((char*)&m_Positions[counter], sizeof(m_Positions[counter]));
		}

		stream.write((char*)&m_NumRotations, sizeof(m_NumRotations));
		for (size_t counter{}; counter < m_Rotations.size(); ++counter)
		{
			stream.write((char*)&m_Rotations[counter], sizeof(m_Rotations[counter]));
		}	

		stream.write((char*)&m_NumScalings, sizeof(m_NumScalings));
		for (size_t counter{}; counter < m_Scales.size(); ++counter)
		{
			stream.write((char*)&m_Scales[counter], sizeof(m_Scales[counter]));
		}

		stream.write((char*)&m_LocalTransform, sizeof(m_LocalTransform));
		stream.write((char*)&interpolatedPos, sizeof(interpolatedPos));
		stream.write((char*)&interpolatedRot, sizeof(interpolatedRot));
		stream.write((char*)&interpolatedScale, sizeof(interpolatedScale));
		size_t size = m_Name.size();
		stream.write((char*)&size, sizeof(size));
		stream.write((char*)&m_Name[0], size);
		stream.write((char*)&m_ID, sizeof(m_ID));
		stream.write((char*)&prevPosIndex, sizeof(prevPosIndex));
		stream.write((char*)&prevRotIndex, sizeof(prevRotIndex));
		stream.write((char*)&prevScaleIndex, sizeof(prevScaleIndex));
	}

	void BeBone::read(std::ifstream& stream)
	{
		stream.read((char*)&m_NumPositions, sizeof(m_NumPositions));
		for (size_t counter{}; counter < m_NumPositions; ++counter)
		{
			m_Positions.emplace_back(KeyPosition());
			stream.read((char*)&m_Positions[counter], sizeof(m_Positions[counter]));
		}

		stream.read((char*)&m_NumRotations, sizeof(m_NumRotations));
		for (size_t counter{}; counter < m_NumRotations; ++counter)
		{
			m_Rotations.emplace_back(KeyRotation());
			stream.read((char*)&m_Rotations[counter], sizeof(m_Rotations[counter]));
		}

		stream.read((char*)&m_NumScalings, sizeof(m_NumScalings));
		for (size_t counter{}; counter < m_NumScalings; ++counter)
		{
			m_Scales.emplace_back(KeyScale());
			stream.read((char*)&m_Scales[counter], sizeof(m_Scales[counter]));
		}

		stream.read((char*)&m_LocalTransform, sizeof(m_LocalTransform));
		stream.read((char*)&interpolatedPos, sizeof(interpolatedPos));
		stream.read((char*)&interpolatedRot, sizeof(interpolatedRot));
		stream.read((char*)&interpolatedScale, sizeof(interpolatedScale));
		size_t size{};
		stream.read((char*)&size, sizeof(size));
		m_Name.resize(size);
		stream.read((char*)&m_Name[0], size);
		stream.read((char*)&m_ID, sizeof(m_ID));
		stream.read((char*)&prevPosIndex, sizeof(prevPosIndex));
		stream.read((char*)&prevRotIndex, sizeof(prevRotIndex));
		stream.read((char*)&prevScaleIndex, sizeof(prevScaleIndex));
	}

	inline float BeBone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
	{
		return (animationTime - lastTimeStamp) / (nextTimeStamp - lastTimeStamp);
	}

	inline void BeBone::InterpolatePosition(float animationTime)
	{
		if (1 == m_NumPositions)
		{
			interpolatedPos = glm::translate(glm::mat4(1.0f), m_Positions[0].position);
			return;
		}

		int p0Index = GetPositionIndex(animationTime);

		interpolatedPos = glm::translate(glm::mat4(1.0f), glm::mix(m_Positions[p0Index].position, m_Positions[p0Index + static_cast<size_t>( 1)].position
			, GetScaleFactor(m_Positions[p0Index].timeStamp,
				m_Positions[p0Index + static_cast<size_t>( 1)].timeStamp, animationTime)));
	}

	inline void BeBone::InterpolateRotation(float animationTime)
	{
		if (1 == m_NumRotations)
		{
			interpolatedRot = glm::toMat4(glm::normalize(m_Rotations[0].orientation));
			return;
		}

		int p0Index = GetRotationIndex(animationTime);

		interpolatedRot = glm::toMat4(glm::normalize(glm::slerp(m_Rotations[p0Index].orientation, m_Rotations[p0Index + static_cast<size_t>( 1)].orientation
			, GetScaleFactor(m_Rotations[p0Index].timeStamp,
				m_Rotations[p0Index + static_cast<size_t>( 1)].timeStamp, animationTime))));
	}

	inline void BeBone::InterpolateScaling(float animationTime)
	{
		if (1 == m_NumScalings)
		{
			interpolatedScale = glm::scale(glm::mat4(1.0f), m_Scales[0].scale);
			return;
		}

		int p0Index = GetScaleIndex(animationTime);

		interpolatedScale = glm::scale(glm::mat4(1.0f), glm::mix(m_Scales[p0Index].scale, m_Scales[p0Index + static_cast<size_t>( 1)].scale
			, GetScaleFactor(m_Scales[p0Index].timeStamp,
				m_Scales[p0Index + static_cast<size_t>( 1)].timeStamp, animationTime)));
	}
}