//path.c

#include "path.h"

#define DMS_P_DEBUG

#ifdef DMS_P_DEBUG
#include "stdio.h"
#endif

void dmsPathRaiseError(char * error) {
  strncpy((char *)&dmsPathError, error, 512);
}


int dmsFreePathNetwork(dmsPathNetwork * pNet) { 
  if (pNet == NULL)
    return 0;
  free(pNet->currentState);
  free(pNet->lastPath);
  free(pNet->pCell);
  free(pNet->histBuffer);
  free(pNet);
  return 1;
}

dmsPNHandle dmsAllocPathNetwork(dmsNetwork * net) {
  
  dmsPathNetwork * newPN;

  unsigned long cellCount;
  unsigned long stateCount; 
  
  //create a new instance of a pathnetwork, assuming that all info is availabe if != null
  if (net == NULL) {
    dmsPathRaiseError("Network not found in memory");
    return NULL;
  }

  newPN = (dmsPathNetwork *) malloc(sizeof(dmsPathNetwork));
  if (newPN == NULL) {
    dmsPathRaiseError("Unable to allocate new pathnetwork in memory");
    return NULL;
  }
  
  newPN->net = net;
  newPN->histCount = MAX_HISTORY;
  cellCount = net->cellCount;
  stateCount = net->stateCount;
  
  //allocate the buffer memory
  newPN->histBuffer = calloc(newPN->histCount, sizeof(dmsCellTime));
  if (newPN->histBuffer == NULL) {
    free(newPN);
    dmsPathRaiseError("Unable to allocate new history buffer for pathnetwork in memory");
    return NULL;
  }
  newPN->histWrite = newPN->histRead = newPN->histBuffer;

  //allocate a 'cellCount'+1 number of pathsCells, +1 to ensure the array allways ends with a null pointer
  newPN->pCell= calloc(cellCount + 1, sizeof(dmsPathCell));
  if (newPN->pCell == NULL) {
    free(newPN->histBuffer);
    free(newPN);
    dmsPathRaiseError("Unable to allocate new pathCell buffer for pathnetwork in memory");
    return NULL;
  }

  //allocate another 'cellCount'+1 number of pathsCells for the actual found paht, +1 to ensure the array allways ends with a null pointer
  newPN->lastPath= calloc(cellCount + 1, sizeof(dmsPathCell));
  if (newPN->lastPath == NULL) {
    free(newPN->pCell);
    free(newPN->histBuffer);
    free(newPN);
    dmsPathRaiseError("Unable to allocate new lastPath buffer for pathnetwork in memory");
    return NULL;
  }

  //allocate currentState memory
  newPN->currentState = calloc(stateCount, sizeof(unsigned long));
  if (newPN->currentState == NULL) {
    free(newPN->lastPath); 
    free(newPN->pCell);
    free(newPN->histBuffer);
    free(newPN);
    dmsPathRaiseError("Unable to allocate new currentState buffer for pathnetwork in memory");
    return NULL;
  }

  return newPN;
}


void dmsCleanPNet(dmsPathNetwork * pNet) {
   
   dmsNetwork * net = pNet->net;
   dmsCell * cell;
   dmsPathCell * pCell;
   unsigned long cellCount = (pNet->net)->cellCount;
   unsigned long i;
   dmsCellTime * hist;
   time_t now = time(NULL);

   //update the history, get current clock_t and keep removing anything that's too old, stop at writepointer or first cell that is recent enough
   hist = pNet->histRead;
   while ((hist != pNet->histWrite) && 
          (hist->timeStamp < (now - (long)(net->globalRepeatTime)))) {
     if ((hist->cell)->globalRepeat > 0)
       (hist->cell)->globalRepeat--;

#ifdef DMS_P_DEBUG  
     printf("cleaning history, cell, timestamp, now-globalrepeat: %s %lu %ld\n", (hist->cell)->name, hist->timeStamp, now - (long)(net->globalRepeatTime)); 
#endif

     hist++;
     //circular buffer:
     if (hist >= pNet->histBuffer + pNet->histCount)
       hist = pNet->histBuffer;
   }
   pNet->histRead = hist;

   //clear lastpath
   for (i=0; i < cellCount; i++) {
     if ((pNet->lastPath + i)->cell == NULL)
       break;
     (pNet->lastPath + i)->cell = NULL; 
   }

   //set stateUpdated and resistanceUpdated to 0 for all cells in pCell array and set all pCell to NULL
   for (i=0; i < cellCount; i++) {
     pCell = pNet->pCell + i ;    
     if (pCell->cell == NULL)
       break;
     cell = pCell->cell;
     cell->pathLength = 0;
     cell->pathOpen = 0;
     cell->stateDiffUpdated = 0;
     cell->resistanceUpdated = 0;
     cell->backTrace = NULL;
     pCell->cell = NULL;
   }
}


static inline void updateStateDiff(dmsCell * cell, dmsPathNetwork * pNet) {
  dmsNetwork * net = pNet->net;
  unsigned long diff = 0;
  unsigned long i;
 
  if (cell->stateDiffUpdated == 0) {
    for (i=0; i < net->stateCount; i++) {
        diff += labs(*(cell->state + i) - *(pNet->currentState + i));
    }
    cell->stateDiff = diff;
    cell->stateDiffUpdated = 1;
  }   
}


static inline void updateResistance(dmsCell * cell, dmsPathNetwork * pNet) {
//if not resistanceUpdated in cell
//check globalrepeat shitjes
//do updateStateDiff, change stateDiff to resistanceus
//add stateresistance and globalrepresistance to borderresistance <-- happens OUTSIDE this function!
  dmsNetwork * net = pNet->net;
  unsigned long resistance;
  double stateAbs;

  if (cell->resistanceUpdated == 0) {
    //update globalrepeat resistance
    resistance = cell->globalRepeat * cell->globalRepeatResistance;
    //add stateDiff resistance
    updateStateDiff(cell, pNet);
    if (cell->stateDiff > 0)
      stateAbs = pow((double)(cell->stateDiff), net->stateExponent);
    resistance += (unsigned long)(stateAbs * (cell->exit)->time + 0.5);
    cell->currentResistance = resistance;
    cell->resistanceUpdated = 1;

#ifdef DMS_P_DEBUG  
    printf("cell resistance updated, cell name, new resistance: %s %lu\n", cell->name, resistance); 
#endif

  }
    
}


static inline dmsPCHandle getLowResPath(dmsPathNetwork * pNet) {
  //find open cell with lowest totalresistance, skip the starcell, if none is found, return NULL
  dmsNetwork * net = pNet->net;
  unsigned long i;
  dmsPathCell * pCell;
  dmsPathCell * lowest = NULL;

#ifdef DMS_P_DEBUG  
   printf("looking for lowest open pathcell: "); 
#endif

  for (i=1; i < net->cellCount; i++) {
    pCell = pNet->pCell + i;
    //break at end of pathcell array
    if (pCell->cell == NULL)
      break;
    if ((pCell->cell)->pathOpen > -1) {
      if (lowest == NULL)
        lowest = pCell;   
      else if ((pCell->cell)->totalResistance < (lowest->cell)->totalResistance)
        lowest = pCell;
    }
  }

#ifdef DMS_P_DEBUG
  if (lowest == NULL)
    printf ("not found!\n");
  else 
    printf ("%s\n", (lowest->cell)->name);
#endif

  return lowest;
}


static inline dmsCHandle foundPath(dmsPathCell * pCell, dmsPathNetwork * pNet) {
//add globalrepeat and maxrepeat if needed, update history if needed, return SECOMD cell in path (first is startcell)
  //dmsNetwork * net = pNet->net;
  if (pCell->cell == NULL)
    return NULL;  
 
  dmsCell * cell = pCell->cell;
  unsigned long count = cell->pathLength;
  unsigned long i;

  //backwards loop with i < cellCount, this is because i is unsigned, so it will never be less than 0
  for (i=count-1; i < (pNet->net)->cellCount; i--) {
    ((pNet->lastPath) + i)->cell = cell;
    if (cell->backTrace != NULL)
      cell = (dmsCell *) cell->backTrace;
  }

  if ((pNet->lastPath + 1)->cell == NULL)
    return NULL;
 
  cell = (pNet->lastPath + 1)->cell;
  
  //update our history and add globalrepeat and maxrepeat
  time_t now = time(NULL);

  (pNet->histWrite)->cell = cell;
  (pNet->histWrite)->timeStamp = now;
  pNet->histWrite++;
  //circular buffer
  if (pNet->histWrite > (pNet->histBuffer + pNet->histCount)) 
    pNet->histWrite = pNet->histBuffer;

  cell->globalRepeat++;
  cell->selfRepeat++;

  return cell;

}

//get the cell with the lowest statediff in our current path
static inline dmsPCHandle getBestPCell(dmsPathNetwork *pNet) {

  //skip the start cell, just do a null pointer check in case of a single cell network
  dmsPathCell * pCell = (pNet->pCell + 1);
  if (pCell == NULL)
    return pNet->pCell;
  dmsPathCell * bestPCell = pCell;
  dmsCell * cell = pCell->cell;
  dmsCell * bestCell = cell;
  unsigned long i;

#ifdef DMS_P_DEBUG  
   printf("perfect cell not found, looking for lowest stateDiff\n"); 
#endif

  //iterate all pCells to find the lowest stateDiff at the lowest cost path, 
  for (i = 0; i < (pNet->net)->cellCount; i++) {
    pCell++;
    //if we find a null pointer then we are at the end of the pCell list, we can return bestCell now
    if (pCell->cell == NULL)
      break;
    cell = pCell->cell;
    //so if stateDiff is lower OR stateDiff is the same and pathcost is lower set this cell as bestCell
    if ((cell->stateDiff < bestCell->stateDiff) ||
       ((cell->stateDiff == bestCell->stateDiff) && (cell->totalResistance < bestCell->totalResistance))) { 
      bestPCell = pCell;
      bestCell = pCell->cell;
    } 
  }//end of for loop finding bestCell
  return bestPCell;
}

//the main pathfinding algorithm

dmsCHandle dmsFindPathBF(dmsPathNetwork * pNet, dmsCell * startCell, unsigned long * state) {
  
  dmsNetwork * net = pNet->net;
  unsigned long cellCount = net->cellCount;
  unsigned long stateCount = net->stateCount;
  unsigned long pCellCount = 1; //pCellcount initializes with 1 because of the startcell
  unsigned long totalResistance = 0;
  dmsCell * cell;
  dmsCell * borderCell;
  dmsPathCell * pCell;
  dmsPathCell * borderPCell;
  unsigned long i, j;

#ifdef DMS_P_DEBUG  
   printf("--- starting pathfinder BF.\n"); 
#endif

  //if no startcell is given set to default startcell (default default is cell 0) 
  if (startCell == NULL)
    startCell = net->cell + (net->startCell);
  
  //if a new state is given, update it 
  if (state != NULL) {
    for (i=0; i < stateCount; i++) 
      *(pNet->currentState+i) = *(state+i); //copy the state value.. for now
  }
  
  //set pCell to the first 'pointer' and create one to the startcell
  pCell = pNet->pCell;
  pCell->cell = startCell;
  startCell->pathOpen = 1;
  startCell->pathLength = 1;

#ifdef DMS_P_DEBUG  
   printf("opening startCell: %s\n", startCell->name); 
#endif

  //check selfrepeat and border-loop properties, this is only interesting for the startcell
  updateStateDiff(startCell, pNet);

#ifdef DMS_P_DEBUG  
   printf("startCell statediff: %lu\n", startCell->stateDiff); 
#endif

  if (startCell->stateDiff == 0) {

#ifdef DMS_P_DEBUG  
      printf("startCell selfrepeat: %lu\n", startCell->selfRepeat); 
#endif

    //current cell has the right state, let's see if selfRepeat is allowed
    if ((startCell->maxSelfRepeat == 0) || (startCell->selfRepeat < startCell->maxSelfRepeat)) {

#ifdef DMS_P_DEBUG  
       printf("selfrepeating\n"); 
#endif

      //create a path and return the startcell for self repeat, forget about resistance
      pCell++;
      pCell->cell = startCell;
      startCell->pathLength = 2;
      startCell->backTrace = (void *) startCell;
      return foundPath(pCell, pNet);
    }
    //open ALL borders (including border-loops) of the startcell and create paths
    for (i=0; i < startCell->borderCount; i++) {
      borderCell = net->cell + ((startCell->border +i)->destination);
      borderCell->backTrace = (void *) startCell;
      //update resistance
      updateResistance(borderCell, pNet);
      borderCell->totalResistance = (startCell->border + i)->resistance + borderCell->currentResistance;
      borderPCell = pNet->pCell + pCellCount;
      borderPCell->cell = borderCell;
      borderCell->pathOpen = 1;
      borderCell->pathLength = 2;

#ifdef DMS_P_DEBUG  
       printf("opened cell, totalresistance, pathlength: %s %lu %lu\n", borderCell->name, borderCell->totalResistance, borderCell->pathLength); 
#endif
      pCellCount++;
    }
    //find the lowes resistance and point towards it, just start the loop after this, but check statediff first. if statdiff is 0 just end doesn't matter if its startcell
    //that's looping or a cell right after it has statediff 0
    pCell = getLowResPath(pNet);
    //if there's a different border/cell with lower resistance, set the startcell to closed and lets go.
    startCell->pathOpen = -1;
  }//end of if startCell has perfect state
  
  cell = pCell->cell;

  //set startCell selfrepeat to zero if we're not going to repeat
  if (cell != startCell)
    startCell->selfRepeat = 0;

  //return a cell if the initial bordercheck of start-cell has happened
  if (cell->stateDiff == 0)
    return foundPath(pCell, pNet);

  //set startcell to zero anyway (sigh)
  startCell->selfRepeat = 0;

  //get i right just for the record
  if (cell == startCell)
    i = 0;
  else
    i = 1;


#ifdef DMS_P_DEBUG  
   printf("starting main pathfinder loop\n"); 
#endif

  //start a for loop to search each and every cell, hopefully we'll be able to break it
  while (i < cellCount) {
    //pCell is the cell with the current lowest path cost, and we're now expanding it 

#ifdef DMS_P_DEBUG  
     printf("opening borders of cell: %s\n", cell->name); 
#endif

    //open all the borders of a cell and add them to pathNetwork
    for (j=0; j < cell->borderCount; j++) {
         
      borderCell = net->cell + ((cell->border + j)->destination);
      //see if our bordercell is open for business, ignore if it has a border-loop that is only relevant for start cell
      if (borderCell->pathOpen > -1) {
        //only go on if borderCell is no border-loop
        if (borderCell != cell) {
          //update resistance
          updateResistance(borderCell, pNet);
          totalResistance = (cell->border + j)->resistance + borderCell->currentResistance;
          //add the resistance of the previous cell, if that previous cell is not the start cell
          if (cell != startCell) 
            totalResistance += cell->totalResistance;
          //the cell has already be opened, but the new resistance is better, if so replace resistance and backTrace
          if ((borderCell->pathOpen == 1) && (totalResistance < borderCell->totalResistance)) {
            borderCell->totalResistance = totalResistance;
            borderCell->backTrace = (void *) cell;
            borderCell->pathLength = cell->pathLength + 1;

#ifdef DMS_P_DEBUG  
             printf("opened cell, totalresistance, pathlength: %s %lu %lu\n", borderCell->name, borderCell->totalResistance, borderCell->pathLength); 
#endif

          }//end of if existing pCell
          //if the cell is new, add it to the pathnetwork
          else if (borderCell->pathOpen == 0) {
            borderCell->totalResistance = totalResistance;
            borderCell->backTrace = (void *) cell;
            borderCell->pathOpen = 1;
            borderCell->pathLength = cell->pathLength + 1;     
            //add the cell to our pathnetwork
            (pNet->pCell + pCellCount)->cell = borderCell;   
            pCellCount++;

#ifdef DMS_P_DEBUG  
             printf("opened cell, totalresistance, pathlength: %s %lu %lu\n", borderCell->name, borderCell->totalResistance, borderCell->pathLength); 
#endif

          }//end of if new pCell
        }//end of if not border-loop
      }//end of if path open      
    }// end of for loop opening borders

    //we've added all borders, set our current path to closed
    cell->pathOpen = -1;

#ifdef DMS_P_DEBUG  
     printf("closing cell: %s\n", cell->name); 
#endif

    //find the new lowest cost path
    pCell = getLowResPath(pNet);

    //if all cells are closed we're at a dead end (and hopefully at i = cellCount -1)
    if (pCell == NULL)
      return foundPath(getBestPCell(pNet), pNet);
    cell = (pCell->cell);
    
    //if this path ends in the right state, then we are succesfull, return current path
    if (cell->stateDiff == 0) 
      return foundPath(pCell, pNet);
    
    //if this path has more resistance than allowed, break of the search
    if ((net->maxPathResistance > 0) && (cell->totalResistance > net->maxPathResistance)) 
      return foundPath(getBestPCell(pNet), pNet);
    //end of if more resistance than allowed
   //go for next cell!
   i++;//!!!!
  }//end of WHILE loop for path searching    

  //fail if ending up here
  return NULL;
}//end of breadthfirst pathsearching function

