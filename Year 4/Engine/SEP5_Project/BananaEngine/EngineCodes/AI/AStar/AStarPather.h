#pragma once
#ifndef ASTAR_PATHER_H
#define ASTAR_PATHER_H

#include "Grid.h"
#include "Pathfinding.h"
namespace BE
{
    const float diagonalCost = sqrt(2.0f);
    const float regularCost = 1.0f;
    constexpr sizeType startNodeParent = std::numeric_limits<sizeType>().max();
    constexpr GridPos startNodeParentPos = { startNodeParent, startNodeParent, startNodeParent };
    enum class ListState
    {
        Open,
        Close,
        Undiscovered
    };

    struct ListNode: public CellInfo
    {
        GridPos pos;
        GridPos parentpos;
        float startDistance = 0.f;
        float totalCost = 0.f;
        float heuristic = 0.f;
        ListState state = ListState::Undiscovered;
        ListNode() = default;
        ListNode(const ListNode&) = default;
        ListNode(GridPos _pos, GridPos _parentpos, float _start, float _h)
            :pos{ _pos }, parentpos{ _parentpos }, startDistance{ _start }, heuristic{ _h }, totalCost{ _start + _h }
        {
        }
        inline bool operator< (const ListNode& in) {
            return totalCost < in.totalCost;
        }
        inline bool operator> (const ListNode& in) {
            return totalCost > in.totalCost;
        }
        inline bool operator== (const ListNode& in) {
            return pos.layer == in.pos.layer && pos.row == in.pos.row && pos.col == in.pos.col && totalCost == in.totalCost;
        }
        inline operator GridPos() { return pos; }

       ListNode& operator=(bool in) { occupied = in;  return *this; }
    };
    inline bool operator<(const ListNode& lhs, const ListNode& rhs)
    {
        return lhs.totalCost < rhs.totalCost;
    }

    inline bool operator>(const ListNode& lhs, const ListNode& rhs)
    {
        return lhs.totalCost > rhs.totalCost;
    }

    inline bool operator==(const ListNode& lhs, const GridPos& rhs)
    {
        return lhs.pos == rhs;
    }

    template class Grid3D<ListNode>;
    using LinkID = uint8_t;
    constexpr LinkID NULL_LINKID = 0;

    struct GridLayerLink
    {
        GridPos cell_lower;
        GridPos cell_higher;
    };
    struct CalculatedPath
    {
        GridPathRequest request;
        LinkID id_lower;
        LinkID id_higher;
        sizeType cost;
        sizeType layer;
    };

    enum class LinkLevel
    {
        Lower,
        Higher
    };

	class AStarPather
	{
    private:
        Grid3D<ListNode> ClosedList;
        std::unordered_map<LinkID , GridLayerLink> layer_links;
        std::vector<CalculatedPath> layer_paths;

        std::priority_queue<ListNode, std::vector<ListNode>, std::greater<ListNode> > OpenList;
        std::list<LinkID> link_processing;

        GridPos start{}, goal{};
        GridPathRequest* requestPtr = nullptr;
        PathResult status = PathResult::PROCESSING;
        float worldToGridMultipliyerSqr = 0;
        LinkID total_ids = 0;

    public:
        /*
            The class should be default constructible, so you may need to define a constructor.
            If needed, you can modify the framework where the class is constructed in the
            initialize functions of ProjectTwo and ProjectThree.
        */
        AStarPather(GridSettings const& _settings, valueType first_height, std::set<TagID> const& tags)
            :ClosedList{ Grid3D<ListNode>(_settings, first_height, tags) }
        {
            worldToGridMultipliyerSqr = _settings.TileSize * _settings.TileSize;
        }
        bool initialize();
        void shutdown();
        PathResult compute_path(PathRequest& request);
        PathResult compute_path(GridPathRequest& request, bool internal = false);
        inline PathResult compute_paths_end_layers(GridPathRequest& main_request, GridPathRequest& start_layer_req, GridPathRequest& goal_layer_req, bool single = true);
        void UpdateGrid();
        void UpdateLayerPaths(PFSettings settings = PFSettings());
        inline void AddGridLayer(valueType height);
        inline void AddLayerLink(GridPos const& a, GridPos const& b);
        /* ************************************************** */
        /*
            You should create whatever functions, variables, or classes you need.
            It doesn't all need to be in this header and cpp, structure it whatever way
            makes sense to you.
        */
        inline Grid3D<ListNode> const& GetGrid()const { return ClosedList; }
        inline bool IsPathable(sizeType x, sizeType y, sizeType z);
        inline bool IsPathable(GridPos const& pos);
        bool IsBlockedByWall(GridPos const& pos);
        void SetPathable(GridPos const& pos, bool occupied);
        inline void SetTags(std::set<TagID> const& tags) { ClosedList.SetTags(tags); }
        inline GridPos GetGridPosition(Vec3f const& in) { return ClosedList.GetGridPosition(in); }
        inline Vec3f GetWorldPosition(GridPos const& in) { return ClosedList.GetWorldPosition(in); }
        inline void FloodOutsideMap(GridPos const& starting) { ClosedList.FloodOutsideMap(starting); }
        inline sizeType GetGridColumns() { return ClosedList.Columns(); }
        inline sizeType GetGridRows() { return ClosedList.Rows(); }
    private:
        inline void CalculateHeuristics(const GridPathRequest& request, sizeType layer_num);
        inline void ProcessOpenList(sizeType layer_num);
        inline void DiscoverNeighbours(const ListNode& target, sizeType layer_num);
        inline void AddToOpenList(ListNode& node, const ListNode& parent, float cost);
        inline void RecursiveInvert(ListNode* parentPtr);
        inline void RubberBandPath(GridPathRequest& request, sizeType layer_num);
        inline void SmoothingSpline(GridPathRequest& request);
        inline void AddInterpolatedPoints(WaypointList& list);
        inline float HeuristicDistance(Heuristic heu, float xDist, float zDist);

        inline LinkID GetLinkID(GridPos const& pos);
        inline bool IsValidID(LinkID id);
        inline GridLayerLink* FindClosestCellLinkOnLayer(Heuristic heu, GridPos layer_pos, LinkLevel check);
        inline bool IsValidLayerLink(GridLayerLink* link);
        inline std::vector<std::pair<LinkID, LinkLevel>> GetLinksOnLayer(sizeType layer_num);
        inline std::list<LinkID> GetPathedLayerLinks(LinkID id, LinkLevel link_way);
        inline sizeType TotalLayerPathCost(std::list<LinkID> const& path);
        inline void GetMinimumVerticalPath(LinkID start_id, LinkID goal_id, std::list<LinkID>& path_result, sizeType& cost, LinkLevel link_way);
        inline void ConcatVerticalWaypoints(std::list<LinkID> const& link_path, WaypointList& path, LinkLevel link_way);
        inline LinkLevel LinkWay(LinkID start_id, LinkID goal_id);
        inline bool EqualLayer(LinkID id_1, LinkID id_2);
    };

    inline void AStarPather::AddGridLayer(valueType height) {
        ClosedList.AddGridLayer(height);
    }

    inline void AStarPather::AddLayerLink(GridPos const& a, GridPos const& b) {
        GridLayerLink link{};
        if (a.layer == b.layer || a.layer >= ClosedList.Layers() || b.layer >= ClosedList.Layers()) {
            return;
        }
        else if (a.layer < b.layer) {
            link.cell_lower = a;
            link.cell_higher = b;
        }
        else {
            link.cell_lower = b;
            link.cell_higher = a;
        }
        layer_links.insert(std::make_pair(++total_ids, link));
    }
}
#endif