/**
 * @file orxMain.c
 *
 * Main program implementation
 *
 */

 /***************************************************************************
 orxMain.c
 Main program implementation

 begin                : 04/09/2005
 author               : (C) Arcallians
 email                : bestel@arcallians.org
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

/** Flags
 */
#define orxMAIN_KU32_STATIC_FLAG_NONE   0x00000000  /**< No flags */
#define orxMAIN_KU32_STATIC_FLAG_READY  0x00000001  /**< Ready flag */

#define orxMAIN_KU32_STATIC_FLAG_EXIT   0x00000002  /**< Exit flag */

#define orxMAIN_KU32_STATIC_MASK_ALL    0xFFFFFFFF  /**< All mask */

/** Misc defines
 */
#define orxMAIN_KZ_CONFIG_SECTION       "Main"      /**< Main config section */
#define orxMAIN_KZ_CONFIG_GAME_FILE     "GameFile"  /**< Game file config key */


/***************************************************************************
 * Structure declaration                                                   *
 ***************************************************************************/

typedef struct __orxMAIN_STATIC_t
{
  orxU32 u32Flags;       /**< Control flags */

} orxMAIN_STATIC;

/***************************************************************************
 * Module global variable                                                  *
 ***************************************************************************/

orxSTATIC orxMAIN_STATIC sstMain;


/***************************************************************************
 * Functions                                                               *
 ***************************************************************************/

/** Main event handler
 * @param[in]   _pstEvent                     Sent event
 * @return      orxSTATUS_SUCCESS if handled / orxSTATUS_FAILURE otherwise
 */
orxSTATIC orxSTATUS orxFASTCALL orxMain_EventHandler(orxCONST orxEVENT *_pstEvent)
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Checks */
  orxASSERT(_pstEvent->eType == orxEVENT_TYPE_SYSTEM);

  /* Depending on event ID */
  switch(_pstEvent->eID)
  {
    /* Close event */
    case orxSYSTEM_EVENT_CLOSE:
    {
      /* Updates status */
      sstMain.u32Flags |= orxMAIN_KU32_STATIC_FLAG_EXIT;

      /* Updates result */
      eResult = orxSTATUS_SUCCESS;

      break;
    }

    default:
    {
      break;
    }
  }

  /* Done! */
  return eResult;
}

/** Main module setup
 */
orxVOID orxMain_Setup()
{
  /* Adds module dependencies */
  orxModule_AddDependency(orxMODULE_ID_MAIN, orxMODULE_ID_PARAM);
  orxModule_AddDependency(orxMODULE_ID_MAIN, orxMODULE_ID_CLOCK);
  orxModule_AddDependency(orxMODULE_ID_MAIN, orxMODULE_ID_CONFIG);
  orxModule_AddDependency(orxMODULE_ID_MAIN, orxMODULE_ID_EVENT);
  orxModule_AddDependency(orxMODULE_ID_MAIN, orxMODULE_ID_PLUGIN);

  return;
}

/** Initialize the main module (will initialize all needed modules)
 */
orxSTATUS orxMain_Init()
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Don't call twice the init function */
  if(!(sstMain.u32Flags & orxMAIN_KU32_STATIC_FLAG_READY))
  {
    orxSTRING zGameFileName;

    /* Sets module as initialized */
    orxFLAG_SET(sstMain.u32Flags, orxMAIN_KU32_STATIC_FLAG_READY, orxMAIN_KU32_STATIC_MASK_ALL);

    /* Selects section */
    orxConfig_SelectSection(orxMAIN_KZ_CONFIG_SECTION);

    /* Has game file? */
    if(orxConfig_HasValue(orxMAIN_KZ_CONFIG_GAME_FILE) != orxFALSE)
    {
      /* Gets the game file name */
      zGameFileName = orxConfig_GetString(orxMAIN_KZ_CONFIG_GAME_FILE);

      /* Loads it */
      eResult = (orxPlugin_Load(zGameFileName, zGameFileName) != orxHANDLE_UNDEFINED) ? orxSTATUS_SUCCESS : orxSTATUS_FAILURE;
    }
    else
    {
      /* Success */
      eResult = orxSTATUS_SUCCESS;
    }

    /* Successful? */
    if(eResult != orxSTATUS_FAILURE)
    {
      /* Registers custom system event handler */
      eResult = orxEvent_AddHandler(orxEVENT_TYPE_SYSTEM, orxMain_EventHandler);
    }
  }
  else
  {
    /* !!! MSG !!! */

    /* Already initialized */
    eResult = orxSTATUS_SUCCESS;
  }

  /* Done! */
  return eResult;
}

/** Exit main module
 */
orxVOID orxMain_Exit()
{
  /* Module initialized ? */
  if((sstMain.u32Flags & orxMAIN_KU32_STATIC_FLAG_READY) == orxMAIN_KU32_STATIC_FLAG_READY)
  {
    /* Sets module as not ready */
    orxFLAG_SET(sstMain.u32Flags, orxMAIN_KU32_STATIC_FLAG_NONE, orxMAIN_KU32_STATIC_FLAG_READY);
  }

  /* Done */
  return;
}

/** Run the main engine
 */
orxSTATUS orxMain_Run()
{
  orxSTATUS eResult;

  /* Updates result */
  eResult = (orxFLAG_TEST(sstMain.u32Flags, orxMAIN_KU32_STATIC_FLAG_EXIT)) ? orxSTATUS_FAILURE : orxSTATUS_SUCCESS;

  /* Done! */
  return eResult;
}

/** Main function
 * @param[in] argc  Number of parameters
 * @param[in] argv  List array of parameters
 * @note Since the event function is not registered, the program will not
 * be able to exit properly.
 */
int main(int argc, char **argv)
{
  /* Executes orx */
  orx_Execute(argc, argv, orxMain_Setup, orxMain_Init, orxMain_Run, orxMain_Exit);

  /* Done! */
  return EXIT_SUCCESS;
}
