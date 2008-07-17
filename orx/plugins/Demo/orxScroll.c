/**
 * @file orxScroll.c
 *
 * Automated scrolling demo
 */

/***************************************************************************
 begin                : 22/10/2007
 author               : (C) Arcallians
 email                : iarwain@arcallians.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License           *
 *   as published by the Free Software Foundation; either version 2.1      *
 *   of the License, or (at your option) any later version.                *
 *                                                                         *
 ***************************************************************************/


#include "orx.h"


/** Misc defines
 */
#define orxSCROLL_RESOURCE_NUMBER   5

/** Resource names
 */
orxSTATIC orxSTRING sazResourceNames[orxSCROLL_RESOURCE_NUMBER] =
{
 "Fuji",
 "Boat1",
 "Boat2",
 "Cloud",
 "Wave"
};


/** Camera
 */
orxSTATIC orxCAMERA *spstCamera;

/** Overlay (for fade in / fade out)
 */
orxSTATIC orxOBJECT *spstOverlay;

/** Root
 */
orxSTATIC orxOBJECT *spstRoot;


/** Update callback used to update the scrolling. N.B.: it could also have been done through FXs, doing the synchro in the .ini, and no clock would have been needed.
 */
orxVOID orxFASTCALL orxScroll_Update(orxCONST orxCLOCK_INFO *_pstClockInfo, orxVOID *_pstContext)
{
  orxVECTOR vPos;
  orxFLOAT  fMove;

  /* Selects scroll section */
  orxConfig_SelectSection("Scroll");

  /* Computes move delta */
  fMove = _pstClockInfo->fDT * orxConfig_GetFloat("ScrollingSpeed");

  /* Gets root position */
  orxObject_GetPosition(spstRoot, &vPos);

  /* End of scrolling? */
  if(vPos.fX >= orxConfig_GetFloat("ScrollingMax")
  && (vPos.fX - orxConfig_GetFloat("ScrollingMax") < fMove))
  {
    /* Adds fade FX on overlay */
    orxObject_AddFX(spstOverlay, "FadeFX");

    /* Adds reinit FX on root */
    orxObject_AddFX(spstRoot, "ReinitFX");
  }

  /* Updates scrolling */
  vPos.fX += fMove;

  /* Updates root position */
  orxObject_SetPosition(spstRoot, &vPos);
}

/** Inits the scroll demo
 */
orxSTATIC orxSTATUS orxScroll_Init()
{
  orxS32      i, s32WaveGroupNumber;
  orxOBJECT  *apstWaveGroupList[32];
  orxSTATUS   eResult = orxSTATUS_FAILURE;

  /* Loads config file and selects its section */
  orxConfig_Load("Scroll.ini");
  orxConfig_SelectSection("Scroll");

  /* Gets wave group number */
  s32WaveGroupNumber = orxMIN(orxConfig_GetS32("WaveGroupNumber"), 32);

  /* Inits the random seed */
  orxFRAND_INIT(orx2F(1000000.0f) * orxSystem_GetTime());

  /* For all wave groups */
  for(i = 0; i < s32WaveGroupNumber; i++)
  {
    orxCHAR acWaveGroupID[32];

    /* Gets its name */
    orxString_Print(acWaveGroupID, "WaveGroup%d", i + 1);

    /* Creates it */
    apstWaveGroupList[i] = orxObject_CreateFromConfig(acWaveGroupID);

    /* Adds wave FX to it */
    orxObject_AddDelayedFX(apstWaveGroupList[i], "WaveFX", orxS2F(i));
  }

  /* For all resources */
  for(i = 0; i < orxSCROLL_RESOURCE_NUMBER; i++)
  {
    orxS32 j;

    /* Selects config section */
    orxConfig_SelectSection(sazResourceNames[i]);

    /* For all requested instances */
    for(j = 0; j < orxConfig_GetS32("Number"); j++)
    {
      orxOBJECT *pstObject;

      /* Creates the object */
      pstObject = orxObject_CreateFromConfig(sazResourceNames[i]);

      /* Selects resource section */
      orxConfig_SelectSection(sazResourceNames[i]);

      /* Is on wave? */
      if(orxConfig_GetBool("OnWave"))
      {
        /* Assigns it to one of the wave group */
        orxObject_SetParent(pstObject, apstWaveGroupList[j % s32WaveGroupNumber]);
      }
    }
  }

  /* Success? */
  if(i == orxSCROLL_RESOURCE_NUMBER)
  {
    orxCLOCK     *pstClock;
    orxVIEWPORT  *pstViewport;
    orxOBJECT    *pstBackground;

    /* Selects main section */
    orxConfig_SelectSection("Scroll");

    /* Creates viewport */
    pstViewport = orxViewport_CreateFromConfig("ScrollViewport");

    /* Creates root */
    spstRoot = orxObject_CreateFromConfig("Root");

    /* Creates background */
    pstBackground = orxObject_CreateFromConfig("Background");

    /* Creates overlay */
    spstOverlay = orxObject_CreateFromConfig("Overlay");

    /* Stores camera pointer */
    spstCamera = orxViewport_GetCamera(pstViewport);

    /* Links camera to root */
    orxFrame_SetParent(orxCamera_GetFrame(spstCamera), orxOBJECT_GET_STRUCTURE(spstRoot, FRAME));

    /* Links background & overlay to camera */
    orxFrame_SetParent(orxOBJECT_GET_STRUCTURE(pstBackground, FRAME), orxCamera_GetFrame(spstCamera));
    orxFrame_SetParent(orxOBJECT_GET_STRUCTURE(spstOverlay, FRAME), orxCamera_GetFrame(spstCamera));

    /* Gets rendering clock */
    pstClock = orxClock_FindFirst(orx2F(-1.0f), orxCLOCK_TYPE_RENDER);

    /* Registers update function */
    eResult = orxClock_Register(pstClock, orxScroll_Update, orxNULL, orxMODULE_ID_MAIN);
  }

  /* Done! */
  return eResult;
}

/** Declares the demo entry point */
orxPLUGIN_DECLARE_ENTRY_POINT(orxScroll_Init);
