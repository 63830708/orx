/**
 * \file orxPlugin_Keyboard.h
 * This header is used to define ID for keyboard plugin registration.
 */

/*
 begin                : 21/11/2003
 author               : (C) Arcallians
 email                : iarwain@arcallians.org
 */

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License           *
 *   as published by the Free Software Foundation; either version 2.1      *
 *   of the License, or (at your option) any later version.                *
 *                                                                         *
 ***************************************************************************/


#ifndef _orxPLUGIN_KEYBOARD_H_
#define _orxPLUGIN_KEYBOARD_H_

#include "plugin/define/orxPlugin_CoreID.h"


/*********************************************
 Constants
 *********************************************/

typedef enum __orxPLUGIN_FUNCTION_BASE_ID_KEYBOARD_t
{
  orxPLUGIN_FUNCTION_BASE_ID_KEYBOARD_INIT = 0,
  orxPLUGIN_FUNCTION_BASE_ID_KEYBOARD_EXIT,
  orxPLUGIN_FUNCTION_BASE_ID_KEYBOARD_IS_KEY_PRESSED,
  orxPLUGIN_FUNCTION_BASE_ID_KEYBOARD_HIT,
  orxPLUGIN_FUNCTION_BASE_ID_KEYBOARD_READ,
  orxPLUGIN_FUNCTION_BASE_ID_KEYBOARD_CLEAR_BUFFER,

  orxPLUGIN_FUNCTION_BASE_ID_KEYBOARD_NUMBER,

  orxPLUGIN_FUNCTION_BASE_ID_KEYBOARD_NONE = orxENUM_NONE

} orxPLUGIN_FUNCTION_BASE_ID_KEYBOARD;


#endif /* _orxPLUGIN_KEYBOARD_H_ */
