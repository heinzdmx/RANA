

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <set>
#include <chrono>
#include <math.h>

#include "physics/phys.h"
#include "eventprocessing.h"
#include "output.h"
#include "agentengine/agents/autonLUA.h"

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::steady_clock;

EventProcessing::EventProcessing()
	:zBlocks(new QHash<QString, ZBlock*>()),
	  simInfo(new EventQueue::simInfo)

{
}

EventProcessing::~EventProcessing()
{
	delete zBlocks;
	delete simInfo;
}

void EventProcessing::resetEventProcessor()
{
	zBlocks->clear();
	eventbin.clear();
}

EventQueue::simInfo* EventProcessing::readEventInfo(std::string path)
{
	std::ifstream file(path.c_str(), std::ifstream::binary);

	if(file.is_open())
	{
		file.read(reinterpret_cast<char*>(simInfo), sizeof(EventQueue::simInfo));
		Output::Inst()->ppprintf("<b>SIMULATION INFO</b>");
		Output::Inst()->ppprintf(simInfo->luaFileName);
		Output::Inst()->ppprintf("\nHeight [m]\t:\t%f\n", simInfo->areaY);
		Output::Inst()->ppprintf( "Width [m]\t:\t%f\n", simInfo->areaX);
		Output::Inst()->ppprintf("Auton Amount\t:\t%d\n", simInfo->numberOfAutons);
		Output::Inst()->ppprintf("Event Amount\t:\t%llu\n", simInfo->eventAmount);
		Output::Inst()->ppprintf("Time Resolution\t:\t%f\n", simInfo->timeResolution);
		Output::Inst()->ppprintf("MacroFactor\t:\t%d\n", simInfo->macroFactor);
		Output::Inst()->ppprintf("Simulation runtime\t:%llu\n",simInfo->tmuAmount);

		//while(!file.eof())
		//{
			//file.read(reinterpret_cast<char*>(&devent), sizeof(EventQueue::dataEvent));
			//eventbin.push_back(devent);
		//}
	}else
	{
		Output::Inst()->ppprintf("File not found ... or worse");
	}

	file.close();

	return simInfo;
}

EventQueue::simInfo* EventProcessing::getDataEvent()
{
	return simInfo;
}

void EventProcessing::binEvents(QRegExp regex,std::string path, int to, int from)
{
	Output::Inst()->ppprintf("Binning Events from %i[s] to %i[s]",from, to);

	std::ifstream file(path.c_str(), std::ifstream::binary);

	eventbin.clear();

	if(file.is_open())
	{
		file.read(reinterpret_cast<char*>(simInfo), sizeof(EventQueue::simInfo));

		while(!file.eof())
		{
			file.read(reinterpret_cast<char*>(&devent), sizeof(EventQueue::dataEvent));
			//calculate the activation in seconds:
			int activation = devent.activationTime/simInfo->timeResolution;
			//vOutput::Inst()->ppprintf("Activation %i to %i from %i",activation,to,from);

			if(activation > from && activation < to)
			{
				QString desc = devent.desc;
				//Output::Inst()->ppprintf("%s,you never know",devent.desc);
				if(desc.contains(regex))
				{
				//Output::Inst()->ppprintf("binning event with activation %llu",devent.activationTime);
					eventbin.push_back(devent);
				}
			}
		}
	}else
		Output::Inst()->ppprintf("File not found ... or worse");

	file.close();
}

QHash<QString, ZBlock*> * EventProcessing::getZBlocks()
{
	return zBlocks;
}

void EventProcessing::processBinnedEvents(double timeResolution, std::string path,
										  double mapResolution, double thresshold)
{

	Output::Inst()->ppprintf("Processing %lu events", eventbin.size());

	int xAmount = (simInfo->areaX * simInfo->mapResolution)/mapResolution;
	int yAmount = (simInfo->areaY * simInfo->mapResolution)/mapResolution;

	//put in a timer:
	auto start = steady_clock::now();
	auto end = steady_clock::now();
	//clear the zblok map:
	QHashIterator<QString, ZBlock*> zitr(*zBlocks);
	while(zitr.hasNext())
	{
		delete zitr.value();
	}
	zBlocks->clear();

	//populate the zblok map:
	for (int x = 0; x >= xAmount; x++)
	{
		for(int y=0; y >= yAmount; y++)
		{
			ZBlock *block = new ZBlock(x,y);
			char buffer[32];
			sprintf(buffer,"%i,%i",x,y);
			//std::string key = buffer;
			//zBlocks->insert(std::pair<std::string, ZBlock>(key, block));
			//The Qt way:
			QString key = buffer;
			zBlocks->insert(key,block);
		}
	}
	Output::Inst()->ppprintf("before event");
	//process all the binned events, new c11 style better get used to it:
	for(auto it = std::begin(eventbin);
		it != std::end(eventbin) && Output::RunEventProcessing.load()==true; ++it)
	{
		processEvent(&*it, thresshold,
					 mapResolution, timeResolution, path);
	Output::Inst()->ppprintf("after event");
		end = steady_clock::now();
		if(duration_cast<milliseconds>(end-start).count() > 50)
		{
			Output::Inst()->ppprogressbar(std::distance(it,begin(eventbin)), eventbin.size());
			start = end;
		}
	}

}

void EventProcessing::processEvent(EventQueue::dataEvent *event,
								   double thresshold, double mapRes, double timeRes,
								   std::string path)
{
	Output::Inst()->ppprintf("during event, id %i ", event->originID);

	Phys::setMacroFactor(simInfo->macroFactor);
	Phys::setTimeRes(simInfo->timeResolution);

	AutonLUA *auton =
			new AutonLUA(1,1,1, 0, NULL, path);

	QSet<QString> *visited = new QSet<QString>();

	int width = simInfo->areaX/mapRes;
	int height = simInfo->areaY/mapRes;

	double z = 1;
	double duration =0;
	//calculate the z value at origin, to get thresshold value:
	//auton->processFunction(event, mapRes, event->originX, event->originY, z, duration);
	double thressholdZ = z * thresshold;

	//recursiveZlevel(auton, event, visited,event->originX, event->originY,
	//				width, height,mapRes,timeRes, thressholdZ);
	delete visited;
	delete auton;
}

void EventProcessing::recursiveZlevel(AutonLUA *auton, EventQueue::dataEvent *event,
									  QSet<QString> *visited,
									  int x, int y, int width, int height,
									  double mapRes, double timeRes, double thressholdZ)
{
	if(Output::RunEventProcessing.load() == false)
		return;

	Output::Inst()->ppprintf("This is not what it looks like");

	char buffer[32];
	sprintf(buffer,"%i,%i",x, y);
	QString key = buffer;
	//zBlocks->insert(key,block);
	//have the position been accessed

	if(visited->find(key)!=visited->end())
	{
		return;
	} else
		visited->insert(key);

	double z;
	double duration;

	auton->processFunction(event, mapRes,x*mapRes, y*mapRes,z,duration);

	double distance = sqrt( pow((event->originX - x*mapRes), 2)
							+ pow((event->originY - y*mapRes),2) );

	double arrivalTime = distance/(event->propagationSpeed*timeRes);

	//insert z value and take event duration into account:
	QHash<QString, ZBlock*>::iterator zitr = zBlocks->find(key);

	for(int i = 0; i < (int)duration*(int)timeRes; i++)
	{
		if(zitr != zBlocks->end())
		{
			zitr.value()->addZValue(z, (int)arrivalTime+i);
		}
	}
	//return if z is below thresshold:
	if (z <= thressholdZ)
		return;

	//do the recursive calls:
//	if(x+1 <= width)
//		recursiveZlevel(auton,event,visited,x+1,y,width,height,mapRes,timeRes,thressholdZ);

//	if(y+1 <= height)
//		recursiveZlevel(auton,event,visited,x,y+1,width,height,mapRes,timeRes,thressholdZ);

//	if(x-1 >= 0)
//		recursiveZlevel(auton,event,visited,x-1,y,width,height,mapRes,timeRes,thressholdZ);

//	if(y-1 >= 0)
//		recursiveZlevel(auton,event,visited,x,y-1,width,height,mapRes,timeRes,thressholdZ);

}
