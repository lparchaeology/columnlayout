#pragma once
#ifndef NODEORDERSUPPLIER_H
#define NODEORDERSUPPLIER_H

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/upward/UpwardPlanarizationLayout.h>

#include <queue>

using namespace std;
using namespace ogdf;

class NodeOrderSupplier {
public:
	virtual void getNodeOrder(vector<node>& retVal) = 0;	
};

class UPRNodeOrderSupplier : public NodeOrderSupplier {
private:
	UpwardPlanRep& upr;
	
public:
	UPRNodeOrderSupplier(UpwardPlanRep& upr) : upr(upr){
		
	}
	
	void getNodeOrder(vector<node>& retVal){
		NodeArray<int> visitedEdges;
		visitedEdges.init(upr, 0);
		
		queue<node> queue;
		queue.push(upr.getSuperSource());
		
		while (!queue.empty()){
			node n = queue.front();
			queue.pop();
			
			retVal.push_back(n);
			
			edge e;
			forall_adj_edges(e, n){
				node t = e->target();
				if(t == n)
					continue;
				visitedEdges[t]++;
				if (visitedEdges[t] == t->indeg())
					queue.push(t);
			}
		}
	}
};


class PseudoUPRNodeOrderSupplier : public NodeOrderSupplier {
private:
	UPRNodeOrderSupplier nos;
	UpwardPlanRep& upr;
	
public:
	PseudoUPRNodeOrderSupplier(UpwardPlanRep& upr) : nos(upr), upr(upr){
		
	}
	
	void getNodeOrder(vector<node>& retVal){
		nos.getNodeOrder(retVal);
	
		for (int i = 0; i < retVal.size(); i++){
			retVal[i] = upr.original(retVal[i]);
			if (retVal[i] == NULL){
				retVal.erase(retVal.begin() + i);
				i--;
			}
		}
	}
};

#endif