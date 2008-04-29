/**
 * @file orxScript.c
 * 
 * Module for core script extension management.
 */ 
 
 /***************************************************************************
 orxScript.c
 Module for core script extension management.
 
 begin                : 03/05/2005
 author               : (C) Arcallians
 email                : bestel@arcallians.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License           *
 *   as published by the Free Software Foundation; either version 2.1      *
 *   of the License, or (at your option) any later version.                *
 *                                                                         *
 ***************************************************************************/

#include "script/orxScript.h"
#include "debug/orxDebug.h"
#include "memory/orxMemory.h"
#include "utils/orxString.h"
#include <stdio.h>

/* Maximum number of registered functions */
#define SCRIPT_MAX_FUNCTION 1024

/** Static structure
 */
typedef struct __orxSCRIPT_STATIC_t
{
  orxSCRIPT_FUNCTION astFunctions[SCRIPT_MAX_FUNCTION]; /**< List of registered function info */
  orxS32 s32Count;                                      /**< registered function count */
} orxSCRIPT_STATIC;

/***************************************************************************
 * Static variables                                                        *
 ***************************************************************************/

/** Static data
 */
orxSTATIC orxSCRIPT_STATIC sstScript;

/********************
 *  Plugin Related  *
 ********************/

/***************************************************************************
 orxScript_Setup
 Script module setup.

 returns: nothing
 ***************************************************************************/
orxVOID orxScript_Setup()
{
  /* Adds module dependencies */
  orxModule_AddDependency(orxMODULE_ID_SCRIPT, orxMODULE_ID_PLUGIN);
  orxModule_AddDependency(orxMODULE_ID_SCRIPT, orxMODULE_ID_MEMORY);

  return;
}

/** Gets the type from a string
 * @param _zValue (IN) String value
 * @return Returns the type associated to the string
 */
orxSCRIPT_TYPE orxScript_GetTypeValue(orxCONST orxSTRING _zValue)
{
  /* Default return value */
  orxSCRIPT_TYPE eRet = orxSCRIPT_TYPE_NONE;
  
  /* orxVOID ? */
  if(orxString_Compare(_zValue, "orxVOID") == 0)
  {
    eRet = orxSCRIPT_TYPE_VOID;
  }
  /* orxS32 */
  else if(orxString_Compare(_zValue, "orxS32") == 0)
  {
    eRet = orxSCRIPT_TYPE_S32;
  }
  /* orxFLOAT */
  else if(orxString_Compare(_zValue, "orxFLOAT") == 0)
  {
    eRet = orxSCRIPT_TYPE_FLOAT;
  }
  /* orxSTRING */
  else if(orxString_Compare(_zValue, "orxPOINTER") == 0)
  {
    eRet = orxSCRIPT_TYPE_POINTER;
  }
  else
  {
    eRet = orxSCRIPT_TYPE_NONE;
  }
  
  /* Returns type */
  return eRet;
}

/** Initialize the Script Module
 * @return Returns the status of the operation
 */
orxSTATUS orxScript_Init()
{
  /** Inits parameters */
  orxMemory_Set(&sstScript, 0, sizeof(orxSCRIPT_STATIC));
  
  /** Calls plugin init */
  return orxScript_PluginInit();
}

/** Uninitialize the Script Module
 */
orxVOID orxScript_Exit()
{
  /** Calls plugin exit */
  orxScript_PluginExit();

  /** Inits parameters */
  orxMemory_Set(&sstScript, 0, sizeof(orxSCRIPT_STATIC));
}

/** Function to register a new function in the system. 
 * This function sotre the parameters in the global list, gets the new entry index and call the plugin register function
 * @param _zFunctionName  (IN)  String value of the function
 * @param _pfnFunction    (IN)  Function pointer
 * @param _zParamTypes    (IN)  List of type for the function (the first parameter is the return type)
 */
orxSTATUS orxScript_RegisterFunctionGlobal(orxCONST orxSTRING _zFunctionName, orxSCRIPT_FUNCTION_PTR _pfnFunction, orxCONST orxSTRING _zParamTypes)
{
  /* Default return value */
  orxSTATUS eStatus = orxSTATUS_FAILURE;

  /* Checks parameters */
  orxASSERT(_zFunctionName != orxNULL);
  orxASSERT(_pfnFunction != orxNULL);
  orxASSERT(_zParamTypes != orxNULL);
  
  /* Gets index */
  orxS32 s32Index = sstScript.s32Count;
  
  /* Stores function name */
  sstScript.astFunctions[s32Index].zFunctionName = _zFunctionName;
  
  /* Stores function pointer */
  sstScript.astFunctions[s32Index].pfnFunction = _pfnFunction;
  
  /* Parse parameters type */
  orxS32 s32Count = 0;
  orxBOOL bEnd = orxFALSE;
  orxS32 s32Previous = 0;
  while(!bEnd)
  {
    orxSCRIPT_TYPE eType;
    orxCHAR zParam[64];
    
    /* Gets next index for comma ? */
    orxS32 s32Current = orxString_SearchCharIndex(_zFunctionName, ',', s32Previous);
    
    /* Found ? */
    if(s32Current >= 0)
    {
      bEnd = orxFALSE;
    }
    else
    {
      bEnd = orxTRUE;
    }
    
    /* Not end ? */
    if(!bEnd)
    {
      orxString_NCopy(zParam, _zFunctionName + s32Previous, s32Current - s32Previous);
    }
    else
    {
      /* Last parameters */
      orxString_NCopy(zParam, _zFunctionName + s32Previous, orxString_GetLength(_zFunctionName) - s32Previous);
    }
    
    /* Gets type */
    eType = orxScript_GetTypeValue(zParam);
    
    /* Checks the type of the string value */
    sstScript.astFunctions[s32Index].aeParamsType[s32Count] = eType;
    
    /* Computes the stack size */
    // TODO :
    // sstScript.astFunctions[s32Index].s32StackSize += orxScript_GetTypeSize(eType);

    /* Increase param count */
    s32Count++;
    
    /* Updates index */
    s32Previous = s32Current + 1;
  }
    
  /* Stores the parameter count */
  sstScript.astFunctions[s32Index].s32NbParams = s32Count;
  
  /* Register function in the plugin */
  eStatus = orxScript_RegisterFunction(s32Index);
  
  /* Increase the global count */
  sstScript.s32Count++;
  
  /* Returns regisration status */
  return eStatus;
}

void orxScript_Test(float j)
{
  printf("value = %f", j);
}

/** Executes a callback with input parameters, returns the result of the called function in the output
 * @param _pstFunctionInfo   (IN)  Function info
 * @param _pstInputValues    (IN)  Array of input parameters
 * @param _pstOutputValue    (OUT) Returned value from the function
 * @return Return orxSTATUS_SUCCESS if the function has been correctly executed, else orxSTATUS_FAILURE
 */
orxSTATUS orxScript_ExecuteFunction(orxSCRIPT_FUNCTION *_pstFunctionInfo, orxCONST orxSCRIPT_PARAM *_pstInputValues, orxSCRIPT_PARAM *_pstOutputValue)
{
  /* Default return value */
  orxSTATUS eRet = orxSTATUS_FAILURE;
  
  /* Checks parameters */
  orxASSERT(_pstFunctionInfo != orxNULL);
  orxASSERT(_pstInputValues != orxNULL);
  orxASSERT(_pstOutputValue != orxNULL);
  
  /* Sets the initial stack size */
  orxS32 s32StackPointer = 12;

  /* Computes the total stack size */
  s32StackPointer += _pstFunctionInfo->s32StackSize;
  
  /* Gets the stack size */

#ifdef __orxGCC__
    /* GCC Use old AT&T convention to write ASM :( */
    asm("#esp");
    
    /* Decrease the stack pointer */
    asm("subl -8(%ebp),   %esp");
    
    /* Stores the current ESP (stack pointer address) into s32StackPointer */
    asm("movl    %esp, -8(%ebp)");
#else
    /* Use Intel instruction */
    asm("");
#endif
  
  orxS32 s32Index;
  
  /* Go through the list of inputs */
  for(s32Index = 0; s32Index < _pstFunctionInfo->s32NbParams - 1; s32Index++)
  {

    /* Switch on type */
    switch(_pstFunctionInfo->aeParamsType[s32Index])
    {
      case orxSCRIPT_TYPE_POINTER:
#ifdef __orxGCC__
    /* GCC Use old AT&T convention to write ASM :( */
    
    // Push elements on the stack
    asm("#POINTER");
#else
    /* Use Intel instruction */
    asm("");
#endif

        break;

      case orxSCRIPT_TYPE_S32:
      case orxSCRIPT_TYPE_FLOAT:

#ifdef __orxGCC__
        /* GCC Use old AT&T convention to write ASM :( */
    
        // Push elements on the stack
        asm("#S32 / FLOAT");
        
        /* ASM push the following value on the stack
         * => (orxS32)  *(orxS32 *)  (_pstInputValues[s32Index].pValue);
         * OR
         * => (orxFLOAT)*(orxFLOAT *)(_pstInputValues[s32Index].pValue);
         */
    
        /* Computes the ESP offset (increase the stack offset of 4 bytes) */
        asm("movl  -8(%ebp),   %eax");
        asm("addl        $4,   %eax");
    
        /* Stores the ESP offset into s32StackPointer */
        asm("movl     %eax, -8(%ebp)");
    
        /* Stores the ESP offset into a temporary register */
        asm("movl     %eax,    %ebx");
    
        /* Gets the value from the input array and manage memory indirection */
        asm("movl -12(%ebp),   %eax");
        asm("sall        $2,   %eax");
        asm("addl  12(%ebp),   %eax");
        asm("movl    (%eax),   %eax");
        asm("movl    (%eax),   %eax");
    
        /* Push the input value on the stack */
        asm("movl     %eax,   (%ebx)");

#else
    /* Use Intel instruction */
    asm("");
#endif

        break;

      case orxSCRIPT_TYPE_DOUBLE:
#ifdef __orxGCC__
    /* GCC Use old AT&T convention to write ASM :( */
    
    // Push elements on the stack
    asm("#DOUBLE");
#else
    /* Use Intel instruction */
    asm("");
#endif

        break;
        
      default:
        break;
    }
    
    // Call the function
    //_pstFunctionInfo->pfnFunction();
    orxScript_Test(5.0f);

    // Return value ?
    if(_pstFunctionInfo->aeParamsType[0] != orxSCRIPT_TYPE_VOID)
    {
      _pstOutputValue[0].eType = _pstFunctionInfo->aeParamsType[0];
      
      /* Gets the result pointer */
      //orxVOID *pValue;
      //pValue = (orxVOID *)_pstOutputValue[0].ps32Value;
      
#ifdef __orxGCC__
    /* GCC Use old AT&T convention to write ASM :( */
    
    // Gets function result from the stack and store it in pResult
    asm("");
#else
    /* Use Intel instruction */

    // Gets function result from the stack and store it in pResult
    asm("");
#endif
    }
  }
  
  /* Returns execution status */
  return eRet;
}

/** Returns function info from an index or orxNULL if not found
 * @param _s32Index (IN)  parameter index
 * @return  Returns the pointer on the function info if valid index, else returns orxNULL.
 */
orxSCRIPT_FUNCTION *orxScript_GetFunctionInfo(orxS32 _s32Index)
{
  /* Default return value */
  orxSCRIPT_FUNCTION *pstRet = orxNULL;

  /* Valid index ? */
  if(_s32Index < sstScript.s32Count)
  {
    /* Gets pointer on info */
    pstRet = &(sstScript.astFunctions[_s32Index]);
  }
  
  /* Returns pointer on function info */
  return pstRet;
}


/* *** Core function info array *** */

orxPLUGIN_BEGIN_CORE_FUNCTION_ARRAY(SCRIPT)
orxPLUGIN_ADD_CORE_FUNCTION_ARRAY(SCRIPT, INIT, orxScript_PluginInit)
orxPLUGIN_ADD_CORE_FUNCTION_ARRAY(SCRIPT, EXIT, orxScript_PluginExit)
orxPLUGIN_ADD_CORE_FUNCTION_ARRAY(SCRIPT, RUN_FILE, orxScript_RunFile)
orxPLUGIN_ADD_CORE_FUNCTION_ARRAY(SCRIPT, RUN_STRING, orxScript_RunString)
orxPLUGIN_ADD_CORE_FUNCTION_ARRAY(SCRIPT, GET_TYPE, orxScript_GetType)
orxPLUGIN_ADD_CORE_FUNCTION_ARRAY(SCRIPT, GET_S32_VALUE, orxScript_GetS32Value)
orxPLUGIN_ADD_CORE_FUNCTION_ARRAY(SCRIPT, GET_FLOAT_VALUE, orxScript_GetFloatValue)
orxPLUGIN_ADD_CORE_FUNCTION_ARRAY(SCRIPT, GET_STRING_VALUE, orxScript_GetStringValue)
orxPLUGIN_ADD_CORE_FUNCTION_ARRAY(SCRIPT, SET_S32_VALUE, orxScript_SetS32Value)
orxPLUGIN_ADD_CORE_FUNCTION_ARRAY(SCRIPT, SET_FLOAT_VALUE, orxScript_SetFloatValue)
orxPLUGIN_ADD_CORE_FUNCTION_ARRAY(SCRIPT, SET_STRING_VALUE, orxScript_SetStringValue)
orxPLUGIN_ADD_CORE_FUNCTION_ARRAY(SCRIPT, REGISTER_FUNCTION, orxScript_RegisterFunction)
orxPLUGIN_END_CORE_FUNCTION_ARRAY(SCRIPT)


/* *** Core function definitions *** */

orxPLUGIN_DEFINE_CORE_FUNCTION(orxScript_PluginInit, orxSTATUS);
orxPLUGIN_DEFINE_CORE_FUNCTION(orxScript_PluginExit, orxVOID);
orxPLUGIN_DEFINE_CORE_FUNCTION(orxScript_RunFile, orxSTATUS, orxCONST orxSTRING);
orxPLUGIN_DEFINE_CORE_FUNCTION(orxScript_RunString, orxSTATUS, orxCONST orxSTRING);
orxPLUGIN_DEFINE_CORE_FUNCTION(orxScript_GetType, orxSCRIPT_TYPE, orxCONST orxSTRING);
orxPLUGIN_DEFINE_CORE_FUNCTION(orxScript_GetS32Value, orxSTATUS, orxCONST orxSTRING, orxS32*);
orxPLUGIN_DEFINE_CORE_FUNCTION(orxScript_GetFloatValue, orxSTATUS, orxCONST orxSTRING, orxFLOAT*);
orxPLUGIN_DEFINE_CORE_FUNCTION(orxScript_GetStringValue, orxSTATUS, orxCONST orxSTRING, orxSTRING*);
orxPLUGIN_DEFINE_CORE_FUNCTION(orxScript_SetS32Value, orxSTATUS, orxCONST orxSTRING, orxS32);
orxPLUGIN_DEFINE_CORE_FUNCTION(orxScript_SetFloatValue, orxSTATUS, orxCONST orxSTRING, orxFLOAT);
orxPLUGIN_DEFINE_CORE_FUNCTION(orxScript_SetStringValue, orxSTATUS, orxCONST orxSTRING, orxSTRING);
orxPLUGIN_DEFINE_CORE_FUNCTION(orxScript_RegisterFunction, orxSTATUS, orxS32);


/***************************************************************************
 * Structure declaration                                                   *
 ***************************************************************************/

/***************************************************************************
 * Module global variable                                                  *
 ***************************************************************************/

/***************************************************************************
 * Public functions                                                        *
 ***************************************************************************/
