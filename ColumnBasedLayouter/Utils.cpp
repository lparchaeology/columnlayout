#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/upward/UpwardPlanarizationLayout.h>

#include "Utils.h"
#include "SpacingConstants.h"

#include <algorithm>
#include <queue>
#include <math.h>

using namespace std;
using namespace ogdf;

int getMedian(vector<int>& values){
	sort(values.begin(), values.end());
	return values[values.size()/2];
}

void carryOverGraphAttributes(GraphAttributes& original, GraphCopy& gc, GraphAttributes& retVal){
	retVal.setAllWidth(10);
	retVal.setAllHeight(10);
	
	node n;
	forall_nodes(n, gc){
		node nOrig = gc.original(n);
		if (nOrig != NULL){
			retVal.width(n) = original.width(nOrig);
			retVal.height(n) = original.height(nOrig);
			retVal.labelNode(n) = original.labelNode(nOrig);
			retVal.colorNode(n) = original.colorNode(nOrig);
			retVal.idNode(n) = original.idNode(nOrig);
			retVal.styleNode(n) = original.styleNode(nOrig);
			retVal.nodeLine(n) = original.nodeLine(nOrig);
			retVal.shapeNode(n) = original.shapeNode(nOrig);
		}
	}
	
	edge e;
	forall_edges(e, gc){
		edge eOrig = gc.original(e);
		if (eOrig != NULL){
			retVal.colorEdge(e) = original.colorEdge(eOrig);
			retVal.styleEdge(e) = original.styleEdge(eOrig);
		}
	}
}

void assignXCoordinates(NodeArray<int>& nodeColumns, GraphAttributes& retVal){
	node n;
	forall_nodes(n, retVal.constGraph()) {
		retVal.x(n) = nodeColumns[n] * (boxWidth + boxBoxSpacing);
	}
}	

void assignYCoordinates(vector<node>& insertionOrder, GraphAttributes& retVal){
	double y = 0;
	for (int i = 0; i < insertionOrder.size(); i++) {
		node n = insertionOrder.at(i);
		int outDegree = n->outdeg();
		y = y + retVal.height(n) / 2.0 + outDegree * edgeEdgeSpacing;
		retVal.y(n) = y;
		int inDegree = n->indeg();
		y = y + retVal.height(n) / 2.0 + inDegree * edgeEdgeSpacing + boxBoxSpacing;
	}
}
