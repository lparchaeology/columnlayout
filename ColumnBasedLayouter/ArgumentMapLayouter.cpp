#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/basic/EdgeArray.h>
#include <ogdf/basic/NodeArray.h>

#include "ArgumentMapLayouter.h"
#include "UPRSupplier.h"
#include "PseudoUPRFourBendLayouter.h"
#include "EdgeLengthCompacter.h"
#include "StopWatch.h"
#include "Config.h"


using namespace std;
using namespace ogdf;

ArgumentMapLayouter::ArgumentMapLayouter(int runs, int normalCost, int sourceSinkCost){
	this->runs = runs;
	this->normalCost = normalCost;
	this->sourceSinkCost = sourceSinkCost;
}

void ArgumentMapLayouter::layout(Graph& g, GraphAttributes& ga){
	// feedback arc set
	StopWatch watch;
	watch.start();
	if (outputDebug){
		cout << " ============================== " << endl;
		cout << "  Computing a Feedback Arc Set  " << endl;
		cout << " ============================== " << endl;
		cout << endl;
	}
	
	List<edge> feedbackArcSet;
	GreedyCycleRemoval gcr;
	gcr.call(g, feedbackArcSet);
	if (outputDebug)
		cout << "\tReverting " << feedbackArcSet.size() << " edges." << endl;
	forall_listiterators(edge, it, feedbackArcSet) {
		if (outputDebug)
			cout << "\t\t" << (*it)->source() << " -> " << (*it)->target() << endl;
		g.reverseEdge(*it);
	}
	if (outputDebug)
		cout << endl;

	watch.end();
	if (outputTime)
		cout << "Elapsed time for preprocessing:\t\t" << watch.getDuration() << endl;
	
	// upward planar reperesention	
	watch.start();
	if (outputDebug){
		cout << " ============================== " << endl;
		cout << "  Upward Planar Representation  " << endl;
		cout << " ============================== " << endl;
		cout << endl;
	}
	
	UpwardPlanRep upr;
	UPRSupplier supplier(this->runs, this->normalCost, this->sourceSinkCost);
	supplier.supply(g, upr);
	
	watch.end();
	if (outputTime)
		cout << "Elapsed time for topology:\t\t" << watch.getDuration() << endl;

	// shape
	watch.start();
	if (outputDebug){
		cout << " ============================== " << endl;
		cout << "       Computing the Shape      " << endl;
		cout << " ============================== " << endl;
		cout << endl;
	}
	
	NodeArray<int> nodeColumns;
	nodeColumns.init(g, 0);
	EdgeArray<int> edgeColumns;
	edgeColumns.init(g, 0);
	PseudoUPRFourBendLayouter layouter;	
	layouter.computeShape(upr, nodeColumns, edgeColumns);
	
	watch.end();
	if (outputTime)
		cout << "Elapsed time for shape:\t\t\t" << watch.getDuration() << endl;

	// compaction
	watch.start();
	if (outputDebug){
		cout << " ============================== " << endl;
		cout << "           Compaction           " << endl;
		cout << " ============================== " << endl;
		cout << endl;
	}
	EdgeLengthCompacter elc;
	elc.compact(nodeColumns, edgeColumns, ga);
	
	watch.end();
	if (outputTime)
		cout << "Elapsed time for metrics:\t\t" << watch.getDuration() << endl;
	
	// reversion
	watch.start();
	if (outputDebug){
		cout << " ============================== " << endl;
		cout << "            Reversion           " << endl;
		cout << " ============================== " << endl;
		cout << endl;
	}

	if (outputDebug)
		cout << "\t" << "Removing super source and super sink." << endl;
	supplier.revertGraph(g);
		
	if (outputDebug)
		cout << "\tRereverting " << feedbackArcSet.size() << " edges." << endl;
	forall_listiterators(edge, it, feedbackArcSet) {
		if (outputDebug)
			cout << "\t\t" << (*it)->source() << " -> " << (*it)->target() << endl;
		g.reverseEdge(*it);
		ga.bends(*it).reverse();
	}
	if (outputDebug)
		cout << endl;

	watch.end();
	if (outputTime)
		cout << "Elapsed time for postprocessing:\t" << watch.getDuration() << endl;
}
