#pragma once
#ifndef PSEUDOUPRFOURBENDLAYOUTER_H
#define PSEUDOUPRFOURBENDLAYOUTER_H

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/upward/UpwardPlanarizationLayout.h>

#include "UPRLayouter.h"
#include "UPRFourBendLayouter.h"

using namespace std;
using namespace ogdf;

class PseudoUPRFourBendLayouter : public UPRLayouter { 
private:
	UPRFourBendLayouter fbl;
public:	
	void computeShape(UpwardPlanRep& upr, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns);
	void layout(UpwardPlanRep& upr, GraphAttributes& ga);
};

#endif