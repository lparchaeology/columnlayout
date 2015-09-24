#include <time.h>
#include "StopWatch.h"
#include <iostream>

using namespace std;

ticks StopWatch::getCurrentTime(){
	return getticks();		
}

unsigned StopWatch::convert(double c){
	return c / 2796188.336;
}

StopWatch::StopWatch(){
	running = false;
}

void StopWatch::start(){
	startTime = getCurrentTime();
	running = true;
}

void StopWatch::end(){
	endTime = getCurrentTime();	
	running = false;
}

bool StopWatch::isRunning(){
	return running;
}

unsigned StopWatch::getDuration(){
	return convert(elapsed(endTime, startTime));
}

unsigned StopWatch::getRunTime(){
	return convert(elapsed(getCurrentTime(), startTime));
}	

