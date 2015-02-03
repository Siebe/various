 /* 
 * loader.h is part of DMS Project
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


#ifndef DMS_LOADER
#define DMS_LOADER

#include "cell.h"
#include "mxml.h"//<mxml.h>
#include <string.h>
#include <stdlib.h>

typedef mxml_node_t * mxmlHandle;

char dmsLoaderError[512];

int dmsFreeNet(dmsNetwork * freeNet);
dmsHandle dmsAllocNet(unsigned long cellCount, unsigned long stateCount, unsigned long borderCount, unsigned long exitCount);
void dmsLoaderRaiseError(char * error);
mxmlHandle dmsLoadXmlFile(char * filename);
dmsHandle dmsAllocNetXML(mxml_node_t *tree);
dmsHandle dmsParseXML(mxml_node_t *tree, dmsHandle newNet);

//static inline long charToInt(char * input);
//static inline double charToDouble(char * input);
//static inline void exitTimeBubbleSort(dmsExit *array,long length);

#endif


