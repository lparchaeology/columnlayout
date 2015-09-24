#pragma once
#ifndef VERTICALEDGELENGTHCOMPACTER_H
#define VERTICALEDGELENGTHCOMPACTER_H

#include <ogdf/basic/EdgeArray.h>
#include <ogdf/basic/NodeArray.h>
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/Graph_d.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/basic/NodeSet.h>

#include <map>
#include <set>

#include "NodeCoordinates.h"
#include "EdgeCoordinates.h"

using namespace ogdf;
using namespace std;

class VerticalEdgeLengthCompacter{
private:
	void computeTransitiveHull(const Graph& g, NodeArray<NodeArray<bool> >& existsPath);
	void computeDirectHull(const Graph& g, NodeArray<NodeArray<bool> >& transitiveHull, NodeArray<NodeArray<bool> >& directHull);
	void computeGroups(const Graph& g, NodeArray<NodeArray<bool> >& transitiveHull, NodeArray<NodeArray<bool> >& directHull, NodeArray<NodeSet*>& groups);
	void computeGroupOrder(const Graph& g, NodeArray<NodeSet*>& groups, List<NodeSet*>& order);

	double computeMaxY(NodeSet* group, map<int, set<edge>* >& topMostEdges, map<int, node>& topMostNode, map<int, double>& topMostEdgeCoordinate, map<int, double>& topMostNodeCoordinate, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns);
	double computeMaxY(edge e, set<edge>* topMostEdges, node topMostNode, double topMostEdgeCoordinate, double topMostNodeCoordinate);
	double computeMaxY(node n, set<edge>* topMostEdges, node topMostNode, double topMostEdgeCoordinate, double topMostNodeCoordinate);
	
	void position(NodeSet* group, map<int, set<edge>* >& topMostEdges, map<int, node>& topMostNode, map<int, double>& topMostEdgeCoordinate, map<int, double>& topMostNodeCoordinate, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns, NodeArray<NodeCoordinates>& nodeCoordinates, EdgeArray<EdgeCoordinates>& edgeCoordinates);

public:
	void compact(const Graph& g, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns, NodeArray<NodeCoordinates>& nodeCoordinates, EdgeArray<EdgeCoordinates>& edgeCoordinates);
};
#endif
