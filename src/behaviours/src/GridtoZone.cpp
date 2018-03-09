#include "GridtoZone.h"
//#include <angles/angles.h>
#include <ros/ros.h>

// needed for polygon shape
#include <geometry_msgs/PolygonStamped.h>

GridtoZone::GridtoZone() { }

GridtoZone* GridtoZone::m_pInstance = NULL;

// OK //
GridtoZone* GridtoZone::Instance(){
	if (!m_pInstance){
		m_pInstance = new GridtoZone;
	}
	return m_pInstance;
}

// OK //
void GridtoZone::setGridMap(GridMap map) {
	Eigen::Vector2d origin(0,0);
	liveMap = map;
	if(verbose){
		cout << "Behavior Map Test" << endl;
		cout << "At (0,0): "<<map.atPosition("elevation", origin) <<endl;
	}
}

int GridtoZone::countRoversInZone(int zone){
	float rovervalue = 1.0;
	getZonePosition(zone);
	return countInSection(getZonePosition(zone),zonesize,rovervalue);
}

// forgot what this is suppose to do.
bool GridtoZone::inZone(Position pos){
	return false;
}


// OK //
Position GridtoZone::getZonePosition(int zoneindex){
	double x = zonesize/2;
	double y = zonesize/2;

	int counter = 1;
	int maxcount = 1;
	int count = 0;
	bool turn = true;

	int distance = zonesize;
	// I couldn't get the other Direction from Spiral to work.
	GridDirection direction = West;

	for (int i = 0; i < zoneindex; i++){
		if(counter % 2 == 0 && counter > 0) {
			maxcount++;
			counter = 0;
		}
		switch(direction)
		{
			case North:
				y += distance;
				if(turn){
					direction = West;
					turn = false;
				}
				break;
			case East:
				x += distance;
				if(turn){
					direction = North;
					turn = false;
				}
				break;
			case South:
				y -= distance;
				if(turn){
					direction = East;
					turn = false;
				}
				break;
			case West:
				x -= distance;
				if(turn){
					direction = South;
					turn = false;
				}
				break;
			default:
				break;
		}
		count++;
		if(maxcount == count) {
			count = 0;
			counter++;
			turn = true;
		}
	}

	Position position = Position(x,y);

	if (positionverbose){
		std::cout<<  "Center of zone" << std::endl;
	    std::cout<<  "X: " << x << "  Y: " << y << std::endl;
	}

	return position;
}


void GridtoZone::updatePaperMap(){
	paperMap = liveMap;
}

int GridtoZone::countInSection(Position center, double length, float value){
	float values[] = {value};
	return countInSection(center,length,values,1);
}

int GridtoZone::countInSection(Position center, double length, float values[], int arrcount){
	double side = length / 2;

	grid_map::Polygon polygon;
	polygon.setFrameId(paperMap.getFrameId());
	// TopRight, BottomRight, TopLeft, BottomLeft
	polygon.addVertex(Position( center.x() + side, center.y() + side ));
	polygon.addVertex(Position( center.x() + side, center.y() - side ));
	polygon.addVertex(Position( center.x() - side, center.y() - side ));
	polygon.addVertex(Position( center.x() - side, center.y() + side ));

	if (positionverbose){
		std::cout<<  "Corners of zone" << std::endl;	
	    std::cout<<  "X: " << center.x() + side << "  Y: " << center.y() + side << std::endl;
	    std::cout<<  "X: " << center.x() + side << "  Y: " << center.y() - side << std::endl;
	    std::cout<<  "X: " << center.x() - side << "  Y: " << center.y() - side << std::endl;
	    std::cout<<  "X: " << center.x() - side << "  Y: " << center.y() + side << std::endl;
	}

	int count = 0;
	int count2 = 0;

	for (grid_map::PolygonIterator iterator(paperMap, polygon); !iterator.isPastEnd(); ++iterator) {
		// ask what each value is with Port to check with.
		// ask about the layer
		if (positionverbose){
			//cout << "The value at index " << (*iterator).transpose() << " is " << paperMap.at("elevation", *iterator) << endl;
		}

		float mapValue = paperMap.at("elevation", *iterator);
				count2++;

		for (int i =0; i < arrcount; i++){
			if (mapValue == values[i]){
				count++;
				break;
			}
		}
	}
	if (positionverbose){
		std::cout<<  "Total itr count" << std::endl;	
	    std::cout<<  count2 << std::endl;
	}
	return count;
}

double GridtoZone::percentInSection(Position center, double length, float values[], int arrcount){
	double side = length / 2;

	grid_map::Polygon polygon;
	polygon.setFrameId(paperMap.getFrameId());
	// TopRight, BottomRight, TopLeft, BottomLeft
	polygon.addVertex(Position( center.x() + side, center.y() + side ));
	polygon.addVertex(Position( center.x() + side, center.y() - side ));
	polygon.addVertex(Position( center.x() - side, center.y() + side ));
	polygon.addVertex(Position( center.x() - side, center.y() - side ));

	if (positionverbose){
		std::cout<<  "Corners of zone" << std::endl;	
	    std::cout<<  "X: " << center.x() + side << "  Y: " << center.y() + side << std::endl;
	    std::cout<<  "X: " << center.x() + side << "  Y: " << center.y() - side << std::endl;
	    std::cout<<  "X: " << center.x() - side << "  Y: " << center.y() + side << std::endl;
	    std::cout<<  "X: " << center.x() - side << "  Y: " << center.y() - side << std::endl;
	}

	int count = 0;
	int totalcount = 0;

	for (grid_map::PolygonIterator iterator(paperMap, polygon); !iterator.isPastEnd(); ++iterator) {
		totalcount++;

		float mapValue = paperMap.at("elevation", *iterator);

		for (int i =0; i < arrcount; i++){
			if (mapValue == values[i]){
				count++;
				break;
			}
		}
	}
	return 1.0*count/totalcount;
}

double GridtoZone::percentOfZoneExplored(int zoneindex){
	return percentOfSectionExplored(getZonePosition(zoneindex),zonesize);
}

double GridtoZone::percentOfSectionExplored(Position center, double length){
	double sectionsize = (length / celldivision) * (length / celldivision);
	double sectionlength = (length / celldivision);
	int count = countInSection(center, sectionlength, floorvalues, 3);
	count = countInSection(center, length, allvalues, 7);
	return count/sectionsize;
}

double GridtoZone::percentOfZoneDiscovered(int zoneindex){
	return percentOfSectionDiscovered(getZonePosition(zoneindex),zonesize);
}

double GridtoZone::percentOfSectionDiscovered(Position center, double length){
	double sectionsize = (length / celldivision) * (length / celldivision);
	double sectionlength = (length / celldivision);
	int count = countInSection(center, sectionlength, discorvedvalues,2);
	return count/sectionsize;
}

// Ambrosio do this
vector<Point> GridtoZone::shortestPath(Point start, Point end){
	// do something
	vector<Point> waypoints;
	return waypoints;
}


int GridtoZone::ClaimZone(int zone){
	// check if available
	zoneclaimed = zone;
	return zone;
	// else
	// return -1;
}

double GridtoZone::percentOfTest(){
	int length = 15;
	double sectionsize = (length / celldivision) * (length / celldivision);
	double sectionlength = (length / celldivision);

	float values[] = {MAT};
//	int count = percentInSection(Position(0,0),length,values,1);
	return percentInSection(Position(0,0),length,values,1);
}

int GridtoZone::countOfTest(){
	int length = 1;

	Position center = Position(0,0);

	float values[] = {MAT};

	return countInSection(center, length, MAT);
}