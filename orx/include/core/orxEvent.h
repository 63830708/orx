/**
 * @file core/orxEvent.h
 */

/***************************************************************************
 begin                : 01/09/2005
 author               : (C) Arcallians
 email                : cursor@arcallians.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/**
 * @page Event
 * @section event_intro Introduction.
 * The event module goal is to provide a simple and generalist interface to manipulate event messages.
 * 
 * The general idea is to register many handler functions to event IDs, register events and - at the process time - each event are handled by the corrresponding function.
 * 
 * @section event_api API.
 * 
 * @subsection event_api_createdelete Create and delete an event manager.
 * In order to use the event capabilities, you must create an event manager (You must destroy it after use).
 * to create it, use the function orxEvent_CreateManager precising the maximum number of event you want to store, the size of handler hash key (256 is good, see orxHashTable),
 * and the flags qualifying the event manager.
 * To destroy, you have to call orxEvent_DeleteManager with the address of the orxEVENT_MANAGER create.
 * @code
 * orxEVENT_MANAGER *pstEventManager;	// The event manager.
 * pstEventManager = orxEvent_CreateManager(200, 256, orxEVENT_KU32_FLAG_MANIPULATION_STANDARD);
 * ...
 * orxEvent_DeleteManager(pstEventManager);
 * @endcode
 * 
 * @subsection event_api_handler Register an handler.
 * In order to use events, you must register some handling functions.
 * Each function can be associate to one (or more) ID.
 * All events corresponding to the ID will be parsed by these functions.
 * To register a function, use orxEvent_RegisterHandler with the address of your function an d the corresponding ID.
 * Only one function can be associated to an ID, if you attach a new function to an ID, the old one is unregistered.
 * If you want to unregister a function, pass orxNULL to orxEvent_RegisterHandler with your ID.
 * @code
 * orxVOID orxTest_Event_Handler_1(orxEVENT_MESSAGE_TYPE u16Type, orxEVENT_MESSAGE_LIFETIME s16Time, orxVOID *pData)
 * {
 *   // Process an event...
 * };
 * orxEvent_RegisterHandler(pstEventManager, u16ID, orxTest_Event_Handler_1);
 * ...
 * orxEvent_RegisterHandler(pstEventManager, u16ID, NULL);
 * @endcode
 * 
 * @subsection event_api_registerfunction Register functions.
 * To register a function, you just have to call a 
 * 
 * @addtogroup Event
 * @{
 */

#ifndef _orxEVENT_H_
#define _orxEVENT_H_

#include "orxInclude.h"
#include "core/orxClock.h"
#include "utils/orxHashTable.h"
#include "utils/orxQueue.h"


/**
 *  Event message manipulation :
 */

/** Event message ID constructor.*/
#define orxEVENT_MESSAGE_GET_ID(TYPE, LIFETIME) ((orxU32)((TYPE << 16) | LIFETIME))

/** Event message type accessor.*/
#define orxEVENT_MESSAGE_GET_TYPE(ID)	((orxU16)((ID & 0xFFFF0000) >> 16))

/** Event message lifetime accessor.*/
#define orxEVENT_MESSAGE_GET_LIFETIME(ID)	((orxS16)(ID & 0xFFFF))

/** Event message lifetime constant.
 * Minimum negative value.*/
#define orxEVENT_KS16_MESSAGE_LIFETIME_CONSTANT	  0x7FFF


/** Event message ID type.*/
typedef orxU32 orxEVENT_MESSAGE_ID;

/** Event message owner/type type.*/
typedef orxU16 orxEVENT_MESSAGE_TYPE;

/** Event message life time step type.*/
typedef orxS16 orxEVENT_MESSAGE_LIFETIME;


/**
 * Event handler callback prototype.
 */
typedef orxVOID (*orxEVENT_FUNCTION)(orxEVENT_MESSAGE_TYPE, orxEVENT_MESSAGE_LIFETIME, orxVOID *);


/**
 * Event manager definition and manipulation :
 */
 
/** Event manager manipulation flags.*/
#define orxEVENT_KU32_FLAG_MANIPULATION_STANDARD                        0x00000000  /**< Nothing special is done.*/
#define orxEVENT_KU32_FLAG_MANIPULATION_REMOVE_NEGATIVE_LIFETIME_EVENT  0x00000001  /**< Remove negative timelife event and do not process it. */
#define orxEVENT_KU32_FLAG_MANIPULATION_REMOVE_UNPROCESSED              0x00000002  /**< Remove all unprocessed events. Dangerous if used with partial process : possible loss of data.*/
#define orxEVENT_KU32_FLAG_MANIPULATION_PARTIAL_PROCESS                 0x00000010  /**< Just do a partial process. */
#define orxEVENT_KU32_FLAG_MANIPULATION_ERROR                           0x00001000  /**< Invalid Manipulation flag. */


/** Event manager.*/
typedef struct __orxEVENT_MANAGER_t
{
	/** Manipulation flags of queue.*/
	orxU32 u32ManipFlags;

	/** Message queue.*/
	orxQUEUE *pstMessageQueue;

	/** Callback hash table.*/
	orxHASHTABLE *pstCallbackTable;
} orxEVENT_MANAGER;


/** Event module setup
 */
extern orxDLLAPI orxVOID                        orxEvent_Setup();
/** Initialize Event Module
 */
extern orxDLLAPI orxSTATUS                      orxEvent_Init();
/** Exit Event module
 */
extern orxDLLAPI orxVOID                        orxEvent_Exit();

/** Create an event manager.
 * @param _u16EventNumber Number of event the manager can store.
 * @param _u16HandlerNumber Number of handler the manager can store.
 * @param _u32Flags Flags of event manager.
 * @return Address of the manager structure, orxNULL if failed.
 */
extern orxDLLAPI orxEVENT_MANAGER *orxFASTCALL  orxEvent_CreateManager(orxU16 _u16EventNumber, orxU16 _u16HandlerNumber, orxU32 _u32Flags);

/** Delete an event manager.
 * @param _pstEventManager Event manager to destroy.
 */
extern orxDLLAPI orxVOID orxFASTCALL            orxEvent_DeleteManager(orxEVENT_MANAGER *_pstEventManager);

/** Set the flags of event manager.
 * @param _pstEventManager Event manager.
 * @param _u32Flags Flags of event manager.
 */
extern orxDLLAPI orxVOID orxFASTCALL            orxEvent_SetManagerFlags(orxEVENT_MANAGER *_pstEventManager, orxU32 _u32Flags);

/** Retrieve the flags of the event manager.
 * @param _pstEventManager Event manager.
 * @return Flags.
 */
extern orxDLLAPI orxU32 orxFASTCALL             orxEvent_GetManagerFlags(orxCONST orxEVENT_MANAGER *_pstEventManager);

/** Register an event callback function.
 * Unregister previous handler and set the new instead.
 * @param _pstEventManager Event manager.
 * @param _u16Type Type of event to intercept.
 * @param _pfnHandler Event callback function, orxNULL to only unregister previous handler.
 */
extern orxDLLAPI orxVOID orxFASTCALL            orxEvent_RegisterHandler(orxEVENT_MANAGER *_pstEventManager, orxEVENT_MESSAGE_TYPE _u16Type, orxEVENT_FUNCTION _pfnHandler);

/** Add an event to the manager.
 * @param _pstEventManager Event manager.
 * @param _u16Type Type of event.
 * @param _s16Life Remaining lifetime.
 * @param _pExtraData Address of extra data.
 */
extern orxDLLAPI orxVOID orxFASTCALL            orxEvent_Add(orxEVENT_MANAGER *_pstEventManager, orxEVENT_MESSAGE_TYPE _u16Type, orxEVENT_MESSAGE_LIFETIME _s16Life, orxVOID *_pExtraData);

/** Process events of the manager.
 * Process the events.
 * @param _pstEventManager Event manager.
 * @param _s16Ticks Number of ticks to remove from the lifetime.
 */
extern orxDLLAPI orxVOID orxFASTCALL            orxEvent_UpdateManager(orxEVENT_MANAGER *_pstEventManager, orxS16 _s16Ticks);

/** Update the event manager.
 * Function to plug into clock system.
 * @param _pstClockInfo Clock infos.
 * @param _pstContext Context. Here it is orxEVENT_MANAGER address.
 **/
extern orxDLLAPI orxVOID orxFASTCALL            orxEvent_Update(orxCONST orxCLOCK_INFO *_pstClockInfo, orxVOID *_pstContext);


#endif /*_orxEVENT_H_*/
