#ifndef WORLDGRID_H_
#define WORLDGRID_H_

#include "../GameObject/GameObject.h"
#include "../GameObject/Wall.h"

#include <cmath>
#include <vector>
#include <memory>

using namespace std;

class WorldGrid {
	public:
		//Properties
		int width;
		int height;
		vector<shared_ptr<GameObject>> list; //store list of all game objects so we don't have to iterate the grid
		vector<shared_ptr<GameObject>> wallList;
		vector<vector<vector<shared_ptr<GameObject>>>> grid;

		//Constructor
		WorldGrid(int width, int height);

		//Object methods
		void add(shared_ptr<GameObject> gameObject);
		void remove(int i, int j, int k);
		void update();
		vector<shared_ptr<GameObject>> getCloseObjects(
			shared_ptr<GameObject> gameObject);
		vector<shared_ptr<GameObject>> getCloseObjects(
			vec3 position, int scanRadius);
};

#endif