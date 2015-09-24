#pragma once
#ifndef EDGEORDERSUPPLIER_H
#define EDGEORDERSUPPLIER_H

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/upward/UpwardPlanarizationLayout.h>

using namespace std;
using namespace ogdf;

class EdgeOrderSupplier {
public:
	virtual void getInEdgeOrder(node n, vector<edge>& retVal) = 0;
	virtual void getOutEdgeOrder(node n, vector<edge>& retVal) = 0;
};	


class UPREdgeOrderSupplier : public EdgeOrderSupplier {
private:
	UpwardPlanRep& upr;
	
public:
	UPREdgeOrderSupplier(UpwardPlanRep& upr) : upr(upr){
		
	}
	
	void getInEdgeOrder(node n, vector<edge>& retVal){
		adjEntry adj = upr.leftInEdge(n);
		if (adj == NULL) { 
			if (n->indeg() == 1 && n->outdeg() == 0)
				retVal.push_back(n->firstAdj()->theEdge());
			return;
		}
		
		for (int i = 0; i < n->indeg(); i++){
			retVal.push_back(adj->theEdge());
			adj = adj->cyclicPred();
		}		
	}
	
	void getOutEdgeOrder(node n, vector<edge>& retVal){
		adjEntry adj = upr.leftInEdge(n);
		if (adj == NULL){
			if (n->outdeg() == 0)
				return;
			edge e;
			forall_adj_edges(e, n){
				retVal.push_back(e);
			}
		} else {
			for (int i = 0; i < n->outdeg(); i++){
				adj = adj->cyclicSucc();
				retVal.push_back(adj->theEdge());
			}
		}		
	}	
};

class PseudoUPREdgeOrderSupplier : public EdgeOrderSupplier {
private:
	UPREdgeOrderSupplier eos;
	UpwardPlanRep& upr;
	
public:
	PseudoUPREdgeOrderSupplier(UpwardPlanRep& upr) : eos(upr), upr(upr){
		
	}
	
	void getInEdgeOrder(node n, vector<edge>& retVal){
		eos.getInEdgeOrder(upr.copy(n), retVal);
		
		for (int i = 0; i < retVal.size(); i++){
			retVal[i] = upr.original(retVal[i]);
			if (retVal[i] == NULL){
				retVal.erase(retVal.begin() + i);
				i--;
			}
		}
	}
	
	void getOutEdgeOrder(node n, vector<edge>& retVal){
		eos.getOutEdgeOrder(upr.copy(n), retVal);
		
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