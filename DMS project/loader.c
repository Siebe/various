 /* 
 * loader.c is part of DMS Project
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

#include "loader.h"

int dmsFreeNet(dmsNetwork * freeNet) {
  if (freeNet == NULL) 
    return 0;
  // x->y is synonym for (*x).y so (x->y)->z is (*(*x).y).z
  free((freeNet->cell)->name);
  free((freeNet->cell)->state);
  free((freeNet->cell)->border);
  free((freeNet->cell)->exit);
  free(freeNet->cell);
  free(freeNet->stateName);
  free(freeNet);
  return 1;
}


dmsHandle dmsAllocNet(unsigned long cellCount, unsigned long stateCount, unsigned long borderCount, unsigned long exitCount) {
  
  //create a new handle and alloc single variables and pointers
  dmsNetwork * newNet;
  newNet = (dmsNetwork *) malloc(sizeof(dmsNetwork));
  
  //quit if it doesn't work
  if (newNet == NULL) 
    return NULL;
  
  //alloc the cells
  newNet->cell = (dmsCell *) calloc(cellCount, sizeof(dmsCell));
  
  if (newNet->cell == NULL) {
    free(newNet);
    return NULL;
  }
  
  //alloc the state names
  newNet->stateName = (char *) calloc(stateCount * CHAR_MAX, sizeof(char));

  //alloc the the states and borders, update the rest of the pointers later
  (newNet->cell)->state = (unsigned long *) calloc((stateCount * cellCount), sizeof(unsigned long));
  (newNet->cell)->border = (dmsBorder *) calloc(borderCount, sizeof(dmsBorder));
  (newNet->cell)->exit = (dmsExit *) calloc(exitCount, sizeof(dmsExit));
  (newNet->cell)->name = (char *) calloc((CHAR_MAX * cellCount), sizeof(char));  

  if ((newNet->stateName == NULL) || ((newNet->cell)->state == NULL) || ((newNet->cell)->border == NULL) || ((newNet->cell)->exit == NULL)) {
    dmsFreeNet(newNet);
    return NULL;
  }

  return newNet;
}


	
void dmsLoaderRaiseError(char * error) {
  strncpy((char *)&dmsLoaderError, error, 512);
}


static inline long charToInt(char * input) {
return atol(input);
/*   
  long temp = -1;

  while (*input >= '0' && *input <='9') {
    temp = (temp * 10) + (*input - '0');
    input++;
  }
  
  if (*input != 0) {
    temp = -1;
  }
 
  return temp;
*/
}


static inline double charToDouble(char * input) {
return atof(input);
/*
  double temp = 0.;

  while (*input >= '0' && *input <='9') {
    temp = (temp * 10) + (*input - '0');
    input++;
  }  

  if ((*input == '.') || (*input == ',')) {
    input++;
    double scale = 0.1;
    while (*input >= '0' && *input <='9') {
      temp += (*input-'0') * scale;
      scale *= 0.1;
      input++;
    }
  }

  if (*input != 0) {
    temp = 0.;
  }
  
  return temp;
*/
}


static inline void exitTimeBubbleSort(dmsExit * array,long length) {
  long i,j;
  dmsExit holder;
  for(i=0;i<length;i++) {
    for(j=0;j<i;j++) {
//    if (array[i]->time > array[i+j]->time) {  //for some reason this wont compile..
      if( (array+j)->time > (array+j+1)->time ) { 
         holder = *(array+j+1);
         *(array+j+1) = *(array+j);
         *(array+j) = holder;
      } //end of if >
    } //end of for j
  } //end of for i
}//end of func


mxmlHandle dmsLoadXmlFile(char * filename) {
  
  FILE *fp;
  mxml_node_t *tree;

  //Load the file
  fp = fopen(filename, "r");
  
  if (fp == NULL) {
    dmsLoaderRaiseError("file not found");
    return NULL;
  }
    
  tree = mxmlLoadFile(NULL, fp,
                    MXML_OPAQUE_CALLBACK);
  if (tree == NULL) 
    dmsLoaderRaiseError("can't load xml from file");

  fclose(fp);
  return tree;

}


dmsHandle dmsAllocNetXML(mxml_node_t *tree) {
  
  dmsNetwork *newNet;
  mxml_node_t *mainNode;
  mxml_node_t *node;
  unsigned long cellCount = 0;
  unsigned long stateCount = 0;       
  unsigned long borderCount = 0;
  unsigned long exitCount = 0;
  
  //extract the cellcount, statecount and bordercount from the file
  //set our main node to the 'dms_network' element
  mainNode = mxmlFindElement(tree, tree, "dms_network", NULL, NULL, MXML_DESCEND);

  if (mainNode == 0) {
    dmsLoaderRaiseError("main node not found: <dms_network>");
    return NULL;
  }

  //no need to descent for the cell count
  for (node = mxmlFindElement(mainNode, tree, "cell", NULL, NULL, MXML_DESCEND_FIRST);
       node != NULL;
       node = mxmlFindElement(node, tree, "cell", NULL, NULL, MXML_NO_DESCEND)) {
    cellCount++;
  }
  
  if (cellCount == 0) {
    dmsLoaderRaiseError("no cells found!");
    return NULL;
  }

  //for state count do not descend, else all states of cells will be counted. the number will later be multiplied with the number of cells
  for (node = mxmlFindElement(mainNode, tree, "state", NULL, NULL, MXML_DESCEND_FIRST);
       node != NULL;
       node = mxmlFindElement(node, tree, "state", NULL, NULL, MXML_NO_DESCEND)) {
    stateCount++;
  }

  if (stateCount == 0) {
    dmsLoaderRaiseError("no states found!");
    return NULL;
  }

  //all the borders need to be counted though
  for (node = mxmlFindElement(mainNode, tree, "border", NULL, NULL, MXML_DESCEND);
       node != NULL;
       node = mxmlFindElement(node, tree, "border", NULL, NULL, MXML_DESCEND)) {
    borderCount++;
  }

  if (borderCount == 0) {
    dmsLoaderRaiseError("no borders found!");
    return NULL;
  }

  //same for the exits
  for (node = mxmlFindElement(mainNode, tree, "exit", NULL, NULL, MXML_DESCEND);
       node != NULL;
       node = mxmlFindElement(node, tree, "exit", NULL, NULL, MXML_DESCEND)) {
    exitCount++;
  }

  if (exitCount == 0) {
    dmsLoaderRaiseError("no exits found!");
    return NULL;
  }
  
  //now we can allocate some memory

  newNet = dmsAllocNet(cellCount, stateCount, borderCount, exitCount);

  if (newNet == NULL) {
    dmsLoaderRaiseError("unable to allocate memory.");
    return NULL;
  }
  
  newNet->cellCount = cellCount;
  newNet->stateCount = stateCount;
  newNet->borderCount = borderCount;
  newNet->exitCount = exitCount;
  newNet->stateExponent = 1.0f / (double)stateCount;

  return newNet;

}


dmsHandle dmsParseXML(mxml_node_t *tree, dmsHandle newNet) {
 
  mxml_node_t *mainNode;
  mxml_node_t *cellNode;
  mxml_node_t *borderNode;
  mxml_node_t *exitNode;
  mxml_node_t *node;
  unsigned long cellCount;
  unsigned long stateCount;
  long i, j, temp;
  double tempf;
  char itoabuf[CHAR_MAX]; //not threadsafe, maybe include in dmsNetwork?
  
  mainNode = mxmlFindElement(tree, tree, "dms_network", NULL, NULL, MXML_DESCEND);

  
  if (mainNode == 0) {
    dmsLoaderRaiseError("main node not found: <dms_network>");
    return NULL;
  }
   
  //get cellCount and statecount
  cellCount = newNet->cellCount;
  stateCount = newNet->stateCount;

  //fill in the Network info
  node = NULL;
  //get the name (optional)
  node = mxmlFindElement(mainNode, tree, "name", NULL, NULL, MXML_DESCEND_FIRST);
  if (node != NULL) {
    strncpy(newNet->name, (node->child)->value.opaque, CHAR_MAX-1);
  }
  else
    newNet->name = "untitled";
  
  //get the startcell (optional)
  node = mxmlFindElement(mainNode, tree, "start_cell", NULL, NULL, MXML_DESCEND_FIRST);
  temp = 0;
  if (node != NULL)
    temp = charToInt((node->child)->value.opaque);
  if ((temp < 0) || (temp >= cellCount))
    temp = 0;
  newNet->startCell = temp;


  //get the globalrepeattime (optional)
  node = mxmlFindElement(mainNode, tree, "globalrepeat_time", NULL, NULL, MXML_DESCEND_FIRST);
  temp = 0;
  if (node != NULL)
   temp = charToInt((node->child)->value.opaque); 
  newNet->globalRepeatTime = temp;


  //get the stateDiffResistance (optional)
  node = mxmlFindElement(mainNode, tree, "statediff_resistance", NULL, NULL, MXML_DESCEND_FIRST);
  temp = 0;
  if (node != NULL)
    temp = charToInt((node->child)->value.opaque); 
  newNet->stateDiffResistance = temp;

  //get the MaxPathResistance (optional)
  node = mxmlFindElement(mainNode, tree, "max_pathresistance", NULL, NULL, MXML_DESCEND_FIRST);
  temp = -1; //-1 unsigned should be equal to MAX_LONG
  if (node != NULL)
    temp = charToInt((node->child)->value.opaque); 
  newNet->maxPathResistance = temp;

  //get the state names
  for (i=0; i<stateCount; i++) {
    //make a string of i
    //atoi(i, atoibuf, 10);
    sprintf(itoabuf, "%ld", i);
    node = mxmlFindElement(mainNode, tree, "state", "id", itoabuf, MXML_DESCEND_FIRST);  
    if (node == NULL){
      dmsLoaderRaiseError("state id number not found.");
      return NULL;
    }
    if (mxmlFindElement(node, tree, "state", "id", itoabuf, MXML_NO_DESCEND) != NULL ) {
      dmsLoaderRaiseError("duplicate id number found in states.");
      return NULL;
    }
    strncpy((newNet->stateName + i*CHAR_MAX), (node->child)->value.opaque, CHAR_MAX-1);
    node = mxmlFindElement(node, tree, "state", NULL, NULL, MXML_NO_DESCEND);
  }
 
  //start parsing the cells
  //note : cells best be parsed by order of id, to ensure borders and exits are lined up by id in memory.. not sure if that's really important, but it just seems tidy,
  //       still assuming that the xml already has the right order, but it can not be guaranteed. no id number may be skipped though
  //note note : what really would be tidy is to give border and exits all the same amount of memory so they would really be ALLIGNED, dunno if it i'll be worth the memory
  //            though, hopefully networks become really huge and cells get loads of exits :D Again i'm not sure if it would really be important for optimization. I can't 
  //            fully imagine a situation where i would need to iterate every border or exit.
  
  //start the first cell
  

  for (i=0; i<cellCount; i++) {
   
    //find the CellNode with id = i
    sprintf(itoabuf, "%ld", i);
    cellNode = mxmlFindElement(mainNode, tree, "cell", "id", itoabuf, MXML_DESCEND_FIRST);   
    if (cellNode == NULL){
      dmsLoaderRaiseError("cell id number not found.");
      return NULL;
    }
    if (mxmlFindElement(cellNode, tree, "cell", "id", itoabuf, MXML_NO_DESCEND) != NULL ) {
      dmsLoaderRaiseError("duplicate id number found in cells.");
      return NULL;
    }

    //create the pointers (if needed)
    if (i > 0) {
      (newNet->cell+i)->name = (((newNet->cell)->name) + i*CHAR_MAX);
      (newNet->cell+i)->state = (((newNet->cell)->state) + i*stateCount);
      //point new border pointer to previous border pointer and add previous border count to the new pointer
      (newNet->cell+i)->border = (((newNet->cell+i-1)->border) + ((newNet->cell+i-1)->borderCount)); 
      //same with exits
      (newNet->cell+i)->exit = (((newNet->cell+i-1)->exit) + ((newNet->cell+i-1)->exitCount));
    } 

    //get some cell info now
    //get the name (optional)
    node = mxmlFindElement(cellNode, tree, "name", NULL, NULL, MXML_DESCEND_FIRST);
    if (node != NULL) 
      strncpy(((newNet->cell+i)->name), (node->child)->value.opaque, CHAR_MAX-1);
    else 
      (newNet->cell+i)->name = "untitled";
    
    //get the selfrepeat (optional)
    node = mxmlFindElement(cellNode, tree, "selfrepeat", NULL, NULL, MXML_DESCEND_FIRST);
    if (node != NULL) {
      temp = charToInt((node->child)->value.opaque);
      if (temp < 0)        
        temp = 0;
      (newNet->cell+i)->maxSelfRepeat = temp;
    }


    //get the globalrepeat (optional)
    node = mxmlFindElement(cellNode, tree, "globalrepeat", NULL, NULL, MXML_DESCEND_FIRST);
    if (node != NULL) {
      temp = charToInt((node->child)->value.opaque);
      if (temp < 0)
        temp = 0;
      (newNet->cell+i)->globalRepeatResistance = temp;
    }

    //get the length (optional) 
    (newNet->cell+i)->length = 0.;
    node = mxmlFindElement(cellNode, tree, "length", NULL, NULL, MXML_DESCEND_FIRST);
    if (node != NULL) {
      tempf = charToDouble((node->child)->value.opaque);
      (newNet->cell+i)->length = tempf;
    }

    //get the state values
    for (j=0; j<stateCount; j++) {
      //find state id=j
      sprintf(itoabuf, "%ld", j);
      node = mxmlFindElement(cellNode, tree, "state", "id", itoabuf, MXML_DESCEND_FIRST);   
      if (node == NULL){
        dmsLoaderRaiseError("cell-state id number not found.");
        return NULL;
      }
      if (mxmlFindElement(node, tree, "state", "id", itoabuf, MXML_NO_DESCEND) != NULL ) {
        dmsLoaderRaiseError("duplicate id number found in cell-states.");
        return NULL;
      }  
      
      //parse each state value, make sure its an long  
      temp = charToInt((node->child)->value.opaque);
      if (temp < 0) {
        dmsLoaderRaiseError("invalid value in cell state definitions.");
        return NULL;
      }
      *((newNet->cell+i)->state+j) = temp;    
    }//end of for loop cell state definitions
  
    //count and parse the borders IN the cell 
    j = 0;
    for (borderNode = mxmlFindElement(cellNode, tree, "border", NULL, NULL, MXML_DESCEND_FIRST);
         borderNode != NULL;
         borderNode = mxmlFindElement(borderNode, tree, "border", NULL, NULL, MXML_NO_DESCEND)) {

      //get destination
      node = mxmlFindElement(borderNode, tree, "destination", NULL, NULL, MXML_DESCEND_FIRST);
      if (node == NULL) {
        dmsLoaderRaiseError("no destination found in border definition.");
        return NULL;
      }
      temp = charToInt((node->child)->value.opaque);
      if (temp < 0) {
        dmsLoaderRaiseError("destination has invalid value in border definition.");
        return NULL;
      }
      ((newNet->cell+i)->border+j)->destination = temp; 
      
      //get resistance 
      node = mxmlFindElement(borderNode, tree, "resistance", NULL, NULL, MXML_DESCEND_FIRST);
      if (node == NULL) {
        dmsLoaderRaiseError("no resistance found in border definition.");
        return NULL;
      }
      temp = charToInt((node->child)->value.opaque);
      if (temp < 0) {
        dmsLoaderRaiseError("resistance has invalid value in border definition.");
        return NULL;
      }      
      ((newNet->cell+i)->border+j)->resistance = temp;

      j++;
    } //end of for loop (borders)

    if (j == 0) {
      dmsLoaderRaiseError("no borders found in cell!");
      return NULL;
    }
    (newNet->cell+i)->borderCount = j;
    
   
    //same for exits
    j = 0;
    for (exitNode = mxmlFindElement(cellNode, tree, "exit", NULL, NULL, MXML_DESCEND_FIRST);
         exitNode != NULL;
         exitNode = mxmlFindElement(exitNode, tree, "exit", NULL, NULL, MXML_NO_DESCEND)) {

      //get area(optional)
      node = mxmlFindElement(exitNode, tree, "area", NULL, NULL, MXML_DESCEND_FIRST);
      temp = 0;
      if (node != NULL) {
          temp = charToInt((node->child)->value.opaque);
        if ((temp < 0) || (temp > 1))
          temp = 0; 
      }
      ((newNet->cell+i)->exit+j)->area = temp;

      //get fade(optional)
      node = mxmlFindElement(exitNode, tree, "fade", NULL, NULL, MXML_DESCEND_FIRST);
      temp = 0;
      if (node != NULL) {
        temp = charToInt((node->child)->value.opaque);
        if ((temp < 0) || (temp > 3)) 
          temp = 0;       
      }
      ((newNet->cell+i)->exit+j)->fade = temp;

      //get time(optional if area=1)
      node = mxmlFindElement(exitNode, tree, "time", NULL, NULL, MXML_DESCEND_FIRST);
      tempf = 0.;
      if (node != NULL)
        tempf = charToDouble((node->child)->value.opaque);
      if ((tempf == 0.) && (((newNet->cell+i)->exit+j)->area = 0)) {
        dmsLoaderRaiseError("exit defined as exitpoint, but time is defined as 0 or undefined.");
        return NULL;
      }      
      ((newNet->cell+i)->exit+j)->time = tempf;

      //endtime(only if area=1)
      tempf = 0.;
      if (((newNet->cell+i)->exit+j)->area == 1) {
        node = mxmlFindElement(exitNode, tree, "endtime", NULL, NULL, MXML_DESCEND_FIRST);        
        if (node != NULL) 
          tempf = charToDouble((node->child)->value.opaque);
        //endtime is not valid if it's less than 'time'
        if (tempf < ((newNet->cell+i)->exit+j)->time) {
          dmsLoaderRaiseError("exit defined as area, but endtime is less than time or undefined.");
          return NULL;
        }              
      }//end of if area=1
      ((newNet->cell+i)->exit+j)->endTime = tempf;

      //fadetime(only if fade>1)
      tempf = 0.;
      if (((newNet->cell+i)->exit+j)->fade > 1) {
        node = mxmlFindElement(exitNode, tree, "fadetime", NULL, NULL, MXML_DESCEND_FIRST);
        if (node != NULL) 
          tempf = charToDouble((node->child)->value.opaque);   
        if (tempf == 0.) {
          dmsLoaderRaiseError("exit defined as fade, but fadetime = 0 or undefined.");
          return NULL;
        }           
      }//end of if fade >1
      ((newNet->cell+i)->exit+j)->fadeTime = tempf;

      j++;
    } //end of for loop (exits)
    if (j == 0) {
      dmsLoaderRaiseError("no exits found in cell!");
      return NULL;
    }
    (newNet->cell+i)->exitCount = j;
  

    //sort the exits based on their time pointer
    exitTimeBubbleSort((newNet->cell+i)->exit, (newNet->cell+i)->exitCount);

  }// end of for loop (cells)

  //succesfully parsed a network
  return newNet;
}
