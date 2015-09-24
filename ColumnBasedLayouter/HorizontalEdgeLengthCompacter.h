#pragma once
#ifndef HORIZONTALEDGELENGTHCOMPACTER_H
#define HORIZONTALEDGELENGTHCOMPACTER_H

#include <ogdf/basic/EdgeArray.h>
#include <ogdf/basic/NodeArray.h>
#include <ogdf/basic/Graph.h>

#include <map>
#include <vector>

#include "NodeCoordinates.h"
#include "EdgeCoordinates.h"

using namespace ogdf;
using namespace std;

class HorizontalEdgeLengthCompacter{
private:
	
	class ColumnEntry{
	public:
		ColumnEntry(double y_top=0, double y_bottom=0){
			this->y_top = y_top;
			this->y_bottom = y_bottom;
			this->column = 0;
			this->index = 0;
		}	
		
		virtual ~ColumnEntry() {
		}
		
		double y_top;
		double y_bottom;
		int column;
		int index;
	};
	
	class NodeColumnEntry : public ColumnEntry {
	public:
		NodeColumnEntry(node n, double y_top=0, double y_bottom=0) : ColumnEntry(y_top, y_bottom){
			this->n = n;
		}	
		
		node n;
	};
	
	class EdgeColumnEntry : public ColumnEntry {
	public:
		EdgeColumnEntry(edge e, double y_top=0, double y_bottom=0) : ColumnEntry(y_top, y_bottom){
			this->e = e;
		}	
		
		edge e;
	};
	
	class RectangleColumnEntry : public ColumnEntry {
	public:
		RectangleColumnEntry(node n, double y_top=0, double y_bottom=0) : ColumnEntry(y_top, y_bottom){
			this->n = n;
		}	
		
		node n;
	};
	
	struct ColumnEntryComparator {
		bool operator() (ColumnEntry* ce1, ColumnEntry* ce2){
			if (ce1->y_top == ce2->y_top)
				return ce1->y_bottom < ce2->y_bottom;			
			else
				return ce1->y_top < ce2->y_top;
		}
	} columnEntryComparator;
	
	Graph dummyGraph;
	node dummyNode;
	edge dummyEdge;

	// general stuff
	void initColumnEntries(const Graph& g, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns, NodeArray<NodeCoordinates>& nodeCoordinates, EdgeArray<EdgeCoordinates>& edgeCoordinates, map<int, vector<ColumnEntry*> >& columnEntries);
	void getMinMaxColumn(const Graph& g, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns, int& minColumnm, int& maxColumn);
	
	//bow treatment
	void treatBows(const Graph& g, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns, NodeArray<NodeCoordinates>& nodeCoordinates, EdgeArray<EdgeCoordinates>& edgeCoordinates, map<int, vector<ColumnEntry*> >& columnEntries);
	bool isLeftBow(edge e, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns);
	bool isRightBow(edge e, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns);
	bool columnIsFree(vector<ColumnEntry*>& column, double y_top, double y_bottom);
	
	// width compaction
	bool compactWidth(const Graph& g, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns, NodeArray<NodeCoordinates>& nodeCoordinates, EdgeArray<EdgeCoordinates>& edgeCoordinates, map<int, vector<ColumnEntry*> >& columnEntries);
	void initRectsGraph(Graph& rects, node source, node sink, int minColumn, int maxColumn, NodeArray<RectangleColumnEntry*>& rectangles, map<int, vector<ColumnEntry*> >& columnEntries);
	double minSpacing(edge e1, edge e2);
	void assignIndices(vector<ColumnEntry*>& column, int columnIndex);
	bool findCompactionPath(const Graph& rects, node source, node sink, EdgeArray<bool>& visitedEdges, NodeArray<RectangleColumnEntry*>& rectangles, map<int, vector<ColumnEntry*> >& columnEntries, vector<node>& path);
	void compactAlongPath(const Graph& g, vector<EdgeColumnEntry*> path, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns, map<int, vector<ColumnEntry*> >& columnEntries);	

public:
	void compact(const Graph& g, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns, NodeArray<NodeCoordinates>& nodeCoordinates, EdgeArray<EdgeCoordinates>& edgeCoordinates);

	HorizontalEdgeLengthCompacter(){
		this->dummyNode = dummyGraph.newNode();
		this->dummyEdge = dummyGraph.newEdge(dummyNode, dummyNode);
	}

};
#endif
