/**
 * @file orxTime.cpp
 * 
 * SFML time plugin
 * 
 */
 
 /***************************************************************************
 orxTime.cpp
 SFML time plugin
 
 begin                : 25/10/2007
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

extern "C"
{
#include "orxInclude.h"

#include "memory/orxMemory.h"
#include "math/orxMath.h"
#include "plugin/orxPluginUser.h"

#include "core/orxTime.h"
}

#include <SFML/Window.hpp>

/** Module flags
 */
#define orxTIME_KU32_STATIC_FLAG_NONE   0x00000000  /**< No flags have been set */
#define orxTIME_KU32_STATIC_FLAG_READY  0x00000001  /**< The module has been initialized */


/***************************************************************************
 * Structure declaration                                                   *
 ***************************************************************************/

/** Static structure
 */
typedef struct __orxTIME_STATIC_t
{
  orxU32      u32Flags;
  sf::Clock  *poClock;

} orxTIME_STATIC;


/***************************************************************************
 * Static variables                                                        *
 ***************************************************************************/

/** Static data
 */
orxSTATIC orxTIME_STATIC sstTime;


/***************************************************************************
 * Private functions                                                       *
 ***************************************************************************/


/***************************************************************************
 * Public functions                                                        *
 ***************************************************************************/

/** Init the time module
 * @return Returns the status of the operation
 */
extern "C" orxSTATUS orxTime_SFML_Init()
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  
  /* Was already initialized. */
  if(!(sstTime.u32Flags & orxTIME_KU32_STATIC_FLAG_READY))
  {
    /* Cleans static controller */
    orxMemory_Set(&sstTime, 0, sizeof(orxTIME_STATIC));

    /* Inits time clock */
    sstTime.poClock = new sf::Clock();

    /* Resets it */
    sstTime.poClock->Reset();

    /* Updates status */
    sstTime.u32Flags |= orxTIME_KU32_STATIC_FLAG_READY;
  }

  /* Done! */
  return eResult;  
}

/** Exit the time module
 */
extern "C" orxVOID orxTime_SFML_Exit()
{
  /* Module initialized ? */
  if((sstTime.u32Flags & orxTIME_KU32_STATIC_FLAG_READY) == orxTIME_KU32_STATIC_FLAG_READY)
  {
    /* Deletes clock */
    delete sstTime.poClock;

    /* Cleans static controller */
    orxMemory_Set(&sstTime, 0, sizeof(orxTIME_STATIC));
  }

  return;
}

/** Gets App Elapsed time.
 * @return Returns the amount of seconds elapsed from the application start.
 */
extern "C" orxFLOAT orxTime_SFML_GetTime()
{
  /* Module initialized ? */
  orxASSERT((sstTime.u32Flags & orxTIME_KU32_STATIC_FLAG_READY) == orxTIME_KU32_STATIC_FLAG_READY);

  return(orx2F(sstTime.poClock->GetElapsedTime()));
}

/** Delay the program for given number of milliseconds.
 * @param[in] _fTime Number of seconds to wait.
 */
extern "C" orxVOID orxTime_SFML_Delay(orxFLOAT _fTime)
{
  /* Module initialized ? */
  orxASSERT((sstTime.u32Flags & orxTIME_KU32_STATIC_FLAG_READY) == orxTIME_KU32_STATIC_FLAG_READY);

  /* Sleeps */
  sf::Sleep(_fTime);
}


/********************
 *  Plugin Related  *
 ********************/

orxPLUGIN_USER_CORE_FUNCTION_CPP_START(DISPLAY);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxTime_SFML_Init, TIME, INIT);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxTime_SFML_Exit, TIME, EXIT);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxTime_SFML_GetTime, TIME, GET_TIME);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxTime_SFML_Delay, TIME, DELAY);
orxPLUGIN_USER_CORE_FUNCTION_END();
