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
 * @file orxThread.h
 * @date 24/11/2013
 * @author iarwain@orx-project.org
 *
 * @todo
 */

/**
 * @addtogroup orxThread
 *
 * Thread file
 * Code that handles threads
 *
 * @{
 */


#ifndef _orxTHREAD_H_
#define _orxTHREAD_H_

#include "orxInclude.h"


/** Thread run function type */
typedef void *(*orxTHREAD_FUNCTION)(void *_pContext);


/** Thread module setup
 */
extern orxDLLAPI void orxFASTCALL                     orxThread_Setup();

/** Inits the thread module
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
extern orxDLLAPI orxSTATUS orxFASTCALL                orxThread_Init();

/** Exits from the thread module
 */
extern orxDLLAPI void orxFASTCALL                     orxThread_Exit();


/** Creates a new thread
 * @param[in]   _pfnRun                               Function to run on the new thread
 * @param[in]   _pContext                             Context that will be transmitted to the function when called
 * @return      Thread ID if successful, orxU32_UNDEFINED otherwise
 */
extern orxDLLAPI orxU32 orxFASTCALL                   orxThread_Create(const orxTHREAD_FUNCTION _pfnRun, void *_pContext);

/** Joins a thread (blocks & waits until the other thread finishes)
 * @param[in]   _u32ThreadID                          ID of the thread for which to wait
 * @param[out]  _ppReturnValue                        Return value from the joined thread
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
extern orxDLLAPI orxSTATUS orxFASTCALL                orxThread_Join(orxU32 _u32ThreadID, void **_ppReturnValue);

/** Gets current thread ID
 * @return      Current thread ID
 */
extern orxDLLAPI orxU32 orxFASTCALL                   orxThread_GetCurrent();

/** Yields to other threads
 */
extern orxDLLAPI void orxFASTCALL                     orxThread_Yield();


#endif /* _orxTHREAD_H_ */

/** @} */
