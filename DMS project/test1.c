 /* 
 * test1.c is part of DMS Project
 * Copyright 2011 by JFK - whydontyouspamme@hotmail.com
 *
 * DMS Project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#include "cell.h"
#include "path.h"
#include "loader.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>



void dmsPrintPNet(dmsPNHandle pNet) {
  dmsCellTime * hist = pNet->histRead;
  dmsNetwork * net = pNet->net;

  if ((pNet != NULL) && (net != NULL)) {
    printf("---printing pathNetwork history of network - %s:\n", net->name);
    for (hist = pNet->histRead; hist < pNet->histWrite; hist++) 
      printf("  cell name, timestamp, stamp in sec %s %lu %f\n", (hist->cell)->name, (unsigned long) hist->timeStamp, (double)hist->timeStamp/(double)CLOCKS_PER_SEC);
  }//end of if not null
  else
    printf("unable to print pNet, pNet pointer is null!");
}


void dmsPrintCell (dmsHandle net, dmsCHandle cell, int temp) {
  unsigned long borderCount = cell->borderCount;
  unsigned long exitCount = cell->exitCount;
  unsigned long stateCount = net->stateCount;
  unsigned long i;

  if (cell != NULL) {
    printf("---cell id, name: (%lu) %s\n", (unsigned long) ((net->cell) - cell), cell->name);
    printf("border-, exit-count: %lu %lu\n", borderCount, exitCount);
    printf("maxselfrepeat, globablrepeat-resistance, length: %lu %lu %f\n", cell->maxSelfRepeat, cell->globalRepeatResistance, cell->length);
    printf("-----States:\n");
    for (i=0; i<stateCount; i++) {
      printf("  state id, val: %lu %lu\n", i, (unsigned long) *(cell->state+i));
    }
    printf("------Borders:\n");
    for (i=0; i<borderCount; i++)
      printf("   destination, resistance: %lu %lu\n", (cell->border+i)->destination, (cell->border+i)->resistance);
    printf("------Exits:\n");
    for(i=0; i<exitCount; i++) {
      printf("   area, fade: %lu %lu\n", (cell->exit+i)->area, (cell->exit+i)->fade);
    printf("   time, endtime, fadetime: %f %f %f\n", (cell->exit+i)->time, (cell->exit+i)->endTime, (cell->exit+i)->fadeTime);
    }//end of for exits
    if (temp > 0) {
      printf("------Temp Data:\n");
      printf("   selfrepeat, globalrepat, statedif: %lu %lu %lu\n", cell->selfRepeat, cell->globalRepeat, cell->stateDiff);
      printf("   currentresistance, totalresistance, pathLength: %lu %lu %lu\n", cell->currentResistance, cell->totalResistance, cell->pathLength);
      printf("   pathopen, resistanceupdated, statediffupdated: %d %d %d\n", cell->pathOpen, cell->resistanceUpdated, cell->stateDiffUpdated);
      if (cell->backTrace != NULL)
        printf("   cell backtracing to: (%lu) %s\n", (unsigned long)((dmsCell*)(cell->backTrace) - net->cell), ((dmsCell *)(cell->backTrace))->name);
      else
        printf("no backtrace cell\n");
    }//end of if temp
  }//end of if not null
  else
    printf("unable to print cell, cell pointer is null!");
}

void dmsPrintNet (dmsHandle net) {
  if (net != NULL) {
    unsigned long cellCount = net->cellCount;
    unsigned long stateCount = net->stateCount;
    unsigned long borderCount = net->borderCount; 
    unsigned long exitCount = net->exitCount;
    unsigned long i;

    printf("---Printing network info:\n");
    printf("   name: %s\n", net->name);
    printf("   startcell id: %lu\n", net->startCell);
    printf("   cell-, state-, border-, exit-count: %lu %lu %lu %lu\n", cellCount, stateCount, borderCount, exitCount);
    printf("   statediff-, maxpath-resistance: %lu %lu\n",  net->stateDiffResistance, net->maxPathResistance);
    printf("   statediff-exponent, globalrepeat-time: %f %f\n", net->stateExponent, net->globalRepeatTime);
    printf("-------States:\n");
    for (i=0; i<stateCount; i++) 
      printf("   state id, name: (%lu) %s\n", i, net->stateName+(i*CHAR_MAX)); 
    printf("-------Cells:\n");  
    for (i=0; i<cellCount; i++) {
      dmsPrintCell(net, net->cell+i, 0);
    }//end of for cellcount
    printf("---Finished printing network info.\n");
  }//end of if not null
  else
    printf("unable to print net, net pointer is null!");
}//end of dmsPrintNet


int main (int argc, char *argv[] ) {
  
  char buf[CHAR_MAX], command[CHAR_MAX];
  char * pCommand = (char *)command;
  mxmlHandle xmlDoc;
  dmsHandle net, newNet;
  dmsPNHandle pNet;
  struct rusage r;
  

  dmsCHandle cell = NULL;
  unsigned long states[2] = {0, 0};
  int a,b, i;
  size_t charMax = CHAR_MAX; 

  if (system("cls")) 
    system("clear");
 
  printf("Starting DMS testprogram 0.1\nCopyright 2011 JFK whydontyouspamme@hotmail.com\n\n");

  if (argc > 1) 
    strncpy(buf, argv[1], CHAR_MAX-1);
  else {
    printf("input filename of xml containing cellnetwork\n>");
    scanf("%s", (char *)buf);
  }

  
  xmlDoc = dmsLoadXmlFile((char *)buf);
  if (xmlDoc == NULL) {
    printf("unable to load XML file.");
    return 1;
  }

  net = dmsAllocNetXML(xmlDoc);
  if (net == NULL) {
     printf("unable to allocate net.");
     mxmlDelete(xmlDoc);
     return 2;
  }

  printf("net allocated\n %lu cells\n %lu states\n %lu borders\n %lu exits\n", net->cellCount, net->stateCount, net->borderCount, net->exitCount);

  newNet = dmsParseXML(xmlDoc, net);
  
  if (net == newNet)
    printf("pointers ok\n");
  else if (newNet == NULL) {
    printf("net is null, error: %s\n", dmsLoaderError);
    dmsFreeNet(newNet);
    mxmlDelete(xmlDoc);
    return 1;
  }
  else {
    printf("pointers differ??\n");
    return 3;
  }
  
  dmsPrintNet(net);

  pNet = dmsAllocPathNetwork(net);
  if (pNet == NULL) {
    printf("Unable to allocate memory, error: %s\n", dmsPathError);
    dmsFreeNet(newNet);
    mxmlDelete(xmlDoc);
    return 2;
  }

  printf("succesfully created pathNet\n");

  cell = (net->cell + (net->startCell));
  printf("current startcell index, name: (%lu) %s\n", net->startCell, cell->name);


  //program loop, break with ctrl-c... or just type quit
  while (1) {

    a = -1;
    b = -1;

    printf("ready for command\n>");
    getline (&pCommand, &charMax, stdin);

      if (!strncmp(command,"quit", 4)) {
        printf("sure?\n>");
        scanf("%s", (char *)command);
        if (command[0] == 'y')
          break; 
      }

      else if (!strncmp(command,  "help", 4)) {
        printf("commands:\nquit - quit the program\nprint - print the cell-network\ngetcell - print the current cell name\n");
        printf("printcell (index) - print all cell info \nprintpnet - print history of pathnetwork\n");
        printf("setcell (index) - set the start cell\nsetstate (index) (value) - set a state value\n");
        printf("find - start the search algorithm\nlastpath - show the last chosen path, starting with startcell\n");
      }

      else if (!strncmp(command,  "printnet", 8)) {
        dmsPrintNet(net);
      }

      else if (!strncmp(command,  "printpnet", 9)) {
        dmsPrintPNet(pNet);
      }
      

      else if (!strncmp(command,  "find", 4)) {
        dmsCleanPNet(pNet);
        cell = dmsFindPathBF(pNet, cell, (unsigned long *)&states);
        if (cell == NULL)
          printf("pathfinder failed\n");
        else {
          printf("path found, current cell is now: %s\n", cell->name);
          printf("last chosen path: %s", ((pNet->lastPath)->cell)->name);
          for (i=1; i < net->cellCount; i++) {
            if ((pNet->lastPath+i)->cell == NULL) 
              break;
            printf("->%s", ((pNet->lastPath+i)->cell)->name);
          }
          printf("\n");
        }
      } 

      else if(!strncmp(command, "lastpath", 8)) {
        if ((pNet->lastPath)->cell != NULL) {
          printf("last chosen path: %s", ((pNet->lastPath)->cell)->name);
          for (i=1; i < net->cellCount; i++) {
            if ((pNet->lastPath+i)->cell == NULL) 
              break;
            printf("->%s", ((pNet->lastPath+i)->cell)->name);
          }
          printf("\n");
        }
        if (i < 2)
          printf("no valid path found\n");
      }

      else if (!strncmp(command,  "getcell", 7)) 
        printf("current cell is: %s\n", cell->name);

      else if (!strncmp(command,  "setcell", 7)) {
        if (sscanf(command, "%s %i",(char *) &buf, &a) == 2) {
          cell = net->cell+a;
          printf("current cell is now: (%lu) %s\n", (unsigned long) ((net->cell) - cell), cell->name);
        }
        else
          printf("usage : setcell (index)\n(index) being an integer between 0 and cellCount-1\n");
      }

      else if (!strncmp(command,  "printcell", 9)) {
        if (sscanf(command, "%s %i",(char *) &buf, &a) == 2) {
          cell = net->cell+a;
          dmsPrintCell(net, cell, 1);
        }
        else
          printf("usage : printcell (index)\n(index) being an integer between 0 and cellCount-1\n");
      }

      else if (!strncmp(command, "getstate", 8)) {
        printf("last search states:");
        for (i=0; i<net->stateCount; i++) 
          printf(" %lu", *(pNet->currentState +i));
        printf("\n");

        printf("next states:");
        for (i=0; i<net->stateCount; i++) 
          printf(" %lu", states[i]);
        printf("\n");
      }

      else if (!strncmp(command, "setstate", 8)) {
        if (sscanf(command, "%s %i %i", (char *)&buf, &a, &b) == 3) {
          states[a] = b;
          printf("state set\n");
        }
        else {
          printf("usage: setstate (index) (value)\n(index) being an integer between 0 and stateCount-1\n");
          printf("(value) being any positive integer, including zero");
        }
      }

      else if (!strncmp(command, "now", 3)) {
        printf("clock timestamp: %ld\n", (long)clock());
        printf("time timestamp %ld\n", (long)time(NULL));
        if (getrusage(RUSAGE_SELF, &r) == 0) 
          printf("rusage timestamp sec, microsec: %ld %ld\n", (r.ru_utime).tv_sec, (r.ru_utime).tv_usec);
        else 
          printf("getrusage failed");
      }

      else
        printf("unkown command, ");
      
      
  }//end of while loop

  dmsFreeNet(net);
  mxmlDelete(xmlDoc);
  dmsFreePathNetwork(pNet);
  printf("Goodbey!\n---DMS Test Program 0.1\n");
  return 0;

}

