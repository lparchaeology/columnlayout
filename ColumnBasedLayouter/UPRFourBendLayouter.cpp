#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/upward/UpwardPlanarizationLayout.h>

#include "Utils.h"
#include "UPRFourBendLayouter.h"
#include "BetterHeuristicLayouter.h"

#include <queue>
#include <math.h>
#include <assert.h>

using namespace std;
using namespace ogdf;

void UPRFourBendLayouter::layout(UpwardPlanRep& upr, GraphAttributes& uga){
	assert(&upr == &uga.constGraph());
	
	BetterHeuristicLayouter bhl;
	UPRNodeOrderSupplier nos(upr);
	UPREdgeOrderSupplier eos(upr);

	bhl.layout(nos, eos, uga);
}
