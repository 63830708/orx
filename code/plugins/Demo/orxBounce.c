/* Orx - Portable Game Engine
 *
 * Copyright (c) 2008-2012 Orx-Project
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
 * @file orxBounce.c
 * @date 07/04/2008
 * @author iarwain@orx-project.org
 *
 * Bounce demo
 *
 */


#include "orxPluginAPI.h"

static orxU32       su32VideoModeIndex = 0;
static orxSPAWNER  *spoBallSpawner;
static orxVIEWPORT *spstViewport;
static orxFLOAT     sfShaderPhase = orx2F(0.0f);
static orxFLOAT     sfShaderAmplitude = orx2F(0.0f);
static orxFLOAT     sfShaderFrequency = orx2F(1.0f);
static orxVECTOR    svColor;
static orxFLOAT     sfColorTime = orx2F(0.0f);
static orxBOOL      sbRecord = orxFALSE;

/** Applies current selected video mode
 */
static void orxBounce_ApplyCurrentVideoMode()
{
  orxDISPLAY_VIDEO_MODE stVideoMode;

  /* Gets desired video mode */
  orxDisplay_GetVideoMode(su32VideoModeIndex, &stVideoMode);

  /* Applies it */
  orxDisplay_SetVideoMode(&stVideoMode);
}

/** Bounce event handler
 * @param[in]   _pstEvent                     Sent event
 * @return      orxSTATUS_SUCCESS if handled / orxSTATUS_FAILURE otherwise
 */
static orxSTATUS orxFASTCALL orxBounce_EventHandler(const orxEVENT *_pstEvent)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Profiles */
  orxPROFILER_PUSH_MARKER("Bounce_EventHandler");

  /* Checks */
  orxASSERT((_pstEvent->eType == orxEVENT_TYPE_PHYSICS)
         || (_pstEvent->eType == orxEVENT_TYPE_INPUT)
         || (_pstEvent->eType == orxEVENT_TYPE_SHADER)
         || (_pstEvent->eType == orxEVENT_TYPE_SOUND)
         || (_pstEvent->eType == orxEVENT_TYPE_DISPLAY)
         || (_pstEvent->eType == orxEVENT_TYPE_TIMELINE));

  /* Depending on event type */
  switch(_pstEvent->eType)
  {
    /* Input */
    case orxEVENT_TYPE_INPUT:
    {
      orxINPUT_EVENT_PAYLOAD *pstPayload;

      /* Gets event payload */
      pstPayload = (orxINPUT_EVENT_PAYLOAD *)_pstEvent->pstPayload;

      /* Has a multi-input info? */
      if(pstPayload->aeType[1] != orxINPUT_TYPE_NONE)
      {
        /* Logs info */
        orxLOG("[%s::%s] is %s (%s/v=%g + %s/v=%g)", pstPayload->zSetName, pstPayload->zInputName, (_pstEvent->eID == orxINPUT_EVENT_ON) ? "ON " : "OFF", orxInput_GetBindingName(pstPayload->aeType[0], pstPayload->aeID[0]), pstPayload->afValue[0], orxInput_GetBindingName(pstPayload->aeType[1], pstPayload->aeID[1]), pstPayload->afValue[1]);
      }
      else
      {
        /* Logs info */
        orxLOG("[%s::%s] is %s (%s/v=%g)", pstPayload->zSetName, pstPayload->zInputName, (_pstEvent->eID == orxINPUT_EVENT_ON) ? "ON " : "OFF", orxInput_GetBindingName(pstPayload->aeType[0], pstPayload->aeID[0]), pstPayload->afValue[0]);
      }

      break;
    }

    /* Physics */
    case orxEVENT_TYPE_PHYSICS:
    {
      /* Colliding? */
      if(_pstEvent->eID == orxPHYSICS_EVENT_CONTACT_ADD)
      {
        /* Adds bump FX on both objects */
        orxObject_AddUniqueFX(orxOBJECT(_pstEvent->hSender), "Bump");
        orxObject_AddUniqueFX(orxOBJECT(_pstEvent->hRecipient), "Bump");
      }

      break;
    }

    /* Shader */
    case orxEVENT_TYPE_SHADER:
    {
      orxSHADER_EVENT_PAYLOAD *pstPayload;

      /* Profiles */
      orxPROFILER_PUSH_MARKER("Bounce_EventHandler_Shader");

      /* Checks */
      orxASSERT(_pstEvent->eID == orxSHADER_EVENT_SET_PARAM);

      /* Gets its payload */
      pstPayload = (orxSHADER_EVENT_PAYLOAD *)_pstEvent->pstPayload;

      /* Phase? */
      if(!orxString_Compare(pstPayload->zParamName, "phase"))
      {
        /* Updates its value */
        pstPayload->fValue = sfShaderPhase;
      }
      else if(!orxString_Compare(pstPayload->zParamName, "color"))
      {
        orxVector_Copy(&pstPayload->vValue, &svColor);
      }
      /* Frequency? */
      else if(!orxString_Compare(pstPayload->zParamName, "frequency"))
      {
        /* Updates its value */
        pstPayload->fValue = sfShaderFrequency;
      }
      /* Amplitude? */
      else if(!orxString_Compare(pstPayload->zParamName, "amplitude"))
      {
        /* Updates its value */
        pstPayload->fValue = sfShaderAmplitude;
      }

      /* Profiles */
      orxPROFILER_POP_MARKER();

      break;
    }

    /* Sound */
    case orxEVENT_TYPE_SOUND:
    {
      /* Recording packet? */
      if(_pstEvent->eID == orxSOUND_EVENT_RECORDING_PACKET)
      {
        orxSOUND_EVENT_PAYLOAD *pstPayload;

        /* Gets event payload */
        pstPayload = (orxSOUND_EVENT_PAYLOAD *)_pstEvent->pstPayload;

        /* Is recording input active? */
        if(orxInput_IsActive("Record") != orxFALSE)
        {
          orxU32 i;

          /* For all samples */
          for(i = 0; i < pstPayload->stStream.stPacket.u32SampleNumber / 2; i++)
          {
            /* Shorten the packets by half */
            pstPayload->stStream.stPacket.as16SampleList[i] = pstPayload->stStream.stPacket.as16SampleList[i * 2];
          }

          /* Updates sample number */
          pstPayload->stStream.stPacket.u32SampleNumber = pstPayload->stStream.stPacket.u32SampleNumber / 2;

          /* Asks for writing it */
          pstPayload->stStream.stPacket.bDiscard = orxFALSE;
        }
        else
        {
          /* Asks for not writing it */
          pstPayload->stStream.stPacket.bDiscard = orxTRUE;
        }
      }

      break;
    }

    /* Display */
    case orxEVENT_TYPE_DISPLAY:
    {
      /* New video mode? */
      if(_pstEvent->eID == orxDISPLAY_EVENT_SET_VIDEO_MODE)
      {
        orxDISPLAY_EVENT_PAYLOAD *pstPayload;
        orxCHAR                   acBuffer[1024];

        /* Gets payload */
        pstPayload = (orxDISPLAY_EVENT_PAYLOAD *)_pstEvent->pstPayload;

        /* Updates title string */
        orxConfig_PushSection("Bounce");
        orxString_NPrint(acBuffer, 1024, "%s (%ldx%ld)", orxConfig_GetString("Title"), pstPayload->u32Width, pstPayload->u32Height);
        orxConfig_PopSection();

        /* Updates display module config content */
        orxConfig_PushSection(orxDISPLAY_KZ_CONFIG_SECTION);
        orxConfig_SetString(orxDISPLAY_KZ_CONFIG_TITLE, acBuffer);
        orxConfig_PopSection();

        /* Updates window */
        orxDisplay_SetVideoMode(orxNULL);
      }

      break;
    }

    /* TimeLine */
    case orxEVENT_TYPE_TIMELINE:
    {
      /* New event triggered? */
      if(_pstEvent->eID == orxTIMELINE_EVENT_TRIGGER)
      {
        orxTIMELINE_EVENT_PAYLOAD *pstPayload;

        /* Gets event payload */
        pstPayload = (orxTIMELINE_EVENT_PAYLOAD *)_pstEvent->pstPayload;

        /* Logs info */
        orxLOG("[%s::%s::%s] has been triggered at (%g)", orxObject_GetName(orxOBJECT(_pstEvent->hSender)), pstPayload->zTrackName, pstPayload->zEvent, pstPayload->fTimeStamp);
      }

      break;
    }

    default:
    {
      break;
    }
  }

  /* Profiles */
  orxPROFILER_POP_MARKER();

  /* Done! */
  return eResult;
}

/** Update callback
 */
static void orxFASTCALL orxBounce_Update(const orxCLOCK_INFO *_pstClockInfo, void *_pstContext)
{
  orxVECTOR vMousePos;
  orxBOOL   bInViewport;

  if((sbRecord == orxFALSE) && (orxInput_IsActive("Record") != orxFALSE))
  {
    /* Starts recording with default settings */
    orxSound_StartRecording("orxSoundRecording.wav", orxFALSE, 0, 0);

    /* Updates status */
    sbRecord = orxTRUE;
  }

  if(orxInput_IsActive("ToggleProfiler") && orxInput_HasNewStatus("ToggleProfiler"))
  {
    /* Toggles profiler rendering */
    orxConfig_PushSection(orxRENDER_KZ_CONFIG_SECTION);
    orxConfig_SetBool(orxRENDER_KZ_CONFIG_SHOW_PROFILER, !orxConfig_GetBool(orxRENDER_KZ_CONFIG_SHOW_PROFILER));
    orxConfig_PopSection();
  }

  if(orxInput_IsActive("PreviousResolution") && orxInput_HasNewStatus("PreviousResolution"))
  {
    /* Updates video mode index */
    su32VideoModeIndex = (su32VideoModeIndex == 0) ? orxDisplay_GetVideoModeCounter() - 1 : su32VideoModeIndex - 1;

    /* Applies it */
    orxBounce_ApplyCurrentVideoMode();
  }
  else if(orxInput_IsActive("NextResolution") && orxInput_HasNewStatus("NextResolution"))
  {
    /* Updates video mode index */
    su32VideoModeIndex = (su32VideoModeIndex >= orxDisplay_GetVideoModeCounter() - 1) ? 0 : su32VideoModeIndex + 1;

    /* Applies it */
    orxBounce_ApplyCurrentVideoMode();
  }
  if(orxInput_IsActive("ToggleFullScreen") && orxInput_HasNewStatus("ToggleFullScreen"))
  {
    /* Toggles full screen display */
    orxDisplay_SetFullScreen(!orxDisplay_IsFullScreen());
  }

  /* Pushes config section */
  orxConfig_PushSection("Bounce");

  /* Updates shader values */
  sfShaderPhase    += orxConfig_GetFloat("ShaderPhaseSpeed") * _pstClockInfo->fDT;
  sfShaderFrequency = orxConfig_GetFloat("ShaderMaxFrequency") * orxMath_Sin(orxConfig_GetFloat("ShaderFrequencySpeed") * _pstClockInfo->fTime);
  sfShaderAmplitude = orxConfig_GetFloat("ShaderMaxAmplitude") * orxMath_Sin(orxConfig_GetFloat("ShaderAmplitudeSpeed") * _pstClockInfo->fTime);

  /* Updates color time */
  sfColorTime -= _pstClockInfo->fDT;

  /* Should update color */
  if(sfColorTime <= orxFLOAT_0)
  {
    orxConfig_PushSection("BounceShader");
    orxConfig_GetVector("color", &svColor);
    orxConfig_PopSection();

    sfColorTime += orx2F(3.0f);
  }

  /* Gets mouse world position */
  bInViewport = (orxRender_GetWorldPosition(orxMouse_GetPosition(&vMousePos), &vMousePos) != orxNULL) ? orxTRUE : orxFALSE;

  /* Is mouse in a viewport? */
  if(bInViewport != orxFALSE)
  {
    /* Updates position */
    vMousePos.fZ += orx2F(0.5f);

    /* Has ball spawner? */
    if(spoBallSpawner != orxNULL)
    {
      /* Updates its position */
      orxSpawner_SetPosition(spoBallSpawner, &vMousePos);
    }

    /* Spawning */
    if(orxInput_IsActive("Spawn"))
    {
      /* Spawns one ball */
      orxSpawner_Spawn(spoBallSpawner, 1);
    }
    /* Picking? */
    else if(orxInput_IsActive("Pick"))
    {
      orxOBJECT *pstObject;

      /* Updates mouse position */
      vMousePos.fZ -= orx2F(0.1f);

      /* Picks object under mouse */
      pstObject = orxObject_Pick(&vMousePos);

      /* Found? */
      if(pstObject)
      {
        /* Adds FX */
        orxObject_AddUniqueFX(pstObject, "Pick");
      }
    }
  }

  /* Pops config section */
  orxConfig_PopSection();

  /* Toggle shader? */
  if(orxInput_IsActive("ToggleShader") && (orxInput_HasNewStatus("ToggleShader")))
  {
    /* Toggles shader */
    orxViewport_EnableShader(spstViewport, !orxViewport_IsShaderEnabled(spstViewport));
  }
}

/** Inits the bounce demo
 */
static orxSTATUS orxBounce_Init()
{
  orxCLOCK   *pstClock;
  orxSTATUS   eResult;

  /* Loads config file and selects its section */
  orxConfig_Load("Bounce.ini");
  orxConfig_SelectSection("Bounce");

  /* Loads input */
  orxInput_Load(orxNULL);

  /* Creates ball spawner */
  spoBallSpawner = orxSpawner_CreateFromConfig("BallSpawner");

  /* Valid? */
  if(spoBallSpawner != orxNULL)
  {
    orxOBJECT *pstParticleSource;

    /* Creates particle source */
    pstParticleSource = orxObject_CreateFromConfig("ParticleSource");

    /* Valid? */
    if(pstParticleSource != orxNULL)
    {
      /* Sets its parent */
      orxObject_SetParent(pstParticleSource, spoBallSpawner);
    }

    /* Updates cursor */
    orxMouse_ShowCursor(orxConfig_GetBool("ShowCursor"));

    /* Creates walls */
    orxObject_CreateFromConfig("Walls");

    /* Creates viewport on screen */
    spstViewport = orxViewport_CreateFromConfig("BounceViewport");
    orxViewport_EnableShader(spstViewport, orxFALSE);

    /* Gets rendering clock */
    pstClock = orxClock_FindFirst(orx2F(-1.0f), orxCLOCK_TYPE_CORE);

    /* Registers callback */
    eResult = orxClock_Register(pstClock, &orxBounce_Update, orxNULL, orxMODULE_ID_MAIN, orxCLOCK_PRIORITY_NORMAL);

    /* Registers event handler */
    eResult = ((eResult != orxSTATUS_FAILURE) && (orxEvent_AddHandler(orxEVENT_TYPE_PHYSICS, orxBounce_EventHandler) != orxSTATUS_FAILURE)) ? orxSTATUS_SUCCESS : orxSTATUS_FAILURE;
    eResult = ((eResult != orxSTATUS_FAILURE) && (orxEvent_AddHandler(orxEVENT_TYPE_INPUT, orxBounce_EventHandler) != orxSTATUS_FAILURE)) ? orxSTATUS_SUCCESS : orxSTATUS_FAILURE;
    eResult = ((eResult != orxSTATUS_FAILURE) && (orxEvent_AddHandler(orxEVENT_TYPE_SHADER, orxBounce_EventHandler) != orxSTATUS_FAILURE)) ? orxSTATUS_SUCCESS : orxSTATUS_FAILURE;
    eResult = ((eResult != orxSTATUS_FAILURE) && (orxEvent_AddHandler(orxEVENT_TYPE_SOUND, orxBounce_EventHandler) != orxSTATUS_FAILURE)) ? orxSTATUS_SUCCESS : orxSTATUS_FAILURE;
    eResult = ((eResult != orxSTATUS_FAILURE) && (orxEvent_AddHandler(orxEVENT_TYPE_DISPLAY, orxBounce_EventHandler) != orxSTATUS_FAILURE)) ? orxSTATUS_SUCCESS : orxSTATUS_FAILURE;
    eResult = ((eResult != orxSTATUS_FAILURE) && (orxEvent_AddHandler(orxEVENT_TYPE_TIMELINE, orxBounce_EventHandler) != orxSTATUS_FAILURE)) ? orxSTATUS_SUCCESS : orxSTATUS_FAILURE;
  }
  else
  {
    /* Failure */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/* Registers plugin entry */
orxPLUGIN_DECLARE_ENTRY_POINT(orxBounce_Init);
