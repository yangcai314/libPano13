/* queryfeature.c
   Feature querying functionality
   See queryfeature.h
*/
#include <stdio.h>
#include <string.h>

#include "version.h"
#include "queryfeature.h"
#include "filter.h"

typedef struct {char* name; int value;}    TIntFeature;
typedef struct {char* name; double value;} TDoubleFeature;
typedef struct {char* name; char* value;}  TStringFeature;

// Fulvio Senore June.2004 changed the check to work with microsoft compiler
#ifdef _MSC_VER
//#ifdef MSVS
#define snprintf _snprintf
#endif

/***************** Feature tables: *************************/
TIntFeature intFeatures[] ={
  {"CPErrorIsDistSphere",1},       // optimizer reports angular control point errors
  {"NumLensTypes",5}               // source lens types 0..4
};

TDoubleFeature doubleFeatures[] ={
  {"MaxFFOV",MAX_FISHEYE_FOV}
};

TStringFeature stringFeatures[]={
  // Version info:
  {PTVERSION_NAME_FILEVERSION,PTVERSION_FILEVERSION},         // "FileVersion"
  {PTVERSION_NAME_LONG,LONGVERSION},                          // "LongVersion"
  {PTVERSION_NAME_LEGALCOPYRIGHT,PTVERSION_LEGALCOPYRIGHT},   // "LegalCopyright"
  {PTVERSION_NAME_COMMENT,PTVERSION_COMMENT},                 // "Comments"
  // Source lens type names
  // If a lens type is unavailable, set its name to ""
  {"LensType0","Normal (rectilinear)"},
  {"LensType1","Cylindrical"},
  {"LensType2","Circular"},
  {"LensType3","Full Frame"},
  {"LensType4","Equirectangular"},
  // Source lens type crop format (C)ircle or (R)ectangle:
  {"LensMask0","R"},
  {"LensMask1","R"},
  {"LensMask2","C"},
  {"LensMask3","R"},
  {"LensMask4","R"},
  // Patches that have been applied
  {"Patch200405a", "Rik Littlefield, Improved optimizer, http://www.janrik.ptools"},
  {"Patch200403a", "Kevin Kratzke, Radial Shift fix, http://www.kekus.com"},
  {"Patch200312a", "Jim Watters, Updated PSD format, http://members.rogers.com/jimwatters/panotools"},
  {"Patch200309a", "Jim Watters, Allowed linking of y, p, & r values, http://members.rogers.com/jimwatters/panotools"},
  {"Patch200308a", "Jim Watters, Improved Radial Luminance, http://members.rogers.com/jimwatters/panotools"}
};
/***************** end feature tables **********************/


int queryFeatureInt(const char *name, int *result)
{
  int i;
  int count = sizeof( intFeatures ) / sizeof( intFeatures[0] );
  for(i=0; i < count; i++)
  {
    if(strcmp(name,intFeatures[i].name)==0)
    {
      *result=intFeatures[i].value;
      return 1;
    }
  }
  return 0;
}

int queryFeatureDouble(const char *name, double *result)
{
  int i;
  int count = sizeof( doubleFeatures ) / sizeof( doubleFeatures[0] );
  for(i=0; i < count; i++)
  {
    if(strcmp(name,doubleFeatures[i].name)==0)
    {
      *result=doubleFeatures[i].value;
      return 1;
    }
  }
  return 0;
}

int queryFeatureString(const char *name,char *result, const int bufsize)
{
  int i,intvalue;
  int count = sizeof( stringFeatures ) / sizeof( stringFeatures[0] );
  double doublevalue;
  int length=0;

  for(i=0; i < count; i++)
  {
    if(strcmp(name,stringFeatures[i].name)==0)
    {
      if(result != NULL)
      {
        strncpy(result,stringFeatures[i].value,bufsize);
      }
      length=strlen(stringFeatures[i].value);
      break;
    }
  }
  if(length <= 0)
  {
    // there's no string value with the specified name
    // Let's check the int values too:
    for(i=0; i < sizeof( intFeatures ) / sizeof( intFeatures[0] ); i++)
    {
      if(queryFeatureInt(name, &intvalue))
      {
        length=snprintf(NULL,0,"%d",intvalue);
        if(result != NULL)
        {
          snprintf(result,bufsize,"%d",intvalue);
        }
        break;
      }
    }
  }
  if(length <= 0)
  {
    // Let's check the double values too:
    for(i=0; i < sizeof( doubleFeatures ) / sizeof( doubleFeatures[0] ); i++)
    {
      if(queryFeatureDouble(name, &doublevalue))
      {
        length=snprintf(NULL,0,"%0.f",doublevalue);
        if(result != NULL)
        {
          snprintf(result,bufsize,"%0.f",doublevalue);
        }
        break;
      }
    }
  }
  // make sure that the copied string always is NULL terminated, even if truncated
  // (except if the buffer holds only zero bytes):
  if( result && (length >= bufsize) && (bufsize > 0) )
  {
    result[bufsize-1]=0;
  }
  return length;
}

int queryFeatureCount()
{
  return sizeof( intFeatures ) / sizeof( intFeatures[0] )+ 
	     sizeof( doubleFeatures ) / sizeof( doubleFeatures[0] ) + 
		 sizeof( stringFeatures ) / sizeof( stringFeatures[0] );
}

void queryFeatures(int index,char** name,Tp12FeatureType* type)
{
  if(index < sizeof( intFeatures ) / sizeof( intFeatures[0] ))
  {
    if(name) *name=intFeatures[index].name;
    if(type) *type=p12FeatureInt;
  }
  else
  {
    index -= sizeof( intFeatures ) / sizeof( intFeatures[0] );
    if(index < sizeof( doubleFeatures ) / sizeof( doubleFeatures[0] ))
    {
      if(name) *name=doubleFeatures[index].name;
      if(type) *type=p12FeatureDouble;
    }
    else
    {
      index -= sizeof( doubleFeatures ) / sizeof( doubleFeatures[0] );
      if(index < sizeof( stringFeatures ) / sizeof( stringFeatures[0] ))
      {
        if(name) *name=stringFeatures[index].name;
        if(type) *type=p12FeatureString;
      }
      else
      {
        if(type) *type=p12FeatureUnknown;
      }
    }
  }
}

