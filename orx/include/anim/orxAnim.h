/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License           *
 *   as published by the Free Software Foundation; either version 2.1      *
 *   of the License, or (at your option) any later version.                *
 *                                                                         *
 ***************************************************************************/

/**
 * @file orxAnim.h
 * @date 11/02/2004
 * @author (C) Arcallians
 * 
 * @todo 
 * Rewrite with new Graphic/Anim system
 */

/**
 * @addtogroup Anim
 * 
 * Animation (Data) Module.
 * Allows to creates and handle Animations data.
 * It consists of a structure containing data for a single animation
 * and functions for handling and accessing them.
 * Animations are structures.
 * They thus can be referenced by Animation Sets (orxAnimSet) Module.
 *
 * @{
 */
 

#ifndef _orxANIM_H_
#define _orxANIM_H_


#include "orxInclude.h"

#include "object/orxStructure.h"


/** Anim ID Flags
 */
#define orxANIM_KU32_ID_FLAG_NONE             0x00000000  /**< No flags */

#define orxANIM_KU32_ID_FLAG_2D               0x00010000  /**< 2D type animation ID flag */

#define orxANIM_KU32_ID_MASK_USER_ALL         0x00FF0000  /**< User all ID mask */

/** Anim defines
 */
#define orxANIM_KU32_KEY_MAX_NUMBER           256         /**< Maximum number of keys for an animation structure */


/** Internal Anim structure
 */
typedef struct __orxANIM_t                    orxANIM;


/** Anim module setup
 */
extern orxDLLAPI orxVOID                      orxAnim_Setup();

/** Inits the Anim module
 */
extern orxDLLAPI orxSTATUS                    orxAnim_Init();

/** Exits from the Anim module
 */
extern orxDLLAPI orxVOID                      orxAnim_Exit();


/** Creates an empty animation
 * @param[in]   _u32IDFLags     ID flags for created animation
 * @param[in]   _u32Size        Number of keys for this animation
 * @return      Created orxANIM / orxNULL
 */
extern orxDLLAPI orxANIM *orxFASTCALL         orxAnim_Create(orxU32 _u32IDFlags, orxU32 _u32Size);

/** Deletes an animation
 * @param[in]   _pstAnim        Anim to delete
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
extern orxDLLAPI orxSTATUS orxFASTCALL        orxAnim_Delete(orxANIM *_pstAnim);


/** Adds a key to an animation
 * @param[in]   _pstAnim        Concerned animation
 * @param[in]   _pstData        Key data to add
 * @param[in]   _u32TimeStamp   Timestamp for this key
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
extern orxDLLAPI orxSTATUS orxFASTCALL        orxAnim_AddKey(orxANIM *_pstAnim, orxSTRUCTURE *_pstData, orxU32 _u32TimeStamp);

/** Removes last added key from an animation
 * @param[in]   _pstAnim        Concerned animation
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
extern orxDLLAPI orxSTATUS orxFASTCALL        orxAnim_RemoveLastKey(orxANIM *_pstAnim);

/** Removes all keys from an animation
 * @param[in]   _pstAnim        Concerned animation
 */
extern orxDLLAPI orxVOID orxFASTCALL          orxAnim_RemoveAllKeys(orxANIM *_pstAnim);


/** Updates anim given a timestamp
 * @param[in]   _pstAnim        Concerned animation
 * @param[in]   _u32TimeStamp   TimeStamp for animation update
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
extern orxDLLAPI orxSTATUS orxFASTCALL        or_xAnim_Update(orxANIM *_pstAnim, orxU32 _u32TimeStamp);

/** Anim current key data accessor
 * @param[in]   _pstAnim        Concerned animation
 * @return      Desired orxSTRUCTURE / orxNULL
 */
extern orxDLLAPI orxSTRUCTURE *orxFASTCALL    orxAnim_GetCurrentKeyData(orxCONST orxANIM *_pstAnim);


/** Anim key data accessor
 * @param[in]   _pstAnim        Concerned animation
 * @param[in]   _u32Index       Index of desired key data
 * @return      Desired orxSTRUCTURE / orxNULL
 */
extern orxDLLAPI orxSTRUCTURE *orxFASTCALL    orxAnim_GetKeyData(orxCONST orxANIM *_pstAnim, orxU32 _u32Index);


/** Anim key storage size accessor
 * @param[in]   _pstAnim        Concerned animation
 * @return      Anim key storage size
 */
extern orxDLLAPI orxU32 orxFASTCALL           orxAnim_GetKeyStorageSize(orxCONST orxANIM *_pstAnim);

/** Anim key counter accessor
 * @param[in]   _pstAnim        Concerned animation
 * @return      Anim key counter
 */
extern orxDLLAPI orxU32 orxFASTCALL           orxAnim_GetKeyCounter(orxCONST orxANIM *_pstAnim);


/** Anim time length accessor
 * @param[in]   _pstAnim        Concerned animation
 * @return      Anim time length
 */
extern orxDLLAPI orxU32 orxFASTCALL           orxAnim_GetLength(orxCONST orxANIM *_pstAnim);


/** Anim flags test accessor
 * @param[in]   _pstAnim        Concerned animation
 * @param[in]   _u32Flags       Flags to test
 * @return      orxTRUE / orxFALSE
 */
extern orxDLLAPI orxBOOL orxFASTCALL          orxAnim_TestFlags(orxCONST orxANIM *_pstAnim, orxU32 _u32Flags);

/** Anim all flags test accessor
 * @param[in]   _pstAnim        Concerned animation
 * @param[in]   _u32Flags       Flags to test
 * @return      orxTRUE / orxFALSE
 */
extern orxDLLAPI orxBOOL orxFASTCALL          orxAnim_TestAllFlags(orxCONST orxANIM *_pstAnim, orxU32 _u32Flags);

/** Anim flag set accessor
 * @param[in]   _pstAnim        Concerned animation
 * @param[in]   _u32AddFlags    Flags to add
 * @param[in]   _u32RemoveFlags Flags to remove
 */
extern orxDLLAPI orxVOID orxFASTCALL          orxAnim_SetFlags(orxANIM *_pstAnim, orxU32 _u32AddFlags, orxU32 _u32RemoveFlags);


#endif /* _orxANIM_H_ */


/** @} */
