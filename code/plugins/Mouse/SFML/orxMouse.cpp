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
 * @file orxMouse.cpp
 * @date 26/11/2007
 * @author iarwain@orx-project.org
 *
 * SFML mouse plugin implementation
 *
 * @todo
 */


extern "C"
{
  #include "orxInclude.h"

  #include "plugin/orxPluginUser.h"
  #include "plugin/orxPlugin.h"

  #include "core/orxConfig.h"
  #include "core/orxEvent.h"
  #include "core/orxSystem.h"
  #include "io/orxMouse.h"
  #include "display/orxDisplay.h"
}

#include <SFML/Graphics.hpp>


/** Module flags
 */
#define orxMOUSE_KU32_STATIC_FLAG_NONE        0x00000000 /**< No flags */

#define orxMOUSE_KU32_STATIC_FLAG_READY       0x00000001 /**< Ready flag */

#define orxMOUSE_KU32_STATIC_MASK_ALL         0xFFFFFFFF /**< All mask */


/***************************************************************************
 * Structure declaration                                                   *
 ***************************************************************************/

/** Static structure
 */
typedef struct __orxMOUSE_STATIC_t
{
  orxU32      u32Flags;
  orxVECTOR   vMouseMove, vMouseBackup;
  orxFLOAT    fWheelMove;
  sf::Input  *poInput;

} orxMOUSE_STATIC;


/***************************************************************************
 * Static variables                                                        *
 ***************************************************************************/

/** Static data
 */
orxSTATIC orxMOUSE_STATIC sstMouse;


/***************************************************************************
 * Private functions                                                       *
 ***************************************************************************/

/** Event handler
 */
orxSTATUS orxFASTCALL EventHandler(orxCONST orxEVENT *_pstEvent)
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Is a mouse move? */
  if((_pstEvent->eType == orxEVENT_TYPE_FIRST_RESERVED + sf::Event::MouseMoved)
  && (_pstEvent->eID == sf::Event::MouseMoved))
  {
    sf::Event *poEvent;

    /* Gets SFML event */
    poEvent = (sf::Event *)(_pstEvent->pstPayload);

    /* Updates mouse move */
    sstMouse.vMouseMove.fX += orxS2F(poEvent->MouseMove.X) - sstMouse.vMouseBackup.fX;
    sstMouse.vMouseMove.fY += orxS2F(poEvent->MouseMove.Y) - sstMouse.vMouseBackup.fY;

    /* Stores last mouse position */
    sstMouse.vMouseBackup.fX = orxS2F(poEvent->MouseMove.X);
    sstMouse.vMouseBackup.fY = orxS2F(poEvent->MouseMove.Y);

    /* Updates result */
    eResult = orxSTATUS_SUCCESS;
  }
  /* Is a mouse wheel? */
  if((_pstEvent->eType == orxEVENT_TYPE_FIRST_RESERVED + sf::Event::MouseWheelMoved)
  && (_pstEvent->eID == sf::Event::MouseWheelMoved))
  {
    sf::Event *poEvent;

    /* Gets SFML event */
    poEvent = (sf::Event *)(_pstEvent->pstPayload);

    /* Updates wheel move */
    sstMouse.fWheelMove += orxS2F(poEvent->MouseWheel.Delta);

    /* Updates result */
    eResult = orxSTATUS_SUCCESS;
  }

  /* Done! */
  return eResult;
}

extern "C" orxSTATUS orxMouse_SFML_ShowCursor(orxBOOL _bShow)
{
  orxEVENT  stEvent;
  orxSTATUS eResult;

  /* Checks */
  orxASSERT((sstMouse.u32Flags & orxMOUSE_KU32_STATIC_FLAG_READY) == orxMOUSE_KU32_STATIC_FLAG_READY);

  /* Inits event */
  orxMemory_Zero(&stEvent, sizeof(orxEVENT));
  stEvent.eType       = (orxEVENT_TYPE)(orxEVENT_TYPE_FIRST_RESERVED + sf::Event::MouseButtonPressed);
  stEvent.eID         = (orxEVENT_TYPE)(orxEVENT_TYPE_FIRST_RESERVED + sf::Event::MouseButtonPressed);
  stEvent.pstPayload  = (orxVOID *)&_bShow;

  /* Sends system close event */
  eResult = orxEvent_Send(&stEvent);

  /* Done! */
  return eResult;
}

extern "C" orxSTATUS orxMouse_SFML_Init()
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Was already initialized. */
  if(!(sstMouse.u32Flags & orxMOUSE_KU32_STATIC_FLAG_READY))
  {
    /* Cleans static controller */
    orxMemory_Zero(&sstMouse, sizeof(orxMOUSE_STATIC));

    /* Registers our mouse event handler */
    if(orxEvent_AddHandler((orxEVENT_TYPE)(orxEVENT_TYPE_FIRST_RESERVED + sf::Event::MouseMoved), EventHandler) != orxSTATUS_FAILURE)
    {
      /* Registers our mouse wheell event handler */
      if(orxEvent_AddHandler((orxEVENT_TYPE)(orxEVENT_TYPE_FIRST_RESERVED + sf::Event::MouseWheelMoved), EventHandler) != orxSTATUS_FAILURE)
      {
        /* Terrible hack : gets application input from display SFML plugin */
        sstMouse.poInput = (sf::Input *)orxDisplay_GetApplicationInput();

        /* Valid? */
        if(sstMouse.poInput != orxNULL)
        {
          /* Updates status */
          sstMouse.u32Flags |= orxMOUSE_KU32_STATIC_FLAG_READY;

          /* Sets config section */
          orxConfig_SelectSection(orxMOUSE_KZ_CONFIG_SECTION);

          /* Has show cursor value? */
          if(orxConfig_HasValue(orxMOUSE_KZ_CONFIG_SHOW_CURSOR) != orxFALSE)
          {
            /* Updates cursor status */
            orxMouse_SFML_ShowCursor(orxConfig_GetBool(orxMOUSE_KZ_CONFIG_SHOW_CURSOR));
          }

          /* Updates result */
          eResult = orxSTATUS_SUCCESS;
        }
        else
        {
          /* Removes event handlers */
          orxEvent_RemoveHandler((orxEVENT_TYPE)(orxEVENT_TYPE_FIRST_RESERVED + sf::Event::MouseMoved), EventHandler);
          orxEvent_RemoveHandler((orxEVENT_TYPE)(orxEVENT_TYPE_FIRST_RESERVED + sf::Event::MouseWheelMoved), EventHandler);
        }
      }
      else
      {
        /* Removes event handler */
        orxEvent_RemoveHandler((orxEVENT_TYPE)(orxEVENT_TYPE_FIRST_RESERVED + sf::Event::MouseMoved), EventHandler);
      }
    }
  }

  /* Done! */
  return eResult;
}

extern "C" orxVOID orxMouse_SFML_Exit()
{
  /* Was initialized? */
  if(sstMouse.u32Flags & orxMOUSE_KU32_STATIC_FLAG_READY)
  {
    /* Unregisters event handlers */
    orxEvent_RemoveHandler((orxEVENT_TYPE)(orxEVENT_TYPE_FIRST_RESERVED + sf::Event::MouseMoved), EventHandler);
    orxEvent_RemoveHandler((orxEVENT_TYPE)(orxEVENT_TYPE_FIRST_RESERVED + sf::Event::MouseWheelMoved), EventHandler);

    /* Cleans static controller */
    orxMemory_Zero(&sstMouse, sizeof(orxMOUSE_STATIC));
  }

  return;
}

extern "C" orxSTATUS orxMouse_SFML_SetPosition(orxCONST orxVECTOR *_pvPosition)
{
  orxEVENT  stEvent;
  orxSTATUS eResult;

  /* Checks */
  orxASSERT((sstMouse.u32Flags & orxMOUSE_KU32_STATIC_FLAG_READY) == orxMOUSE_KU32_STATIC_FLAG_READY);

  /* Inits event */
  orxMemory_Zero(&stEvent, sizeof(orxEVENT));
  stEvent.eType       = (orxEVENT_TYPE)(orxEVENT_TYPE_FIRST_RESERVED + sf::Event::MouseMoved);
  stEvent.eID         = (orxEVENT_TYPE)(orxEVENT_TYPE_FIRST_RESERVED + sf::Event::MouseMoved);
  stEvent.pstPayload  = (orxVOID *)_pvPosition;

  /* Sends system close event */
  eResult = orxEvent_Send(&stEvent);

  /* Done! */
  return eResult;
}

extern "C" orxVECTOR *orxMouse_SFML_GetPosition(orxVECTOR *_pvPosition)
{
  orxVECTOR *pvResult = _pvPosition;

  /* Checks */
  orxASSERT((sstMouse.u32Flags & orxMOUSE_KU32_STATIC_FLAG_READY) == orxMOUSE_KU32_STATIC_FLAG_READY);
  orxASSERT(_pvPosition != orxNULL);

  /* Gets mouse position */
  _pvPosition->fX = orxS2F(sstMouse.poInput->GetMouseX());
  _pvPosition->fY = orxS2F(sstMouse.poInput->GetMouseY());
  _pvPosition->fZ = orxFLOAT_0;

  /* Done! */
  return pvResult;
}

extern "C" orxBOOL orxMouse_SFML_IsButtonPressed(orxMOUSE_BUTTON _eButton)
{
  orxBOOL bResult;

  /* Checks */
  orxASSERT((sstMouse.u32Flags & orxMOUSE_KU32_STATIC_FLAG_READY) == orxMOUSE_KU32_STATIC_FLAG_READY);
  orxASSERT(_eButton < orxMOUSE_BUTTON_NUMBER);

  /* Depending on button */
  switch(_eButton)
  {
    case orxMOUSE_BUTTON_LEFT:
    {
      /* Updates result */
      bResult = sstMouse.poInput->IsMouseButtonDown(sf::Mouse::Left) ? orxTRUE : orxFALSE;
      break;
    }

    case orxMOUSE_BUTTON_RIGHT:
    {
      /* Updates result */
      bResult = sstMouse.poInput->IsMouseButtonDown(sf::Mouse::Right) ? orxTRUE : orxFALSE;
      break;
    }

    case orxMOUSE_BUTTON_MIDDLE:
    {
      /* Updates result */
      bResult = sstMouse.poInput->IsMouseButtonDown(sf::Mouse::Middle) ? orxTRUE : orxFALSE;
      break;
    }

    case orxMOUSE_BUTTON_EXTRA_1:
    {
      /* Updates result */
      bResult = sstMouse.poInput->IsMouseButtonDown(sf::Mouse::XButton1) ? orxTRUE : orxFALSE;
      break;
    }

    case orxMOUSE_BUTTON_EXTRA_2:
    {
      /* Updates result */
      bResult = sstMouse.poInput->IsMouseButtonDown(sf::Mouse::XButton2) ? orxTRUE : orxFALSE;
      break;
    }

    default:
    {
      /* Updates result */
      bResult = orxFALSE;
      break;
    }
  }

  /* Done! */
  return bResult;
}

extern "C" orxVECTOR *orxMouse_SFML_GetMoveDelta(orxVECTOR *_pvMoveDelta)
{
  orxVECTOR *pvResult = _pvMoveDelta;

  /* Checks */
  orxASSERT((sstMouse.u32Flags & orxMOUSE_KU32_STATIC_FLAG_READY) == orxMOUSE_KU32_STATIC_FLAG_READY);
  orxASSERT(_pvMoveDelta != orxNULL);

  /* Updates result */
  orxVector_Copy(_pvMoveDelta, &(sstMouse.vMouseMove));

  /* Clears move */
  orxVector_Copy(&(sstMouse.vMouseMove), &orxVECTOR_0);

  /* Done! */
  return pvResult;
}

extern "C" orxFLOAT orxMouse_SFML_GetWheelDelta()
{
  orxFLOAT fResult;

  /* Checks */
  orxASSERT((sstMouse.u32Flags & orxMOUSE_KU32_STATIC_FLAG_READY) == orxMOUSE_KU32_STATIC_FLAG_READY);

  /* Updates result */
  fResult = sstMouse.fWheelMove;

  /* Clears wheel move */
  sstMouse.fWheelMove = orxFLOAT_0;

  /* Done! */
  return fResult;
}


/***************************************************************************
 * Plugin related                                                          *
 ***************************************************************************/

orxPLUGIN_USER_CORE_FUNCTION_START(MOUSE);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxMouse_SFML_Init, MOUSE, INIT);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxMouse_SFML_Exit, MOUSE, EXIT);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxMouse_SFML_SetPosition, MOUSE, SET_POSITION);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxMouse_SFML_GetPosition, MOUSE, GET_POSITION);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxMouse_SFML_IsButtonPressed, MOUSE, IS_BUTTON_PRESSED);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxMouse_SFML_GetMoveDelta, MOUSE, GET_MOVE_DELTA);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxMouse_SFML_GetWheelDelta, MOUSE, GET_WHEEL_DELTA);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxMouse_SFML_ShowCursor, MOUSE, SHOW_CURSOR);
orxPLUGIN_USER_CORE_FUNCTION_END();
