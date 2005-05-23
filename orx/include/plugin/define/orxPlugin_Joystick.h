/**
 * \file orxPlugin_Joystick.h
 * This header is used to define ID for joystick plugin registration.
 */

/*
 begin                : 22/11/2003
 author               : (C) Arcallians
 email                : iarwain@arcallians.org
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef _orxPLUGIN_JOYSTICK_H_
#define _orxPLUGIN_JOYSTICK_H_

#include "orxPlugin_CoreID.h"


/*********************************************
 Constants
 *********************************************/

typedef enum __orxPLUGIN_FUNCTION_BASE_ID_JOYSTICK_t
{
  orxPLUGIN_FUNCTION_BASE_ID_JOYSTICK_INIT = 0,
  orxPLUGIN_FUNCTION_BASE_ID_JOYSTICK_EXIT,

  orxPLUGIN_FUNCTION_BASE_ID_JOYSTICK_NUMBER,

  orxPLUGIN_FUNCTION_BASE_ID_JOYSTICK_NONE = orxENUM_NONE
    
} orxPLUGIN_FUNCTION_BASE_ID_JOYSTICK;


#endif /* _orxPLUGIN_JOYSTICK_H_ */
