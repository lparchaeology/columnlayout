#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/upward/UpwardPlanarizationLayout.h>

#include <queue>
#include <math.h>

using namespace std;
using namespace ogdf;

void assignXCoordinates(NodeArray<int>& nodeColumns, GraphAttributes& retVal);
void assignYCoordinates(vector<node>& insertionOrder, GraphAttributes& retVal);
void carryOverGraphAttributes(GraphAttributes& original, GraphCopy& gc, GraphAttributes& retVal);
int getMedian(vector<int>& values);
void deleteAuxiliaryElements(UpwardPlanRep& upr, GraphAttributes& ga, vector<node>& addedNodes);

#endif 