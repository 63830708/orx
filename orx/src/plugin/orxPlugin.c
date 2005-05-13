/***************************************************************************
 orxPlugin.c
 Plugin module
 
 begin                : 04/09/2002
 author               : (C) Gdp
 email                : david.anderson@calixo.net
                      : iarwain@ifrance.com       (2003->)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "plugin/orxPlugin.h"

#include "debug/orxDebug.h"
#include "memory/orxMemory.h"
#include "plugin/orxPluginUser.h"
#include "plugin/orxPluginCore.h"
#include "utils/utils.h"

#include "msg/msg_plugin.h"


#ifdef __orxLINUX__

  #include <dlfcn.h>

#else /* __orxLINUX__ */

  #ifdef __orxWINDOWS__

    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>

  #endif /* __orxWINDOWS__ */

#endif /* __orxLINUX__ */


/*
 * Platform dependant type & function defines
 */

/* WINDOWS */
#ifdef __orxWINDOWS__

typedef HMODULE                                         orxSYSPLUGIN_HANDLE;

#define orxPLUGIN_OPEN(PLUGIN)                          LoadLibrary(PLUGIN)
#define orxPLUGIN_GET_SYMBOL_ADDRESS(PLUGIN, SYMBOL)    GetProcAddress(PLUGIN, SYMBOL)
#define orxPLUGIN_CLOSE(PLUGIN)                         FreeLibrary(PLUGIN)

orxSTATIC orxCONST orxSTRING                            szPluginLibraryExt = "dll";

/* OTHERS */
#else

typedef orxVOID *                                       orxSYSPLUGIN_HANDLE;

#define orxPLUGIN_OPEN(PLUGIN)                          dlopen(PLUGIN, RTLD_LAZY)
#define orxPLUGIN_GET_SYMBOL_ADDRESS(PLUGIN, SYMBOL)    dlsym(PLUGIN, SYMBOL)
#define orxPLUGIN_CLOSE(PLUGIN)                         dlclose(PLUGIN)

orxSTATIC orxCONST orxSTRING                            szPluginLibraryExt = "so";

#endif



/*
 * Platform independant defines
 */

#define orxPLUGIN_KU32_FLAG_NONE                0x00000000
#define orxPLUGIN_KU32_FLAG_READY               0x00000001

/*
 * Information structure on a plugin function
 */
typedef struct __orxPLUGIN_FUNCTION_INFO_t
{
  orxPLUGIN_FUNCTION pfnFunction;                       /**< Function Address : 4 */
  orxCHAR zFunctionArgs[orxPLUGIN_KU32_FUNCTION_ARG_SIZE];/**< Function Argument Types : 132 */

  orxCHAR zFunctionName[orxPLUGIN_KU32_NAME_SIZE];      /**< Function Name : 164 */
  orxPLUGIN_FUNCTION_ID eFunctionID;                    /**< Function ID : 168 */

  /* 8 extra bytes of padding : 176 */
  orxU8 au8Unused[8];

} orxPLUGIN_FUNCTION_INFO;


/*
 * Information structure on a plugin
 */
typedef struct __orxPLUGIN_INFO_t
{
  orxSYSPLUGIN_HANDLE hSysPlugin;                             /**< Plugin handle : 4 */

  orxCHAR zPluginName[orxPLUGIN_KU32_NAME_SIZE];        /**< Plugin name : 36 */
  orxHANDLE hPluginHandle;                              /**< Plugin handle : 40 */

  map p_function_map;                                   /**< Plugin Function List : 44 */

  /* 4 extra bytes of padding : 48 */
  orxU8 au8Unused[4];

} orxPLUGIN_INFO;


/*
 * Static structure
 */
typedef struct __orxPLUGIN_STATIC_t
{
  /* Control flags */
  orxU32 u32Flags;

} orxPLUGIN_STATIC;

/*
 * Static data
 */
orxSTATIC orxPLUGIN_STATIC sstPlugin;


/*
 * Static information structures
 */
orxSTATIC orxPLUGIN_CORE_FUNCTION orxCONST *sapst_function[orxPLUGIN_CORE_ID_NUMBER];
orxSTATIC orxS32 si_function_number[orxPLUGIN_CORE_ID_NUMBER];

orxSTATIC map sp_plugin_map = orxNULL;


/***************************************************************************
 ***************************************************************************
 ******                       LOCAL FUNCTIONS                         ******
 ***************************************************************************
 ***************************************************************************/

/***************************************************************************
 key_create
 
 This function creates a key for plugin/function maps. 
 Returns created key.
 ***************************************************************************/
orxU8 *key_create(orxHANDLE _u32_id, orxCONST orxSTRING _z_name)
{
  orxU8 *puc_key = orxNULL;
  orxS32 i_shift = sizeof(orxU32);

  /* Allocate memory for key */
  puc_key = (orxU8 *)orxMemory_Allocate(i_shift + orxPLUGIN_KU32_NAME_SIZE, orxMEMORY_TYPE_MAIN);

  if(puc_key != orxNULL)
  {
    *puc_key = *((orxU8 *)&_u32_id);

    strcpy(puc_key + i_shift, _z_name);
  }

  return puc_key;
}

/***************************************************************************
 key_delete
 
 This function a key. 
 Returns nothing.
 ***************************************************************************/
orxVOID key_delete(orxU8 *_puc_key)
{
  if(_puc_key != orxNULL)
  {
    orxMemory_Free(_puc_key);
  }
}

/***************************************************************************
 key_compare
 
 This function compares a key with plugin info. 
 Returns orxTRUE/FALSE based upon comparison result.
 ***************************************************************************/
orxBOOL key_compare(orxU8 *_puc_key1, orxU8 *_puc_key2)
{
  orxBOOL b_result;
  orxS32 i_shift = sizeof(orxU32);

  /* ID Comparison */
  b_result = ((orxU32) *_puc_key1 == (orxU32) *_puc_key2) ?
             orxTRUE :
             orxFALSE;

  /* Name Compare ? */
  if(b_result == orxFALSE)
  {
    b_result = (strcmp((_puc_key1 + i_shift), (_puc_key2 + i_shift)) == 0) ?
              orxTRUE :
              orxFALSE;
  }

  return b_result;
}

/***************************************************************************
 function_cell_create
 
 This function creates & initiates a function_info cell. 
 Returns created cell.
 ***************************************************************************/
orxPLUGIN_FUNCTION_INFO *function_cell_create()
{
  orxPLUGIN_FUNCTION_INFO *pstCell;

  /* Creates a function info cell */
  pstCell = (orxPLUGIN_FUNCTION_INFO *) orxMemory_Allocate(sizeof(orxPLUGIN_FUNCTION_INFO), orxMEMORY_TYPE_MAIN);

  /* Initiates it */
  pstCell->pfnFunction    = orxNULL;
  pstCell->eFunctionID  = orxPLUGIN_FUNCTION_ID_NONE;

  return pstCell;
}

/***************************************************************************
 plugin_cell_create
 
 This function creates & initiates a plugin_info cell. 
 Returns created cell.
 ***************************************************************************/
orxPLUGIN_INFO *plugin_cell_create()
{
  orxPLUGIN_INFO *pstCell;

  /* Creates a plugin info cell */
  pstCell = (orxPLUGIN_INFO *) orxMemory_Allocate(sizeof(orxPLUGIN_INFO), orxMEMORY_TYPE_MAIN);

  /* Initiates it */
  pstCell->hPluginHandle   = orxNULL;
  pstCell->hPluginHandle     = orxHANDLE_Undefined;
  pstCell->p_function_map    = 
    (orxPLUGIN_FUNCTION_INFO *)map_create(orxPLUGIN_KU32_NAME_SIZE,
                                          sizeof(orxPLUGIN_FUNCTION_INFO),
                                          &key_compare);

  return pstCell;
}

/***************************************************************************
 function_cell_delete

 This function deletes a function_info cell

 Returns nothing.
 ***************************************************************************/
orxVOID function_cell_delete(orxPLUGIN_FUNCTION_INFO *_pstCell)
{
  /* Is cell valid? */
  if(_pstCell != orxNULL)
  {
    /* Delete cell */
    orxMemory_Free(_pstCell);
  }
      
  /* Done */
  return;
}

/***************************************************************************
 plugin_cell_delete
 
 This function shuts down the given plugin.
 It should be called from orxPlugin_Unload or orxPlugin_UnloadByName.
 
 Returns nothing.
 ***************************************************************************/
orxVOID plugin_cell_delete(orxPLUGIN_INFO *_pstCell)
{
  orxU8 *puc_key;

  /* Is cell valid? */
  if(_pstCell != orxNULL)
  {
    /* Unload plugin */
    if(_pstCell->hPluginHandle != orxNULL)
    {
      orxPLUGIN_CLOSE(_pstCell->hPluginHandle);
    }

    /* Free function map */
    if(_pstCell->p_function_map != orxNULL)
    {
      map_destroy(_pstCell->p_function_map);
    }

    /* Compute a complete key */
    puc_key = key_create(_pstCell->hPluginHandle, _pstCell->zPluginName);

    /* Delete plugin cell */
    map_delete(sp_plugin_map, puc_key);

    /* Delete computed key */
    key_delete(puc_key);
  }
      
  /* Done */
  return;
}

/***************************************************************************
 plugin_locate_by_id
 
 This function finds a plugin by its ID, and returns a reference to it
 if it exists, otherwise orxNULL.

 ***************************************************************************/
orxPLUGIN_INFO *plugin_locate_by_id(orxHANDLE _hPluginHandle)
{
  orxPLUGIN_INFO *pst_plugin_cell;
  orxU8 *puc_key;

  /* Compute key with id */
  puc_key = key_create(_hPluginHandle, orxNULL);

  /* Search for the requested plugin */
  pst_plugin_cell = (orxPLUGIN_INFO *)map_find(sp_plugin_map, puc_key);

  /* Delete the key */
  key_delete(puc_key);

  return pst_plugin_cell;
}

/***************************************************************************
 plugin_locate_by_name
 
 This function finds a plugin by its name, and returns a reference to it
 if it exists, otherwise orxNULL.

 ***************************************************************************/
orxPLUGIN_INFO *plugin_locate_by_name(orxU8 *_zPluginName)
{
  orxPLUGIN_INFO *pst_plugin_cell;
  orxU8 *puc_key;

  /* Compute key with id */
  puc_key = key_create(0, _zPluginName);

  /* Search for the requested plugin */
  pst_plugin_cell = (orxPLUGIN_INFO *)map_find(sp_plugin_map, puc_key);

  /* Delete the key */
  key_delete(puc_key);

  return pst_plugin_cell;
}

/***************************************************************************
 get_func_addr
 Returns a orxVOID pointer to the function located in the plugin of handle param1,
  named param2.
 
 returns: pointer if success, orxNULL if error (do plugin_error to find out)
 ***************************************************************************/
orxVOID *get_func_addr(orxSYSPLUGIN_HANDLE _hPluginHandle, orxU8 *_zFunctionName)
{
  orxVOID *p_function_handle = orxNULL;

  /* Check validity of parameters */
  if(_zFunctionName == orxNULL)
  {
    /* Log an error */
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_BAD_PARAMETERS);

    return orxNULL;
  }
  
  if(_hPluginHandle == orxNULL)
  {
    /* Log an error */
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_NOT_FOUND);

    return orxNULL;
  }
  
  /* Attempt to obtain the pointer to the func */
  p_function_handle = orxPLUGIN_GET_SYMBOL_ADDRESS(_hPluginHandle, _zFunctionName);

  if(p_function_handle == orxNULL)
  {
    /* Log an error */
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_FUNCTION_NOT_FOUND);
  }

  /* Finally, return the orxVOID pointer to the caller */
  return p_function_handle;
}

/***************************************************************************
 function_core_register
 
 This function registers a core function.
 Returns orxVOID.
 ***************************************************************************/
orxVOID function_core_register(orxPLUGIN_FUNCTION _pfnFunction, orxU32 _eFunctionID)
{
  orxCONST orxPLUGIN_CORE_FUNCTION *pst_core_function;
  orxU32 i_index;

  /* Locates corresponding core function info array */
  i_index = (_eFunctionID & orxPLUGIN_KU32_MASK_PLUGIN_ID) >> orxPLUGIN_KU32_SHIFT_PLUGIN_ID;

  if(i_index < orxPLUGIN_CORE_ID_NUMBER)
  {
    pst_core_function = sapst_function[i_index];

    i_index = _eFunctionID & orxPLUGIN_KU32_MASK_FUNCTION_ID;

//    fprintf(stdout, "ID %x index %d ppfn %p pfn %p\n", _eFunctionID, i_index, pst_core_function[i_index].pfnFunction, _pfnFunction);
    if(pst_core_function[i_index].pfnFunction != orxNULL)
    {
      *(pst_core_function[i_index].pfnFunction) = _pfnFunction;
    }
  }

  /* Done */
  return;
}

/***************************************************************************
 plugin_register
 
 This function registers a plugin.
 Returns orxVOID.
 ***************************************************************************/
orxVOID plugin_register(orxSYSPLUGIN_HANDLE _p_handle, orxPLUGIN_INFO *_pst_plugin)
{
  orxVOID (*pfn_init)(orxS32 *, orxPLUGIN_USER_FUNCTION_INFO **) = get_func_addr(_p_handle, orxPLUGIN_USER_KZ_FUNCTION_INIT);
  orxPLUGIN_FUNCTION_INFO *pst_function_cell;
  orxS32 i_function_number = 0, i;
  orxPLUGIN_USER_FUNCTION_INFO *pst_function;
  orxU8* puc_key;

  /*
   * Call plugin init function
   */
 
  if(pfn_init != orxNULL)
  {
    pfn_init(&i_function_number, &pst_function);
  }

  /* Creates a cell */
  pst_function_cell = function_cell_create();

  /* Adds all functions to plugin info */
  for(i = 0; i < i_function_number; i++)
  {
    if(pst_function[i].pfnFunction != orxNULL)
    {
      /* Copies infos */
      pst_function_cell->pfnFunction = pst_function[i].pfnFunction;
      strcpy(pst_function_cell->zFunctionArgs, pst_function[i].zFunctionArgs);
      strcpy(pst_function_cell->zFunctionName, pst_function[i].zFunctionName);
      pst_function_cell->eFunctionID = pst_function[i].eFunctionID;

      /* Adds function info in plugin info structure */
      puc_key = key_create((orxHANDLE)pst_function_cell->eFunctionID, pst_function_cell->zFunctionName);
      map_add(_pst_plugin->p_function_map, puc_key, (orxU8 *)pst_function_cell);
      key_delete(puc_key);

      /* Checks if it's a core plugin */
      if(pst_function_cell->eFunctionID & orxPLUGIN_KU32_FLAG_CORE_ID)
      {
        function_core_register(pst_function_cell->pfnFunction, pst_function_cell->eFunctionID);
      }
    }
  }

  /* Deletes function cell */
  function_cell_delete(pst_function_cell);

  /* Done */
  return;
}

/***************************************************************************
 orxPlugin_AddCoreInfo
 
 This function adds a core plugin info structure to the global info array.
 Returns orxVOID.
 ***************************************************************************/
orxVOID orxFASTCALL orxPlugin_AddCoreInfo(orxPLUGIN_CORE_ID _ePluginCoreID, orxCONST orxPLUGIN_CORE_FUNCTION *_astCoreFunction, orxU32 _u32CoreFunctionNumber)
{
  /* Checks */
  orxASSERT(_ePluginCoreID < orxPLUGIN_CORE_ID_NUMBER);

  /* Stores info */
  sapst_function[_ePluginCoreID]      = _astCoreFunction;
  si_function_number[_ePluginCoreID]  = _u32CoreFunctionNumber;

  return;
}


/***************************************************************************
 ***************************************************************************
 ******                       PUBLIC FUNCTIONS                        ******
 ***************************************************************************
 ***************************************************************************/


/***************************************************************************
 orxPlugin_DefaultCoreFunction
 Default function for non-initialized core plugin functions.
 Log the problem.

 returns: orxNULL
 ***************************************************************************/
orxVOID *orxFASTCALL orxPlugin_DefaultCoreFunction(orxCONST orxSTRING _zFunctionName, orxCONST orxSTRING _zFileName, orxU32 _u32Line)
{
  orxDEBUG_FLAG_BACKUP();
  orxDEBUG_FLAG_SET(orxDEBUG_KU32_FLAG_CONSOLE
                |orxDEBUG_KU32_FLAG_FILE
                |orxDEBUG_KU32_FLAG_TIMESTAMP
                |orxDEBUG_KU32_FLAG_TYPE,
                 orxDEBUG_KU32_FLAG_ALL);
  orxDEBUG_LOG(orxDEBUG_LEVEL_ALL, MSG_PLUGIN_KZ_DEFAULT_NOT_LOADED_ZZI, _zFunctionName, _zFileName, _u32Line);
  orxDEBUG_FLAG_RESTORE();

  return orxNULL;
}

/***************************************************************************
 orxPlugin_Load
 Loads the shared object specified as a null terminated string, and makes
 it subsequently available under the name in the 2nd param.
 
 returns: plugin handle on success, orxHANDLE_Undefined if load error
 ***************************************************************************/
orxHANDLE orxFASTCALL orxPlugin_Load(orxCONST orxSTRING _zPluginFileName, orxCONST orxSTRING _zPluginName)
{
  orxSYSPLUGIN_HANDLE p_handle;

  orxPLUGIN_INFO *pstCell;
  orxU8 *puc_key;

  /* Ready to Work? */
  if(!(sstPlugin.u32Flags & orxPLUGIN_KU32_FLAG_READY))
  {
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_NOT_READY);
    return orxHANDLE_Undefined;
  }

  /* Check params */
  if(_zPluginFileName == orxNULL)
  {
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_INVALID_FILENAME);
    return orxHANDLE_Undefined;
  }
  if(_zPluginName == orxNULL)
  {
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_INVALID_NAME);
    return orxHANDLE_Undefined;
  }
  
  /* Attempt to link in the shared object */
  if((p_handle = orxPLUGIN_OPEN(_zPluginFileName)) == orxNULL)
  {
    /* Log an error message here, but I forgot the prototype ^^ */
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_LOAD_FAILED);
    return orxHANDLE_Undefined;
  }
  
  /*
   * Create the control structure used within the plugin module to keep
   * track of modules
   */
  if((pstCell = plugin_cell_create()) == orxNULL)
  {
    orxPLUGIN_CLOSE(p_handle);
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_MALLOC_FAILED);
    return orxHANDLE_Undefined;
  }
  
  /*
   * Fill in the control structure
   */

  /* First, plug in the reference to the dynamic code */
  pstCell->hPluginHandle = p_handle;

  /* Then, assign an ID */
  pstCell->hPluginHandle = (orxHANDLE)map_count(sp_plugin_map) + 1;

  /* Store info on registered functions */
  plugin_register(p_handle, pstCell);

  /* Finally, enter the name */  
  strcpy(pstCell->zPluginName, _zPluginName);

  /* Compute a complete key */
  puc_key = key_create(pstCell->hPluginHandle, _zPluginName);

  /* Enter the new structure into the map */
  map_add(sp_plugin_map, puc_key, (orxU8 *)pstCell);

  /* Delete key */
  key_delete(puc_key);

  /* Return ID */
  return pstCell->hPluginHandle;
}


/***************************************************************************
 orxPlugin_LoadUsingExt
 Loads the shared object specified as a null terminated string using OS library extension,
 and makes it subsequently available under the name in the 2nd param.

 returns: plugin handle on success, orxHANDLE_Undefined if load error
 ***************************************************************************/
orxHANDLE orxFASTCALL orxPlugin_LoadUsingExt(orxCONST orxSTRING _zPluginFileName, orxCONST orxSTRING _zPluginName)
{
  orxCHAR zFilename[128];

  sprintf(zFilename, "%s.%s", _zPluginFileName, szPluginLibraryExt);

  return(orxPlugin_Load(zFilename, _zPluginName));
}


/***************************************************************************
 orxPlugin_Unload
 Unloads the shared object designated by its handle
 
 returns: orxSTATUS_SUCCESS/orxSTATUS_FAILED
 ***************************************************************************/
orxSTATUS orxFASTCALL orxPlugin_Unload(orxHANDLE _hPluginHandle)
{
  orxPLUGIN_INFO *pst_node;

  if((pst_node = plugin_locate_by_id(_hPluginHandle)) != orxNULL)
  {
    /* Plugin found, kill it and return okay */
    plugin_cell_delete(pst_node);

    return orxSTATUS_SUCCESS;
  }
  else
  {
    /* Search did not return positive, so return error */
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_NOT_FOUND);

    return orxSTATUS_FAILED;
  }
}

/***************************************************************************
 orxPlugin_GetFunction
 Returns a orxVOID pointer to the function located in the plugin of handle param1,
  named param2.
 
 returns: orxPLUGIN_FUNCTION / orxNULL
 ***************************************************************************/
orxPLUGIN_FUNCTION orxFASTCALL orxPlugin_GetFunction(orxHANDLE _hPluginHandle, orxCONST orxSTRING _zFunctionName)
{
  orxPLUGIN_INFO *pstCell;
  orxVOID *p_function_handle = orxNULL;

  /* Gets the plugin cell */
  pstCell = (orxPLUGIN_INFO *)plugin_locate_by_id(_hPluginHandle);

  /* Try to get the function handle */
  p_function_handle = get_func_addr(pstCell->hPluginHandle,
                                    _zFunctionName);

  /* No function found ? */
  if(p_function_handle == orxNULL)
  {
    /* Log an error */
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_FUNCTION_NOT_FOUND);
  }

  /* Finally, return the orxVOID pointer to the caller */
  return (orxPLUGIN_FUNCTION)p_function_handle;
}

/***************************************************************************
 orxPlugin_GetHandle
 
 This function returns the integer ID of the plugin of given name
 
 returns ID on success, orxHANDLE_Undefined on error
 ***************************************************************************/
orxHANDLE orxFASTCALL orxPlugin_GetHandle(orxCONST orxSTRING _zPluginName)
{
  orxPLUGIN_INFO *pstCell;

  /* Check parameter validity */
  if(_zPluginName == orxNULL)
  {
    /* Log an error */
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_BAD_PARAMETERS);

    return orxHANDLE_Undefined;
  }

  if((pstCell = plugin_locate_by_name(_zPluginName)) == orxNULL)
  {
    /* Log an error */
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_NOT_FOUND);

    return orxHANDLE_Undefined;
  }

  return(pstCell->hPluginHandle);
}

/***************************************************************************
 orxPlugin_GetName
 
 This function returns the name of the plugin of given ID
 
 returns ID on success, -1 on error
 ***************************************************************************/
orxCONST orxSTRING orxFASTCALL orxPlugin_GetName(orxHANDLE _hPluginHandle)
{
  orxPLUGIN_INFO *pstCell;

  if((pstCell = plugin_locate_by_id(_hPluginHandle)) == orxNULL)
  {
    /* Log an error */
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_NOT_FOUND);

    return orxSTRING_Empty;
  }

  return(pstCell->zPluginName);
}

orxSTATUS orxPlugin_Init()
{
  orxS32 i;

  if(!(sstPlugin.u32Flags & orxPLUGIN_KU32_FLAG_READY))
  {
    /* Creates an empty spst_plugin_list */
    sp_plugin_map = (orxPLUGIN_INFO *)map_create(orxPLUGIN_KU32_NAME_SIZE,
                                                        sizeof(orxPLUGIN_INFO),
                                                        &key_compare);

    /* Inits static core info structures */
    for(i = 0; i < orxPLUGIN_CORE_ID_NUMBER; i++)
    {
      sapst_function[i] = orxNULL;
      si_function_number[i] = 0;
    }

    /* Updates status flags */
    sstPlugin.u32Flags |= orxPLUGIN_KU32_FLAG_READY;

    return orxSTATUS_SUCCESS;
  }

  return orxSTATUS_FAILED;
}

orxVOID orxPlugin_Exit()
{
  orxU32 i;
  orxPLUGIN_INFO *pstCell;

  /* Delete all plugin cells */
  for(i = 1; i <= map_count(sp_plugin_map); i++)
  {
    /* Gets plugin */
    pstCell = plugin_locate_by_id((orxHANDLE)i);

    /* Delete plugin cell */
    plugin_cell_delete(pstCell);
  }

  /* Destroy plugin map */
  map_destroy(sp_plugin_map);

  /* Updates status flags */
  sstPlugin.u32Flags = orxPLUGIN_KU32_FLAG_NONE;

  return;
}
