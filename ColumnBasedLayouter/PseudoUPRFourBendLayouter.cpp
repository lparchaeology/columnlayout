#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/upward/UpwardPlanarizationLayout.h>

#include "Utils.h"
#include "PseudoUPRFourBendLayouter.h"
#include "UPRFourBendLayouter.h"
#include "BetterHeuristicLayouter.h"

#include <queue>
#include <math.h>
#include <assert.h>

#include <iostream>
#include <sstream>

using namespace std;
using namespace ogdf;

void PseudoUPRFourBendLayouter::computeShape(UpwardPlanRep& upr, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns){
	PseudoUPRNodeOrderSupplier nos(upr);
	PseudoUPREdgeOrderSupplier eos(upr);
	BetterHeuristicLayouter bhl;
	bhl.computeShape(nos, eos, nodeColumns, edgeColumns);
}

void PseudoUPRFourBendLayouter::layout(UpwardPlanRep& upr, GraphAttributes& ga){
	assert(&upr.original() == &ga.constGraph());
	
	PseudoUPRNodeOrderSupplier nos(upr);
	PseudoUPREdgeOrderSupplier eos(upr);
	BetterHeuristicLayouter bhl;
	bhl.layout(nos, eos, ga);
}
