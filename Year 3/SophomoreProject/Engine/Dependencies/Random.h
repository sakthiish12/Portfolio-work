// /******************************************************************************/
// /*!
// \file         Random.h
// \project name Engine
// \author(s)    (100%) s.sakthiish@digipen.edu
// \date         09th Jan, 2022
// \brief        Random class that generates randomized objects and values.
// 
// All content © 2022 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2022 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <random>


using UIntRange = std::pair<unsigned, unsigned>;
using IntRange = std::pair<int, int>;
using FloatRange = std::pair<float, float>;
using Vector2Range = std::pair<glm::vec2, glm::vec2>;
using Vector3Range = std::pair<glm::vec3, glm::vec3>;


namespace Random {

    // Returns a random number from a range
    // * Min & Max values are both inclusive
    // * Implementation uses uniform_real_distribution
    template<typename T>
    T Range(T min, T max) {
        thread_local static std::mt19937 gen(std::random_device{}());

        using dist_type = typename std::conditional
            <std::is_integral<T>::value,
            std::uniform_int_distribution<T>,
            std::uniform_real_distribution<T>>::type;

        thread_local static dist_type dist;

        return dist(gen, typename dist_type::param_type{ min, max });
    }

    // Returns a random number from a range of pair<T, T>
    // * Min & Max values are both inclusive
    // * Implementation uses uniform_real_distribution
    template <typename T>
    T Range(const std::pair<T, T>& rangePair) {
        return Range(rangePair.first, rangePair.second);
    }

    // Returns a random number from a range of pair<Vector2, Vector2>
    // * Min & Max values are both inclusive
    // * Implementation uses uniform_real_distribution
    template <>
    inline glm::vec2 Range(const Vector2Range& rangePair) {
        const float x = Range(rangePair.first.x, rangePair.second.x);
        const float y = Range(rangePair.first.y, rangePair.second.y);
        return glm::vec2(x, y);
    }

    // Returns a random number from a range of pair<Vector3, Vector3>
    // * Min & Max values are both inclusive
    // * Implementation uses uniform_real_distribution
    template <>
    inline glm::vec3 Range(const Vector3Range& rangePair) {
        const float x = Range(rangePair.first.x, rangePair.second.x);
        const float y = Range(rangePair.first.y, rangePair.second.y);
        const float z = Range(rangePair.first.z, rangePair.second.z);
        return glm::vec3(x, y, z);
    }

};