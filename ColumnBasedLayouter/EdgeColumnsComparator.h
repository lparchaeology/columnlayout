#pragma once
#ifndef EDGECOLUMNSCOMPARATOR_H
#define EDGECOLUMNSCOMPARATOR_H

#include <ogdf/basic/Graph.h>

using namespace ogdf;

struct EdgeColumnsComparator{
	EdgeColumnsComparator(EdgeArray<int>* edgeColumns) {
		this->edgeColumns = edgeColumns;
	}
	
	EdgeArray<int>* edgeColumns;
	
	bool operator() (edge e1, edge e2){
		return (*edgeColumns)[e1] < (*edgeColumns)[e2];
	}
};

#endif