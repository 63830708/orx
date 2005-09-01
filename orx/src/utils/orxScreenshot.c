/***************************************************************************
 orxScreenshot.c
 Screenshot module
 
 begin                : 07/12/2003
 author               : (C) Arcallians
 email                : iarwain@arcallians.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "utils/orxScreenshot.h"

#include <string.h>
#include <stdio.h>

#include "debug/orxDebug.h"
#include "display/orxDisplay.h"
#include "io/orxFile.h"
#include "io/orxTextIO.h"
#include "memory/orxMemory.h"
#include "msg/msg_screenshot.h"


/*
 * Platform independant defines
 */

#define orxSCREENSHOT_KU32_FLAG_NONE            0x00000000
#define orxSCREENSHOT_KU32_FLAG_READY           0x00000001

/*
 * Static structure
 */
typedef struct __orxSCREENSHOT_STATIC_t
{
  /* Counter */
  orxU32 u32Counter;

  /* Control flags */
  orxU32 u32Flags;

} orxSCREENSHOT_STATIC;

/*
 * Static data
 */
orxSTATIC orxSCREENSHOT_STATIC sstScreenshot;



/***************************************************************************
 ***************************************************************************
 ******                       LOCAL FUNCTIONS                         ******
 ***************************************************************************
 ***************************************************************************/


/***************************************************************************
 ***************************************************************************
 ******                       PUBLIC FUNCTIONS                        ******
 ***************************************************************************
 ***************************************************************************/

/***************************************************************************
 orxScreenshot_Init
 Inits the screenshot system.

 returns: orxSTATUS_SUCCESS/orxSTATUS_FAILED
 ***************************************************************************/
orxSTATUS orxScreenshot_Init()
{
  orxCHAR zFileName[256];
  orxFILE_INFO stFileInfos;
  orxSTATUS eResult = orxSTATUS_FAILED;

  /* Init dependencies */
  if ((orxMAIN_INIT_MODULE(Memory)  == orxSTATUS_SUCCESS) &&
      (orxMAIN_INIT_MODULE(File)    == orxSTATUS_SUCCESS) &&
      (orxMAIN_INIT_MODULE(TextIO)  == orxSTATUS_SUCCESS) &&
      (orxMAIN_INIT_MODULE(Display) == orxSTATUS_SUCCESS))
  {
    /* Not already Initialized? */
    if(!(sstScreenshot.u32Flags & orxSCREENSHOT_KU32_FLAG_READY))
    {
      /* Cleans control structure */
      orxMemory_Set(&sstScreenshot, 0, sizeof(orxSCREENSHOT_STATIC));
  
      /* Valid? */
      if(orxFile_Exists(orxSCREENSHOT_KZ_DIRECTORY) != orxFALSE)
      {
        /* Gets file to find name */
        orxTextIO_Printf(zFileName, "%s/%s*.*", orxSCREENSHOT_KZ_DIRECTORY, orxSCREENSHOT_KZ_PREFIX);
  
        /* Finds first screenshot file */
        if(orxFile_FindFirst(zFileName, &stFileInfos) != orxFALSE)
        {
          do
          {
            /* Updates screenshot counter */
            sstScreenshot.u32Counter++;
          }
          /* Till all screenshots have been found */
          while(orxFile_FindNext(&stFileInfos) != orxFALSE);
          
          /* Ends the search */
          orxFile_FindClose(&stFileInfos);
        }
  
        /* Inits Flags */
        sstScreenshot.u32Flags = orxSCREENSHOT_KU32_FLAG_READY;
        
        /* Success */
        eResult = orxSTATUS_SUCCESS;
      }
      else
      {
          /* !!! MSG !!! */
  
          /* Can't find folder */
      }
    }
    else
    {
      /* !!! MSG !!! */
  
      /* Already initialized */
    }
  }

  /* Done! */
  return eResult;
}

/***************************************************************************
 orxScreenshot_Exit
 Exits from the screenshot system.

 returns: orxSTATUS_SUCCESS/orxSTATUS_FAILED
 ***************************************************************************/
orxVOID orxScreenshot_Exit()
{
  /* Initialized? */
  if(sstScreenshot.u32Flags & orxSCREENSHOT_KU32_FLAG_READY)
  {
    /* Updates flags */
    sstScreenshot.u32Flags &= ~orxSCREENSHOT_KU32_FLAG_READY;
  }
  else
  {
    /* !!! MSG !!! */
  }

  /* Exit dependencies */
  orxMAIN_EXIT_MODULE(Display);
  orxMAIN_EXIT_MODULE(TextIO);
  orxMAIN_EXIT_MODULE(File);
  orxMAIN_EXIT_MODULE(Memory);

  return;
}

/***************************************************************************
 orxScreenshot_Take
 Takes a screenshot.

 returns: orxSTATUS_SUCCESS/orxSTATUS_FAILED
 ***************************************************************************/
orxVOID orxScreenshot_Take()
{
  orxCHAR zName[256];

  /* Checks */
  orxASSERT(sstScreenshot.u32Flags & orxSCREENSHOT_KU32_FLAG_READY)

  /* Computes screenshot name */
  orxTextIO_Printf(zName, "%s/%s-%04ld.%s", orxSCREENSHOT_KZ_DIRECTORY, orxSCREENSHOT_KZ_PREFIX, sstScreenshot.u32Counter, orxSCREENSHOT_KZ_EXT);

  /* Saves it */
  orxDisplay_SaveBitmap(orxDisplay_GetScreenBitmap(), zName);

  /* Logs */
  orxDEBUG_LOG(orxDEBUG_LEVEL_SCREENSHOT, KZ_MSG_SHOT_TAKEN_S, zName);

  /* Updates screenshot counter */
  sstScreenshot.u32Counter++;

  return;
}
