#pragma once
#ifndef BETTERHEURISTICLAYOUTER_H
#define BETTERHEURISTICLAYOUTER_H

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/upward/UpwardPlanarizationLayout.h>

#include "NodeOrderSupplier.h"
#include "EdgeOrderSupplier.h"
#include "PseudoUPRFourBendLayouter.h"

using namespace std;
using namespace ogdf;

class BetterHeuristicLayouter {
friend class PseudoUPRFourBendLayouter;
private:
	void shift(int index, int leftOffset, int rightOffset, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns);
	void assignCoordinates(NodeOrderSupplier& nos, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns, GraphAttributes& retVal);	
	void computeShape(NodeOrderSupplier& nos, EdgeOrderSupplier& eos, NodeArray<int>& nodeColumns, EdgeArray<int>& edgeColumns);
		
public:
	virtual void layout(NodeOrderSupplier& nos, EdgeOrderSupplier& eos, GraphAttributes& ga);
};

#endif