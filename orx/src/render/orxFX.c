/**
 * @file orxFX.c
 */

/***************************************************************************
 orxFX.c
 FX module
 begin                : 30/06/2008
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


#include "render/orxFX.h"

#include "debug/orxDebug.h"
#include "memory/orxMemory.h"
#include "core/orxConfig.h"
#include "core/orxClock.h"
#include "object/orxStructure.h"
#include "utils/orxHashTable.h"
#include "utils/orxString.h"


/** Module flags
 */
#define orxFX_KU32_STATIC_FLAG_NONE             0x00000000

#define orxFX_KU32_STATIC_FLAG_READY            0x00000001

#define orxFX_KU32_STATIC_MASK_ALL              0xFFFFFFFF


/** Flags
 */
#define orxFX_KU32_FLAG_NONE                    0x00000000  /**< No flags */

#define orxFX_KU32_FLAG_ENABLED                 0x10000000  /**< Enabled flag */

#define orxFX_KU32_MASK_ALL                     0xFFFFFFFF  /**< All mask */


/** Misc defines
 */
#define orxFX_KU32_REFERENCE_TABLE_SIZE         32

#define orxFX_KU32_SLOT_NUMBER                  8


/***************************************************************************
 * Structure declaration                                                   *
 ***************************************************************************/

/** FX type enum
 */
typedef enum __orxFX_TYPE_t
{
	orxFX_TYPE_ALPHA_FADE = 0,
	orxFX_TYPE_COLOR_BLEND,
	orxFX_TYPE_ROTATION,
	orxFX_TYPE_SCALE,
	orxFX_TYPE_TRANSLATION,

	orxFX_TYPE_NUMBER,

	orxFX_TYPE_MAX_NUMBER = 16,

	orxFX_TYPE_NONE = orxENUM_NONE

} orxFX_TYPE;

/** FX parameters
 */
typedef struct __orxFX_PARAM_t
{
	orxFX_TYPE  eFXType;                          /**< Type : 4 */
  orxFLOAT    fStartTime;                       /**< Start Time : 8 */
  orxFLOAT    fEndTime;                         /**< End Time : 12 */
  orxFLOAT    fCyclePeriod;                     /**< Cycle period : 16 */
  orxFLOAT    fCyclePhasis;                     /**< Cycle phasis : 20 */

  union
  {
    struct
    {
      orxU8 u8StartAlpha;                       /**< Alpha start value : 21 */
      orxU8 u8EndAlpha;                         /**< Alpha end value : 22 */
    };                                          /**< Alpha Fade  : 22 */

    struct
    {
      orxRGBA stStartColor;                     /**< ColorBlend start value : 24 */
      orxRGBA stEndColor;                       /**< ColorBlend end value : 28 */
    };                                          /** Color blend : 28 */

    struct
    {
      orxFLOAT fStartRotation;                  /**< Rotation start value : 24 */
      orxFLOAT fEndRotation;                    /**< Rotation end value : 28 */
    };                                          /**< Scale : 28 */

    struct
    {
      orxVECTOR vStartScale;                    /**< Scale start value : 32 */
      orxVECTOR vEndScale;                      /**< Scale end value : 44 */
    };                                          /**< Scale : 44 */

    struct
    {
      orxVECTOR vStartPosition;                 /**< Translation vector : 32 */
      orxVECTOR vEndPosition;                   /**< Translation end position : 44 */
    };                                          /**< Translation : 44 */
  };

  orxU32 u32Flags;                              /**< Flags : 48 */

} orxFX_PARAM;

/** FX structure
 */
struct __orxFX_t
{
  orxSTRUCTURE  stStructure;                            /**< Public structure, first structure member : 16 */
  orxFX_PARAM   astFXSlotList[orxFX_KU32_SLOT_NUMBER];  /**< FX slot list : 64 */
  orxU32        u32ID;                                  /**< FX ID : 68 */

  /* Padding */
  orxPAD(68)
};

/** Static structure
 */
typedef struct __orxFX_STATIC_t
{
  orxHASHTABLE *pstReferenceTable;              /**< Reference hash table */
  orxU32        u32Flags;                       /**< Control flags */

} orxFX_STATIC;


/***************************************************************************
 * Static variables                                                        *
 ***************************************************************************/

/** Static data
 */
orxSTATIC orxFX_STATIC sstFX;


/***************************************************************************
 * Private functions                                                       *
 ***************************************************************************/

/** Deletes all the FXs
 */
orxSTATIC orxINLINE orxVOID orxFX_DeleteAll()
{
  orxFX *pstFX;

  /* Gets first FX */
  pstFX = orxFX(orxStructure_GetFirst(orxSTRUCTURE_ID_FX));

  /* Non empty? */
  while(pstFX != orxNULL)
  {
    /* Deletes it */
    orxFX_Delete(pstFX);

    /* Gets first FX */
    pstFX = orxFX(orxStructure_GetFirst(orxSTRUCTURE_ID_FX));
  }

  return;
}


/***************************************************************************
 * Public functions                                                        *
 ***************************************************************************/

/** FX module setup
 */
orxVOID orxFX_Setup()
{
  /* Adds module dependencies */
  orxModule_AddDependency(orxMODULE_ID_FX, orxMODULE_ID_MEMORY);
  orxModule_AddDependency(orxMODULE_ID_FX, orxMODULE_ID_STRUCTURE);
  orxModule_AddDependency(orxMODULE_ID_FX, orxMODULE_ID_CONFIG);
  orxModule_AddDependency(orxMODULE_ID_FX, orxMODULE_ID_HASHTABLE);

  return;
}

/** Inits the FX module
 * @return orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFX_Init()
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Checks */
  orxASSERT(orxFX_TYPE_NUMBER <= orxFX_TYPE_MAX_NUMBER);

  /* Not already Initialized? */
  if(!(sstFX.u32Flags & orxFX_KU32_STATIC_FLAG_READY))
  {
    /* Cleans static controller */
    orxMemory_Zero(&sstFX, sizeof(orxFX_STATIC));

    /* Creates reference table */
    sstFX.pstReferenceTable = orxHashTable_Create(orxFX_KU32_REFERENCE_TABLE_SIZE, orxHASHTABLE_KU32_FLAG_NONE, orxMEMORY_TYPE_MAIN);

    /* Valid? */
    if(sstFX.pstReferenceTable != orxNULL)
    {
      /* Registers structure type */
      eResult = orxSTRUCTURE_REGISTER(FX, orxSTRUCTURE_STORAGE_TYPE_LINKLIST, orxMEMORY_TYPE_MAIN, orxNULL);
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

  /* Initialized? */
  if(eResult == orxSTATUS_SUCCESS)
  {
    /* Inits Flags */
    sstFX.u32Flags = orxFX_KU32_STATIC_FLAG_READY;
  }

  /* Done! */
  return eResult;
}

/** Exits from the FX module
 */
orxVOID orxFX_Exit()
{
  /* Initialized? */
  if(sstFX.u32Flags & orxFX_KU32_STATIC_FLAG_READY)
  {
    /* Deletes FX list */
    orxFX_DeleteAll();

    /* Unregisters structure type */
    orxStructure_Unregister(orxSTRUCTURE_ID_FX);

    /* Deletes reference table */
    orxHashTable_Delete(sstFX.pstReferenceTable);

    /* Updates flags */
    sstFX.u32Flags &= ~orxFX_KU32_STATIC_FLAG_READY;
  }
  else
  {
    /* !!! MSG !!! */
  }

  return;
}

/** Creates an empty FX
 * @return      Created orxFX / orxNULL
 */
orxFX *orxFX_Create()
{
  orxFX *pstResult;

  /* Checks */
  orxASSERT(sstFX.u32Flags & orxFX_KU32_STATIC_FLAG_READY);

  /* Creates FX */
  pstResult = orxFX(orxStructure_Create(orxSTRUCTURE_ID_FX));

  /* Created? */
  if(pstResult != orxNULL)
  {
    /* Inits flags */
    orxStructure_SetFlags(pstResult, orxFX_KU32_FLAG_ENABLED, orxFX_KU32_MASK_ALL);
  }
  else
  {
    /* !!! MSG !!! */
  }

  /* Done! */
  return pstResult;
}

/** Deletes an FX
 * @param[in] _pstFX            Concerned FX
 * @return orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxFX_Delete(orxFX *_pstFX)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstFX.u32Flags & orxFX_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstFX);

  /* Not referenced? */
  if(orxStructure_GetRefCounter(_pstFX) == 0)
  {
    /* Deletes structure */
    orxStructure_Delete(_pstFX);
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

/** Creates an FX from config
 * @param[in]   _zConfigID            Config ID
 * @ return orxFX / orxNULL
 */
orxFX *orxFASTCALL orxFX_CreateFromConfig(orxCONST orxSTRING _zConfigID)
{
  orxU32  u32ID;
  orxFX  *pstResult;

  /* Checks */
  orxASSERT(sstFX.u32Flags & orxFX_KU32_STATIC_FLAG_READY);
  orxASSERT((_zConfigID != orxNULL) && (*_zConfigID != *orxSTRING_EMPTY));

  /* Gets FX ID */
  u32ID = orxString_ToCRC(_zConfigID);

  /* Search for reference */
  pstResult = orxHashTable_Get(sstFX.pstReferenceTable, u32ID);

  /* Not already created? */
  if(pstResult == orxNULL)
  {
    orxSTRING zPreviousSection;

    /* Gets previous config section */
    zPreviousSection = orxConfig_GetCurrentSection();

    /* Selects section */
    if((orxConfig_HasSection(_zConfigID) != orxFALSE)
    && (orxConfig_SelectSection(_zConfigID) != orxSTATUS_FAILURE))
    {
      /* Creates FX */
      pstResult = orxFX_Create();

      /* Valid? */
      if(pstResult != orxNULL)
      {
        /* Stores its ID */
        pstResult->u32ID = orxString_ToCRC(_zConfigID);

        //! TODO
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
  }
  else
  {
    /* Updates reference counter */
    orxStructure_IncreaseCounter(pstResult);
  }

  /* Done! */
  return pstResult;
}

/** Enables/disables an FX
 * @param[in]   _pstFX        Concerned FX
 * @param[in]   _bEnable      enable / disable
 */
orxVOID orxFASTCALL orxFX_Enable(orxFX *_pstFX, orxBOOL _bEnable)
{
  /* Checks */
  orxASSERT(sstFX.u32Flags & orxFX_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstFX);

  /* Enable? */
  if(_bEnable != orxFALSE)
  {
    /* Updates status flags */
    orxStructure_SetFlags(_pstFX, orxFX_KU32_FLAG_ENABLED, orxFX_KU32_FLAG_NONE);
  }
  else
  {
    /* Updates status flags */
    orxStructure_SetFlags(_pstFX, orxFX_KU32_FLAG_NONE, orxFX_KU32_FLAG_ENABLED);
  }

  return;
}

/** Is FX enabled?
 * @param[in]   _pstFX        Concerned FX
 * @return      orxTRUE if enabled, orxFALSE otherwise
 */
orxBOOL orxFASTCALL orxFX_IsEnabled(orxCONST orxFX *_pstFX)
{
  /* Checks */
  orxASSERT(sstFX.u32Flags & orxFX_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstFX);

  /* Done! */
  return(orxStructure_TestFlags((orxFX *)_pstFX, orxFX_KU32_FLAG_ENABLED));
}

/** Adds alpha fade to an FX
 * @param[in]   _pstFX          Concerned FX
 * @param[in]   _fStartTime     Time start
 * @param[in]   _fEndTime       Time end
 * @param[in]   _fCyclePeriod   Cycle period
 * @param[in]   _fCyclePhasis   Cycle phasis (at start)
 * @param[in]   _u8StartAlpha   Starting alpha value
 * @param[in]   _u8EndAlpha     Ending alpha value
 * @param[in]   _u32Flags       Param flags
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxFX_AddAlphaFade(orxFX *_pstFX, orxFLOAT _fStartTime, orxFLOAT _fEndTime, orxFLOAT _fCyclePeriod, orxFLOAT _fCyclePhasis, orxU8 _u8StartAlpha, orxU8 _u8EndAlpha, orxU32 _u32Flags)
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Checks */
  orxASSERT(sstFX.u32Flags & orxFX_KU32_STATIC_FLAG_READY);
  orxASSERT((_u32Flags & orxFX_PARAM_KU32_MASK_USER_ALL) == _u32Flags);
  orxSTRUCTURE_ASSERT(_pstFX);

  //! TODO

  /* Done! */
  return eResult;
}

/** Adds color blend to an FX
 * @param[in]   _pstFX          Concerned FX
 * @param[in]   _fStartTime     Time start
 * @param[in]   _fEndTime       Time end
 * @param[in]   _fCyclePeriod   Cycle period
 * @param[in]   _fCyclePhasis   Cycle phasis (at start)
 * @param[in]   _stStartColor   Starting color value
 * @param[in]   _stEndColor     Ending color value
 * @param[in]   _u32Flags       Param flags
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxFX_AddColorBlend(orxFX *_pstFX, orxFLOAT _fStartTime, orxFLOAT _fEndTime, orxFLOAT _fCyclePeriod, orxFLOAT _fCyclePhasis, orxRGBA _stStartColor, orxRGBA _stEndColor, orxU32 _u32Flags)
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Checks */
  orxASSERT(sstFX.u32Flags & orxFX_KU32_STATIC_FLAG_READY);
  orxASSERT((_u32Flags & orxFX_PARAM_KU32_MASK_USER_ALL) == _u32Flags);
  orxSTRUCTURE_ASSERT(_pstFX);

  //! TODO

  /* Done! */
  return eResult;
}

/** Adds rotation to an FX
 * @param[in]   _pstFX          Concerned FX
 * @param[in]   _fStartTime     Time start
 * @param[in]   _fEndTime       Time end
 * @param[in]   _fCyclePeriod   Cycle period
 * @param[in]   _fCyclePhasis   Cycle phasis (at start)
 * @param[in]   _fStartRotation Starting rotation value
 * @param[in]   _fEndRotation   Ending rotation value
 * @param[in]   _u32Flags       Param flags
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxFX_AddRotation(orxFX *_pstFX, orxFLOAT _fStartTime, orxFLOAT _fEndTime, orxFLOAT _fCyclePeriod, orxFLOAT _fCyclePhasis, orxFLOAT _fStartRotation, orxFLOAT _fEndRotation, orxU32 _u32Flags)
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Checks */
  orxASSERT(sstFX.u32Flags & orxFX_KU32_STATIC_FLAG_READY);
  orxASSERT((_u32Flags & orxFX_PARAM_KU32_MASK_USER_ALL) == _u32Flags);
  orxSTRUCTURE_ASSERT(_pstFX);

  //! TODO

  /* Done! */
  return eResult;
}

/** Adds scale to an FX
 * @param[in]   _pstFX          Concerned FX
 * @param[in]   _fStartTime     Time start
 * @param[in]   _fEndTime       Time end
 * @param[in]   _fCyclePeriod   Cycle period
 * @param[in]   _fCyclePhasis   Cycle phasis (at start)
 * @param[in]   _pvStartScale   Starting scale value
 * @param[in]   _pvEndScale     Ending scale value
 * @param[in]   _u32Flags       Param flags
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxFX_AddScale(orxFX *_pstFX, orxFLOAT _fStartTime, orxFLOAT _fEndTime, orxFLOAT _fCyclePeriod, orxFLOAT _fCyclePhasis, orxCONST orxVECTOR *_pvStartScale, orxCONST orxVECTOR *_pvEndScale, orxU32 _u32Flags)
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Checks */
  orxASSERT(sstFX.u32Flags & orxFX_KU32_STATIC_FLAG_READY);
  orxASSERT((_u32Flags & orxFX_PARAM_KU32_MASK_USER_ALL) == _u32Flags);
  orxSTRUCTURE_ASSERT(_pstFX);

  //! TODO

  /* Done! */
  return eResult;
}

/** Adds translation to an FX
 * @param[in]   _pstFX          Concerned FX
 * @param[in]   _fStartTime     Time start
 * @param[in]   _fEndTime       Time end
 * @param[in]   _fCyclePeriod   Cycle period
 * @param[in]   _fCyclePhasis   Cycle phasis (at start)
 * @param[in]   _pvStartTranslation Starting translation value
 * @param[in]   _pvEndTranslation Ending translation value
 * @param[in]   _u32Flags       Param flags
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxFX_AddTranslation(orxFX *_pstFX, orxFLOAT _fStartTime, orxFLOAT _fEndTime, orxFLOAT _fCyclePeriod, orxFLOAT _fCyclePhasis, orxCONST orxVECTOR *_pvStartTranslation, orxCONST orxVECTOR *_pvEndTranslation, orxU32 _u32Flags)
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Checks */
  orxASSERT(sstFX.u32Flags & orxFX_KU32_STATIC_FLAG_READY);
  orxASSERT((_u32Flags & orxFX_PARAM_KU32_MASK_USER_ALL) == _u32Flags);
  orxSTRUCTURE_ASSERT(_pstFX);

  //! TODO

  /* Done! */
  return eResult;
}

/** Gets FX duration
 * @param[in]   _pstFX          Concerned FX
 * @return      orxFLOAT
 */
orxFLOAT orxFASTCALL orxFX_GetDuration(orxCONST orxFX *_pstFX)
{
  orxFLOAT fResult;

  /* Checks */
  orxASSERT(sstFX.u32Flags & orxFX_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstFX);

  //! TODO
  /* Updates result */
  fResult = orxFLOAT_0;

  /* Done! */
  return fResult;
}

/** Tests FX name against given one
 * @param[in]   _pstFX          Concerned FX
 * @param[in]   _zName          Name to test
 * @return      orxTRUE if it's FX name, orxFALSE otherwise
 */
orxBOOL orxFASTCALL orxFX_IsName(orxCONST orxFX *_pstFX, orxCONST orxSTRING _zName)
{
  orxBOOL bResult;

  /* Checks */
  orxASSERT(sstFX.u32Flags & orxFX_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstFX);

  /* Updates result */
  bResult = (orxString_ToCRC(_zName) == _pstFX->u32ID) ? orxTRUE : orxFALSE;

  /* Done! */
  return bResult;
}
