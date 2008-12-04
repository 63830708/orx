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
 * @file orxText.h
 * @date 02/12/2008
 * @author iarwain@orx-project.org
 *
 * @todo
 */

/**
 * @addtogroup orxText
 *
 * Text module
 * Module that handles texts
 *
 * @{
 */


#ifndef _orxTEXT_H_
#define _orxTEXT_H_

#include "orxInclude.h"

#include "display/orxDisplay.h"


/** Internal text structure */
typedef struct __orxTEXT_t                orxTEXT;


/** Setups the text module
 */
extern orxDLLAPI orxVOID                  orxText_Setup();

/** Inits the text module
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
extern orxDLLAPI orxSTATUS                orxText_Init();

/** Exits from the text module
 */
extern orxDLLAPI orxVOID                  orxText_Exit();


/** Creates an empty text
 * @return      orxTEXT / orxNULL
 */
extern orxDLLAPI orxTEXT *                orxText_Create();

/** Creates a text from config
 * @param[in]   _zConfigID    Config ID
 * @return      orxTEXT / orxNULL
 */
extern orxDLLAPI orxTEXT *orxFASTCALL     orxText_CreateFromConfig(orxCONST orxSTRING _zConfigID);

/** Deletes a text
 * @param[in]   _pstText      Concerned text
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
extern orxDLLAPI orxSTATUS orxFASTCALL    orxText_Delete(orxTEXT *_pstText);


/** Gets text size
 * @param[in]   _pstText      Concerned text
 * @param[out]  _pfWidth      Text's width
 * @param[out]  _pfHeight     Text's height
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
extern orxDLLAPI orxSTATUS orxFASTCALL    orxText_GetSize(orxCONST orxTEXT *_pstText, orxFLOAT *_pfWidth, orxFLOAT *_pfHeight);

/** Gets text name
 * @param[in]   _pstText      Concerned text
 * @return      Text name / orxNULL
 */
extern orxDLLAPI orxSTRING orxFASTCALL    orxText_GetName(orxCONST orxTEXT *_pstText);

/** Gets text given its name
 * @param[in]   _zName        Text name
 * @return      orxTEXT / orxNULL
 */
extern orxDLLAPI orxTEXT *orxFASTCALL     orxText_Get(orxCONST orxSTRING _zName);


/** Gets text string 
 * @param[in]   _pstText      Concerned text
 * @return      Text string / orxSTRING_EMPTY
 */
extern orxDLLAPI orxSTRING orxFASTCALL    orxText_GetString(orxCONST orxTEXT *_pstText);

/** Gets text font
 * @param[in]   _pstText      Concerned text
 * @return      Text font / orxNULL
 */
extern orxDLLAPI orxSTRING orxFASTCALL    orxText_GetFont(orxCONST orxTEXT *_pstText);

/** Sets text string 
 * @param[in]   _pstText      Concerned text
 * @param[in]   _zString      String to contain
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
extern orxDLLAPI orxSTATUS orxFASTCALL    orxText_SetString(orxTEXT *_pstText, orxCONST orxSTRING _zString);

/** Sets text font
 * @param[in]   _pstText      Concerned text
 * @param[in]   _zFont        Font name / orxNULL to use default
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
extern orxDLLAPI orxSTATUS orxFASTCALL    orxText_SetFont(orxTEXT *_pstText, orxCONST orxSTRING _zFont);


#endif /* _orxTEXT_H_ */

/** @} */
