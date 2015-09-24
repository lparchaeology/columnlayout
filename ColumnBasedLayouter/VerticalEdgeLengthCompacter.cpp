#include <ogdf/basic/Graph.h>
#include <ogdf/basic/EdgeArray.h>
#include <ogdf/basic/NodeArray.h>
#include <ogdf/basic/NodeSet.h>

#include "VerticalEdgeLengthCompacter.h"
#include "SpacingConstants.h"
#include "EdgeColumnsComparator.h"
#include "NodeCoordinates.h"
#include "EdgeCoordinates.h"
#include "Config.h"

#include <algorithm>
#include <assert.h>
#include <set>
#include <map>
#include <queue>

using namespace ogdf;
using namespace std;

void VerticalEdgeLengthCompacter::computeTransitiveHull(const Graph& g, NodeArray<NodeArray<bool> >& transitiveHull){
	transitiveHull.init(g);
	node n;
	forall_nodes(n, g){
		transitiveHull[n].init(g, false);
	}
	
	edge e;
	forall_edges(e, g){
		transitiveHull[e->source()][e->target()] = true;
	}
	
	node n1, n2, n3;
	forall_nodes(n1, g){
		forall_nodes(n2, g){
			forall_nodes(n3, g){
				if (transitiveHull[n2][n1] && transitiveHull[n1][n3])
					transitiveHull[n2][n3] = true;
			}
		}		
	}	

	// DEBUG
	/*cout << "Transitive Hull:" << endl;
	cout << "\t";

	forall_nodes(n, g){
		cout << n->index() << "\t";
	}
	cout << endl;
	 
	forall_nodes(n1, g){
		cout << n1->index() << "\t";
		forall_nodes(n2, g){
			cout << transitiveHull[n1][n2] << "\t";
		}
		cout << endl;
	}	*/
}

void VerticalEdgeLengthCompacter::computeDirectHull(const Graph& g, NodeArray<NodeArray<bool> >& transitiveHull, NodeArray<NodeArray<bool> >& directHull){
	assert(&g == transitiveHull.graphOf());
	
	directHull.init(g);
	node n;
	forall_nodes(n, g){
		directHull[n].init(g, false);
	}
	
	edge e;
	forall_edges(e, g){
		bool direct = true;
		forall_nodes(n, g){
			if (transitiveHull[e->source()][n] && transitiveHull[n][e->target()])
				direct = false;
		}
		directHull[e->source()][e->target()] = direct;
	}	
	
	// DEBUG
	/*cout << "Direct Hull:" << endl;
	cout << "\t";
	forall_nodes(n, g){
		cout << n->index() << "\t";
	}
	cout << endl;
	 
	node n1;
	node n2;
	forall_nodes(n1, g){
		cout << n1->index() << "\t";
		forall_nodes(n2, g){
			cout << directHull[n1][n2] << "\t";
		}
		cout << endl;
	}*/	
}


void VerticalEdgeLengthCompacter::computeGroups(const Graph& g, NodeArray<NodeArray<bool> >& transitiveHull, NodeArray<NodeArray<bool> >& directHull, NodeArray<NodeSet*>& groups){
	groups.init(g, NULL);

	NodeArray<bool> grouped(g, false);
	NodeArray<bool> infeasibleSucc(g, false);
	
	node n;
	forall_nodes(n, g){
		if (groups[n] != NULL)
			continue;
		
		NodeSet* group = new NodeSet(g);
		group->insert(n);
		grouped[n] = true;
		NodeSet* succs = new NodeSet(g);
		NodeSet* newSuccs = new NodeSet(g);
		edge e;
		forall_adj_edges(e, n){
			if (e->source() != n)
				continue;
			newSuccs->insert(e->target());
		}

		while (newSuccs->size() != 0) {
			NodeSet* nextSuccs = new NodeSet(g);

			for (ListConstIterator<node> it = newSuccs->nodes().begin(); it != newSuccs->nodes().end(); ++it){
				node succ = *it;
				
				if (infeasibleSucc[succ])
					continue;
				
				for (ListConstIterator<node> it2 = group->nodes().begin(); it2 != group->nodes().end(); ++it2){
					forall_adj_edges(e, succ){
						if (e->target() != succ)
							continue;
						if (!directHull[e->source()][e->target()])
							continue;

						if (transitiveHull[*it2][e->source()] || transitiveHull[e->source()][*it2]){
							infeasibleSucc[succ] = true;
							break;
						}
					}
					if (infeasibleSucc[succ])
						break;
				}
				
				if (infeasibleSucc[succ])
					continue;

				
				forall_adj_edges(e, succ){
					if (e->target() != succ)
						continue;
					if (!directHull[e->source()][e->target()])
						continue;
					
					node s = e->source();
					
					if (!grouped[s]){
						group->insert(s);
						grouped[s] = true;
						edge e2;
						forall_adj_edges(e2, s){
							if (e2->source() != s)
								continue;
							if (!succs->isMember(e2->target()) && !newSuccs->isMember(e2->target()) && !nextSuccs->isMember(e2->target()))
								nextSuccs->insert(e2->target());
						}
					}
				}
			}
				
			delete newSuccs;
			newSuccs = nextSuccs;
		}
		
		delete newSuccs;
		delete succs;
		
		for (ListConstIterator<node> it = group->nodes().begin(); it != group->nodes().end(); ++it)
			groups[*it] = group;
	}
	
	// DEBUG
	/*cout << "Groups:" << endl;
	forall_nodes(n, g){
		cout << n->index() << "\t" << groups[n] << endl;
	}*/
}

void VerticalEdgeLengthCompacter::computeGroupOrder(const Graph& g, NodeArray<NodeSet*>& groups, List<NodeSet*>& order){
	order.clear();

	map<NodeSet*, int> missing;
	node n;
	forall_nodes(n, g){
		missing[groups[n]] += n->outdeg();
	}
	
	// DEBUG
/*	cout << "Outdegree of groups" << endl;
	forall_nodes(n, g){
		cout << groups[n] << "\t" << missing[groups[n]] << endl;
	}*/
	
	queue<NodeSet*> q;
	forall_nodes(n, g){
		if (missing[groups[n]] == 0){
			q.push(groups[n]);
			break;
		}
	}
	
	while (!q.empty()){
		NodeSet* set = q.front();
		q.pop();
		
		for (ListConstIterator<node> it = set->nodes().begin(); it != set->nodes().end(); ++it){
			edge e;
			forall_adj_edges(e, *it){
				if (e->target() != *it)
					continue;
				
				missing[groups[e->source()]]--;
				
				if (missing[groups[e->source()]] == 0)
					q.push(groups[e->source()]);
			}
		}
		
		order.pushBack(set);
	}

	if (outputDebug){
		cout << "\tGroup order:" << endl;
		cout << "\t\tgroup\t\tnodes" << endl;
		for (ListConstIterator<NodeSet*> it = order.begin(); it != order.end(); ++it){
			cout << "\t\t" << *it;
			for (ListConstIterator<node> it2 = (*it)->nodes().begin(); it2 != (*it)->nodes().end(); ++it2){
				cout << "\t" << *it2;
			}
			cout << endl;
		}
		cout << endl;
	}
}

double VerticalEdgeLengthCompacter::computeMaxY(edge e, set<edge>* topMostEdges, node topMostNode, double topMostEdgesCoordinate, double topMostNodeCoordinate){
	double result = 0;
	
	if (topMostEdges != NULL){
		result = std::min(result, topMostEdgesCoordinate);
		
		// multiple edges at the sink's side
		bool add = true;
		if (topMostEdges->find(e) != topMostEdges->end())
			add = false;
		
		// multiple edges at the source's side
		bool sameSources = true;
		for (set<edge>::iterator it = topMostEdges->begin(); it != topMostEdges->end(); ++it)
			if ((*it)->source() != e->source())
				sameSources = false;
		if (sameSources)
			add = false;
		
		if (add){
			bool addSmallDistance = true;
			for (set<edge>::iterator it = topMostEdges->begin(); it != topMostEdges->end(); ++it)
				if ((*it)->source() != e->source() && (*it)->target() != e->target())
					addSmallDistance = false;
					
			if (addSmallDistance)
				result = result - edgeEdgeSpacing;
			else
				result = result - largeEdgeEdgeSpacing;			
		}
	}

	if (topMostNode != NULL)
		result = std::min(result, topMostNodeCoordinate - edgeBoxSpacing);
	
	return result;
}

double VerticalEdgeLengthCompacter::computeMaxY(node n, set<edge>* topMostEdges, node topMostNode, double topMostEdgesCoordinate, double topMostNodeCoordinate){
	double result = 0;
	
	if (topMostEdges != NULL)
		result = std::min(result, topMostEdgesCoordinate - edgeBoxSpacing);
	
	if (topMostNode != NULL)
		result = std::min(result, topMostNodeCoordinate - boxBoxSpacing);
	
	return result;
}

double VerticalEdgeLengthCompacter::computeMaxY(NodeSet* group, map<int, set<edge>* >& topMostEdges, map<int, node>& topMostNode, map<int, double>& topMostEdgesCoordinate, map<int, double>& topMostNodeCoordinate, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns){
	//cout << "Compute max y for group " << group << endl;
	
	double max = 0;
	
	for (ListConstIterator<node> it = group->nodes().begin(); it != group->nodes().end(); ++it){
		node n = *it;
		int column = nodeColumns[n];

		EdgeColumnsComparator ecc(&edgeColumns);
		vector<edge> edgeOrder;
		edge e;
		forall_adj_edges(e, n){
			if (e->source() != n)
				continue;
			edgeOrder.push_back(e);
		}
		sort(edgeOrder.begin(), edgeOrder.end(), ecc);
		
		// left out edges
		int leftCount = 0;
		for (vector<edge>::iterator it2 = edgeOrder.begin(); it2 != edgeOrder.end(); ++it2){
			e = *it2;
			
			if (edgeColumns[e] >= column)
				break;
			
			//cout << "\thandling edge " << e << endl;

			double min = 0; 
			for (int i = edgeColumns[e]; i <= column; i++){
				double v = computeMaxY(e, topMostEdges[i], topMostNode[i], topMostEdgesCoordinate[i], topMostNodeCoordinate[i]);
			//	cout << "\t\tmax y for edge " << e << " column " << i << " is " << v << endl;
				min = std::min(min, v);
			}
			
			max = std::min(max, min - edgeBoxSpacing - leftCount * edgeEdgeSpacing);
			leftCount++;
		}

		// right out edges
		int rightCount = 0;
		for (vector<edge>::reverse_iterator it2 = edgeOrder.rbegin(); it2 != edgeOrder.rend(); ++it2){
			e = *it2;
					
			if (edgeColumns[e] <= column)
				break;

			//cout << "\thandling edge " << e << endl;

			double min = 0; 
			for (int i = edgeColumns[e]; i >= column; i--){
				double v = computeMaxY(e, topMostEdges[i], topMostNode[i], topMostEdgesCoordinate[i], topMostNodeCoordinate[i]);
				//cout << "\t\tmax y for edge " << e << " column " << i << " is " << v << endl;
				min = std::min(min, v);
			}
			
			max = std::min(max, min - edgeBoxSpacing - rightCount * edgeEdgeSpacing);
			rightCount++;
		}
		
		double edgeSpacing = std::max(std::max(leftCount-1, rightCount-1), 0) * edgeEdgeSpacing;
		double v = computeMaxY(n, topMostEdges[column], topMostNode[column], topMostEdgesCoordinate[column], topMostNodeCoordinate[column]);
		//cout << "\tmax y for node " << n->index() << " is " << v << " / edge spacing is " << edgeSpacing<< endl;
		max = std::min(max, v - edgeSpacing);
	}
	
	return max;
}

void VerticalEdgeLengthCompacter::position(NodeSet* group, map<int, set<edge>* >& topMostEdges, map<int, node>& topMostNode, map<int, double>& topMostEdgesCoordinate, map<int, double>& topMostNodeCoordinate, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns, NodeArray<NodeCoordinates>& nodeCoordinates, EdgeArray<EdgeCoordinates>& edgeCoordinates){
	double y = computeMaxY(group, topMostEdges, topMostNode, topMostEdgesCoordinate, topMostNodeCoordinate, nodeColumns, edgeColumns);

	if (outputDebug)
		cout << "\t\t" << group << "\t" << y << endl;
	
	// assign coordinates to nodes
	for (ListConstIterator<node> it = group->nodes().begin(); it != group->nodes().end(); ++it){
		node n = *it;
		nodeCoordinates[n].y_bottom = y;
		nodeCoordinates[n].y_top = nodeCoordinates[n].y_bottom - nodeCoordinates[n].height;
		topMostNode[nodeColumns[n]] = n;
		topMostNodeCoordinate[nodeColumns[n]] = nodeCoordinates[n].y_top;
		if (topMostEdges[nodeColumns[n]] != NULL)
			topMostEdges[nodeColumns[n]]->clear();
	}
		
	// assign coordinates to edges
	for (ListConstIterator<node> it = group->nodes().begin(); it != group->nodes().end(); ++it){
		node n = *it;

		EdgeColumnsComparator ecc(&edgeColumns);

		// out edges		
		{
			edge e;
			vector<edge> edgeOrder;
			forall_adj_edges(e, n){
				if (e->source() != n)
					continue;
				edgeOrder.push_back(e);
			}
			sort(edgeOrder.begin(), edgeOrder.end(), ecc);
				
			// left out edges
			int leftCount = 0;
			for (int i = 0; i < edgeOrder.size(); i++){
				e = edgeOrder[i];
				if (edgeColumns[e] >= nodeColumns[n]){
					edgeCoordinates[e].y_1 = y + edgeBoxSpacing;
					edgeCoordinates[e].x_offset_source = 0;
					break;
				}
				
				double py = y + edgeBoxSpacing + leftCount * edgeEdgeSpacing;
			
				edgeCoordinates[e].y_1 = py;
				edgeCoordinates[e].x_offset_source = (i - (int)edgeOrder.size() / 2) * edgeEdgeSpacing;

				leftCount++;
				
				if (i == 0){
					for (int j = edgeColumns[e]; j <= nodeColumns[n]; j++){
						if (topMostEdges[j] == NULL)
							topMostEdges[j] = new set<edge>();
						topMostEdges[j]->clear();
						topMostEdges[j]->insert(e);
						topMostEdgesCoordinate[j] = py;
					}
				}
			}
			
			// right out edges
			int rightCount = 0;
			for (int i = edgeOrder.size()-1; i >= 0; i--){
				e = edgeOrder[i];
				if (edgeColumns[e] <= nodeColumns[n]){
					edgeCoordinates[e].y_1 = y + edgeBoxSpacing;
					edgeCoordinates[e].x_offset_source = 0;
					break;
				}
				
				double py = y + edgeBoxSpacing + rightCount * edgeEdgeSpacing;
				
				edgeCoordinates[e].y_1 = py;
				edgeCoordinates[e].x_offset_source = (i - (int)edgeOrder.size() / 2) * edgeEdgeSpacing;
								
				rightCount++;

				if (i == edgeOrder.size()-1){
					for (int j = edgeColumns[e]; j >= nodeColumns[n]; j--){
						if (topMostEdges[j] == NULL)
							topMostEdges[j] = new set<edge>();
						topMostEdges[j]->clear();
						topMostEdges[j]->insert(e);
						topMostEdgesCoordinate[j] = py;
					}
				}
			}
		}
		
		// in edges		
		{
			edge e;
			vector<edge> edgeOrder;
			forall_adj_edges(e, n){
				if (e->target() != n)
					continue;
				edgeOrder.push_back(e);
			}
			sort(edgeOrder.begin(), edgeOrder.end(), ecc);
			
			// left in edges
			set<edge> lastEdge;
			double lastEdgeCoordinate = 0;
			for (int i = 0; i < edgeOrder.size(); i++){
				e = edgeOrder[i];
				if (edgeColumns[e] >= nodeColumns[n]){
					edgeCoordinates[e].y_2 = nodeCoordinates[e->target()].y_top - edgeBoxSpacing;
					edgeCoordinates[e].x_offset_target = 0;
					break;
				}
							
				double py = computeMaxY(e, &lastEdge, topMostNode[nodeColumns[n]], lastEdgeCoordinate, topMostNodeCoordinate[nodeColumns[n]]);
				for (int j =  edgeColumns[e]; j < nodeColumns[n]; j++){
					double v = computeMaxY(e, topMostEdges[j], topMostNode[j], topMostEdgesCoordinate[j], topMostNodeCoordinate[j]);
					py = std::min(py, v);
				}

				for (int j = edgeColumns[e]; j <= nodeColumns[n]; j++){
					if (topMostEdges[j] == NULL)
						topMostEdges[j] = new set<edge>();
					if (py < topMostEdgesCoordinate[j]){
						topMostEdges[j]->clear();
						topMostEdges[j]->insert(e);
						topMostEdgesCoordinate[j] = py;
					} else if (py == topMostEdgesCoordinate[j]){
						topMostEdges[j]->insert(e);
					}
				}
				
				lastEdge.clear();
				lastEdge.insert(e);
				lastEdgeCoordinate = py;
				
				edgeCoordinates[e].y_2 = py;
				edgeCoordinates[e].x_offset_target = (i - (int)edgeOrder.size() / 2) * edgeEdgeSpacing;
			}
			
			// right in edges
			lastEdge.clear();
			lastEdgeCoordinate = 0;
			for (int i = edgeOrder.size()-1; i >= 0; i--){
				e = edgeOrder[i];
				if (edgeColumns[e] <= nodeColumns[n]){
					edgeCoordinates[e].y_2 = nodeCoordinates[e->target()].y_top - edgeBoxSpacing;
					edgeCoordinates[e].x_offset_target = 0;
					break;
				}
				
							
				double py = computeMaxY(e, &lastEdge, topMostNode[nodeColumns[n]], lastEdgeCoordinate, topMostNodeCoordinate[nodeColumns[n]]);
				for (int j = nodeColumns[n]+1; j <= edgeColumns[e]; j++){
					double v = computeMaxY(e, topMostEdges[j], topMostNode[j], topMostEdgesCoordinate[j], topMostNodeCoordinate[j]);
					py = std::min(py, v);
				}
				
				for (int j = nodeColumns[n]; j <= edgeColumns[e]; j++){
					if (topMostEdges[j] == NULL)
						topMostEdges[j] = new set<edge>();
					if (py < topMostEdgesCoordinate[j]){
						topMostEdges[j]->clear();
						topMostEdges[j]->insert(e);
						topMostEdgesCoordinate[j] = py;
					} else if (py == topMostEdgesCoordinate[j]){
						topMostEdges[j]->insert(e);
					}
				}

				lastEdge.clear();
				lastEdge.insert(e);
				lastEdgeCoordinate = py;
				
				edgeCoordinates[e].y_2 = py;
				edgeCoordinates[e].x_offset_target = (i - (int)edgeOrder.size() / 2) * edgeEdgeSpacing;
			}
			
			for (int i = 0; i < edgeOrder.size(); i++){
				if (edgeColumns[edgeOrder[i]] == nodeColumns[n]){
					if (topMostEdges[nodeColumns[n]] == NULL)
						topMostEdges[nodeColumns[n]] = new set<edge>();
					topMostEdges[nodeColumns[n]]->insert(edgeOrder[i]);
				}
			}
		}
	}
}

void VerticalEdgeLengthCompacter::compact(const Graph& g, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns, NodeArray<NodeCoordinates>& nodeCoordinates, EdgeArray<EdgeCoordinates>& edgeCoordinates){
	NodeArray<NodeArray<bool> > transitiveHull;
	computeTransitiveHull(g, transitiveHull);
	
	NodeArray<NodeArray<bool> > directHull;
	computeDirectHull(g, transitiveHull, directHull);
	
	NodeArray<NodeSet*> groups;
	computeGroups(g, transitiveHull, directHull, groups);
	
	List<NodeSet*> order;
	computeGroupOrder(g, groups, order);
	
	map<int, set<edge>* > topMostEdges;
	map<int, node> topMostNode;
	map<int, double> topMostEdgesCoordinate;
	map<int, double> topMostNodeCoordinate;
	
	if (outputDebug){
		cout << "\tAssign y coordinates:" << endl;
		cout << "\t\tgroup" << "\ty coordinate" << endl;
	}
	for (ListConstIterator<NodeSet*> it = order.begin(); it != order.end(); ++it){
		position(*it, topMostEdges, topMostNode, topMostEdgesCoordinate, topMostNodeCoordinate, nodeColumns, edgeColumns, nodeCoordinates, edgeCoordinates);	
	}
	if (outputDebug)
		cout << endl;
	
	for (ListConstIterator<NodeSet*> it = order.begin(); it != order.end(); ++it)
		delete *it;
}
