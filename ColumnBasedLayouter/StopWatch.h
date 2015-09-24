#ifndef STOPWATCH_H_
#define STOPWATCH_H_

#include <time.h>
#include "StopWatchUtils.h"

using namespace std;

class StopWatch{
private:
	ticks startTime, endTime;
	bool running;

	ticks getCurrentTime();
	
	unsigned convert(double c);
	
public:	
	StopWatch();
	
	void start();
	
	void end();
	
	bool isRunning();
	
	unsigned getDuration();
	
	unsigned getRunTime();
};
#endif
