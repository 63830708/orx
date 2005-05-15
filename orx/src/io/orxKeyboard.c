/**
 * \file keyboard.c
 */

/***************************************************************************
 begin                : 22/11/2003
 author               : (C) Gdp
 email                : iarwain@ifrance.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "io/orxKeyboard.h"
#include "plugin/orxPluginCore.h"

/********************
 *  Plugin Related  *
 ********************/

orxSTATIC orxCONST orxPLUGIN_CORE_FUNCTION sastKeyboardPluginFunctionInfo[orxPLUGIN_FUNCTION_BASE_ID_KEYBOARD_NUMBER] =
{
  {(orxPLUGIN_FUNCTION *) &orxKeyboard_Init,        orxPLUGIN_FUNCTION_BASE_ID_KEYBOARD_INIT},
  {(orxPLUGIN_FUNCTION *) &orxKeyboard_Exit,        orxPLUGIN_FUNCTION_BASE_ID_KEYBOARD_EXIT},
  {(orxPLUGIN_FUNCTION *) &orxKeyboard_Hit,         orxPLUGIN_FUNCTION_BASE_ID_KEYBOARD_HIT},
  {(orxPLUGIN_FUNCTION *) &orxKeyboard_Read,        orxPLUGIN_FUNCTION_BASE_ID_KEYBOARD_READ},
  {(orxPLUGIN_FUNCTION *) &orxKeyboard_ClearBuffer, orxPLUGIN_FUNCTION_BASE_ID_KEYBOARD_CLEAR_BUFFER}
};

/** Init the keyboard core plugin
 */
orxVOID keyboard_plugin_init()
{
  /* Plugin init */
  orxPlugin_AddCoreInfo(orxPLUGIN_CORE_ID_KEYBOARD, sastKeyboardPluginFunctionInfo, sizeof(sastKeyboardPluginFunctionInfo) / sizeof(orxPLUGIN_CORE_FUNCTION));

  return;
}

/********************
 *   Core Related   *
 ********************/

orxPLUGIN_DEFINE_CORE_FUNCTION(orxKeyboard_Init, orxSTATUS);
orxPLUGIN_DEFINE_CORE_FUNCTION(orxKeyboard_Exit, orxVOID);

orxPLUGIN_DEFINE_CORE_FUNCTION(orxKeyboard_Hit, orxBOOL);
orxPLUGIN_DEFINE_CORE_FUNCTION(orxKeyboard_Read, orxS32);
orxPLUGIN_DEFINE_CORE_FUNCTION(orxKeyboard_ClearBuffer, orxVOID);
