#include <algorithm>
#include <cmath>
#include <limits>
#include "actor.hpp"
#include "attacker.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "destructible.hpp"
#include "dice.hpp"
#include "effect.hpp"
#include "map.hpp"
#include "map_utils.hpp"
#include "pickable.hpp"
#include "point.hpp"
#include "rect.hpp"
#include "status_effect.hpp"
#include "vec.hpp"
#include "world.hpp"

#include <SFML/Graphics/Color.hpp>

Map::Map() :
Map(constants::DEFAULT_MAP_WIDTH,
	  constants::DEFAULT_MAP_HEIGHT,
		MapType::BUILDING) { }

Map::Map(int width, int height, MapType mapType) :
width(width), height(height) {
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
	// DEBUG PRINT
	std::cout << "\n\n";
	std::cout << "MAP ROOMS:\n";
	std::cout << rooms.size() << " rooms\n";
	for(auto& room : rooms) {
		std::cout << "room " << room.id << " "
		// << (room.value.roomType == RoomType::COMMAND_CENTER ? "(command center)" : "(normal)")
		<< ": "
		<< "(" << room.value.coordinates.x0() << ", " << room.value.coordinates.y0() << "), "
		<< "(" << room.value.coordinates.x1() << ", " << room.value.coordinates.y1() << ")\n";
		std::cout << "  neighbours: ";
		for(auto& a: room.neighbours){
			std::cout << a << " ";
		}
		std::cout << "\n";
	}
	std::cout << "END MAP ROOMS\n";
	std::cout << "MAP TILES:\n";
	int x = 0;
	int y = 0;
	for(auto& tile : tiles) {
		++x;
		if(!tile.walkable) std::cout << "#";
		else {
			if(getRooms(Point {x, y}).size() == 0) {
				std::cout << ".";
			} else if(getRooms(Point {x, y}).at(0).roomType == RoomType::COMMAND_CENTER) {
				std::cout << "c";
			} else if(getRooms(Point {x, y}).at(0).roomType == RoomType::START) {
				std::cout << "s";
			} else if(getRooms(Point {x, y}).at(0).roomType == RoomType::ARMOURY) {
				std::cout << "a";
			} else if(getRooms(Point {x, y}).at(0).roomType == RoomType::MARKET) {
				std::cout << "m";
			} else {
				std::cout << ".";
			}
		}
		if(x >= width) {
			std::cout << "\n";
			x = 0;
			++y;
		}
	}
	std::cout << "END MAP TILES\n";
	std::cout << "MAP GRAPH EDGES\n";
	auto g = getEdges(rooms);
	for(auto p : g) {
		std::cout << "(" << p.first << ", " << p.second << ")\n";
	}
	std::cout << "END MAP GRAPH EDGES\n";
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

	// open doors
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
			// std::cout << "Overlaps: "
			// << "(" << overlapMinX << ", " << overlapMinY << "), "
			// << "(" << overlapMaxX << ", " << overlapMaxY << ")\n";
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
				tiles(x, y).animation = std::experimental::nullopt;
			}
		}
	}
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			if(x == 0 || x == width - 1 || y == 0 || y == height - 1) {
				tiles(x, y).walkable = false;
				tiles(x, y).animation = std::experimental::nullopt;
			}
		}
	}
}

Graph<Room> Map::breakRooms(Rect area, BreakDirection direction) {
	int minDim = 18;
	Graph<Room> areas;

	if(area.width() < minDim || area.height() < minDim) {
		areas.push_back({0, {area, RoomType::UNASSIGNED, RoomDecor::NONE}, {}});
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
			// std::cout << "overlap: "
			// << "(" << room.x0() << ", " << room.y0() << "), (" << room.x1() << ", " << room.y1() << ")"
			// << " with "
			// << "(" << other.x0() << ", " << other.y0() << "), (" << other.x1() << ", " << other.y1() << ")"
			// << "\n";
			roomNode.neighbours.emplace_back(otherNode.id);
		}
	}
	return ret;
}

// Simple (and horrible) Prim's algorithm implementation
Graph<Room> Map::pruneEdges(Graph<Room> rooms) {
	const auto src = rooms;
	Graph<Room> ret {};

	int s = randomInRange(0, rooms.size());
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
	{ // scope for temporaries
	int rnd = randomInRange(0, leafIndices.size() - 1);
	int start = leafIndices.at(rnd);
	std::find_if(ret.begin(), ret.end(), [&](const auto& r) { return r.id == start; })->value.roomType = RoomType::START;
	leafIndices.erase(std::remove_if(leafIndices.begin(), leafIndices.end(), [&](const auto& a) { return a == start; }), leafIndices.end());
	} // scope for temporaries

	// Market
	{ // scope for temporaries
	int rnd = randomInRange(0, leafIndices.size() - 1);
	int start = leafIndices.at(rnd);
	std::find_if(ret.begin(), ret.end(), [&](const auto& r) { return r.id == start; })->value.roomType = RoomType::MARKET;
	leafIndices.erase(std::remove_if(leafIndices.begin(), leafIndices.end(), [&](const auto& a) { return a == start; }), leafIndices.end());
	} // scope for temporaries

	// Armoury
	{ // scope for temporaries
	int rnd = randomInRange(0, leafIndices.size() - 1);
	int start = leafIndices.at(rnd);
	std::find_if(ret.begin(), ret.end(), [&](const auto& r) { return r.id == start; })->value.roomType = RoomType::ARMOURY;
	leafIndices.erase(std::remove_if(leafIndices.begin(), leafIndices.end(), [&](const auto& a) { return a == start; }), leafIndices.end());
	} // scope for temporaries

	// Command  centers
	for(auto& r : ret) {
		if(std::count(leafIndices.begin(), leafIndices.end(), r.id) != 0) r.value.roomType = RoomType::COMMAND_CENTER;
	}

	// Make all the rest normal rooms
	for(auto& r : ret) {
		if (r.value.roomType == RoomType::UNASSIGNED) {
			r.value.roomType = RoomType::NORMAL;
		}
	}

	return ret;
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
