/* Orx - Portable Game Engine
 *
 * Copyright (c) 2008-2014 Orx-Project
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
 * @file orxAndroid.h
 * @date 26/06/2011
 * @author iarwain@orx-project.org
 *
 * @todo
 */

/**
 * @addtogroup orxAndroid
 *
 * Android support module
 *
 * @{
 */


#ifndef _orxANDROID_H_
#define _orxANDROID_H_

#include <jni.h>
#include <pthread.h>
#include <stdlib.h>

#include <android/native_window.h>

#ifdef __orxANDROID_NATIVE__

#include <android_native_app_glue.h>

#endif

#if defined(__cplusplus)
extern "C"
{
#endif

#ifdef __orxANDROID__

enum {
    /**
     * Looper data ID of commands coming from the app's main thread, which
     * is returned as an identifier from ALooper_pollOnce().  The data for this
     * identifier is a pointer to an android_poll_source structure.
     * These can be retrieved and processed with android_app_read_cmd()
     * and android_app_exec_cmd().
     */
    LOOPER_ID_MAIN = 1,
    LOOPER_ID_SENSOR = 2,
    LOOPER_ID_KEY_EVENT = 3,
    LOOPER_ID_TOUCH_EVENT = 4,
    LOOPER_ID_USER = 5
};

enum {
    APP_CMD_PAUSE,
    APP_CMD_RESUME,
    APP_CMD_SURFACE_DESTROYED,
    APP_CMD_SURFACE_CREATED,
    APP_CMD_SURFACE_CHANGED,
    APP_CMD_QUIT,
    APP_CMD_FOCUS_LOST,
    APP_CMD_FOCUS_GAINED
};

typedef struct __orxANDROID_TOUCH_EVENT_t {
        orxU32   u32ID;
        orxFLOAT fX;
        orxFLOAT fY;
        orxU32   u32Action;

} orxANDROID_TOUCH_EVENT;

#endif /*__orxANDROID__*/

typedef struct __orxANDROID_KEY_EVENT_t {
#ifdef __orxANDROID__
       orxU32 u32Action;
#endif
       orxU32 u32KeyCode;
       orxU32 u32Unicode;

} orxANDROID_KEY_EVENT;

typedef struct __orxANDROID_SURFACE_CHANGED_EVENT_t {
        orxU32   u32Width;
        orxU32   u32Height;

} orxANDROID_SURFACE_CHANGED_EVENT;

ANativeWindow * orxAndroid_GetNativeWindow();

/**
  Get the internal storage path
  */
const char * orxAndroid_GetInternalStoragePath();

orxU32 orxAndroid_JNI_GetRotation();
void   orxAndroid_JNI_SetWindowFormat(orxU32 format);

/**
  Register APK resources IO
  */
orxSTATUS orxAndroid_RegisterAPKResource();

int orxAndroid_JNI_SetupThread();
void orxAndroid_PumpEvents();
void *orxAndroid_GetJNIEnv();
jobject orxAndroid_GetActivity();

#ifdef __orxANDROID_NATIVE__
ANativeActivity* orxAndroid_GetNativeActivity();
#endif

#if defined(__cplusplus)
}
#endif

#define orxANDROID_EVENT_TYPE_KEYBOARD       (orxEVENT_TYPE)(orxEVENT_TYPE_FIRST_RESERVED + 0)
#define orxANDROID_EVENT_KEYBOARD_DOWN       0
#define orxANDROID_EVENT_KEYBOARD_UP         1

#define orxANDROID_EVENT_TYPE_SURFACE        (orxEVENT_TYPE)(orxEVENT_TYPE_FIRST_RESERVED + 1)
#define orxANDROID_EVENT_SURFACE_DESTROYED   0
#define orxANDROID_EVENT_SURFACE_CREATED     1
#define orxANDROID_EVENT_SURFACE_CHANGED     2

#endif /* _orxANDROID_H_ */

/** @} */
