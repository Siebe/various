

#include "halfband.h"


LADSPA_Data A_COEFFICIENTS[6]=
	{0.036681502163648017
	,0.2746317593794541
	,0.56109896978791948
	,0.769741833862266
	,0.8922608180038789
	,0.962094548378084
	};


LADSPA_Data B_COEFFICIENTS[6]=
	{0.13654762463195771
	,0.42313861743656667
	,0.6775400499741616
	,0.839889624849638
	,0.9315419599631839
	,0.9878163707328971
	};


HalfBandFilter * instantiateHBF() {

   unsigned int i;
   HalfBandFilter * psHBF;

   psHBF = (HalfBandFilter *) malloc(sizeof(HalfBandFilter));
   
   if (psHBF == NULL) {
      free(psHBF);
      return NULL;
   }

   psHBF->AllPassCascadeA = calloc(NUMFILTERS, sizeof(AllPassFilter));
   psHBF->AllPassCascadeB = calloc(NUMFILTERS, sizeof(AllPassFilter));

   if (psHBF->AllPassCascadeA == NULL || psHBF->AllPassCascadeB == NULL) {
      free(psHBF->AllPassCascadeA);
      free(psHBF->AllPassCascadeB);
      free(psHBF);
      return NULL;
   }
  
   //input coefficients

   for (i=0; i < NUMFILTERS; i++) {(psHBF->AllPassCascadeA)->a = A_COEFFICIENTS[i];}
   for (i=0; i < NUMFILTERS; i++) {(psHBF->AllPassCascadeB)->a = B_COEFFICIENTS[i];}

   return psHBF;
}


LADSPA_Data processHBFcascade(AllPassFilter * AllPassCascade, LADSPA_Data input) {

   unsigned int i;
   LADSPA_Data output = input;
   AllPassFilter * psAPS;

   for (i=0; i < NUMFILTERS; i++) {
      psAPS = (AllPassCascade + i);
      if (psAPS != NULL) {
         
         //shuffle inputs
         psAPS->x2 = psAPS->x1;
         psAPS->x1 = psAPS->x0;
         psAPS->x0 = output;

         //shuffle outputs
         psAPS->y2 = psAPS->y1;
         psAPS->y1 = psAPS->y0;

         //allpassfilter 1
         output = psAPS->x2+((output-psAPS->y2)*psAPS->a);
   
         psAPS->y0 = output;
      }
      else return 0.;
   }

   return output;

}


LADSPA_Data processHBF(HalfBandFilter * HBF, LADSPA_Data input) {

   if (HBF == NULL) {return 0.;}

   LADSPA_Data output = (processHBFcascade(HBF->AllPassCascadeA, input) + HBF->oldout) * 0.5;
   HBF->oldout = processHBFcascade(HBF->AllPassCascadeB, input);

   return output;
}


void cleanupHBF(HalfBandFilter * psHBF) {

   free(psHBF->AllPassCascadeA);
   free(psHBF->AllPassCascadeB);
   free(psHBF);
}  


