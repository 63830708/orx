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
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License           *
 *   as published by the Free Software Foundation; either version 2.1      *
 *   of the License, or (at your option) any later version.                *
 *                                                                         *
 ***************************************************************************/

#ifndef _orxDEBUG_H_
#define _orxDEBUG_H_

#include "orxInclude.h"


/* *** orxDEBUG flags *** */

#define orxDEBUG_KU32_STATIC_FLAG_NONE                0x00000000

#define orxDEBUG_KU32_STATIC_FLAG_TIMESTAMP           0x00000001
#define orxDEBUG_KU32_STATIC_FLAG_TYPE                0x00000002
#define orxDEBUG_KU32_STATIC_FLAG_TAGGED              0x00000004

#define orxDEBUG_KU32_STATIC_FLAG_FILE                0x00000010
#define orxDEBUG_KU32_STATIC_FLAG_CONSOLE             0x00000020
#define orxDEBUG_KU32_STATIC_FLAG_GRAPHIC             0x00000040

#define orxDEBUG_KU32_STATIC_MASK_DEFAULT             0x00000037

#define orxDEBUG_KU32_STATIC_MASK_USER_ALL            0x0FFFFFFF


/* *** Misc *** */

#define orxDEBUG_KZ_DEFAULT_DEBUG_FILE                "OrxDebug.txt"
#define orxDEBUG_KZ_DEFAULT_LOG_FILE                  "OrxLog.txt"


/* *** Debug Macros *** */

/* Log message, compiler specific */
#ifdef __orxGCC__

  #define orxLOG(STRING, ...)                                                                               \
    _orxDebug_BackupFlags();                                                                                \
    _orxDebug_SetFlags(orxDEBUG_KU32_STATIC_FLAG_CONSOLE                                                    \
                      |orxDEBUG_KU32_STATIC_FLAG_FILE                                                       \
                      |orxDEBUG_KU32_STATIC_FLAG_TYPE                                                       \
                      |orxDEBUG_KU32_STATIC_FLAG_TIMESTAMP,                                                 \
                       orxDEBUG_KU32_STATIC_MASK_USER_ALL);                                                 \
    _orxDebug_Log(orxDEBUG_LEVEL_LOG, (orxSTRING)__FUNCTION__, __FILE__, __LINE__, STRING, ##__VA_ARGS__);  \
    _orxDebug_RestoreFlags();

#else /* __orxGCC__ */                                                                              
  #ifdef __orxMSVC__

    #define orxLOG(STRING, ...)                                                                               \
      orxDEBUG_FLAG_BACKUP();                                                                                 \
      orxDEBUG_FLAG_SET(orxDEBUG_KU32_STATIC_FLAG_CONSOLE                                                     \
                       |orxDEBUG_KU32_STATIC_FLAG_FILE                                                        \
                       |orxDEBUG_KU32_STATIC_FLAG_TYPE                                                        \
                       |orxDEBUG_KU32_STATIC_FLAG_TIMESTAMP,                                                  \
                        orxDEBUG_KU32_STATIC_MASK_USER_ALL);                                                  \
      _orxDebug_Log(orxDEBUG_LEVEL_LOG, (orxSTRING)__FUNCTION__, __FILE__, __LINE__, STRING, ##__VA_ARGS__);  \
      orxDEBUG_FLAG_RESTORE();

  #endif /* __orxMSVC__ */
#endif /* __orcGCC__ */

#define orxDEBUG_INIT()                     _orxDebug_Init()
#define orxDEBUG_EXIT()                     _orxDebug_Exit()

#ifdef __orxDEBUG__

  /* Debug print, compiler specific */
  #ifdef __orxGCC__
    #define orxDEBUG_PRINT(LEVEL, STRING, ...)  _orxDebug_Log(LEVEL, (orxSTRING)__FUNCTION__, __FILE__, __LINE__, STRING, ##__VA_ARGS__)
  #else /* __orxGCC__ */
    #ifdef __orxMSVC__
      #define orxDEBUG_PRINT(LEVEL, STRING, ...)  _orxDebug_Log(LEVEL, (orxSTRING)__FUNCTION__, __FILE__, __LINE__, STRING, __VA_ARGS__)
    #endif /* __orxMSVC__ */
  #endif /* __orcGCC__ */
  
  /* End platform specific */

  #define orxDEBUG_FLAG_SET(SET, UNSET)       _orxDebug_SetFlags(SET, UNSET)
  #define orxDEBUG_FLAG_BACKUP()              _orxDebug_BackupFlags()
  #define orxDEBUG_FLAG_RESTORE()             _orxDebug_RestoreFlags()

  /* Break */
  #define orxBREAK()                          _orxDebug_Break()

  /* Assert */
  #define orxASSERT(TEST)                     \
  if(!(TEST))                                 \
  {                                           \
    orxDEBUG_PRINT(orxDEBUG_LEVEL_ASSERT, "[Assertion failed] : <" #TEST ">"); \
    orxBREAK();                               \
  }

#else /* __orxDEBUG__ */

  /* Log message */
  #define orxDEBUG_PRINT(LEVEL, STRING, ...)

  #define orxBREAK()

  #define orxASSERT(TEST)

  #define orxDEBUG_FLAG_SET(SET, UNSET)
  #define orxDEBUG_FLAG_BACKUP()
  #define orxDEBUG_FLAG_RESTORE()

#endif /* __orxDEBUG__ */



/*****************************************************************************/

/* *** Debug defines. *** */

#define orxDEBUG_KS32_BUFFER_MAX_NUMBER       32
#define orxDEBUG_KS32_BUFFER_OUTPUT_SIZE      1024

#define orxDEBUG_KZ_DATE_FORMAT               "[%Y-%m-%d %H:%M:%S]"


/*****************************************************************************/

/* *** Debug types. *** */
typedef enum __orxDEBUG_LEVEL_t
{
  orxDEBUG_LEVEL_MOUSE = 0,                   /**< Mouse Debug */
  orxDEBUG_LEVEL_KEYBOARD,                    /**< Keyboard Debug */
  orxDEBUG_LEVEL_JOYSTICK,                    /**< Joystick Debug */
  orxDEBUG_LEVEL_INTERACTION,                 /**< Interaction Debug */
  orxDEBUG_LEVEL_DISPLAY,                     /**< Display Debug */
  orxDEBUG_LEVEL_SOUND,                       /**< Sound Debug */
  orxDEBUG_LEVEL_TIMER,                       /**< Timer Debug */
  orxDEBUG_LEVEL_MEMORY,                      /**< Memory Debug */
  orxDEBUG_LEVEL_SCREENSHOT,                  /**< Screenshot Debug */
  orxDEBUG_LEVEL_FILE,                        /**< File Debug */
  orxDEBUG_LEVEL_PATHFINDER,                  /**< Pathfinder Debug */
  orxDEBUG_LEVEL_PLUGIN,                      /**< Plug-in Debug */
  orxDEBUG_LEVEL_PARAM,                       /**< Param Debug */
  orxDEBUG_LEVEL_RENDER,                      /**< Render Debug */

  orxDEBUG_LEVEL_LOG,                         /**< Log Debug */

  orxDEBUG_LEVEL_ASSERT,                      /**< Assert Debug */
  orxDEBUG_LEVEL_CRITICAL_ASSERT,             /**< Critical Assert Debug */

  orxDEBUG_LEVEL_NUMBER,

  orxDEBUG_LEVEL_MAX_NUMBER = 32,

  orxDEBUG_LEVEL_ALL = 0xFFFFFFFE,            /**< All Debugs */

  orxDEBUG_LEVEL_NONE = orxENUM_NONE

} orxDEBUG_LEVEL;


/*****************************************************************************/

/* *** Functions *** */

/** Debug init function. */
extern orxDLLAPI orxSTATUS                    _orxDebug_Init();

/** Debug exit function. */
extern orxDLLAPI orxVOID                      _orxDebug_Exit();

/** Debug output function. */
extern orxDLLAPI orxVOID orxFASTCALL          _orxDebug_Log(orxDEBUG_LEVEL _eLevel, orxCONST orxSTRING _zFunction, orxCONST orxSTRING _zFile, orxU32 _u32Line, orxCONST orxSTRING _zFormat, ...);

/** Debug flag backup function. */
extern orxDLLAPI orxVOID                      _orxDebug_BackupFlags();

/** Debug flag restore function. */
extern orxDLLAPI orxVOID                      _orxDebug_RestoreFlags();

/** Debug flag get/set accessor. */
extern orxDLLAPI orxVOID orxFASTCALL          _orxDebug_SetFlags(orxU32 _u32Add, orxU32 _u32Remove);

/** Software break function. */
extern orxDLLAPI orxVOID                      _orxDebug_Break();


#endif /* __orxDEBUG__ */

