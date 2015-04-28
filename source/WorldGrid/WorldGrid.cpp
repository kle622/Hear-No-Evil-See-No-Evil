#include "WorldGrid.h"
#define CELL_SIZE 4

WorldGrid::WorldGrid(int width, int height) {
	this->width = width;
	this->height = height;
	this->grid.resize(width / CELL_SIZE);
	for (int i = 0; i < grid.size(); i++)
		grid[i].resize(height / CELL_SIZE);
}

void WorldGrid::add(shared_ptr<GameObject> gameObject) {
	int x = (floor(gameObject.get()->position.x) + (width / 2)) / CELL_SIZE;
	int z = (floor(gameObject.get()->position.z) + (height / 2)) / CELL_SIZE;

	list.push_back(gameObject);
	grid[x][z].push_back(gameObject);
}

void WorldGrid::remove(int i, int j, int k) {
	for (int i = 0; i < list.size(); i++) {
		if (grid[i][j][k] == list[i]) {
			list.erase(list.begin() + i);
			break;
		}
	}
	grid[i][j].erase(grid[i][j].begin() + k);
}

void WorldGrid::update() {
	for (int i = 0; i < grid.size(); i++) {
		for (int j = 0; j < grid[i].size(); j++) {
			for (int k = 0; k < grid[i][j].size(); k++) {
				shared_ptr<GameObject> temp = grid[i][j][k];
				int x = (floor(temp.get()->position.x) + (width / 2)) / CELL_SIZE;
				int z = (floor(temp.get()->position.z) + (height / 2)) / CELL_SIZE;

				if ((x != i || z != j) &&
					((x >= 0) && (x <= width-1) &&
					(z >= 0) && (z <= height-1))) {
					grid[i][j].erase(grid[i][j].begin() + k);
					grid[x][z].push_back(temp);
				}
			}
		}
	}
}

vector<shared_ptr<GameObject>> WorldGrid::getCloseObjects(
	shared_ptr<GameObject> gameObject) {
	vector<shared_ptr<GameObject>> inProximity;
	int distance = gameObject.get()->scanRadius;
	if (distance == 0) {
		return inProximity;
	}
	int x = (floor(gameObject.get()->position.x) + (width / 2)) / CELL_SIZE;
	int z = (floor(gameObject.get()->position.z) + (height / 2)) / CELL_SIZE;

	for (int i = -distance; i <= distance; i++) {
		for (int j = -distance; j <= distance; j++) {
			//get objects
			if ((x + i >= 0) && (z + j >= 0) && 
				(x + i <= width-1) && (z + j <= height-1)) {
				inProximity.insert(inProximity.end(), this->grid[x+i][z+j].begin(),
					this->grid[x+i][z+j].end());
			}
		}
	}

	return inProximity;
}