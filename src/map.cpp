#include "map.hpp"
#include "actor.hpp"
#include "attacker.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "destructible.hpp"
#include "dice.hpp"
#include "effect.hpp"
#include "ignore.hpp"
#include "log.hpp"
#include "pathfinding.hpp" // For PriorityQueue for room-based pathfinding
#include "pickable.hpp"
#include "point.hpp"
#include "rect.hpp"
#include "status_effect.hpp"
#include "vec.hpp"
#include "world.hpp"

#include <SFML/Graphics/Color.hpp>

#include <sstream>
#include <unordered_map> // For room-based pathfinding

// This ctor is required for s11n
Map::Map() :
Map(constants::DEFAULT_MAP_WIDTH,
	  constants::DEFAULT_MAP_HEIGHT,
		MapType::BUILDING,
	  nullptr) { }

Map::Map(int width, int height, MapType mapType, World* world) :
width(width), height(height), world(world) {
	tiles = Mat2d<Tile>(width, height);
	for(int i = 0; i < width; ++i) {
		for(int j = 0; j < height; ++j) {
			tiles(i, j) = Tile();
		}
	}
	generateMap(mapType);
}

void Map::generateMap(MapType mapType) {
	switch(mapType) {
		case MapType::BUILDING:
			generateBuildingMap();
			break;
		case MapType::PILLARS:
			generatePillarsMap();
			break;
		case MapType::WATER:
			generateWaterMap();
			break;
		default:
			break;
	}

	log::info("\n\n");
	log::info("Map rooms:");
	log::info(std::to_string(rooms.size()).append(" rooms"));
	for(auto& room : rooms) {
		log::info(
			std::string{"room #"}
			.append(std::to_string(room.id))
			.append(": ")
		);
		log::info(
			std::string{"  "}
			.append("(")
			.append(std::to_string(room.value.coordinates.x0()))
			.append(", ")
			.append(std::to_string(room.value.coordinates.y0()))
			.append("), (")
			.append(std::to_string(room.value.coordinates.x1()))
			.append(", ")
			.append(std::to_string(room.value.coordinates.y1()))
			.append(")")
		);
		std::stringstream ss;
		ss << "  neighbours: ";
		for(auto& a: room.neighbours){
			ss << "#" << a << " ";
		}
		log::info(ss.str());
	}
	log::info("--");
	log::info("Map tiles:");
	int x = 0;
	int y = 0;
	std::stringstream ss;
	for(auto& tile : tiles) {
		if(!tile.walkable) ss << "#";
		else {
			if(getRooms(Point {x, y}).size() == 0) {
				ss << ".";
			} else if(getRooms(Point {x, y}).at(0).roomType == RoomType::COMMAND_CENTER) {
				ss << "c";
			} else if(getRooms(Point {x, y}).at(0).roomType == RoomType::START) {
				ss << "s";
			} else if(getRooms(Point {x, y}).at(0).roomType == RoomType::ARMOURY) {
				ss << "a";
			} else if(getRooms(Point {x, y}).at(0).roomType == RoomType::MARKET) {
				ss << "m";
			} else if(getRooms(Point {x, y}).at(0).roomType == RoomType::UPSTAIRS) {
				ss << "u";
			} else if(getRooms(Point {x, y}).at(0).roomType == RoomType::HYDROPONICS) {
				ss << "h";
			} else if(getRooms(Point {x, y}).at(0).roomType == RoomType::CLANDESTINE_LAB) {
				ss << "l";
			} else {
				ss << ".";
			}
		}
		++x;
		if(x >= width) {
			log::info(ss.str());
			ss = std::stringstream {};
			x = 0;
			++y;
		}
	}
	log::info("--");
	log::info("Map graph edges:");
	auto g = getEdges(rooms);
	for(auto p : g) {
		log::info(
			std::string{"("}
			.append(std::to_string(p.first))
			.append(", ")
			.append(std::to_string(p.second))
			.append(")")
		);
	}
	log::info("--");
}

std::vector<Room> Map::getRooms(Point location) {
	std::vector<Room> r;
	for(auto room : rooms) {
		if(   location.x >= room.value.x0()
			 && location.x <= room.value.x1()
		   && location.y >= room.value.y0()
		   && location.y <= room.value.y1()) {
			r.push_back(room.value);
		}
	}
	return r;
}

void Map::generateBuildingMap() {
	rooms = breakRooms(Rect(0, 0, (width - 1), (height - 1)));
	rooms = indexRooms(rooms);
	physicalConnectionsBetweenRooms = connectRooms(rooms);
	rooms = pruneEdges(physicalConnectionsBetweenRooms);
	rooms = makeEdgesBidirectional(rooms);
	minimumSpanningTree = cullDoubleEdges(rooms);
	rooms = makeLoops(minimumSpanningTree, physicalConnectionsBetweenRooms, 0.3f);
	rooms = specializeRooms(rooms, minimumSpanningTree);

	// initialize whole map to walkable
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			if(true) {
				tiles(x, y).walkable = true;
			}
		}
	}

	// draw walls
	for(auto& aNode : rooms) {
		auto a = aNode.value;
		for(int x = a.x0(); x <= a.x1(); ++x) {
			for(int y = a.y0(); y <= a.y1(); ++y) {
				if(x == a.x0() || x == a.x1() || y == a.y0() || y == a.y1()) {
					tiles(x, y).walkable = false;
				}
				// if(x == a.x0() + 1 || x == a.x1() - 1 || y == a.y0() + 1 || y == a.y1() - 1) {
				// 	//tiles(x, y).walkable = true;
				// }
			}
		}
	}

	// open doorways
	for(auto& roomNode : rooms) {
		auto room = roomNode.value;
		for(auto& otherIndex : roomNode.neighbours) {
			auto other = std::find_if(rooms.begin(), rooms.end(), [&](const auto& r) {return r.id == otherIndex;})->value;
			int overlapMinX = fmax(room.x0(), other.x0());
			int overlapMaxX = fmin(room.x1(), other.x1());
			int overlapMinY = fmax(room.y0(), other.y0());
			int overlapMaxY = fmin(room.y1(), other.y1());
			int centerX = floor((overlapMinX + overlapMaxX) / 2);
			int centerY = floor((overlapMinY + overlapMaxY) / 2);
			tiles(centerX, centerY).walkable = true;
		}
	}

	// close map boundaries
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			if(x == 0 || x == width - 1 || y == 0 || y == height - 1) {
				tiles(x, y).walkable = false;
			}
		}
	}
}

void Map::generatePillarsMap() {
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			if(x % 3 == 0 && y % 3 == 0) tiles(x, y).walkable = false;
		}
	}
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			if(x == 0 || x == width - 1 || y == 0 || y == height - 1) {
				tiles(x, y).walkable = false;
			}
		}
	}
}

void Map::generateWaterMap() {
	hasAnimations = true;
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			tiles(x, y) = Tile();
			tiles(x, y).terrain = Terrain::WATER;
			Animation anim;
			anim.chars = std::vector<char>(); // empty on purpose
			anim.colors = std::vector<sf::Color>();
			anim.colors.push_back(colors::get("darkestBlue"));
			anim.charFreq = 1;
			anim.colFreq = 10;
			anim.phase = 0.0f;
			tiles(x, y).animation = anim;
		}
	}
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			if(x % 3 == 0 && y % 3 == 0) {
				tiles(x, y).walkable = false;
				tiles(x, y).animation = std::nullopt;
			}
		}
	}
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			if(x == 0 || x == width - 1 || y == 0 || y == height - 1) {
				tiles(x, y).walkable = false;
				tiles(x, y).animation = std::nullopt;
			}
		}
	}
}

Graph<Room> Map::breakRooms(Rect area, BreakDirection direction) {
	int minDim = 18;
	Graph<Room> areas;

	if(area.width() < minDim || area.height() < minDim) {
		areas.push_back({0, {area, RoomType::UNASSIGNED, RoomDecor::NONE, factions::neutral}, {}});
		return areas;
	}
	else {
		Rect area1 = area;
		Rect area2 = area;
		if(direction == BreakDirection::HORIZONTAL) {
			// 3 = minimum width for a corridor
			int xBreak = randomInRange(3, area.width()-3);
			// if xBreak - 0 were xBreak - 1, this would cause areas to NOT overlap
			area1 = Rect(area.x0(),          area.y0(), area.x0() + xBreak - 0, area.y1());
			area2 = Rect(area.x0() + xBreak, area.y0(), area.x1(),              area.y1());
		} else {
			int yBreak = randomInRange(3, area.height()-3);
			area1 = Rect(area.x0(), area.y0(),          area.x1(), area.y0() + yBreak - 0);
			area2 = Rect(area.x0(), area.y0() + yBreak, area.x1(), area.y1()             );
		}

		BreakDirection nextDir = (direction == BreakDirection::HORIZONTAL) ? BreakDirection::VERTICAL : BreakDirection::HORIZONTAL;

		Graph<Room> areas1 = breakRooms(area1, nextDir);
		areas.insert(areas.end(), areas1.begin(), areas1.end());
		Graph<Room> areas2 = breakRooms(area2, nextDir);
		areas.insert(areas.end(), areas2.begin(), areas2.end());

		return areas;
	}
}

Graph<Room> Map::indexRooms(Graph<Room> rooms) {
	Graph<Room> ret;
	int i = 0;
	for(auto& r : rooms) {
		ret.push_back({i, r.value, {}});
		++i;
	}
	return ret;
}

Graph<Room> Map::connectRooms(Graph<Room> rooms) {
	Graph<Room> ret = rooms;
	for(auto& roomNode : ret) {
		for(auto& otherNode : ret) {
			auto room = roomNode.value;
			auto other = otherNode.value;
			if(room == other) continue;
			// check if overlap
			if(room.x0() > other.x1() || other.x0() > room.x1()) continue; // no overlap
			if(room.y0() > other.y1() || other.y0() > room.y1()) continue; // no overlap
			// overlap!
			roomNode.neighbours.emplace_back(otherNode.id);
		}
	}
	return ret;
}

// Simple (and horrible) Prim's algorithm implementation
Graph<Room> Map::pruneEdges(Graph<Room> rooms) {
	const auto src = rooms;
	Graph<Room> ret {};

	int s = randomInRange(0, rooms.size() - 1);
	auto currentRoom = src.at(s);
	auto currentRoomNoNBs = GraphNode<Room>{currentRoom.id, currentRoom.value, {}};
	ret.push_back(currentRoomNoNBs);
	rooms.erase(
    std::remove_if(rooms.begin(), rooms.end(),
        [&](const auto& a) { return a.id == currentRoom.id; }),
    rooms.end());

	while(!rooms.empty()) {
		std::vector<int> possibleNextRooms {};
		std::copy_if ( // only those ids that are not in ret yet
			currentRoom.neighbours.begin(), currentRoom.neighbours.end(),
			std::back_inserter(possibleNextRooms),
			[&](const auto& a) { return std::count_if(ret.begin(), ret.end(), [&](const auto& b) { return b.id == a; }) == 0; }
		);

		if(possibleNextRooms.size() > 0) {
			int rnd = randomInRange(0, possibleNextRooms.size() - 1);
			int nextId = possibleNextRooms.at(rnd);
			// add neighbour to previously-added node
			auto& previousRoom = *std::find_if(ret.begin(), ret.end(), [&](const auto& a) { return a.id == currentRoom.id; });
			previousRoom.neighbours.push_back(nextId);

			currentRoom = *std::find_if(src.begin(), src.end(), [&](const auto& a) { return a.id == nextId; });
			auto currentRoomNoNBs = GraphNode<Room>{currentRoom.id, currentRoom.value, {}};
			ret.push_back(currentRoomNoNBs);
			rooms.erase(
		    std::remove_if(rooms.begin(), rooms.end(),
		        [&](const auto& a) { return a.id == currentRoom.id; }),
		    rooms.end());
		} else { // currentRoom does not have any possible neighbours, select new room
			// collect all rooms that are connected to some of the room(s) currently in ret
			// and that are not in ret themselves
			std::vector<int> possibleNextNodes {};
			for(auto& potentialRoom : rooms) {
				for(auto& roomInRet : ret) {
					auto roomWithNBs = *std::find_if(src.begin(), src.end(), [&](const auto& a) { return a.id == roomInRet.id; });
					for(auto& n : roomWithNBs.neighbours) {
						if (potentialRoom.id == n) {
							possibleNextNodes.push_back(n);
						}
					}
				}
			}
			// remove duplicates
			std::sort(possibleNextNodes.begin(), possibleNextNodes.end());
			possibleNextNodes.erase(unique(possibleNextNodes.begin(), possibleNextNodes.end()), possibleNextNodes.end());

			int rnd = randomInRange(0, possibleNextNodes.size() - 1);
			int nextId = possibleNextNodes.at(rnd);

			// add neighbour to some previously-added node that connects to upcoming node
			std::vector<int> possiblePreviousRooms {};
			for(auto& roomInRet : ret) {
				auto roomWithNBs = *std::find_if(src.begin(), src.end(), [&](const auto& a) { return a.id == roomInRet.id; });
				for(auto& n : roomWithNBs.neighbours) {
					if(n == nextId) {
						possiblePreviousRooms.push_back(roomWithNBs.id);
					}
				}
			}
			// remove duplicates
			std::sort(possiblePreviousRooms.begin(), possiblePreviousRooms.end());
			possiblePreviousRooms.erase(unique(possiblePreviousRooms.begin(), possiblePreviousRooms.end()), possiblePreviousRooms.end());

			int rndPrev = randomInRange(0, possiblePreviousRooms.size() - 1);
			int prevId = possiblePreviousRooms.at(rndPrev);

			auto& previousRoom = *std::find_if(ret.begin(), ret.end(), [&](const auto& a) { return a.id == prevId; });
			previousRoom.neighbours.push_back(nextId);

			currentRoom = *std::find_if(src.begin(), src.end(), [&](const auto& a) { return a.id == nextId; });
			auto currentRoomNoNBs = GraphNode<Room>{currentRoom.id, currentRoom.value, {}};
			ret.push_back(currentRoomNoNBs);
			rooms.erase(
		    std::remove_if(rooms.begin(), rooms.end(),
		        [&](const auto& a) { return a.id == currentRoom.id; }),
		    rooms.end());
		}
	}

	return ret;
}

Graph<Room> Map::makeEdgesBidirectional(Graph<Room> rooms) {
	Graph<Room> ret = rooms;

	// add bidirectional edges: if there's an edge from A to B, there's an edge from B to A
	for(auto& room : ret) {
		for(auto neighbourIndex : room.neighbours) {
			std::find_if(ret.begin(), ret.end(), [&](const auto& r) {return r.id == neighbourIndex;})->neighbours.push_back(room.id);
		}
	}

	return ret;
}

Graph<Room> Map::cullDoubleEdges(Graph<Room> rooms) {
	Graph<Room> ret = rooms;
	// clear double edges: at this stage we shouldn't have two edges A-B, A-B
	for(auto& room : ret) {
		std::sort(room.neighbours.begin(), room.neighbours.end());
		room.neighbours.erase(unique(room.neighbours.begin(), room.neighbours.end()), room.neighbours.end());
	}

	return ret;
}

Graph<Room> Map::makeLoops(Graph<Room> rooms, const Graph<Room> physicalConnections, float loopFactor) {
	Graph<Room> ret = rooms;
	for(auto& room : ret) {
		// TODO actually use loopFactor here for condition
		// TODO move loopFactor, and other major mapgen vars, to TOML
		if(randomInRange(0, 99) <= 30) {
			int id = room.id;
			auto roomWithNBs = std::find_if(physicalConnections.begin(), physicalConnections.end(), [&](const auto& r) {return r.id == id;});
			auto allPhysicalNBs = roomWithNBs->neighbours;
			std::vector<int> filteredNBs {};
			for(auto& nbCandidate : allPhysicalNBs) {
				if(std::count(room.neighbours.begin(), room.neighbours.end(), nbCandidate) == 0) {
					filteredNBs.push_back(nbCandidate);
				}
			}
			if(filteredNBs.size() > 0) {
				int rnd = randomInRange(0, filteredNBs.size() - 1);
				int newNeighbour = filteredNBs.at(rnd);

				// add neighbour bidirectionally
				room.neighbours.push_back(newNeighbour);
				for(auto neighbourIndex : room.neighbours) {
					std::find_if(ret.begin(), ret.end(), [&](const auto& r) {return r.id == newNeighbour;})->neighbours.push_back(room.id);
					ignore(neighbourIndex);
				}
			}
		}
	}
	return ret;
}

Graph<Room> Map::specializeRooms(Graph<Room> rooms, const Graph<Room> mst) {
	Graph<Room> ret = rooms;
	std::vector<GraphNode<Room>> leafNodes {};
	std::copy_if(mst.begin(), mst.end(), std::back_inserter(leafNodes), [&](const auto& r) { return r.neighbours.size() == 1; });
	std::vector<int> leafIndices (leafNodes.size());
	std::transform(leafNodes.begin(), leafNodes.end(), leafIndices.begin(), [&](const auto& r) { return r.id; });

	// Starting room
	// There are at least 2 leaf rooms at this point
	{ // scope for temporaries
	int rnd = randomInRange(0, leafIndices.size() - 1);
	int start = leafIndices.at(rnd);
	std::find_if(ret.begin(), ret.end(), [&](const auto& r) { return r.id == start; })->value.roomType = RoomType::START;
	leafIndices.erase(std::remove_if(leafIndices.begin(), leafIndices.end(), [&](const auto& a) { return a == start; }), leafIndices.end());
	} // scope for temporaries

	// Upstairs
	// There is at least 1 leaf room at this point
	{ // scope for temporaries
	int rnd = randomInRange(0, leafIndices.size() - 1);
	int start = leafIndices.at(rnd);
	std::find_if(ret.begin(), ret.end(), [&](const auto& r) { return r.id == start; })->value.roomType = RoomType::UPSTAIRS;
	leafIndices.erase(std::remove_if(leafIndices.begin(), leafIndices.end(), [&](const auto& a) { return a == start; }), leafIndices.end());
	} // scope for temporaries


	// The code is shit
	int startingRoom = std::find_if(ret.begin(), ret.end(), [&](const auto& r) { return r.value.roomType == RoomType::START; })->id;
	int endingRoom = std::find_if(ret.begin(), ret.end(), [&](const auto& r) { return r.value.roomType == RoomType::UPSTAIRS; })->id;
	log::info(std::string{"Starting room: "}.append(std::to_string(startingRoom)));
	log::info(std::string{"Ending room: "}.append(std::to_string(endingRoom)));

	primaryPath = findPathBetweenRooms(mst, startingRoom, endingRoom);
	log::info("Path:");
	for(auto i : primaryPath) { log::info(std::to_string(i)); }

	// Walk through the path in reverse, sprinkling special rooms
	for(auto i = primaryPath.rbegin(); i != primaryPath.rend(); ++i) {
		auto& currentRoom = *std::find_if(ret.begin(), ret.end(), [&](const auto& r) { return r.id == *i; });
		if(currentRoom.value.roomType == RoomType::UNASSIGNED) {
			switch(d2()) {
			case 1: {
				currentRoom.value.roomType = RoomType::HYDROPONICS;
				break;
			}
			case 2: {
				currentRoom.value.roomType = RoomType::CLANDESTINE_LAB;
				break;
			}
			}
		}
	}

	// while(leafIndices.size() > 0) {
	// 	int rnd = randomInRange(0, leafIndices.size() - 1);
	// 	int start = leafIndices.at(rnd);
	// 	std::find_if(ret.begin(), ret.end(), [&](const auto& r) { return r.id == start; })->value.roomType = RoomType::COMMAND_CENTER;
	// 	leafIndices.erase(std::remove_if(leafIndices.begin(), leafIndices.end(), [&](const auto& a) { return a == start; }), leafIndices.end());
	// }

	// Make all the rest normal rooms
	for(auto& r : ret) {
		if (r.value.roomType == RoomType::UNASSIGNED) {
			r.value.roomType = RoomType::NORMAL;
		}
	}

	return ret;
}

// TODO work the data structures into such a form that the *actual* pathfinding::findPath can be used here instead of this copy-paste crap
std::vector<int> Map::findPathBetweenRooms(Graph<Room> mst, int from, int to) {
	std::vector<int> path;
	PriorityQueue<int, float> frontier;
	frontier.put(from, 0.0f);

	std::unordered_map<int, int> came_from;
	came_from[from] = from;

	std::unordered_map<int, float> cost_thus_far;
	cost_thus_far[from] = 0.0f;

	while(!frontier.empty()) {
		int current = frontier.get();

		if(current == to) {
			break;
		}

		auto currentRoom = std::find_if(mst.begin(), mst.end(), [&](const auto& r) { return r.id == current; });

		for(auto i : currentRoom->neighbours) {
			auto neighbour = std::find_if(mst.begin(), mst.end(), [&](const auto& r) { return r.id == i; });
			int next = neighbour->id;
			float nextDeltaCost = 1.0f; // Plug in cost function here
			float nextCost = cost_thus_far[current] + nextDeltaCost;
			if(cost_thus_far.find(next) == cost_thus_far.end() ||
				 nextCost < cost_thus_far[next]) {
				cost_thus_far[next] = nextCost;
				float heuristic = 1.0f; // Plug in heuristic function here
				float priority = nextCost + heuristic;
				came_from[next] = current;
				frontier.put(next, priority);
			}
		}
	}
	int current = to;

	while(current != from) {
		path.push_back(current);
		current = came_from[current];
	}
	path.push_back(from);
	std::reverse(path.begin(), path.end());
	return path;
}

bool Map::isWall(int x, int y) const {
	return !tiles(x, y).walkable;
}

bool Map::canWalk(int x, int y) const {
	if(isWall(x, y)) return false;
	if(!world->canWalk(x, y)) return false;
	return true;
}

bool Map::isExplored(int x, int y) const {
	if(x < 0 || x >= width || y < 0 || y >= height) return false; // I mean, if it's not on map, it can't be explored, right?
	return tiles(x, y).explored;
}
