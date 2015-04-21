#ifndef WORLDGRID_H_
#define WORLDGRID_H_

#include "../GameObject/GameObject.h"
#include <vector>

using namespace std;

class WorldGrid {
	public:
		//Properties
		int width;
		int height;
		vector<int> mine;
		shared_ptr<vector<shared_ptr<GameObject>>> gameObjects;
		vector<vector<vector<shared_ptr<GameObject>>>> grid;

		//Constructor
		WorldGrid(int width, int height,
			shared_ptr<vector<shared_ptr<GameObject>>> gameObjects);

		//Object methods
		void update();
		vector<shared_ptr<GameObject>> getCloseObjects(
			shared_ptr<GameObject> gameObject);
};

#endif