 /* 
 * cell.h is part of DMS Project
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


#ifndef DMS_CELL
#define DMS_CELL

#define CHAR_MAX 	50

#define EXIT_POINT 	0
#define EXIT_AREA 	1

#define EXIT_STOP_EXIT	0
#define EXIT_STOP_END	1
#define EXIT_FADE_LIN	2
#define EXIT_FADE_LOG	3


typedef struct {
  unsigned long destination;
  unsigned long resistance;
} dmsBorder;

typedef struct {
  unsigned long area; //0=exitpoint 1=exitarea
  unsigned long fade; //fade out: 0=stop at exit 1=stop at end of frame 2=fade linear  to end 3=fade log
  double time; //this one is NOT optional, there has to be an exit somewhere
  double endTime; //if area=1
  double fadeTime; //time of fade out, if fade>1
} dmsExit; 

typedef struct {
   char * name; //array of chars with name in it 
   unsigned long * state;
   
   unsigned long maxSelfRepeat; //0 = unlimited repeat, 1 = no selfrepeat, 2 = repeat once etc
   unsigned long globalRepeatResistance; // resistance that any globalrepeatcount will add 0 = unlimited repeat
   double length; // 0 = no end (something that loops.. or something)
   unsigned long borderCount;
   unsigned long exitCount;
   dmsBorder * border;
   dmsExit * exit;

   //pathfinder specific data:
   unsigned long selfRepeat; 
   unsigned long globalRepeat; 
   unsigned long stateDiff;
   unsigned long currentResistance;
   unsigned long totalResistance; //resistance it takes to get to cell
   unsigned long pathLength; //size of the path towards the cell (including startcell, even with selfrepeat it be > 2 IF the cell was considered)
   int pathOpen; //0=unchecked 1=opened -1=close
   int resistanceUpdated;
   int stateDiffUpdated;
   void * backTrace; //pointer towards previous cell in path

   //unsigned long class;
   //unsigned long bridgeCount;
   //unsigned long * bridge;
   //double * sample;
   //void * music;
} dmsCell;

typedef struct {
  char * name;
  unsigned long cellCount;
  unsigned long stateCount;
  unsigned long borderCount;
  unsigned long exitCount;
  unsigned long startCell;
  char * stateName;
  dmsCell * cell;
  double globalRepeatTime;
  double stateExponent;
  unsigned long stateDiffResistance;
  unsigned long maxPathResistance; //pretty important actually...
  //unsigned long classes;
  //unsigned long borderCount;
  //unsigned long bridgeCount;
} dmsNetwork;
 
typedef dmsNetwork * dmsHandle;
typedef dmsCell * dmsCHandle;

#endif


