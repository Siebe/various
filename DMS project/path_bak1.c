//path.c

#include "path.h"

void dmsPathRaiseError(char * error);


dmsPNHandle dmsAllocPathNetwork(dmsNetwork * net) {
  
  dmsPathnetwork * newPN;

  unsigned long cellCount;
  //unsigned long stateCount;
  
  //create a new instance of a pathnetwork, assuming that all info is availabe and net != null
  if (net == NULL) {
    dmsPathRaiseError("Network not found in memory");
    return NULL
  }

  newPN = (dmsPatNetwork *) malloc(sizeof(dmsPathNetwork));
  if (newPN == NULL) {
    dmsPathRaiseError("Unable to allocate new pathnetwork in memory");
    return NULL
  }
  
  newPN->net = net;
  newPN->histCount = MAX_HISTORY
  cellCount = net->cellCount;
  //stateCount = net->stateCount;
  
  //allocate the buffer memory
  newPN->histBuffer = calloc(newPN->histCount, sizeof(cellTime));
  if (newPN-histBuffer == NULL) {
    free(newPN);
    dmsPathRaiseError("Unable to allocate new history buffer for pathnetwork in memory");
    return NULL
  }

  //allocate a 'cellcount' number of paths (just as much paths as there are cells is absolute maximum)
  newPN->path = calloc(cellCount, sizeof(cellPath));
  if (newPN->path == NULL) {
    free(newPN->histBuffer);
    free(newPN);
    dmsPathRaiseError("Unable to allocate new path buffer for pathnetwork in memory");
    return NULL
  }  

  //allocate 'cellcount*cellcount' number of pointers for the paths
  (newPN->path)->pCell = calloc((cellCount * cellCount), sizeof(dmsCell *));
  if ((newPN->path)->pCell == NULL) {
    free(newPN->path);
    free(newPN->histBuffer);
    free(newPN);
    dmsPathRaiseError("Unable to allocate new cellpointer buffer for pathnetwork in memory");
    return NULL
  }

  //prepare the pointers for the pathCells
  for (i = 0; i < cellCount; i++) 
    (newPN->path + i)->pCell = (newPN->path)->pCell + (i * cellCount);

 //...
}

//--startup:
//  clear pathbuffer
//  open all of the successors of the start cell and create a new path for them
//  if one of the successors has the right state, just choose it and return a path
//  calculate resistance over the borders and successors themselves
//  bubble sort the paths on resistance  
//--loop till state is found:
//  start with the first path (lowest resistance), 
//  --if that path is over maximum resistance: 
//    break loop and look for lowest statediff cell create a path and return it
//  open successors of latest added cell (should be sorted on border-resistance) 
//  for each successor: 
//    if a successor hasn't been opened yet open it, else skip to next successor
//    copy (or replace) the path and add the successor to it
//    if the successor has the exact right state, then return the path and go
//    calculate resistancep over path and sort it into array
//

dmsCHandle dmsFindPathBF(dmsPathNetwork * pNet, dmsCell * startCell, dmsState * state, dmsPath * bestPath) {
  
   dmsNetWork * net = pNet->net;
   unsigned long cellCount = net->cellCount;
   unsigned long stateCount = net->cellCount;
   dmsCell * destCell;
   dmsPath * path;
   unsigned long pathCount = 0;
   
   

   for (i=0; i < startCell->exitCount; i++) {
     //get the destination cell
     exit = startCell->exit + i;
     destCell = net->cell + (exit->destination);
     //compare the state
     if (stateCompare(startCell, destCell, stateCount) > 0) {
        //create a path to put in *bestPath, if it isn't null!!
        if (bestPath != NULL) {
          bestPath->cellCount = 1;
          bestPath->currentResistance = exit->resistance;
          bestPath->minStateDiff = 0;
          bestPath->minStateDiffCell = 1;
          bestPath->cell = startCell;
          (bestPath->cell + 1) = destCell;
        }
        //found a path very quickly, beybey
        return destCell;
     }
     //too bad, no easy find, add a new path
     path = pNet->path + pathCount;
     path->cellCount
     
   
   
