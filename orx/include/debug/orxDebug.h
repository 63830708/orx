/** 
 * \file orxDebug.h
 * 
 * Debug Module.
 * Debugging help features.
 * 
 * \todo
 * - Add mask test for level displaying
 * - Add graphical debug from outside, using a shared debug info array
 * - Add Assert/Assert after code
 * - Enhance logging, use of different log levels
 */


/***************************************************************************
 orxDebug.h
 Debug module
 
 begin                : 10/12/2003
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

#ifndef _orxDEBUG_H_
#define _orxDEBUG_H_

#include "orxInclude.h"


/* *** orxDEBUG FLAGS *** */

#define orxDEBUG_KU32_FLAG_NONE               0x00000000
#define orxDEBUG_KU32_FLAG_ALL                0x0FFFFFFF

#define orxDEBUG_KU32_FLAG_TIMESTAMP          0x00000001
#define orxDEBUG_KU32_FLAG_TYPE               0x00000002
#define orxDEBUG_KU32_FLAG_TAGGED             0x00000004

#define orxDEBUG_KU32_FLAG_FILE               0x00000010
#define orxDEBUG_KU32_FLAG_CONSOLE            0x00000020
#define orxDEBUG_KU32_FLAG_GRAPHIC            0x00000040

#define orxDEBUG_KU32_FLAG_DEFAULT            0x00000037


/* *** Misc *** */

#define orxDEBUG_KZ_DEFAULT_DEBUG_FILE        "OrxDebug.log"
#define orxDEBUG_KZ_DEFAULT_LOG_FILE          "OrxLog.log"


/* *** Debug Macros *** */

#ifdef DEBUG

  #define orxDEBUG_LOG(LEVEL, STRING, ...)    _orxDebug_Log(LEVEL, __FUNCTION__, __FILE__, __LINE__, STRING, ##__VA_ARGS__)

  #define orxDEBUG_INIT()                     _orxDebug_Init()
  #define orxDEBUG_EXIT()                     _orxDebug_Exit()
  #define orxDEBUG_FLAG_SET(SET, UNSET)       _orxDebug_SetFlag(SET, UNSET)
  #define orxDEBUG_FLAG_BACKUP()              _orxDebug_BackupFlags()
  #define orxDEBUG_FLAG_RESTORE()             _orxDebug_RestoreFlags()

  /* Break */
  #define orxBREAK()                          _orxDebug_Break()

  /* Assert */
  #define orxASSERT(TEST)                     \
  if(!(TEST))                                 \
  {                                           \
    orxDEBUG_LOG(orxDEBUG_LEVEL_ASSERT, "[Assertion failed] : !!!" #TEST "!!!"); \
    orxBREAK();                               \
  }

#else /* DEBUG */

  #define orxDEBUG_LOG(LEVEL, STRING, ...)

  #define orxBREAK()

  #define orxASSERT(TEST)

  #define orxDEBUG_INIT()
  #define orxDEBUG_EXIT()
  #define orxDEBUG_FLAG_SET(SET, UNSET)
  #define orxDEBUG_FLAG_BACKUP()
  #define orxDEBUG_FLAG_RESTORE()

#endif /* DEBUG */


/*****************************************************************************/

/* *** Debug defines. *** */

#define orxDEBUG_KS32_BUFFER_MAX_NUMBER       32
#define orxDEBUG_KS32_BUFFER_OUTPUT_SIZE      512

#define orxDEBUG_KZ_DATE_FORMAT               "[%Y-%m-%d %H:%M:%S']"


/*****************************************************************************/

/* *** Debug types. *** */
typedef enum __orxDEBUG_LEVEL_t
{
  orxDEBUG_LEVEL_MOUSE = 0,                   /**< Mouse Debug */
  orxDEBUG_LEVEL_KEYBOARD,                    /**< Keyboard Debug */
  orxDEBUG_LEVEL_JOYSTICK,                    /**< Joystick Debug */
  orxDEBUG_LEVEL_INTERACTION,                 /**< Interaction Debug */
  orxDEBUG_LEVEL_GRAPH,                       /**< Graph Debug */
  orxDEBUG_LEVEL_SOUND,                       /**< Sound Debug */
  orxDEBUG_LEVEL_TIMER,                       /**< Timer Debug */
  orxDEBUG_LEVEL_MEMORY,                      /**< Memory Debug */
  orxDEBUG_LEVEL_SCREENSHOT,                  /**< Screenshot Debug */
  orxDEBUG_LEVEL_FILE,                        /**< File Debug */
  orxDEBUG_LEVEL_PATHFINDER,                  /**< Pathfinder Debug */
  orxDEBUG_LEVEL_PLUGIN,                      /**< Plug-in Debug */
  orxDEBUG_LEVEL_LOG,                         /**< Log Debug */
  orxDEBUG_LEVEL_ALL,                         /**< All Debugs */
  orxDEBUG_LEVEL_ASSERT,                      /**< Assert Debug */
  orxDEBUG_LEVEL_CRITICAL_ASSERT,             /**< Critical Assert Debug */
  
  orxDEBUG_LEVEL_NUMBER,
  
  orxDEBUG_LEVEL_NONE = 0xFFFFFFFF
  
} orxDEBUG_LEVEL;


/*****************************************************************************/

/* *** Functions *** */

/** Debug init function. */
extern orxSTATUS                              _orxDebug_Init();

/** Debug exit function. */
extern orxVOID                                _orxDebug_Exit();

/** Debug output function. */
extern orxVOID                                _orxDebug_Log(orxDEBUG_LEVEL _eLevel, orxCONST orxSTRING _zFunction, orxCONST orxSTRING _zFile, orxU32 _u32Line, orxCONST orxSTRING _zFormat, ...);

/** Debug flag backup function. */
extern orxVOID                                _orxDebug_BackupFlags();

/** Debug flag restore function. */
extern orxVOID                                _orxDebug_RestoreFlags();

/** Debug flag get/set accessor. */
extern orxVOID                                _orxDebug_SetFlag(orxU32 _u32Add, orxU32 _u32Remove);

/** Software break function. */
extern orxVOID                                _orxDebug_Break();


#endif /* _orxDEBUG_H_ */
