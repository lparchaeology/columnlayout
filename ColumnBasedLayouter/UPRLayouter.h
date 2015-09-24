#pragma once
#ifndef UPRLAYOUTER_H
#define UPRLAYOUTER_H

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/upward/UpwardPlanarizationLayout.h>

using namespace std;
using namespace ogdf;

class UPRLayouter { 

public:	
	virtual void layout(UpwardPlanRep& upr, GraphAttributes& uga) = 0;
};

#endif