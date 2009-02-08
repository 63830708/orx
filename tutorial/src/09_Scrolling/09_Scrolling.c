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
 * @file 09_Scrolling.c
 * @date 26/08/2008
 * @author iarwain@orx-project.org
 *
 * Scrolling tutorial
 */


#include "orxPluginAPI.h"


/* This is a basic C tutorial showing how to do a differential parallax scrolling.
 * As we are using the default executable for this tutorial, this code
 * will be loaded and executed as a runtime plugin.
 *
 * In addition, some basics are handled for us by the main executable.
 * First of all, it will load all available plugins and modules. If you
 * require only some of those, then it's better to write your own executable
 * instead of a plugin. This will be covered in a later tutorial.
 *
 * The main executable also handles some keys:
 * - F11 as vertical sync toggler
 * - Escape as exit key
 * - Backspace to reload all configuration files (provided that config history is turned on)
 * It also exits if the orxSYSTEM_EVENT_CLOSE signal is sent.
 *
 * See previous tutorials for more info about the basic object creation, clock, animation, viewport, sound, FX and physics/collision handling.
 *
 * This tutorial shows how to display a differential scrolling.
 *
 * As you can see, there's no special code for the differential scrolling. Actually,
 * orx's default 2D render plugin will take care of this for you, depending on how you
 * set the objects' properties in the config file. By default, AutoScroll is set to 'both'.
 *
 * This means a differential scrolling will happen on both X and Y axis when the camera moves.
 * You can try to set this value to x, y or even remove it.
 *
 * Along the AutoScroll property, you can find the DepthScale one. This one is used to automatically
 * adjust object's scale depending on how far it is from the camera.
 * The smaller the camera frustum is, the faster this autoscale will apply. You can try to play with
 * object positionning and camera near & far planes to achieve the desired scrolling and depth scale you want.
 *
 * You can change the scrolling speed (ie. the camera move speed) in the config file. As usual, you can
 * modify its value in real time and ask for a config history reload.
 *
 * As you can see, our update code simply moves the camera in the 3D space.
 * Pressing arrows will move it along X and Y axis, and pressing keypad '+' & '-' will move it along the Z one.
 * As told before, all the differential scrolling will happen because objects have been flagged accordingly.
 * Your code merely needs to move your camera in your scenery, without having to bother about scrolling effect.
 * This gives you a full control about how many scrolling planes you want, and which objects should be affected by it.
 *
 * Thus, by pressing 'S' and 'N' keys, we update the corresponding config value directly in memory.
 * The same effect could be achieved by modifying the config file and asking for a reload by pressing backspace.
 *
 * The last point concerns the sky. As seen in the tutorial 03_Frame, we set the sky object's frame as a child
 * of the camera one. This means the position set for the sky object in the config file will always be
 * relative to the camera one.
 * In other words, the sky will always follow the camera, and as we put it, by default, at a depth of 1000 (ie. the
 * same value as the camera far frustum plane), it'll stay in the background.
 */


/** Tutorial objects
 */
orxCAMERA *pstCamera;


/** Update callback
 */
void orxFASTCALL Update(const orxCLOCK_INFO *_pstClockInfo, void *_pstContext)
{
  orxVECTOR vMove, vPosition, vScrollSpeed;

  /* *** SCROLLING UPDATE *** */

  /* Clears move vector */
  orxVector_Copy(&vMove, &orxVECTOR_0);

  /* Selects tutorial config section */
  orxConfig_SelectSection("Tutorial");

  /* Gets scroll speed */
  orxConfig_GetVector("ScrollSpeed", &vScrollSpeed);

  /* Updates scroll speed with our current DT */
  orxVector_Mulf(&vScrollSpeed, &vScrollSpeed, _pstClockInfo->fDT);

  /* Going right? */
  if(orxInput_IsActive("CameraRight"))
  {
    /* Updates move vector */
    vMove.fX += vScrollSpeed.fX;
  }
  /* Going left? */
  if(orxInput_IsActive("CameraLeft"))
  {
    /* Updates move vector */
    vMove.fX -= vScrollSpeed.fX;
  }
  /* Going down? */
  if(orxInput_IsActive("CameraDown"))
  {
    /* Updates move vector */
    vMove.fY += vScrollSpeed.fY;
  }
  /* Going up? */
  if(orxInput_IsActive("CameraUp"))
  {
    /* Updates move vector */
    vMove.fY -= vScrollSpeed.fY;
  }
  /* Zoom in? */
  if(orxInput_IsActive("CameraZoomIn"))
  {
    /* Updates move vector */
    vMove.fZ += vScrollSpeed.fZ;
  }
  /* Zoom out? */
  if(orxInput_IsActive("CameraZoomOut"))
  {
    /* Updates move vector */
    vMove.fZ -= vScrollSpeed.fZ;
  }

  /* Updates camera position */
  orxCamera_SetPosition(pstCamera, orxVector_Add(&(vPosition), orxCamera_GetPosition(pstCamera, &vPosition), &vMove));
}


/** Inits the tutorial
 */
orxSTATUS Init()
{
  orxVIEWPORT  *pstViewport;
  orxCLOCK     *pstClock;
  orxOBJECT    *pstSky;
  orxU32        i;
  orxINPUT_TYPE eType;
  orxENUM       eID;
  orxSTRING     zInputCameraLeft, zInputCameraRight, zInputCameraUp, zInputCameraDown;
  orxSTRING     zInputCameraZoomIn, zInputCameraZoomOut;

  /* Loads config file and selects main section */
  orxConfig_Load("../09_Scrolling.ini");
  orxConfig_SelectSection("Tutorial");

  /* Gets input binding names */
  orxInput_GetBinding("CameraLeft", 0, &eType, &eID);
  zInputCameraLeft = orxInput_GetBindingName(eType, eID);

  orxInput_GetBinding("CameraRight", 0, &eType, &eID);
  zInputCameraRight = orxInput_GetBindingName(eType, eID);

  orxInput_GetBinding("CameraUp", 0, &eType, &eID);
  zInputCameraUp = orxInput_GetBindingName(eType, eID);

  orxInput_GetBinding("CameraDown", 0, &eType, &eID);
  zInputCameraDown = orxInput_GetBindingName(eType, eID);

  orxInput_GetBinding("CameraZoomIn", 0, &eType, &eID);
  zInputCameraZoomIn = orxInput_GetBindingName(eType, eID);

  orxInput_GetBinding("CameraZoomOut", 0, &eType, &eID);
  zInputCameraZoomOut = orxInput_GetBindingName(eType, eID);

  /* Displays a small hint in console */
  orxLOG("\n- '%s', '%s', '%s' & '%s' will move the camera"
         "\n- '%s' & '%s' will zoom in/out"
         "\n* The scrolling and auto-scaling of objects is data-driven, no code required"
         "\n* The sky background will follow the camera (parent/child frame relation)",
         zInputCameraUp, zInputCameraLeft, zInputCameraDown, zInputCameraRight,
         zInputCameraZoomIn, zInputCameraZoomOut);

  /* Creates viewport */
  pstViewport = orxViewport_CreateFromConfig("Viewport");

  /* Gets camera */
  pstCamera = orxViewport_GetCamera(pstViewport);

  /* Creates a 100 Hz clock */
  pstClock = orxClock_Create(orx2F(0.01f), orxCLOCK_TYPE_USER);

  /* Registers our update callback */
  orxClock_Register(pstClock, Update, orxNULL, orxMODULE_ID_MAIN, orxCLOCK_PRIORITY_NORMAL);

  /* Creates sky */
  pstSky = orxObject_CreateFromConfig("Sky");

  /* For all requested clouds */
  for(i = 0; i < orxConfig_GetU32("CloudNumber"); i++)
  {
    /* Creates it */
    orxObject_CreateFromConfig("Cloud");
  }

  /* Done! */
  return orxSTATUS_SUCCESS;
}

/* Registers plugin entry */
orxPLUGIN_DECLARE_ENTRY_POINT(Init);
