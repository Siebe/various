 //path.h

#include "cell.h"
#include <math.h>

//powers of two please:
#define MAX_PATHS 512
#define MAX_PATHCELLS 2048
#define MAX_HISTORY 2048 



typedef struct {
  (dmsCell *) * pCell; //pointer towards array of pointers to original cells, this would mean the original cell would be found at *(*dmsPath->pCell + index)
  //note : this would mean total pCell will have to be cellcount*cellcount*sizeof(dmscell *)
  unsigned long cellCount;
  double minStateDiff;
  unsigned long minSateDiffCell;
}dmsPath;

typedef struct {
  dmsCell * cell; //pointer towards single original cell
  long timestamp;
}cellTime;


typedef struct dmsPathNetwork {
  dmsNetwork * network; //pointer to original network in memory
  dmsPath * path; //pointer to array of paths
  unsigned long pathCount;
  unsigned long histCount;
  cellTime * histBuffer; //pointer towards start of buffer, histBuffer+MAX_HISTORY = end of buffer
  cellTime * histWritePointer; //pointer towards current cellTime should cycle through the buffer, remove the previous entry and add a new.
  cellTime * histReadPointer; //pointer towards cellTime of x minutes ago, should "follow" writepointer from a distance and remove repeatcounts from cells
}dmsPathNetwork;

typedef dmsPathNetwork * dmsPNHandle;
typedef dmsPath * dmsPHandle;


double dmsGetStateDiff(dmsCell * cell, dmsState * state);
int dmsFreePathNetwork(dmsPathNetwork * pNet);
dmsPNHandle dmsAllocPathNetWork(dmsNetwork * net);
dmsPHandle dmsFindPath(dmsPathNetwork * pNet, dmsCell * startCell, dmsState * state);

