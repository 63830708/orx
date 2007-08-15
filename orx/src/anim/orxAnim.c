/**
 * @file orxAnim.c
 * 
 * Animation (Data) module
 * 
 */

 /***************************************************************************
 orxAnim.c
 Animation (Data) module
 
 begin                : 12/02/2004
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


#include "anim/orxAnim.h"

#include "debug/orxDebug.h"
#include "memory/orxMemory.h"


/** Module flags
 */
#define orxANIM_KU32_FLAG_NONE              0x00000000  /**< No flags */

#define orxANIM_KU32_FLAG_READY             0x00000001  /**< Ready flag */

#define orxANIM_KU32_MASK_ALL               0xFFFFFFFF  /**< All flags */


/** orxANIM ID flags/masks/shifts
 */

#define orxANIM_KU32_ID_MASK_SIZE           0x000000FF  /**< Size ID mask */
#define orxANIM_KU32_ID_MASK_COUNTER        0x0000FF00  /**< Counter ID mask */
#define orxANIM_KU32_ID_MASK_FLAGS          0xFFFF0000  /**< Flags ID mask */

#define orxANIM_KU32_ID_FLAG_CURRENT_KEY    0x10000000  /**< Has current key? */

#define orxANIM_KS32_ID_SHIFT_SIZE          0           /**< Size ID shift */
#define orxANIM_KS32_ID_SHIFT_COUNTER       8           /**< Counter ID shift */


/***************************************************************************
 * Structure declaration                                                   *
 ***************************************************************************/

/** Animation key structure
 */
typedef struct __orxANIM_KEY_t
{
  orxSTRUCTURE *pstData;                    /**< Data key : 4 */
  orxU32        u32TimeStamp;               /**< Data timestamp : 8 */

} orxANIM_KEY;

/** Animation structure
 */
struct __orxANIM_t
{
  orxSTRUCTURE  stStructure;                /**< Public structure, first structure member : 16 */
  orxU32        u32IDFlags;                 /**< ID flags : 20 */
  orxANIM_KEY  *astKeyList;                 /**< Key array : 24 */
  orxU16        u16CurrentKey;              /**< Current key : 26 */

  orxPAD(26)
};


/** Static structure
 */
typedef struct __orxANIM_STATIC_t
{
  orxU32 u32Flags;                          /**< Control flags : 4 */

} orxANIM_STATIC;


/***************************************************************************
 * Static variables                                                        *
 ***************************************************************************/

/** Static data
 */
orxSTATIC orxANIM_STATIC sstAnim;


/***************************************************************************
 * Private functions                                                       *
 ***************************************************************************/

/** Finds a key index given a timestamp
 * @param[in]   _pstAnim        Concerned animation
 * @param[in]   _u32TimeStamp   Desired timestamp
 * @return      Key index / orxU32_Undefined
 */
orxSTATIC orxU32 orxFASTCALL orxAnim_FindKeyIndex(orxCONST orxANIM *_pstAnim, orxU32 _u32TimeStamp)
{
  orxU32 u32Counter, u32MaxIndex, u32MinIndex, u32Index;

  /* Checks */
  orxASSERT(_pstAnim != orxNULL);

  /* Gets counter */
  u32Counter = orxAnim_GetKeyCounter(_pstAnim);

  /* Is animation not empty? */
  if(u32Counter != 0)
  {
    /* Dichotomic search */
    for(u32MinIndex = 0, u32MaxIndex = u32Counter - 1, u32Index = u32MaxIndex >> 1;
        u32MinIndex < u32MaxIndex;
        u32Index = (u32MinIndex + u32MaxIndex) >> 1)
    {
      /* Updates search range */
      if(_u32TimeStamp > _pstAnim->astKeyList[u32Index].u32TimeStamp)
      {
        u32MinIndex = u32Index + 1;
      }
      else
      {
        u32MaxIndex = u32Index;
      }
    }

    /* Not found? */
    if(_pstAnim->astKeyList[u32Index].u32TimeStamp < _u32TimeStamp)
    {
      /* !!! MSG !!! */

      /* Not defined */
      u32Index = orxU32_Undefined;
    }
  }
  /* Empty animation */
  else
  {
    /* !!! MSG !!! */

    /* Not defined */
    u32Index = orxU32_Undefined;
  }

  /* Done! */
  return u32Index;
}

/** Sets an animation storage size
 * @param[in]   _pstAnim        Concerned animation
 * @param[in]   _u32Size        Desired size
 */
orxSTATIC orxINLINE orxVOID orxAnim_SetStorageSize(orxANIM *_pstAnim, orxU32 _u32Size)
{
  /* Checks */
  orxASSERT(_pstAnim != orxNULL);
  orxASSERT(_u32Size <= orxANIM_KU32_KEY_MAX_NUMBER);

  /* Updates storage size */
  orxAnim_SetFlags(_pstAnim, _u32Size << orxANIM_KS32_ID_SHIFT_SIZE, orxANIM_KU32_ID_MASK_SIZE);

  return;
}  

/** Sets an animation internal key counter
 * @param[in]   _pstAnim        Concerned animation
 * @param[in]   _u32KeyCounter  Desired key counter
 */
orxSTATIC orxINLINE orxVOID orxAnim_SetKeyCounter(orxANIM *_pstAnim, orxU32 _u32KeyCounter)
{
  /* Checks */
  orxASSERT(_pstAnim != orxNULL);
  orxASSERT(_u32KeyCounter <= orxAnim_GetKeyStorageSize(_pstAnim));

  /* Updates counter */
  orxAnim_SetFlags(_pstAnim, _u32KeyCounter << orxANIM_KS32_ID_SHIFT_COUNTER, orxANIM_KU32_ID_MASK_COUNTER);

  return;
}

/** Increases an animation internal key counter
 * @param[in]   _pstAnim        Concerned animation
 */
orxSTATIC orxINLINE orxVOID orxAnim_IncreaseKeyCounter(orxANIM *_pstAnim)
{
  orxREGISTER orxU32 u32KeyCounter;

  /* Checks */
  orxASSERT(_pstAnim != orxNULL);

  /* Gets key counter */
  u32KeyCounter = orxAnim_GetKeyCounter(_pstAnim);

  /* Updates key counter */
  orxAnim_SetKeyCounter(_pstAnim, u32KeyCounter + 1);

  return;
}  

/** Increases an animation internal key counter
 * @param[in]   _pstAnim        Concerned animation
 */
orxSTATIC orxINLINE orxVOID orxAnim_DecreaseKeyCounter(orxANIM *_pstAnim)
{
  orxREGISTER orxU32 u32KeyCounter;

  /* Checks */
  orxASSERT(_pstAnim != orxNULL);

  /* Gets key counter */
  u32KeyCounter = orxAnim_GetKeyCounter(_pstAnim);

  /* Updates key counter*/
  orxAnim_SetKeyCounter(_pstAnim, u32KeyCounter - 1);

  return;
}  

/** Deletes all animations
 */
orxSTATIC orxINLINE orxVOID orxAnim_DeleteAll()
{
  orxREGISTER orxANIM *pstAnim;

  /* Gets first anim */
  pstAnim = (orxANIM *)orxStructure_GetFirst(orxSTRUCTURE_ID_ANIM);

  /* Non empty? */
  while(pstAnim != orxNULL)
  {
    /* Deletes Animation */
    orxAnim_Delete(pstAnim);

    /* Gets first Animation */
    pstAnim = (orxANIM *)orxStructure_GetFirst(orxSTRUCTURE_ID_ANIM);
  }

  return;
}


/***************************************************************************
 * Public functions                                                        *
 ***************************************************************************/

/** Animation module setup
 */
orxVOID orxAnim_Setup()
{
  /* Adds module dependencies */
  orxModule_AddDependency(orxMODULE_ID_ANIM, orxMODULE_ID_MEMORY);
  orxModule_AddDependency(orxMODULE_ID_ANIM, orxMODULE_ID_TIME);
  orxModule_AddDependency(orxMODULE_ID_ANIM, orxMODULE_ID_STRUCTURE);

  return;
}

/** Inits the Animation module
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxAnim_Init()
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Not already Initialized? */
  if(!(sstAnim.u32Flags & orxANIM_KU32_FLAG_READY))
  {
    /* Cleans static controller */
    orxMemory_Set(&sstAnim, 0, sizeof(orxANIM_STATIC));

    /* Registers structure type */
    eResult = orxSTRUCTURE_REGISTER(ANIM, orxSTRUCTURE_STORAGE_TYPE_LINKLIST, orxMEMORY_TYPE_MAIN, orxNULL);
  }
  else
  {
    /* !!! MSG !!! */

    /* Already initialized */
    eResult = orxSTATUS_SUCCESS;
  }

  /* Initialized? */
  if(eResult == orxSTATUS_SUCCESS)
  {
    /* Inits Flags */
    sstAnim.u32Flags = orxANIM_KU32_FLAG_READY;
  }
  else
  {
    /* !!! MSG !!! */
  }

  /* Done! */
  return eResult;
}

/** Exits from the Animation module
 */
orxVOID orxAnim_Exit()
{
  /* Initialized? */
  if(sstAnim.u32Flags & orxANIM_KU32_FLAG_READY)
  {
    /* Deletes anim list */
    orxAnim_DeleteAll();

    /* Unregisters structure type */
    orxStructure_Unregister(orxSTRUCTURE_ID_ANIM);

    /* Updates flags */
    sstAnim.u32Flags &= ~orxANIM_KU32_FLAG_READY;
  }
  else
  {
    /* !!! MSG !!! */
  }

  return;
}

/** Creates an empty animation
 * @param[in]   _u32IDFLags     ID flags for created animation
 * @param[in]   _u32Size        Number of keys for this animation
 * @return      Created orxANIM / orxNULL
 */
orxANIM *orxFASTCALL orxAnim_Create(orxU32 _u32IDFlags, orxU32 _u32Size)
{
  orxANIM *pstAnim;

  /* Checks */
  orxASSERT(sstAnim.u32Flags & orxANIM_KU32_FLAG_READY);
  orxASSERT((_u32IDFlags & orxANIM_KU32_ID_MASK_USER_ALL) == _u32IDFlags); 
  orxASSERT(_u32Size <= orxANIM_KU32_KEY_MAX_NUMBER);

  /* Creates anim */
  pstAnim = (orxANIM *)orxStructure_Create(orxSTRUCTURE_ID_ANIM);

  /* Valid? */
  if(pstAnim != orxNULL)
  {
    /* Inits flags */
    orxAnim_SetFlags(pstAnim, _u32IDFlags & orxANIM_KU32_ID_MASK_USER_ALL, orxANIM_KU32_ID_MASK_FLAGS);

    /* 2D Animation? */
    if(_u32IDFlags & orxANIM_KU32_ID_FLAG_2D)
    {
      /* Allocates key array */
      pstAnim->astKeyList = (orxANIM_KEY *)orxMemory_Allocate(_u32Size * sizeof(orxANIM_KEY), orxMEMORY_TYPE_MAIN);

      /* Valid? */
      if(pstAnim->astKeyList == orxNULL)
      {
        /* Cleans key array */
        orxMemory_Set(pstAnim->astKeyList, 0, _u32Size * sizeof(orxANIM_KEY));

        /* Sets storage size & counter */
        orxAnim_SetStorageSize(pstAnim, _u32Size);
        orxAnim_SetKeyCounter(pstAnim, 0);
      }
      else
      {
        /* !!! MSG !!! */

        /* Frees partially allocated anim */
        orxMemory_Free(pstAnim);

        /* Updates result */
        pstAnim = orxNULL;
      }
    }
    /* Other Animation Type? */
    else
    {
      /* !!! MSG !!! */

      /* Frees partially allocated anim */
      orxMemory_Free(pstAnim);

      /* Updates result */
      pstAnim = orxNULL;
    }
  }

  /* Done! */
  return pstAnim;
}

/** Deletes an animation
 * @param[in]   _pstAnim        Animation to delete
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxAnim_Delete(orxANIM *_pstAnim)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstAnim.u32Flags & orxANIM_KU32_FLAG_READY);
  orxASSERT(_pstAnim != orxNULL);

  /* Not referenced? */
  if(orxStructure_GetRefCounter((orxSTRUCTURE *)_pstAnim) == 0)
  {
    /* Cleans members */

    /* 2D Animation? */
    if(orxAnim_TestFlags(_pstAnim, orxANIM_KU32_ID_FLAG_2D))
    {
      /* Removes all keys */
      orxAnim_RemoveAllKeys(_pstAnim);
    }
    /* Other Animation Type? */
    else
    {
      /* !!! MSG !!! */
    }

    /* Deletes structure */
    orxStructure_Delete((orxSTRUCTURE *)_pstAnim);
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

/** Adds a key to an animation
 * @param[in]   _pstAnim        Animation concerned
 * @param[in]   _pstData        Key data to add
 * @param[in]   _u32TimeStamp   Timestamp for this key
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxAnim_AddKey(orxANIM *_pstAnim, orxSTRUCTURE *_pstData, orxU32 _u32TimeStamp)
{
  orxU32    u32Counter, u32Size;
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstAnim.u32Flags & orxANIM_KU32_FLAG_READY);
  orxASSERT(_pstAnim != orxNULL);
  orxASSERT(_pstData != orxNULL);
  orxASSERT(orxAnim_TestFlags(_pstAnim, orxANIM_KU32_ID_FLAG_2D) != orxFALSE);
  orxASSERT((orxAnim_GetKeyCounter(_pstAnim) == 0) || (_u32TimeStamp > _pstAnim->astKeyList[orxAnim_GetKeyCounter(_pstAnim) - 1].u32TimeStamp)); 

  /* Gets storage size & counter */
  u32Size     = orxAnim_GetKeyStorageSize(_pstAnim);
  u32Counter  = orxAnim_GetKeyCounter(_pstAnim);

  /* Is there free room? */
  if(u32Counter < u32Size)
  {
    orxANIM_KEY *pstKey;

    /* Gets key pointer */
    pstKey                = &(_pstAnim->astKeyList[u32Counter]);

    /* Stores key info */
    pstKey->pstData       = _pstData;
    pstKey->u32TimeStamp  = _u32TimeStamp;

    /* Updates structure reference counter */
    orxStructure_IncreaseCounter((orxSTRUCTURE *)_pstData);

    /* Updates key counter */
    orxAnim_IncreaseKeyCounter(_pstAnim);

    /* Updates result */
    eResult = orxSTATUS_SUCCESS;
  }
  else
  {
    /* !!! MSG !!! */

    /* Updates status */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Removes last added key from an animation
 * @param[in]   _pstAnim        Concerned animation
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxAnim_RemoveLastKey(orxANIM *_pstAnim)
{
  orxU32    u32Counter;
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstAnim.u32Flags & orxANIM_KU32_FLAG_READY);
  orxASSERT(_pstAnim != orxNULL);
  orxASSERT(orxAnim_TestFlags(_pstAnim, orxANIM_KU32_ID_FLAG_2D) != orxFALSE);

  /* Gets counter */
  u32Counter = orxAnim_GetKeyCounter(_pstAnim);

  /* Has key? */
  if(u32Counter != 0)
  {
    orxANIM_KEY *pstKey;

    /* Gets real index */
    u32Counter--;

    /* Gets key pointer */
    pstKey = &(_pstAnim->astKeyList[u32Counter]);

    /* Updates key counter */
    orxAnim_DecreaseKeyCounter(_pstAnim);

    /* Updates structure reference counter */
    orxStructure_DecreaseCounter(pstKey->pstData);

    /* Cleans the key info */
    orxMemory_Set(pstKey, 0, sizeof(orxANIM_KEY));
    
    /* Had a current key? */
    if(_pstAnim->u32IDFlags & orxANIM_KU32_ID_FLAG_CURRENT_KEY)
    {
      /* Was the removed one? */
      if(_pstAnim->u16CurrentKey == u32Counter)
      {
        /* Removes current key */
        _pstAnim->u16CurrentKey = 0;

        /* Updates flags */
        _pstAnim->u32IDFlags   &= ~orxANIM_KU32_ID_FLAG_CURRENT_KEY;
      }
    }

    /* Updates result */
    eResult = orxSTATUS_SUCCESS;
  }
  else
  {
    /* !!! MSG !!! */

    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Removes all keys from an animation
 * @param[in]   _pstAnim        Concerned animation
 */
orxVOID orxFASTCALL orxAnim_RemoveAllKeys(orxANIM *_pstAnim)
{
  /* Checks */
  orxASSERT(sstAnim.u32Flags & orxANIM_KU32_FLAG_READY);
  orxASSERT(_pstAnim != orxNULL);
  orxASSERT(orxAnim_TestFlags(_pstAnim, orxANIM_KU32_ID_FLAG_2D) != orxFALSE);

  /* Until there are no key left */
  while(orxAnim_RemoveLastKey(_pstAnim) != orxSTATUS_FAILURE);

  /* Done! */
  return;
}

/** Updates animation given a timestamp
 * @param[in]   _pstAnim        Concerned animation
 * @param[in]   _u32TimeStamp   TimeStamp for animation update
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxAnim_Update(orxANIM *_pstAnim, orxU32 _u32TimeStamp)
{
  orxU32    u32Index;
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstAnim.u32Flags & orxANIM_KU32_FLAG_READY);
  orxASSERT(_pstAnim != orxNULL);
  orxASSERT(orxAnim_TestFlags(_pstAnim, orxANIM_KU32_ID_FLAG_2D) != orxFALSE);

  /* Finds corresponding key index */
  u32Index = orxAnim_FindKeyIndex(_pstAnim, _u32TimeStamp);

  /* Found? */
  if(u32Index != orxU32_Undefined)
  {
    /* Updates current key */
    _pstAnim->u16CurrentKey = u32Index;

    /* Updates result */
    eResult = orxSTATUS_SUCCESS;
  }
  else
  {
    /* !!! MSG !!! */

    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Anim current key data accessor
 * @param[in]   _pstAnim        Concerned animation
 * @return      Desired orxSTRUCTURE / orxNULL
 */
orxSTRUCTURE *orxFASTCALL orxAnim_GetCurrentKeyData(orxCONST orxANIM *_pstAnim)
{
  orxSTRUCTURE *pstResult;

  /* Has current key? */
  if(_pstAnim->u32IDFlags & orxANIM_KU32_ID_FLAG_CURRENT_KEY)
  {
    /* Updates result */
    pstResult = _pstAnim->astKeyList[_pstAnim->u16CurrentKey].pstData;
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

/** Animation key data accessor
 * @param[in]   _pstAnim        Concerned animation
 * @param[in]   _u32Index       Index of desired key
 * @return      Desired orxSTRUCTURE / orxNULL
 */
orxSTRUCTURE *orxFASTCALL orxAnim_GetKeyData(orxCONST orxANIM *_pstAnim, orxU32 _u32Index)
{
  orxU32        u32Counter;
  orxSTRUCTURE *pstResult;

  /* Checks */
  orxASSERT(sstAnim.u32Flags & orxANIM_KU32_FLAG_READY);
  orxASSERT(_pstAnim != orxNULL);
  orxASSERT(orxAnim_TestFlags(_pstAnim, orxANIM_KU32_ID_FLAG_2D) != orxFALSE);

  /* Gets counter */
  u32Counter = orxAnim_GetKeyCounter(_pstAnim);

  /* Is index valid? */
  if(_u32Index < u32Counter)
  {
    /* Updates result */
    pstResult = _pstAnim->astKeyList[_u32Index].pstData;
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

/** Animation key storage size accessor
 * @param[in]   _pstAnim        Concerned animation
 * @return      Animation key storage size
 */
orxU32 orxFASTCALL orxAnim_GetKeyStorageSize(orxCONST orxANIM *_pstAnim)
{
  /* Checks */
  orxASSERT(sstAnim.u32Flags & orxANIM_KU32_FLAG_READY);
  orxASSERT(_pstAnim != orxNULL);
  orxASSERT(orxAnim_TestFlags(_pstAnim, orxANIM_KU32_ID_FLAG_2D) != orxFALSE);

  /* Gets storage size */
  return((_pstAnim->u32IDFlags & orxANIM_KU32_ID_MASK_SIZE) >> orxANIM_KS32_ID_SHIFT_SIZE);
}  

/** Animation key counter accessor
 * @param[in]   _pstAnim        Concerned animation
 * @return      Animation key counter
 */
orxU32 orxFASTCALL orxAnim_GetKeyCounter(orxCONST orxANIM *_pstAnim)
{
  /* Checks */
  orxASSERT(sstAnim.u32Flags & orxANIM_KU32_FLAG_READY);
  orxASSERT(_pstAnim != orxNULL);
  orxASSERT(orxAnim_TestFlags(_pstAnim, orxANIM_KU32_ID_FLAG_2D) != orxFALSE);

  /* Gets counter */
  return((_pstAnim->u32IDFlags & orxANIM_KU32_ID_MASK_COUNTER) >> orxANIM_KS32_ID_SHIFT_COUNTER);
}

/** Animation time length accessor
 * @param[in]   _pstAnim        Concerned animation
 * @return      Animation time length
 */
orxU32 orxFASTCALL orxAnim_GetLength(orxCONST orxANIM *_pstAnim)
{
  orxU32 u32Counter, u32Length = 0;

  /* Checks */
  orxASSERT(sstAnim.u32Flags & orxANIM_KU32_FLAG_READY);
  orxASSERT(_pstAnim != orxNULL);

  /* 2D? */
  if(orxAnim_TestFlags(_pstAnim, orxANIM_KU32_ID_FLAG_2D) != orxFALSE)
  {
    /* Gets key counter */
    u32Counter = orxAnim_GetKeyCounter(_pstAnim);

    /* Is animation non empty? */
    if(u32Counter != 0)
    {
      /* Gets length */
      u32Length = _pstAnim->astKeyList[u32Counter - 1].u32TimeStamp;
    }
  }
  else
  {
    /* !!! MSG !!! */

    /* Updates result */
    u32Length = orxU32_Undefined;
  }

  /* Done! */
  return u32Length;
}

/** Animation flags test accessor
 * @param[in]   _pstAnim        Concerned animation
 * @param[in]   _u32Flags       Flags to test
 * @return      orxTRUE / orxFALSE
 */
orxBOOL orxFASTCALL orxAnim_TestFlags(orxCONST orxANIM *_pstAnim, orxU32 _u32Flags)
{
  /* Checks */
  orxASSERT(sstAnim.u32Flags & orxANIM_KU32_FLAG_READY);
  orxASSERT(_pstAnim != orxNULL);

  /* Done! */
  return((_pstAnim->u32IDFlags & _u32Flags) != orxANIM_KU32_FLAG_NONE);
}

/** Animation all flags test accessor
 * @param[in]   _pstAnim        Concerned animation
 * @param[in]   _u32Flags       Flags to test
 * @return      orxTRUE / orxFALSE
 */
orxBOOL orxFASTCALL orxAnim_TestAllFlags(orxCONST orxANIM *_pstAnim, orxU32 _u32Flags)
{
  /* Checks */
  orxASSERT(sstAnim.u32Flags & orxANIM_KU32_FLAG_READY);
  orxASSERT(_pstAnim != orxNULL);

  /* Done! */
  return((_pstAnim->u32IDFlags & _u32Flags) == _u32Flags);
}

/** Animation flag set accessor
 * @param[in]   _pstAnim        Concerned animation
 * @param[in]   _u32AddFlags    Flags to add
 * @param[in]   _u32RemoveFlags Flags to remove
 */
orxVOID orxFASTCALL orxAnim_SetFlags(orxANIM *_pstAnim, orxU32 _u32AddFlags, orxU32 _u32RemoveFlags)
{
  /* Checks */
  orxASSERT(sstAnim.u32Flags & orxANIM_KU32_FLAG_READY);
  orxASSERT(_pstAnim != orxNULL);

  /* Updates flags */
  _pstAnim->u32IDFlags &= ~_u32RemoveFlags;
  _pstAnim->u32IDFlags |= _u32AddFlags;

  return;
}
