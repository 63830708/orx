/* Orx - Portable Game Engine
 *
 * Orx is the legal property of its developers, whose names
 * are listed in the COPYRIGHT file distributed 
 * with this source distribution.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @file orxMouse.c
 * @date 22/11/2003
 * @author iarwain@orx-project.org
 *
 * @todo
 */


#include "io/orxMouse.h"
#include "plugin/orxPluginCore.h"


/** Mouse module setup
 */
orxVOID orxMouse_Setup()
{
  /* Adds module dependencies */
  orxModule_AddDependency(orxMODULE_ID_MOUSE, orxMODULE_ID_PLUGIN);
  orxModule_AddDependency(orxMODULE_ID_MOUSE, orxMODULE_ID_MEMORY);
  orxModule_AddDependency(orxMODULE_ID_MOUSE, orxMODULE_ID_DISPLAY);

  return;
}


/***************************************************************************
 * Plugin related                                                          *
 ***************************************************************************/

/* *** Core function definitions *** */

orxPLUGIN_DEFINE_CORE_FUNCTION(orxMouse_Init, orxSTATUS);
orxPLUGIN_DEFINE_CORE_FUNCTION(orxMouse_Exit, orxVOID);
orxPLUGIN_DEFINE_CORE_FUNCTION(orxMouse_GetPosition, orxSTATUS, orxS32 *, orxS32 *);
orxPLUGIN_DEFINE_CORE_FUNCTION(orxMouse_IsButtonPressed, orxBOOL, orxMOUSE_BUTTON);


/* *** Core function info array *** */

orxPLUGIN_BEGIN_CORE_FUNCTION_ARRAY(MOUSE)

orxPLUGIN_ADD_CORE_FUNCTION_ARRAY(MOUSE, INIT, orxMouse_Init)
orxPLUGIN_ADD_CORE_FUNCTION_ARRAY(MOUSE, EXIT, orxMouse_Exit)
orxPLUGIN_ADD_CORE_FUNCTION_ARRAY(MOUSE, GET_POSITION, orxMouse_GetPosition)
orxPLUGIN_ADD_CORE_FUNCTION_ARRAY(MOUSE, IS_BUTTON_PRESSED, orxMouse_IsButtonPressed)

orxPLUGIN_END_CORE_FUNCTION_ARRAY(MOUSE)


/* *** Core function implementations *** */

/** Inits the mouse module
 * @return Returns the status of the operation
 */
orxSTATUS orxMouse_Init()
{
  return orxPLUGIN_CORE_FUNCTION_POINTER_NAME(orxMouse_Init)();
}

/** Exits from the mouse module
 */
orxVOID orxMouse_Exit()
{
  orxPLUGIN_CORE_FUNCTION_POINTER_NAME(orxMouse_Exit)();
}

/** Gets mouse on screen position
 * @param[out] _ps32x   X coordinates
 * @param[out] _ps32y   Y coordinates
 * @return orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxMouse_GetPosition(orxS32 *_s32X, orxS32 *_s32Y)
{
  return orxPLUGIN_CORE_FUNCTION_POINTER_NAME(orxMouse_GetPosition)(_s32X, _s32Y);
}

/** Is mouse button pressed?
 * @param _eButton      Mouse button to check
 * @return orxTRUE if presse / orxFALSE otherwise
 */
orxBOOL orxMouse_IsButtonPressed(orxMOUSE_BUTTON _eButton)
{
  return orxPLUGIN_CORE_FUNCTION_POINTER_NAME(orxMouse_IsButtonPressed)(_eButton);
}
