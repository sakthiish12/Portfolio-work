#include "pch.h"
#include "AStarPather.h"
namespace BE
{
	bool AStarPather::initialize()
	{
		std::vector<ListNode> preOpen;
		preOpen.reserve(99999);
		OpenList = std::priority_queue <ListNode, std::vector<ListNode>, std::greater<ListNode> >(std::greater<ListNode>(), std::move(preOpen));
		return true; // return false if any errors actually occur, to stop engine initialization

		// pre-create waypoint list for each floors connections, so that at run time do not have to recalculate.

	}

	void AStarPather::shutdown()
	{
	}

	PathResult AStarPather::compute_path(GridPathRequest& request, bool internal)
	{
		requestPtr = &request;
		if (internal) {
			if(!IsPathable(request.start) || !IsPathable(request.goal))
			return PathResult::IMPOSSIBLE;
		}
		if (request.newRequest)
		{
			request.newRequest = false;
			start = request.start;
			goal = request.goal;
			int layer_diff = static_cast<int>(goal.layer) - static_cast<int>(start.layer);
			if (goal.layer == start.layer) {
				requestPtr->path.clear();
				CalculateHeuristics(request, start.layer);
				for (sizeType row = 0; row < ClosedList[start.layer].Rows(); row++)
				{
					for (auto& col : ClosedList[start.layer][row])
					{
						col.state = ListState::Undiscovered;
					}
				}
				while (!OpenList.empty())
				{
					OpenList.pop();
				}
				OpenList.push(ListNode(start, startNodeParentPos, 0.0f, 0.0f));
				ClosedList[start.layer][start.row][start.col].state = ListState::Open;
				status = PathResult::PROCESSING;

				/*if (request.settings.singleStep)
				{
					ProcessOpenList(start.layer);
				}
				else */while (status == PathResult::PROCESSING)
				{
					ProcessOpenList(start.layer);
				}
			}
			else if (std::abs(layer_diff) == 1) {
				GridPathRequest start_layer_req{};
				GridPathRequest goal_layer_req{};
				status = compute_paths_end_layers(request, start_layer_req, goal_layer_req);
				if (status == PathResult::COMPLETE)
				{
					start_layer_req.path.insert(start_layer_req.path.end(), goal_layer_req.path.begin(), goal_layer_req.path.end());
					request.path = start_layer_req.path;
				}
			}
			else
			{
				LinkLevel link_way = layer_diff > 0 ? LinkLevel::Higher : LinkLevel::Lower;
				GridPathRequest start_layer_req{};
				GridPathRequest goal_layer_req{};
				PathResult temp = compute_paths_end_layers(request, start_layer_req, goal_layer_req, false);
				if (temp == PathResult::COMPLETE)
				{
					LinkID start_link_id = GetLinkID(start_layer_req.goal);
					LinkID goal_link_id = GetLinkID(goal_layer_req.start);
					if (IsValidID(start_link_id) && IsValidID(goal_link_id)) {
						std::list<LinkID> link_path;
						sizeType cost = 0;
						GetMinimumVerticalPath(start_link_id, goal_link_id, link_path, cost, link_way);
						if (!link_path.empty()) {
							request.path = start_layer_req.path;
							ConcatVerticalWaypoints(link_path, request.path, link_way);
							request.path.insert(request.path.end(), goal_layer_req.path.begin(), goal_layer_req.path.end());
							status = PathResult::COMPLETE;
						}
						else {
							status = PathResult::IMPOSSIBLE;
						}
					}
				}
				else {
					status = temp;
				}
			}

		}

		// Just sample code, safe to delete
		return status;
	}

	PathResult AStarPather::compute_path(PathRequest& request)
	{
		GridPathRequest gridrequest{};
		gridrequest.start = ClosedList.GetGridPosition(request.start);
		gridrequest.goal = ClosedList.GetGridPosition(request.goal);
		gridrequest.newRequest = true;
		gridrequest.settings = request.settings;
		PathResult toret = compute_path(gridrequest);
		request.path = gridrequest.path;
		request.newRequest = false;
		return toret;
	}

	PathResult AStarPather::compute_paths_end_layers(GridPathRequest& main_request, GridPathRequest& start_layer_req, GridPathRequest& goal_layer_req, bool single)
	{
		int layer_diff = goal.layer - start.layer;
		GridLayerLink* link_start = nullptr;
		GridLayerLink* link_goal = nullptr;

		if (layer_diff > 0) {
			link_start = FindClosestCellLinkOnLayer(main_request.settings.heuristic, main_request.start, LinkLevel::Higher);

			if (single) link_goal = link_start;
			else link_goal = FindClosestCellLinkOnLayer(main_request.settings.heuristic, main_request.goal, LinkLevel::Lower);

			if (!IsValidLayerLink(link_start) || !IsValidLayerLink(link_goal)) return PathResult::IMPOSSIBLE;
			start_layer_req = { start, link_start->cell_lower, WaypointList(), main_request.settings, true };
			goal_layer_req = { link_goal->cell_higher, goal, WaypointList(), main_request.settings, true };
		}
		else {
			link_start = FindClosestCellLinkOnLayer(main_request.settings.heuristic, main_request.start, LinkLevel::Lower);

			if (single) link_goal = link_start;
			else link_goal = FindClosestCellLinkOnLayer(main_request.settings.heuristic, main_request.goal, LinkLevel::Higher);

			if (!IsValidLayerLink(link_start) || !IsValidLayerLink(link_goal)) return PathResult::IMPOSSIBLE;
			start_layer_req = { start, link_start->cell_higher, WaypointList(), main_request.settings, true };
			goal_layer_req = { link_goal->cell_lower, goal, WaypointList(), main_request.settings, true };
		}
		PathResult start_layer_result = compute_path(start_layer_req);
		if (start_layer_result == PathResult::IMPOSSIBLE) {
			return start_layer_result;
		}
		PathResult goal_layer_result = compute_path(goal_layer_req, true);
		if (start_layer_result == PathResult::COMPLETE && goal_layer_result == PathResult::COMPLETE)
		{
			return PathResult::COMPLETE;
		}
		else return PathResult::IMPOSSIBLE;
	}

	void AStarPather::UpdateGrid()
	{
		ClosedList.UpdateGrid();
		for (sizeType y = 0; y < ClosedList.Layers(); ++y) {
			for (sizeType z = 0; z < ClosedList.Rows(); ++z)
			{
				for (sizeType x = 0; x < ClosedList.Columns(); ++x)
				{
					ClosedList[y][z][x].pos.col = x;
					ClosedList[y][z][x].pos.row = z;
					ClosedList[y][z][x].pos.layer = y;
				}
			}
		}
		//worldToGridMultipliyerSqr = ClosedList.GetTileSize() * ClosedList.GetTileSize();
	}

	void AStarPather::UpdateLayerPaths(PFSettings settings)
	{
		for (sizeType layer = 0; layer < ClosedList.Layers(); layer++)
		{
			auto ids = GetLinksOnLayer(layer);
			for (size_t i = 0; i < ids.size(); i++) {
				for (size_t j = i + 1; j < ids.size(); j++) {
					//ignore layer links of the same level 
					if (ids[i].second == ids[j].second) continue;
					bool ID1_ishigher = ids[i].second == LinkLevel::Higher;
					GridPos* pos1 = ID1_ishigher ? &layer_links.at(ids[i].first).cell_lower : &layer_links.at(ids[i].first).cell_higher;
					GridPos* pos2 = !ID1_ishigher ? &layer_links.at(ids[j].first).cell_lower : &layer_links.at(ids[j].first).cell_higher;
					CalculatedPath calcPath;
					calcPath.request.start = ID1_ishigher ? *pos2 : *pos1;
					calcPath.request.goal = !ID1_ishigher ? *pos2 : *pos1;
					calcPath.request.settings = settings;
					PathResult result = compute_path(calcPath.request, true);
					if (result == PathResult::COMPLETE) {
						calcPath.id_lower = !ID1_ishigher ? ids[i].first : ids[j].first;
						calcPath.id_higher = ID1_ishigher ? ids[i].first : ids[j].first;
						calcPath.cost = static_cast<sizeType>(calcPath.request.path.size());
						calcPath.layer = layer;
						layer_paths.emplace_back(calcPath);
					}
				}
			}
		}
	}

	inline void AStarPather::CalculateHeuristics(const GridPathRequest& request, sizeType layer_num)
	{
		float xDist = static_cast<float>(goal.col);
		float zDist = static_cast<float>(goal.row);
		for (uint8_t z = 0; z < ClosedList.Rows(); ++z)
		{
			xDist = static_cast<short>(goal.col);
			for (uint8_t x = 0; x < ClosedList.Columns(); ++x)
			{
				if (IsPathable(x, layer_num, z))
				{
					ClosedList[layer_num][z][x].heuristic = HeuristicDistance(request.settings.heuristic, xDist, zDist);
				}
				--xDist;
			}
			--zDist;
		}
	}

	float AStarPather::HeuristicDistance(Heuristic heu, float xDist, float zDist)
	{
		switch (heu)
		{
		case Heuristic::MANHATTAN: {
			return std::abs(xDist) + std::abs(zDist);
			break;
		}
		case Heuristic::OCTILE: {
			float absX{ static_cast<float>(std::abs(xDist)) }, absZ{ static_cast<float>(std::abs(zDist)) };
			return std::min(absX, absZ) * diagonalCost + std::max(absX, absZ) - std::min(absX, absZ);
			break;
		}
		case Heuristic::CHEBYSHEV: {
			return std::max(std::abs(xDist), std::abs(zDist));
			break;
		}
		case Heuristic::EUCLIDEAN: {
			return sqrt(xDist * xDist + zDist * zDist);
			break;
		}
		}
		return 0.f;
	}

	inline void AStarPather::ProcessOpenList(sizeType layer_num)
	{
		ListNode current;
		while (!OpenList.empty())
		{
			current = OpenList.top();
			OpenList.pop();
			if (ClosedList[layer_num][current.pos.row][current.pos.col] == current)
				break;
		}
		if (current == goal)
		{
			RecursiveInvert(&ClosedList[layer_num][current.pos.row][current.pos.col]);
			if (requestPtr->settings.rubberBanding)
			{
				RubberBandPath(*requestPtr, layer_num);
			}
			if (requestPtr->settings.smoothing)
			{
				SmoothingSpline(*requestPtr);
			}
			status = PathResult::COMPLETE;
			return;
		}
		DiscoverNeighbours(current, layer_num);
		if (OpenList.empty())
		{
			status = PathResult::IMPOSSIBLE;
			return;
		}
		current.state = ListState::Close;
		ClosedList[layer_num][current.pos.row][current.pos.col] = current;

	}

	inline void AStarPather::DiscoverNeighbours(const ListNode& target, sizeType layer_num)
	{
		sizeType x = target.pos.col;
		sizeType z = target.pos.row;
		if (IsPathable(x + 1, layer_num, z))
		{
			AddToOpenList(ClosedList[layer_num][z][x + 1], target, regularCost);
			if (IsPathable(x, layer_num, z + 1) && IsPathable(x + 1, layer_num, z + 1))
				AddToOpenList(ClosedList[layer_num][z + 1][x + 1], target, diagonalCost);
			if (IsPathable(x, layer_num, z - 1) && IsPathable(x + 1, layer_num, z - 1))
				AddToOpenList(ClosedList[layer_num][z - 1][x + 1], target, diagonalCost);
		}
		if (IsPathable(x - 1, layer_num, z))
		{
			AddToOpenList(ClosedList[layer_num][z][x - 1], target, regularCost);
			if (IsPathable(x, layer_num, z + 1) && IsPathable(x - 1, layer_num, z + 1))
				AddToOpenList(ClosedList[layer_num][z + 1][x - 1], target, diagonalCost);
			if (IsPathable(x, layer_num, z - 1) && IsPathable(x - 1, layer_num, z - 1))
				AddToOpenList(ClosedList[layer_num][z - 1][x - 1], target, diagonalCost);
		}
		if (IsPathable(x, layer_num, z + 1))
			AddToOpenList(ClosedList[layer_num][z + 1][x], target, regularCost);
		if (IsPathable(x, layer_num, z - 1))
			AddToOpenList(ClosedList[layer_num][z - 1][x], target, regularCost);

		/*if (target.pos.layer == 1) {
			std::cout << OpenList.size() << " lol\n";
		}*/
		// check special case of layer links, ie staircases
		// if (IsPathable)
	}

	inline void AStarPather::AddToOpenList(ListNode& node, const ListNode& parent, float cost)
	{
		float finalCost = node.heuristic * requestPtr->settings.weight + parent.startDistance + cost;
		if (node == parent)
			return;
		if (node.state == ListState::Undiscovered)
		{
			node.parentpos = parent.pos;
			node.startDistance = parent.startDistance + cost;
			node.totalCost = finalCost;
			node.state = ListState::Open;
			OpenList.push(node);
		}
		else if (finalCost < node.totalCost)
		{
			node.parentpos = parent.pos;
			node.state = ListState::Open;
			node.startDistance = parent.startDistance + cost;
			node.totalCost = finalCost;
			OpenList.push(node);
		}
	}

	bool AStarPather::IsPathable(sizeType x, sizeType y, sizeType z)
	{
		if (x >= ClosedList.Columns() || z >= ClosedList.Rows() || y >= ClosedList.Layers()) return false;
		return !(bool(ClosedList[y][z][x]));
	}

	bool AStarPather::IsPathable(GridPos const& pos)
	{
		if (pos.col >= ClosedList.Columns() || pos.row >= ClosedList.Rows() || pos.layer >= ClosedList.Layers()) return false;
		return !(bool(ClosedList[pos.layer][pos.row][pos.col]));
	}

	bool AStarPather::IsBlockedByWall(GridPos const& pos)
	{
		if (pos.col >= ClosedList.Columns() || pos.row >= ClosedList.Rows() || pos.layer >= ClosedList.Layers()) return false;
		return ClosedList[pos.layer][pos.row][pos.col].occupied && ClosedList[pos.layer][pos.row][pos.col].isWall;
	}

	void AStarPather::SetPathable(GridPos const& pos, bool occupied)
	{
		if (pos.col >= ClosedList.Columns() || pos.row >= ClosedList.Rows() || pos.layer >= ClosedList.Layers()) return;
		if (!occupied && ClosedList[pos.layer][pos.row][pos.col].isWall) {
			return;
		}
		ClosedList[pos.layer][pos.row][pos.col].occupied = occupied;
	}

	inline void AStarPather::RecursiveInvert(ListNode* parentPtr)
	{
		if (!(*parentPtr == start))
		{
			RecursiveInvert(&ClosedList[parentPtr->parentpos.layer][parentPtr->parentpos.row][parentPtr->parentpos.col]);
		}

		auto pos = ClosedList.GetWorldPosition(parentPtr->pos);
		requestPtr->path.push_back(pos);
	}

	inline void AStarPather::RubberBandPath(GridPathRequest& request, sizeType layer_num)
	{
		if (request.path.size() < 3)
			return;
		decltype(request.path) newPath;
		request.path.reverse();
		newPath.push_back(request.path.front());
		request.path.pop_front();
		while (true)
		{
			Vec3 middle = request.path.front();
			request.path.pop_front();
			if (request.path.empty())
			{
				newPath.push_back(middle);
				break;
			}
			bool PathFail = false;
			GridPos p1{ ClosedList.GetGridPosition(newPath.back()) }, p2{ ClosedList.GetGridPosition(request.path.front()) }, p3{ ClosedList.GetGridPosition(middle) };
			GridPos minP{ std::min(p3.row,std::min(p1.row,p2.row)),std::min(p3.col,std::min(p1.col,p2.col)) }, maxP{ std::max(p3.row,std::max(p1.row,p2.row)),std::max(p3.col,std::max(p1.col,p2.col)) };
			for (sizeType z = minP.row; z < maxP.row + 1; ++z)
			{
				for (sizeType x = minP.col; x < maxP.col + 1; ++x)
				{
					if (IsPathable(x, layer_num, z))
					{
						PathFail = true;
						break;
					}
				}
				if (PathFail)
					break;
			}
			if (PathFail)
				newPath.push_back(middle);
		}
		std::swap(newPath, request.path);
		request.path.reverse();
	}

	inline void AStarPather::SmoothingSpline(GridPathRequest& request)
	{
		if (request.path.size() < 2)
			return;
		WaypointList newPath;
		newPath.emplace_front(request.path.front());
		request.path.pop_front();
		while (!request.path.empty())
		{
			newPath.emplace_front(request.path.front());
			request.path.pop_front();
			AddInterpolatedPoints(newPath);
		}
		newPath.reverse();
		if (newPath.size() < 3)
		{
			std::swap(newPath, request.path);
			return;
		}
		auto itr = newPath.begin();
		request.path.emplace_back(*itr);
		request.path.emplace_back(CatmullRom(*itr, *itr, *std::next(itr), *std::next(itr, 2), 0.25f));
		request.path.emplace_back(CatmullRom(*itr, *itr, *std::next(itr), *std::next(itr, 2), 0.5f));
		request.path.emplace_back(CatmullRom(*itr, *itr, *std::next(itr), *std::next(itr, 2), 0.75f));
		request.path.emplace_back(*++itr);
		while (std::next(itr, 2) != newPath.end())
		{
			request.path.emplace_back(CatmullRom(*std::prev(itr), *itr, *std::next(itr), *std::next(itr, 2), 0.25f));
			request.path.emplace_back(CatmullRom(*std::prev(itr), *itr, *std::next(itr), *std::next(itr, 2), 0.5f));
			request.path.emplace_back(CatmullRom(*std::prev(itr), *itr, *std::next(itr), *std::next(itr, 2), 0.75f));
			request.path.emplace_back(*++itr);
		}
		request.path.emplace_back(CatmullRom(*std::prev(itr), *itr, *std::next(itr), *std::next(itr), 0.25f));
		request.path.emplace_back(CatmullRom(*std::prev(itr), *itr, *std::next(itr), *std::next(itr), 0.5f));
		request.path.emplace_back(CatmullRom(*std::prev(itr), *itr, *std::next(itr), *std::next(itr), 0.75f));
	}
	inline void AStarPather::AddInterpolatedPoints(WaypointList& list)
	{
		const float interTolerance = worldToGridMultipliyerSqr * 1.5f * 1.5f;
		auto itr = list.begin();
		while (std::next(itr) != list.end())
		{
			while (itr->SqDistBetween(*std::next(itr)) > interTolerance)
			{
				list.insert(std::next(itr), (*itr + *std::next(itr)) / 2.0f);
			}
			++itr;
		}

	}

	inline LinkID AStarPather::GetLinkID(GridPos const& pos) {
		for (auto& [k, link] : layer_links) {
			if (link.cell_higher == pos || link.cell_lower == pos)
				return k;
		}
		return 0;
	}
	inline bool AStarPather::IsValidID(LinkID id) {
		return id != NULL_LINKID && id <= total_ids;
	}

	inline GridLayerLink* AStarPather::FindClosestCellLinkOnLayer(Heuristic heu, GridPos layer_pos, LinkLevel check)
	{
		GridLayerLink* temp = nullptr;
		float dist = std::numeric_limits<float>::max();
		for (auto& [k, link] : layer_links) {

			if (check == LinkLevel::Higher) {
				if (link.cell_lower.layer == layer_pos.layer &&
					link.cell_higher.layer == layer_pos.layer + 1) {
					float curr_dist = HeuristicDistance(heu, static_cast<float>(layer_pos.col) - static_cast<float>(link.cell_lower.col), static_cast<float>(layer_pos.row) - static_cast<float>(link.cell_lower.row));
					if (dist > curr_dist) {
						dist = curr_dist;
						temp = &link;
					}
				}
			}
			else {
				if (link.cell_higher.layer == layer_pos.layer &&
					link.cell_lower.layer == layer_pos.layer - 1) {
					float curr_dist = HeuristicDistance(heu, static_cast<float>(layer_pos.col) - static_cast<float>(link.cell_higher.col), static_cast<float>(layer_pos.row) - static_cast<float>(link.cell_higher.row));
					if (dist > curr_dist) {
						dist = curr_dist;
						temp = &link;
					}
				}
			}
		}
		return temp;
	}

	inline bool AStarPather::IsValidLayerLink(GridLayerLink* link)
	{
		if (link == nullptr) return false;
		return link->cell_higher.layer != link->cell_lower.layer;
	}


	inline std::vector<std::pair<LinkID, LinkLevel>> AStarPather::GetLinksOnLayer(sizeType layer_num)
	{
		std::vector<std::pair<LinkID, LinkLevel>> result;
		for (auto& [id, link] : layer_links) {
			if (link.cell_lower.layer == layer_num)
				result.emplace_back(std::make_pair(id, LinkLevel::Higher));
			else if (link.cell_higher.layer == layer_num)
				result.emplace_back(std::make_pair(id, LinkLevel::Lower));
		}
		return result;
	}

	inline std::list<LinkID> AStarPather::GetPathedLayerLinks(LinkID id, LinkLevel link_way)
	{
		std::list<LinkID> result;
		for (auto& paths : layer_paths) {
			if (link_way == LinkLevel::Higher && paths.id_lower == id) {
				result.push_back(paths.id_higher);
			}
			else if (link_way == LinkLevel::Lower && paths.id_higher == id) {
				result.push_back(paths.id_lower);
			}
		}
		return result;
	}

	inline sizeType AStarPather::TotalLayerPathCost(std::list<LinkID> const& path)
	{
		sizeType total = 0;
		for (auto itr = path.begin(); std::next(itr) != path.end(); itr++) {
			for (auto& stored_path : layer_paths) {
				if ((stored_path.id_lower == *itr && stored_path.id_higher == *std::next(itr)) ||
					(stored_path.id_higher == *itr && stored_path.id_lower == *std::next(itr))) {
					total += stored_path.cost;
				}
			}
		}
		return total;
	}
	inline void AStarPather::GetMinimumVerticalPath(LinkID start_id, LinkID goal_id, std::list<LinkID>& path_result, sizeType& cost, LinkLevel link_way)
	{
		if (link_processing.empty()) {
			link_processing.push_back(start_id);
			cost = std::numeric_limits<sizeType>::max();
		}
		for (auto& linked_id : GetPathedLayerLinks(start_id, link_way)) {
			link_processing.push_back(linked_id);
			if (linked_id == goal_id) {
				sizeType newtotal = TotalLayerPathCost(link_processing);
				if (newtotal < cost) {
					cost = newtotal;
					path_result = link_processing;
				}
			}
			else if (!EqualLayer(linked_id, goal_id))
				GetMinimumVerticalPath(linked_id, goal_id, path_result, cost, link_way);
			link_processing.pop_back();
		}
		if (link_processing.size() == 1) {
			link_processing.pop_back();
		}
	}

	inline void AStarPather::ConcatVerticalWaypoints(std::list<LinkID> const& link_path, WaypointList& path, LinkLevel link_way)
	{
		for (auto path_itr = link_path.begin(); std::next(path_itr) != link_path.end(); path_itr++) {
			if (link_way == LinkLevel::Higher) {
				for (auto itr = layer_paths.begin(); itr != layer_paths.end(); ++itr) {
					if (itr->id_lower == *path_itr && itr->id_higher == *std::next(path_itr)) {
						path.insert(path.end(), itr->request.path.begin(), itr->request.path.end());
					}
				}
			}
			else {
				for (auto rever_itr = layer_paths.rbegin(); rever_itr != layer_paths.rend(); ++rever_itr) {
					if (rever_itr->id_higher == *path_itr && rever_itr->id_lower == *std::next(path_itr)) {
						path.insert(path.end(), rever_itr->request.path.rbegin(), rever_itr->request.path.rend());
					}
				}
			}
		}
	}

	inline LinkLevel AStarPather::LinkWay(LinkID start_id, LinkID goal_id) {
		if (layer_links.at(start_id).cell_higher.layer < layer_links.at(goal_id).cell_higher.layer)
			return LinkLevel::Higher;
		else if (layer_links.at(start_id).cell_higher.layer > layer_links.at(goal_id).cell_higher.layer)
			return LinkLevel::Lower;
		else {
			AssertCore(false, "Comparing 2 Layer Links at the same level");
			return LinkLevel::Lower;
		}
	}

	inline bool AStarPather::EqualLayer(LinkID id_1, LinkID id_2) {
		return layer_links.at(id_1).cell_higher.layer == layer_links.at(id_2).cell_higher.layer;
	}
}