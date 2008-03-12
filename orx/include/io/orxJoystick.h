/**
 * \file orxJoystick.h
 */

/***************************************************************************
 begin                : 22/11/2003
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

#ifndef _orxJOYSTICK_H_
#define _orxJOYSTICK_H_

#include "orxInclude.h"
#include "plugin/orxPluginCore.h"


/***************************************************************************
 * Functions directly implemented by orx core
 ***************************************************************************/

/** Joystick module setup */
extern orxDLLAPI orxVOID                              orxJoystick_Setup();


/***************************************************************************
 * Functions extended by plugins
 ***************************************************************************/

orxPLUGIN_DECLARE_CORE_FUNCTION(orxJoystick_Init, orxSTATUS);
orxPLUGIN_DECLARE_CORE_FUNCTION(orxJoystick_Exit, orxVOID);


/** Inits the joystick module.
 * @return The status of the operation.
 */
orxSTATIC orxINLINE orxSTATUS orxJoystick_Init()
{
  return orxPLUGIN_CORE_FUNCTION_POINTER_NAME(orxJoystick_Init)();
}

/** Exits from the joystick module.
 * @return The status of the operation.
 */
orxSTATIC orxINLINE orxVOID orxJoystick_Exit()
{
  orxPLUGIN_CORE_FUNCTION_POINTER_NAME(orxJoystick_Exit)();
}

#endif /* _orxJOYSTICK_H_ */
