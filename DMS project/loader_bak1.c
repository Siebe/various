//loader.c
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
   
  long temp = -1;

  while (*input >= '0' && *input <='9') {
    temp = (temp * 10) + (*input - '0');
    input++;
  }
  
  if (*input != 0) {
    temp = -1;
  }
 
  return temp;
}


static inline double charToDouble(char * input) {

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
}


static inline void exitTimeBubbleSort(dmsExit * array,long length) {
  long i,j;
  dmsExit holder;
  for(i=0;i<length;i++) {
    for(j=0;j<length;j++) {
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
                    MXML_TEXT_CALLBACK);
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
  for (node = mxmlFindElement(mainNode, tree, "cell", NULL, NULL, MXML_DESCEND);
       node != NULL;
       node = mxmlFindElement(node, tree, "cell", NULL, NULL, MXML_NO_DESCEND)) {
    cellCount++;
  }
  
  if (cellCount == 0) {
    dmsLoaderRaiseError("no cells found!");
    return NULL;
  }

  //for state count do not descend, else all states of cells will be counted. the number will later be multiplied with the number of cells
  for (node = mxmlFindElement(mainNode, tree, "state", NULL, NULL, MXML_DESCEND);
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
  newNet->stateExponent = 1. / (double)cellCount;

  return newNet;

}


dmsHandle dmsParseXML(mxml_node_t *tree, dmsHandle newNet) {

  mxml_node_t *mainNode;
  mxml_node_t *cellNode;
  mxml_node_t *borderNode;
  mxml_node_t *exitNode;
  mxml_node_t *searchNode;
  mxml_node_t *node;
  unsigned long cellCount;
  unsigned long stateCount;
  long i, j, id, temp;
  double tempf;

  mainNode = mxmlFindElement(tree, tree, "dms_network", NULL, NULL, MXML_DESCEND);

  if (mainNode == 0) {
    dmsLoaderRaiseError("main node not found: <dms_network>");
    return NULL;
  }

  //get cellCount and statecount
  cellCount = newNet->cellCount;
  stateCount = newNet->stateCount;
  //fill in the Network info
  
  //get the name (optional)
  node = mxmlFindElement(mainNode, tree, "name", NULL, NULL, MXML_DESCEND); 
  if (node != NULL)
    strncpy(newNet->name, node->value.text.string, CHAR_MAX);
  else
    newNet->name = "untitled";

  //get the startcell (optional)
  node = mxmlFindElement(mainNode, tree, "start_cell", NULL, NULL, MXML_DESCEND);
  temp = charToInt(node->value.text.string); 
  if (temp > 0)
    newNet->startCell = temp;
  else
    newNet->startCell = 0;

  //get the globalrepeattime (optional)
  node = mxmlFindElement(mainNode, tree, "globalrepeat_time", NULL, NULL, MXML_DESCEND);
  temp = charToInt(node->value.text.string); 
  if (temp > 0)
    newNet->globalRepeatTime = temp;
  else
    newNet->globalRepeatTime = 0;

  //get the stateDiffResistance (optional)
  node = mxmlFindElement(mainNode, tree, "statediff_resistance", NULL, NULL, MXML_DESCEND);
  temp = charToInt(node->value.text.string); 
  if (temp > 0)
    newNet->stateDiffResistance = temp;
  else
    newNet->stateDiffResistance = 0;
  
  //get the state names
  for (i=0; i<stateCount; i++) {
    node = mxmlFindElement(mainNode, tree, "state", NULL, NULL, MXML_DESCEND);
    id = charToInt((char *)mxmlElementGetAttr(node, "id"));
    if ((id < 0) || (id > stateCount)) {
      dmsLoaderRaiseError("invalid id number in global state definitions.");
      return NULL;
    }
    if ((newNet->stateName + id) != NULL) {
      dmsLoaderRaiseError("duplicate id number in global state definitions.");
      return NULL;
    }  
    strncpy((newNet->stateName + id*CHAR_MAX), node->value.text.string, CHAR_MAX);
  }

  //start parsing the cells
  //note : cells best be parsed by order of id, to ensure borders and exits are lined up by id in memory.. not sure if that's really important, but it just seems tidy,
  //       still assuming that the xml already has the right order, but it can not be guaranteed. no id number may be skipped though
  //note note : what really would be tidy is to give border and exits all the same amount of memory so they would really be ALLIGNED, dunno if it i'll be worth the memory
  //            though, hopefully networks become really huge and cells get loads of exits :D Again i'm not sure if it would really be important for optimization. I can't 
  //            fully imagine a situation where i would need to iterate every border or exit.
  
  //start the first cell
  cellNode = mxmlFindElement(mainNode, tree, "cell", NULL, NULL, MXML_DESCEND);
  
  for (i=0; i<cellCount; i++) {
    
    id= -1; //unsigned long, assuming number of cells will never go over maxof(unsigned long)-1
    
    //yay while loops, basically keep searching for the id until it's i, when found, start for the next searching where stopped
    searchNode = cellNode; //let's remember though where we stopped/are starting, to make sure we wont make a full loop, this breaks the whole function if a cell is missing.
    while (i != id) {       
      id = charToInt((char *)mxmlElementGetAttr(searchNode, "id"));
      if ((id < 0) || (id > cellCount)) {
        dmsLoaderRaiseError("invalid id number in cell definitions.");
        return NULL;
      }
      if ((newNet->cell + id) != NULL) {
        dmsLoaderRaiseError("duplicate id number in cell definitions.");
        return NULL;
      }
      
      //skip the cellpointer to the next cel
      searchNode = mxmlFindElement(searchNode, tree, "cell", NULL, NULL, MXML_NO_DESCEND);
      if (!node)
        //wrap at end of the cell list
        searchNode = mxmlFindElement(mainNode, tree, "cell", NULL, NULL, MXML_NO_DESCEND);
      //next cell should be loaded, so if cellNode is 'node' again, break it
      if (cellNode == searchNode) {
        dmsLoaderRaiseError("missing id number in cell definitions.");
        return NULL;
      }
      
    } //end of while loop to find cell id
   
    //create the pointers (if needed)
    if (i > 0) {
      (newNet->cell+i)->name = ((newNet->cell)->name + i*CHAR_MAX);
      (newNet->cell+i)->state = ((newNet->cell)->state + i*stateCount);
      //point new border pointer to previous border pointer and add previous border count to the new pointer
      (newNet->cell+i)->border = ((newNet->cell+i-1)->border + (newNet->cell+i-1)->borderCount); 
      //same with exits
      (newNet->cell+i)->exit = ((newNet->cell+i-1)->exit + (newNet->cell+i-1)->exitCount);
    }

    //get some cell info now
    //get the name (optional)
    node = mxmlFindElement(cellNode, tree, "name", NULL, NULL, MXML_DESCEND_FIRST);
    if (node != NULL) 
      strncpy(((newNet->cell+i)->name), node->value.text.string, CHAR_MAX);
    else 
      (newNet->cell+i)->name = "untitled";
    
    //get the selfrepeat (optional)
    node = mxmlFindElement(cellNode, tree, "selfrepeat", NULL, NULL, MXML_DESCEND_FIRST);
    if (node != NULL) {
      temp = charToInt(node->value.text.string);
      if (temp < 0)
        temp = 0;
      (newNet->cell+i)->maxSelfRepeat = temp;
    }


    //get the globalrepeat (optional)
    node = mxmlFindElement(cellNode, tree, "globalrepeat", NULL, NULL, MXML_DESCEND_FIRST);
    if (node != NULL) {
      temp = charToInt(node->value.text.string);
      if (temp < 0)
        temp = 0;
      (newNet->cell+i)->globalRepeatResistance = temp;
    }

    //get the length (optional) 
    (newNet->cell+i)->length = 0.;
    node = mxmlFindElement(cellNode, tree, "length", NULL, NULL, MXML_DESCEND_FIRST);
    if (node != NULL) {
      tempf = charToDouble(node->value.text.string);
      (newNet->cell+i)->length = tempf;
    }
    
    //get the states, same way as cells   
    node = mxmlFindElement(cellNode, tree, "cell", NULL, NULL, MXML_DESCEND_FIRST);
    
    for (j=0; j<stateCount; j++) {
      id = -1;
      searchNode = node;
      while (j != id) {       
        id = charToInt((char *)mxmlElementGetAttr(searchNode, "id"));
        if ((id < 0) || (id > stateCount))
          dmsLoaderRaiseError("invalid id number in cell state definitions.");
          return NULL;
        }
        if ((newNet->cell+i)->state+j != NULL) {
          dmsLoaderRaiseError("duplicate id number in cell state definitions.");
          return NULL;
        }
      
        searchNode = mxmlFindElement(node, tree, "state", NULL, NULL, MXML_NO_DESCEND);
        if (!searchNode)
          searchNode = mxmlFindElement(cellNode, tree, "state", NULL, NULL, MXML_NO_DESCEND);
        if (node == searchNode) {
          dmsLoaderRaiseError("missing id number in cell state definitions.");
          return NULL;
        }
      } //end of while loop cell state definitions id
      
      //parse each state value, make sure its an long  
      temp = charToInt(node->value.text.string);
      if (temp < 0) {
        dmsLoaderRaiseError("invalid value in cell state definitions.");
        return NULL;
      }
      *((newNet->cell+i)->state+j) = temp;    
    }//end of for loop cell state definitions
    
    //count and parse the borders IN the cell 
    j = 0;
    for (borderNode = mxmlFindElement(cellNode, tree, "border", NULL, NULL, MXML_DESCEND);
         borderNode != NULL;
         borderNode = mxmlFindElement(borderNode, tree, "border", NULL, NULL, MXML_NO_DESCEND)) {

      //get destination
      node = mxmlFindElement(borderNode, tree, "destination", NULL, NULL, MXML_DESCEND);
      temp = charToInt(node->value.text.string);
      if (temp < 0) {
        dmsLoaderRaiseError("no destination found or invalid value in border definition.");
        return NULL;
      }
      ((newNet->cell+i)->border+j)->destination = temp; 
      
      //get resistance 
      node = mxmlFindElement(borderNode, tree, "resistance", NULL, NULL, MXML_DESCEND);
      temp = charToInt(node->value.text.string);
      if (temp < 0) {
        dmsLoaderRaiseError("no resistance found or invalid value in border definition.");
        return NULL;
      }      
      ((newNet->cell+i)->border+j)->resistance = temp;

      j++;
    } //end of for loop (borders)
    if (j == 0) {
      dmsLoaderRaiseError("no borders found in cell!");
      return NULL;
    }
    newNet->cell->borderCount = j;


    //same for exits
    j = 0;
    for (exitNode = mxmlFindElement(cellNode, tree, "exit", NULL, NULL, MXML_DESCEND);
         exitNode != NULL;
         exitNode = mxmlFindElement(exitNode, tree, "exit", NULL, NULL, MXML_NO_DESCEND)) {

      //get area(optional)
      node = mxmlFindElement(exitNode, tree, "area", NULL, NULL, MXML_DESCEND);
      temp = charToInt(node->value.text.string);
      if ((temp < 0) || (temp > 1))
        temp = 0;
      ((newNet->cell+i)->exit+j)->area = temp; 
      
      //get fade(optional)
      node = mxmlFindElement(exitNode, tree, "fade", NULL, NULL, MXML_DESCEND);
      temp = charToInt(node->value.text.string);
      if ((temp < 0) || (temp > 3)) {
        temp = 0;
      ((newNet->cell+i)->exit+j)->fade = temp;
      
      //get time(optional if area=1)
      node = mxmlFindElement(exitNode, tree, "time", NULL, NULL, MXML_DESCEND);
      tempf = charToDouble(node->value.text.string);
      if ((temp == 0.) && (((newNet->cell+i)->exit+j)->area = 0)) {
        dmsLoaderRaiseError("exit defined as exitpoint, but time is defined as 0 or undefined.");
        return NULL;
      }      
      ((newNet->cell+i)->exit+j)->time = tempf;

      //endtime(only if area=1)
      if (((newNet->cell+i)->exit+j)->area == 1) {
        node = mxmlFindElement(exitNode, tree, "endtime", NULL, NULL, MXML_DESCEND);
        tempf = charToDouble(node->value.text.string);
        //endtime is not valid if it's less than 'time'
        if (temp <= ((newNet->cell+i)->exit+j)->time) {
          dmsLoaderRaiseError("exit defined as area, but endtime is less than time or undefined.");
          return NULL;
        }      
        ((newNet->cell+i)->exit+j)->time = tempf;        
      }
      else
        ((newNet->cell+i)->exit+j)->time = 0.;

      //fadetime(only if fade>1)
      if (((newNet->cell+i)->exit+j)->fade > 1) {
        node = mxmlFindElement(exitNode, tree, "fadetime", NULL, NULL, MXML_DESCEND);
        tempf = charToDouble(node->value.text.string);
        if (temp == 0.) {
          dmsLoaderRaiseError("exit defined as fade, but fadetime = 0 or undefined.");
          return NULL;
        }      
        ((newNet->cell+i)->exit+j)->fadeTime = tempf;        
      }
      else
        ((newNet->cell+i)->exit+j)->fadeTime = 0.;
      j++;
    } //end of for loop (exits)
    if (j == 0) {
      dmsLoaderRaiseError("no exits found in cell!");
      return NULL;
    }
    newNet->cell->exitCount = j;

    //sort the exits based on their time pointer
    exitTimeBubbleSort((newNet->cell+i)->exit, (newNet->cell+i)->exitCount);
  
  }// end of for loop (cells)
  
  //succesfully parsed a network
  return newNet;
}
