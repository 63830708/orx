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
#include "graph/graph.h"
#include "io/orxFile.h"
#include "memory/orxMemory.h"
#include "msg/msg_screenshot.h"
#include "utils/orxString.h"


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
  orxSTATUS eResult = orxSTATUS_SUCCESS;
  orxFILE_SEARCH_INFOS stFileInfos;

  /* Not already Initialized? */
  if(!(sstScreenshot.u32Flags & orxSCREENSHOT_KU32_FLAG_READY))
  {
    /* Cleans control structure */
    orxMemory_Set(&sstScreenshot, 0, sizeof(orxSCREENSHOT_STATIC));

    /* Valid? */
    if(orxFile_Exists(orxSCREENSHOT_KZ_DIRECTORY) != orxFALSE)
    {
      /* Gets file to find name */
      orxString_Printf(zFileName, "%s/%s*.*", orxSCREENSHOT_KZ_DIRECTORY, orxSCREENSHOT_KZ_PREFIX);

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
    }
    else
    {
        /* !!! MSG !!! */

        /* Can't find folder */
        eResult = orxSTATUS_FAILED;
    }
  }
  else
  {
    /* !!! MSG !!! */

    /* Already initialized */
    eResult = orxSTATUS_FAILED;
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
  orxString_Printf(zName, "%s/%s-%04ld.%s", orxSCREENSHOT_KZ_DIRECTORY, orxSCREENSHOT_KZ_PREFIX, sstScreenshot.u32Counter, orxSCREENSHOT_KZ_EXT);

  /* Saves it */
  graph_bitmap_save(zName, graph_screen_bitmap_get());

  /* Logs */
  orxDEBUG_LOG(orxDEBUG_LEVEL_SCREENSHOT, KZ_MSG_SHOT_TAKEN_S, zName);

  /* Updates screenshot counter */
  sstScreenshot.u32Counter++;

  return;
}
