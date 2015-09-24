#pragma once
#ifndef UPRFOURBENDLAYOUTER_H
#define UPRFOURBENDLAYOUTER_H

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/upward/UpwardPlanarizationLayout.h>

#include "UPRLayouter.h"

using namespace std;
using namespace ogdf;

class UPRFourBendLayouter : public UPRLayouter { 
public:	
	void layout(UpwardPlanRep& upr, GraphAttributes& uga);
};

#endif