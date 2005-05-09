/***************************************************************************
 plugin.c
 plugin managment system
 
 begin                : 04/09/2002
 author               : (C) Gdp
 email                : david.anderson@calixo.net
                      : iarwain@ifrance.com       (v1.10->)
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

#include <stdio.h>

#ifdef __orxLINUX__

  #include <dlfcn.h>

#else /* __orxLINUX__ */

  #ifdef __orxWINDOWS__

    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>

  #endif /* __orxWINDOWS__ */

#endif /* __orxLINUX__ */


/*
 * Platform independant defines
 */

#define PLUGIN_KU32_FLAG_NONE    0x00000000
#define PLUGIN_KU32_FLAG_READY   0x00000001

/*
 * Platform dependant type & function defines
 */

/* WINDOWS */
#ifdef __orxWINDOWS__

#define PLUGIN_HANDLE       HMODULE

#define PLUGIN_OPEN(X)      LoadLibrary(X)
#define PLUGIN_SYM(X, Y)    GetProcAddress(X, Y)
#define PLUGIN_CLOSE(X)     FreeLibrary(X)

#define PLUGIN_LIBRARY_EXT  "dll"

/* OTHERS */
#else

#define PLUGIN_HANDLE       orxVOID *

#define PLUGIN_OPEN(X)      dlopen(X, RTLD_LAZY)
#define PLUGIN_SYM(X, Y)    dlsym(X, Y)
#define PLUGIN_CLOSE(X)     dlclose(X)

#define PLUGIN_LIBRARY_EXT  "so"

#endif


/*
 * Information structure on a plugin function
 */
typedef struct st_function_info_t
{
  orxPLUGIN_FUNCTION pfnFunction;                       /**< Function Address : 4 */
  orxU8 zFunctionArgs[orxPLUGIN_KU32_FUNCTION_ARG_SIZE];/**< Function Argument Types : 132 */

  orxU8 zFunctionName[orxPLUGIN_KU32_NAME_SIZE];        /**< Function Name : 164 */
  orxU32 eFunctionID;                                   /**< Function ID : 168 */

  /* 8 extra bytes of padding : 176 */
  orxU8 au8Unused[8];
} plugin_st_function_info;

/*
 * Information structure on a plugin
 */
typedef struct st_plugin_info_t
{
  PLUGIN_HANDLE p_plugin_handle;            /**< Plugin handle : 4 */

  orxU8 z_plugin_name[orxPLUGIN_KU32_NAME_SIZE];  /**< Plugin name : 36 */
  orxU32 u32_plugin_id;                      /**< Plugin ID : 40 */

  map p_function_map;                       /**< Plugin Function List : 44 */

  /* 4 extra bytes of padding : 48 */
  orxU8 au8Unused[4];
} plugin_st_plugin_info;



/*
 * Static information structures
 */
orxSTATIC orxPLUGIN_CORE_FUNCTION orxCONST *sapst_function[orxPLUGIN_CORE_ID_NUMBER];
orxSTATIC orxS32 si_function_number[orxPLUGIN_CORE_ID_NUMBER];

orxSTATIC map sp_plugin_map = orxNULL;
orxSTATIC orxU32 plugin_su32Flags = PLUGIN_KU32_FLAG_NONE;

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
orxU8 *key_create(orxU32 _u32_id, orxU8 *_z_name)
{
  orxU8 *puc_key = orxNULL;
  orxS32 i_shift = sizeof(orxU32);

  /* Allocate memory for key */
  puc_key = (orxU8 *)orxMemory_Allocate(i_shift + orxPLUGIN_KU32_NAME_SIZE, orxMEMORY_TYPE_MAIN);

  if(puc_key != orxNULL)
  {
    *puc_key = (orxU8)_u32_id;

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
plugin_st_function_info *function_cell_create()
{
  plugin_st_function_info *pstCell;

  /* Creates a function info cell */
  pstCell = (plugin_st_function_info *) orxMemory_Allocate(sizeof(plugin_st_function_info), orxMEMORY_TYPE_MAIN);

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
plugin_st_plugin_info *plugin_cell_create()
{
  plugin_st_plugin_info *pstCell;

  /* Creates a plugin info cell */
  pstCell = (plugin_st_plugin_info *) orxMemory_Allocate(sizeof(plugin_st_plugin_info), orxMEMORY_TYPE_MAIN);

  /* Initiates it */
  pstCell->p_plugin_handle   = orxNULL;
  pstCell->u32_plugin_id      = orxPLUGIN_FUNCTION_ID_NONE;
  pstCell->p_function_map    = 
    (plugin_st_function_info *)map_create(orxPLUGIN_KU32_NAME_SIZE,
                                          sizeof(plugin_st_function_info),
                                          &key_compare);

  return pstCell;
}

/***************************************************************************
 function_cell_delete

 This function deletes a function_info cell

 Returns nothing.
 ***************************************************************************/
orxVOID function_cell_delete(plugin_st_function_info *_pstCell)
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
 It should be called from plugin_unload_by_id or plugin_unload_by_name.
 
 Returns nothing.
 ***************************************************************************/
orxVOID plugin_cell_delete(plugin_st_plugin_info *_pstCell)
{
  orxU8 *puc_key;

  /* Is cell valid? */
  if(_pstCell != orxNULL)
  {
    /* Unload plugin */
    if(_pstCell->p_plugin_handle != orxNULL)
    {
      PLUGIN_CLOSE(_pstCell->p_plugin_handle);
    }

    /* Free function map */
    if(_pstCell->p_function_map != orxNULL)
    {
      map_destroy(_pstCell->p_function_map);
    }

    /* Compute a complete key */
    puc_key = key_create(_pstCell->u32_plugin_id, _pstCell->z_plugin_name);

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
plugin_st_plugin_info *plugin_locate_by_id(orxU32 _u32_plugin_id)
{
  plugin_st_plugin_info *pst_plugin_cell;
  orxU8 *puc_key;

  /* Compute key with id */
  puc_key = key_create(_u32_plugin_id, orxNULL);

  /* Search for the requested plugin */
  pst_plugin_cell = (plugin_st_plugin_info *)map_find(sp_plugin_map, puc_key);

  /* Delete the key */
  key_delete(puc_key);

  return pst_plugin_cell;
}

/***************************************************************************
 plugin_locate_by_name
 
 This function finds a plugin by its name, and returns a reference to it
 if it exists, otherwise orxNULL.

 ***************************************************************************/
plugin_st_plugin_info *plugin_locate_by_name(orxU8 *_z_plugin_name)
{
  plugin_st_plugin_info *pst_plugin_cell;
  orxU8 *puc_key;

  /* Compute key with id */
  puc_key = key_create(0, _z_plugin_name);

  /* Search for the requested plugin */
  pst_plugin_cell = (plugin_st_plugin_info *)map_find(sp_plugin_map, puc_key);

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
orxVOID *get_func_addr(PLUGIN_HANDLE _p_plugin_handle, orxU8 *_zFunctionName)
{
  orxVOID *p_function_handle = orxNULL;

  /* Check validity of parameters */
  if(_zFunctionName == orxNULL)
  {
    /* Log an error */
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_BAD_PARAMETERS);

    return orxNULL;
  }
  
  if(_p_plugin_handle == orxNULL)
  {
    /* Log an error */
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_NOT_FOUND);

    return orxNULL;
  }
  
  /* Attempt to obtain the pointer to the func */
  p_function_handle = PLUGIN_SYM(_p_plugin_handle, _zFunctionName);

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
orxVOID plugin_register(PLUGIN_HANDLE _p_handle, plugin_st_plugin_info *_pst_plugin)
{
  orxVOID (*pfn_init)(orxS32 *, orxPLUGIN_USER_FUNCTION_INFO **) = get_func_addr(_p_handle, orxPLUGIN_USER_KZ_FUNCTION_INIT);
  plugin_st_function_info *pst_function_cell;
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
      puc_key = key_create(pst_function_cell->eFunctionID, pst_function_cell->zFunctionName);
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
orxFASTCALL orxVOID orxPlugin_AddCoreInfo(orxPLUGIN_CORE_ID _ePluginCoreID, orxCONST orxPLUGIN_CORE_FUNCTION *_astCoreFunction, orxU32 _u32CoreFunctionNumber)
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
orxFASTCALL orxVOID *orxPlugin_DefaultCoreFunction(orxCONST orxSTRING _zFunctionName, orxCONST orxSTRING _zFileName, orxU32 _u32Line)
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
 plugin_load
 Loads the shared object specified as a null terminated string, and makes
 it subsequently available under the name in the 2nd param.
 
 returns: plugin ID on success, orxU32_Undefined if load error
 ***************************************************************************/
orxU32 plugin_load(orxU8 *_z_plugin_filename, orxU8 *_z_plugin_name)
{
  PLUGIN_HANDLE p_handle;

  plugin_st_plugin_info *pstCell;
  orxU8 *puc_key;

  /* Ready to Work? */
  if(!(plugin_su32Flags & PLUGIN_KU32_FLAG_READY))
  {
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_NOT_READY);
    return orxU32_Undefined;
  }

  /* Check params */
  if(_z_plugin_filename == orxNULL)
  {
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_INVALID_FILENAME);
    return orxU32_Undefined;
  }
  if(_z_plugin_name == orxNULL)
  {
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_INVALID_NAME);
    return orxU32_Undefined;
  }
  
  /* Attempt to link in the shared object */
  if((p_handle = PLUGIN_OPEN(_z_plugin_filename)) == orxNULL)
  {
    /* Log an error message here, but I forgot the prototype ^^ */
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_LOAD_FAILED);
    return orxU32_Undefined;
  }
  
  /*
   * Create the control structure used within the plugin module to keep
   * track of modules
   */
  if((pstCell = plugin_cell_create()) == orxNULL)
  {
    PLUGIN_CLOSE(p_handle);
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_MALLOC_FAILED);
    return orxU32_Undefined;
  }
  
  /*
   * Fill in the control structure
   */

  /* First, plug in the reference to the dynamic code */
  pstCell->p_plugin_handle = p_handle;

  /* Then, assign an ID */
  pstCell->u32_plugin_id = (orxU32)map_count(sp_plugin_map) + 1;

  /* Store info on registered functions */
  plugin_register(p_handle, pstCell);

  /* Finally, enter the name */  
  strcpy(pstCell->z_plugin_name, _z_plugin_name);

  /* Compute a complete key */
  puc_key = key_create(pstCell->u32_plugin_id, _z_plugin_name);

  /* Enter the new structure into the map */
  map_add(sp_plugin_map, puc_key, (orxU8 *)pstCell);

  /* Delete key */
  key_delete(puc_key);

  /* Return ID */
  return pstCell->u32_plugin_id;
}


/***************************************************************************
 plugin_load_ext
 Loads the shared object specified as a null terminated string using OS library extension,
 and makes it subsequently available under the name in the 2nd param.

 returns: plugin ID on success, orxU32_Undefined if load error
 ***************************************************************************/
orxU32 plugin_load_ext(orxU8 *_z_plugin_filename, orxU8 *_z_plugin_name)
{
  orxU8 zFilename[128];

  sprintf(zFilename, "%s.%s", _z_plugin_filename, PLUGIN_LIBRARY_EXT);

  return(plugin_load(zFilename, _z_plugin_name));
}


/***************************************************************************
 plugin_unload_by_id
 Unloads the shared object designated by its ID
 
 returns: orxTRUE if success, orxFALSE if unload error (usually 'plugin not loaded')
 ***************************************************************************/
orxBOOL plugin_unload_by_id(orxU32 _u32_plugin_id)
{
  plugin_st_plugin_info *pst_node;

  if((pst_node = plugin_locate_by_id(_u32_plugin_id)) != orxNULL)
  {
    /* Plugin found, kill it and return okay */
    plugin_cell_delete(pst_node);

    return orxTRUE;
  }
  else
  {
    /* Search did not return positive, so return error */
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_NOT_FOUND);

    return orxFALSE;
  }
}

/***************************************************************************
 plugin_unload_by_name
 Unloads the shared object designated by its name instead of ID
 
 returns: orxTRUE if success, orxFALSE if unload error (usually 'plugin not loaded')
 ***************************************************************************/
orxBOOL plugin_unload_by_name(orxU8 *_z_plugin_name)
{
  plugin_st_plugin_info *pst_node;

  if((pst_node = plugin_locate_by_name(_z_plugin_name)) != orxNULL)
  {
    /* Plugin found, kill it and return okay */
    plugin_cell_delete(pst_node);

    return orxTRUE;
  }
  else
  {
    /* Search did not return positive, so return error */
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_NOT_FOUND);

    return orxFALSE;
  }
}

/***************************************************************************
 plugin_get_func_addr_by_id
 Returns a orxVOID pointer to the function located in the plugin of ID param1,
  named param2.
 
 returns: pointer if success, orxNULL if error (do plugin_error to find out)
 ***************************************************************************/
orxVOID *plugin_get_func_addr_by_id(orxU32 _u32_plugin_id, orxU8 *_zFunctionName)
{
  plugin_st_plugin_info *pstCell;
  orxVOID *p_function_handle = orxNULL;

  /* Gets the plugin cell */
  pstCell = (plugin_st_plugin_info *)plugin_locate_by_id(_u32_plugin_id);

  /* Try to get the function handle */
  p_function_handle = get_func_addr(pstCell->p_plugin_handle,
                                    _zFunctionName);

  /* No function found ? */
  if(p_function_handle == orxNULL)
  {
    /* Log an error */
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_FUNCTION_NOT_FOUND);
  }

  /* Finally, return the orxVOID pointer to the caller */
  return p_function_handle;
}

/***************************************************************************
 plugin_get_func_addr_by_name
 Returns a orxVOID pointer to the function located in the plugin of name param1,
  named param2.
 
 returns: pointer if success, orxNULL if error (do plugin_error to find out)
 ***************************************************************************/
orxVOID *plugin_get_func_addr_by_name(orxU8 *_z_plugin_name, orxU8 *_zFunctionName)
{
  plugin_st_plugin_info *pstCell;
  orxVOID *p_function_handle = orxNULL;

  /* Gets the plugin cell */
  pstCell = (plugin_st_plugin_info *)plugin_locate_by_name(_z_plugin_name);

  /* Plugin found? */
  if(pstCell != orxNULL)
  {
    /* Try to get the function handle */
    p_function_handle = get_func_addr(pstCell->p_plugin_handle,
                                      _zFunctionName);

    /* No function found ? */
    if(p_function_handle == orxNULL)
    {
      /* Log an error */
      orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_FUNCTION_NOT_FOUND);
    }
  }
  else
  {
    /* Log an error */
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_NOT_FOUND);
  }

  return p_function_handle;
}

/***************************************************************************
 plugin_get_id_by_name
 
 This function returns the integer ID of the plugin of given name
 
 returns ID on success, orxU32_Undefined on error
 ***************************************************************************/
orxU32 plugin_get_id_by_name(orxU8 *_z_plugin_name)
{
  plugin_st_plugin_info *pstCell;

  /* Check parameter validity */
  if(_z_plugin_name == orxNULL)
  {
    /* Log an error */
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_BAD_PARAMETERS);

    return orxU32_Undefined;
  }

  if((pstCell = plugin_locate_by_name(_z_plugin_name)) == orxNULL)
  {
    /* Log an error */
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_NOT_FOUND);

    return orxU32_Undefined;
  }
  
  return(pstCell->u32_plugin_id);
}

/***************************************************************************
 plugin_get_name_by_id
 
 This function returns the name of the plugin of given ID
 
 returns ID on success, -1 on error
 ***************************************************************************/
orxU8 *plugin_get_name_by_id(orxU32 _u32_plugin_id)
{
  plugin_st_plugin_info *pstCell;

  if((pstCell = plugin_locate_by_id(_u32_plugin_id)) == orxNULL)
  {
    /* Log an error */
    orxDEBUG_LOG(orxDEBUG_LEVEL_PLUGIN, MSG_PLUGIN_KZ_NOT_FOUND);

    return orxNULL;
  }

  return(pstCell->z_plugin_name);
}

orxU32 plugin_init()
{
  orxS32 i;

  if(!(plugin_su32Flags & PLUGIN_KU32_FLAG_READY))
  {
    /* Creates an empty spst_plugin_list */
    sp_plugin_map = (plugin_st_plugin_info *)map_create(orxPLUGIN_KU32_NAME_SIZE,
                                                        sizeof(plugin_st_plugin_info),
                                                        &key_compare);

    /* Inits static core info structures */
    for(i = 0; i < orxPLUGIN_CORE_ID_NUMBER; i++)
    {
      sapst_function[i] = orxNULL;
      si_function_number[i] = 0;
    }

    /* Updates status flags */
    plugin_su32Flags |= PLUGIN_KU32_FLAG_READY;

    return orxSTATUS_SUCCESS;
  }

  return orxSTATUS_FAILED;
}

orxVOID plugin_exit()
{
  orxU32 i;
  plugin_st_plugin_info *pstCell;

  /* Delete all plugin cells */
  for(i = 1; i <= map_count(sp_plugin_map); i++)
  {
    /* Gets plugin */
    pstCell = plugin_locate_by_id((orxU32)i);

    /* Delete plugin cell */
    plugin_cell_delete(pstCell);
  }

  /* Destroy plugin map */
  map_destroy(sp_plugin_map);

  /* Updates status flags */
  plugin_su32Flags = PLUGIN_KU32_FLAG_NONE;

  return;
}
