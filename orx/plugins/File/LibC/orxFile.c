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
 * @file orxFile.c
 * @date 10/12/2006
 * @author bestel@arcallians.org
 *
 * Lib C file plugin implementation
 *
 * @todo
 */


#include "orxInclude.h"
#include "plugin/orxPluginUser.h"
#include "debug/orxDebug.h"
#include "io/orxFile.h"

#include <stdio.h>


/***************************************************************************
 * Structure declaration                                                   *
 ***************************************************************************/

#define orxFILE_KU32_STATIC_FLAG_NONE   0x00000000  /**< No flags have been set */
#define orxFILE_KU32_STATIC_FLAG_READY  0x00000001  /**< The module has been initialized */

typedef struct __orxFILE_STATIC_t
{
  orxU32 u32Flags;
} orxFILE_STATIC;

struct __orxFILE_t
{
  FILE* pstFile;
};

/***************************************************************************
 * Module global variable                                                  *
 ***************************************************************************/
orxSTATIC orxFILE_STATIC sstFile;

/***************************************************************************
 * Private functions                                                       *
 ***************************************************************************/


/***************************************************************************
 * Public functions                                                        *
 ***************************************************************************/

/** Initialize the File Module
 * @return Returns the status of the module initialization
 */
orxSTATUS orxFile_LibC_Init()
{
  /* Module not already initialized ? */
  orxASSERT(!(sstFile.u32Flags & orxFILE_KU32_STATIC_FLAG_READY));

  /* Cleans static controller */
  orxMemory_Zero(&sstFile, sizeof(orxFILE_STATIC));

	/* Set module has ready */
	sstFile.u32Flags = orxFILE_KU32_STATIC_FLAG_READY;

  /* Module successfully initialized ? */
  if(sstFile.u32Flags & orxFILE_KU32_STATIC_FLAG_READY)
  {
    return orxSTATUS_SUCCESS;
  }
  else
  {
    return orxSTATUS_FAILURE;
  }
}

/** Uninitialize the File Module
 */
orxVOID orxFile_LibC_Exit()
{
  /* Module initialized ? */
  orxASSERT((sstFile.u32Flags & orxFILE_KU32_STATIC_FLAG_READY) == orxFILE_KU32_STATIC_FLAG_READY);

  /* Module not ready now */
  sstFile.u32Flags = orxFILE_KU32_STATIC_FLAG_NONE;
}

/** Open a file for later read or write operation.
 * @param _zPath         (IN)      Full file's path to open
 * @param _u32OpenFlags  (IN)      List of used flags when opened
 * @return a File pointer (or orxNULL if an error has occured)
 */
orxFILE* orxFile_LibC_Open(orxCONST orxSTRING _zPath, orxU32 _u32OpenFlags)
{
  /* Convert the open flags into a string */
  orxCHAR zMode[3];

  /* Module initialized ? */
  orxASSERT((sstFile.u32Flags & orxFILE_KU32_STATIC_FLAG_READY) == orxFILE_KU32_STATIC_FLAG_READY);

  /* Fills with null terminated characters */
  orxMemory_Set(&zMode, orxCHAR_NULL, 3);

  /*** LIB C MODES :
   * r   : Open text file for reading.
   *       The stream is positioned at the beginning of the file.
   * r+  : Open for reading and writing.
   *       The stream is positioned at the beginning of the file.
   * w   : Truncate file to zero length or create text file for writing.
   *       The stream is positioned at the beginning of the file.
   * w+  : Open for reading and writing.
   *       The file is created if it does not exist, otherwise it is truncated.
   *       The stream is positioned at the beginning of the file.
   * a   : Open for appending (writing at end of file).
   *       The file is created if it does not exist.
   *       The stream is positioned at the end of the file.
   * a+  : Open for reading and appending (writing at end of file).
   *       The file is created if it does not exist.
   *       The initial file position for reading is at the beginning of the file, but output is always appended to the end of the file.
       *
   *** AVAILABLE CONVERSIONS :
   * READ | WRITE | APPEND | result
   *  X   |       |        | r
   *      |  X    |        | w
   *      |       |   X    | a
   *      |  X    |   X    | a
   *  X   |  X    |        | w+
   *  X   |       |   X    | a+
   *  X   |  X    |   X    | a+
   */

  /* Read only ? */
  if(_u32OpenFlags == orxFILE_KU32_FLAG_OPEN_READ)
  {
    /* Copy the mode in the string */
    orxMemory_Copy(&zMode, "r", sizeof(orxCHAR));
  }
  /* Write only ?*/
  else if(_u32OpenFlags == orxFILE_KU32_FLAG_OPEN_WRITE)
  {
    /* Copy the mode in the string */
    orxMemory_Copy(&zMode, "w", sizeof(orxCHAR));
  }
  /* Appen only ? */
  else if((_u32OpenFlags == orxFILE_KU32_FLAG_OPEN_APPEND)
       || (_u32OpenFlags == (orxFILE_KU32_FLAG_OPEN_WRITE | orxFILE_KU32_FLAG_OPEN_APPEND)))
  {
    /* Copy the mode in the string */
    orxMemory_Copy(&zMode, "a", sizeof(orxCHAR));
  }
  else if(_u32OpenFlags == (orxFILE_KU32_FLAG_OPEN_READ | orxFILE_KU32_FLAG_OPEN_WRITE))
  {
    /* Copy the mode in the string */
    orxMemory_Copy(&zMode, "w+", 2 * sizeof(orxCHAR));
  }
  else if((_u32OpenFlags == (orxFILE_KU32_FLAG_OPEN_READ | orxFILE_KU32_FLAG_OPEN_APPEND))
       || (_u32OpenFlags == (orxFILE_KU32_FLAG_OPEN_READ | orxFILE_KU32_FLAG_OPEN_WRITE | orxFILE_KU32_FLAG_OPEN_APPEND)))
  {
    /* Copy the mode in the string */
    orxMemory_Copy(&zMode, "a+", 2 * sizeof(orxCHAR));
  }

  /* Open the file */
  return(orxFILE*)fopen(_zPath, zMode);
}

/** Read datas from a file
 * @param _pReadData     (OUT)     Pointer where will be stored datas
 * @param _u32ElemSize   (IN)      Size of 1 element
 * @param _u32NbElem     (IN)      Number of elements
 * @param _pstFile       (IN)      Pointer on the file descriptor
 * @return Returns the number of read elements (not bytes)
 */
orxU32 orxFile_LibC_Read(orxVOID *_pReadData, orxU32 _u32ElemSize, orxU32 _u32NbElem, orxFILE *_pstFile)
{
  /* Default return value */
  orxU32 u32Ret = 0;

  /* Module initialized ? */
  orxASSERT((sstFile.u32Flags & orxFILE_KU32_STATIC_FLAG_READY) == orxFILE_KU32_STATIC_FLAG_READY);

  /* Valid input ? */
  if(_pstFile != orxNULL)
  {
    u32Ret = (orxU32)fread(_pReadData, _u32ElemSize, _u32NbElem, (FILE*)_pstFile);
  }

  /* Returns the number of read elements */
  return u32Ret;
}

/** write datas to a file
 * @param _pDataToWrite  (IN)      Pointer where will be stored datas
 * @param _u32ElemSize   (IN)      Size of 1 element
 * @param _u32NbElem     (IN)      Number of elements
 * @param _pstFile       (IN)      Pointer on the file descriptor
 * @return Returns the number of written elements (not bytes)
 */
orxU32 orxFile_LibC_Write(orxVOID *_pDataToWrite, orxU32 _u32ElemSize, orxU32 _u32NbElem, orxFILE *_pstFile)
{
  /* Default return value */
  orxU32 u32Ret = 0;

  /* Module initialized ? */
  orxASSERT((sstFile.u32Flags & orxFILE_KU32_STATIC_FLAG_READY) == orxFILE_KU32_STATIC_FLAG_READY);

  /* Checks inputs */
  orxASSERT(_pstFile != orxNULL);

  /* Valid input ? */
  if(_pstFile != orxNULL)
  {
    u32Ret = (orxU32)fwrite(_pDataToWrite, _u32ElemSize, _u32NbElem, (FILE*)_pstFile);
  }

  /* Returns the number of read elements */
  return u32Ret;
}

/** get text line from a file
 * @param _zBuffer  (OUT)     Pointer where will be stored datas
 * @param _u32Size  (IN)      Size of buffer
 * @param _pstFile  (IN)      Pointer on the file descriptor
 * @return Returns true if a line has been read, else returns false.
 */
orxBOOL orxFile_LibC_ReadLine(orxSTRING _zBuffer, orxU32 _u32Size, orxFILE *_pstFile)
{
  /* Default return value */
  orxBOOL bRet = orxFALSE;

  /* Module initialized ? */
  orxASSERT((sstFile.u32Flags & orxFILE_KU32_STATIC_FLAG_READY) == orxFILE_KU32_STATIC_FLAG_READY);

  /* Checks inputs */
  orxASSERT(_pstFile != orxNULL);

  /* Valid input ? */
  if(_pstFile != orxNULL)
  {
    /* Try to read a line */
    if(fgets(_zBuffer, _u32Size, (FILE*)_pstFile))
    {
      bRet = orxTRUE;
    }
    else
    {
      bRet = orxFALSE;
    }
  }

  /* Returns orxTRUE if a line has been read, else orxFALSE */
  return bRet;
}


/** Close an oppened file
 * @param _pstFile       (IN)      File's pointer to close
 * @return Returns the status of the operation
 */
orxSTATUS orxFile_LibC_Close(orxFILE *_pstFile)
{
  /* Default return value */
  orxSTATUS eRet = orxSTATUS_FAILURE;

  /* Module initialized ? */
  orxASSERT((sstFile.u32Flags & orxFILE_KU32_STATIC_FLAG_READY) == orxFILE_KU32_STATIC_FLAG_READY);

  /* Checks inputs */
  orxASSERT(_pstFile != orxNULL);

  /* valid ? */
  if(_pstFile != orxNULL)
  {
    /* Close file pointer */
    if(fclose((FILE*)_pstFile) == 0)
    {
      /* Success ! */
      eRet = orxSTATUS_SUCCESS;
    }
  }

  /* return success status */
  return eRet;
}


/***************************************************************************
 * Plugin Related                                                          *
 ***************************************************************************/

/***************************************************************************
 * Plugin Related                                                          *
 ***************************************************************************/
orxPLUGIN_USER_CORE_FUNCTION_START(FILE);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxFile_LibC_Init, FILE, INIT);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxFile_LibC_Exit, FILE, EXIT);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxFile_LibC_Open, FILE, OPEN);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxFile_LibC_Read, FILE, READ);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxFile_LibC_Write, FILE, WRITE);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxFile_LibC_ReadLine, FILE, READ_LINE);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxFile_LibC_Close, FILE, CLOSE);
orxPLUGIN_USER_CORE_FUNCTION_END();
