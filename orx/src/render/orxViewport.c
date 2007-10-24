/**
 * @file orxViewport.c
 * 
 * Viewport module
 * 
 */

 /***************************************************************************
 orxViewport.c
 Viewport module
 
 begin                : 14/12/2003
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


#include "render/orxViewport.h"

#include "debug/orxDebug.h"
#include "math/orxMath.h"
#include "memory/orxMemory.h"
#include "object/orxStructure.h"


/** Module flags
 */
#define orxVIEWPORT_KU32_STATIC_FLAG_NONE     0x00000000 /**< No flags */

#define orxVIEWPORT_KU32_STATIC_FLAG_READY    0x00000001 /**< Ready flag */

#define orxVIEWPORT_KU32_STATIC_MASK_ALL      0xFFFFFFFF /**< All mask */

/** orxVIEWPORT flags / masks
 */
#define orxVIEWPORT_KU32_FLAG_NONE            0x00000000 /**< No flags */

#define orxVIEWPORT_KU32_FLAG_ENABLED         0x00000001 /**< Enabled flag */
#define orxVIEWPORT_KU32_FLAG_CAMERA          0x00000002 /**< Has camera flag */
#define orxVIEWPORT_KU32_FLAG_TEXTURE         0x00000004 /**< Has texture flag */

#define orxVIEWPORT_KU32_MASK_ALIGN           0xF0000000  /**< Alignment mask */

#define orxVIEWPORT_KU32_MASK_ALL             0xFFFFFFFF  /** All mask */


/***************************************************************************
 * Structure declaration                                                   *
 ***************************************************************************/

/** Viewport structure
 */
struct __orxVIEWPORT_t
{
  orxSTRUCTURE  stStructure;                  /**< Public structure, first structure member : 16 */
  orxU32        u32X;                         /**< X position (top left corner) : 20 */
  orxU32        u32Y;                         /**< Y position (top left corner) : 24 */
  orxU32        u32Width;                     /**< Width : 28 */
  orxU32        u32Height;                    /**< Height : 32 */
  orxCAMERA    *pstCamera;                    /**< Associated camera : 36 */
  orxTEXTURE   *pstTexture;                   /**< Associated texture : 40 */

  orxPAD(40)
};


/** Static structure
 */
typedef struct __orxVIEWPORT_STATIC_t
{
  orxU32 u32Flags;                            /**< Control flags : 4 */

} orxVIEWPORT_STATIC;


/***************************************************************************
 * Static variables                                                        *
 ***************************************************************************/

/** Static data
 */
orxSTATIC orxVIEWPORT_STATIC sstViewport;


/***************************************************************************
 * Private functions                                                       *
 ***************************************************************************/

/** Deletes all viewports
 */
orxSTATIC orxINLINE orxVOID orxViewport_DeleteAll()
{
  orxVIEWPORT *pstViewport;

  /* Gets first viewport */
  pstViewport = (orxVIEWPORT *)orxStructure_GetFirst(orxSTRUCTURE_ID_VIEWPORT);

  /* Non empty? */
  while(pstViewport != orxNULL)
  {
    /* Deletes viewport */
    orxViewport_Delete(pstViewport);

    /* Gets first remaining viewport */
    pstViewport = (orxVIEWPORT *)orxStructure_GetFirst(orxSTRUCTURE_ID_VIEWPORT);
  }

  return;
}


/***************************************************************************
 * Public functions                                                        *
 ***************************************************************************/

/** Viewport module setup
 */
orxVOID orxViewport_Setup()
{
  /* Adds module dependencies */
  orxModule_AddDependency(orxMODULE_ID_VIEWPORT, orxMODULE_ID_MEMORY);
  orxModule_AddDependency(orxMODULE_ID_VIEWPORT, orxMODULE_ID_STRUCTURE);
  orxModule_AddDependency(orxMODULE_ID_VIEWPORT, orxMODULE_ID_TEXTURE);
  orxModule_AddDependency(orxMODULE_ID_VIEWPORT, orxMODULE_ID_CAMERA);

  return;
}

/** Inits the Viewport module
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxViewport_Init()
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Not already Initialized? */
  if(!(sstViewport.u32Flags & orxVIEWPORT_KU32_STATIC_FLAG_READY))
  {
    /* Cleans static controller */
    orxMemory_Set(&sstViewport, 0, sizeof(orxVIEWPORT_STATIC));

    /* Registers structure type */
    eResult = orxSTRUCTURE_REGISTER(VIEWPORT, orxSTRUCTURE_STORAGE_TYPE_LINKLIST, orxMEMORY_TYPE_MAIN, orxNULL);
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
    sstViewport.u32Flags = orxVIEWPORT_KU32_STATIC_FLAG_READY;
  }
  else
  {
    /* !!! MSG !!! */
  }

  /* Done! */
  return eResult;
}

/** Exits from the Viewport module
 */
orxVOID orxViewport_Exit()
{
  /* Initialized? */
  if(sstViewport.u32Flags & orxVIEWPORT_KU32_STATIC_FLAG_READY)
  {
    /* Deletes viewport list */
    orxViewport_DeleteAll();

    /* Unregisters structure type */
    orxStructure_Unregister(orxSTRUCTURE_ID_VIEWPORT);

    /* Updates flags */
    sstViewport.u32Flags &= ~orxVIEWPORT_KU32_STATIC_FLAG_READY;
  }
  else
  {
    /* !!! MSG !!! */
  }

  return;
}

/** Creates a viewport
 * @return      Created orxVIEWPORT / orxNULL
 */
orxVIEWPORT *orxViewport_Create()
{
  orxVIEWPORT *pstViewport = orxNULL;

  /* Checks */
  orxASSERT(sstViewport.u32Flags & orxVIEWPORT_KU32_STATIC_FLAG_READY);

  /* Creates viewport */
  pstViewport = (orxVIEWPORT *)orxStructure_Create(orxSTRUCTURE_ID_VIEWPORT);

  /* Valid? */
  if(pstViewport != orxNULL)
  {
    /* Inits viewport flags */
    orxStructure_SetFlags(pstViewport, orxVIEWPORT_KU32_FLAG_ENABLED, orxVIEWPORT_KU32_FLAG_NONE);

    /* Inits vars */
    pstViewport->u32X = pstViewport->u32Y = pstViewport->u32Width = pstViewport->u32Height = orxU32_UNDEFINED;
  }
  else
  {
    /* !!! MSG !!! */

    /* Not created */
    pstViewport = orxNULL;
  }

  /* Done! */
  return pstViewport;
}

/** Deletes a viewport
 * @param[in]   _pstViewport    Viewport to delete
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxViewport_Delete(orxVIEWPORT *_pstViewport)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstViewport.u32Flags & orxVIEWPORT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstViewport);

  /* Not referenced? */
  if(orxStructure_GetRefCounter(_pstViewport) == 0)
  {
    /* Was linked to a camera? */
    if(_pstViewport->pstCamera != orxNULL)
    {
      /* Removes its reference */
      orxStructure_DecreaseCounter((_pstViewport->pstCamera));
    }

    /* Was linked to a texture? */
    if(_pstViewport->pstTexture != orxNULL)
    {
      /* Removes its reference */
      orxStructure_DecreaseCounter((_pstViewport->pstTexture));
    }

    /* Deletes structure */
    orxStructure_Delete(_pstViewport);
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

/** Sets a viewport alignment
 * @param[in]   _pstViewport    Concerned viewport
 * @param[in]   _u32AlignFlags  Alignment flags (must be OR'ed)
 */
orxVOID orxFASTCALL orxViewport_SetAlignment(orxVIEWPORT *_pstViewport, orxU32 _u32AlignFlags)
{
  /* Checks */
  orxASSERT(sstViewport.u32Flags & orxVIEWPORT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstViewport);
  orxASSERT((_u32AlignFlags & orxVIEWPORT_KU32_MASK_ALIGN) == _u32AlignFlags)

  /* Updates alignement flags */
  orxStructure_SetFlags(_pstViewport, _u32AlignFlags & orxVIEWPORT_KU32_MASK_ALIGN, orxVIEWPORT_KU32_MASK_ALIGN);

  return;
}

/** Sets a viewport texture
 * @param[in]   _pstViewport    Concerned viewport
 * @param[in]   _pstTexture     Texture to associate with the viewport
 */
orxVOID orxFASTCALL orxViewport_SetTexture(orxVIEWPORT *_pstViewport, orxTEXTURE *_pstTexture)
{
  /* Checks */
  orxASSERT(sstViewport.u32Flags & orxVIEWPORT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstViewport);

  /* Has already a texture? */
  if(orxStructure_TestFlags(_pstViewport, orxVIEWPORT_KU32_FLAG_TEXTURE) != orxFALSE)
  {
    /* Updates previous texture reference counter */
    orxStructure_DecreaseCounter((_pstViewport->pstTexture));

    /* Updates flags */
    orxStructure_SetFlags(_pstViewport, orxVIEWPORT_KU32_FLAG_NONE, orxVIEWPORT_KU32_FLAG_TEXTURE);
  }

  /* Updates texture pointer */
  _pstViewport->pstTexture = _pstTexture;
  
  /* Has a new texture? */
  if(_pstTexture != orxNULL)
  {
    /* Updates texture reference counter */
    orxStructure_IncreaseCounter(_pstTexture);

    /* Updates flags */
    orxStructure_SetFlags(_pstViewport, orxVIEWPORT_KU32_FLAG_TEXTURE, orxVIEWPORT_KU32_FLAG_NONE);
  }
  else
  {
    /* Deactivates viewport */
    orxStructure_SetFlags(_pstViewport, orxVIEWPORT_KU32_FLAG_NONE, orxVIEWPORT_KU32_FLAG_ENABLED);
  }

  return;
}

/** Gets a viewport texture
 * @param[in]   _pstViewport    Concerned viewport
 * @return      Associated orxTEXTURE / orxNULL
 */
orxTEXTURE *orxFASTCALL orxViewport_GetTexture(orxCONST orxVIEWPORT *_pstViewport)
{
  orxTEXTURE *pstResult = orxNULL;

  /* Checks */
  orxASSERT(sstViewport.u32Flags & orxVIEWPORT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstViewport);

  /* Has texture? */
  if(orxStructure_TestFlags((orxVIEWPORT *)_pstViewport, orxVIEWPORT_KU32_FLAG_TEXTURE) != orxFALSE)
  {
    /* Updates result */
    pstResult = _pstViewport->pstTexture;
  }
  else
  {
    /* Gets screen texture */
    pstResult = orxTexture_GetScreenTexture();
  }

  /* Done! */
  return pstResult;
}

/** Enables / disables a viewport
 * @param[in]   _pstViewport    Concerned viewport
 * @param[in]   _bEnable        Enable / disable
 */
orxVOID orxFASTCALL orxViewport_Enable(orxVIEWPORT *_pstViewport, orxBOOL _bEnable)
{
  /* Checks */
  orxASSERT(sstViewport.u32Flags & orxVIEWPORT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstViewport);

  /* Enable? */
  if(_bEnable != orxFALSE)
  {
    /* Updates flags */
    orxStructure_SetFlags(_pstViewport, orxVIEWPORT_KU32_FLAG_ENABLED, orxVIEWPORT_KU32_FLAG_NONE);
  }
  else
  {
    /* Updates flags */
    orxStructure_SetFlags(_pstViewport, orxVIEWPORT_KU32_FLAG_NONE, orxVIEWPORT_KU32_FLAG_ENABLED);
  }

  return;
}

/** Is a viewport enabled?
 * @param[in]   _pstViewport    Concerned viewport
 * @return      orxTRUE / orxFALSE
 */
orxBOOL orxFASTCALL orxViewport_IsEnabled(orxCONST orxVIEWPORT *_pstViewport)
{
  /* Checks */
  orxASSERT(sstViewport.u32Flags & orxVIEWPORT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstViewport);

  /* Tests */
  return(orxStructure_TestFlags((orxVIEWPORT *)_pstViewport, orxVIEWPORT_KU32_FLAG_ENABLED));
}

/** Sets a viewport camera
 * @param[in]   _pstViewport    Concerned viewport
 * @param[in]   _pstCamera      Associated camera
 */
orxVOID orxFASTCALL orxViewport_SetCamera(orxVIEWPORT *_pstViewport, orxCAMERA *_pstCamera)
{
  /* Checks */
  orxASSERT(sstViewport.u32Flags & orxVIEWPORT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstViewport);

  /* Has already a camera? */
  if(orxStructure_TestFlags(_pstViewport, orxVIEWPORT_KU32_FLAG_CAMERA) != orxFALSE)
  {
    /* Updates its reference counter */
    orxStructure_DecreaseCounter((_pstViewport->pstCamera));

    /* Updates flags */
    orxStructure_SetFlags(_pstViewport, orxVIEWPORT_KU32_FLAG_NONE, orxVIEWPORT_KU32_FLAG_CAMERA);
  }

  /* Updates camera pointer */
  _pstViewport->pstCamera = _pstCamera;

  /* Has a new camera? */
  if(_pstCamera != orxNULL)
  {
    /* Updates its reference counter */
    orxStructure_IncreaseCounter(_pstCamera);

    /* Updates flags */
    orxStructure_SetFlags(_pstViewport, orxVIEWPORT_KU32_FLAG_CAMERA, orxVIEWPORT_KU32_FLAG_NONE);
  }

  return;
} 

/** Gets a viewport camera
 * @param[in]   _pstViewport    Concerned viewport
 * @return      Associated camera / orxNULL
 */
orxCAMERA *orxFASTCALL orxViewport_GetCamera(orxCONST orxVIEWPORT *_pstViewport)
{
  orxCAMERA *pstResult = orxNULL;

  /* Checks */
  orxASSERT(sstViewport.u32Flags & orxVIEWPORT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstViewport);

  /* Has a camera? */
  if(orxStructure_TestFlags((orxVIEWPORT *)_pstViewport, orxVIEWPORT_KU32_FLAG_CAMERA) != orxFALSE)
  {
    /* Updates result */
    pstResult = _pstViewport->pstCamera;
  }

  /* Done! */
  return pstResult;
}

/** Sets a viewport position
 * @param[in]   _pstViewport    Concerned viewport
 * @param[in]   _u32X           X axis position (top left corner)
 * @param[in]   _u32Y           Y axis position (top left corner)
 */
orxVOID orxFASTCALL orxViewport_SetPosition(orxVIEWPORT *_pstViewport, orxU32 _u32X, orxU32 _u32Y)
{
  /* Checks */
  orxASSERT(sstViewport.u32Flags & orxVIEWPORT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstViewport);

  /* Updates position */
  _pstViewport->u32X = _u32X;
  _pstViewport->u32Y = _u32Y;

  return;
}

/** Sets a viewport relative position
 * @param[in]   _pstViewport    Concerned viewport
 * @param[in]   _u32AlignFlags  Alignment flags
 */
orxSTATUS orxFASTCALL orxViewport_SetRelativePosition(orxVIEWPORT *_pstViewport, orxU32 _u32AlignFlags)
{
  orxTEXTURE *pstTexture;
  orxSTATUS   eResult;

  /* Checks */
  orxASSERT(sstViewport.u32Flags & orxVIEWPORT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstViewport);
  orxASSERT((_u32AlignFlags & orxVIEWPORT_KU32_MASK_ALIGN) == _u32AlignFlags);
  orxASSERT(_pstViewport->u32Width != orxU32_UNDEFINED);
  orxASSERT(_pstViewport->u32Height != orxU32_UNDEFINED);

  /* Gets associated texture */
  pstTexture = orxViewport_GetTexture(_pstViewport);
  
  /* Valid? */
  if(pstTexture != orxNULL)
  {
    orxFLOAT fHeight, fWidth;

    /* Gets texture size */
    orxTexture_GetSize(pstTexture, &fWidth, &fHeight);

    /* Align left? */
    if(_u32AlignFlags & orxVIEWPORT_KU32_FLAG_ALIGN_LEFT)
    {
      /* Updates x position */
      _pstViewport->u32X = 0;
    }
    /* Align right? */
    else if(_u32AlignFlags & orxVIEWPORT_KU32_FLAG_ALIGN_RIGHT)
    {
      /* Updates x position */
      _pstViewport->u32X = orxF2U(fWidth) - _pstViewport->u32Width;
    }
    /* Align center */
    else
    {
      /* Updates x position */
      _pstViewport->u32X = (orxF2U(fWidth) - _pstViewport->u32Width) >> 1;
    }

    /* Align top? */
    if(_u32AlignFlags & orxVIEWPORT_KU32_FLAG_ALIGN_TOP)
    {
      /* Updates y position */
      _pstViewport->u32Y = 0;
    }
    /* Align bottom? */
    else if(_u32AlignFlags & orxVIEWPORT_KU32_FLAG_ALIGN_BOTTOM)
    {
      /* Updates y position */
      _pstViewport->u32Y = orxF2U(fHeight) - _pstViewport->u32Height;
    }
    /* Align center */
    else
    {
      /* Updates y position */
      _pstViewport->u32Y = (orxF2U(fHeight) - _pstViewport->u32Height) >> 1;
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

/** Gets a viewport position
 * @param[in]   _pstViewport    Concerned viewport
 * @param[out]  _pu32X          X axis position (top left corner)
 * @param[out]  _pu32Y          Y axis position (top left corner)
 */
orxVOID orxFASTCALL orxViewport_GetPosition(orxCONST orxVIEWPORT *_pstViewport, orxU32 *_pu32X, orxU32 *_pu32Y)
{
  /* Checks */
  orxASSERT(sstViewport.u32Flags & orxVIEWPORT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstViewport);
  orxASSERT(_pu32X != orxNULL);
  orxASSERT(_pu32Y != orxNULL);

  /* Gets position */
  *_pu32X = _pstViewport->u32X;
  *_pu32Y = _pstViewport->u32Y;

  return;
}

/** Sets a viewport size
 * @param[in]   _pstViewport    Concerned viewport
 * @param[in]   _u32W           Width
 * @param[in]   _u32H           Height
 */
orxVOID orxFASTCALL orxViewport_SetSize(orxVIEWPORT *_pstViewport, orxU32 _u32W, orxU32 _u32H)
{
  /* Checks */
  orxASSERT(sstViewport.u32Flags & orxVIEWPORT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstViewport);

  /* Updates size */
  _pstViewport->u32Width  = _u32W;
  _pstViewport->u32Height = _u32H;

  return;
}

/** Sets a viewport relative size
 * @param[in]   _pstViewport    Concerned viewport
 * @param[in]   _fW             Width (0.0f - 1.0f)
 * @param[in]   _fH             Height (0.0f - 1.0f)
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxViewport_SetRelativeSize(orxVIEWPORT *_pstViewport, orxFLOAT _fW, orxFLOAT _fH)
{
  orxTEXTURE *pstTexture;
  orxFLOAT    fTextureWidth, fTextureHeight;
  orxSTATUS   eResult;

  /* Checks */
  orxASSERT(sstViewport.u32Flags & orxVIEWPORT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstViewport);
  orxASSERT((_fW >= 0.0f) && (_fW <= 1.0f))
  orxASSERT((_fH >= 0.0f) && (_fH <= 1.0f))

  /* Gets viewport texture */
  pstTexture = orxViewport_GetTexture(_pstViewport);

  /* Valid? */
  if(pstTexture != orxNULL)
  {
    /* Gets texture size */
    orxTexture_GetSize(pstTexture, &fTextureWidth, &fTextureHeight);

    /* Updates viewport size */
    _pstViewport->u32Width  = orxF2U(fTextureWidth * _fW);
    _pstViewport->u32Height = orxF2U(fTextureHeight * _fH);

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

/** Gets a viewport size
 * @param[in]   _pstViewport    Concerned viewport
 * @param[out]  _pu32W          Width
 * @param[out]  _pu32H          Height
 */
orxVOID orxFASTCALL orxViewport_GetSize(orxCONST orxVIEWPORT *_pstViewport, orxU32 *_pu32W, orxU32 *_pu32H)
{
  /* Checks */
  orxASSERT(sstViewport.u32Flags & orxVIEWPORT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstViewport);
  orxASSERT(_pu32W != orxNULL);
  orxASSERT(_pu32H != orxNULL);

  /* Gets size */
  *_pu32W = _pstViewport->u32Width;
  *_pu32H = _pstViewport->u32Height;

  return;
}

/** Gets a viewport relative size
 * @param[in]   _pstViewport    Concerned viewport
 * @param[out]  _f32W           Relative width
 * @param[out]  _f32H           Relative height
 */
orxVOID orxFASTCALL orxViewport_GetRelativeSize(orxCONST orxVIEWPORT *_pstViewport, orxFLOAT *_pfW, orxFLOAT *_pfH)
{
  orxTEXTURE *pstTexture;
  orxFLOAT    fTextureWidth, fTextureHeight;

  /* Checks */
  orxASSERT(sstViewport.u32Flags & orxVIEWPORT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstViewport);
  orxASSERT(_pfW != orxNULL);
  orxASSERT(_pfH != orxNULL);

  /* Gets viewport texture */
  pstTexture = orxViewport_GetTexture(_pstViewport);

  /* Valid? */
  if(pstTexture != orxNULL)
  {
    /* Gets texture size */
    orxTexture_GetSize(pstTexture, &fTextureWidth, &fTextureHeight);

    /* Gets relative size */
    *_pfW = orxU2F(_pstViewport->u32Width) / fTextureWidth;
    *_pfH = orxU2F(_pstViewport->u32Height) / fTextureHeight;
  }
  else
  {
    /* !!! MSG !!! */

    /* Empties result */
    *_pfW = orxFLOAT_0;
    *_pfH = orxFLOAT_0;
  }

  /* Done! */
  return;
}

/** Gets a viewport clipping
 * @param[in]   _pstViewport    Concerned viewport
 * @param[out]  _pu32TLX        X coordinate of top left corner
 * @param[out]  _pu32TLY        Y coordinate of top left corner
 * @param[out]  _pu32BRX        X coordinate of bottom right corner
 * @param[out]  _pu32BRY        Y coordinate of bottom right corner
 */
orxVOID orxFASTCALL orxViewport_GetClipping(orxCONST orxVIEWPORT *_pstViewport, orxU32 *_pu32TLX, orxU32 *_pu32TLY, orxU32 *_pu32BRX, orxU32 *_pu32BRY)
{
  /* Checks */
  orxASSERT(sstViewport.u32Flags & orxVIEWPORT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstViewport);
  orxASSERT(_pu32TLX != orxNULL);
  orxASSERT(_pu32TLY != orxNULL);
  orxASSERT(_pu32BRX != orxNULL);
  orxASSERT(_pu32BRY != orxNULL);

  /* Gets top left corner coordinates */
  orxViewport_GetPosition(_pstViewport, _pu32TLX, _pu32TLY);

  /* Gets bottom right corner coordinates */
  orxViewport_GetSize(_pstViewport, _pu32BRX, _pu32BRY);
  (*_pu32BRX) += *_pu32TLX;
  (*_pu32BRY) += *_pu32TLY;

  return;
}
