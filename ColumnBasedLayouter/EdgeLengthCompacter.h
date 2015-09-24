#pragma once
#ifndef EDGELENGTHCOMPACTER_H
#define EDGELENGTHCOMPACTER_H

#include <ogdf/basic/EdgeArray.h>
#include <ogdf/basic/NodeArray.h>
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/Graph_d.h>
#include <ogdf/basic/GraphAttributes.h>

#include "NodeCoordinates.h"
#include "EdgeCoordinates.h"

using namespace ogdf;
using namespace std;

class EdgeLengthCompacter{
private:
	void assignCoordinates(const Graph& g, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns, NodeArray<NodeCoordinates>& nodeCoordinates, EdgeArray<EdgeCoordinates>& edgeCoordinates, GraphAttributes& retVal);

public:
	void compact(NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns, GraphAttributes& retVal);	
};
#endif
