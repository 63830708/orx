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
 * @file orxSpawner.c
 * @date 06/09/2008
 * @author iarwain@orx-project.org
 *
 * @todo
 */


#include "object/orxSpawner.h"

#include "debug/orxDebug.h"
#include "core/orxConfig.h"
#include "core/orxEvent.h"
#include "memory/orxMemory.h"
#include "object/orxObject.h"
#include "render/orxCamera.h"


/** Module flags
 */
#define orxSPAWNER_KU32_STATIC_FLAG_NONE          0x00000000

#define orxSPAWNER_KU32_STATIC_FLAG_READY         0x00000001

#define orxSPAWNER_KU32_STATIC_MASK_ALL           0xFFFFFFFF


/** Flags
 */
#define orxSPAWNER_KU32_FLAG_NONE                 0x00000000  /**< No flags */

#define orxSPAWNER_KU32_FLAG_ENABLED              0x10000000  /**< Enabled flag */
#define orxSPAWNER_KU32_FLAG_AUTO_DELETE          0x20000000  /**< Auto delete flag */
#define orxSPAWNER_KU32_FLAG_AUTO_RESET           0x40000000  /**< Auto delete flag */
#define orxSPAWNER_KU32_FLAG_TOTAL_LIMIT          0x01000000  /**< Total limit flag */
#define orxSPAWNER_KU32_FLAG_ACTIVE_LIMIT         0x02000000  /**< Active limit flag */

#define orxSPAWNER_KU32_MASK_ALL                  0xFFFFFFFF  /**< All mask */


/** Misc defines
 */
#define orxSPAWNER_KZ_CONFIG_OBJECT               "Object"
#define orxSPAWNER_KZ_CONFIG_POSITION             "Position"
#define orxSPAWNER_KZ_CONFIG_ROTATION             "Rotation"
#define orxSPAWNER_KZ_CONFIG_SCALE                "Scale"
#define orxSPAWNER_KZ_CONFIG_TOTAL_OBJECT         "TotalObject"
#define orxSPAWNER_KZ_CONFIG_ACTIVE_OBJECT        "ActiveObject"
#define orxSPAWNER_KZ_CONFIG_AUTO_DELETE          "AutoDelete"
#define orxSPAWNER_KZ_CONFIG_AUTO_RESET           "AutoReset"


/***************************************************************************
 * Structure declaration                                                   *
 ***************************************************************************/

/** Spawner structure
 */
struct __orxSPAWNER_t
{
  orxSTRUCTURE        stStructure;                /**< Public structure, first structure member : 16 */
  orxSTRING           zReference;                 /**< Spawner reference : 20 */
  orxU16              u16TotalObjectLimit;        /**< Limit of objects that can be spawned, 0 for unlimited stock : 22 */
  orxU16              u16ActiveObjectLimit;       /**< Limit of active objects at the same time, 0 for unlimited : 24 */
  orxU16              u16TotalObjectCounter;      /**< Total spawned objects counter : 26 */
  orxU16              u16ActiveObjectCounter;     /**< Active objects counter : 28 */
  orxSTRING           zObjectName;                /**< Object name : 32 */
  orxFRAME           *pstFrame;                   /**< Frame : 36 */

  /* Padding */
  orxPAD(36)
};

/** Static structure
 */
typedef struct __orxSPAWNER_STATIC_t
{
  orxU32 u32Flags;                                /**< Control flags */

} orxSPAWNER_STATIC;


/***************************************************************************
 * Static variables                                                        *
 ***************************************************************************/

/** Static data
 */
orxSTATIC orxSPAWNER_STATIC sstSpawner;


/***************************************************************************
 * Private functions                                                       *
 ***************************************************************************/

/** Event handler
 * @param[in]   _pstEvent                     Sent event
 * @return      orxSTATUS_SUCCESS if handled / orxSTATUS_FAILURE otherwise
 */
orxSTATIC orxSTATUS orxFASTCALL orxSpawner_EventHandler(orxCONST orxEVENT *_pstEvent)
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Checks */
  orxASSERT(_pstEvent->eType == orxEVENT_TYPE_OBJECT);

  /* Depending on event ID */
  switch(_pstEvent->eID)
  {
    /* Delete event */
    case orxOBJECT_EVENT_DELETE:
    {
      orxOBJECT  *pstObject;
      orxSPAWNER *pstSpawner;

      /* Gets corresponding object */
      pstObject = orxOBJECT(_pstEvent->hSender);

      /* Gets owner */
      pstSpawner = orxSPAWNER(orxObject_GetOwner(pstObject));

      /* Is a spawner linked to it? */
      if(pstSpawner != orxNULL)
      {
        /* Checks */
        orxASSERT(pstSpawner->u16ActiveObjectCounter > 0);

        /* Decreases its active objects counter */
        pstSpawner->u16ActiveObjectCounter--;

        break;
      }

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

/** Deletes all the spawners
 */
orxSTATIC orxINLINE orxVOID orxSpawner_DeleteAll()
{
  orxSPAWNER *pstSpawner;

  /* Gets first spawner */
  pstSpawner = orxSPAWNER(orxStructure_GetFirst(orxSTRUCTURE_ID_SPAWNER));

  /* Non empty? */
  while(pstSpawner != orxNULL)
  {
    /* Deletes spawner */
    orxSpawner_Delete(pstSpawner);

    /* Gets first spawner */
    pstSpawner = orxSPAWNER(orxStructure_GetFirst(orxSTRUCTURE_ID_SPAWNER));
  }

  return;
}

/** Updates the spawner (Callback for generic structure update calling)
 * @param[in]   _pstStructure                 Generic Structure or the concerned Body
 * @param[in]   _pstCaller                    Structure of the caller
 * @param[in]   _pstClockInfo                 Clock info used for time updates
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATIC orxSTATUS orxFASTCALL orxSpawner_Update(orxSTRUCTURE *_pstStructure, orxCONST orxSTRUCTURE *_pstCaller, orxCONST orxCLOCK_INFO *_pstClockInfo)
{
  orxSPAWNER *pstSpawner;
  orxOBJECT  *pstObject;
  orxSTATUS   eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstSpawner.u32Flags & orxSPAWNER_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstStructure);
  orxSTRUCTURE_ASSERT(_pstCaller);

  /* Gets spawner */
  pstSpawner = orxSPAWNER(_pstStructure);

  /* Gets calling object */
  pstObject = orxOBJECT(_pstCaller);

  /* Is enabled? */
  if(orxSpawner_IsEnabled(pstSpawner) != orxFALSE)
  {
    //! TODO
  }

  /* Done! */
  return eResult;
}


/***************************************************************************
 * Public functions                                                        *
 ***************************************************************************/

/** Spawner module setup
 */
orxVOID orxSpawner_Setup()
{
  /* Adds module dependencies */
  orxModule_AddDependency(orxMODULE_ID_SPAWNER, orxMODULE_ID_MEMORY);
  orxModule_AddDependency(orxMODULE_ID_SPAWNER, orxMODULE_ID_BANK);
  orxModule_AddDependency(orxMODULE_ID_SPAWNER, orxMODULE_ID_STRUCTURE);
  orxModule_AddDependency(orxMODULE_ID_SPAWNER, orxMODULE_ID_CONFIG);
  orxModule_AddDependency(orxMODULE_ID_SPAWNER, orxMODULE_ID_EVENT);
  orxModule_AddDependency(orxMODULE_ID_SPAWNER, orxMODULE_ID_FRAME);

  return;
}

/** Inits the spawner module
 * @return orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxSpawner_Init()
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Not already Initialized? */
  if(!orxFLAG_TEST(sstSpawner.u32Flags, orxSPAWNER_KU32_STATIC_FLAG_READY))
  {
    /* Cleans static controller */
    orxMemory_Zero(&sstSpawner, sizeof(orxSPAWNER_STATIC));

    /* Registers event handler */
    eResult = orxEvent_AddHandler(orxEVENT_TYPE_OBJECT, orxSpawner_EventHandler);

    /* Valid? */
    if(eResult != orxSTATUS_FAILURE)
    {
      /* Registers structure type */
      eResult = orxSTRUCTURE_REGISTER(SPAWNER, orxSTRUCTURE_STORAGE_TYPE_LINKLIST, orxMEMORY_TYPE_MAIN, orxSpawner_Update);

      /* Initialized? */
      if(eResult != orxSTATUS_FAILURE)
      {
        /* Inits Flags */
        orxFLAG_SET(sstSpawner.u32Flags, orxSPAWNER_KU32_STATIC_FLAG_READY, orxSPAWNER_KU32_STATIC_MASK_ALL);
      }
      else
      {
        /* !!! MSG !!! */
      }
    }
    else
    {
      /* !!! MSG !!! */
    }
  }
  else
  {
    /* !!! MSG !!! */

    /* Already initialized */
    eResult = orxSTATUS_SUCCESS;
  }

  /* Done! */
  return eResult;
}

/** Exits from the spawner module
 */
orxVOID orxSpawner_Exit()
{
  /* Initialized? */
  if(orxFLAG_TEST(sstSpawner.u32Flags, orxSPAWNER_KU32_STATIC_FLAG_READY))
  {
    /* Deletes spawner list */
    orxSpawner_DeleteAll();

    /* Removes event handler */
    orxEvent_RemoveHandler(orxEVENT_TYPE_OBJECT, orxSpawner_EventHandler);

    /* Unregisters structure type */
    orxStructure_Unregister(orxSTRUCTURE_ID_SPAWNER);

    /* Updates flags */
    orxFLAG_SET(sstSpawner.u32Flags, orxSPAWNER_KU32_FLAG_NONE, orxSPAWNER_KU32_STATIC_FLAG_READY);
  }
  else
  {
    /* !!! MSG !!! */
  }

  return;
}

/** Creates an empty spawner
 * @return      Created orxSPAWNER / orxNULL
 */
orxSPAWNER *orxSpawner_Create()
{
  orxSPAWNER *pstResult;

  /* Checks */
  orxASSERT(sstSpawner.u32Flags & orxSPAWNER_KU32_STATIC_FLAG_READY);

  /* Creates spawner */
  pstResult = orxSPAWNER(orxStructure_Create(orxSTRUCTURE_ID_SPAWNER));

  /* Created? */
  if(pstResult != orxNULL)
  {
    /* Creates frame */
    pstResult->pstFrame = orxFrame_Create(orxFRAME_KU32_FLAG_NONE);

    /* Valid? */
    if(pstResult->pstFrame != orxNULL)
    {
      /* Increases its frame counter */
      orxStructure_IncreaseCounter(pstResult->pstFrame);

      /* Inits flags */
      orxStructure_SetFlags(pstResult, orxSPAWNER_KU32_FLAG_ENABLED, orxSPAWNER_KU32_MASK_ALL);
    }
    else
    {
      /* !!! MSG !!! */

      /* Deletes spawner */
      orxStructure_Delete(pstResult);

      /* Updates result */
      pstResult = orxNULL;
    }
  }
  else
  {
    /* !!! MSG !!! */
  }

  return pstResult;
}

/** Creates a spawner from config
 * @param[in]   _zConfigID            Config ID
 * @ return orxSPAWNER / orxNULL
 */
orxSPAWNER *orxFASTCALL orxSpawner_CreateFromConfig(orxCONST orxSTRING _zConfigID)
{
  orxSPAWNER  *pstResult;
  orxSTRING   zPreviousSection;

  /* Checks */
  orxASSERT(sstSpawner.u32Flags & orxSPAWNER_KU32_STATIC_FLAG_READY);
  orxASSERT((_zConfigID != orxNULL) && (*_zConfigID != *orxSTRING_EMPTY));

  /* Gets previous config section */
  zPreviousSection = orxConfig_GetCurrentSection();

  /* Selects section */
  if((orxConfig_HasSection(_zConfigID) != orxFALSE)
  && (orxConfig_SelectSection(_zConfigID) != orxSTATUS_FAILURE))
  {
    /* Creates spawner */
    pstResult = orxSpawner_Create();

    /* Valid? */
    if(pstResult != orxNULL)
    {
      orxVECTOR vValue;
      orxU32    u32Value;

      /* Stores its reference */
      pstResult->zReference = orxConfig_GetCurrentSection();

      /* Sets object name */
      pstResult->zObjectName = orxConfig_GetString(orxSPAWNER_KZ_CONFIG_OBJECT);

      /* Gets total limit */
      u32Value = orxConfig_GetU32(orxSPAWNER_KZ_CONFIG_TOTAL_OBJECT);

      /* Checks */
      orxASSERT(u32Value <= 0xFFFF);

      /* Has limit? */
      if(u32Value > 0)
      {
        /* Sets it */
        pstResult->u16TotalObjectLimit = (orxU16)u32Value;

        /* Updates status */
        orxStructure_SetFlags(pstResult, orxSPAWNER_KU32_FLAG_TOTAL_LIMIT, orxSPAWNER_KU32_FLAG_NONE);
      }

      /* Gets active limit */
      u32Value = orxConfig_GetU32(orxSPAWNER_KZ_CONFIG_ACTIVE_OBJECT);

      /* Checks */
      orxASSERT(u32Value <= 0xFFFF);

      /* Has limit? */
      if(u32Value > 0)
      {
        /* Sets it */
        pstResult->u16ActiveObjectLimit = (orxU16)u32Value;

        /* Updates status */
        orxStructure_SetFlags(pstResult, orxSPAWNER_KU32_FLAG_ACTIVE_LIMIT, orxSPAWNER_KU32_FLAG_NONE);
      }

      /* Has a position? */
      if(orxConfig_GetVector(orxSPAWNER_KZ_CONFIG_POSITION, &vValue) != orxNULL)
      {
        /* Updates object position */
        orxSpawner_SetPosition(pstResult, &vValue);
      }

      /* Updates object rotation */
      orxSpawner_SetRotation(pstResult, orxMATH_KF_DEG_TO_RAD * orxConfig_GetFloat(orxSPAWNER_KZ_CONFIG_ROTATION));

      /* Has scale? */
      if(orxConfig_HasValue(orxSPAWNER_KZ_CONFIG_SCALE) != orxFALSE)
      {
        /* Is config scale not a vector? */
        if(orxConfig_GetVector(orxSPAWNER_KZ_CONFIG_SCALE, &vValue) == orxNULL)
        {
          orxFLOAT fScale;

          /* Gets config uniformed scale */
          fScale = orxConfig_GetFloat(orxSPAWNER_KZ_CONFIG_SCALE);

          /* Updates vector */
          orxVector_SetAll(&vValue, fScale);
        }

        /* Updates object scale */
        orxSpawner_SetScale(pstResult, &vValue);
      }

      /* Auto delete? */
      if(orxConfig_GetBool(orxSPAWNER_KZ_CONFIG_AUTO_DELETE) != orxFALSE)
      {
        /* Updates status */
        orxStructure_SetFlags(pstResult, orxSPAWNER_KU32_FLAG_AUTO_DELETE, orxSPAWNER_KU32_FLAG_NONE);
      }

      /* Auto reset? */
      if(orxConfig_GetBool(orxSPAWNER_KZ_CONFIG_AUTO_RESET) != orxFALSE)
      {
        /* Updates status */
        orxStructure_SetFlags(pstResult, orxSPAWNER_KU32_FLAG_AUTO_RESET, orxSPAWNER_KU32_FLAG_NONE);
      }
    }

    /* Restores previous section */
    orxConfig_SelectSection(zPreviousSection);
  }
  else
  {
    /* !!! MSG !!! */

    /* Updates result */
    pstResult = orxNULL;
  }

  /* Done! */
  return pstResult;
}

/** Deletes a spawner
 * @param[in] _pstSpawner        Concerned spawner
 * @return orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxSpawner_Delete(orxSPAWNER *_pstSpawner)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstSpawner.u32Flags & orxSPAWNER_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSpawner);

  /* Not referenced? */
  if(orxStructure_GetRefCounter(_pstSpawner) == 0)
  {
    orxOBJECT  *pstObject;
    orxEVENT    stEvent;

    /* Inits event */
    orxMemory_Zero(&stEvent, sizeof(orxEVENT));
    stEvent.eType   = orxEVENT_TYPE_SPAWNER;
    stEvent.eID     = orxSPAWNER_EVENT_DELETE;
    stEvent.hSender = _pstSpawner;

    /* Sends it */
    orxEvent_Send(&stEvent);

    /* For all objects */
    for(pstObject = orxOBJECT(orxStructure_GetFirst(orxSTRUCTURE_ID_OBJECT));
        pstObject != orxNULL;
        pstObject = orxOBJECT(orxStructure_GetNext(pstObject)))
    {
      /* Is spawner the owner */
      if(orxObject_GetOwner(pstObject) == _pstSpawner)
      {
        /* Removes it */
        orxObject_SetOwner(pstObject, orxNULL);
      }
    }

    /* Decreases frame's ref counter */
    orxStructure_DecreaseCounter(_pstSpawner->pstFrame);

    /* Deletes its frame */
    orxFrame_Delete(_pstSpawner->pstFrame);

    /* Deletes structure */
    orxStructure_Delete(_pstSpawner);
  }
  else
  {
    /* !!! MSG !!! */

    /* Referenced by others */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Enables/disables a spawner
 * @param[in]   _pstSpawner     Concerned spawner
 * @param[in]   _bEnable      Enable / disable
 */
orxVOID orxFASTCALL orxSpawner_Enable(orxSPAWNER *_pstSpawner, orxBOOL _bEnable)
{
  /* Checks */
  orxASSERT(sstSpawner.u32Flags & orxSPAWNER_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSpawner);

  /* Enable? */
  if(_bEnable != orxFALSE)
  {
    /* Updates status flags */
    orxStructure_SetFlags(_pstSpawner, orxSPAWNER_KU32_FLAG_ENABLED, orxSPAWNER_KU32_FLAG_NONE);
  }
  else
  {
    /* Updates status flags */
    orxStructure_SetFlags(_pstSpawner, orxSPAWNER_KU32_FLAG_NONE, orxSPAWNER_KU32_FLAG_ENABLED);
  }

  return;
}

/** Is spawner enabled?
 * @param[in]   _pstSpawner     Concerned spawner
 * @return      orxTRUE if enabled, orxFALSE otherwise
 */
orxBOOL orxFASTCALL orxSpawner_IsEnabled(orxCONST orxSPAWNER *_pstSpawner)
{
  /* Checks */
  orxASSERT(sstSpawner.u32Flags & orxSPAWNER_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSpawner);

  /* Done! */
  return(orxStructure_TestFlags(_pstSpawner, orxSPAWNER_KU32_FLAG_ENABLED));
}

/** Resets (and re-enables) a spawner
 * @param[in]   _pstSpawner     Concerned spawner
 */
orxVOID orxFASTCALL orxSpawner_Reset(orxSPAWNER *_pstSpawner)
{
  orxOBJECT  *pstObject;
  orxEVENT    stEvent;
  
  /* Checks */
  orxASSERT(sstSpawner.u32Flags & orxSPAWNER_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSpawner);

  /* Updates status */
  orxStructure_SetFlags(_pstSpawner, orxSPAWNER_KU32_FLAG_ENABLED, orxSPAWNER_KU32_FLAG_NONE);

  /* Inits event */
  orxMemory_Zero(&stEvent, sizeof(orxEVENT));
  stEvent.eType   = orxEVENT_TYPE_SPAWNER;
  stEvent.eID     = orxSPAWNER_EVENT_RESET;
  stEvent.hSender = _pstSpawner;

  /* Sends it */
  orxEvent_Send(&stEvent);

  /* Resets counters */
  _pstSpawner->u16ActiveObjectCounter = 0;
  _pstSpawner->u16TotalObjectCounter  = 0;

  /* For all objects */
  for(pstObject = orxOBJECT(orxStructure_GetFirst(orxSTRUCTURE_ID_OBJECT));
      pstObject != orxNULL;
      pstObject = orxOBJECT(orxStructure_GetNext(pstObject)))
  {
    /* Is spawner the owner */
    if(orxObject_GetOwner(pstObject) == _pstSpawner)
    {
      /* Removes it */
      orxObject_SetOwner(pstObject, orxNULL);
    }
  }

  return;
}

/** Spawns objects
 * @param[in]   _pstSpawner     Concerned spawner
 * @param[in]   _u32Number      Number of objects to spawn
 * @return      Number of spawned objects
 */
orxU32 orxFASTCALL orxSpawner_Spawn(orxSPAWNER *_pstSpawner, orxU32 _u32Number)
{
  orxU32 u32Result = 0;

  /* Checks */
  orxASSERT(sstSpawner.u32Flags & orxSPAWNER_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSpawner);
  orxASSERT(_u32Number > 0);

  /* Enabled? */
  if(orxStructure_TestFlags(_pstSpawner, orxSPAWNER_KU32_FLAG_ENABLED))
  {
    orxEVENT  stEvent;
    orxU32    u32SpawnNumber, i;

    /* Inits event */
    orxMemory_Zero(&stEvent, sizeof(orxEVENT));
    stEvent.eType   = orxEVENT_TYPE_SPAWNER;
    stEvent.eID     = orxSPAWNER_EVENT_SPAWN;
    stEvent.hSender = _pstSpawner;

    /* Has a total limit? */
    if(orxStructure_TestFlags(_pstSpawner, orxSPAWNER_KU32_FLAG_TOTAL_LIMIT))
    {
      orxU32 u32AvailableNumber;

      /* Gets number of total available objects left */
      u32AvailableNumber = (orxU32)_pstSpawner->u16TotalObjectLimit - (orxU32)_pstSpawner->u16TotalObjectCounter;

      /* Gets total spawnable number */
      u32SpawnNumber = orxMIN(_u32Number, u32AvailableNumber);
    }
    else
    {
      /* Gets full requested number */
      u32SpawnNumber = _u32Number;
    }

    /* Has an active limit? */
    if(orxStructure_TestFlags(_pstSpawner, orxSPAWNER_KU32_FLAG_ACTIVE_LIMIT))
    {
      orxU32 u32AvailableNumber;

      /* Gets number of available active objects left */
      u32AvailableNumber = (orxU32)_pstSpawner->u16ActiveObjectLimit - (orxU32)_pstSpawner->u16ActiveObjectCounter;

      /* Gets active spawnable number */
      u32SpawnNumber = orxMIN(u32SpawnNumber, u32AvailableNumber);
    }

    /* For all objects to spawn */
    for(i = 0; i < u32SpawnNumber; i++)
    {
      orxOBJECT *pstObject;

      /* Creates object */
      pstObject = orxObject_CreateFromConfig(_pstSpawner->zObjectName);

      /* Valid? */
      if(pstObject != orxNULL)
      {
        orxVECTOR vPosition, vSpawnerPosition, vScale, vSpawnerScale;
        orxFLOAT  fSpawnerRotation;

        /* Updates active object counter */
        _pstSpawner->u16ActiveObjectCounter++;

        /* Updates total object counter */
        _pstSpawner->u16TotalObjectCounter++;

        /* Sets spawner as owner */
        orxObject_SetOwner(pstObject, _pstSpawner);

        /* Gets spawner rotation */
        fSpawnerRotation = orxSpawner_GetWorldRotation(_pstSpawner);

        /* Updates object rotation */
        orxObject_SetRotation(pstObject, orxObject_GetRotation(pstObject) + fSpawnerRotation);

        /* Updates object scale */
        orxObject_SetScale(pstObject, orxVector_Mul(&vScale, orxObject_GetScale(pstObject, &vScale), orxSpawner_GetWorldScale(_pstSpawner, &vSpawnerScale)));

        /* Updates object position */
        orxObject_SetPosition(pstObject, orxVector_Add(&vPosition, orxVector_2DRotate(&vPosition, orxVector_Mul(&vPosition, orxObject_GetPosition(pstObject, &vPosition), &vSpawnerScale), fSpawnerRotation), orxSpawner_GetWorldPosition(_pstSpawner, &vSpawnerPosition)));

        /* Updates result */
        u32Result++;

        /* Updates event */
        stEvent.hRecipient = pstObject;

        /* Sends it */
        orxEvent_Send(&stEvent);
      }
    }

    /* Has a total limit? */
    if(orxStructure_TestFlags(_pstSpawner, orxSPAWNER_KU32_FLAG_TOTAL_LIMIT))
    {
      /* No available object left? */
      if((orxU32)_pstSpawner->u16TotalObjectLimit - (orxU32)_pstSpawner->u16TotalObjectCounter == 0)
      {
        /* Updates event */
        stEvent.eID         = orxSPAWNER_EVENT_EMPTY;
        stEvent.hRecipient  = orxNULL;

        /* Sends it */
        orxEvent_Send(&stEvent);

        /* Auto delete? */
        if(orxStructure_TestFlags(_pstSpawner, orxSPAWNER_KU32_FLAG_AUTO_DELETE))
        {
          /* Deletes spawner */
          orxSpawner_Delete(_pstSpawner);
        }
        /* Auto reset? */
        else if(orxStructure_TestFlags(_pstSpawner, orxSPAWNER_KU32_FLAG_AUTO_RESET))
        {
          /* Resets spawner */
          orxSpawner_Reset(_pstSpawner);
        }
        /* Disables */
        else
        {
          /* Updates status */
          orxStructure_SetFlags(_pstSpawner, orxSPAWNER_KU32_FLAG_NONE, orxSPAWNER_KU32_FLAG_ENABLED);
        }
      }
    }
  }

  /* Done! */
  return u32Result;
}

/** Gets spawner frame
 * @param[in]   _pstSpawner     Concerned spawner
 * @return      orxFRAME
 */
orxFRAME *orxFASTCALL orxSpawner_GetFrame(orxCONST orxSPAWNER *_pstSpawner)
{
  /* Checks */
  orxASSERT(sstSpawner.u32Flags & orxSPAWNER_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSpawner);

  /* Gets spawner frame */
  return(_pstSpawner->pstFrame);
}

/** Sets spawner position
 * @param[in]   _pstSpawner     Concerned spawner
 * @param[in]   _pvPosition     Spawner position
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxSpawner_SetPosition(orxSPAWNER *_pstSpawner, orxCONST orxVECTOR *_pvPosition)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstSpawner.u32Flags & orxSPAWNER_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSpawner);
  orxASSERT(_pvPosition != orxNULL);

  /* Sets spawner position */
  orxFrame_SetPosition(_pstSpawner->pstFrame, _pvPosition);

  /* Done! */
  return eResult;
}

/** Sets spawner rotation
 * @param[in]   _pstSpawner     Concerned spawner
 * @param[in]   _fRotation      Spawner rotation
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxSpawner_SetRotation(orxSPAWNER *_pstSpawner, orxFLOAT _fRotation)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstSpawner.u32Flags & orxSPAWNER_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSpawner);

  /* Sets Spawner rotation */
  orxFrame_SetRotation(_pstSpawner->pstFrame, _fRotation);

  /* Done! */
  return eResult;
}

/** Sets Spawner scale
 * @param[in]   _pstSpawner     Concerned Spawner
 * @param[in]   _pvScale        Spawner scale vector
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxSpawner_SetScale(orxSPAWNER *_pstSpawner, orxCONST orxVECTOR *_pvScale)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstSpawner.u32Flags & orxSPAWNER_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSpawner);
  orxASSERT(_pvScale != orxNULL);

  /* Sets frame scale */
  orxFrame_SetScale(_pstSpawner->pstFrame, _pvScale);

  /* Done! */
  return eResult;
}

/** Get spawner position
 * @param[in]   _pstSpawner     Concerned spawner
 * @param[out]  _pvPosition     Spawner position
 * @return      orxVECTOR / orxNULL
 */
orxVECTOR *orxFASTCALL orxSpawner_GetPosition(orxCONST orxSPAWNER *_pstSpawner, orxVECTOR *_pvPosition)
{
  orxVECTOR *pvResult;

  /* Checks */
  orxASSERT(sstSpawner.u32Flags & orxSPAWNER_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSpawner);
  orxASSERT(_pvPosition != orxNULL);

  /* Gets spawner position */
  pvResult = orxFrame_GetPosition(_pstSpawner->pstFrame, orxFRAME_SPACE_LOCAL, _pvPosition);

  /* Done! */
  return pvResult;
}

/** Get spawner world position
 * @param[in]   _pstSpawner     Concerned spawner
 * @param[out]  _pvPosition     Spawner world position
 * @return      orxVECTOR / orxNULL
 */
orxVECTOR *orxFASTCALL orxSpawner_GetWorldPosition(orxCONST orxSPAWNER *_pstSpawner, orxVECTOR *_pvPosition)
{
  orxVECTOR *pvResult;

  /* Checks */
  orxASSERT(sstSpawner.u32Flags & orxSPAWNER_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSpawner);
  orxASSERT(_pvPosition != orxNULL);

  /* Gets spawner position */
  pvResult = orxFrame_GetPosition(_pstSpawner->pstFrame, orxFRAME_SPACE_GLOBAL, _pvPosition);

  /* Done! */
  return pvResult;
}

/** Get spawner rotation
 * @param[in]   _pstSpawner     Concerned spawner
 * @return      orxFLOAT
 */
orxFLOAT orxFASTCALL orxSpawner_GetRotation(orxCONST orxSPAWNER *_pstSpawner)
{
  orxFLOAT fResult;

  /* Checks */
  orxASSERT(sstSpawner.u32Flags & orxSPAWNER_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSpawner);

  /* Gets spawner rotation */
  fResult = orxFrame_GetRotation(_pstSpawner->pstFrame, orxFRAME_SPACE_LOCAL);

  /* Done! */
  return fResult;
}

/** Get spawner world rotation
 * @param[in]   _pstSpawner     Concerned spawner
 * @return      orxFLOAT
 */
orxFLOAT orxFASTCALL orxSpawner_GetWorldRotation(orxCONST orxSPAWNER *_pstSpawner)
{
  orxFLOAT fResult;

  /* Checks */
  orxASSERT(sstSpawner.u32Flags & orxSPAWNER_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSpawner);

  /* Gets spawner rotation */
  fResult = orxFrame_GetRotation(_pstSpawner->pstFrame, orxFRAME_SPACE_GLOBAL);

  /* Done! */
  return fResult;
}

/** Gets spawner scale
 * @param[in]   _pstSpawner     Concerned spawner
 * @param[out]  _pvScale        Spawner scale vector
 * @return      Scale vector
 */
orxVECTOR *orxFASTCALL orxSpawner_GetScale(orxCONST orxSPAWNER *_pstSpawner, orxVECTOR *_pvScale)
{
  orxVECTOR *pvResult;

  /* Checks */
  orxASSERT(sstSpawner.u32Flags & orxSPAWNER_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSpawner);
  orxASSERT(_pvScale != orxNULL);

  /* Gets spawner scale */
  pvResult = orxFrame_GetScale(_pstSpawner->pstFrame, orxFRAME_SPACE_LOCAL, _pvScale);

  /* Done! */
  return pvResult;
}

/** Gets spawner world scale
 * @param[in]   _pstSpawner     Concerned spawner
 * @param[out]  _pvScale        Spawner world scale
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxVECTOR *orxFASTCALL orxSpawner_GetWorldScale(orxCONST orxSPAWNER *_pstSpawner, orxVECTOR *_pvScale)
{
  orxVECTOR *pvResult;

  /* Checks */
  orxASSERT(sstSpawner.u32Flags & orxSPAWNER_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSpawner);
  orxASSERT(_pvScale != orxNULL);

  /* Gets spawner scale */
  pvResult = orxFrame_GetScale(_pstSpawner->pstFrame, orxFRAME_SPACE_GLOBAL, _pvScale);

  /* Done! */
  return pvResult;
}

/** Sets an spawner parent
 * @param[in]   _pstSpawner     Concerned spawner
 * @param[in]   _pParent        Parent structure to set (spawner, spawner, camera or frame) / orxNULL
 * @return orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxSpawner_SetParent(orxSPAWNER *_pstSpawner, orxVOID *_pParent)
{
  orxFRAME   *pstFrame;
  orxSTATUS   eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstSpawner.u32Flags & orxSPAWNER_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSpawner);
  orxASSERT((_pParent == orxNULL) || (((orxSTRUCTURE *)(_pParent))->eID ^ orxSTRUCTURE_MAGIC_TAG_ACTIVE) < orxSTRUCTURE_ID_NUMBER);

  /* Gets frame */
  pstFrame = _pstSpawner->pstFrame;

  /* No parent? */
  if(_pParent == orxNULL)
  {
    /* Removes parent */
    orxFrame_SetParent(pstFrame, orxNULL);
  }
  else
  {
    /* Depending on parent ID */
    switch(orxStructure_GetID(_pParent))
    {
      case orxSTRUCTURE_ID_CAMERA:
      {
        /* Updates its parent */
        orxFrame_SetParent(pstFrame, orxCamera_GetFrame(orxCAMERA(_pParent)));

        break;
      }

      case orxSTRUCTURE_ID_FRAME:
      {
        /* Updates its parent */
        orxFrame_SetParent(pstFrame, orxFRAME(_pParent));

        break;
      }

      case orxSTRUCTURE_ID_OBJECT:
      {
        /* Updates its parent */
        orxFrame_SetParent(pstFrame, orxOBJECT_GET_STRUCTURE(orxOBJECT(_pParent), FRAME));

        break;
      }

      case orxSTRUCTURE_ID_SPAWNER:
      {
        /* Updates its parent */
        orxFrame_SetParent(pstFrame, orxSPAWNER(_pParent)->pstFrame);

        break;
      }

      default:
      {
        /* !!! MSG !!! */

        /* Updates result */
        eResult = orxSTATUS_FAILURE;

        break;
      }
    }
  }

  /* Done! */
  return eResult;
}

/** Gets spawner name
 * @param[in]   _pstSpawner     Concerned spawner
 * @return      orxSTRING / orxSTRING_EMPTY
 */
orxSTRING orxFASTCALL orxSpawner_GetName(orxCONST orxSPAWNER *_pstSpawner)
{
  orxSTRING zResult;

  /* Checks */
  orxASSERT(sstSpawner.u32Flags & orxSPAWNER_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSpawner);

  /* Has reference? */
  if(_pstSpawner->zReference != orxNULL)
  {
    /* Updates result */
    zResult = _pstSpawner->zReference;
  }
  else
  {
    /* Updates result */
    zResult = orxSTRING_EMPTY;
  }

  /* Done! */
  return zResult;
}
