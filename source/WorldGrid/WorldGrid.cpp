#include "WorldGrid.h"
#define CELL_SIZE 10

WorldGrid::WorldGrid(int width, int height) {
	this->width = width;
	this->height = height;
	this->grid.resize(width / CELL_SIZE);
	for (int i = 0; i < grid.size(); i++)
		grid[i].resize(height / CELL_SIZE);
}

void WorldGrid::add(shared_ptr<GameObject> gameObject) {
	if (dynamic_cast<Wall*>(gameObject.get())) {
		static int count = 0;
		int x = (floor(gameObject.get()->position.x) + (width / 2)) / CELL_SIZE;
		int z = (floor(gameObject.get()->position.z) + (height / 2)) / CELL_SIZE;

		list.push_back(gameObject);

		int xDim = ceil(gameObject->scale.x / CELL_SIZE);
		int zDim = ceil(gameObject->scale.z / CELL_SIZE);
		for (int i = -xDim; i < xDim; i++) {
			for (int j = -zDim; j < zDim; j++) {
				grid[x+i][z+j].push_back(gameObject);
			}
		}
	}
	else {
		int x = (floor(gameObject.get()->position.x) + (width / 2)) / CELL_SIZE;
		int z = (floor(gameObject.get()->position.z) + (height / 2)) / CELL_SIZE;

		list.push_back(gameObject);
		grid[x][z].push_back(gameObject);		
	}
}

void WorldGrid::remove(int i, int j, int k) {
	for (int x = 0; x < list.size(); x++) {
		if (grid[i][j][k] == list[x]) {
			list.erase(list.begin() + x);
			break;
		}
	}
	grid[i][j].erase(grid[i][j].begin() + k);
}

void WorldGrid::remove(int index) {
    bool getOut = false;
	for (int i = 0; i < grid.size() && !getOut; i++) {
		for (int j = 0; j < grid[i].size() && !getOut; j++) {
			for (int k = 0; k < grid[i][j].size() && !getOut; k++) {
				if (list[index].get() == grid[i][j][k].get()) {
					list.erase(list.begin() + index);
                    printf("************************************************************************************\n");
					grid[i][j].erase(grid[i][j].begin() + k);
                    getOut = true;
				}
			}
		}
	}
}

void WorldGrid::update() {
	for (int i = 0; i < grid.size(); i++) {
		for (int j = 0; j < grid[i].size(); j++) {
			for (int k = 0; k < grid[i][j].size(); k++) {
				shared_ptr<GameObject> temp = grid[i][j][k];
				if (!dynamic_cast<Wall*>(temp.get())) {
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

vector<shared_ptr<GameObject>> WorldGrid::getCloseObjects(
	vec3 position, int scanRadius) {
	vector<shared_ptr<GameObject>> inProximity;
	int distance = scanRadius;
	if (distance == 0) {
		return inProximity;
	}
	int x = (floor(position.x) + (width / 2)) / CELL_SIZE;
	int z = (floor(position.z) + (height / 2)) / CELL_SIZE;

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