#include <ogdf/basic/Graph.h>
#include <ogdf/basic/Graph_d.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/basic/EdgeArray.h>
#include <ogdf/basic/NodeArray.h>

#include <assert.h>

#include "EdgeLengthCompacter.h"
#include "VerticalEdgeLengthCompacter.h"
#include "HorizontalEdgeLengthCompacter.h"
#include "Utils.h"
#include "SpacingConstants.h"
#include "NodeCoordinates.h"
#include "EdgeCoordinates.h"

using namespace ogdf;
using namespace std;

void EdgeLengthCompacter::assignCoordinates(const Graph& g, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns, NodeArray<NodeCoordinates>& nodeCoordinates, EdgeArray<EdgeCoordinates>& edgeCoordinates, GraphAttributes& retVal){
	assignXCoordinates(nodeColumns, retVal);

	node n;
	forall_nodes(n, g){
		retVal.y(n) = (nodeCoordinates[n].y_top + nodeCoordinates[n].y_bottom) / 2;
	}
	
	edge e;
	forall_edges(e, g){
		EdgeCoordinates& ec = edgeCoordinates[e];
		
//		cout << "Edge "<< e->source() << " -> " << e->target() << endl;
//		cout << "\ty1\t"<< ec.y_1<< endl;
//		cout << "\ty2\t"<< ec.y_2<< endl;
//		cout << "\tt off\t"<< ec.x_offset_target<< endl;
//		cout << "\ts_off\t"<< ec.x_offset_source<< endl;
		
		node source = e->source();
		node target = e->target();
		
		double sourceColumn = nodeColumns[source];
		double edgeColumn = edgeColumns[e];
		double targetColumn = nodeColumns[target];
		
		double sourceX = sourceColumn * (boxWidth + boxBoxSpacing) + ec.x_offset_source;
		double targetX = targetColumn * (boxWidth + boxBoxSpacing) + ec.x_offset_target;
		double edgeX = edgeColumns[e] * (boxWidth + boxBoxSpacing);
		
		retVal.bends(e).clear();

		DPoint p0(sourceX, nodeCoordinates[source].y_bottom + 0.001);
		retVal.bends(e).pushBack(p0);

		DPoint p1(sourceX, ec.y_1);
		retVal.bends(e).pushBack(p1);

		if (sourceColumn == edgeColumns[e]){
			DPoint p2(sourceX, ec.y_1);	
			retVal.bends(e).pushBack(p2);

			DPoint p3(sourceX, ec.y_2);
			retVal.bends(e).pushBack(p3);
		} else if (targetColumn == edgeColumns[e]){
			DPoint p2(targetX, ec.y_1);	
			retVal.bends(e).pushBack(p2);

			DPoint p3(targetX, ec.y_2);			
			retVal.bends(e).pushBack(p3);
		} else {
			DPoint p2(edgeX, ec.y_1);	
			retVal.bends(e).pushBack(p2);

			DPoint p3(edgeX, ec.y_2);			
			retVal.bends(e).pushBack(p3);
		}
			
		DPoint p4(targetX, ec.y_2);	
		retVal.bends(e).pushBack(p4);

		DPoint p5(targetX, nodeCoordinates[target].y_top - 0.001);
		retVal.bends(e).pushBack(p5);
	}
}


void EdgeLengthCompacter::compact(NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns, GraphAttributes& retVal){
	assert(nodeColumns.graphOf() == edgeColumns.graphOf());
	assert(&retVal.constGraph() == edgeColumns.graphOf());
	const Graph& g = *(nodeColumns.graphOf());

	NodeArray<NodeCoordinates> nodeCoordinates(g);
	EdgeArray<EdgeCoordinates> edgeCoordinates(g);

	node n;
	forall_nodes(n, g){
		nodeCoordinates[n].height = retVal.height(n);
	}
	
	VerticalEdgeLengthCompacter velc;
	velc.compact(g, nodeColumns, edgeColumns, nodeCoordinates, edgeCoordinates);

	HorizontalEdgeLengthCompacter helc;
	helc.compact(g, nodeColumns, edgeColumns, nodeCoordinates, edgeCoordinates);
	
	assignCoordinates(g, nodeColumns, edgeColumns, nodeCoordinates, edgeCoordinates, retVal);
}
