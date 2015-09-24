#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/upward/UpwardPlanarizationLayout.h>

#include "UPRFourBendLayouter.h"
#include "PseudoUPRFourBendLayouter.h"
#include "ArgumentMapLayouter.h"
#include "UPRLayouter.h"
#include "UPRSupplier.h"
#include "Utils.h"
#include "Config.h"
#include "SpacingConstants.h"

#include <limits.h>
#include <iostream>
#include <sstream>

using namespace std;
using namespace ogdf;

int main(int argc, const char* argv[])
{
	int runs;
	int normalCost;
	int sourceSinkCost;

	if ( argc == 9 ) 
	{
		runs = atoi(argv[1]);
		normalCost = atoi(argv[2]);
		sourceSinkCost = atoi(argv[3]);
		boxBoxSpacing = atoi(argv[4]);
		edgeBoxSpacing = atoi(argv[5]);
		edgeEdgeSpacing = atoi(argv[6]);
		largeEdgeEdgeSpacing = atoi(argv[7]);
		boxWidth = atoi(argv[8]);
		

		if (outputDebug){
			cout << " ============================== " << endl;
			cout << "       Parameter Settings       " << endl;
			cout << " ============================== " << endl;
			cout << endl;
			cout << "\truns:\t" << runs << endl;
			cout << "\tnormalCost:\t" << normalCost << endl;
			cout << "\tsourceSinkCost:\t" << sourceSinkCost << endl;
			cout << "\tboxBoxSpacing:\t" << boxBoxSpacing << endl;
			cout << "\tedgeBoxSpacing:\t" << edgeBoxSpacing << endl;
			cout << "\tedgeEdgeSpacing:\t" << edgeEdgeSpacing << endl;
			cout << "\tlargeEdgeEdgeSpacing:\t" << largeEdgeEdgeSpacing << endl;
			cout << "\tboxWidth:\t" << boxWidth << endl;
			cout << endl;
			cout << flush;
		}
	} else {
		cerr << "Wrong number of parameters!" << endl << flush;
		return 1;
	}	
	
	long attributes = GraphAttributes::nodeGraphics | GraphAttributes::edgeGraphics
	| GraphAttributes::nodeColor    | GraphAttributes::edgeColor
	| GraphAttributes::nodeStyle    | GraphAttributes::edgeStyle
	| GraphAttributes::nodeId       | GraphAttributes::nodeLabel;
	
	{
		if (outputDebug){
			cout << " ============================== " << endl;
			cout << " |                            | " << endl;
			cout << " |     ArgumentMapLayouter    | " << endl;
			cout << " |                            | " << endl;
			cout << " ============================== " << endl;
			cout << endl;
		}
		
		Graph g;
		GraphAttributes ga(g, attributes);
		ga.readGML(g, cin);
		
		ArgumentMapLayouter aml(runs, normalCost, sourceSinkCost);
		aml.layout(g, ga);
		
		ga.writeGML(cout);
	}
	
	return 0;
}