#include "WorldGrid.h"

WorldGrid::WorldGrid(int width, int height,
	shared_ptr<vector<shared_ptr<GameObject>>> gameObjects) {
	this->width = width;
	this->height = height;
	this->gameObjects = gameObjects;

	for (int i = 0; i < gameObjects.get()->size(); i++) {
		int x = floor((*gameObjects.get())[i].get()->position.x + (width / 2));
		int z = floor((*gameObjects.get())[i].get()->position.z + (height / 2));

		//add game object to grid
		this->grid[x][z].push_back((*gameObjects.get())[i]);
	}
}

void WorldGrid::update() {
	for (int i = 0; i < grid.size(); i++) {
		for (int j = 0; j < grid[i].size(); j++) {
			for (int k = 0; k < grid[i][j].size(); k++) {
				shared_ptr<GameObject> temp = grid[i][j][k];
				grid[i][j].erase(grid[i][j].begin() + k);
			}
		}
	}
}

vector<shared_ptr<GameObject>> WorldGrid::getCloseObjects(
	shared_ptr<GameObject> gameObject) {
	vector<shared_ptr<GameObject>> inProximity;
	int x = floor(gameObject.get()->position.x + (width / 2));
	int z = floor(gameObject.get()->position.z + (height / 2));
	int distance = ceil(gameObject.get()->scanRadius);

	for (int i = -distance; i <= distance; i++) {
		for (int j = -distance; j <= distance; j++) {
			//get objects
			if ((x + i >= 0) && (z + j >= 0)) {
				inProximity.insert(inProximity.end(), this->grid[x+i][z+j].begin(),
					this->grid[x+i][z+j].end());
			}
		}
	}

	return inProximity;
}