/* Orx - Portable Game Engine
 *
 * Copyright (c) 2008-2011 Orx-Project
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *    1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 *    2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 *    3. This notice may not be removed or altered from any source
 *    distribution.
 */

/**
 * @file orx.h
 * @date 02/09/2005
 * @author
 *
 * @todo
 */

/**
 * @addtogroup Orx
 *
 * Main orx include
 *
 * @{
 */

#ifdef __cplusplus
  extern "C" {
#endif /* __cplusplus */

#ifndef _orx_H_
#define _orx_H_


#define __orxEXTERN__ /* Not compiling orx library */


#include "orxInclude.h"

#include "orxKernel.h"

#include "orxUtils.h"


/***************************************************************************
 * Static variables                                                        *
 ***************************************************************************/

/** Should stop execution by default event handling?
 */
static orxBOOL sbStopByEvent = orxFALSE;


/***************************************************************************
 * Public functions                                                        *
 ***************************************************************************/

/** Orx default basic event handler
 * @param[in]   _pstEvent                     Sent event
 * @return      orxSTATUS_SUCCESS if handled / orxSTATUS_FAILURE otherwise
 */
static orxSTATUS orxFASTCALL orx_DefaultEventHandler(const orxEVENT *_pstEvent)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(_pstEvent->eType == orxEVENT_TYPE_SYSTEM);

  /* Depending on event ID */
  switch(_pstEvent->eID)
  {
    /* Close event */
    case orxSYSTEM_EVENT_CLOSE:
    {
      /* Updates status */
      sbStopByEvent = orxTRUE;

      break;
    }

    default:
    {
      break;
    }
  }

  /* Done! */
  return eResult;
}

/** Default main setup (module dependencies)
*/
static void orxFASTCALL orx_MainSetup()
{
  /* Adds module dependencies */
  orxModule_AddDependency(orxMODULE_ID_MAIN, orxMODULE_ID_PARAM);
  orxModule_AddDependency(orxMODULE_ID_MAIN, orxMODULE_ID_CLOCK);
  orxModule_AddDependency(orxMODULE_ID_MAIN, orxMODULE_ID_CONFIG);
  orxModule_AddDependency(orxMODULE_ID_MAIN, orxMODULE_ID_INPUT);
  orxModule_AddDependency(orxMODULE_ID_MAIN, orxMODULE_ID_EVENT);
  orxModule_AddDependency(orxMODULE_ID_MAIN, orxMODULE_ID_FILE);
  orxModule_AddDependency(orxMODULE_ID_MAIN, orxMODULE_ID_LOCALE);
  orxModule_AddDependency(orxMODULE_ID_MAIN, orxMODULE_ID_PLUGIN);
  orxModule_AddDependency(orxMODULE_ID_MAIN, orxMODULE_ID_OBJECT);

  orxModule_AddOptionalDependency(orxMODULE_ID_MAIN, orxMODULE_ID_SCREENSHOT);

  return;
}

#ifdef __orxANDROID__

  #include <jni.h>
  #include <android/log.h>

  #define orxEVENT_TYPE_ANDROID          orxEVENT_TYPE_FIRST_RESERVED

/** JNI environment
 *  This will be loaded in android-support.cpp.
 */
extern JNIEnv *mEnv;
extern JavaVM *mVM;
extern JNIEnv *globalEnv;

/** Event enum
 */
typedef enum __orxANDROID_EVENT_t
{
  orxANDROID_EVENT_TOUCH_BEGIN = 0,
  orxANDROID_EVENT_TOUCH_MOVE,
  orxANDROID_EVENT_TOUCH_END,
  orxANDROID_EVENT_ACCELERATE,
  orxANDROID_EVENT_MOTION_SHAKE,

  orxANDROID_EVENT_NUMBER,

} orxANDROID_EVENT;

/** Locale event payload
 */
typedef struct __orxANDROID_EVENT_PAYLOAD_t
{
  union
  {
    /* UI event */
    struct
    {
      unsigned int pointId;
      float x, y, p; //p: the presure of the touch
    };

    /* Accelerate event */
    struct
    {
      unsigned int accelEventPtr;
      float accelX, accelY, accelZ; //the value of accel
    };
  };

} orxANDROID_EVENT_PAYLOAD;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * the method only use here and define in jni android-support.cpp
 * and then they will call corresponding method in java
 */
extern orxSTATUS ANDROID_startLoop();
extern orxSTATUS ANDROID_endLoop();

/****************in android there is no actual main, so define it as orxMain will be called by jni******/
#define main orxMain
/****************end******************/

/** Orx main execution function
 * @param[in]   _u32NbParams                  Main function parameters number (argc)
 * @param[in]   _azParams                     Main function parameter list (argv)
 * @param[in]   _pfnInit                      Main init function (should init all the main stuff and register the main event handler to override the default one)
 * @param[in]   _pfnRun                       Main run function (will be called once per frame, should return orxSTATUS_SUCCESS to continue processing)
 * @param[in]   _pfnExit                      Main exit function (should clean all the main stuff)
 */
static orxINLINE void orx_Execute(orxU32 _u32NbParams, orxSTRING _azParams[], const orxMODULE_INIT_FUNCTION _pfnInit, const orxMODULE_RUN_FUNCTION _pfnRun, const orxMODULE_EXIT_FUNCTION _pfnExit)
{
  /* Inits the Debug System */
  orxDEBUG_INIT();

  /* Checks */
  orxASSERT(_u32NbParams > 0); orxASSERT(_azParams != orxNULL); orxASSERT(_pfnRun != orxNULL);

  /* Registers main module */
  orxModule_Register(orxMODULE_ID_MAIN, orx_MainSetup, _pfnInit, _pfnExit);

  /* Registers all other modules */
  orxModule_RegisterAll();

  /* Calls all modules setup */
  orxModule_SetupAll();

  /* Sends the command line arguments to orxParam module */
  if(orxParam_SetArgs(_u32NbParams, _azParams) != orxSTATUS_FAILURE)
  {
    /* Inits the engine */
    if(orxModule_Init(orxMODULE_ID_MAIN) != orxSTATUS_FAILURE)
    {
      /* Registers default event handler */
      orxEvent_AddHandler(orxEVENT_TYPE_SYSTEM, orx_DefaultEventHandler);

      /* Displays help */
      if(orxParam_DisplayHelp() != orxSTATUS_FAILURE)
      {
        orxSTATUS eClockStatus, eMainStatus;
        orxBOOL bStop;

        /* Main loop */
        for(bStop = orxFALSE, sbStopByEvent = orxFALSE; bStop == orxFALSE; bStop = ((sbStopByEvent != orxFALSE) || (eMainStatus == orxSTATUS_FAILURE) || (eClockStatus == orxSTATUS_FAILURE)) ? orxTRUE : orxFALSE)
        {
          //! TODO: Use events instead?
          /* Notifies android of loop start */
          ANDROID_startLoop();

          /* Runs the engine */
          eMainStatus = _pfnRun();

          /* Updates clock system */
          eClockStatus = orxClock_Update();

          /* Notifies android of loop end */
          ANDROID_endLoop();
        }
      }

      /* Removes event handler */
      orxEvent_RemoveHandler(orxEVENT_TYPE_SYSTEM, orx_DefaultEventHandler);

      /* Exits from engine */
      orxModule_Exit(orxMODULE_ID_MAIN);
    }

    /* Exits from all modules */
    orxModule_ExitAll();
  }

  /* Exits from the Debug system */
  orxDEBUG_EXIT();
}

#ifdef __cplusplus
}
#endif

#endif /* __orxANDROID__ */

#ifdef __orxIPHONE__

  #ifdef __orxOBJC__

  #import <UIKit/UIKit.h>

  #define orxEVENT_TYPE_IPHONE          orxEVENT_TYPE_FIRST_RESERVED

/** Event enum
  */
typedef enum __orxIPHONE_EVENT_t
{
  orxIPHONE_EVENT_TOUCH_BEGIN = 0,
  orxIPHONE_EVENT_TOUCH_MOVE,
  orxIPHONE_EVENT_TOUCH_END,
  orxIPHONE_EVENT_TOUCH_CANCEL,
  orxIPHONE_EVENT_ACCELERATE,
  orxIPHONE_EVENT_MOTION_SHAKE,

  orxIPHONE_EVENT_NUMBER,

} orxIPHONE_EVENT;

/** Locale event payload
 */
typedef struct __orxIPHONE_EVENT_PAYLOAD_t
{
  union
  {
    /* UI event */
    struct
    {
      UIEvent *poUIEvent;

      union
      {
        /* Touch event */
        NSSet          *poTouchList;

        /* Motion event */
        UIEventSubtype  eMotion;
      };
    };

    /* Accelerate event */
    struct
    {
      UIAccelerometer *poAccelerometer;
      UIAcceleration  *poAcceleration;
    };
  };

} orxIPHONE_EVENT_PAYLOAD;

/** Orx application interface
 */
@interface orxAppDelegate : NSObject <UIAccelerometerDelegate>
{
  UIWindow *poWindow;
  orxView  *poView;
}

@property (nonatomic, retain) UIWindow *poWindow;
@property (nonatomic, retain) UIView   *poView;

- (void)  MainLoop;

@end

extern orxSTATUS (orxFASTCALL *spfnRun)();

/** Orx main execution function
 * @param[in]   _u32NbParams                  Main function parameters number (argc)
 * @param[in]   _azParams                     Main function parameter list (argv)
 * @param[in]   _pfnInit                      Main init function (should init all the main stuff and register the main event handler to override the default one)
 * @param[in]   _pfnRun                       Main run function (will be called once per frame, should return orxSTATUS_SUCCESS to continue processing)
 * @param[in]   _pfnExit                      Main exit function (should clean all the main stuff)
 */
static orxINLINE void orx_Execute(orxU32 _u32NbParams, orxSTRING _azParams[], const orxMODULE_INIT_FUNCTION _pfnInit, const orxMODULE_RUN_FUNCTION _pfnRun, const orxMODULE_EXIT_FUNCTION _pfnExit)
{
  /* Inits the Debug System */
  orxDEBUG_INIT();

  /* Checks */
  orxASSERT(_u32NbParams > 0);
  orxASSERT(_azParams != orxNULL);
  orxASSERT(_pfnRun != orxNULL);

  /* Registers main module */
  orxModule_Register(orxMODULE_ID_MAIN, orx_MainSetup, _pfnInit, _pfnExit);

  /* Stores run callback */
  spfnRun = _pfnRun;

  /* Registers all other modules */
  orxModule_RegisterAll();

  /* Calls all modules setup */
  orxModule_SetupAll();

  /* Sends the command line arguments to orxParam module */
  if(orxParam_SetArgs(_u32NbParams, _azParams) != orxSTATUS_FAILURE)
  {
    NSAutoreleasePool *poPool;

    /* Allocates memory pool */
    poPool = [[NSAutoreleasePool alloc] init];

    /* Launches application */
    UIApplicationMain(_u32NbParams, _azParams, nil, @"orxAppDelegate");

    /* Releases memory pool */
    [poPool release];
  }

  /* Done! */
  return;
}

  #endif /* __orxOBJC__ */

#else /* __orxIPHONE__ */

/** Orx main execution function
 * @param[in]   _u32NbParams                  Main function parameters number (argc)
 * @param[in]   _azParams                     Main function parameter list (argv)
 * @param[in]   _pfnInit                      Main init function (should init all the main stuff and register the main event handler to override the default one)
 * @param[in]   _pfnRun                       Main run function (will be called once per frame, should return orxSTATUS_SUCCESS to continue processing)
 * @param[in]   _pfnExit                      Main exit function (should clean all the main stuff)
 */
static orxINLINE void orx_Execute(orxU32 _u32NbParams, orxSTRING _azParams[], const orxMODULE_INIT_FUNCTION _pfnInit, const orxMODULE_RUN_FUNCTION _pfnRun, const orxMODULE_EXIT_FUNCTION _pfnExit)
{
  /* Inits the Debug System */
  orxDEBUG_INIT();

  /* Checks */
  orxASSERT(_u32NbParams > 0);
  orxASSERT(_azParams != orxNULL);
  orxASSERT(_pfnRun != orxNULL);

  /* Registers main module */
  orxModule_Register(orxMODULE_ID_MAIN, orx_MainSetup, _pfnInit, _pfnExit);

  /* Registers all other modules */
  orxModule_RegisterAll();

  /* Calls all modules setup */
  orxModule_SetupAll();

  /* Sends the command line arguments to orxParam module */
  if(orxParam_SetArgs(_u32NbParams, _azParams) != orxSTATUS_FAILURE)
  {
    /* Inits the engine */
    if(orxModule_Init(orxMODULE_ID_MAIN) != orxSTATUS_FAILURE)
    {
      /* Registers default event handler */
      orxEvent_AddHandler(orxEVENT_TYPE_SYSTEM, orx_DefaultEventHandler);

      /* Displays help */
      if(orxParam_DisplayHelp() != orxSTATUS_FAILURE)
      {
        orxSTATUS eClockStatus, eMainStatus;
        orxBOOL   bStop;

        /* Main loop */
        for(bStop = orxFALSE, sbStopByEvent = orxFALSE;
            bStop == orxFALSE;
            bStop = ((sbStopByEvent != orxFALSE) || (eMainStatus == orxSTATUS_FAILURE) || (eClockStatus == orxSTATUS_FAILURE)) ? orxTRUE : orxFALSE)
        {
          /* Runs the engine */
          eMainStatus = _pfnRun();

          /* Updates clock system */
          eClockStatus = orxClock_Update();
        }
      }

      /* Removes event handler */
      orxEvent_RemoveHandler(orxEVENT_TYPE_SYSTEM, orx_DefaultEventHandler);

      /* Exits from engine */
      orxModule_Exit(orxMODULE_ID_MAIN);
    }

    /* Exits from all modules */
    orxModule_ExitAll();
  }

  /* Exits from the Debug system */
  orxDEBUG_EXIT();
}

  #ifdef __orxMSVC__

  #include "windows.h"

/** Orx main execution function (console-less windows application)
 * @param[in]   _pfnInit                      Main init function (should init all the main stuff and register the main event handler to override the default one)
 * @param[in]   _pfnRun                       Main run function (will be called once per frame, should return orxSTATUS_SUCCESS to continue processing)
 * @param[in]   _pfnExit                      Main exit function (should clean all the main stuff)
 */
static orxINLINE void orx_WinExecute(const orxMODULE_INIT_FUNCTION _pfnInit, const orxMODULE_RUN_FUNCTION _pfnRun, const orxMODULE_EXIT_FUNCTION _pfnExit)
{
  #define orxMAX_ARGS 256

  int   argc;
  char *argv[orxMAX_ARGS];
  char *pcToken, *pcNextToken, *pcFirstDelimiters;
  LPSTR lpFullCmdLine;

  /* Gets full command line */
  lpFullCmdLine = GetCommandLineA();

  /* Starts with a double quote? */
  if(*orxString_SkipWhiteSpaces(lpFullCmdLine) == '"')
  {
    /* Gets first delimiters */
    pcFirstDelimiters = "\"";
  }
  else
  {
    /* Gets first delimiters */
    pcFirstDelimiters = " ";
  }

  /* Process command line */
  for(argc = 0, pcNextToken = NULL, pcToken = strtok_s(lpFullCmdLine, pcFirstDelimiters, &pcNextToken);
      pcToken && (argc < orxMAX_ARGS);
      pcToken = strtok_s(NULL, " ", &pcNextToken))
  {
    argv[argc++] = pcToken;
  }

  /* Inits and executes orx */
  orx_Execute(argc, argv, _pfnInit, _pfnRun, _pfnExit);
}

  #endif /* __orxMSVC__ */

#endif /* __orxIPHONE__ */

#endif /*_orx_H_*/

#ifdef __cplusplus
  }
#endif /* __cplusplus */

/** @} */
