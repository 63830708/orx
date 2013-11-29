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
 * @file orxThread.c
 * @date 24/11/2013
 * @author iarwain@orx-project.org
 *
 */

#define NO_WIN32_LEAN_AND_MEAN

#include "core/orxThread.h"

#include "debug/orxDebug.h"
#include "memory/orxMemory.h"

#ifdef __orxWINDOWS__

  #include <process.h>

#else /* __orxWINDOWS__ */

  #include <pthread.h>

  #if defined(__orxLINUX__) || defined(__orxRASPBERRY_PI__)

    #include <sched.h>

  #endif /* __orxLINUX__ || __orxRASPBERRY_PI__ */

#endif /* __orxWINDOWS__ */


#if defined(__orxX86_64__) || defined(__orxPPC64__)

  #define orxTHREAD_CAST_HELPER   (orxU64)

#else /* __orxX86_64__ || __orxPPC64__ */

  #define orxTHREAD_CAST_HELPER

#endif /* __orxX86_64__ || __orxPPC64__ */


/** Module flags
 */
#define orxTHREAD_KU32_STATIC_FLAG_NONE               0x00000000  /**< No flags have been set */
#define orxTHREAD_KU32_STATIC_FLAG_READY              0x00000001  /**< Static flag */
#define orxTHREAD_KU32_STATIC_MASK_ALL                0xFFFFFFFF  /**< The module has been initialized */

#define orxTHREAD_KU32_INFO_FLAG_NONE                 0x00000000  /**< No flags have been set */
#define orxTHREAD_KU32_INFO_FLAG_INITIALIZED          0x00000001  /**< Initialized flag */
#define orxTHREAD_KU32_INFO_FLAG_STOP                 0x10000000  /**< Stop flag */
#define orxTHREAD_KU32_INFO_MASK_ALL                  0xFFFFFFFF  /**< The module has been initialized */

#define orxTHREAD_KU32_MAIN_THREAD_ID                 0           /**< Main thread ID */
#define orxTHREAD_KU32_MAX_THREAD_NUMBER              16          /**< Max thread number */


/***************************************************************************
 * Structure declaration                                                   *
 ***************************************************************************/

/* Thread info structure
 */
typedef struct __orxTHREAD_INFO_t
{
#ifdef __orxWINDOWS__

  HANDLE                  hThread;
  orxU32                  u32ThreadID;

#else /* __orxWINDOWS__ */

  pthread_t               hThread;

#endif /* __orxWINDOWS__ */

  orxTHREAD_FUNCTION      pfnRun;
  void                   *pContext;
  orxU32                  u32ParentID;
  orxU32                  u32Flags;

} orxTHREAD_INFO;

/** Static structure
 */
typedef struct __orxTHREAD_STATIC_t
{
  volatile orxTHREAD_INFO astThreadInfoList[orxTHREAD_KU32_MAX_THREAD_NUMBER];
  orxTHREAD_SEMAPHORE     stSemaphore;

  orxU32                  u32Flags;

} orxTHREAD_STATIC;


/***************************************************************************
 * Static variables                                                        *
 ***************************************************************************/

/** Static data
 */
static orxTHREAD_STATIC sstThread;


/***************************************************************************
 * Private functions                                                       *
 ***************************************************************************/

#ifdef __orxWINDOWS__
static unsigned int WINAPI orxThread_Execute(void *_pContext)
#else /* __orxWINDOWS__ */
static void *orxThread_Execute(void *_pContext)
#endif /* __orxWINDOWS__ */
{
  volatile orxTHREAD_INFO  *pstInfo;
  orxSTATUS                 eResult;

  /* Gets thread's info */
  pstInfo = &(sstThread.astThreadInfoList[(orxU32) orxTHREAD_CAST_HELPER _pContext]);

#ifdef __orxWINDOWS__

  /* Stores thread ID */
  pstInfo->u32ThreadID = GetCurrentThreadId();
  orxMEMORY_BARRIER();

#endif /* __orxWINDOWS__ */

  do
  {
    /* Runs thread function */
    eResult = pstInfo->pfnRun(pstInfo->pContext);
  }
  while(eResult != orxSTATUS_FAILURE);

  /* Done! */
  return 0;
}


/***************************************************************************
 * Public functions                                                        *
 ***************************************************************************/

/** Thread module setup
 */
void orxFASTCALL orxThread_Setup()
{
  /* Adds module dependencies */
  orxModule_AddDependency(orxMODULE_ID_THREAD, orxMODULE_ID_MEMORY);

  /* Done! */
  return;
}

/** Init the thread module
 * @return Returns the status of the operation
 */
orxSTATUS orxFASTCALL orxThread_Init()
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Was not already initialized? */
  if(!(sstThread.u32Flags & orxTHREAD_KU32_STATIC_FLAG_READY))
  {
    /* Cleans static controller */
    orxMemory_Zero(&sstThread, sizeof(orxTHREAD_STATIC));

    /* Updates status */
    sstThread.u32Flags |= orxTHREAD_KU32_STATIC_FLAG_READY;

    /* Inits semaphore */
    if(orxThread_InitSemaphore(&(sstThread.stSemaphore), 1) != orxSTATUS_FAILURE)
    {
#if defined(__orxWINDOWS__)

      /* Inits main thread info */
      sstThread.astThreadInfoList[orxTHREAD_KU32_MAIN_THREAD_ID].hThread      = GetCurrentThread();
      sstThread.astThreadInfoList[orxTHREAD_KU32_MAIN_THREAD_ID].u32ThreadID  = GetCurrentThreadId();
      sstThread.astThreadInfoList[orxTHREAD_KU32_MAIN_THREAD_ID].u32Flags     = orxTHREAD_KU32_INFO_FLAG_INITIALIZED;

      /* Sets thread CPU affinity to remain on the same core */
      SetThreadAffinityMask(sstThread.astThreadInfoList[orxTHREAD_KU32_MAIN_THREAD_ID].hThread, 1);

      /* Asks for small time slices */
      timeBeginPeriod(1);

#elif defined(__orxLINUX__) || defined(__orxRASPBERRY_PI__)

      {
        cpu_set_t stSet;

        /* Inits main thread info */
        sstThread.astThreadInfoList[orxTHREAD_KU32_MAIN_THREAD_ID].hThread  = pthread_self();
        sstThread.astThreadInfoList[orxTHREAD_KU32_MAIN_THREAD_ID].u32Flags = orxTHREAD_KU32_INFO_FLAG_INITIALIZED;

        /* Sets CPU affinity mask */
        CPU_ZERO(&stSet);
        CPU_SET(0, &stSet);

        /* Applies it */
        pthread_setaffinity_np(sstThread.astThreadInfoList[orxTHREAD_KU32_MAIN_THREAD_ID].hThread, sizeof(cpu_set_t), &stSet);
      }

#endif
    }
    else
    {
      /* Logs message */
      orxDEBUG_PRINT(orxDEBUG_LEVEL_SYSTEM, "Couldn't initialize internal semaphore.");

      /* Updates status */
      sstThread.u32Flags &= ~orxTHREAD_KU32_STATIC_FLAG_READY;

      /* Updates result */
      eResult = orxSTATUS_FAILURE;
    }
  }

  /* Done! */
  return eResult;
}

/** Exit the thread module
 */
void orxFASTCALL orxThread_Exit()
{
  /* Checks */
  if((sstThread.u32Flags & orxTHREAD_KU32_STATIC_FLAG_READY) == orxTHREAD_KU32_STATIC_FLAG_READY)
  {
#ifdef __orxWINDOWS__

    /* Resets time slices */
    timeEndPeriod(1);

#endif /* __orxWINDOWS__ */

     /* Exits from semaphore */
    orxThread_ExitSemaphore(&(sstThread.stSemaphore));

    /* Cleans static controller */
    orxMemory_Zero(&sstThread, sizeof(orxTHREAD_STATIC));
  }

  /* Done! */
  return;
}

/** Creates a new thread
 * @param[in]   _pfnRun                               Function to run on the new thread
 * @param[in]   _pContext                             Context that will be transmitted to the function when called
 * @return      Thread ID if successful, orxU32_UNDEFINED otherwise
 */
orxU32 orxFASTCALL orxThread_Create(const orxTHREAD_FUNCTION _pfnRun, void *_pContext)
{
  orxU32 u32Index, u32Result = orxU32_UNDEFINED;

  /* Checks */
  orxASSERT((sstThread.u32Flags & orxTHREAD_KU32_STATIC_FLAG_READY) == orxTHREAD_KU32_STATIC_FLAG_READY);
  orxASSERT(_pfnRun != orxNULL);

  /* Waits for semaphore */
  orxThread_WaitSemaphore(&(sstThread.stSemaphore));

  /* For all slots */
  for(u32Index = 0; u32Index < orxTHREAD_KU32_MAX_THREAD_NUMBER; u32Index++)
  {
    /* Is unused? */
    if(!orxFLAG_TEST(sstThread.astThreadInfoList[u32Index].u32Flags, orxTHREAD_KU32_INFO_FLAG_INITIALIZED))
    {
      /* Selects it */
      break;
    }
  }

  /* Found? */
  if(u32Index != orxTHREAD_KU32_MAX_THREAD_NUMBER)
  {
    volatile orxTHREAD_INFO *pstInfo;

    /* Gets its info */
    pstInfo = &(sstThread.astThreadInfoList[u32Index]);

    /* Inits its info */
    pstInfo->pfnRun       = _pfnRun;
    pstInfo->pContext     = _pContext;
    pstInfo->u32ParentID  = orxThread_GetCurrent();
    pstInfo->u32Flags     = orxTHREAD_KU32_INFO_FLAG_INITIALIZED;
    orxMEMORY_BARRIER();

#ifdef __orxWINDOWS__

    /* Creates thread */
    pstInfo->hThread = (HANDLE)_beginthreadex(NULL, 0, orxThread_Execute, (void *)u32Index, 0, NULL);

    /* Success? */
    if(pstInfo->hThread != NULL)
    {
      /* Updates result */
      u32Result = u32Index;
    }
    else

#else /* __orxWINDOWS__ */

    /* Creates thread */
    if(pthread_create((pthread_t *)&(pstInfo->hThread), NULL, orxThread_Execute, (void *) orxTHREAD_CAST_HELPER u32Index) == 0)
    {
      /* Updates result */
      u32Result = u32Index;
    }
    else

#endif /* __orxWINDOWS__ */

    {
      /* Clears its info */
      pstInfo->hThread      = 0;
      pstInfo->pfnRun       = orxNULL;
      pstInfo->pContext     = orxNULL;
      pstInfo->u32ParentID  = 0;
      pstInfo->u32Flags     = orxTHREAD_KU32_INFO_FLAG_NONE;
    }
  }

  /* Signals semaphore */
  orxThread_SignalSemaphore(&(sstThread.stSemaphore));

  /* Done! */
  return u32Result;
}

/** Joins a thread (blocks & waits until the other thread finishes)
 * @param[in]   _u32ThreadID                          ID of the thread for which to wait
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxThread_Join(orxU32 _u32ThreadID)
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Checks */
  orxASSERT((sstThread.u32Flags & orxTHREAD_KU32_STATIC_FLAG_READY) == orxTHREAD_KU32_STATIC_FLAG_READY);
  orxASSERT(_u32ThreadID < orxTHREAD_KU32_MAX_THREAD_NUMBER);

  /* Updates stop flag */
  orxFLAG_SET(sstThread.astThreadInfoList[_u32ThreadID].u32Flags, orxTHREAD_KU32_INFO_FLAG_STOP, orxTHREAD_KU32_INFO_FLAG_NONE);

#ifdef __orxWINDOWS__

  /* Waits for thread */
  WaitForSingleObject(sstThread.astThreadInfoList[_u32ThreadID].hThread, INFINITE);

  /* Clears thread ID */
  sstThread.astThreadInfoList[_u32ThreadID].u32ThreadID = 0;

#else /* __orxWINDOWS__ */

  /* Joins thread */
  pthread_join(sstThread.astThreadInfoList[_u32ThreadID].hThread, NULL);

#endif /* __orxWINDOWS__ */

  /* Clears thread info */
  sstThread.astThreadInfoList[_u32ThreadID].hThread     = 0;
  sstThread.astThreadInfoList[_u32ThreadID].u32ParentID = 0;
  sstThread.astThreadInfoList[_u32ThreadID].pfnRun      = orxNULL;
  sstThread.astThreadInfoList[_u32ThreadID].pContext    = orxNULL;
  orxMEMORY_BARRIER();
  sstThread.astThreadInfoList[_u32ThreadID].u32Flags    = orxTHREAD_KU32_INFO_FLAG_NONE;

  /* Done! */
  return eResult;
}

/** Gets current thread ID
 * @return      Current thread ID
 */
orxU32 orxFASTCALL orxThread_GetCurrent()
{
  orxU32 u32Result = orxU32_UNDEFINED, i;

  /* Checks */
  orxASSERT((sstThread.u32Flags & orxTHREAD_KU32_STATIC_FLAG_READY) == orxTHREAD_KU32_STATIC_FLAG_READY);

#ifdef __orxWINDOWS__

  orxU32 u32ThreadID;

  /* Gets current thread ID */
  u32ThreadID = GetCurrentThreadId();

  /* For all threads */
  for(i = 0; i < orxTHREAD_KU32_MAX_THREAD_NUMBER; i++)
  {
    /* Matches? */
    if(sstThread.astThreadInfoList[i].u32ThreadID == u32ThreadID)
    {
      /* Updates result */
      u32Result = i;
      break;
    }
  }

#else /* __orxWINDOWS__ */

  pthread_t hThread;

  /* Gets current thread */
  hThread = pthread_self();

  /* For all threads */
  for(i = 0; i < orxTHREAD_KU32_MAX_THREAD_NUMBER; i++)
  {
    /* Matches? */
    if(sstThread.astThreadInfoList[i].hThread == hThread)
    {
      /* Updates result */
      u32Result = i;
      break;
    }
  }

#endif /* __orxWINDOWS__ */

  /* Done! */
  return u32Result;
}

/** Yields to other threads
 */
void orxFASTCALL orxThread_Yield()
{
  /* Checks */
  orxASSERT((sstThread.u32Flags & orxTHREAD_KU32_STATIC_FLAG_READY) == orxTHREAD_KU32_STATIC_FLAG_READY);

#ifdef __orxWINDOWS__

  /* Yields */
  Sleep(0);

#else /* __orxWINDOWS__ */

  /* Yields */
  sched_yield();

#endif /* __orxWINDOWS__ */

  /* Done! */
  return;
}

/** Inits a semaphore with the given value
 * @param[in]   _pstSemaphore                         Concerned semaphore
 * @param[in]   _u32Value                             Value with which to init the semaphore
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxThread_InitSemaphore(orxTHREAD_SEMAPHORE *_pstSemaphore, orxU32 _u32Value)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT((sstThread.u32Flags & orxTHREAD_KU32_STATIC_FLAG_READY) == orxTHREAD_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstSemaphore != orxNULL);
  orxASSERT(_u32Value != 0);

#ifdef __orxWINDOWS__

  /* Creates semaphore */
  *_pstSemaphore = CreateSemaphore(NULL, (LONG)_u32Value, (LONG)_u32Value, NULL);

  /* Updates result */
  eResult = (*_pstSemaphore != NULL) ? orxSTATUS_SUCCESS : orxSTATUS_FAILURE;

#else /* __orxWINDOWS__ */

  /* Inits semaphore */
  eResult = (sem_init(_pstSemaphore, 0, _u32Value) != -1) ? orxSTATUS_SUCCESS : orxSTATUS_FAILURE;

#endif /* __orxWINDOWS__ */

  /* Done! */
  return eResult;
}

/** Exits from a semaphore (ie. "deletes" it)
 * @param[in]   _pstSemaphore                         Concerned semaphore
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxThread_ExitSemaphore(orxTHREAD_SEMAPHORE *_pstSemaphore)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT((sstThread.u32Flags & orxTHREAD_KU32_STATIC_FLAG_READY) == orxTHREAD_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstSemaphore != orxNULL);

#ifdef __orxWINDOWS__

  /* Closes the semaphore */
  eResult = (CloseHandle(*_pstSemaphore) != 0) ? orxSTATUS_SUCCESS : orxSTATUS_FAILURE;

#else /* __orxWINDOWS__ */

  /* Destroys semaphore */
  eResult = (sem_destroy(_pstSemaphore) != -1) ? orxSTATUS_SUCCESS : orxSTATUS_FAILURE;

#endif /* __orxWINDOWS__ */

  /* Done! */
  return eResult;
}

/** Waits for a semaphore
 * @param[in]   _pstSemaphore                         Concerned semaphore
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxThread_WaitSemaphore(orxTHREAD_SEMAPHORE *_pstSemaphore)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT((sstThread.u32Flags & orxTHREAD_KU32_STATIC_FLAG_READY) == orxTHREAD_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstSemaphore != orxNULL);

#ifdef __orxWINDOWS__

  /* Waits for semaphore */
  eResult = (WaitForSingleObject(*_pstSemaphore, INFINITE) != WAIT_FAILED) ? orxSTATUS_SUCCESS : orxSTATUS_FAILURE;

#else /* __orxWINDOWS__ */

  /* Waits for semaphore */
  eResult = (sem_wait(_pstSemaphore) != -1) ? orxSTATUS_SUCCESS : orxSTATUS_FAILURE;

#endif /* __orxWINDOWS__ */

  /* Done! */
  return eResult;
}

/** Signals a semaphore
 * @param[in]   _pstSemaphore                         Concerned semaphore
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxThread_SignalSemaphore(orxTHREAD_SEMAPHORE *_pstSemaphore)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT((sstThread.u32Flags & orxTHREAD_KU32_STATIC_FLAG_READY) == orxTHREAD_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstSemaphore != orxNULL);

#ifdef __orxWINDOWS__

  /* Releases semaphore */
  eResult = (ReleaseSemaphore(*_pstSemaphore, 1, NULL) != 0) ? orxSTATUS_SUCCESS : orxSTATUS_FAILURE;

#else /* __orxWINDOWS__ */

  /* Posts to a semaphore */
  eResult = (sem_post(_pstSemaphore) != -1) ? orxSTATUS_SUCCESS : orxSTATUS_FAILURE;

#endif /* __orxWINDOWS__ */

  /* Done! */
  return eResult;
}
