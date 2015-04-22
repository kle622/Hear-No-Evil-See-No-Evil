#include "WorldGrid.h"

WorldGrid::WorldGrid(int width, int height) {
	this->width = width;
	this->height = height;
	this->grid.resize(width);
	for (int i = 0; i < grid.size(); i++)
		grid[i].resize(height);
}

void WorldGrid::add(shared_ptr<GameObject> gameObject) {
	int x = floor(gameObject.get()->position.x) + (width / 2);
	int z = floor(gameObject.get()->position.z) + (height /2);

	grid[x][z].push_back(gameObject);
}

void WorldGrid::remove(int i, int j, int k) {
	grid[i][j].erase(grid[i][j].begin() + k);
}

void WorldGrid::update() {
	for (int i = 0; i < grid.size(); i++) {
		for (int j = 0; j < grid[i].size(); j++) {
			for (int k = 0; k < grid[i][j].size(); k++) {
				shared_ptr<GameObject> temp = grid[i][j][k];
				int x = floor(temp.get()->position.x) + (width / 2);
				int z = floor(temp.get()->position.z) + (height /2);

				if (x != i || z != j &&
					x >= 0 && x <= width-1 &&
					z >= 0 && z <= height-1) {
					grid[x][z].push_back(temp);
					grid[i][j].erase(grid[i][j].begin() + k);
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
	int x = floor(gameObject.get()->position.x) + (width / 2);
	int z = floor(gameObject.get()->position.z) + (height / 2);

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