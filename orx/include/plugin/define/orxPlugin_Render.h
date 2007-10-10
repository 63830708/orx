/**
 * \file orxPlugin_Render.h
 * This header is used to define ID for render plugin registration.
 */

/*
 begin                : 25/09/2007
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

#ifndef _orxPLUGIN_RENDER_H_
#define _orxPLUGIN_RENDER_H_

#include "orxPlugin_CoreID.h"


/*********************************************
 Constants
 *********************************************/

typedef enum __orxPLUGIN_FUNCTION_BASE_ID_RENDER_t
{
  orxPLUGIN_FUNCTION_BASE_ID_RENDER_INIT = 0,
  orxPLUGIN_FUNCTION_BASE_ID_RENDER_EXIT,

  orxPLUGIN_FUNCTION_BASE_ID_RENDER_NUMBER,

  orxPLUGIN_FUNCTION_BASE_ID_RENDER_NONE = orxENUM_NONE
    
} orxPLUGIN_FUNCTION_BASE_ID_RENDER;


#endif /* _orxPLUGIN_RENDER_H_ */
