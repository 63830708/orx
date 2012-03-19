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
 * @file orxSound.c
 * @date 13/07/2008
 * @author iarwain@orx-project.org
 *
 */


#include "sound/orxSound.h"

#include "debug/orxDebug.h"
#include "memory/orxMemory.h"
#include "memory/orxBank.h"
#include "core/orxConfig.h"
#include "core/orxClock.h"
#include "object/orxStructure.h"
#include "utils/orxHashTable.h"
#include "utils/orxString.h"


/** Module flags
 */
#define orxSOUND_KU32_STATIC_FLAG_NONE                  0x00000000  /**< No flags */

#define orxSOUND_KU32_STATIC_FLAG_READY                 0x00000001  /**< Ready flag */

#define orxSOUND_KU32_STATIC_MASK_ALL                   0xFFFFFFFF  /**< All mask */


/** Flags
 */
#define orxSOUND_KU32_FLAG_NONE                         0x00000000  /**< No flags */

#define orxSOUND_KU32_FLAG_HAS_SAMPLE                   0x00000001  /**< Has referenced sample flag */
#define orxSOUND_KU32_FLAG_HAS_STREAM                   0x00000002  /**< Has referenced stream flag */
#define orxSOUND_KU32_FLAG_INTERNAL_REFERENCE           0x00000004  /**< Internal reference flag */

#define orxSOUND_KU32_MASK_ALL                          0xFFFFFFFF  /**< All mask */


/** Misc defines
 */
#define orxSOUND_KU32_SAMPLE_BANK_SIZE                  32

#define orxSOUND_KZ_STREAM_DEFAULT_CHANNEL_NUMBER       1
#define orxSOUND_KZ_STREAM_DEFAULT_SAMPLE_RATE          44100

#define orxSOUND_KZ_CONFIG_SOUND                        "Sound"
#define orxSOUND_KZ_CONFIG_MUSIC                        "Music"
#define orxSOUND_KZ_CONFIG_LOOP                         "Loop"
#define orxSOUND_KZ_CONFIG_PITCH                        "Pitch"
#define orxSOUND_KZ_CONFIG_VOLUME                       "Volume"
#define orxSOUND_KZ_CONFIG_EMPTY_STREAM                 "empty"
#define orxSOUND_KZ_CONFIG_REFERENCE_DISTANCE           "RefDistance"
#define orxSOUND_KZ_CONFIG_ATTENUATION                  "Attenuation"
#define orxSOUND_KZ_CONFIG_KEEP_IN_CACHE                "KeepInCache"


/***************************************************************************
 * Structure declaration                                                   *
 ***************************************************************************/

/** Sound sample structure
 */
typedef struct __orxSOUND_SAMPLE_t
{
  orxSOUNDSYSTEM_SAMPLE  *pstData;                      /**< Sound data : 4 */
  orxU32                  u32ID;                        /**< Sample ID : 8 */
  orxU32                  u32Counter;                   /**< Reference counter : 12 */  
  orxBOOL                 bInternal;                    /**< Internal : 16 */

} orxSOUND_SAMPLE;

/** Sound structure
 */
struct __orxSOUND_t
{
  orxSTRUCTURE          stStructure;                    /**< Public structure, first structure member : 16 */
  const orxSTRING       zReference;                     /**< Sound reference : 20 */
  orxSOUNDSYSTEM_SOUND *pstData;                        /**< Sound data : 24 */
  orxSOUND_SAMPLE      *pstSample;                      /**< Sound sample : 28 */
};

/** Static structure
 */
typedef struct __orxSOUND_STATIC_t
{
  orxHASHTABLE *pstReferenceTable;                      /**< Reference hash table */
  orxBANK      *pstSampleBank;                          /**< Sample bank */
  orxU32        u32Flags;                               /**< Control flags */

} orxSOUND_STATIC;


/***************************************************************************
 * Static variables                                                        *
 ***************************************************************************/

/** static data
 */
static orxSOUND_STATIC sstSound;


/***************************************************************************
 * Private functions                                                       *
 ***************************************************************************/

/** Loads a sound sample
 * @return orxSOUND_SAMPLE / orxNULL
 */
static orxINLINE orxSOUND_SAMPLE *orxSound_LoadSample(const orxSTRING _zFileName, orxBOOL _bKeepInCache)
{
  orxSOUND_SAMPLE  *pstResult;
  orxU32            u32ID;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);

  /* Gets its ID */
  u32ID = orxString_ToCRC(_zFileName);

  /* Looks for reference */
  pstResult = (orxSOUND_SAMPLE *)orxHashTable_Get(sstSound.pstReferenceTable, u32ID);

  /* Found? */
  if(pstResult != orxNULL)
  {
    /* Increases its reference counter */
    pstResult->u32Counter++;
  }
  else
  {
    /* Allocates a sample */
    pstResult = (orxSOUND_SAMPLE *)orxBank_Allocate(sstSound.pstSampleBank);

    /* Valid? */
    if(pstResult != orxNULL)
    {
      /* Loads its data */
      pstResult->pstData = orxSoundSystem_LoadSample(_zFileName);

      /* Adds it to reference table */
      if((pstResult->pstData != orxNULL)
      && (orxHashTable_Add(sstSound.pstReferenceTable, u32ID, pstResult) != orxSTATUS_FAILURE))
      {
        /* Inits its reference counter */
        pstResult->u32Counter = (_bKeepInCache != orxFALSE) ? 1 : 0;

        /* Stores its ID */
        pstResult->u32ID = u32ID;

        /* Updates its status */
        pstResult->bInternal = orxTRUE;
      }
      else
      {
        /* Deletes it */
        orxBank_Free(sstSound.pstSampleBank, pstResult);

        /* Updates result */
        pstResult = orxNULL;

        /* Logs message */
        orxDEBUG_PRINT(orxDEBUG_LEVEL_SOUND, "Failed to add sound to hashtable.");
      }
    }
  }

  /* Done! */
  return pstResult;  
}

/** Unloads a sound sample
 */
static orxINLINE void orxSound_UnloadSample(orxSOUND_SAMPLE *_pstSample)
{
  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstSample != orxNULL);

  /* Not referenced anymore? */
  if(_pstSample->u32Counter == 0)
  {
    /* Is internal? */
    if(_pstSample->bInternal != orxFALSE)
    {
      /* Unloads its data */
      orxSoundSystem_DeleteSample(_pstSample->pstData);
    }

    /* Removes it from reference table */
    orxHashTable_Remove(sstSound.pstReferenceTable, _pstSample->u32ID);

    /* Deletes it */
    orxBank_Free(sstSound.pstSampleBank, _pstSample);
  }
  else
  {
    /* Updates its reference counter */
    _pstSample->u32Counter--;
  }

  return;
}

/** Unloads all the sound samples
 */
static orxINLINE void orxSound_UnloadAllSample()
{
  orxSOUND_SAMPLE *pstSample;

  /* Gets first sample */
  pstSample = (orxSOUND_SAMPLE *)orxBank_GetNext(sstSound.pstSampleBank, orxNULL);

  /* Non empty? */
  while(pstSample != orxNULL)
  {
    /* Deletes it */
    orxSound_UnloadSample(pstSample);

    /* Gets first sample */
    pstSample = (orxSOUND_SAMPLE *)orxBank_GetNext(sstSound.pstSampleBank, orxNULL);
  }

  return;
}

/** Deletes all the sounds
 */
static orxINLINE void orxSound_DeleteAll()
{
  orxSOUND *pstSound;

  /* Gets first sound */
  pstSound = orxSOUND(orxStructure_GetFirst(orxSTRUCTURE_ID_SOUND));

  /* Non empty? */
  while(pstSound != orxNULL)
  {
    /* Deletes it */
    orxSound_Delete(pstSound);

    /* Gets first sound */
    pstSound = orxSOUND(orxStructure_GetFirst(orxSTRUCTURE_ID_SOUND));
  }

  return;
}


/***************************************************************************
 * Public functions                                                        *
 ***************************************************************************/

/** Sound module setup
 */
void orxFASTCALL orxSound_Setup()
{
  /* Adds module dependencies */
  orxModule_AddDependency(orxMODULE_ID_SOUND, orxMODULE_ID_MEMORY);
  orxModule_AddDependency(orxMODULE_ID_SOUND, orxMODULE_ID_BANK);
  orxModule_AddDependency(orxMODULE_ID_SOUND, orxMODULE_ID_STRUCTURE);
  orxModule_AddDependency(orxMODULE_ID_SOUND, orxMODULE_ID_SOUNDSYSTEM);
  orxModule_AddDependency(orxMODULE_ID_SOUND, orxMODULE_ID_CONFIG);
  orxModule_AddDependency(orxMODULE_ID_SOUND, orxMODULE_ID_CLOCK);

  return;
}

/** Inits the sound module
 * @return orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxSound_Init()
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Not already Initialized? */
  if(!orxFLAG_TEST(sstSound.u32Flags, orxSOUND_KU32_STATIC_FLAG_READY))
  {
    /* Cleans control structure */
    orxMemory_Zero(&sstSound, sizeof(orxSOUND_STATIC));

    /* Creates reference table */
    sstSound.pstReferenceTable = orxHashTable_Create(orxSOUND_KU32_SAMPLE_BANK_SIZE, orxHASHTABLE_KU32_FLAG_NONE, orxMEMORY_TYPE_MAIN);

    /* Valid? */
    if(sstSound.pstReferenceTable != orxNULL)
    {
      /* Creates sample bank */
      sstSound.pstSampleBank = orxBank_Create(orxSOUND_KU32_SAMPLE_BANK_SIZE, sizeof(orxSOUND_SAMPLE), orxBANK_KU32_FLAG_NONE, orxMEMORY_TYPE_MAIN);

      /* Valid? */
      if(sstSound.pstSampleBank != orxNULL)
      {
        /* Registers structure type */
        eResult = orxSTRUCTURE_REGISTER(SOUND, orxSTRUCTURE_STORAGE_TYPE_LINKLIST, orxMEMORY_TYPE_MAIN, orxNULL);
      }
      else
      {
        /* Deletes reference table */
        orxHashTable_Delete(sstSound.pstReferenceTable);

        /* Logs message */
        orxDEBUG_PRINT(orxDEBUG_LEVEL_SOUND, "Failed to create sample bank.");
      }
    }
    else
    {
      /* Logs message */
      orxDEBUG_PRINT(orxDEBUG_LEVEL_SOUND, "Failed to create reference table.");
    }
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_SOUND, "Tried to initialize sound module when it was already initialized.");

    /* Already initialized */
    eResult = orxSTATUS_SUCCESS;
  }

  /* Initialized? */
  if(eResult != orxSTATUS_FAILURE)
  {
    /* Inits Flags */
    orxFLAG_SET(sstSound.u32Flags, orxSOUND_KU32_STATIC_FLAG_READY, orxSOUND_KU32_STATIC_FLAG_NONE);
  }

  /* Done! */
  return eResult;
}

/** Exits from the sound module
 */
void orxFASTCALL orxSound_Exit()
{
  /* Initialized? */
  if(orxFLAG_TEST(sstSound.u32Flags, orxSOUND_KU32_STATIC_FLAG_READY))
  {
    /* Deletes all sounds */
    orxSound_DeleteAll();

    /* Deletes all sound samples */
    orxSound_UnloadAllSample();

    /* Deletes reference table */
    orxHashTable_Delete(sstSound.pstReferenceTable);

    /* Deletes sample bank */
    orxBank_Delete(sstSound.pstSampleBank);

    /* Unregisters structure type */
    orxStructure_Unregister(orxSTRUCTURE_ID_SOUND);

    /* Updates flags */
    orxFLAG_SET(sstSound.u32Flags, orxSOUND_KU32_STATIC_FLAG_NONE, orxSOUND_KU32_STATIC_MASK_ALL);
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_SOUND, "Tried to exit from sound module when it wasn't initialized.");
  }

  return;
}

/** Creates an empty sound
 * @return      Created orxSOUND / orxNULL
 */
orxSOUND *orxFASTCALL orxSound_Create()
{
  orxSOUND *pstResult;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);

  /* Creates sound */
  pstResult = orxSOUND(orxStructure_Create(orxSTRUCTURE_ID_SOUND));

  /* Created? */
  if(pstResult != orxNULL)
  {
    /* Increases counter */
    orxStructure_IncreaseCounter(pstResult);
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_SOUND, "Failed to create structure for sound.");
  }

  /* Done! */
  return pstResult;
}

/** Creates a sound with an empty stream (ie. you'll need to provide actual sound data for each packet sent to the sound card using the event system)
 * @param[in] _u32ChannelNumber Number of channels of the stream
 * @param[in] _u32SampleRate    Sampling rate of the stream (ie. number of frames per second)
 * @param[in] _zName            Name to associate with this sound
 * @return orxSOUNDSYSTEM_SAMPLE / orxNULL
 */
orxSOUND *orxFASTCALL orxSound_CreateWithEmptyStream(orxU32 _u32ChannelNumber, orxU32 _u32SampleRate, const orxSTRING _zName)
{
  orxSOUND *pstResult = orxNULL;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxASSERT(_zName != orxNULL);

  /* Valid name? */
  if(_zName != orxSTRING_EMPTY)
  {
    /* Creates sound */
    pstResult = orxSound_Create();

    /* Valid? */
    if(pstResult != orxNULL)
    {
      /* Creates empty stream */
      pstResult->pstData = orxSoundSystem_CreateStream(_u32ChannelNumber, _u32SampleRate, _zName);

      /* Stores its reference */
      pstResult->zReference = orxString_Duplicate(_zName);

      /* Updates its status */
      orxStructure_SetFlags(pstResult, orxSOUND_KU32_FLAG_HAS_STREAM | orxSOUND_KU32_FLAG_INTERNAL_REFERENCE, orxSOUND_KU32_MASK_ALL);
    }
  }

  /* Done! */
  return pstResult;
}

/** Creates a sound from config
 * @param[in]   _zConfigID    Config ID
 * @ return orxSOUND / orxNULL
 */
orxSOUND *orxFASTCALL orxSound_CreateFromConfig(const orxSTRING _zConfigID)
{
  orxSOUND *pstResult = orxNULL;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxASSERT((_zConfigID != orxNULL) && (_zConfigID != orxSTRING_EMPTY));

  /* Pushes section */
  if((orxConfig_HasSection(_zConfigID) != orxFALSE)
  && (orxConfig_PushSection(_zConfigID) != orxSTATUS_FAILURE))
  {
    /* Is a sound? */
    if(orxConfig_HasValue(orxSOUND_KZ_CONFIG_SOUND) != orxFALSE)
    {
      /* Creates sound */
      pstResult = orxSound_Create();

      /* Valid? */
      if(pstResult != orxNULL)
      {
        const orxSTRING zSoundName;

        /* Gets sound name */
        zSoundName = orxConfig_GetString(orxSOUND_KZ_CONFIG_SOUND);

        /* Loads its corresponding sample */
        pstResult->pstSample = orxSound_LoadSample(zSoundName, orxConfig_GetBool(orxSOUND_KZ_CONFIG_KEEP_IN_CACHE));

        /* Valid? */
        if(pstResult->pstSample != orxNULL)
        {
          /* Creates sound data based on it */
          pstResult->pstData = orxSoundSystem_CreateFromSample(pstResult->pstSample->pstData);

          /* Valid? */
          if(pstResult->pstData != orxNULL)
          {
            /* Stores its reference */
            pstResult->zReference = orxConfig_GetCurrentSection();

            /* Protects it */
            orxConfig_ProtectSection(pstResult->zReference, orxTRUE);

            /* Updates its status */
            orxStructure_SetFlags(pstResult, orxSOUND_KU32_FLAG_HAS_SAMPLE, orxSOUND_KU32_MASK_ALL);
          }
          else
          {
            /* Deletes sample */
            orxSound_UnloadSample(pstResult->pstSample);

            /* Removes its reference */
            pstResult->pstSample = orxNULL;

            /* Updates its status */
            orxStructure_SetFlags(pstResult, orxSOUND_KU32_FLAG_NONE, orxSOUND_KU32_MASK_ALL);
          }
        }
      }
    }
    /* Is a music? */
    else if(orxConfig_HasValue(orxSOUND_KZ_CONFIG_MUSIC) != orxFALSE)
    {
      const orxSTRING zMusicName;

      /* Gets music name */
      zMusicName = orxConfig_GetString(orxSOUND_KZ_CONFIG_MUSIC);

      /* Is empty stream ? */
      if(orxString_ICompare(zMusicName, orxSOUND_KZ_CONFIG_EMPTY_STREAM) == 0)
      {
        /* Creates empty stream */
        pstResult = orxSound_CreateWithEmptyStream(orxSOUND_KZ_STREAM_DEFAULT_CHANNEL_NUMBER, orxSOUND_KZ_STREAM_DEFAULT_SAMPLE_RATE, orxSOUND_KZ_CONFIG_EMPTY_STREAM);
      }
      else
      {
        /* Creates sound */
        pstResult = orxSound_Create();

        /* Valid? */
        if(pstResult != orxNULL)
        {
          /* Loads it */
          pstResult->pstData = orxSoundSystem_CreateStreamFromFile(zMusicName, pstResult->zReference);

          /* Stores its ID */
          pstResult->zReference = orxConfig_GetCurrentSection();

          /* Protects it */
          orxConfig_ProtectSection(pstResult->zReference, orxTRUE);

          /* Updates its status */
          orxStructure_SetFlags(pstResult, orxSOUND_KU32_FLAG_HAS_STREAM, orxSOUND_KU32_MASK_ALL);
        }
      }
    }

    /* Valid sound? */
    if(pstResult != orxNULL)
    {
      /* Valid content? */
      if(pstResult->pstData != orxNULL)
      {
        /* Should loop? */
        if(orxConfig_GetBool(orxSOUND_KZ_CONFIG_LOOP) != orxFALSE)
        {
          /* Updates looping status */
          orxSoundSystem_Loop(pstResult->pstData, orxTRUE);
        }
        else
        {
          /* Updates looping status */
          orxSoundSystem_Loop(pstResult->pstData, orxFALSE);
        }

        /* Has volume? */
        if(orxConfig_HasValue(orxSOUND_KZ_CONFIG_VOLUME) != orxFALSE)
        {
          /* Updates volume */
          orxSoundSystem_SetVolume(pstResult->pstData, orxConfig_GetFloat(orxSOUND_KZ_CONFIG_VOLUME));
        }
        else
        {
          /* Updates volume */
          orxSoundSystem_SetVolume(pstResult->pstData, orxFLOAT_1);
        }

        /* Has pitch? */
        if(orxConfig_HasValue(orxSOUND_KZ_CONFIG_PITCH) != orxFALSE)
        {
          /* Updates volume */
          orxSoundSystem_SetPitch(pstResult->pstData, orxConfig_GetFloat(orxSOUND_KZ_CONFIG_PITCH));
        }
        else
        {
          /* Updates volume */
          orxSoundSystem_SetPitch(pstResult->pstData, orxFLOAT_1);
        }

        /* Has attenuation? */
        if(orxConfig_HasValue(orxSOUND_KZ_CONFIG_ATTENUATION) != orxFALSE)
        {
          /* Updates volume */
          orxSoundSystem_SetAttenuation(pstResult->pstData, orxConfig_GetFloat(orxSOUND_KZ_CONFIG_ATTENUATION));
        }
        else
        {
          /* Updates volume */
          orxSoundSystem_SetAttenuation(pstResult->pstData, orxFLOAT_1);
        }

        /* Has reference distance? */
        if(orxConfig_HasValue(orxSOUND_KZ_CONFIG_REFERENCE_DISTANCE) != orxFALSE)
        {
          /* Updates volume */
          orxSoundSystem_SetReferenceDistance(pstResult->pstData, orxConfig_GetFloat(orxSOUND_KZ_CONFIG_REFERENCE_DISTANCE));
        }
        else
        {
          /* Updates volume */
          orxSoundSystem_SetReferenceDistance(pstResult->pstData, orxFLOAT_1);
        }
      }
      else
      {
        /* Logs message */
        orxDEBUG_PRINT(orxDEBUG_LEVEL_SOUND, "Can't create sound <%s>: invalid content.", _zConfigID);

        /* Deletes it */
        orxSound_Delete(pstResult);

        /* Updates result */
        pstResult = orxNULL;
      }
    }

    /* Pops previous section */
    orxConfig_PopSection();
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_SOUND, "Couldn't find sound section (%s) in config.", _zConfigID);

    /* Updates result */
    pstResult = orxNULL;
  }

  /* Done! */
  return pstResult;
}
/** Deletes a sound
 * @param[in] _pstSound       Concerned Sound
 * @return orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxSound_Delete(orxSOUND *_pstSound)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSound);

  /* Decreases counter */
  orxStructure_DecreaseCounter(_pstSound);

  /* Not referenced? */
  if(orxStructure_GetRefCounter(_pstSound) == 0)
  {
    /* Has an ID? */
    if((_pstSound->zReference != orxNULL)
    && (_pstSound->zReference != orxSTRING_EMPTY))
    {
      /* Stops it */
      orxSound_Stop(_pstSound);

      /* Has internal reference? */
      if(orxStructure_TestFlags(_pstSound, orxSOUND_KU32_FLAG_INTERNAL_REFERENCE) != orxFALSE)
      {
        /* Deletes its reference */
        orxString_Delete((orxSTRING)_pstSound->zReference);
      }
      else
      {
        /* Unprotects it */
        orxConfig_ProtectSection(_pstSound->zReference, orxFALSE);
      }

      /* Has data? */
      if(_pstSound->pstData != orxNULL)
      {
        /* Deletes it */
        orxSoundSystem_Delete(_pstSound->pstData);
      }

      /* Has a referenced sample? */
      if(orxStructure_TestFlags(_pstSound, orxSOUND_KU32_FLAG_HAS_SAMPLE))
      {
        /* Unloads it */
        orxSound_UnloadSample(_pstSound->pstSample);
      }
    }

    /* Deletes structure */
    orxStructure_Delete(_pstSound);
  }
  else
  {
    /* Referenced by others */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Creates a sample
 * @param[in] _u32ChannelNumber Number of channels of the sample
 * @param[in] _u32FrameNumber   Number of frame of the sample (number of "samples" = number of frames * number of channels)
 * @param[in] _u32SampleRate    Sampling rate of the sample (ie. number of frames per second)
 * @param[in] _zName            Name to associate with the sample
 * @return orxSOUNDSYSTEM_SAMPLE / orxNULL
 */
orxSOUNDSYSTEM_SAMPLE *orxFASTCALL orxSound_CreateSample(orxU32 _u32ChannelNumber, orxU32 _u32FrameNumber, orxU32 _u32SampleRate, const orxSTRING _zName)
{
  orxSOUNDSYSTEM_SAMPLE *pstResult = orxNULL;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxASSERT(_zName != orxNULL);

  /* Valid name? */
  if(_zName != orxSTRING_EMPTY)
  {
    orxU32 u32ID;

    /* Gets its ID */
    u32ID = orxString_ToCRC(_zName);

    /* Not already present? */
    if(orxHashTable_Get(sstSound.pstReferenceTable, u32ID) == orxNULL)
    {
      orxSOUNDSYSTEM_SAMPLE *pstSample;

      /* Creates sample */
      pstSample = orxSoundSystem_CreateSample(_u32ChannelNumber, _u32FrameNumber, _u32SampleRate);

      /* Success? */
      if(pstSample != orxNULL)
      {
        orxSOUND_SAMPLE *pstSoundSample;

        /* Creates sound sample */
        pstSoundSample = (orxSOUND_SAMPLE *)orxBank_Allocate(sstSound.pstSampleBank);

        /* Success? */
        if(pstSoundSample != orxNULL)
        {
          /* Inits it */
          pstSoundSample->pstData     = pstSample;
          pstSoundSample->u32Counter  = 0;
          pstSoundSample->u32ID       = u32ID;
          pstSoundSample->bInternal   = orxFALSE;

          /* Stores it */
          orxHashTable_Add(sstSound.pstReferenceTable, u32ID, pstSoundSample);

          /* Updates result */
          pstResult = pstSample;
        }
        else
        {
          /* Logs message */
          orxDEBUG_PRINT(orxDEBUG_LEVEL_SOUND, "Can't create sample <%s>: couldn't allocate internal structure.", _zName);

          /* Deletes sample */
          orxSoundSystem_DeleteSample(pstSample);
        }
      }
    }
    else
    {
      /* Logs message */
      orxDEBUG_PRINT(orxDEBUG_LEVEL_SOUND, "Can't create sample <%s>: a sample with the same name is already present.", _zName);
    }
  }

  /* Done! */
  return pstResult;
}

/** Gets a sample
 * @param[in] _zName            Sample's name
 * @return orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSOUNDSYSTEM_SAMPLE *orxFASTCALL orxSound_GetSample(const orxSTRING _zName)
{
  orxSOUNDSYSTEM_SAMPLE *pstResult = orxNULL;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxASSERT(_zName != orxNULL);

  /* Valid name? */
  if(_zName != orxSTRING_EMPTY)
  {
    orxSOUND_SAMPLE  *pstSoundSample;
    orxU32            u32ID;

    /* Gets its ID */
    u32ID = orxString_ToCRC(_zName);

    /* Gets associated sound sample from table */
    pstSoundSample = (orxSOUND_SAMPLE *)orxHashTable_Get(sstSound.pstReferenceTable, u32ID);

    /* Success? */
    if(pstSoundSample != orxNULL)
    {
      /* Updates result */
      pstResult = pstSoundSample->pstData;
    }
  }

  /* Done! */
  return pstResult;
}

/** Deletes a sample
 * @param[in] _zName            Sample's name
 * @return orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxSound_DeleteSample(const orxSTRING _zName)
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxASSERT(_zName != orxNULL);

  /* Valid name? */
  if(_zName != orxSTRING_EMPTY)
  {
    orxSOUND_SAMPLE  *pstSoundSample;
    orxU32            u32ID;

    /* Gets its ID */
    u32ID = orxString_ToCRC(_zName);

    /* Gets associated sound sample from table */
    pstSoundSample = (orxSOUND_SAMPLE *)orxHashTable_Get(sstSound.pstReferenceTable, u32ID);

    /* Success? */
    if(pstSoundSample != orxNULL)
    {
      /* Not referenced anymore? */
      if(pstSoundSample->u32Counter == 0)
      {
        /* Deletes its data */
        orxSoundSystem_DeleteSample(pstSoundSample->pstData);

        /* Removes it from reference table */
        orxHashTable_Remove(sstSound.pstReferenceTable, pstSoundSample->u32ID);

        /* Deletes it */
        orxBank_Free(sstSound.pstSampleBank, pstSoundSample);

        /* Updates result */
        eResult = orxSTATUS_SUCCESS;
      }
      else
      {
        /* Logs message */
        orxDEBUG_PRINT(orxDEBUG_LEVEL_SOUND, "Can't delete sample <%s>: sample is still in use by at least a sound.", _zName);
      }
    }
  }

  /* Done! */
  return eResult;
}

/** Links a sample
 * @param[in]   _pstSound     Concerned sound
 * @param[in]   _zSampleName  Name of the sample to link (must already be loaded/created)
 * @return orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxSound_LinkSample(orxSOUND *_pstSound, const orxSTRING _zSampleName)
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSound);
  orxASSERT(_zSampleName != orxNULL);

  /* Unlink previous sample if needed */
  orxSound_UnlinkSample(_pstSound);

  /* Has no sample now? */
  if(orxStructure_TestFlags(_pstSound, orxSOUND_KU32_FLAG_HAS_SAMPLE | orxSOUND_KU32_FLAG_HAS_STREAM) == orxFALSE)
  {
    orxSOUND_SAMPLE *pstSoundSample;

    /* Loads corresponding sample */
    pstSoundSample = orxSound_LoadSample(_zSampleName, orxFALSE);

    /* Found? */
    if(pstSoundSample != orxNULL)
    {
      /* Stores it */
      _pstSound->pstSample = pstSoundSample;

      /* Creates sound data based on it */
      _pstSound->pstData = orxSoundSystem_CreateFromSample(pstSoundSample->pstData);

      /* Valid? */
      if(_pstSound->pstData != orxNULL)
      {
        /* Stores its reference */
        _pstSound->zReference = orxString_Duplicate(_zSampleName);

        /* Updates its status */
        orxStructure_SetFlags(_pstSound, orxSOUND_KU32_FLAG_HAS_SAMPLE | orxSOUND_KU32_FLAG_INTERNAL_REFERENCE, orxSOUND_KU32_MASK_ALL);

        /* Updates result */
        eResult = orxSTATUS_SUCCESS;
      }
      else
      {
        /* Unloads sound sample */
        orxSound_UnloadSample(pstSoundSample);

        /* Removes its reference */
        _pstSound->pstSample = orxNULL;

        /* Updates its status */
        orxStructure_SetFlags(_pstSound, orxSOUND_KU32_FLAG_NONE, orxSOUND_KU32_MASK_ALL);
      }
    }
    else
    {
      /* Logs message */
      orxDEBUG_PRINT(orxDEBUG_LEVEL_SOUND, "Can't link sample <%s> to sound: sample not found.", _zSampleName);
    }
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_SOUND, "Can't link sample <%s>: sound is already linked to another sample or a stream.", _zSampleName);
  }

  /* Done! */
  return eResult;
}

/** Unlinks (and deletes if not used anymore) a sample
 * @param[in]   _pstSound     Concerned sound
 * @return orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxSound_UnlinkSample(orxSOUND *_pstSound)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSound);

  /* Has sample? */
  if(orxStructure_TestFlags(_pstSound, orxSOUND_KU32_FLAG_HAS_SAMPLE) != orxFALSE)
  {
    /* Stops it */
    orxSound_Stop(_pstSound);

    /* Has internal reference? */
    if(orxStructure_TestFlags(_pstSound, orxSOUND_KU32_FLAG_INTERNAL_REFERENCE) != orxFALSE)
    {
      /* Deletes its reference */
      orxString_Delete((orxSTRING)_pstSound->zReference);
    }
    else
    {
      /* Unprotects it */
      orxConfig_ProtectSection(_pstSound->zReference, orxFALSE);
    }
    _pstSound->zReference = orxNULL;

    /* Has data? */
    if(_pstSound->pstData != orxNULL)
    {
      /* Deletes it */
      orxSoundSystem_Delete(_pstSound->pstData);
      _pstSound->pstData = orxNULL;
    }

    /* Unloads sound sample */
    orxSound_UnloadSample(_pstSound->pstSample);
    _pstSound->pstSample = orxNULL;

    /* Updates its status */
    orxStructure_SetFlags(_pstSound, orxSOUND_KU32_FLAG_NONE, orxSOUND_KU32_MASK_ALL);
  }
  else
  {
    /* No sample found */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Is a stream (ie. music)?
 * @param[in] _pstSound       Concerned Sound
 * @return orxTRUE / orxFALSE
 */
orxBOOL orxFASTCALL orxSound_IsStream(orxSOUND *_pstSound)
{
  orxBOOL bResult;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSound);

  /* Updates result */
  bResult = orxStructure_TestFlags(_pstSound, orxSOUND_KU32_FLAG_HAS_STREAM);

  /* Done! */
  return bResult;
}

/** Plays sound
 * @param[in] _pstSound       Concerned Sound
 * @return orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxSound_Play(orxSOUND *_pstSound)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSound);

  /* Has sound? */
  if(_pstSound->pstData != orxNULL)
  {
    /* Plays it */
    eResult = orxSoundSystem_Play(_pstSound->pstData);
  }
  else
  {
    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Pauses sound
 * @param[in] _pstSound       Concerned Sound
 * @return orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxSound_Pause(orxSOUND *_pstSound)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSound);

  /* Has sound? */
  if(_pstSound->pstData != orxNULL)
  {
    /* Pauses it */
    eResult = orxSoundSystem_Pause(_pstSound->pstData);
  }
  else
  {
    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Stops sound
 * @param[in] _pstSound       Concerned Sound
 * @return orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxSound_Stop(orxSOUND *_pstSound)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSound);

  /* Has sound? */
  if(_pstSound->pstData != orxNULL)
  {
    /* Stops it */
    eResult = orxSoundSystem_Stop(_pstSound->pstData);
  }
  else
  {
    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Starts recording
 * @param[in] _zName             Name for the recorded sound/file
 * @param[in] _bWriteToFile      Should write to file?
 * @param[in] _u32SampleRate     Sample rate, 0 for default rate (44100Hz)
 * @param[in] _u32ChannelNumber  Channel number, 0 for default mono channel
 * @return orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxSound_StartRecording(const orxCHAR *_zName, orxBOOL _bWriteToFile, orxU32 _u32SampleRate, orxU32 _u32ChannelNumber)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);

  /* Starts recording */
  eResult = orxSoundSystem_StartRecording(_zName, _bWriteToFile, _u32SampleRate, _u32ChannelNumber);

  /* Done! */
  return eResult;
}

/** Stops recording
 * @return orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxSound_StopRecording()
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);

  /* Stops recording */
  eResult = orxSoundSystem_StopRecording();

  /* Done! */
  return eResult;
}

/** Is recording possible on the current system?
 * @return orxTRUE / orxFALSE
 */
orxBOOL orxFASTCALL orxSound_HasRecordingSupport()
{
  orxBOOL bResult;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);

  /* Updates result */
  bResult = orxSoundSystem_HasRecordingSupport();

  /* Done! */
  return bResult;
}

/** Sets sound volume
 * @param[in] _pstSound       Concerned Sound
 * @param[in] _fVolume        Desired volume (0.0 - 1.0)
 * @return orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxSound_SetVolume(orxSOUND *_pstSound, orxFLOAT _fVolume)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSound);

  /* Valid? */
  if(_fVolume >= orxFLOAT_0)
  {
    /* Has sound? */
    if(_pstSound->pstData != orxNULL)
    {
      /* Sets its volume */
      eResult = orxSoundSystem_SetVolume(_pstSound->pstData, _fVolume);
    }
    else
    {
      /* Updates result */
      eResult = orxSTATUS_FAILURE;
    }
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_SOUND, "Volume (%f) for sound <%s> must be >= 0.0.", _fVolume, orxSound_GetName(_pstSound));

    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Sets sound pitch
 * @param[in] _pstSound       Concerned Sound
 * @param[in] _fPitch         Desired pitch
 * @return orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxSound_SetPitch(orxSOUND *_pstSound, orxFLOAT _fPitch)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSound);

  /* Has sound? */
  if(_pstSound->pstData != orxNULL)
  {
    /* Sets its pitch */
    eResult = orxSoundSystem_SetPitch(_pstSound->pstData, _fPitch);
  }
  else
  {
    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Sets sound position
 * @param[in] _pstSound       Concerned Sound
 * @param[in] _pvPosition     Desired position
 * @return orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxSound_SetPosition(orxSOUND *_pstSound, const orxVECTOR *_pvPosition)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSound);
  orxASSERT(_pvPosition != orxNULL);

  /* Has sound? */
  if(_pstSound->pstData != orxNULL)
  {
    /* Sets its position */
    eResult = orxSoundSystem_SetPosition(_pstSound->pstData, _pvPosition);
  }
  else
  {
    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Sets sound attenuation
 * @param[in] _pstSound       Concerned Sound
 * @param[in] _fAttenuation   Desired attenuation
 * @return orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxSound_SetAttenuation(orxSOUND *_pstSound, orxFLOAT _fAttenuation)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSound);

  /* Has sound? */
  if(_pstSound->pstData != orxNULL)
  {
    /* Sets its position */
    eResult = orxSoundSystem_SetAttenuation(_pstSound->pstData, _fAttenuation);
  }
  else
  {
    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Sets sound reference distance
 * @param[in] _pstSound       Concerned Sound
 * @param[in] _fDistance      Within this distance, sound is perceived at its maximum volume
 * @return orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxSound_SetReferenceDistance(orxSOUND *_pstSound, orxFLOAT _fDistance)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSound);

  /* Has sound? */
  if(_pstSound->pstData != orxNULL)
  {
    /* Sets its position */
    eResult = orxSoundSystem_SetReferenceDistance(_pstSound->pstData, _fDistance);
  }
  else
  {
    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Loops sound
 * @param[in] _pstSound       Concerned Sound
 * @param[in] _bLoop          orxTRUE / orxFALSE
 * @return orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxSound_Loop(orxSOUND *_pstSound, orxBOOL _bLoop)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSound);

  /* Has sound? */
  if(_pstSound->pstData != orxNULL)
  {
    /* Sets its looping status */
    eResult = orxSoundSystem_Loop(_pstSound->pstData, _bLoop);
  }
  else
  {
    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Gets sound volume
 * @param[in] _pstSound       Concerned Sound
 * @return orxFLOAT
 */
orxFLOAT orxFASTCALL orxSound_GetVolume(const orxSOUND *_pstSound)
{
  orxFLOAT fResult;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSound);

  /* Has sound? */
  if(_pstSound->pstData != orxNULL)
  {
    /* Updates result */
    fResult = orxSoundSystem_GetVolume(_pstSound->pstData);
  }
  else
  {
    /* Updates result */
    fResult = orxFLOAT_0;
  }

  /* Done! */
  return fResult;
}

/** Gets sound pitch
 * @param[in] _pstSound       Concerned Sound
 * @return orxFLOAT
 */
orxFLOAT orxFASTCALL orxSound_GetPitch(const orxSOUND *_pstSound)
{
  orxFLOAT fResult;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSound);

  /* Has sound? */
  if(_pstSound->pstData != orxNULL)
  {
    /* Updates result */
    fResult = orxSoundSystem_GetPitch(_pstSound->pstData);
  }
  else
  {
    /* Updates result */
    fResult = orxFLOAT_0;
  }

  /* Done! */
  return fResult;
}

/** Gets sound position
 * @param[in]   _pstSound     Concerned Sound
 * @param[out]  _pvPosition   Sound's position
 * @return orxVECTOR / orxNULL
 */
orxVECTOR *orxFASTCALL orxSound_GetPosition(const orxSOUND *_pstSound, orxVECTOR *_pvPosition)
{
  orxVECTOR *pvResult;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSound);
  orxASSERT(_pvPosition != orxNULL);

  /* Has sound? */
  if(_pstSound->pstData != orxNULL)
  {
    /* Updates result */
    pvResult = orxSoundSystem_GetPosition(_pstSound->pstData, _pvPosition);
  }
  else
  {
    /* Updates result */
    pvResult = orxNULL;
  }

  /* Done! */
  return pvResult;
}

/** Gets sound attenuation
 * @param[in] _pstSound       Concerned Sound
 * @return orxFLOAT
 */
orxFLOAT orxFASTCALL orxSound_GetAttenuation(const orxSOUND *_pstSound)
{
  orxFLOAT fResult;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSound);

  /* Has sound? */
  if(_pstSound->pstData != orxNULL)
  {
    /* Updates result */
    fResult = orxSoundSystem_GetAttenuation(_pstSound->pstData);
  }
  else
  {
    /* Updates result */
    fResult = orxFLOAT_0;
  }

  /* Done! */
  return fResult;
}

/** Gets sound reference distance
 * @param[in] _pstSound       Concerned Sound
 * @return orxFLOAT
 */
orxFLOAT orxFASTCALL orxSound_GetReferenceDistance(const orxSOUND *_pstSound)
{
  orxFLOAT fResult;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSound);

  /* Has sound? */
  if(_pstSound->pstData != orxNULL)
  {
    /* Updates result */
    fResult = orxSoundSystem_GetReferenceDistance(_pstSound->pstData);
  }
  else
  {
    /* Updates result */
    fResult = orxFLOAT_0;
  }

  /* Done! */
  return fResult;
}

/** Is sound looping?
 * @param[in] _pstSound       Concerned Sound
 * @return orxTRUE / orxFALSE
 */
orxBOOL orxFASTCALL orxSound_IsLooping(const orxSOUND *_pstSound)
{
  orxBOOL bResult;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSound);

  /* Has sound? */
  if(_pstSound->pstData != orxNULL)
  {
    /* Updates result */
    bResult = orxSoundSystem_IsLooping(_pstSound->pstData);
  }
  else
  {
    /* Updates result */
    bResult = orxFALSE;
  }

  /* Done! */
  return bResult;
}

/** Gets sound duration
 * @param[in] _pstSound       Concerned Sound
 * @return orxFLOAT
 */
orxFLOAT orxFASTCALL orxSound_GetDuration(const orxSOUND *_pstSound)
{
  orxFLOAT fResult;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSound);

  /* Has sound? */
  if(_pstSound->pstData != orxNULL)
  {
    /* Updates result */
    fResult = orxSoundSystem_GetDuration(_pstSound->pstData);
  }
  else
  {
    /* Updates result */
    fResult = orxFLOAT_0;
  }

  /* Done! */
  return fResult;
}

/** Gets sound status
 * @param[in] _pstSound       Concerned Sound
 * @return orxSOUND_STATUS
 */
orxSOUND_STATUS orxFASTCALL orxSound_GetStatus(const orxSOUND *_pstSound)
{
  orxSOUND_STATUS eResult;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSound);

  /* Has sound? */
  if(_pstSound->pstData != orxNULL)
  {
    /* Depending on sound system status */
    switch(orxSoundSystem_GetStatus(_pstSound->pstData))
    {
      case orxSOUNDSYSTEM_STATUS_PLAY:
      {
        /* Updates result */
        eResult = orxSOUND_STATUS_PLAY;

        break;
      }

      case orxSOUNDSYSTEM_STATUS_PAUSE:
      {
        /* Updates result */
        eResult = orxSOUND_STATUS_PAUSE;

        break;
      }

      default:
      case orxSOUNDSYSTEM_STATUS_STOP:
      {
        /* Updates result */
        eResult = orxSOUND_STATUS_STOP;

        break;
      }
    }
  }
  else
  {
    /* Updates result */
    eResult = orxSOUND_STATUS_NONE;
  }

  /* Done! */
  return eResult;
}

/** Gets sound config name
 * @param[in]   _pstSound     Concerned sound
 * @return      orxSTRING / orxSTRING_EMPTY
 */
const orxSTRING orxFASTCALL orxSound_GetName(const orxSOUND *_pstSound)
{
  const orxSTRING zResult;

  /* Checks */
  orxASSERT(sstSound.u32Flags & orxSOUND_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSound);

  /* Updates result */
  zResult = (_pstSound->zReference != orxNULL) ? _pstSound->zReference : orxSTRING_EMPTY;

  /* Done! */
  return zResult;
}
