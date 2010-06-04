/* Orx - Portable Game Engine
 *
 * Copyright (c) 2008-2010 Orx-Project
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
 * @file orxSoundSystem.c
 * @date 07/01/2009
 * @author iarwain@orx-project.org
 *
 * SDL sound system plugin implementation
 *
 */


#include "orxPluginAPI.h"

#include <SDL.h>
#include <SDL_mixer.h>


/** Module flags
 */
#define orxSOUNDSYSTEM_KU32_STATIC_FLAG_NONE      0x00000000 /**< No flags */

#define orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY     0x00000001 /**< Ready flag */

#define orxSOUNDSYSTEM_KU32_STATIC_MASK_ALL       0xFFFFFFFF /**< All mask */


/** Misc defines
 */
#define orxSOUNDSYSTEM_KS32_DEFAULT_FREQUENCY     MIX_DEFAULT_FREQUENCY
#define orxSOUNDSYSTEM_KU16_DEFAULT_FORMAT        MIX_DEFAULT_FORMAT
#define orxSOUNDSYSTEM_KS32_DEFAULT_CHANNELS      2
#define orxSOUNDSYSTEM_KS32_DEFAULT_BUFFER_SIZE   4096
#define orxSOUNDSYSTEM_KU32_BANK_SIZE             32
#define orxSOUNDSYSTEM_KF_DEFAULT_DIMENSION_RATIO orx2F(0.01f)


/***************************************************************************
 * Structure declaration                                                   *
 ***************************************************************************/

/** Internal sound structure
 */
struct __orxSOUNDSYSTEM_SOUND_t
{
  orxS32    s32Channel;
  orxFLOAT  fReferenceDistance;
  orxFLOAT  fAttenuation;
  orxFLOAT  fVolume;
  orxS16    s16RelativeAngle;
  orxS8     s8RelativeDistance;

  union
  {
    Mix_Chunk *pstSound;
    Mix_Music *pstMusic;
  };
  union
  {
    orxU32 bIsMusic : 1;
    orxU32 bIsLoop  : 1;
  };
};

/** Static structure
 */
typedef struct __orxSOUNDSYSTEM_STATIC_t
{
  orxBANK          *pstSoundBank;       /**< Sound bank */
  orxVECTOR         vListenerPosition;  /**< Listener position */
  orxFLOAT          fDimensionRatio;    /**< Dimension ratio */
  orxFLOAT          fRecDimensionRatio; /**< Reciprocal dimension ratio */
  orxU32            u32Flags;

} orxSOUNDSYSTEM_STATIC;


/***************************************************************************
 * Static variables                                                        *
 ***************************************************************************/

/** Static data
 */
static orxSOUNDSYSTEM_STATIC sstSoundSystem;


/***************************************************************************
 * Private functions                                                       *
 ***************************************************************************/

orxSTATUS orxFASTCALL orxSoundSystem_SDL_Init()
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Was already initialized. */
  if(!(sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY))
  {
    /* Cleans static controller */
    orxMemory_Zero(&sstSoundSystem, sizeof(orxSOUNDSYSTEM_STATIC));

    /* Is SDL partly initialized? */
    if(SDL_WasInit(SDL_INIT_EVERYTHING) != 0)
    {
      /* Inits the audio subsystem */
      eResult = (SDL_InitSubSystem(SDL_INIT_AUDIO) == 0) ? orxSTATUS_SUCCESS : orxSTATUS_FAILURE;
    }
    else
    {
      /* Inits SDL with audio */
      eResult = (SDL_Init(SDL_INIT_AUDIO) == 0) ? orxSTATUS_SUCCESS : orxSTATUS_FAILURE;
    }

    /* Valid? */
    if(eResult != orxSTATUS_FAILURE)
    {
      /* Opens audio mixer */
      eResult = (Mix_OpenAudio(orxSOUNDSYSTEM_KS32_DEFAULT_FREQUENCY, orxSOUNDSYSTEM_KU16_DEFAULT_FORMAT, orxSOUNDSYSTEM_KS32_DEFAULT_CHANNELS, orxSOUNDSYSTEM_KS32_DEFAULT_BUFFER_SIZE) >= 0) ? orxSTATUS_SUCCESS : orxSTATUS_FAILURE;

      /* Success? */
      if(eResult != orxSTATUS_FAILURE)
      {
        orxFLOAT fRatio;

        /* Gets dimension ratio */
        orxConfig_PushSection(orxSOUNDSYSTEM_KZ_CONFIG_SECTION);
        fRatio = orxConfig_GetFloat(orxSOUNDSYSTEM_KZ_CONFIG_RATIO);

        /* Valid? */
        if(fRatio > orxFLOAT_0)
        {
          /* Stores it */
          sstSoundSystem.fDimensionRatio = fRatio;
        }
        else
        {
          /* Stores default one */
          sstSoundSystem.fDimensionRatio = orxSOUNDSYSTEM_KF_DEFAULT_DIMENSION_RATIO;
        }

        /* Stores reciprocal dimenstion ratio */
        sstSoundSystem.fRecDimensionRatio = orxFLOAT_1 / sstSoundSystem.fDimensionRatio;

        /* Creates sound bank */
        sstSoundSystem.pstSoundBank = orxBank_Create(orxSOUNDSYSTEM_KU32_BANK_SIZE, sizeof(orxSOUNDSYSTEM_SOUND), orxBANK_KU32_FLAG_NONE, orxMEMORY_TYPE_MAIN);

        /* Updates status */
        orxFLAG_SET(sstSoundSystem.u32Flags, orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY, orxSOUNDSYSTEM_KU32_STATIC_MASK_ALL);

        /* Pops config section */
        orxConfig_PopSection();
      }
      else
      {
        /* Is audio the only subsystem initialized? */
        if(SDL_WasInit(SDL_INIT_EVERYTHING) == SDL_INIT_AUDIO)
        {
          /* Exits from SDL */
          SDL_Quit();
        }
        else
        {
          /* Exits from audio subsystem */
          SDL_QuitSubSystem(SDL_INIT_AUDIO);
        }
      }
    }
  }

  /* Done! */
  return eResult;
}

void orxFASTCALL orxSoundSystem_SDL_Exit()
{
  /* Was initialized? */
  if(sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY)
  {
    /* Deletes sound bank */
    orxBank_Delete(sstSoundSystem.pstSoundBank);

    /* Closes audio */
    Mix_CloseAudio();

    /* Is audio the only subsystem initialized? */
    if(SDL_WasInit(SDL_INIT_EVERYTHING) == SDL_INIT_AUDIO)
    {
      /* Exits from SDL */
      SDL_Quit();
    }
    else
    {
      /* Exits from audio subsystem */
      SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }

    /* Cleans static controller */
    orxMemory_Zero(&sstSoundSystem, sizeof(orxSOUNDSYSTEM_STATIC));
  }

  return;
}

orxSOUNDSYSTEM_SAMPLE *orxFASTCALL orxSoundSystem_SDL_LoadSample(const orxSTRING _zFilename)
{
  orxSOUNDSYSTEM_SAMPLE *pstResult;
  Mix_Chunk             *pstSample;

  /* Checks */
  orxASSERT((sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY) == orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY);
  orxASSERT(_zFilename != orxNULL);

  /* Loads it from file */
  if((pstSample = Mix_LoadWAV(_zFilename)) != orxNULL)
  {
    /* Updates result */
    pstResult = (orxSOUNDSYSTEM_SAMPLE *)pstSample;
  }
  else
  {
    /* Updates result */
    pstResult = (orxSOUNDSYSTEM_SAMPLE *)orxNULL;
  }

  /* Done! */
  return pstResult;
}

orxSTATUS orxFASTCALL orxSoundSystem_SDL_UnloadSample(orxSOUNDSYSTEM_SAMPLE *_pstSample)
{
  Mix_Chunk *pstSample;

  /* Checks */
  orxASSERT((sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY) == orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstSample != orxNULL);

  /* Gets sound sample */
  pstSample = (Mix_Chunk *)_pstSample;

  /* Deletes it */
  Mix_FreeChunk(pstSample);

  /* Done! */
  return orxSTATUS_SUCCESS;
}

orxSOUNDSYSTEM_SOUND *orxFASTCALL orxSoundSystem_SDL_CreateFromSample(const orxSOUNDSYSTEM_SAMPLE *_pstSample)
{
  orxSOUNDSYSTEM_SOUND *pstResult = 0;

  /* Checks */
  orxASSERT((sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY) == orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstSample != orxNULL);

  /* Creates result */
  pstResult = (orxSOUNDSYSTEM_SOUND *)orxBank_Allocate(sstSoundSystem.pstSoundBank);

  /* Stores sound */
  pstResult->pstSound = (Mix_Chunk *)_pstSample;

  /* Updates its status */
  pstResult->bIsMusic = orxFALSE;

  /* Clears channel */
  pstResult->s32Channel = -1;

  /* Done! */
  return pstResult;
}

orxSOUNDSYSTEM_SOUND *orxFASTCALL orxSoundSystem_SDL_CreateStreamFromFile(const orxSTRING _zFilename)
{
  orxSOUNDSYSTEM_SOUND *pstResult;
  Mix_Music            *pstMusic;

  /* Checks */
  orxASSERT((sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY) == orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY);
  orxASSERT(_zFilename != orxNULL);

  /* Loads it from file */
  if((pstMusic = Mix_LoadMUS(_zFilename)) != orxNULL)
  {
    /* Creates result */
    pstResult = (orxSOUNDSYSTEM_SOUND *)orxBank_Allocate(sstSoundSystem.pstSoundBank);

    /* Stores musics */
    pstResult->pstMusic = pstMusic;
    
    /* Updates its status */
    pstResult->bIsMusic = orxTRUE;

    /* Clears channel */
    pstResult->s32Channel = -1;
  }
  else
  {
    /* Updates result */
    pstResult = (orxSOUNDSYSTEM_SOUND *)orxNULL;
  }

  /* Done! */
  return pstResult;
}

orxSTATUS orxFASTCALL orxSoundSystem_SDL_Delete(orxSOUNDSYSTEM_SOUND *_pstSound)
{
  /* Checks */
  orxASSERT((sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY) == orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstSound != orxNULL);

  /* Deletes it */
  orxBank_Free(sstSoundSystem.pstSoundBank, _pstSound);

  /* Done! */
  return orxSTATUS_SUCCESS;
}

orxSTATUS orxFASTCALL orxSoundSystem_SDL_Play(orxSOUNDSYSTEM_SOUND *_pstSound)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT((sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY) == orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstSound != orxNULL);

  /* Is a music? */
  if(_pstSound->bIsMusic != orxFALSE)
  {
    /* Plays it */
    _pstSound->s32Channel = Mix_PlayMusic(_pstSound->pstMusic, (_pstSound->bIsLoop != orxFALSE) ? -1 : 0);
  }
  else
  {
    /* Plays it */
    _pstSound->s32Channel = Mix_PlayChannel(_pstSound->s32Channel, _pstSound->pstSound, (_pstSound->bIsLoop != orxFALSE) ? -1 : 0);
  }

  /* Success? */
  if(_pstSound->s32Channel >= 0)
  {
    /* Is music? */
    if(_pstSound->bIsMusic != orxFALSE)
    {
      /* Updates its volume */
      Mix_VolumeMusic(orxF2S(orx2F(128.0f) * _pstSound->fVolume));
    }
    else
    {
      /* Updates its volume */
      Mix_Volume(_pstSound->s32Channel, orxF2S(orx2F(128.0f) * _pstSound->fVolume));
    }

    /* Updates result */
    eResult = orxSTATUS_SUCCESS;
  }
  else
  {
    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

orxSTATUS orxFASTCALL orxSoundSystem_SDL_Pause(orxSOUNDSYSTEM_SOUND *_pstSound)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT((sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY) == orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstSound != orxNULL);

  /* Is music? */
  if(_pstSound->bIsMusic != orxFALSE)
  {
    /* Pauses it */
    Mix_PauseMusic();
  }
  else
  {
    /* Pauses it */
    Mix_Pause(_pstSound->s32Channel);
  }

  /* Done! */
  return eResult;
}

orxSTATUS orxFASTCALL orxSoundSystem_SDL_Stop(orxSOUNDSYSTEM_SOUND *_pstSound)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT((sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY) == orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstSound != orxNULL);

  /* Is music? */
  if(_pstSound->bIsMusic != orxFALSE)
  {
    /* Pauses it */
    Mix_HaltMusic();
  }
  else
  {
    /* Stops it */
    Mix_HaltChannel(_pstSound->s32Channel);
  }

  /* Clears channel */
  _pstSound->s32Channel = -1;

  /* Done! */
  return eResult;
}

orxSTATUS orxFASTCALL orxSoundSystem_SDL_SetVolume(orxSOUNDSYSTEM_SOUND *_pstSound, orxFLOAT _fVolume)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT((sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY) == orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstSound != orxNULL);

  /* Stores it */
  _pstSound->fVolume = _fVolume;

  /* Is music? */
  if(_pstSound->bIsMusic != orxFALSE)
  {
      /* Updates its volume */
      Mix_VolumeMusic(orxF2S(orx2F(128.0f) * _fVolume));
  }
  else
  {
      /* Updates its volume */
      Mix_Volume(_pstSound->s32Channel, orxF2S(orx2F(128.0f) * _fVolume));
  }

  /* Done! */
  return eResult;
}

orxSTATUS orxFASTCALL orxSoundSystem_SDL_SetPitch(orxSOUNDSYSTEM_SOUND *_pstSound, orxFLOAT _fPitch)
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Checks */
  orxASSERT((sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY) == orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstSound != orxNULL);

  /* Logs message */
  orxLOG("This sound plugin doesn't handle sound pitch variation.");

  /* Done! */
  return eResult;
}

orxSTATUS orxFASTCALL orxSoundSystem_SDL_SetPosition(orxSOUNDSYSTEM_SOUND *_pstSound, const orxVECTOR *_pvPosition)
{
  orxVECTOR vRelativePosition;
  orxFLOAT  fRelativeDistance, fRelativeAngle;
  orxSTATUS eResult;

  /* Checks */
  orxASSERT((sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY) == orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstSound != orxNULL);
  orxASSERT(_pvPosition != orxNULL);

  /* Is not music? */
  if(_pstSound->bIsMusic == orxFALSE)
  {
    /* Gets relative position */
    orxVector_Sub(&vRelativePosition, orxVector_Mulf(&vRelativePosition, _pvPosition, sstSoundSystem.fDimensionRatio), &(sstSoundSystem.vListenerPosition));

    /* Gets it in spherical coordinate */
    orxVector_FromCartesianToSpherical(&vRelativePosition, &vRelativePosition);

    /* Gets relative angle */
    fRelativeAngle = orxMATH_KF_RAD_TO_DEG * vRelativePosition.fTheta;

    /* Gets relaitve distance */
    fRelativeDistance = (vRelativePosition.fPhi - _pstSound->fReferenceDistance) * _pstSound->fAttenuation;

    /* Stores angle */
    _pstSound->s16RelativeAngle = (orxS16)orxF2S(fRelativeAngle);

    /* Stores distance */
    _pstSound->s8RelativeDistance = (orxU8)orxF2U(orxCLAMP(fRelativeDistance, orxFLOAT_0, orx2F(255.0f)));

    /* Updates sound position */
    eResult = (Mix_SetPosition(_pstSound->s32Channel, _pstSound->s16RelativeAngle, _pstSound->s8RelativeDistance) == 0) ? orxSTATUS_SUCCESS : orxSTATUS_FAILURE;
  }
  else
  {
    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

orxSTATUS orxFASTCALL orxSoundSystem_SDL_SetAttenuation(orxSOUNDSYSTEM_SOUND *_pstSound, orxFLOAT _fAttenuation)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT((sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY) == orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstSound != orxNULL);

  /* Stores it */
  _pstSound->fAttenuation = _fAttenuation;

  /* Done! */
  return eResult;
}

orxSTATUS orxFASTCALL orxSoundSystem_SDL_SetReferenceDistance(orxSOUNDSYSTEM_SOUND *_pstSound, orxFLOAT _fDistance)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT((sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY) == orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstSound != orxNULL);

  /* Stores it */
  _pstSound->fReferenceDistance = _fDistance;

  /* Done! */
  return eResult;
}

orxSTATUS orxFASTCALL orxSoundSystem_SDL_Loop(orxSOUNDSYSTEM_SOUND *_pstSound, orxBOOL _bLoop)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT((sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY) == orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstSound != orxNULL);

  /* Stores it */
  _pstSound->bIsLoop = _bLoop;

  /* Done! */
  return eResult;
}

orxFLOAT orxFASTCALL orxSoundSystem_SDL_GetVolume(const orxSOUNDSYSTEM_SOUND *_pstSound)
{
  orxFLOAT fResult;

  /* Checks */
  orxASSERT((sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY) == orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstSound != orxNULL);

  /* Updates result */
  fResult = _pstSound->fVolume;

  /* Done! */
  return fResult;
}

orxFLOAT orxFASTCALL orxSoundSystem_SDL_GetPitch(const orxSOUNDSYSTEM_SOUND *_pstSound)
{
  orxFLOAT fResult = orxFLOAT_0;

  /* Checks */
  orxASSERT((sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY) == orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstSound != orxNULL);

  /* Logs message */
  orxLOG("This sound plugin doesn't handle sound pitch.");

  /* Done! */
  return fResult;
}

orxVECTOR *orxFASTCALL orxSoundSystem_SDL_GetPosition(const orxSOUNDSYSTEM_SOUND *_pstSound, orxVECTOR *_pvPosition)
{
  orxVECTOR *pvResult = orxNULL;

  /* Checks */
  orxASSERT((sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY) == orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstSound != orxNULL);
  orxASSERT(_pvPosition != orxNULL);

  /* Logs message */
  orxLOG("This sound plugin doesn't handle GetPosition.");

  /* Done! */
  return pvResult;
}

orxFLOAT orxFASTCALL orxSoundSystem_SDL_GetAttenuation(const orxSOUNDSYSTEM_SOUND *_pstSound)
{
  orxFLOAT fResult;

  /* Checks */
  orxASSERT((sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY) == orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstSound != orxNULL);

  /* Gets it */
  fResult = _pstSound->fAttenuation;

  /* Done! */
  return fResult;
}

orxFLOAT orxFASTCALL orxSoundSystem_SDL_GetReferenceDistance(const orxSOUNDSYSTEM_SOUND *_pstSound)
{
  orxFLOAT fResult;

  /* Checks */
  orxASSERT((sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY) == orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstSound != orxNULL);

  /* Gets it */
  fResult = _pstSound->fReferenceDistance;

  /* Done! */
  return fResult;
}

orxBOOL orxFASTCALL orxSoundSystem_SDL_IsLooping(const orxSOUNDSYSTEM_SOUND *_pstSound)
{
  orxBOOL bResult;

  /* Checks */
  orxASSERT((sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY) == orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstSound != orxNULL);

  /* Updates result*/
  bResult = _pstSound->bIsLoop;

  /* Done! */
  return bResult;
}

orxFLOAT orxFASTCALL orxSoundSystem_SDL_GetDuration(const orxSOUNDSYSTEM_SOUND *_pstSound)
{
  orxFLOAT fResult = orxFLOAT_0;

  /* Checks */
  orxASSERT((sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY) == orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstSound != orxNULL);

  /* Logs message */
  orxLOG("This sound plugin doesn't handle GetDuration.");

  /* Done! */
  return fResult;
}

orxSOUNDSYSTEM_STATUS orxFASTCALL orxSoundSystem_SDL_GetStatus(const orxSOUNDSYSTEM_SOUND *_pstSound)
{
  orxSOUNDSYSTEM_STATUS eResult;

  /* Checks */
  orxASSERT((sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY) == orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstSound != orxNULL);

  /* Is music? */
  if(_pstSound->bIsMusic != orxFALSE)
  {
    /* Is paused? */
    if(Mix_PausedMusic())
    {
      /* Updates result */
      eResult = orxSOUNDSYSTEM_STATUS_PAUSE;
    }
    /* Is playing? */
    else if(Mix_PlayingMusic())
    {
      /* Updates result */
      eResult = orxSOUNDSYSTEM_STATUS_PLAY;
    }
    else
    {
      /* Updates result */
      eResult = orxSOUNDSYSTEM_STATUS_STOP;
    }
  }
  else
  {
    /* Is paused? */
    if(Mix_Paused(_pstSound->s32Channel))
    {
      /* Updates result */
      eResult = orxSOUNDSYSTEM_STATUS_PAUSE;
    }
    /* Is playing? */
    else if(Mix_Playing(_pstSound->s32Channel))
    {
      /* Updates result */
      eResult = orxSOUNDSYSTEM_STATUS_PLAY;
    }
    else
    {
      /* Updates result */
      eResult = orxSOUNDSYSTEM_STATUS_STOP;
    }
  }

  /* Done! */
  return eResult;
}

orxSTATUS orxFASTCALL orxSoundSystem_SDL_SetGlobalVolume(orxFLOAT _fVolume)
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Checks */
  orxASSERT((sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY) == orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY);

  /* Logs message */
  orxLOG("This sound plugin doesn't handle global volume.");

  /* Done! */
  return eResult;
}

orxFLOAT orxFASTCALL orxSoundSystem_SDL_GetGlobalVolume()
{
  orxFLOAT fResult = orxFLOAT_0;

  /* Checks */
  orxASSERT((sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY) == orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY);

  /* Logs message */
  orxLOG("This sound plugin doesn't handle global volume.");

  /* Done! */
  return fResult;
}

orxSTATUS orxFASTCALL orxSoundSystem_SDL_SetListenerPosition(const orxVECTOR *_pvPosition)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT((sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY) == orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY);
  orxASSERT(_pvPosition != orxNULL);

  /* Stores it */
  orxVector_Mulf(&(sstSoundSystem.vListenerPosition), _pvPosition, sstSoundSystem.fDimensionRatio);

  /* Done! */
  return eResult;
}

orxVECTOR *orxFASTCALL orxSoundSystem_SDL_GetListenerPosition(orxVECTOR *_pvPosition)
{
  orxVECTOR *pvResult;

  /* Checks */
  orxASSERT((sstSoundSystem.u32Flags & orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY) == orxSOUNDSYSTEM_KU32_STATIC_FLAG_READY);
  orxASSERT(_pvPosition != orxNULL);

  /* Updates result */
  pvResult = _pvPosition;
  orxVector_Mulf(pvResult, &(sstSoundSystem.vListenerPosition), sstSoundSystem.fRecDimensionRatio);

  /* Done! */
  return pvResult;
}


/***************************************************************************
 * Plugin related                                                          *
 ***************************************************************************/

orxPLUGIN_USER_CORE_FUNCTION_START(SOUNDSYSTEM);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_Init, SOUNDSYSTEM, INIT);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_Exit, SOUNDSYSTEM, EXIT);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_LoadSample, SOUNDSYSTEM, LOAD_SAMPLE);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_UnloadSample, SOUNDSYSTEM, UNLOAD_SAMPLE);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_CreateFromSample, SOUNDSYSTEM, CREATE_FROM_SAMPLE);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_CreateStreamFromFile, SOUNDSYSTEM, CREATE_STREAM_FROM_FILE);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_Delete, SOUNDSYSTEM, DELETE);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_Play, SOUNDSYSTEM, PLAY);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_Pause, SOUNDSYSTEM, PAUSE);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_Stop, SOUNDSYSTEM, STOP);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_SetVolume, SOUNDSYSTEM, SET_VOLUME);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_SetPitch, SOUNDSYSTEM, SET_PITCH);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_SetPosition, SOUNDSYSTEM, SET_POSITION);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_SetAttenuation, SOUNDSYSTEM, SET_ATTENUATION);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_SetReferenceDistance, SOUNDSYSTEM, SET_REFERENCE_DISTANCE);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_Loop, SOUNDSYSTEM, LOOP);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_GetVolume, SOUNDSYSTEM, GET_VOLUME);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_GetPitch, SOUNDSYSTEM, GET_PITCH);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_GetPosition, SOUNDSYSTEM, GET_POSITION);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_GetAttenuation, SOUNDSYSTEM, GET_ATTENUATION);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_GetReferenceDistance, SOUNDSYSTEM, GET_REFERENCE_DISTANCE);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_IsLooping, SOUNDSYSTEM, IS_LOOPING);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_GetDuration, SOUNDSYSTEM, GET_DURATION);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_GetStatus, SOUNDSYSTEM, GET_STATUS);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_SetGlobalVolume, SOUNDSYSTEM, SET_GLOBAL_VOLUME);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_GetGlobalVolume, SOUNDSYSTEM, GET_GLOBAL_VOLUME);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_SetListenerPosition, SOUNDSYSTEM, SET_LISTENER_POSITION);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxSoundSystem_SDL_GetListenerPosition, SOUNDSYSTEM, GET_LISTENER_POSITION);
orxPLUGIN_USER_CORE_FUNCTION_END();
