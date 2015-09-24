#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/upward/UpwardPlanarizationLayout.h>

#include "Utils.h"
#include "BetterHeuristicLayouter.h"
#include "SpacingConstants.h"
#include "EdgeColumnsComparator.h"
#include "Config.h"

#include <algorithm>
#include <queue>
#include <math.h>

using namespace std;
using namespace ogdf;

void BetterHeuristicLayouter::shift(int index, int leftOffset, int rightOffset, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns){
	node n;
	forall_nodes(n, *nodeColumns.graphOf()) {
		int value = nodeColumns[n];
		if (value < index)
			nodeColumns[n] = value - leftOffset;
		if (value > index)
			nodeColumns[n] = value + rightOffset;
	}
	
	edge e;
	forall_edges(e, *edgeColumns.graphOf()) {
		int value = edgeColumns[e];
		if (value < index)
			edgeColumns[e] = value - leftOffset;
		if (value > index)
			edgeColumns[e] = value + rightOffset;
	}
}

int absoluteValue(int value){
	if (value >= 0)
		return value;
	else
		return -value;
}

void BetterHeuristicLayouter::assignCoordinates(NodeOrderSupplier& nos, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns, GraphAttributes& retVal) {	
	if (outputDebug)
		cout << "Assigning the coordinates ..." << endl;
	assignXCoordinates(nodeColumns, retVal);
	
	vector<node> insertionOrder;
	nos.getNodeOrder(insertionOrder);
	assignYCoordinates(insertionOrder, retVal);		
	retVal.clearAllBends();
	
	// coordinate assignment for edges
	EdgeColumnsComparator ecc(&edgeColumns);
	
	node n;
	forall_nodes(n, retVal.constGraph()){
		vector<edge> edgeOrder;
		edge e;
		forall_edges(e, retVal.constGraph()){
			if (e->source() != n)
				continue;
			edgeOrder.push_back(e);
		}
		sort(edgeOrder.begin(), edgeOrder.end(), ecc);
		
		double sx = retVal.x(n);
		double sy = retVal.y(n) + retVal.height(n)/2.0;
		for (int i = 0; i < edgeOrder.size(); i++) {
			edge e = edgeOrder.at(i);
			double portX = (i - (int)edgeOrder.size() / 2) * edgeEdgeSpacing;
			double py = sy + edgeBoxSpacing + edgeEdgeSpacing * ((int)edgeOrder.size() / 2 - absoluteValue(i - (int)edgeOrder.size() / 2));
			
			DPoint p0(sx + portX, sy - 1);
			retVal.bends(e).pushBack(p0);
			
			DPoint p1(sx + portX, py);
			retVal.bends(e).pushBack(p1);
			
			DPoint p2(edgeColumns[e] * (boxWidth + boxBoxSpacing), py);
			retVal.bends(e).pushBack(p2);
		}
	}
	
	forall_nodes(n, retVal.constGraph()){
		vector<edge> edgeOrder;
		edge e;
		forall_edges(e, retVal.constGraph()){
			if (e->target() != n)
				continue;
			edgeOrder.push_back(e);
		}
		sort(edgeOrder.begin(), edgeOrder.end(), ecc);
		
		double tx = retVal.x(n);
		double ty = retVal.y(n) - retVal.height(n)/2.0;
		for (int i = 0; i < edgeOrder.size(); i++) {
			edge e = edgeOrder.at(i);
			double portX = (i - (int)edgeOrder.size() / 2) * edgeEdgeSpacing;
			double py = ty - edgeBoxSpacing - edgeEdgeSpacing * ((int)edgeOrder.size() / 2 - absoluteValue(i - (int)edgeOrder.size() / 2));
			
			DPoint p3(edgeColumns[e] * (boxWidth + boxBoxSpacing), py);
			retVal.bends(e).pushBack(p3);
			
			DPoint p4(tx + portX, py);
			retVal.bends(e).pushBack(p4);
			
			DPoint p5(tx + portX, ty + 1);
			retVal.bends(e).pushBack(p5);
		}
	}
	
	retVal.addNodeCenter2Bends(0);
	retVal.removeUnnecessaryBendsHV();
	if (outputDebug)
		cout << "The coordinates have been assigned." << endl;
}


void BetterHeuristicLayouter::computeShape(NodeOrderSupplier& nos, EdgeOrderSupplier& eos, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns){
	vector<node> insertionOrder;
	nos.getNodeOrder(insertionOrder);
	
	if (outputDebug)
		cout << "\tnode\tedge\t\tcolumn"<<endl;
	
	for (int i = 0; i < insertionOrder.size(); i++) {
		node currentNode = insertionOrder.at(i);
		vector<int> inEdgeColumns;
		
		edge e;
		forall_adj_edges(e, currentNode){
			if (e->target() != currentNode)
				continue;
			
			// DEBUG
			// cout << "\tEdge " << e << " is in column " << edgeColumns[e] << endl;
			
			inEdgeColumns.push_back(edgeColumns[e]);
		}
		
		if (inEdgeColumns.size() != 0) {
			nodeColumns[currentNode] = getMedian(inEdgeColumns);
		} else {
			nodeColumns[currentNode] = 0;
		}
		if (outputDebug)
			cout << "\t" << currentNode->index() << "\t\t\t" << nodeColumns[currentNode] << endl;
		
		int outDegree = currentNode->outdeg();
		if (outDegree != 0) {
			shift(nodeColumns[currentNode], outDegree / 2, (outDegree - 1) / 2, nodeColumns, edgeColumns);
			
			vector<edge> edgeOrder;
			eos.getOutEdgeOrder(currentNode, edgeOrder);
			for (int j = 0; j < edgeOrder.size(); j++) {
				edge currentEdge = edgeOrder.at(j);
				int column = nodeColumns[currentNode] + j - outDegree / 2;
				if (outputDebug)
					cout << "\t\t" << currentEdge->source()->index() << "->" << currentEdge->target()->index() << "\t\t" << column << endl;
				edgeColumns[currentEdge] = column;
			}
		}
		if (outputDebug)
			cout << endl;
	}

	// DEBUG
/*	cout << "Final columns assignment for nodes:" << endl;
	cout << "\tnode\tcolumn" << endl;
	for (int i = 0; i < insertionOrder.size(); i++)
		cout << "\t" << insertionOrder[i] << "\t" << nodeColumns[insertionOrder[i]] << endl;
	cout << endl;*/
}

void BetterHeuristicLayouter::layout(NodeOrderSupplier& nos, EdgeOrderSupplier& eos, GraphAttributes& ga){
	NodeArray<int> nodeColumns;
	nodeColumns.init(ga.constGraph(), 0);
	EdgeArray<int> edgeColumns;
	edgeColumns.init(ga.constGraph(), 0);
		
	computeShape(nos, eos, nodeColumns, edgeColumns);

	assignCoordinates(nos, nodeColumns, edgeColumns, ga);
}
