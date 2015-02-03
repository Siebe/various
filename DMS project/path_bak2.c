//path.c

#include "path.h"

void dmsPathRaiseError(char * error);


dmsPNHandle dmsAllocPathNetwork(dmsNetwork * net) {
  
  dmsPathnetwork * newPN;

  unsigned long cellCount;
  //unsigned long stateCount; 
  
  //create a new instance of a pathnetwork, assuming that all info is availabe if != null
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
  newPN->histBuffer = calloc(newPN->histCount, sizeof(dmsCellTime));
  if (newPN-histBuffer == NULL) {
    free(newPN);
    dmsPathRaiseError("Unable to allocate new history buffer for pathnetwork in memory");
    return NULL
  }

  //allocate a 'cellcount' number of pathsCells
  newPN->pCellcel= calloc(cellCount, sizeof(dmsPathCell));
  if (newPN->p == NULL) {
    free(newPN->histBuffer);
    free(newPN);
    dmsPathRaiseError("Unable to allocate new path buffer for pathnetwork in memory");
    return NULL
  }  


 //...
}


void updateStateDiff(dmsPathNetwork pNet, dmsCell * cell) {
//if not stateUpdated in cell
//check the states in pNet
//compare with states of cmsCell do (abs)diff*(abs)diff*(abs)diff* etc
//set stateupdated true
}

void updateResistance(dmsPathNetwork pNet, dmsCell * cell) {
//if not resistanceUpdated in cell
//check globalrepeat shitjes
//do updateStateDiff, change statediff to resistance
//add stateresistance and globalrepresistance to borderresistance
}

dmsPCHandle getLowResPath(dmsPathNetwork * pNet) {
//ithereate through pnet->cell
//if celOpen = 0 and lowestres < resistance then remember
}

dmsCHandle foundPath(dmsPathCell pCell) {
//add globalrepeat and maxrepeat if needed, update history if needed, return first next cell in parth
}

//--startup:
//  clear pathbuffer
//  if state of startcell is the currentstate, then check the selfrepeat properties and loop-exit

//  open all of the successors of the start cell, remember lowest resistance
//  


dmsCHandle dmsFindPathBF(dmsPathNetwork * pNet, dmsCell * startCell) {
  
  dmsNetWork * net = pNet->net;
  unsigned long cellCount = net->cellCount;
  unsigned long stateCount = net->stateCount;
  dmsCell * cell;
  dmsCell * exitCell;
  dmsPathCell * pCell;
  dmsPathCell * exitPCell;
  dmsPathCell * bestPCell;

  unsigned long pCellCount;
   
  if (startCell == NULL)
    startCell = net->Cell + (net->startCell);
  
  //create a pathCell for the startCell
  pCell = pNet->pCell;
  pCell->cell = &startCell; // so this means pCell->cell is a pointer towards pointer towards struct of our startcell! this will occur more often

  //make sure the resistance of the startcell is maximum, this will only be altered if an exit-loop is called
  startCell->totalResistance = MAX_DBL; //might not be needed, just maake getlowrespath ignore the start cell (pNet->cell)

  //check selfrepeat and exit-loop properties, this is only interesting for the startcell
  dmsUpdateStateDiff(startCell);
  if (startCell->stateDiff = 0) {
    //current cell has the right state, let's see if we can stick around
    if ((startCell->maxSelfRepeat = 0) || (startCell->SelfRepeat < startCell->maxSelfRepeat)) {
      //create a path and return the startcell for self repeat
      pCell++;
      pCell->cell = &startCell;//pointer towards pointer
      pCell->backtrace = (pCell - 1);
      return foundPath(pCell);
    }
    //check for ALL exits (including exit-loops) create paths
    for (i=0; i < startCell->exitCount; i++) {
      exitCell = net->cell + ((startCell->exit +i)->destination)
      exitPCell = pNet->pCell + pCellCount;
      exitPCell->cell = &exitCell //pointer towards pointer
      exitPCell->backtrace = startCell;
      //update resistance
      dmsUpdateResistance(exitCell);
      exitCell->pathOpen = 1;
      pCellCount++;
    }
    //break if the exit-loop is the exit with the lowest cost, else just start the loop to see if there's a cheaper path
    pCell = getLowResPath(pNet);
    if (*pCell == startCell) //pCell being pointer towards pointer towards struct. startCell is a pointer towards struct, so one * operater
       return foundPath(pCell)
    startCell->pathOpen = -1;
  }
  
  cell = pCell->cell;

  //start a for loop to search each and every cell, hopefully we'll be able to break it
  for (i=0; i < cellCount; i++) {
    //pCell is the cell with the current lowest path cost, and we're now expanding it 

    //open all the exits of a cell and add them to pathNetwork
      
    for (j=0; j < cell->exitCount; j++) {
         
      exitCell = net->cell + ((cell->exit + j)->destination)
      //see if our exitcell is open for business, ignore if it has a loop-exit that is only relevant for start cell
      if (exitCell->pathOpen = 0) {
        //add the cell to our pathnetwork
        exitPCell = pNet->pCell + pCellCount;
        exitPCell->cell = &exitCell// pointer towards pointer
        exitPCell->backtrace = pCell;
        //update resistance
        dmsUpdateResistance(exitCell);
        exitCell->totalResistance = cell->totalResistance + exitCell->resistance;
        exitCell->pathOpen = 1;           
        pCellCount++;
      }
      else if (exitCell->pathOpen = 1) {
        //if resistance is lower, it should be altered, as well as the backtrace of original pCell
      }
      
    }// end of for loop opening exits

    cell->pathOpen = -1;
    //find the new lowest cost path
   
    pCell = getLowResPath(pNet);
    cell = *(pCell->cell); //pCell->cell has pointer towards pointer towards struct. cell will have pointer towards struct
    
    if (cell->StateDiff == 0) 
      return foundPath(pCell);

    if ((net->maxPathResistance > 0) && (cell->totalResistance > net->maxPathResistance)) {
      //get lowest stateDiff/cost, this is taking too long
      pCell = pNet->pCell;
      bestPCell = pCell;
      bestCell = *(pCell->cell);
      for (j=1; j < cellCount; j++) {
        pCell++
        if (pCell = NULL)
          break;
        cell = *(pCell->cell);
        if (pCell->stateDiff < bestPCell->stateDiff) 
          bestPCell = pCell;
        else if ((pCell->stateDiff == bestPCell->stateDiff) && (pCell->totalResistance < bestCell->totalResistance))
          bestPCell = pCell;
      }
      return foundPath(bestPCell);
        
    }

  }//end of for loop for path searching    

  //if we end up here, there's no exact cell path and maxResistance is too high or 0, let's return null so we know there's something wrong
  return null;
}

