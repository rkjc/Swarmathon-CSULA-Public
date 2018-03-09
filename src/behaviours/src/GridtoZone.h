#ifndef GRIDTOZONE
#define GRIDTOZONE

#include <grid_map_ros/grid_map_ros.hpp>
#include <Eigen/Dense>
#include <vector>
#include "Point.h"

using namespace std;
using namespace grid_map;
using namespace Eigen;

class GridtoZone {

public:
	static GridtoZone* Instance();

	GridMap paperMap;

	void setGridMap(GridMap map);


	void updatePaperMap();

	Position getZonePosition(int zoneindex);


	int countInSection(Position center, double length, float value);
	int countInSection(Position center, double length, float values[], int arrcount);
	double percentOfZoneExplored(int zoneindex);


	double percentOfSectionExplored(Position center, double length);


	double percentOfZoneDiscovered(int zoneindex);

	double percentOfSectionDiscovered(Position center, double length);

// Ambrosio do this
	vector<Point> shortestPath(Point start, Point end);

	int ClaimZone(int zone);



	int countRoversInZone(int zone);
	bool inZone(Position pos);

	double percentOfTest();
	int countOfTest();
	double percentInSection(Position center, double length, float values[], int arrcount);
	enum GridDirection { North, East, South, West };


private:
	static GridtoZone* m_pInstance;

	GridtoZone();

	GridMap liveMap;
	bool verbose = false;
	bool positionverbose = true;

	int accuracy = 2;
	float zonesize = 3.5;
	int zoneclaimed = -1;

	// for a fixed number of turns
	float spiralsize = 2.0;


	// Ask Port
	string layer = "elevation";
	const float celldivision 	= 0.05;

	const float FOG 	= -1.00;
	const float REVEALED 	= 0.00;
	const float MAT 	= 0.10;
	const float CUBES	= 0.20;
	const float SONAR 	= 0.30;
	const float ROVER 	= 1.00;
	const float WALL 	= 2.00;

	float wallvalues[2] = {WALL, ROVER};
	float floorvalues[3] = {REVEALED, SONAR, MAT};
	float discorvedvalues[2] = {REVEALED, MAT};
	float allvalues[7] = {FOG, REVEALED, MAT, CUBES, SONAR, ROVER, WALL};

};
#endif // GRIDTOZONE