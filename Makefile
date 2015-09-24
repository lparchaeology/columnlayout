build_ColumnBasedLayouter:
	rm -f columnlayout
	g++ -I ../OGDF/ ./ColumnBasedLayouter/*.cpp -Wl,../OGDF/_release/libOGDF.a -lpthread -o columnlayout
