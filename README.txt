Instructions for columnlayout:

    Build OGDF. Release 2012.07 (Sakura) is required (http://www.ogdf.net/lib/exe/fetch.php/tech:ogdf.v2012.07.zip).

    Modify makefile to point to OGDF build.

    Run make

    Run 'columnlayout' on the commandline passing in a generic GML file using cin, the resulting layout is written to cout.

Parameters:

    NumberOfRandomisedRuns    10
    WeightCrossings            1
    WeightSourceSinkDistance   1
    BoxBoxSpacing             40
    EdgeBoxSpacing            20
    EdgeEdgeSpacing           20
    LargeEdgeEdgeSpacing       0
    BOX_WIDTH                 40

Orignal source code:

    http://sourceforge.net/p/argunet/argunet-git-repo/ci/master/tree/org.argunet.client.layout.algorithms.ColumnBasedLayouter/src_cpp/
