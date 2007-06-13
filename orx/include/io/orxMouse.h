/**
 * \file orxMouse.h
 */

/***************************************************************************
 begin                : 22/11/2003
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

#ifndef _orxMOUSE_H_
#define _orxMOUSE_H_

#include "orxInclude.h"
#include "plugin/orxPluginCore.h"


/***************************************************************************
 * Functions directly implemented by orx core
 ***************************************************************************/

/** Mouse module setup */
extern orxDLLAPI orxVOID                              orxMouse_Setup();


/***************************************************************************
 * Functions extended by plugins
 ***************************************************************************/

orxPLUGIN_DECLARE_CORE_FUNCTION(orxMouse_Init, orxSTATUS);
orxPLUGIN_DECLARE_CORE_FUNCTION(orxMouse_Exit, orxVOID);
orxPLUGIN_DECLARE_CORE_FUNCTION(orxMouse_GetMove, orxSTATUS, orxS32 *, orxS32 *);


/** Init the mouse module
 * @return Returns the status of the operation
 */
orxSTATIC orxINLINE orxSTATUS orxMouse_Init()
{
  return orxPLUGIN_CORE_FUNCTION_POINTER_NAME(orxMouse_Init)();
}

/** Exit the mouse module
 */
orxSTATIC orxINLINE orxVOID orxMouse_Exit()
{
  orxPLUGIN_CORE_FUNCTION_POINTER_NAME(orxMouse_Exit)();
}

/** Measures how far the mouse has moved since the last call to this function
 * @param _ps32x (OUT)   X coordinates
 * @param _ps32y (OUT)   Y coordinates
 */
orxSTATIC orxINLINE orxSTATUS orxMouse_GetMove(orxS32 *_s32X, orxS32 *_s32Y)
{
  return orxPLUGIN_CORE_FUNCTION_POINTER_NAME(orxMouse_GetMove)(_s32X, _s32Y);
}


#endif /* _orxMOUSE_H_ */
