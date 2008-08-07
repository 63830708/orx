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
 * @file orxGraphic.h
 * @date 08/12/2003
 * @author iarwain@orx-project.org
 *
 * @todo
 * Later on, add a texture cache system for rotated/scaled texture.
 * This cache system should be activated or not by user.
 * Adds 3D system (Not planned yet).
 */

/**
 * @addtogroup orxGraphic
 *
 * Graphic Module.
 * Allows to creates and handle 2D/3D Graphics.
 * They are used as texture/animation container, with associated properties.
 * 2D Graphics are used by objects.
 * They thus can be referenced by objects as structures.
 *
 * @{
 */


#ifndef _orxGRAPHIC_H_
#define _orxGRAPHIC_H_

#include "orxInclude.h"

#include "display/orxTexture.h"
#include "object/orxStructure.h"


/** Graphic flags
 */
#define orxGRAPHIC_KU32_FLAG_NONE             0x00000000  /**< No flags */

#define orxGRAPHIC_KU32_FLAG_2D               0x00000001  /**< 2D type graphic flag  */

#define orxGRAPHIC_KU32_FLAG_FLIP_X           0x00000010  /**< X axis flipping flag */
#define orxGRAPHIC_KU32_FLAG_FLIP_Y           0x00000020  /**< Y axis flipping flag */
#define orxGRAPHIC_KU32_MASK_FLIP_BOTH        0x00000030  /**< Both axis flippinf mask */

#define orxGRAPHIC_KU32_MASK_USER_ALL         0x000000FF  /**< User all ID mask */


/** Internal Graphic structure
 */
typedef struct __orxGRAPHIC_t                 orxGRAPHIC;


/** Graphic module setup
 */
extern orxDLLAPI orxVOID                      orxGraphic_Setup();

/** Inits the Graphic module
 */
extern orxDLLAPI orxSTATUS                    orxGraphic_Init();

/** Exits from the Graphic module
 */
extern orxDLLAPI orxVOID                      orxGraphic_Exit();


/** Creates an empty graphic
 * @param[in]   _u32Flags                     Graphic flags (2D / ...)
 * @return      Created orxGRAPHIC / orxNULL
 */
extern orxDLLAPI orxGRAPHIC *orxFASTCALL      orxGraphic_Create(orxU32 _u32Flags);

/** Creates a graphic from config
 * @param[in]   _zConfigID            Config ID
 * @ return orxGRAPHIC / orxNULL
 */
extern orxDLLAPI orxGRAPHIC *orxFASTCALL      orxGraphic_CreateFromConfig(orxCONST orxSTRING _zConfigID);

/** Deletes a graphic
 * @param[in]   _pstGraphic       Graphic to delete
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
extern orxDLLAPI orxSTATUS orxFASTCALL        orxGraphic_Delete(orxGRAPHIC *_pstGraphic);


/** Sets graphic data
 * @param[in]   _pstGraphic     Concerned graphic
 * @param[in]   _pstData        Data structure to set / orxNULL
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
extern orxDLLAPI orxSTATUS orxFASTCALL        orxGraphic_SetData(orxGRAPHIC *_pstGraphic, orxSTRUCTURE *_pstData);

/** Gets graphic data
 * @param[in]   _pstGraphic     Concerned graphic
 * @return      OrxSTRUCTURE / orxNULL
 */
extern orxDLLAPI orxSTRUCTURE *orxFASTCALL    orxGraphic_GetData(orxCONST orxGRAPHIC *_pstGraphic);

/** Sets graphic pivot
 * @param[in]   _pstGraphic     Concerned graphic
 * @param[in]   _pvPivot        Pivot to set
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
extern orxDLLAPI orxSTATUS orxFASTCALL        orxGraphic_SetPivot(orxGRAPHIC *_pstGraphic, orxCONST orxVECTOR *_pvPivot);

/** Gets graphic pivot
 * @param[in]   _pstGraphic     Concerned graphic
 * @param[out]  _pvPivot        Graphic pivot
 * @return      orxPIVOT / orxNULL
 */
extern orxDLLAPI orxVECTOR *orxFASTCALL       orxGraphic_GetPivot(orxCONST orxGRAPHIC *_pstGraphic, orxVECTOR *_pvPivot);


/** Gets graphic size
 * @param[in]   _pstGraphic     Concerned graphic
 * @param[out]  _pfWidth        Object's width
 * @param[out]  _pfHeight       Object's height
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
extern orxDLLAPI orxSTATUS orxFASTCALL        orxGraphic_GetSize(orxCONST orxGRAPHIC *_pstGraphic, orxFLOAT *_pfWidth, orxFLOAT *_pfHeight);

/** Sets graphic color
 * @param[in]   _pstGraphic     Concerned graphic
 * @param[in]   _pstColor       Color to set
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
extern orxDLLAPI orxSTATUS orxFASTCALL        orxGraphic_SetColor(orxGRAPHIC *_pstGraphic, orxCONST orxCOLOR *_pstColor);

/** Sets graphic repeat (wrap) value
 * @param[in]   _pstGraphic     Concerned graphic
 * @param[in]   _fRepeatX       X-axis repeat value
 * @param[in]   _fRepeatY       Y-axis repeat value
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
extern orxDLLAPI orxSTATUS orxFASTCALL        orxGraphic_SetRepeat(orxGRAPHIC *_pstGraphic, orxFLOAT _fRepeatX, orxFLOAT _fRepeatY);

/** Clears graphic color
 * @param[in]   _pstGraphic     Concerned graphic
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
extern orxDLLAPI orxSTATUS orxFASTCALL        orxGraphic_ClearColor(orxGRAPHIC *_pstGraphic);

/** Graphic has color accessor
 * @param[in]   _pstGraphic     Concerned graphic
 * @return      orxTRUE / orxFALSE
 */
extern orxDLLAPI orxBOOL orxFASTCALL          orxGraphic_HasColor(orxCONST orxGRAPHIC *_pstGraphic);

/** Gets graphic color
 * @param[in]   _pstGraphic     Concerned graphic
 * @param[out]  _pstColor       Object's color
 * @return      orxCOLOR / orxNULL
 */
extern orxDLLAPI orxCOLOR *orxFASTCALL        orxGraphic_GetColor(orxCONST orxGRAPHIC *_pstGraphic, orxCOLOR *_pstColor);

/** Gets graphic repeat (wrap) values
 * @param[in]   _pstGraphic     Concerned graphic
 * @param[out]  _pfRepeatX      X-axis repeat value
 * @param[out]  _pfRepeatY      Y-axis repeat value
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
extern orxDLLAPI orxSTATUS orxFASTCALL        orxGraphic_GetRepeat(orxCONST orxGRAPHIC *_pstGraphic, orxFLOAT *_pfRepeatX, orxFLOAT *_pfRepeatY);

/** Gets graphic top
 * @param[in]   _pstGraphic     Concerned graphic
 * @return      Top coordinate
 */
extern orxDLLAPI orxFLOAT orxFASTCALL         orxGraphic_GetTop(orxCONST orxGRAPHIC *_pstGraphic);

/** Gets graphic left
 * @param[in]   _pstGraphic     Concerned graphic
 * @return      Left coordinate
 */
extern orxDLLAPI orxFLOAT orxFASTCALL         orxGraphic_GetLeft(orxCONST orxGRAPHIC *_pstGraphic);

#endif /* _orxGRAPHIC_H_ */


/** @} */
