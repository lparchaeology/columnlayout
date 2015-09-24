#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/upward/UpwardPlanarizationLayout.h>

#include "UPRSupplier.h"
#include "FUPSSourceSink.h"
#include "OutputUpwardEdgeInserter.h"
#include "Config.h"

#include <queue>
#include <math.h>
#include <assert.h>

using namespace std;
using namespace ogdf;

UPRSupplier::UPRSupplier(int runs, int normalCost, int sourceSinkCost){
	this->runs = runs;
	this->normalCost = normalCost;
	this->sourceSinkCost = sourceSinkCost;
}

void UPRSupplier::supply(Graph& graph, UpwardPlanRep& retVal){
	node addedSource = graph.newNode();
	node addedSink = graph.newNode();
	if (outputDebug){
		cout << "\tAdded super source (" << addedSource->index() << ") and super sink (" << addedSink->index() << ")." << endl;
		cout << endl;
	}
	
	vector<edge> addedEdges;
	node n;
	forall_nodes(n, graph){
		if (n->indeg() == 0 && n != addedSource && n != addedSink) {
			edge e = graph.newEdge(addedSource, n);
			addedEdges.push_back(e);
		}
	}
	
	forall_nodes(n, graph){
		if (n->outdeg() == 0 && n != addedSource && n != addedSink) {
			edge e = graph.newEdge(n, addedSink);
			addedEdges.push_back(e);
		}
	}
	
	retVal.createEmpty(graph);
	assert(&retVal.original() == &graph);
	
	EdgeArray<int> cost (graph, this->normalCost);
	if (outputDebug)
		cout << "\tAssigning cost " << this->normalCost << " as default cost per edge" << endl;
	EdgeArray<bool> forbid (graph, false);
	
	for (vector<edge>::iterator it = addedEdges.begin(); it != addedEdges.end(); ++it){
		cost[*it] = this->sourceSinkCost;
		if (outputDebug)
			cout << "\tAssigning cost " << cost[*it] << " to " << *it << endl;
	}
	
	if (outputDebug)
		cout << endl;	
	
	SubgraphUpwardPlanarizer sup;
	FUPSSourceSink* fups = new FUPSSourceSink();
	
	fups->runs(this->runs);
	sup.setSubgraph(fups);
	
	sup.setInserter(new OutputUpwardEdgeInserter());
	
	sup.runs(this->runs);
	sup.call(retVal, &cost, &forbid);
}

node getSource(Graph& graph){
	node n;
	forall_nodes(n, graph){
		if (n->indeg() == 0)
			return n;
	}
	return NULL;
}


node getSink(Graph& graph){
	node n;
	forall_nodes(n, graph){
		if (n->outdeg() == 0)
			return n;
	}
	return NULL;
}

void UPRSupplier::revertGraph(Graph& graph){
	graph.delNode(getSource(graph));
	graph.delNode(getSink(graph));
}

void UPRSupplier::revertUPR(UpwardPlanRep& upr, GraphAttributes& uga){
	upr.delNode(getSource(upr));
	upr.delNode(getSink(upr));
	upr.delNode(getSink(upr));
	node source = getSource(upr);
	node sink = getSink(upr);
	
	vector<edge> edgesToDelete;
	edge e;
	forall_edges(e, upr){
		if (upr.original(e)->source() == upr.original(source) || upr.original(e)->target() == upr.original(sink))
			edgesToDelete.push_back(e);
	}

	for (vector<edge>::iterator it = edgesToDelete.begin(); it != edgesToDelete.end(); ++it){
		upr.delEdge(*it);
	}
	upr.delNode(source);
	upr.delNode(sink);
	
	vector<node> nodesToUnsplit;
	node n;
	forall_nodes(n, upr){
		if (n->indeg() == 1 && n->outdeg() == 1 && upr.isDummy(n)){
			nodesToUnsplit.push_back(n);
		}
	}
	
	for (vector<node>::iterator it = nodesToUnsplit.begin(); it != nodesToUnsplit.end(); ++it){
		edge in = (*it)->firstAdj()->theEdge();
		edge out = (*it)->lastAdj()->theEdge();
		if (in->source() == *it)
			swap(in, out);
		DPolyline& inBends = uga.bends(in);
		DPolyline& outBends = uga.bends(out);
		inBends.popBack();
		double x = inBends.back().m_x;
		int counter = 0;
		for (ListIterator<DPoint> it = outBends.begin(); it != outBends.end(); ++it){
			if (counter < 2){
				DPoint p(x, (*it).m_y);
				inBends.pushBack(p);
				counter++;
			} else {
				inBends.pushBack(*it);
			}
		}
		upr.unsplit(in, out);
	}
	
}
