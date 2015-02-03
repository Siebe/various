 //path.h

#ifndef DMS_PATH
#define DMS_PATH

#include "cell.h"
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

//power of two please:
#define MAX_HISTORY 4092 
//4092 is about 34 mins of memory if average cell-length is .5 seconds

char dmsPathError[512];

typedef struct {
  dmsCell * cell; //pointer towards single original, this would mean the original cell would be found at (pNet->pCell+index)->cell
}dmsPathCell;

typedef struct {
  dmsCell * cell; //pointer towards single original cell, (pNet->hist+index)->cell
  clock_t timeStamp;
}dmsCellTime;


typedef struct dmsPathNetwork {
  dmsNetwork * net; //pointer to original network in memory
  dmsPathCell * pCell; //pointer to array with pointers to cells that are considered for a path, this is a seperate array so it's clear what cells have been considered
  dmsPathCell * lastPath; //pointer to array with pointers to the cells of the chosen path, will start with last starcell.
  unsigned long histCount;
  dmsCellTime * histBuffer; //pointer towards start of buffer, histBuffer+MAX_HISTORY = end of buffer
  dmsCellTime * histWrite; //pointer towards current cellTime should cycle through the buffer, remove the previous entry and add a new.
  dmsCellTime * histRead; //pointer towards cellTime of x minutes ago, should "follow" writepointer from a distance and remove repeatcounts from cells
  unsigned long * currentState;
}dmsPathNetwork;

typedef dmsPathNetwork * dmsPNHandle;
typedef dmsPathCell * dmsPCHandle;

void dmsPathRaiseError(char * error);
int dmsFreePathNetwork(dmsPathNetwork * pNet);
dmsPNHandle dmsAllocPathNetwork(dmsNetwork * net);
void dmsCleanPNet(dmsPathNetwork * pNet);
dmsCHandle dmsFindPathBF(dmsPathNetwork * pNet, dmsCell * startCell, unsigned long * state);

//static inline void updateStateDiff(dmsCell * cell, dmsPathNetwork * pNet);
//static inline void updateResistance(dmsCell * cell, dmsPathNetwork * pNet);
//static inline dmsPCHandle getLowResPath(dmsPathNetwork * pNet);
//static inline dmsCHandle foundPath(dmsPathCell * pCell, dmsPathNetwork * pNet);

#endif

