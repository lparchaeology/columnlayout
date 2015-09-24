#pragma once
#ifndef UPRSUPPLIER_H
#define UPRSUPPLIER_H

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/upward/UpwardPlanarizationLayout.h>

#include <queue>
#include <math.h>


using namespace std;
using namespace ogdf;

class UPRSupplier {
private:
	int runs;
	int normalCost;
	int sourceSinkCost;

public:
	UPRSupplier(int runs, int normalCost, int sourceSinkCost);
	void supply(Graph& graph, UpwardPlanRep& retVal);
	void revertGraph(Graph& graph);
	void revertUPR(UpwardPlanRep& upr, GraphAttributes& uga);
	
};
#endif 