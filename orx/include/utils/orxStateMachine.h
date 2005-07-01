/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/**
 * @file
 * @date 20/06/2005
 * @author (C) Arcallians
 * 
 * @todo 
 */

/**
 * @addtogroup StateMachine
 * 
 * State Machine.
 * Allows to create state machines, for various purposes.
 * @{
 */

#ifndef _orxSTATEMACHINE_H_
#define _orxSTATEMACHINE_H_

#include "orxInclude.h"
#include "memory/orxMemory.h"

/* Internal state structure. */
typedef struct __orxSTATEMACHINE_STATE_t orxSTATEMACHINE_STATE;

/* Internal link structure. */
typedef struct __orxSTATEMACHINE_LINK_t orxSTATEMACHINE_LINK;

/* Internal state machine structure. */
typedef struct __orxSTATEMACHINE_t orxSTATEMACHINE;

/* Internal instance structure. */
typedef struct __orxSTATEMACHINE_INSTANCE_t orxSTATEMACHINE_INSTANCE;

/* Define flags. */
#define orxSTATEMACHINE_KU32_FLAGS_NONE            0x00000000  /**< No flags (default behaviour) */
#define orxSTATEMACHINE_KU32_FLAGS_NOT_EXPANDABLE  0x00000001  /**< The state machine will not be expandable */

/** @name Module management.
 * @{ */
/** Initialize StateMachine Module.
 * @return Returns the initialization status.
 */
extern orxDLLAPI orxSTATUS orxStateMachine_Init();

/** Exit StateMachine module.
 */
extern orxDLLAPI orxVOID orxStateMachine_Exit();
/** @} */


/***************************************************************************
 * Enum declaration                                                        *
 ***************************************************************************/

/** State types. */
typedef enum __orxSTATEMACHINE_STATE_TYPE_t
{
  orxSTATEMACHINE_STATE_TYPE_EXECUTE = 0,
  orxSTATEMACHINE_STATE_TYPE_SKIP
} orxSTATEMACHINE_STATE_TYPE;


/***************************************************************************
 * Pointer declaration                                                     *
 ***************************************************************************/

/** Action pointer for state machines. */
typedef orxVOID (*orxSTATEMACHINE_ACTION_PTR)(orxVOID);

/** condition pointer for state machines. */
typedef orxBOOL (*orxSTATEMACHINE_CONDITION_PTR)(orxVOID);


/***************************************************************************
 * Public functions                                                        *
 ***************************************************************************/

/** Create a state machine and return a pointer on it.
 * @param[in] _u16NbStates          Number of states.
 * @param[in] _u32NbLinks           Number of links.
 * @param[in] _u32Flags             Flags used by the state machine.
 * @param[in] _eMemType             Memory type to use.
 * @return Returns a pointer on the state machine or orxNULL if failed.
 */
extern orxDLLAPI orxSTATEMACHINE *          orxStateMachine_Create(orxU16 _u16NbStates, orxU32 _u32NbLinks, orxU32 _u32Flags, orxMEMORY_TYPE _eMemType);

/** Delete a state machine.
 * @param[in] _pstStateMachine      The state machine to remove.
 * @return Returns the status of the operation.
 */
extern orxDLLAPI orxSTATUS                  orxStateMachine_Delete(orxSTATEMACHINE * _pstStateMachine);

/** Clear a state machine
 * @param[in] _pstStateMachine      The state machine to clear.
 */
extern orxDLLAPI orxVOID                    orxStateMachine_Clear(orxSTATEMACHINE * _pstStateMachine);

/** Add a state.
 * @param[in] _pstStateMachine      The state machine.
 * @param[in] _u16Id                Identifier for the state.
 * @param[in] _eStateType           Type of state.
 * @param[in] _cbAction             Action callback.
 * @return Returns the new state.
 */
extern orxDLLAPI orxSTATEMACHINE_STATE *    orxStateMachine_State_Add(orxSTATEMACHINE * _pstStateMachine, orxU16 _u16Id, orxSTATEMACHINE_STATE_TYPE _eStateType, orxSTATEMACHINE_ACTION_PTR _cbInit, orxSTATEMACHINE_ACTION_PTR _cbExecute, orxSTATEMACHINE_ACTION_PTR _cbExit);

/** Find a state.
 * @param[in] _pstStateMachine      The state machine.
 * @param[in] _u16Id                The identifier of the state.
 * @return Returns the state.
 */
extern orxDLLAPI orxSTATEMACHINE_STATE *    orxStateMachine_State_Get(orxSTATEMACHINE * _pstStateMachine, orxU16 _u16Id);

/** Remove a state.
 * @param[in] _pstStateMachine      The state machine.
 * @param[in] _pstState             The state to remove.
 * @return Returns the status of the operation.
 */
extern orxDLLAPI orxSTATUS                  orxStateMachine_State_Remove(orxSTATEMACHINE * _pstStateMachine, orxSTATEMACHINE_STATE * _pstState, orxBOOL _bRemoveLinks);

/** Add a link.
 * @param[in] _pstStateMachine      The state machine.
 * @param[in] _pstBeginningState    The state marking the beginning of the link.
 * @param[in] _pstEndingState       The state marking the ending of the link.
 * @param[in] _cbCondition          Condition callback.
 * @return Returns the new link.
 */
extern orxDLLAPI orxSTATEMACHINE_LINK *     orxStateMachine_Link_Add(orxSTATEMACHINE * _pstStateMachine, orxSTATEMACHINE_STATE * _pstBeginningState, orxSTATEMACHINE_STATE * _pstEndingState, orxSTATEMACHINE_CONDITION_PTR _cbCondition);

/** Find a link.
 * @param[in] _pstStateMachine      The state machine.
 * @param[in] _pstBeginningState    The state marking the beginning of the link.
 * @param[in] _pstEndingState       The state marking the ending of the link.
 * @return Returns the corresponding link.
 */
extern orxDLLAPI orxSTATEMACHINE_LINK *     orxStateMachine_Link_Get(orxSTATEMACHINE * _pstStateMachine, orxSTATEMACHINE_STATE * _pstBeginningState, orxSTATEMACHINE_STATE * _pstEndingState);

/** Remove a link.
 * @param[in] _pstStateMachine      The state machine.
 * @param[in] _pstLink              The link to remove.
 * @return Returns the status of the operation.
 */
extern orxDLLAPI orxSTATUS                  orxStateMachine_Link_Remove(orxSTATEMACHINE * _pstStateMachine, orxSTATEMACHINE_LINK * _pstLink);

/** Clear all links.
 * @param[in] _pstStateMachine      The state machine.
 */
extern orxDLLAPI orxVOID                    orxStateMachine_Link_Clear(orxSTATEMACHINE * _pstStateMachine);

/** Create an instance of a state machine.
 * @param[in] _pstStateMachine      The state machine.
 * @return Returns the instance.
 */
extern orxDLLAPI orxSTATEMACHINE_INSTANCE * orxStateMachine_Instance_Create(orxSTATEMACHINE * _pstStateMachine);

/** Remove an instance of a state machine.
 * @param[in] _pstInstance          The instance to remove.
 * @return Returns the status of the operation.
 */
extern orxDLLAPI orxSTATUS                  orxStateMachine_Instance_Remove(orxSTATEMACHINE_INSTANCE * _pstInstance);

/** Update an instance of a state machine. If current state is orxNULL, it enters the initial state. 
 * @param[in] _pstInstance          The instance.
 * @return Returns the status of the operation. It fails if nothing has happend.
 */
extern orxDLLAPI orxSTATUS orxStateMachine_Instance_Update(orxSTATEMACHINE_INSTANCE * _pstInstance);

/** Update all instances of a state machine. If current state is orxNULL, it enters the initial state.
 * @param[in] _pstStateMachine      The state machine.
 * @return Returns the status of the operation. It fails if nothing has happend.
 */
extern orxDLLAPI orxSTATUS orxStateMachine_Instance_UpdateAll(orxSTATEMACHINE * _pstStateMachine);


#endif /** _orxSTATEMACHINE_H_ */

/** @} */
