
#ifndef FS_HALFBAND
#define FS_HALFBAND


#include <stdlib.h>
#include "ladspa.h"
#define NUMFILTERS 6

typedef struct  {

   LADSPA_Data a;

   LADSPA_Data x0;
   LADSPA_Data x1;
   LADSPA_Data x2;

   LADSPA_Data y0;
   LADSPA_Data y1;
   LADSPA_Data y2;

} AllPassFilter;


typedef struct  {

   AllPassFilter * AllPassCascadeA;
   AllPassFilter * AllPassCascadeB;
   LADSPA_Data oldout;

} HalfBandFilter;


HalfBandFilter * instantiateHBF();
LADSPA_Data processHBFcascade(AllPassFilter* , LADSPA_Data);
LADSPA_Data processHBF(HalfBandFilter* , LADSPA_Data);
void cleanupHBF(HalfBandFilter*);

#endif
