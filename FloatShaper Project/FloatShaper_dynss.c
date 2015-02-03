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
#define TABLE_INTERP 1
/*****************************************************************************/

/* The port numbers for the plugin: */

#define FS_DRIVE 	0
#define FS_SHAPE	1
#define FS_OVERDRIVE    2
#define FS_INPUT        3
#define FS_OUTPUT       4

/*****************************************************************************/

#define PI 3.14159265

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
  LADSPA_Data m_fLastOD;

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
  lTableSize = TABLESIZE;
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

  //fill the "constant" tables
  for (i=0; i < lTableSize; i++) {
    //scale table index -1 to 1
    fPoint = -1. + (((LADSPA_Data)i * 2.) / (LADSPA_Data)(lTableSize - 1)); //-1 + (i*2)/tablehigh
    //linear table (DEFAULT)
    *(psFloatShaper->m_pfTable + i) = fPoint;
    *(psFloatShaper->m_pfLinTable + i) = fPoint;
    //sinoid table
    *(psFloatShaper->m_pfSinTable + i) = (LADSPA_Data)sin((LADSPA_Data)PI * fPoint);
    //rectangle table
    if (fPoint < 0.) {*(psFloatShaper->m_pfRectTable + i) = -1.;}
    else {*(psFloatShaper->m_pfRectTable + i) = 1.;}
  }

  //set memory input to default
  psFloatShaper->m_fLastDrive = 1.;
  psFloatShaper->m_fLastShape = 0.;
  psFloatShaper->m_fLastOD = 0.;

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
  LADSPA_Data * pfLastOD;
  //other local vars
  LADSPA_Data fInputSample;
  LADSPA_Data fInputIndex;
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
  pfLastOD = &(psFloatShaper->m_fLastOD);
  
  //some local vars
  lTableSize = psFloatShaper->m_lTableSize;

  //recreate the table if needed
  if ((fDrive != *pfLastDrive) || (fShape != *pfLastShape) || (fOD != *pfLastOD)) {
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
      //middle between overdrive types, weighted by fOD. original was fading overdrive types, this might be a bit crazier.
      if (fPoint > 1.) {
        if (fOD <= .5) {
            //or fade between overdrive types
            //if (fPoint < (1. + (fOD * 2))) {fPoint = 1.};
            //else {fPoint = 1. - (fPoint -1;}
          //middle between limiting and folding for positive amounts
          fPoint = 1. - ((fPoint - 1.) * (2. * fOD));
        }
        else {
          //middle between folding and warping for positive amounts
          fPoint = (2. * (1. - (fPoint - 1.)) * (.5 - (fOD -.5))) + (2. * (-1. + (fPoint - 1.)) * (fOD - .5));
        }
        //just limit if point is still out of range
        if (fPoint > 1.)  {fPoint = 1.;}
        else if (fPoint < -1.) {fPoint = -1.;}
      }  
      //same for negative amounts
      else if (fPoint < -1.) {
        if (fOD <= .5) {
          fPoint = -1. - ((fPoint + 1.) * (2. * fOD));
        }
        else {
          fPoint = (2. * (-1. - (fPoint + 1.)) * (.5 - (fOD -.5))) + (2. * (1. + (fPoint + 1.)) * (fOD - .5));
        }
        if (fPoint > 1.)  {fPoint = 1.;}
        else if (fPoint < -1.) {fPoint = -1.;}
      }
      //and finally write the point to the table
      *(pfTable + i) = fPoint;
    }//end of for-loop table writing
    //update last data
    *pfLastDrive = fDrive;
    *pfLastShape = fShape;
    *pfLastOD = fOD;
  }//end of table writing 
    
  if (SUPERSAMPLE > 1) {
    //check if ss is power of two
    if (SUPERSAMPLE & (SUPERSAMPLE - 1)) == 0) {lSuperSample = SUPERSAMPLE}
    else {lSuperSample = 1);}
  else {SuperSample = 1;}


  //read out the table for each sample in buffer
  for (i= 0; i < SampleCount * SUPERSAMPLE; i++) {
    
    fInputSample = *(pfInput++);

    //sanitize input.. sigh
    if (fInputSample > 1.) {fInputSample = 1.;}
    else if (fInputSample < -1.) {fInputSample = -1.;}
    
    //do supersampling if needed, linear interpolation
    for (j = 1; j <= lSuperSample; j++) {
      if (lSuperSample > 1) {
        fSSweight = (LADSPA_Data)j / (LADSPA_Data)lSuperSample); //could be better, supersample is always power of 2
        fInputSample = fLastInputSample * (1. - fSSweight) + fInputSample * fSSweight; 
      }   

      //find out what indexes to access and the weight
      fInputIndex = (fInputSample + 1.) * ((LADSPA_Data)lTableSize - 1.) * .5; // ((input+1) * tablehigh) / 2
      lTableIndex = (unsigned long)floor(fInputIndex);
      fWeight = fInputIndex - (LADSPA_Data)lTableIndex;
      
      //run each (supersampled) sample through the table
      //totally ignore if input is zero 
      if (fInputSample == 0.) {
        fOutputSample = 0.;
      }
      //ignore weight at last table index and 'close encounters' for interpolation
      else if ((lTableIndex >= (lTableSize -1)) || (fWeight < 0.001) || (TABLE_INTERP == 0)) {
        fOutputSample = (pfTable[lTableIndex]);
      }
      else if (fWeight > 0.999) {
        fOutputSample = (pfTable[lTableIndex + 1]);
      
      else {
        fOutPutSample = (pfTable[lTableIndex] * (1. - fWeight)) + (pfTable[lTableIndex + 1] * fWeight);
      }
      
      //buffer the supersampled output
      if (lSuperSample > 1) {
        *((fSSbuffer + j) & fSSbufferSize) = fOutPutSample;
      }
      
    } //end of supersampleloop
    
    //filter and downsample the buffer
   if (lSuperSample > 1) 

    *(pfOutput++) =
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
