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
 * @file 08_Physics.c
 * @date 21/08/2008
 * @author iarwain@orx-project.org
 *
 * Physics tutorial
 */


#include "orxPluginAPI.h"


/* This is a basic C tutorial creating physical bodies and playing with physics and collisions.
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
 * - F12 to capture a screenshot
 * - Backspace to reload all configuration files (provided that config history is turned on)
 * It also exits if the orxSYSTEM_EVENT_CLOSE signal is sent.
 *
 * See previous tutorials for more info about the basic object creation, clock, animation, viewport, sound and FX handling.
 *
 * This tutorial shows how to add physical properties to objects and handle collisions.
 *
 * As you can see, the physical properties are completely data-driven. So creating an object
 * with physical properties (ie. with a body) or without is the exact same line of code.
 *
 * Objects can be linked to a body which can be static or dynamic.
 * Each body can be made of up to 8 parts.
 *
 * A body part is defined by
 * - its shape (currently box, sphere and mesh (ie. convex polygon) are the only available)
 * - information about the shape size (corners for the box, center and radius for the sphere, vertices for the mesh)
 *   if no size data is specified, the shape will try to fill the complete body (using the
 *   object size and scale)
 * - collision "self" flags that defines this part
 * - collision "check" mask that defines with which other parts this one will collide
 *   (two parts in the same body will never collide)
 * - a flag (Solid) specifying if this shaped should only give information about collisions
 *   or if it should impact on the body physics simulation (bouncing, etc...)
 * - various attributes such as restitution, friction, density, ...
 *
 * Here we create static solid walls around our screen. We then spawn boxes in the middle.
 * The number of boxes created is tweakable through the config file and is 100 by default.
 *
 * The only interaction possible is using left and right mouse buttons to rotate the camera.
 * As we rotate it, we also update the gravity vector of our simulation.
 * Doing so, it gives the impression that the boxes will be always falling toward the bottom of our screen
 * no matter how the camera is rotated.
 *
 * We also register to the physics events to add a visual FXs on two colliding objects.
 * By default the FX is a fast color flash and is, as usual, tweakable in realtime (ie. reloading
 * the config history will apply the new settings immediately as the FX isn't kept in cache by default).
 * 
 * Updating an object scale (including changing its scale with FXs) will update its physical properties.
 * Keep in mind that scaling an object with a physical body is more expensive as we have to delete
 * the current shapes and recreate them at the correct size.
 * This is done as our current single physics plugin is based on Box2D which doesn't allow realtime
 * rescaling of shapes.
 *
 * This tutorial does only show basic physics and collision control, but, for example, you can also
 * be notified with events for object separating or keeping contact.
 *
 */


/** Tutorial objects
 */
orxCAMERA *pstCamera;


/** Event handler
 */
orxSTATUS orxFASTCALL EventHandler(const orxEVENT *_pstEvent)
{
  /* Is a new contact? */
  if(_pstEvent->eID == orxPHYSICS_EVENT_CONTACT_ADD)
  {
    orxOBJECT *pstObject1, *pstObject2;

    /* Gets colliding objects */
    pstObject1 = orxOBJECT(_pstEvent->hRecipient);
    pstObject2 = orxOBJECT(_pstEvent->hSender);

    /* Adds bump FX on both objects */
    orxObject_AddFX(pstObject1, "Bump");
    orxObject_AddFX(pstObject2, "Bump");
  }

  /* Done! */
  return orxSTATUS_SUCCESS;
}


/** Update callback
 */
void orxFASTCALL Update(const orxCLOCK_INFO *_pstClockInfo, void *_pstContext)
{
  orxFLOAT fDeltaRotation = orxFLOAT_0;

  /* Rotating left? */
  if(orxInput_IsActive("RotateLeft"))
  {
    /* Computes rotation delta */
    fDeltaRotation = orx2F(4.0f) * _pstClockInfo->fDT;
  }    
  /* Rotating right? */
  if(orxInput_IsActive("RotateRight"))
  {
    /* Computes rotation delta */
    fDeltaRotation = orx2F(-4.0f) * _pstClockInfo->fDT;
  }

  /* Turned? */
  if(fDeltaRotation != orxFLOAT_0)
  {
    orxVECTOR vGravity;

    /* Rotates camera */
    orxCamera_SetRotation(pstCamera, orxCamera_GetRotation(pstCamera) + fDeltaRotation);

    /* Gets gravity */
    if(orxPhysics_GetGravity(&vGravity))
    {
      /* Updates it */
      orxVector_2DRotate(&vGravity, &vGravity, fDeltaRotation);

      /* Applies it */
      orxPhysics_SetGravity(&vGravity);
    }
  }
}


/** Inits the tutorial
 */
orxSTATUS Init()
{
  orxCLOCK     *pstClock;
  orxVIEWPORT  *pstViewport;
  orxU32        i;
  orxINPUT_TYPE eType;
  orxENUM       eID;
  orxSTRING     zInputRotateLeft, zInputRotateRight;

  /* Loads config file and selects main section */
  orxConfig_Load("../08_Physics.ini");
  orxConfig_SelectSection("Tutorial");

  /* Gets input binding names */
  orxInput_GetBinding("RotateLeft", 0, &eType, &eID);
  zInputRotateLeft = orxInput_GetBindingName(eType, eID);

  orxInput_GetBinding("RotateRight", 0, &eType, &eID);
  zInputRotateRight = orxInput_GetBindingName(eType, eID);

  /* Displays a small hint in console */
  orxLOG("\n- '%s' & '%s' will rotate the camera"
         "\n* Gravity will follow the camera"
         "\n* a bump visual FX is played on objects that collide", zInputRotateLeft, zInputRotateRight);

  /* Creates viewport */
  pstViewport = orxViewport_CreateFromConfig("Viewport");

  /* Gets camera */
  pstCamera = orxViewport_GetCamera(pstViewport);

  /* Creates a 100 Hz clock */
  pstClock = orxClock_Create(orx2F(0.01f), orxCLOCK_TYPE_USER);

  /* Registers our update callback */
  orxClock_Register(pstClock, Update, orxNULL, orxMODULE_ID_MAIN, orxCLOCK_PRIORITY_NORMAL);

  /* Registers event handler */
  orxEvent_AddHandler(orxEVENT_TYPE_PHYSICS, EventHandler);

  /* Creates sky */
  orxObject_CreateFromConfig("Sky");

  /* Creates walls */
  orxObject_CreateFromConfig("Walls");

  /* For all requested boxes */
  for(i = 0; i < orxConfig_GetU32("BoxNumber"); i++)
  {
    /* Creates it */
    orxObject_CreateFromConfig("Box");
  }

  /* Done! */
  return orxSTATUS_SUCCESS;
}

/* Registers plugin entry */
orxPLUGIN_DECLARE_ENTRY_POINT(Init);
