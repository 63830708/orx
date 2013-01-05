/* Orx - Portable Game Engine
 *
 * Copyright (c) 2008-2013 Orx-Project
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
 * @file orxModule.h
 * @date 12/09/2005
 * @author iarwain@orx-project.org
 *
 */


#include "orxInclude.h"
#include "orxKernel.h"
#include "orxUtils.h"

#ifdef __orxMSVC__
  #pragma warning(disable : 4276)
#endif /* __orxMSVC__ */

/** Module registration macro
 */
#define orxMODULE_REGISTER(MODULE_ID, MODULE_BASENAME)  orxModule_Register(MODULE_ID, MODULE_BASENAME##_Setup, MODULE_BASENAME##_Init, MODULE_BASENAME##_Exit)


/** Registers all engine modules
 */
void orxFASTCALL orxModule_RegisterAll()
{
  /* *** All modules registration *** */
  orxMODULE_REGISTER(orxMODULE_ID_ANIM, orxAnim);
  orxMODULE_REGISTER(orxMODULE_ID_ANIMPOINTER, orxAnimPointer);
  orxMODULE_REGISTER(orxMODULE_ID_ANIMSET, orxAnimSet);
  orxMODULE_REGISTER(orxMODULE_ID_BANK, orxBank);
  orxMODULE_REGISTER(orxMODULE_ID_BODY, orxBody);
  orxMODULE_REGISTER(orxMODULE_ID_CAMERA, orxCamera);
  orxMODULE_REGISTER(orxMODULE_ID_CLOCK, orxClock);
  orxMODULE_REGISTER(orxMODULE_ID_COMMAND, orxCommand);
  orxMODULE_REGISTER(orxMODULE_ID_CONFIG, orxConfig);
  orxMODULE_REGISTER(orxMODULE_ID_CONSOLE, orxConsole);
  orxMODULE_REGISTER(orxMODULE_ID_DISPLAY, orxDisplay);
  orxMODULE_REGISTER(orxMODULE_ID_EVENT, orxEvent);
  orxMODULE_REGISTER(orxMODULE_ID_FILE, orxFile);
  orxMODULE_REGISTER(orxMODULE_ID_FONT, orxFont);
  orxMODULE_REGISTER(orxMODULE_ID_FPS, orxFPS);
  orxMODULE_REGISTER(orxMODULE_ID_FRAME, orxFrame);
  orxMODULE_REGISTER(orxMODULE_ID_FX, orxFX);
  orxMODULE_REGISTER(orxMODULE_ID_FXPOINTER, orxFXPointer);
  orxMODULE_REGISTER(orxMODULE_ID_GRAPHIC, orxGraphic);
  orxMODULE_REGISTER(orxMODULE_ID_INPUT, orxInput);
  orxMODULE_REGISTER(orxMODULE_ID_JOYSTICK, orxJoystick);
  orxMODULE_REGISTER(orxMODULE_ID_KEYBOARD, orxKeyboard);
  orxMODULE_REGISTER(orxMODULE_ID_LOCALE, orxLocale);
  orxMODULE_REGISTER(orxMODULE_ID_MEMORY, orxMemory);
  orxMODULE_REGISTER(orxMODULE_ID_MOUSE, orxMouse);
  orxMODULE_REGISTER(orxMODULE_ID_OBJECT, orxObject);
  orxMODULE_REGISTER(orxMODULE_ID_PARAM, orxParam);
  orxMODULE_REGISTER(orxMODULE_ID_PHYSICS, orxPhysics);
  orxMODULE_REGISTER(orxMODULE_ID_PLUGIN, orxPlugin);
  orxMODULE_REGISTER(orxMODULE_ID_PROFILER, orxProfiler);
  orxMODULE_REGISTER(orxMODULE_ID_RENDER, orxRender);
  orxMODULE_REGISTER(orxMODULE_ID_SCREENSHOT, orxScreenshot);
  orxMODULE_REGISTER(orxMODULE_ID_SHADER, orxShader);
  orxMODULE_REGISTER(orxMODULE_ID_SHADERPOINTER, orxShaderPointer);
  orxMODULE_REGISTER(orxMODULE_ID_SOUND, orxSound);
  orxMODULE_REGISTER(orxMODULE_ID_SOUNDPOINTER, orxSoundPointer);
  orxMODULE_REGISTER(orxMODULE_ID_SOUNDSYSTEM, orxSoundSystem);
  orxMODULE_REGISTER(orxMODULE_ID_SPAWNER, orxSpawner);
  orxMODULE_REGISTER(orxMODULE_ID_STRUCTURE, orxStructure);
  orxMODULE_REGISTER(orxMODULE_ID_SYSTEM, orxSystem);
  orxMODULE_REGISTER(orxMODULE_ID_TEXT, orxText);
  orxMODULE_REGISTER(orxMODULE_ID_TEXTURE, orxTexture);
  orxMODULE_REGISTER(orxMODULE_ID_TIMELINE, orxTimeLine);
  orxMODULE_REGISTER(orxMODULE_ID_VIEWPORT, orxViewport);

  /* Done! */
  return;
}

/** Module flags
 */
#define orxMODULE_KU32_STATIC_FLAG_NONE         0x00000000


/** Module status flags
 */
#define orxMODULE_KU32_STATUS_FLAG_NONE         0x00000000

#define orxMODULE_KU32_STATUS_FLAG_REGISTERED   0x00000001
#define orxMODULE_KU32_STATUS_FLAG_INITIALIZED  0x00000002
#define orxMODULE_KU32_STATUS_FLAG_PENDING      0x00010000

#define orxMODULE_KU32_STATUS_MASK_ALL          0xFFFFFFFF


/***************************************************************************
 * Structure declaration                                                   *
 ***************************************************************************/

/** Internal module info structure
 */
typedef struct __orxMODULE_INFO_t
{
  orxU64                    u64DependFlags;                 /**< Dependency flags : 8 */
  orxU64                    u64OptionalDependFlags;         /**< Optional dependency flags : 16 */
  orxMODULE_SETUP_FUNCTION  pfnSetup;                       /**< Setup function : 20 */
  orxMODULE_INIT_FUNCTION   pfnInit;                        /**< Init function : 24 */
  orxMODULE_EXIT_FUNCTION   pfnExit;                        /**< Exit function : 28 */
  orxU32                    u32StatusFlags;                 /**< Status flags : 32 */

} orxMODULE_INFO;

/** Static structure
 */
typedef struct __orxMODULE_STATIC_t
{
  orxMODULE_INFO  astModuleInfo[orxMODULE_ID_NUMBER];
  orxU32          u32InitLoopCounter;

} orxMODULE_STATIC;


/***************************************************************************
 * Static variables                                                        *
 ***************************************************************************/

/** static data
 */
static orxMODULE_STATIC sstModule;


/***************************************************************************
 * Private functions                                                       *
 ***************************************************************************/


/***************************************************************************
 * Public functions                                                        *
 ***************************************************************************/

/** Registers a module
 */
void orxFASTCALL orxModule_Register(orxMODULE_ID _eModuleID, const orxMODULE_SETUP_FUNCTION _pfnSetup, const orxMODULE_INIT_FUNCTION _pfnInit, const orxMODULE_EXIT_FUNCTION _pfnExit)
{
  /* Checks */
  orxASSERT(_eModuleID < orxMODULE_ID_NUMBER);

  /* Clears module info */
  orxMemory_Zero(&(sstModule.astModuleInfo[_eModuleID]), sizeof(orxMODULE_INFO));

  /* Stores module functions */
  sstModule.astModuleInfo[_eModuleID].pfnSetup  = _pfnSetup;
  sstModule.astModuleInfo[_eModuleID].pfnInit   = _pfnInit;
  sstModule.astModuleInfo[_eModuleID].pfnExit   = _pfnExit;

  /* Updates module status flags */
  sstModule.astModuleInfo[_eModuleID].u32StatusFlags = orxMODULE_KU32_STATUS_FLAG_REGISTERED;

  /* Done! */
  return;
}

/** Adds dependencies between 2 modules
 */
void orxFASTCALL orxModule_AddDependency(orxMODULE_ID _eModuleID, orxMODULE_ID _eDependID)
{
  /* Checks */
  orxASSERT(_eModuleID < orxMODULE_ID_NUMBER);
  orxASSERT(_eDependID < orxMODULE_ID_NUMBER);

  /* Stores dependency */
  sstModule.astModuleInfo[_eModuleID].u64DependFlags |= ((orxU64)1) << _eDependID;

  /* Done! */
  return;
}

/** Adds optional dependencies between 2 modules
 */
void orxFASTCALL orxModule_AddOptionalDependency(orxMODULE_ID _eModuleID, orxMODULE_ID _eDependID)
{
  /* Checks */
  orxASSERT(_eModuleID < orxMODULE_ID_NUMBER);
  orxASSERT(_eDependID < orxMODULE_ID_NUMBER);

  /* Stores dependency */
  sstModule.astModuleInfo[_eModuleID].u64OptionalDependFlags |= ((orxU64)1) << _eDependID;

  /* Done! */
  return;
}

/** Calls a module setup
 */
void orxFASTCALL orxModule_Setup(orxMODULE_ID _eModuleID)
{
  /* Checks */
  orxASSERT(_eModuleID < orxMODULE_ID_NUMBER);

  /* Is registered? */
  if(sstModule.astModuleInfo[_eModuleID].u32StatusFlags & orxMODULE_KU32_STATUS_FLAG_REGISTERED)
  {
    /* Has setup function? */
    if(sstModule.astModuleInfo[_eModuleID].pfnSetup != orxNULL)
    {
      /* Calls it */
      sstModule.astModuleInfo[_eModuleID].pfnSetup();
    }
  }

  /* Done! */
  return;
}

/** Calls all module setups
 */
void orxFASTCALL orxModule_SetupAll()
{
  orxU32 eID;

  /* Clears static variable */
  sstModule.u32InitLoopCounter = 0;

  /* For all modules */
  for(eID = 0; eID < orxMODULE_ID_NUMBER; eID++)
  {
    /* Calls module setup */
    orxModule_Setup((orxMODULE_ID)eID);
  }

  return;
}

/** Inits a module recursively
 */
orxSTATUS orxFASTCALL orxModule_Init(orxMODULE_ID _eModuleID)
{
  orxU64    u64Depend;
  orxU32    u32Index;
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(orxMODULE_ID_NUMBER <= orxMODULE_ID_MAX_NUMBER);
  orxASSERT(_eModuleID < orxMODULE_ID_NUMBER);

  /* Increases loop counter */
  sstModule.u32InitLoopCounter++;

  /* Is module registered? */
  if(sstModule.astModuleInfo[_eModuleID].u32StatusFlags & orxMODULE_KU32_STATUS_FLAG_REGISTERED)
  {
    /* Is not initialized? */
    if(!(sstModule.astModuleInfo[_eModuleID].u32StatusFlags & (orxMODULE_KU32_STATUS_FLAG_INITIALIZED|orxMODULE_KU32_STATUS_FLAG_PENDING)))
    {
      /* For all dependencies */
      for(u64Depend = sstModule.astModuleInfo[_eModuleID].u64DependFlags, u32Index = 0;
          u64Depend != (orxU64)0;
          u64Depend >>= 1, u32Index++)
      {
        /* Depends? */
        if(u64Depend & (orxU64)1)
        {
          /* Not already initialized */
          if(!(sstModule.astModuleInfo[u32Index].u32StatusFlags & (orxMODULE_KU32_STATUS_FLAG_INITIALIZED|orxMODULE_KU32_STATUS_FLAG_PENDING)))
          {
            /* Inits it */
            eResult = orxModule_Init((orxMODULE_ID)u32Index);

            /* Failed ? */
            if(eResult == orxSTATUS_FAILURE)
            {
              /* Stops init here */
              break;
            }
          }
        }
      }

      /* For all optional dependencies */
      for(u64Depend = sstModule.astModuleInfo[_eModuleID].u64OptionalDependFlags, u32Index = 0;
          u64Depend != (orxU64)0;
          u64Depend >>= 1, u32Index++)
      {
        /* Depends? */
        if(u64Depend & (orxU64)1)
        {
          /* Not already initialized */
          if(!(sstModule.astModuleInfo[u32Index].u32StatusFlags & orxMODULE_KU32_STATUS_FLAG_INITIALIZED))
          {
            /* Inits it */
            orxModule_Init((orxMODULE_ID)u32Index);
          }
        }
      }

      /* All dependencies initialized? */
      if(eResult != orxSTATUS_FAILURE)
      {
        /* Not already initialized */
        if(!(sstModule.astModuleInfo[_eModuleID].u32StatusFlags & orxMODULE_KU32_STATUS_FLAG_INITIALIZED))
        {
          /* Updates temp flag */
          sstModule.astModuleInfo[_eModuleID].u32StatusFlags |= orxMODULE_KU32_STATUS_FLAG_PENDING;

          /* Calls module init function */
          eResult = (sstModule.astModuleInfo[_eModuleID].pfnInit != orxNULL) ? sstModule.astModuleInfo[_eModuleID].pfnInit() : orxSTATUS_SUCCESS;

          /* Successful? */
          if(eResult != orxSTATUS_FAILURE)
          {
            /* Updates initialized flag */
            sstModule.astModuleInfo[_eModuleID].u32StatusFlags |= orxMODULE_KU32_STATUS_FLAG_INITIALIZED;
          }
          else
          {
            /* Updates temp flag */
            sstModule.astModuleInfo[_eModuleID].u32StatusFlags &= ~orxMODULE_KU32_STATUS_FLAG_PENDING;
          }
        }
      }
    }
  }
  else
  {
    /* Not initialized */
    eResult = orxSTATUS_FAILURE;
  }

  /* Decreases loop counter */
  sstModule.u32InitLoopCounter--;

  /* Was external call? */
  if(sstModule.u32InitLoopCounter == 0)
  {
    /* Failed? */
    if(eResult == orxSTATUS_FAILURE)
    {
      /* For all modules */
      for(u32Index = 0; u32Index < orxMODULE_ID_NUMBER; u32Index++)
      {
        /* Is temporary initialized? */
        if(sstModule.astModuleInfo[u32Index].u32StatusFlags & orxMODULE_KU32_STATUS_FLAG_PENDING)
        {
          /* Internal exit call */
          orxModule_Exit((orxMODULE_ID)u32Index);
        }
      }
    }
    /* Successful */
    else
    {
      /* For all modules */
      for(u32Index = 0; u32Index < orxMODULE_ID_NUMBER; u32Index++)
      {
        /* Cleans temp status */
        sstModule.astModuleInfo[u32Index].u32StatusFlags &= ~orxMODULE_KU32_STATUS_FLAG_PENDING;
      }
    }
  }

  /* Done! */
  return eResult;
}

/** Inits all modules
 */
orxSTATUS orxFASTCALL orxModule_InitAll()
{
  orxU32    eID, u32InitCounter;
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* For all modules */
  for(eID = 0, u32InitCounter = 0; eID < orxMODULE_ID_NUMBER; eID++)
  {
    /* Calls module init */
    eResult = orxModule_Init((orxMODULE_ID)eID);

    /* Failed? */
    if(eResult != orxSTATUS_FAILURE)
    {
      /* Updates init counter */
      u32InitCounter++;
    }
    else
    {
      /* Logs message */
      orxDEBUG_PRINT(orxDEBUG_LEVEL_SYSTEM, "Failed to initialized module ID (%d).", eID);
    }
  }

  /* Nothing initialized? */
  if(u32InitCounter == 0)
  {
    /* Failed */
    eResult = orxSTATUS_FAILURE;

    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_SYSTEM, "No modules initialized.");
  }
  else
  {
    /* Success */
    eResult = orxSTATUS_SUCCESS;
  }

  /* Done! */
  return eResult;
}

/** Exits from a module recursively
 */
void orxFASTCALL orxModule_Exit(orxMODULE_ID _eModuleID)
{
  orxU64 u64Depend;
  orxU32 u32Index;

  /* Checks */
  orxASSERT(_eModuleID < orxMODULE_ID_NUMBER);

  /* Is initialized? */
  if(sstModule.astModuleInfo[_eModuleID].u32StatusFlags & orxMODULE_KU32_STATUS_FLAG_INITIALIZED)
  {
    /* Cleans flags */
    sstModule.astModuleInfo[_eModuleID].u32StatusFlags &= ~(orxMODULE_KU32_STATUS_FLAG_INITIALIZED|orxMODULE_KU32_STATUS_FLAG_PENDING);

    /* Computes dependency flag */
    u64Depend = (orxU64)1 << _eModuleID;

    /* For all modules */
    for(u32Index = 0; u32Index < orxMODULE_ID_NUMBER; u32Index++)
    {
      /* Is module dependent? */
      if(sstModule.astModuleInfo[u32Index].u64DependFlags & u64Depend)
      {
        /* Exits from it */
        orxModule_Exit((orxMODULE_ID)u32Index);
      }
    }

    /* For all optional modules */
    for(u32Index = 0; u32Index < orxMODULE_ID_NUMBER; u32Index++)
    {
      /* Is module dependent? */
      if(sstModule.astModuleInfo[u32Index].u64OptionalDependFlags & u64Depend)
      {
        /* Exits from it */
        orxModule_Exit((orxMODULE_ID)u32Index);
      }
    }

    /* Has module exit function? */
    if(sstModule.astModuleInfo[_eModuleID].pfnExit != orxNULL)
    {
      /* Calls it */
      sstModule.astModuleInfo[_eModuleID].pfnExit();
    }
  }

  return;
}

/** Exits from all modules
 */
void orxFASTCALL orxModule_ExitAll()
{
  orxU32 eID;

  /* For all modules */
  for(eID = 0; eID < orxMODULE_ID_NUMBER; eID++)
  {
    /* Calls module exit */
    orxModule_Exit((orxMODULE_ID)eID);
  }

  /* Done! */
  return;
}

/** Is module initialized?
 * @param[in] _eModulueID       Concerned module ID
 * @return orxTRUE / orxFALSE
 */
orxBOOL orxFASTCALL orxModule_IsInitialized(orxMODULE_ID _eModuleID)
{
  orxBOOL bResult;

  /* Checks */
  orxASSERT(_eModuleID < orxMODULE_ID_NUMBER);

  /* Updates result */
  bResult = orxFLAG_TEST(sstModule.astModuleInfo[_eModuleID].u32StatusFlags, orxMODULE_KU32_STATUS_FLAG_INITIALIZED) ? orxTRUE : orxFALSE;

  /* Done! */
  return bResult;
}
