#pragma once
#ifndef NODECOORDINATES_H
#define NODECOORDINATES_H

#include <ogdf/basic/Graph.h>

using namespace ogdf;

class NodeCoordinates {
public:	
	NodeCoordinates(double y_top=0, double y_bottom=0, double height=0){
		this->y_top = y_top;
		this->y_bottom = y_bottom;
		this->height = height;
	}
		
	double y_top;
	double y_bottom;	
	double height;
};

#endif