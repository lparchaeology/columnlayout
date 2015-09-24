#include <ogdf/basic/Graph.h>
#include <ogdf/basic/EdgeArray.h>
#include <ogdf/basic/NodeArray.h>

#include "HorizontalEdgeLengthCompacter.h"
#include "SpacingConstants.h"
#include "EdgeColumnsComparator.h"
#include "NodeCoordinates.h"
#include "EdgeCoordinates.h"
#include "Config.h"

#include <algorithm>
#include <limits.h>
#include <map>
#include <typeinfo>
#include <queue>

using namespace ogdf;
using namespace std;

// general stuff

void HorizontalEdgeLengthCompacter::initColumnEntries(const Graph& g, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns, NodeArray<NodeCoordinates>& nodeCoordinates, EdgeArray<EdgeCoordinates>& edgeCoordinates, map<int, vector<ColumnEntry*> >& columnEntries){
	node n;
	forall_nodes(n, g){
		NodeCoordinates& nc = nodeCoordinates[n];
		NodeColumnEntry* ne = new NodeColumnEntry(n, nc.y_top, nc.y_bottom);
		columnEntries[nodeColumns[n]].push_back(ne);
	}
		
	edge e;
	forall_edges(e, g){
		node source = e->source();
		node target = e->target();
		EdgeCoordinates& ec = edgeCoordinates[e];
		NodeCoordinates& sc = nodeCoordinates[source];
		NodeCoordinates& tc = nodeCoordinates[target];
		
		EdgeColumnEntry* ee;
		ee = new EdgeColumnEntry(e, sc.y_bottom, ec.y_1);
		columnEntries[nodeColumns[source]].push_back(ee);			
		
		for (int i=std::min(edgeColumns[e], nodeColumns[source])+1; i<std::max(edgeColumns[e], nodeColumns[source]); i++){
			ee = new EdgeColumnEntry(e, ec.y_1, ec.y_1);
			columnEntries[i].push_back(ee);							
		}
		
		ee = new EdgeColumnEntry(e, ec.y_1, ec.y_2);
		columnEntries[edgeColumns[e]].push_back(ee);			
		
		for (int i=std::min(edgeColumns[e], nodeColumns[target])+1; i<std::max(edgeColumns[e], nodeColumns[target]); i++){
			ee = new EdgeColumnEntry(e, ec.y_2, ec.y_2);
			columnEntries[i].push_back(ee);							
		}
			
		ee = new EdgeColumnEntry(e, ec.y_2, tc.y_top);
		columnEntries[nodeColumns[target]].push_back(ee);			
	}
	
	// sort the column entries	
	for (map<int, vector<ColumnEntry*> >::iterator it = columnEntries.begin(); it != columnEntries.end(); ++it){
		sort(it->second.begin(), it->second.end(), columnEntryComparator);
		
		// DEBUG
		/* cout << "\tEntries in column " << i << ":" << endl;
		 vector<ColumnEntry*>& column = columnEntries[i];
		 for (int j=0; j<column.size(); j++){
		 cout << "\t\t" << column[j]->y_top << "\t" << column[j]->y_bottom << endl;
		 }*/
	}
}

void HorizontalEdgeLengthCompacter::getMinMaxColumn(const Graph& g, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns, int& minColumn, int& maxColumn){
	minColumn = 0;
	maxColumn = 0;
	
	node n;
	forall_nodes(n, g){
		maxColumn = std::max(maxColumn, nodeColumns[n]);
		minColumn = std::min(minColumn, nodeColumns[n]);
	}
	
	edge e;
	forall_edges(e, g){
		maxColumn = std::max(maxColumn, edgeColumns[e]);
		minColumn = std::min(minColumn, edgeColumns[e]);
	}
}

// bow treatment

void HorizontalEdgeLengthCompacter::treatBows(const Graph& g, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns, NodeArray<NodeCoordinates>& nodeCoordinates, EdgeArray<EdgeCoordinates>& edgeCoordinates, map<int, vector<ColumnEntry*> >& columnEntries){
	if (outputDebug)
		cout << "\t\tTreatment of bows:" << endl;
	
	vector<edge> bows;
	
	// detect bows
	edge e;
	forall_edges(e, g){
		if (isLeftBow(e, nodeColumns, edgeColumns)){
			bows.push_back(e);
		}
		if (isRightBow(e, nodeColumns, edgeColumns)){
			bows.push_back(e);
		}
	}
	
	// treat bows
	for (vector<edge>::iterator it = bows.begin(); it != bows.end(); ++it){
		edge bow = *it;
		if (outputDebug)
			cout << "\t\t\tTreating bow " << bow << endl;
		EdgeCoordinates& ec = edgeCoordinates[bow];
		node source = bow->source();
		node target = bow->target();

		vector<int> indices;
		indices.push_back(nodeColumns[source]);
		indices.push_back(nodeColumns[target]);
		indices.push_back(edgeColumns[bow]);
		sort(indices.begin(), indices.end());
		int columnIndex = indices[1];
		
		if (outputDebug)
			cout << "\t\t\t\tGoal column index is " << columnIndex << endl;
		
		vector<ColumnEntry*>& column = columnEntries[columnIndex];
		if (columnIsFree(column, ec.y_1, ec.y_2)){
			if (outputDebug)
				cout << "\t\t\t\tColumn " << columnIndex << " is free." << endl;
			
			column.push_back(new EdgeColumnEntry(bow, ec.y_1, ec.y_2));
			sort(column.begin(), column.end(), columnEntryComparator);
			
			for (int i=std::min(edgeColumns[bow], columnIndex); i<std::max(edgeColumns[bow], columnIndex); i++){
				if (i == columnIndex)
					continue;
				for (int j=0; j<columnEntries[i].size(); j++){
					if (typeid(*columnEntries[i][j]) == typeid(EdgeColumnEntry) && (static_cast<EdgeColumnEntry*>(columnEntries[i][j]))->e == bow){
						columnEntries[i].erase(columnEntries[i].begin()+j);
						j--;
					}
				}
			}
			
			edgeColumns[bow] = columnIndex;
		}		
	}
	
	if (outputDebug)
		cout << endl;
}

bool HorizontalEdgeLengthCompacter::isLeftBow(edge e, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns){
	node s = e->source();
	node t = e->target();
	if (edgeColumns[e] < nodeColumns[s] && edgeColumns[e] < nodeColumns[t])
		return true;
	return false;
}

bool HorizontalEdgeLengthCompacter::isRightBow(edge e, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns){
	node s = e->source();
	node t = e->target();
	if (edgeColumns[e] > nodeColumns[s] && edgeColumns[e] > nodeColumns[t])
		return true;
	return false;
}

bool HorizontalEdgeLengthCompacter::columnIsFree(vector<ColumnEntry*>& column, double y_top, double y_bottom){
	for (int i=0; i<column.size(); i++){
		ColumnEntry* ce = column[i];
		
		if (typeid(*ce) == typeid(NodeColumnEntry)){
			if (y_top <= ce->y_top && ce->y_top < y_bottom)
				return false;
			if (y_top < ce->y_bottom && ce->y_bottom <= y_bottom)
				return false;
			if (ce->y_top <= y_top && y_bottom <= ce->y_bottom)
				return false;
		}
	}
	return true;
}

// width compaction

bool HorizontalEdgeLengthCompacter::compactWidth(const Graph& g, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns, NodeArray<NodeCoordinates>& nodeCoordinates, EdgeArray<EdgeCoordinates>& edgeCoordinates, map<int, vector<ColumnEntry*> >& columnEntries){
	// get min and max column
	int minColumn = 0;
	int maxColumn = 0;
	getMinMaxColumn(g, nodeColumns, edgeColumns, minColumn, maxColumn);
	
	// compute the boundaries of the layout
	double upperBoundary = edgeCoordinates[g.firstEdge()].y_1;
	double lowerBoundary = edgeCoordinates[g.firstEdge()].y_2;
	edge e;
	forall_edges(e, g){
		upperBoundary = std::min(upperBoundary, edgeCoordinates[e].y_1);
		lowerBoundary = std::max(lowerBoundary, edgeCoordinates[e].y_2);
	}
	
	// add dummy edges at the top and at the bottom
	EdgeColumnEntry* upperDummy = new EdgeColumnEntry(this->dummyEdge, upperBoundary - edgeBoxSpacing - edgeEdgeSpacing - largeEdgeEdgeSpacing, upperBoundary - edgeBoxSpacing - edgeEdgeSpacing - largeEdgeEdgeSpacing);
	EdgeColumnEntry* lowerDummy = new EdgeColumnEntry(this->dummyEdge, lowerBoundary + edgeBoxSpacing + edgeEdgeSpacing + largeEdgeEdgeSpacing, lowerBoundary + edgeBoxSpacing + edgeEdgeSpacing + largeEdgeEdgeSpacing);
	
	for (int i=minColumn; i<=maxColumn; i++){
		vector<ColumnEntry*>& column = columnEntries[i];
		column.insert(column.begin(), upperDummy);
		column.insert(column.end(), lowerDummy);		
	}
	
	// init data structures for compaction path computation
	
	Graph rects;
	node source = rects.newNode();
	node sink = rects.newNode();
	NodeArray<RectangleColumnEntry*> rectangles;
		
	initRectsGraph(rects, source, sink, minColumn, maxColumn, rectangles, columnEntries);
	
	// find the path along which the compaction is done
	
	EdgeArray<bool> visitedEdges(rects, false);
	vector<node> path;
	bool foundPath = findCompactionPath(rects, source, sink, visitedEdges, rectangles, columnEntries, path);
	
	if (!foundPath)
		return false;
		
	// transform rectangle node path to crossed edges path/rectangle path
	if (outputDebug)
		cout << "\t\tCompaction path:" << endl;
	vector<EdgeColumnEntry*> edgePath;
	vector<RectangleColumnEntry*> rectanglePath;
	RectangleColumnEntry* last = rectangles[*(path.begin())];
	rectanglePath.push_back(last);
	for (vector<node>::iterator it = path.begin()+1; it != path.end(); ++it){
		RectangleColumnEntry* current = rectangles[*it];
		rectanglePath.push_back(current);
		if (last->column == current->column){
			EdgeColumnEntry* ee = static_cast<EdgeColumnEntry*>(columnEntries[last->column][last->index+1]);
			
			if (ee->e != this->dummyEdge){
				edgePath.push_back(ee);
				if (outputDebug)
					cout << "\t\t\t" << ee->e->source() << "->" << ee->e->target() << "\t(column " << ee->column << ")" << endl;
			}
		}
		last = current;
	}
	if (outputDebug)
		cout << endl;		

	// compact along the path
	compactAlongPath(g, edgePath, nodeColumns, edgeColumns, columnEntries);

	return true;
	
	// DEBUG
	/*GraphAttributes ga(rects, GraphAttributes::nodeGraphics | GraphAttributes::edgeGraphics
	 | GraphAttributes::nodeColor    | GraphAttributes::edgeColor
	 | GraphAttributes::nodeStyle    | GraphAttributes::edgeStyle
	 | GraphAttributes::nodeId       | GraphAttributes::nodeLabel);
	 ga.setAllWidth(30);
	 ga.setAllHeight(30);
	 
	 ga.writeGML("rects.gml");*/
}

void HorizontalEdgeLengthCompacter::initRectsGraph(Graph& rects, node source, node sink, int minColumn, int maxColumn, NodeArray<RectangleColumnEntry*>& rectangles, map<int, vector<ColumnEntry*> >& columnEntries){
	rectangles.init(rects, NULL);
	
	for (int i=minColumn; i<=maxColumn; i++){
		vector<ColumnEntry*>& column = columnEntries[i];
		if (column.size() <= 1)
			continue;
		
		double lowest = column[0]->y_top;
		node lastRect = NULL;
		for (int j=0; j<column.size(); j++){
			ColumnEntry* current = column[j];
			
			if (current->y_top > lowest){
				node n = rects.newNode();
				// cout << "\t\tIn column " << i << " creating rectangle from y_top = " << lowest << " to y_bottom = " << current->y_top << " (node index = " << n->index() << ")" << endl;
				
				if (lastRect != NULL && typeid(*column[j-1]) == typeid(EdgeColumnEntry) && column[j-1]->y_top == column[j-1]->y_bottom && typeid(*column[j-2]) == typeid(RectangleColumnEntry)){
					rects.newEdge(lastRect, n);
					// cout << "\t\tAdded edge " << lastRect->index() << " -> " << n->index() << endl;
				}
				
				RectangleColumnEntry* re = new RectangleColumnEntry(n, lowest, current->y_top);
				rectangles[n] = re;
				column.insert(column.begin() + j, re);
				j++;
				
				lastRect = n;
			}
			
			lowest = std::max(lowest, current->y_bottom);
		}
	}	
	
	// add edges between two columns
	for (int i=minColumn; i<maxColumn; i++){
		vector<ColumnEntry*>& currentColumn = columnEntries[i];
		vector<ColumnEntry*>& nextColumn = columnEntries[i+1];
		
		int currentJ = 0;
		int nextJ = 0;
		int lastNextJ = 0;
		for (; currentJ < currentColumn.size(); currentJ++){
			if (typeid(*currentColumn[currentJ]) != typeid(RectangleColumnEntry))
				continue;
			
			RectangleColumnEntry* currentRe = static_cast<RectangleColumnEntry*> (currentColumn[currentJ]);
			
			for (; nextJ < nextColumn.size(); nextJ++){
				if (typeid(*nextColumn[nextJ]) != typeid(RectangleColumnEntry))
					continue;
				
				RectangleColumnEntry* nextRe = static_cast<RectangleColumnEntry*> (nextColumn[nextJ]);
				
				// cout << "Checking " << currentRe->n->index() << " - " << nextRe->n->index() << endl;			
				
				bool addLeftToRight = false;
				bool addRightToLeft = false;
				if (nextRe->y_top <= currentRe->y_top && currentRe->y_top < nextRe->y_bottom){			
					EdgeColumnEntry* currentBefore = static_cast<EdgeColumnEntry*> (currentColumn[currentJ-1]);
					EdgeColumnEntry* nextAfter = static_cast<EdgeColumnEntry*> (currentColumn[currentJ+1]);
					
					addLeftToRight = true;
					if (nextRe->y_bottom - currentRe->y_top >= minSpacing(currentBefore->e, nextAfter->e))
						addRightToLeft = true;					
				} else if (nextRe->y_top < currentRe->y_bottom && currentRe->y_bottom <= nextRe->y_bottom) {
					EdgeColumnEntry* currentAfter = static_cast<EdgeColumnEntry*> (currentColumn[currentJ+1]);
					EdgeColumnEntry* nextBefore = static_cast<EdgeColumnEntry*> (currentColumn[currentJ-1]);
					
					addRightToLeft = true;
					if (currentRe->y_bottom - nextRe->y_top >= minSpacing(currentAfter->e, nextBefore->e))
						addLeftToRight = true;
				} else if (currentRe->y_top <= nextRe->y_top && nextRe->y_bottom <= currentRe->y_bottom){
					addLeftToRight = true;
					addRightToLeft = true;
				} else if (nextRe->y_top >= currentRe->y_bottom) {
					nextJ = lastNextJ;
					break;				
				}
				
				if (addLeftToRight){
					edge e = rects.newEdge(currentRe->n, nextRe->n);
					lastNextJ = nextJ;
				}

				if (addRightToLeft){
					edge e = rects.newEdge(nextRe->n, currentRe->n);
					lastNextJ = nextJ;
				}
			}	
		}
	}
		
	rectangles[source] = new RectangleColumnEntry(source, INT_MIN, INT_MIN);
	rectangles[source]->column = maxColumn+1;
	rectangles[sink] = new RectangleColumnEntry(sink, INT_MAX, INT_MAX);
	rectangles[sink]->column = maxColumn+1;
	
	for (int i=minColumn; i<maxColumn; i++){
		vector<ColumnEntry*>& currentColumn = columnEntries[i];

		RectangleColumnEntry* topRectangle = static_cast<RectangleColumnEntry*> (currentColumn[1]);
		rects.newEdge(source, topRectangle->n);

		RectangleColumnEntry* bottomRectangle = static_cast<RectangleColumnEntry*> (currentColumn[currentColumn.size()-2]);
		rects.newEdge(bottomRectangle->n, sink);
	}
	
	// assign the indices to the rectangles
	for (int i=minColumn; i<=maxColumn; i++){
		assignIndices(columnEntries[i], i);
	}	
}


double HorizontalEdgeLengthCompacter::minSpacing(edge e1, edge e2){
	if (e1->source() == e2->source() || e1->target() == e2->target())
		return edgeEdgeSpacing;
	else
		return largeEdgeEdgeSpacing;
}

void HorizontalEdgeLengthCompacter::assignIndices(vector<ColumnEntry*>& column, int columnIndex){
	for (int j=0; j<column.size(); j++){
		ColumnEntry* current = column[j];
		current->column = columnIndex;
		current->index = j;
	}	
}

bool HorizontalEdgeLengthCompacter::findCompactionPath(const Graph& rects, node source, node sink, EdgeArray<bool>& visitedEdges, NodeArray<RectangleColumnEntry*>& rectangles, map<int, vector<ColumnEntry*> >& columnEntries, vector<node>& path){	
	path.clear();
	path.push_back(source);
	NodeArray<bool> visitedNodes(rects, false);

	vector<double> path_y;	
	path_y.push_back(rectangles[source]->y_top);
	
	while (path.size() > 0 && path[path.size()-1] != sink){
		node current = path[path.size()-1];
		visitedNodes[current] = true;
		int currentColumn = rectangles[current]->column;
		
		// cout <<"current = " << current->index() <<"\tcurrentColumn = " << currentColumn << endl<<flush;
	
		edge rightColumnTopMost = NULL;
		edge sameColumn = NULL;
		edge leftColumnBottomMost = NULL;
		
		edge e;
		forall_adj_edges(e, current){
			if (e->source() != current)
				continue;

			if (visitedEdges[e])
				continue;			

			if (rectangles[e->target()]->y_bottom < path_y.back()){
				continue;			
			}
						
			if (visitedNodes[e->target()]){
				visitedEdges[e] = true;
				continue;			
			}

			node target = e->target();
			
			if (target == sink){
				sameColumn = e;
			} else {
				RectangleColumnEntry* re = rectangles[target];
			
				if (re->column > currentColumn){
					if (rightColumnTopMost == NULL
					 || re->column > rectangles[rightColumnTopMost->target()]->column
					 || (re->column == rectangles[rightColumnTopMost->target()]->column
					  && re->y_top < rectangles[rightColumnTopMost->target()]->y_top))
						rightColumnTopMost = e;
				} else if (re->column == currentColumn){
					sameColumn = e;
				} else if (re->column < currentColumn){
					if (leftColumnBottomMost == NULL
					 || re->column > rectangles[leftColumnBottomMost->target()]->column
					 || (re->column == rectangles[leftColumnBottomMost->target()]->column
					  && re->y_top > rectangles[leftColumnBottomMost->target()]->y_top))
						leftColumnBottomMost = e;				
				}
			}
		}
		
		// pick right most edge
		edge rightMost = rightColumnTopMost;
		if (rightMost == NULL)
			rightMost = sameColumn;
		if (rightMost == NULL)
			rightMost = leftColumnBottomMost;
		
		// step back if there is no free edge
		if (rightMost == NULL){
			visitedNodes[path.back()] = false;
			path.erase(path.end()-1);
			path_y.erase(path_y.end()-1);
			continue;
		}

		// cout << "right most edge " << rightMost << endl;
		
		// otherwise, use the edge
		visitedEdges[rightMost] = true;
		path.push_back(rightMost->target());
		path_y.push_back(std::max(path_y.back(), rectangles[rightMost->target()]->y_top));
	}
	return path.size() > 0;
}

void HorizontalEdgeLengthCompacter::compactAlongPath(const Graph& g, vector<EdgeColumnEntry*> path, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns, map<int, vector<ColumnEntry*> >& columnEntries){
	EdgeArray<bool> visitedEdges(g, false);
	NodeArray<bool> visitedNodes(g, false);
	int pathLength = path.size();
	
	queue<node> nodes;
	
	// handle edges on the cut
	
	for (int j=0; j<pathLength; j++){
		EdgeColumnEntry* ee = path[j];
		if (edgeColumns[ee->e] > ee->column){
			edgeColumns[ee->e]--;
		}
		visitedEdges[ee->e] = true;
		
		node s = ee->e->source();
		node t = ee->e->target();
		
		if (nodeColumns[s] > ee->column)
			nodes.push(s);
		else
			nodes.push(t);
	}
	
	// handle edges on the right side of the cut
	while (!nodes.empty()){
		node current = nodes.front();			
		nodes.pop();
		
		if (visitedNodes[current] == false){
			visitedNodes[current] = true;
			nodeColumns[current]--;
			edge e;
			forall_adj_edges(e, current){
				if (visitedEdges[e] == false){
					visitedEdges[e] = true;
					edgeColumns[e]--;
					nodes.push(e->opposite(current));
				}
			}
		}
	}
}

void HorizontalEdgeLengthCompacter::compact(const Graph& g, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns, NodeArray<NodeCoordinates>& nodeCoordinates, EdgeArray<EdgeCoordinates>& edgeCoordinates){
	if (outputDebug)
		cout << "\tCompacting width:" << endl;
	
	int minColumn;
	int maxColumn;
	
	getMinMaxColumn(g, nodeColumns, edgeColumns, minColumn, maxColumn);
	
	for (int i = minColumn; i<=maxColumn; i++){
		// init column entries
		map<int, vector<ColumnEntry*> > columnEntries;
		initColumnEntries(g, nodeColumns, edgeColumns, nodeCoordinates, edgeCoordinates, columnEntries);
	
		if (i == minColumn){
			//bow treatment
			treatBows(g, nodeColumns, edgeColumns, nodeCoordinates, edgeCoordinates, columnEntries);
		}
	
		// min cost width compaction
		bool compacted = compactWidth(g, nodeColumns, edgeColumns, nodeCoordinates, edgeCoordinates, columnEntries);	
		if (!compacted)
			return;
	}
}
