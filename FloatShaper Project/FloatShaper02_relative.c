/* FloatShaper.C
   
   LADSPA plugin by BlastDoors   

   This file has poor memory protection. Failures during malloc() will
   not recover nicely. */

/*****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <math.h>

/*****************************************************************************/

#include "ladspa.h"

/*****************************************************************************/

#define TABLESIZE 512
#define MAXDRIVE 3
#define TABLE_INTERP 2 //0 - no interpolation; 1 - linear; 2 - quintic;
/*****************************************************************************/

/* The port numbers for the plugin: */

#define FS_DRIVE 	0
#define FS_SHAPE	1
#define FS_OVERDRIVE    2
#define FS_INPUT        3
#define FS_OUTPUT       4

/*****************************************************************************/

#define PI 3.14159265
#define HALFPI (PI*0.5)

/*****************************************************************************
 * Description: Interpolates between p0 and n0 taking the previous (p1)
 *              and next (n1) points into account, using a 5th order
 *              polynomial.
 *
 *   Arguments: interval    Normalised time interval between inteprolated
 *                           sample and p0
 *              p1, p0      Samples prior to interpolated one
 *              n0, n1      Samples following interpolated one
 *
 *     Returns: interpolated sample
 *
 *        Code: http://www.musicdsp.org/archive.php?classid=5#62
 *        Source: http://sourceforge.net/projects/blop/
 *
 *****************************************************************************/
static inline LADSPA_Data
interpolate_quintic (LADSPA_Data interval,
                     LADSPA_Data p1,
                     LADSPA_Data p0,
                     LADSPA_Data n0,
                     LADSPA_Data n1)
{
	return p0 + 0.5f * interval * (n0 - p1 +
	                   interval * (n0 - 2.0f * p0 + p1 +
	                   interval * ( 9.0f * (n0 - p0) + 3.0f * (p1 - n1) +
	                   interval * (15.0f * (p0 - n0) + 5.0f * (n1 - p1) +
	                   interval * ( 6.0f * (n0 - p0) + 2.0f * (p1 - n1))))));
}

/*****************************************************************************/

/* Instance data for the FloatShaper plugin. */
typedef struct {

  //Pointers towards tables, memory needs to be allocated when plugin is loaded
  LADSPA_Data * m_pfTable; //main table
  LADSPA_Data * m_pfLinTable; //const table with linear shape
  LADSPA_Data * m_pfSinTable;  //const table with sinoid shape
  LADSPA_Data * m_pfRectTable; //const table with rectangle shape

  unsigned long m_lTableSize; //size of the tables
  
  //input memory
  LADSPA_Data m_fLastDrive;
  LADSPA_Data m_fLastShape;

  /* Ports:
     ------ */

  //Pointers towards input data given by host
  LADSPA_Data * m_pfDrive;
  LADSPA_Data * m_pfShape;
  LADSPA_Data * m_pfOD;

  /* Input audio port data location. */
  LADSPA_Data * m_pfInput;
  /* Output audio port data location. */
  LADSPA_Data * m_pfOutput;

} FloatShaper;



/*****************************************************************************/

/* Construct a new plugin instance. */
LADSPA_Handle 
instantiateFloatShaper(const LADSPA_Descriptor * Descriptor,
			   unsigned long             SampleRate) {
  //local var
  unsigned long i;
  unsigned long lTableSize;
  LADSPA_Data fPoint;

  //create new pointer for instance
  FloatShaper * psFloatShaper;

  //allocate memory as defined in struct
  psFloatShaper
    = (FloatShaper *)malloc(sizeof(FloatShaper));

  //quit if it doesn't work
  if (psFloatShaper == NULL) 
    return NULL;

  //dump the tablesize
  //make tablesize for construction +3 if we use quintic interpolation
  //quintic interpolation requires t-1 till t+2 so the tables need to be a bit bigger.
  if (TABLE_INTERP == 2) { lTableSize = TABLESIZE + 3; }
  else { lTableSize = TABLESIZE;} 
  //always use base tablesize as runtime reference, even with quintic interpolation
  psFloatShaper->m_lTableSize = TABLESIZE;
  

  //allocate memory for the Tables, size doesn't really matter, but a power of two is prefered
  psFloatShaper->m_pfTable = (LADSPA_Data *)calloc(lTableSize, sizeof(LADSPA_Data));
  psFloatShaper->m_pfLinTable = (LADSPA_Data *)calloc(lTableSize, sizeof(LADSPA_Data));
  psFloatShaper->m_pfSinTable = (LADSPA_Data *)calloc(lTableSize, sizeof(LADSPA_Data));
  psFloatShaper->m_pfRectTable = (LADSPA_Data *)calloc(lTableSize, sizeof(LADSPA_Data));
  
  //quit if that doesn't work
  if ((psFloatShaper->m_pfTable == NULL) || (psFloatShaper->m_pfLinTable == NULL) || (psFloatShaper->m_pfSinTable == NULL) || (psFloatShaper->m_pfRectTable == NULL)) {
    free(psFloatShaper->m_pfTable);
    free(psFloatShaper->m_pfLinTable);
    free(psFloatShaper->m_pfSinTable);
    free(psFloatShaper->m_pfRectTable);
    free(psFloatShaper);
    return NULL;
  }
  
  //fill the "constant" tables, divide all values with fPoint to get a relative table. 
  for (i = 0; i < lTableSize; i++) {
    //scale table index -1 to 1
    if (TABLE_INTERP == 2) {fPoint = (LADSPA_Data)(i-1) / (LADSPA_Data)(lTableSize - 1);} // (i-2)/tablehigh, for quitic interpolation (points from -1 to tablehigh+2).      
    else {fPoint = (LADSPA_Data)i / (LADSPA_Data)(lTableSize - 1);} // i/tablehigh
    //linear table (DEFAULT)
    *(psFloatShaper->m_pfTable + i) = 1.; // fpoint/fpoint = 1.
    *(psFloatShaper->m_pfLinTable + i) = 1.;
    //sinoid and rectangle table table
    if (fPoint == 0.) {
      //avoid divide by 0
      *(psFloatShaper->m_pfSinTable + i) = 0.;
      *(psFloatShaper->m_pfRectTable + i) = 0.;
    }
    else {
      //sinoid table
      *(psFloatShaper->m_pfSinTable + i) = sin((LADSPA_Data)PI * fPoint) / fPoint;
      //rectangle table
      if (fPoint < 0.) {*(psFloatShaper->m_pfRectTable + i) = -1. / fPoint;}
      else {*(psFloatShaper->m_pfRectTable + i) = 1. / fPoint;}
    }
  }

  //set memory input to default
  psFloatShaper->m_fLastDrive = 1.;
  psFloatShaper->m_fLastShape = 0.;

  return psFloatShaper;
}

/*****************************************************************************/

/* Initialise and activate a plugin instance. */
void
activateFloatShaper(LADSPA_Handle Instance) {

  FloatShaper * psFloatShaper;
  psFloatShaper = (FloatShaper *)Instance;
}

/*****************************************************************************/

/* Connect a port to a data location. */
void 
connectPortToFloatShaper(LADSPA_Handle Instance,
			     unsigned long Port,
			     LADSPA_Data * DataLocation) {

  FloatShaper * psFloatShaper;

  psFloatShaper = (FloatShaper *)Instance;
  switch (Port) {
  case FS_DRIVE:
    psFloatShaper->m_pfDrive = DataLocation;
    break;
  case FS_SHAPE:
    psFloatShaper->m_pfShape = DataLocation;
    break;
  case FS_OVERDRIVE:
    psFloatShaper->m_pfOD = DataLocation;
    break;
  case FS_INPUT:
    psFloatShaper->m_pfInput = DataLocation;
    break;
  case FS_OUTPUT:
    psFloatShaper->m_pfOutput = DataLocation;
    break;
  }
}

/*****************************************************************************/

/* Run a FloatShaper instance for a block of SampleCount samples. */
void 
runFloatShaper(LADSPA_Handle Instance,
		   unsigned long SampleCount) {
  //declare pointers for arrays
  LADSPA_Data * pfTable;
  LADSPA_Data * pfLinTable;
  LADSPA_Data * pfSinTable;
  LADSPA_Data * pfRectTable;
  LADSPA_Data * pfInput;
  LADSPA_Data * pfOutput;
  //declare local vars for input data (pointer given from host)
  LADSPA_Data fDrive;
  LADSPA_Data fShape;
  LADSPA_Data fOD;
  //declare pointers for memory data (within struct, might need to be changed)
  LADSPA_Data * pfLastDrive;
  LADSPA_Data * pfLastShape;
  //other local vars
  LADSPA_Data fInputSample;
  LADSPA_Data fInputAbs;
  LADSPA_Data fInputIndex;
  LADSPA_Data fTableSample;
  LADSPA_Data fOutputSample;
  LADSPA_Data fPoint;
  LADSPA_Data fWeight;
  unsigned long lTableSize;
  unsigned long lTableIndex;
  unsigned long i;
  
  //Declare a pointer for the plugin struct and point it towards the current loaded instance
  FloatShaper * psFloatShaper;
  psFloatShaper = (FloatShaper *)Instance;
  
  //create pointers towards the arrays within our instance
  pfTable = psFloatShaper->m_pfTable;
  pfLinTable = psFloatShaper->m_pfLinTable;
  pfSinTable = psFloatShaper->m_pfSinTable;
  pfRectTable = psFloatShaper->m_pfRectTable;
  pfInput = psFloatShaper->m_pfInput;
  pfOutput = psFloatShaper->m_pfOutput;
  
  //load inputdata
  fDrive = *(psFloatShaper->m_pfDrive);
  fShape = *(psFloatShaper->m_pfShape);
  fOD = *(psFloatShaper->m_pfOD);
 
  //point towards memory data
  pfLastDrive = &(psFloatShaper->m_fLastDrive);
  pfLastShape = &(psFloatShaper->m_fLastShape);
  
  //some local vars
  lTableSize = psFloatShaper->m_lTableSize;
  
  //recreate the table if needed
  if ((fDrive != *pfLastDrive) || (fShape != *pfLastShape)) {
    //create a new shape 
    for (i = 0; i < lTableSize; i++) {
      //middle between two shapes, weighted by fShape
      if (fShape < .5) {
        /*fLowPoint = *(pfLinTable++);
        fHighPoint = *(pfSinTable++);*/
        fPoint = (2. * (.5 - fShape) * pfLinTable[i]) + (2. * fShape * pfSinTable[i]);
      }
      else {
        /*fLowPoint = *(pfSinTable++);
        fHighPoint = *(pfRectTable++);*/
        fPoint = ((2. * (.5 - (fShape -.5)) * pfSinTable[i]) + (2. * (fShape-.5) * pfRectTable[i]));
      }
          
      //add the drive
      fPoint *= fDrive;

      //and write the point to the table
      *(pfTable + i) = fPoint;

    }//end of for-loop table writing
    //update last data
    *pfLastDrive = fDrive;
    *pfLastShape = fShape;
  }//end of table writing 
  
//*****DSP: 
  
  //read out the table for each sample in buffer
  for (i= 0; i < SampleCount; i++) {
    
    fInputSample = *(pfInput++);

    //totally ignore if input is zero 
    if (fInputSample == 0.) {
      *(pfOutput++) = 0;
    }
    else {
      
      //get the absolute value of the input to use for the table
      //use fabs instead of bitmasking (strict aliasing rules), it *should* be just as efficient.
      fInputAbs = (LADSPA_Data) fabs(fInputSample);        
      
      //sanitize input.. sigh
      if (fInputAbs > 1.) {fInputAbs = 1.;}    

      //find out what indexes to access and the weight
      fInputIndex = fInputAbs * ((LADSPA_Data)lTableSize - 1.); // abs(input) * tablehigh
      lTableIndex = (unsigned long)floor(fInputIndex);
      fWeight = fInputIndex - (LADSPA_Data)lTableIndex;
        
      //quintic interpolation
      if (TABLE_INTERP == 2) {
        fTableSample = interpolate_quintic(fWeight, pfTable[lTableIndex], pfTable[lTableIndex + 1], pfTable[lTableIndex + 2], pfTable[lTableIndex + 3]);   
      } 
      else {
        //ignore weight at last table index and 'clos(LADSPA_Data)e encounters' for interpolation
        if ((lTableIndex >= (lTableSize -1)) || (fWeight < 0.001) || (TABLE_INTERP == 0)) {
          fTableSample = (pfTable[lTableIndex]);
        }
        else if (fWeight > 0.999) {
          fTableSample = (pfTable[lTableIndex + 1]);
        }
        //linear interpolation    
        else {
          fTableSample = (pfTable[lTableIndex] * (1. - fWeight)) + (pfTable[lTableIndex + 1] * fWeight);
        }
      } // end of linear or zero interpolation
      
      fOutputSample = fInputSample * fTableSample;
      
      //overdrive
      if (fOutputSample > 1.) {
        if (fOD <= .5) {
          //middle between limiting and folding for positive amounts
          fOutputSample = 1. - ((fOutputSample - 1.) * (2. * fOD));
          //or fade between overdrive types
          /*if (fOutputSample < (1. + (fOD * 2))) {fOutputSample = 1.};
          else {fOutputSample = 1. - (fOutputSample -1;}*/ 
        }
        else {
          //middle between folding and warping for positive amounts
          fOutputSample = (2. * (1. - (fOutputSample - 1.)) * (.5 - (fOD -.5))) + (2. * (-1. + (fOutputSample - 1.)) * (fOD - .5));
        }
        //just limit if point is still out of range
        if (fOutputSample > 1.)  {fOutputSample = 1.;}
        else if (fOutputSample < -1.) {fOutputSample = -1.;}
      }  
      //same for negative amounts
      else if (fOutputSample < -1.) {
        if (fOD <= .5) {
          fOutputSample = -1. - ((fOutputSample + 1.) * (2. * fOD));
        }
        else {
          fOutputSample = (2. * (-1. - (fOutputSample + 1.)) * (.5 - (fOD -.5))) + (2. * (1. + (fOutputSample + 1.)) * (fOD - .5));
        }
        if (fOutputSample > 1.)  {fOutputSample = 1.;}
        else if (fOutputSample < -1.) {fOutputSample = -1.;}
      }      

      //write non-zero output
      *(pfOutput++) = fOutputSample; 
    
    }//end of non-zero sample-handing
    
  }//end of bufferloop 
      //finished
}

/*****************************************************************************/

/* Throw away a FloatShaper. */
void 
cleanupFloatShaper(LADSPA_Handle Instance) {

  FloatShaper * psFloatShaper;

  psFloatShaper = (FloatShaper *)Instance;
  //free the tables
  free(psFloatShaper->m_pfTable);
  free(psFloatShaper->m_pfLinTable);
  free(psFloatShaper->m_pfSinTable);
  free(psFloatShaper->m_pfRectTable);
  //and the rest
  free(psFloatShaper);
}

/*****************************************************************************/

LADSPA_Descriptor * g_psDescriptor = NULL;

/*****************************************************************************/

/* _init() is called automatically when the plugin library is first
   loaded. */
void 
_init() {

  char ** pcPortNames;
  LADSPA_PortDescriptor * piPortDescriptors;
  LADSPA_PortRangeHint * psPortRangeHints;

  g_psDescriptor
    = (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor));
  if (g_psDescriptor) {
    g_psDescriptor->UniqueID
      = 701;

    g_psDescriptor->Label
      = strdup("floatshaper");
    g_psDescriptor->Properties
      = LADSPA_PROPERTY_HARD_RT_CAPABLE;
    g_psDescriptor->Name 
      = strdup("FloatShaper");
    g_psDescriptor->Maker
      = strdup("BlastDoors");
    g_psDescriptor->Copyright
      = strdup("Use at own risk.");
    g_psDescriptor->PortCount 
      = 5;
    piPortDescriptors
      = (LADSPA_PortDescriptor *)calloc(5, sizeof(LADSPA_PortDescriptor));
    g_psDescriptor->PortDescriptors 
      = (const LADSPA_PortDescriptor *)piPortDescriptors;
    piPortDescriptors[FS_DRIVE]
      = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
    piPortDescriptors[FS_SHAPE]
      = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
    piPortDescriptors[FS_OVERDRIVE]
      = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
    piPortDescriptors[FS_INPUT]
      = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
    piPortDescriptors[FS_OUTPUT]
      = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
    pcPortNames
      = (char **)calloc(5, sizeof(char *));
    g_psDescriptor->PortNames
      = (const char **)pcPortNames;
    pcPortNames[FS_DRIVE]
      = strdup("Drive of Table");
    pcPortNames[FS_SHAPE] 
      = strdup("Shape of Table");
    pcPortNames[FS_OVERDRIVE] 
      = strdup("Overdrive Type");
    pcPortNames[FS_INPUT] 
      = strdup("Input");
    pcPortNames[FS_OUTPUT]
      = strdup("Output");
    psPortRangeHints = ((LADSPA_PortRangeHint *)
			calloc(5, sizeof(LADSPA_PortRangeHint)));
    g_psDescriptor->PortRangeHints
      = (const LADSPA_PortRangeHint *)psPortRangeHints;
    psPortRangeHints[FS_DRIVE].HintDescriptor
      = (LADSPA_HINT_BOUNDED_BELOW 
	 | LADSPA_HINT_BOUNDED_ABOVE
	 | LADSPA_HINT_DEFAULT_1);
    psPortRangeHints[FS_DRIVE].LowerBound 
      = 0;
    psPortRangeHints[FS_DRIVE].UpperBound
      = (LADSPA_Data)MAXDRIVE;
    psPortRangeHints[FS_SHAPE].HintDescriptor
      = (LADSPA_HINT_BOUNDED_BELOW 
	 | LADSPA_HINT_BOUNDED_ABOVE
	 | LADSPA_HINT_DEFAULT_MINIMUM);
    psPortRangeHints[FS_SHAPE].LowerBound 
      = 0;
    psPortRangeHints[FS_SHAPE].UpperBound
      = 1;
    psPortRangeHints[FS_OVERDRIVE].HintDescriptor
      = (LADSPA_HINT_BOUNDED_BELOW 
	 | LADSPA_HINT_BOUNDED_ABOVE
	 | LADSPA_HINT_DEFAULT_MINIMUM);
    psPortRangeHints[FS_OVERDRIVE].LowerBound 
      = 0;
    psPortRangeHints[FS_OVERDRIVE].UpperBound
      = 1;
    psPortRangeHints[FS_INPUT].HintDescriptor
      = 0;
    psPortRangeHints[FS_OUTPUT].HintDescriptor
      = 0;
    g_psDescriptor->instantiate
      = instantiateFloatShaper;
    g_psDescriptor->connect_port 
      = connectPortToFloatShaper;
    g_psDescriptor->activate
      = activateFloatShaper;
    g_psDescriptor->run 
      = runFloatShaper;
    g_psDescriptor->run_adding
      = NULL;
    g_psDescriptor->set_run_adding_gain
      = NULL;
    g_psDescriptor->deactivate
      = NULL;
    g_psDescriptor->cleanup
      = cleanupFloatShaper;
  }
}

/*****************************************************************************/

/* _fini() is called automatically when the library is unloaded. */
void 
_fini() {
  long lIndex;
  if (g_psDescriptor) {
    free((char *)g_psDescriptor->Label);
    free((char *)g_psDescriptor->Name);
    free((char *)g_psDescriptor->Maker);
    free((char *)g_psDescriptor->Copyright);
    free((LADSPA_PortDescriptor *)g_psDescriptor->PortDescriptors);
    for (lIndex = 0; lIndex < g_psDescriptor->PortCount; lIndex++)
      free((char *)(g_psDescriptor->PortNames[lIndex]));
    free((char **)g_psDescriptor->PortNames);
    free((LADSPA_PortRangeHint *)g_psDescriptor->PortRangeHints);
    free(g_psDescriptor);
  }
}

/*****************************************************************************/

/* Return a descriptor of the requested plugin type. Only one plugin
   type is available in this library. */
const LADSPA_Descriptor * 
ladspa_descriptor(unsigned long Index) {
  if (Index == 0)
    return g_psDescriptor;
  else
    return NULL;
}

/*****************************************************************************/

/* EOF */
