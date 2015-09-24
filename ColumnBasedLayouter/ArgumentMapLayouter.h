#pragma once
#ifndef ARGUMENTMAPLAYOUTER_H
#define ARGUMENTMAPLAYOUTER_H

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>

using namespace std;
using namespace ogdf;

class ArgumentMapLayouter {		
private:
	int normalCost;
	int sourceSinkCost;
	int runs;

public:
	ArgumentMapLayouter(int runs, int normalCost, int sourceSinkCost);
	void layout(Graph& g, GraphAttributes& ga);
};

#endif