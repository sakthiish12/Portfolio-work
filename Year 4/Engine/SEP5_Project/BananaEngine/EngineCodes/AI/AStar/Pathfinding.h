#pragma once
#ifndef PATHFINDING_H
#define PATHFINDING_H
#include <Maths/Vector3D.h>
#include "../../Precompiled/pch.h"
namespace BE
{
    using valueType = float;
    using WaypointList = std::list<Vec3<valueType>>;

    enum class Heuristic
    {
        OCTILE,
        CHEBYSHEV,
        MANHATTAN,
        EUCLIDEAN,

        NUM_ENTRIES
    };

    static const std::string heuristicText[] =
    { "Octile", "Chebyshev", "Manhattan", "Euclidean", "Invalid" };

    inline const std::string& get_heuristic_text(Heuristic heuristic)
    {
        return heuristicText[static_cast<size_t>(heuristic)];
    }

    inline Heuristic string_to_heuristic(const std::string& string)
    {
        for (size_t i = 0; i < static_cast<size_t>(Heuristic::NUM_ENTRIES); ++i)
        {
            if (heuristicText[i] == string)
            {
                return static_cast<Heuristic>(i);
            }
        }
        return Heuristic::NUM_ENTRIES;
    }

    struct PFSettings
    {
        Heuristic heuristic = Heuristic::MANHATTAN;
        float weight = 0.f;
        bool smoothing = false;
        bool rubberBanding = false;
        bool singleStep = false;
    };

    struct PathRequest
    {
        Vec3<valueType> start;
        Vec3<valueType> goal;
        WaypointList path;
        PFSettings settings;
        bool newRequest = true;
    };

    struct GridPathRequest
    {
        GridPos start;
        GridPos goal;
        WaypointList path;
        PFSettings settings;
        bool newRequest = true;
    };

    enum class PathResult
    {
        PROCESSING,
        COMPLETE,
        IMPOSSIBLE
    };

};

#endif