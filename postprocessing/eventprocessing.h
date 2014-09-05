#ifndef EVENTPROCESSING_H
#define EVENTPROCESSING_H

#include <QtGui>
#include <vector>
#include <string>

#include "mainwindow.h"
#include "graphics/zblock.h"
#include "eventqueue.h"

class EventProcessing
{

public:
	EventProcessing();
	~EventProcessing();

	void resetEventProcessor();

	void binEvents(std::string path, int from, int to);

	EventQueue::simInfo *readEventInfo(std::string path);
	EventQueue::simInfo *getDataEvent();
	void processBinnedEvents(double timeResolution, int mapResolution, double zThresshold);

private:

	std::vector<EventQueue::dataEvent> eventbin;
	std::vector<ZBlock> *zBlocks;
	//std::unorderd_map<>
	EventQueue::simInfo *simInfo;
	EventQueue::dataEvent devent;

};

#endif // EVENTPROCESSING_H
