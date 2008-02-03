/**
 * @file orxModule.c
 * 
 * Module registration
 * 
 * @todo
 * - Add optimizations (space/time) for dependencies storage/computation
 * - Add extra non-core modules registering + dependencies declaration
 */
 
 /***************************************************************************
 orxModule.c
 Module registration

 begin                : 12/09/2005
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

#include "orx.h"


/***************************************************************************
 ***************************************************************************
 ******                       PUBLIC ZONE                             ******
 ***************************************************************************
 ***************************************************************************/

/* *** Module registration macro *** */
#define orxMODULE_REGISTER(MODULE_ID, MODULE_BASENAME)  orxModule_Register(MODULE_ID, MODULE_BASENAME##_Setup, MODULE_BASENAME##_Init, MODULE_BASENAME##_Exit)


/** Registers all engine modules
 */
orxVOID orxModule_RegisterAll()
{
  /* *** All modules registration *** */
  orxMODULE_REGISTER(orxMODULE_ID_ANIM, orxAnim);
  orxMODULE_REGISTER(orxMODULE_ID_ANIMPOINTER, orxAnimPointer);
  orxMODULE_REGISTER(orxMODULE_ID_ANIMSET, orxAnimSet);
  orxMODULE_REGISTER(orxMODULE_ID_BANK, orxBank);
  orxMODULE_REGISTER(orxMODULE_ID_CAMERA, orxCamera);
  orxMODULE_REGISTER(orxMODULE_ID_CLOCK, orxClock);
  orxMODULE_REGISTER(orxMODULE_ID_DISPLAY, orxDisplay);
  orxMODULE_REGISTER(orxMODULE_ID_EVENT, orxEvent);
  orxMODULE_REGISTER(orxMODULE_ID_FILE, orxFile);
  orxMODULE_REGISTER(orxMODULE_ID_FILESYSTEM, orxFileSystem);
    orxMODULE_REGISTER(orxMODULE_ID_FPS, orxFPS);
  orxMODULE_REGISTER(orxMODULE_ID_FRAME, orxFrame);
  orxMODULE_REGISTER(orxMODULE_ID_FSM, orxFSM);
  orxMODULE_REGISTER(orxMODULE_ID_GRAPHIC, orxGraphic);
  orxMODULE_REGISTER(orxMODULE_ID_HASHTABLE, orxHashTable);
  orxMODULE_REGISTER(orxMODULE_ID_JOYSTICK, orxJoystick);
  orxMODULE_REGISTER(orxMODULE_ID_KEYBOARD, orxKeyboard);
  orxMODULE_REGISTER(orxMODULE_ID_LINKLIST, orxLinkList);
  orxMODULE_REGISTER(orxMODULE_ID_MEMORY, orxMemory);
  orxMODULE_REGISTER(orxMODULE_ID_MOUSE, orxMouse);
  orxMODULE_REGISTER(orxMODULE_ID_OBJECT, orxObject);
  orxMODULE_REGISTER(orxMODULE_ID_PACKAGE, orxPackage);
  orxMODULE_REGISTER(orxMODULE_ID_PARAM, orxParam);
  orxMODULE_REGISTER(orxMODULE_ID_PLUGIN, orxPlugin);
  orxMODULE_REGISTER(orxMODULE_ID_QUEUE, orxQueue);
  orxMODULE_REGISTER(orxMODULE_ID_RENDER, orxRender);
  orxMODULE_REGISTER(orxMODULE_ID_SCRIPT, orxScript);
  orxMODULE_REGISTER(orxMODULE_ID_SOUND, orxSound);
  orxMODULE_REGISTER(orxMODULE_ID_SCREENSHOT, orxScreenshot);
  orxMODULE_REGISTER(orxMODULE_ID_STRUCTURE, orxStructure);
  orxMODULE_REGISTER(orxMODULE_ID_TEXTIO, orxTextIO);
  orxMODULE_REGISTER(orxMODULE_ID_TEXTURE, orxTexture);
  orxMODULE_REGISTER(orxMODULE_ID_TIME, orxTime);
  orxMODULE_REGISTER(orxMODULE_ID_TREE, orxTree);
  orxMODULE_REGISTER(orxMODULE_ID_VIEWPORT, orxViewport);

#ifdef __orxTEST__
  orxMODULE_REGISTER(orxMODULE_ID_TEST, orxTest);
#endif /* __orxTEST__ */

  /* Computes all dependencies */
  orxModule_UpdateDependencies();

  return;
}


/***************************************************************************
 ***************************************************************************
 ******                       PRIVATE ZONE                            ******
 ***************************************************************************
 ***************************************************************************/

/*
 * Module flags
 */
#define orxMODULE_KU32_STATIC_FLAG_NONE         0x00000000L


/*
 * Module status flags
 */
#define orxMODULE_KU32_STATUS_FLAG_NONE         0x00000000L

#define orxMODULE_KU32_STATUS_FLAG_REGISTERED   0x00000001L
#define orxMODULE_KU32_STATUS_FLAG_INITIALIZED  0x00000002L
#define orxMODULE_KU32_STATUS_FLAG_TEMP         0x00010000L


/*
 * Internal module info structure
 */
typedef struct __orxMODULE_INFO_t
{
  /* Dependency flags : 8 */
  orxU64                    u64DependFlags;

  /* Setup function : 12 */
  orxMODULE_SETUP_FUNCTION  pfnSetup;

  /* Init function : 16 */
  orxMODULE_INIT_FUNCTION   pfnInit;

  /* Exit function : 20 */
  orxMODULE_EXIT_FUNCTION   pfnExit;

  /* Status flags : 24 */
  orxU32                    u32StatusFlags;

  /* Padding */
  orxPAD(24)

} orxMODULE_INFO;

/*
 * Static structure 
 */
typedef struct __orxMODULE_STATIC_t
{
  orxMODULE_INFO astModuleInfo[orxMODULE_ID_NUMBER];

  orxU32 u32Flags;

} orxMODULE_STATIC;


/*
 * Static data
 */
orxSTATIC orxMODULE_STATIC sstModule;


/***************************************************************************
 ***************************************************************************
 ******                       LOCAL FUNCTIONS                         ******
 ***************************************************************************
 ***************************************************************************/

/** Exits from a module recursively
 */
orxVOID orxFASTCALL _orxModule_Exit(orxMODULE_ID _eModuleID)
{
  orxU64    u64Depend;
  orxU32    u32Index;

  /* Is initialized? */
  if(sstModule.astModuleInfo[_eModuleID].u32StatusFlags & orxMODULE_KU32_STATUS_FLAG_INITIALIZED)
  {
    /* Cleans flags */
    sstModule.astModuleInfo[_eModuleID].u32StatusFlags &= ~(orxMODULE_KU32_STATUS_FLAG_INITIALIZED|orxMODULE_KU32_STATUS_FLAG_TEMP);

    /* Computes dependency flag */
    u64Depend = (orxU64)1 << _eModuleID;

    /* For all modules */
    for(u32Index = 0; u32Index < orxMODULE_ID_NUMBER; u32Index++)
    {
      /* Is module dependent? */
      if(sstModule.astModuleInfo[u32Index].u64DependFlags & u64Depend)
      {
        /* Exits from it */
        _orxModule_Exit(u32Index);
      }
    }

    /* Calls module exit function */
    sstModule.astModuleInfo[_eModuleID].pfnExit();
  }

  return;
}

/** Inits a module recursively
 */
orxSTATUS orxFASTCALL _orxModule_Init(orxMODULE_ID _eModuleID, orxBOOL _bExternCall)
{
  orxU64    u64Depend;
  orxU32    u32Index;
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Is module registered? */
  if(sstModule.astModuleInfo[_eModuleID].u32StatusFlags & orxMODULE_KU32_STATUS_FLAG_REGISTERED)
  {
    /* Is not initialized? */
    if(!(sstModule.astModuleInfo[_eModuleID].u32StatusFlags & orxMODULE_KU32_STATUS_FLAG_INITIALIZED))
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
          if(!(sstModule.astModuleInfo[u32Index].u32StatusFlags & orxMODULE_KU32_STATUS_FLAG_INITIALIZED))
          {
            /* Inits it */
            eResult = _orxModule_Init(u32Index, orxFALSE);
    
            /* Failed ? */
            if(eResult != orxSTATUS_SUCCESS)
            {
              /* Stops init here */
              break;
            }
          }
        }
      }

      /* All dependencies initialized? */
      if(eResult == orxSTATUS_SUCCESS)
      {
        /* Calls module init function */
        eResult = sstModule.astModuleInfo[_eModuleID].pfnInit();

        /* Successful? */
        if(eResult == orxSTATUS_SUCCESS)
        {
          /* Updates initialized flag */
          sstModule.astModuleInfo[_eModuleID].u32StatusFlags |= orxMODULE_KU32_STATUS_FLAG_INITIALIZED|orxMODULE_KU32_STATUS_FLAG_TEMP;
        }
      }
    }
  }
  else
  {
    /* Not initialized */
    eResult = orxSTATUS_FAILURE;
  }

  /* Was external call? */
  if(_bExternCall != orxFALSE)
  {
    /* Failed? */
    if(eResult != orxSTATUS_SUCCESS)
    {
      /* For all modules */
      for(u32Index = 0; u32Index < orxMODULE_ID_NUMBER; u32Index++)
      {
        /* Is temporary initialized? */
        if(sstModule.astModuleInfo[u32Index].u32StatusFlags & orxMODULE_KU32_STATUS_FLAG_TEMP)
        {
          /* Internal exit call */
          _orxModule_Exit(u32Index);
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
        sstModule.astModuleInfo[u32Index].u32StatusFlags &= ~orxMODULE_KU32_STATUS_FLAG_TEMP;
      }
    }
  }

  /* Done! */
  return eResult;
}


/***************************************************************************
 ***************************************************************************
 ******                       PUBLIC FUNCTIONS                        ******
 ***************************************************************************
 ***************************************************************************/

/** Registers a module
 */
orxVOID orxFASTCALL orxModule_Register(orxMODULE_ID _eModuleID, orxCONST orxMODULE_SETUP_FUNCTION _pfnSetup, orxCONST orxMODULE_INIT_FUNCTION _pfnInit, orxCONST orxMODULE_EXIT_FUNCTION _pfnExit)
{
  /* Checks */
  orxASSERT(_eModuleID < orxMODULE_ID_NUMBER);
  orxASSERT(!(sstModule.astModuleInfo[_eModuleID].u32StatusFlags & orxMODULE_KU32_STATUS_FLAG_REGISTERED));

  /* Stores module functions */
  sstModule.astModuleInfo[_eModuleID].pfnSetup  = _pfnSetup;
  sstModule.astModuleInfo[_eModuleID].pfnInit   = _pfnInit;
  sstModule.astModuleInfo[_eModuleID].pfnExit   = _pfnExit;

  /* Updates module status flags */
  sstModule.astModuleInfo[_eModuleID].u32StatusFlags |= orxMODULE_KU32_STATUS_FLAG_REGISTERED;

  /* Done! */
  return;
}

/** Adds dependencies between 2 modules
 */
orxVOID orxFASTCALL orxModule_AddDependency(orxMODULE_ID _eModuleID, orxMODULE_ID _eDependID)
{
  /* Checks */
  orxASSERT(_eModuleID < orxMODULE_ID_NUMBER);
  orxASSERT(_eDependID < orxMODULE_ID_NUMBER);

  /* Stores dependency */
  sstModule.astModuleInfo[_eModuleID].u64DependFlags |= ((orxU64)1) << _eDependID;

  /* Done! */
  return;
}

/** Updates dependencies for all modules
 */
orxVOID orxModule_UpdateDependencies()
{
  /* !!! TODO !!! */

  return;
}

/** Calls a module setup
 */
orxVOID orxFASTCALL orxModule_Setup(orxMODULE_ID _eModuleID)
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
orxVOID orxModule_SetupAll()
{
  orxMODULE_ID eID;

  /* For all modules */
  for(eID = 0; eID < orxMODULE_ID_NUMBER; eID++)
  {
    /* Calls module setup */
    orxModule_Setup(eID);
  }

  return;
}

/** Inits a module
 */
orxSTATUS orxFASTCALL orxModule_Init(orxMODULE_ID _eModuleID)
{
  /* Checks */
  orxASSERT(orxMODULE_ID_NUMBER <= orxMODULE_ID_MAX_NUMBER);
  orxASSERT(_eModuleID < orxMODULE_ID_NUMBER);

  /* Calls internal init */
  return(_orxModule_Init(_eModuleID, orxTRUE));
}

/** Inits all modules
 */
orxSTATUS orxModule_InitAll()
{
  orxMODULE_ID  eID;
  orxSTATUS     eResult = orxSTATUS_SUCCESS;
  orxU32        u32InitCounter;

  /* For all modules */
  for(eID = 0, u32InitCounter = 0; eID < orxMODULE_ID_NUMBER; eID++)
  {
    /* Calls module init */
    eResult = orxModule_Init(eID);

    /* Failed? */
    if(eResult == orxSTATUS_SUCCESS)
    {
      /* Updates init counter */
      u32InitCounter++;
    }
    else
    {
      /* !!! MSG !!! */
    }
  }

  /* Nothing initialized? */
  if(u32InitCounter == 0)
  {
    /* Failed */
    eResult = orxSTATUS_FAILURE;

    /* !!! MSG !!! */
  }
  else
  {
    /* Success */
    eResult = orxSTATUS_SUCCESS;
  }

  /* Done! */
  return eResult;
}

/** Exits from a module
 */
orxVOID orxFASTCALL orxModule_Exit(orxMODULE_ID _eModuleID)
{
  /* Checks */
  orxASSERT(_eModuleID < orxMODULE_ID_NUMBER);

  /* Calls internal exit */
  _orxModule_Exit(_eModuleID);

  return;
}

/** Exits from all modules
 */
orxVOID orxModule_ExitAll()
{
  orxMODULE_ID  eID;

  /* For all modules */
  for(eID = 0; eID < orxMODULE_ID_NUMBER; eID++)
  {
    /* Calls module exit */
    orxModule_Exit(eID);
  }

  /* Done! */
  return;
}
