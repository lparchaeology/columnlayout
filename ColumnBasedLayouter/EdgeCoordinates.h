#pragma once
#ifndef EDGECOORDINATES_H
#define EDGECOORDINATES_H

#include <ogdf/basic/Graph.h>

using namespace ogdf;

class EdgeCoordinates {
public:
	EdgeCoordinates(double y_1=0, double y_2=0, double x_offset_source=0, double x_offset_target=0){
		this->y_1 = y_1;
		this->y_2 = y_2;
		this->x_offset_source = x_offset_source;
		this->x_offset_target = x_offset_target;
	}
		
	double y_1;
	double y_2;
	double x_offset_source;
	double x_offset_target;	
};


#endif