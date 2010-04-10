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
 * @file orxFont.c
 * @date 08/03/2010
 * @author iarwain@orx-project.org
 *
 */


#include "display/orxFont.h"

#include "memory/orxBank.h"
#include "core/orxConfig.h"
#include "display/orxDisplay.h"
#include "object/orxStructure.h"
#include "utils/orxHashTable.h"


/** Module flags
 */
#define orxFONT_KU32_STATIC_FLAG_NONE           0x00000000  /**< No flags */

#define orxFONT_KU32_STATIC_FLAG_READY          0x00000001  /**< Ready flag */

#define orxFONT_KU32_STATIC_MASK_ALL            0xFFFFFFFF  /**< All mask */

/** orxFONT flags / masks
 */
#define orxFONT_KU32_FLAG_NONE                  0x00000000  /**< No flags */

#define orxFONT_KU32_FLAG_INTERNAL              0x10000000  /**< Internal structure handling flag  */
#define orxFONT_KU32_FLAG_REFERENCED            0x20000000  /**< Referenced flag */

#define orxFONT_KU32_MASK_ALL                   0xFFFFFFFF  /**< All mask */


/** Misc defines
 */
#define orxFONT_KU32_MAP_BANK_SIZE              2           /**< Map bank size */
#define orxFONT_KU32_REFERENCE_TABLE_SIZE       4           /**< Reference table size */

#define orxFONT_KZ_DEFAULT_FONT_NAME            "OrxDefaultFont"

#define orxFONT_KZ_CONFIG_TEXTURE_NAME          "Texture"
#define orxFONT_KZ_CONFIG_CHARACTER_LIST        "CharacterList"
#define orxFONT_KZ_CONFIG_CHARACTER_SIZE        "CharacterSize"
#define orxFONT_KZ_CONFIG_TEXTURE_CORNER        "TextureCorner"
#define orxFONT_KZ_CONFIG_TEXTURE_SIZE          "TextureSize"


/***************************************************************************
 * Structure declaration                                                   *
 ***************************************************************************/

/** Font structure
 */
struct __orxFONT_t
{
  orxSTRUCTURE      stStructure;                /**< Public structure, first structure member : 16 */
  orxCHARACTER_MAP *pstMap;                     /**< Font's map : 20 */
  orxVECTOR         vCharacterSize;             /**< Character size : 32 */
  orxTEXTURE       *pstTexture;                 /**< Texture : 36 */
  orxFLOAT          fTop;                       /**< Top coordinate : 40 */
  orxFLOAT          fLeft;                      /**< Left coordinate : 44 */
  orxFLOAT          fWidth;                     /**< Width : 48 */
  orxFLOAT          fHeight;                    /**< Height : 52 */
  const orxSTRING   zCharacterList;             /**< Character list : 56 */
  const orxSTRING   zReference;                 /**< Config reference : 60 */
};

/** Static structure
 */
typedef struct __orxFONT_STATIC_t
{
  orxBANK          *pstMapBank;                 /**< Map bank : 4 */
  orxHASHTABLE     *pstReferenceTable;          /**< Reference table : 8 */
  orxFONT          *pstDefaultFont;             /**< Default font : 12 */
  orxU32            u32Flags;                   /**< Control flags : 16 */

} orxFONT_STATIC;


/***************************************************************************
 * Module global variable                                                  *
 ***************************************************************************/

static orxFONT_STATIC sstFont;

#include "../src/display/orxDefaultFont.c"


/***************************************************************************
 * Private functions                                                       *
 ***************************************************************************/

/** Updates font's map
 * @param[in]   _pstFont       Concerned font
 */
static void orxFASTCALL orxFont_UpdateMap(orxFONT *_pstFont)
{
  orxU32 i;

  /* Check */
  orxSTRUCTURE_ASSERT(_pstFont);

  /* For all entries */
  for(i = 0; i < orxCHAR_NUMBER; i++)
  {
    /* Cleans it */
    _pstFont->pstMap->astCharacterList[i].fX = -orxFLOAT_1;
    _pstFont->pstMap->astCharacterList[i].fY = -orxFLOAT_1;
  }

  /* Has texture, texture size, character size and character list? */
  if((_pstFont->pstTexture != orxNULL)
  && (_pstFont->fWidth > orxFLOAT_0)
  && (_pstFont->fHeight > orxFLOAT_0)
  && (_pstFont->vCharacterSize.fX > orxFLOAT_0)
  && (_pstFont->vCharacterSize.fY > orxFLOAT_0)
  && (_pstFont->zCharacterList != orxSTRING_EMPTY))
  {
    const orxCHAR  *pc;
    orxVECTOR       vOrigin;

    /* For all defined characters */
    for(pc = _pstFont->zCharacterList, orxVector_Set(&vOrigin, _pstFont->fLeft, _pstFont->fTop, orxFLOAT_0); (*pc != orxCHAR_NULL) && (vOrigin.fY < _pstFont->fTop + _pstFont->fHeight); pc++)
    {
      /* Stores its origin */
      _pstFont->pstMap->astCharacterList[*pc].fX = vOrigin.fX;
      _pstFont->pstMap->astCharacterList[*pc].fY = vOrigin.fY;

      /* Updates current origin X value */
      vOrigin.fX += _pstFont->vCharacterSize.fX;

      /* Out of bound? */
      if(vOrigin.fX >= _pstFont->fLeft + _pstFont->fWidth)
      {
        /* Reinits its X value */
        vOrigin.fX = _pstFont->fLeft;

        /* Updates its Y value */
        vOrigin.fY += _pstFont->vCharacterSize.fY;
      }
    }

    /* Stores character size */
    orxVector_Copy(&(_pstFont->pstMap->vCharacterSize), &(_pstFont->vCharacterSize));

    /* Had more defined characters? */
    if(*pc != orxCHAR_NULL)
    {
      /* Logs message */
      orxDEBUG_PRINT(orxDEBUG_LEVEL_DISPLAY, "Too many characters defined for font <%s>: couldn't map characters [%s].", _pstFont->zReference, pc);
    }
  }

  return;
}

/* Creates default font
 */
static orxINLINE void orxFont_CreateDefaultFont()
{
  orxTEXTURE *pstTexture;

  /* Creates texture */
  pstTexture = orxTexture_Create();

  /* Success? */
  if(pstTexture != orxNULL)
  {
    orxBITMAP *pstBitmap;

    /* Creates bitmap */
    pstBitmap = orxDisplay_CreateBitmap(sstDefaultFont.u32Width, sstDefaultFont.u32Height);

    /* Success? */
    if(pstBitmap != orxNULL)
    {
      /* Sets it data */
      if(orxDisplay_SetBitmapData(pstBitmap, sstDefaultFont.au8Data, sstDefaultFont.u32Width * sstDefaultFont.u32Height * 4) != orxSTATUS_FAILURE)
      {
        /* Links it to texture */
        if(orxTexture_LinkBitmap(pstTexture, pstBitmap, orxFONT_KZ_DEFAULT_FONT_NAME) != orxSTATUS_FAILURE)
        {
          /* Creates default font */
          sstFont.pstDefaultFont = orxFont_Create();

          /* Success? */
          if(sstFont.pstDefaultFont != orxNULL)
          {
            /* Sets its texture */
            if(orxFont_SetTexture(sstFont.pstDefaultFont, pstTexture) != orxSTATUS_FAILURE)
            {
              orxVECTOR vSize;

              /* Inits it */
              orxFont_SetCharacterList(sstFont.pstDefaultFont, sstDefaultFont.zCharacterList);
              orxFont_SetCharacterSize(sstFont.pstDefaultFont, orxVector_Set(&vSize, orxU2F(sstDefaultFont.u32CharacterWidth), orxU2F(sstDefaultFont.u32CharacterHeight), orxFLOAT_0));

              /* Stores its reference key */
              sstFont.pstDefaultFont->zReference = orxFONT_KZ_DEFAULT_FONT_NAME;

              /* Adds it to reference table */
              orxHashTable_Add(sstFont.pstReferenceTable, orxString_ToCRC(sstFont.pstDefaultFont->zReference), sstFont.pstDefaultFont);

              /* Updates its flags */
              orxStructure_SetFlags(sstFont.pstDefaultFont, orxFONT_KU32_FLAG_REFERENCED | orxFONT_KU32_FLAG_INTERNAL, orxFONT_KU32_FLAG_NONE);
            }
            else
            {
              /* Logs message */
              orxDEBUG_PRINT(orxDEBUG_LEVEL_DISPLAY, "Can't set default font's texture.");

              /* Deletes font */
              orxFont_Delete(sstFont.pstDefaultFont);
              sstFont.pstDefaultFont = orxNULL;

              /* Unlinks bitmap */
              orxTexture_UnlinkBitmap(pstTexture);

              /* Deletes bitmap */
              orxDisplay_DeleteBitmap(pstBitmap);

              /* Deletes texture */
              orxTexture_Delete(pstTexture);
            }
          }
          else
          {
            /* Logs message */
            orxDEBUG_PRINT(orxDEBUG_LEVEL_DISPLAY, "Can't create default font.");

            /* Unlinks bitmap */
            orxTexture_UnlinkBitmap(pstTexture);
            
            /* Deletes bitmap */
            orxDisplay_DeleteBitmap(pstBitmap);
            
            /* Deletes texture */
            orxTexture_Delete(pstTexture);
          }
        }
        else
        {
          /* Logs message */
          orxDEBUG_PRINT(orxDEBUG_LEVEL_DISPLAY, "Can't link default font's bitmap to texture.");

          /* Deletes bitmap */
          orxDisplay_DeleteBitmap(pstBitmap);
          
          /* Deletes texture */
          orxTexture_Delete(pstTexture);
        }
      }
      else
      {
        /* Logs message */
        orxDEBUG_PRINT(orxDEBUG_LEVEL_DISPLAY, "Can't set default font's bitmap's data.");

        /* Deletes bitmap */
        orxDisplay_DeleteBitmap(pstBitmap);
        
        /* Deletes texture */
        orxTexture_Delete(pstTexture);
      }
    }
    else
    {
      /* Logs message */
      orxDEBUG_PRINT(orxDEBUG_LEVEL_DISPLAY, "Can't create default font's bitmap.");

      /* Deletes texture */
      orxTexture_Delete(pstTexture);
    }
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_DISPLAY, "Can't create default font's texture.");
  }
}

/** Deletes all fonts
 */
static orxINLINE void orxFont_DeleteAll()
{
  orxFONT *pstFont;

  /* Gets first font */
  pstFont = orxFONT(orxStructure_GetFirst(orxSTRUCTURE_ID_FONT));

  /* Non empty? */
  while(pstFont != orxNULL)
  {
    /* Deletes font */
    orxFont_Delete(pstFont);

    /* Gets first font */
    pstFont = orxFONT(orxStructure_GetFirst(orxSTRUCTURE_ID_FONT));
  }

  return;
}


/***************************************************************************
 * Public functions                                                        *
 ***************************************************************************/

/** Setups the font module
 */
void orxFASTCALL orxFont_Setup()
{
  /* Adds module dependencies */
  orxModule_AddDependency(orxMODULE_ID_FONT, orxMODULE_ID_MEMORY);
  orxModule_AddDependency(orxMODULE_ID_FONT, orxMODULE_ID_BANK);
  orxModule_AddDependency(orxMODULE_ID_FONT, orxMODULE_ID_CONFIG);
  orxModule_AddDependency(orxMODULE_ID_FONT, orxMODULE_ID_STRUCTURE);
  orxModule_AddDependency(orxMODULE_ID_FONT, orxMODULE_ID_TEXTURE);

  return;
}

/** Inits the font module
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxFont_Init()
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Not already Initialized? */
  if(!(sstFont.u32Flags & orxFONT_KU32_STATIC_FLAG_READY))
  {
    /* Cleans static controller */
    orxMemory_Zero(&sstFont, sizeof(orxFONT_STATIC));
    
    /* Creates reference table */
    sstFont.pstReferenceTable = orxHashTable_Create(orxFONT_KU32_REFERENCE_TABLE_SIZE, orxHASHTABLE_KU32_FLAG_NONE, orxMEMORY_TYPE_MAIN);

    /* Valid? */
    if(sstFont.pstReferenceTable != orxNULL)
    {
      /* Creates font map bank */
      sstFont.pstMapBank = orxBank_Create(orxFONT_KU32_MAP_BANK_SIZE, sizeof(orxCHARACTER_MAP), orxBANK_KU32_FLAG_NONE, orxMEMORY_TYPE_MAIN);

      /* Valid? */
      if(sstFont.pstMapBank != orxNULL)
      {
        /* Registers structure type */
        eResult = orxSTRUCTURE_REGISTER(FONT, orxSTRUCTURE_STORAGE_TYPE_LINKLIST, orxMEMORY_TYPE_MAIN, orxNULL);
      }
    }
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_DISPLAY, "Tried to initialize font module when it was already initialized.");

    /* Already initialized */
    eResult = orxSTATUS_SUCCESS;
  }

  /* Initialized? */
  if(eResult == orxSTATUS_SUCCESS)
  {
    /* Inits Flags */
    sstFont.u32Flags = orxFONT_KU32_STATIC_FLAG_READY;

    /* Creates default font */
    orxFont_CreateDefaultFont();
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_DISPLAY, "Initializing font module failed.");

    /* Has reference table? */
    if(sstFont.pstReferenceTable != orxNULL)
    {
      /* Deletes it */
      orxHashTable_Delete(sstFont.pstReferenceTable);
      sstFont.pstReferenceTable = orxNULL;
    }

    /* Has map bank? */
    if(sstFont.pstMapBank != orxNULL)
    {
      /* Deletes it */
      orxBank_Delete(sstFont.pstMapBank);
      sstFont.pstMapBank = orxNULL;
    }

    /* Updates Flags */
    sstFont.u32Flags &= ~orxFONT_KU32_STATIC_FLAG_READY;
  }

  /* Done! */
  return eResult;
}

/** Exits from the font module
 */
void orxFASTCALL orxFont_Exit()
{
  /* Initialized? */
  if(sstFont.u32Flags & orxFONT_KU32_STATIC_FLAG_READY)
  {
    /* Deletes font list */
    orxFont_DeleteAll();

    /* Deletes reference table */
    orxHashTable_Delete(sstFont.pstReferenceTable);
    sstFont.pstReferenceTable = orxNULL;

    /* Deletes map bank */
    orxBank_Delete(sstFont.pstMapBank);
    sstFont.pstMapBank = orxNULL;

    /* Unregisters structure type */
    orxStructure_Unregister(orxSTRUCTURE_ID_FONT);

    /* Updates flags */
    sstFont.u32Flags &= ~orxFONT_KU32_STATIC_FLAG_READY;
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_DISPLAY, "Tried to exit font module when it wasn't initialized.");
  }

  return;
}

/** Creates an empty font
 * @return      orxFONT / orxNULL
 */
orxFONT *orxFASTCALL orxFont_Create()
{
  orxFONT *pstResult;

  /* Checks */
  orxASSERT(sstFont.u32Flags & orxFONT_KU32_STATIC_FLAG_READY);

  /* Creates font */
  pstResult = orxFONT(orxStructure_Create(orxSTRUCTURE_ID_FONT));

  /* Created? */
  if(pstResult != orxNULL)
  {
    /* Allocates its map */
    pstResult->pstMap = (orxCHARACTER_MAP *)orxBank_Allocate(sstFont.pstMapBank);

    /* Valid? */
    if(pstResult->pstMap != orxNULL)
    {
      /* Clears its character list */
      pstResult->zCharacterList = orxSTRING_EMPTY;
    }
    else
    {
      /* Deletes structure */
      orxStructure_Delete(pstResult);

      /* Updates result */
      pstResult = orxNULL;

      /* Logs message */
      orxDEBUG_PRINT(orxDEBUG_LEVEL_DISPLAY, "Couldn't allocate font's map.");
    }
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_DISPLAY, "Failed to create structure for font.");
  }

  return pstResult;
}

/** Creates a font from config
 * @param[in]   _zConfigID    Config ID
 * @return      orxFONT / orxNULL
 */
orxFONT *orxFASTCALL orxFont_CreateFromConfig(const orxSTRING _zConfigID)
{
  orxFONT *pstResult;

  /* Checks */
  orxASSERT(sstFont.u32Flags & orxFONT_KU32_STATIC_FLAG_READY);
  orxASSERT((_zConfigID != orxNULL) && (_zConfigID != orxSTRING_EMPTY));

  /* Search for font */
  pstResult = (orxFONT *)orxHashTable_Get(sstFont.pstReferenceTable, orxString_ToCRC(_zConfigID));

  /* Not already created? */
  if(pstResult == orxNULL)
  {
    /* Pushes section */
    if((orxConfig_HasSection(_zConfigID) != orxFALSE)
    && (orxConfig_PushSection(_zConfigID) != orxSTATUS_FAILURE))
    {
      /* Creates font */
      pstResult = orxFont_Create();

      /* Valid? */
      if(pstResult != orxNULL)
      {
        const orxSTRING zName;

        /* Gets texture name */
        zName = orxConfig_GetString(orxFONT_KZ_CONFIG_TEXTURE_NAME);

        /* Valid? */
        if((zName != orxNULL) && (zName != orxSTRING_EMPTY))
        {
          orxTEXTURE *pstTexture;

          /* Creates texture */
          pstTexture = orxTexture_CreateFromFile(zName);

          /* Valid? */
          if(pstTexture != orxNULL)
          {
            /* Links it */
            if(orxFont_SetTexture(pstResult, pstTexture) != orxSTATUS_FAILURE)
            {
              orxVECTOR vCharacterSize;

              /* Updates flags */
              orxStructure_SetFlags(pstResult, orxFONT_KU32_FLAG_INTERNAL, orxFONT_KU32_MASK_ALL);

              /* Has corners? */
              if((orxConfig_HasValue(orxFONT_KZ_CONFIG_TEXTURE_CORNER) != orxFALSE)
              && (orxConfig_HasValue(orxFONT_KZ_CONFIG_TEXTURE_SIZE) != orxFALSE))
              {
                orxVECTOR vTextureCorner, vTextureSize;

                /* Gets both values */
                orxConfig_GetVector(orxFONT_KZ_CONFIG_TEXTURE_CORNER, &vTextureCorner);
                orxConfig_GetVector(orxFONT_KZ_CONFIG_TEXTURE_SIZE, &vTextureSize);

                /* Updates them */
                orxFont_SetOrigin(pstResult, &vTextureCorner);
                orxFont_SetSize(pstResult, &vTextureSize);
              }

              /* Gets character size */
              if(orxConfig_GetVector(orxFONT_KZ_CONFIG_CHARACTER_SIZE, &vCharacterSize) != orxNULL)
              {
                /* Sets it */
                if(orxFont_SetCharacterSize(pstResult, &vCharacterSize) != orxSTATUS_FAILURE)
                {
                  const orxSTRING zCharacterList;

                  /* Gets character list */
                  zCharacterList = orxConfig_GetString(orxFONT_KZ_CONFIG_CHARACTER_LIST);

                  /* Sets it */
                  if(orxFont_SetCharacterList(pstResult, zCharacterList) != orxSTATUS_FAILURE)
                  {
                    /* Stores its reference key */
                    pstResult->zReference = orxConfig_GetCurrentSection();

                    /* Protects it */
                    orxConfig_ProtectSection(pstResult->zReference, orxTRUE);

                    /* Adds it to reference table */
                    orxHashTable_Add(sstFont.pstReferenceTable, orxString_ToCRC(pstResult->zReference), pstResult);

                    /* Updates status flags */
                    orxStructure_SetFlags(pstResult, orxFONT_KU32_FLAG_REFERENCED, orxFONT_KU32_FLAG_NONE);
                  }
                  else
                  {
                    /* Logs message */
                    orxDEBUG_PRINT(orxDEBUG_LEVEL_DISPLAY, "Invalid character list (%s) for font (%s).", zCharacterList, _zConfigID);

                    /* Deletes structure */
                    orxFont_Delete(pstResult);

                    /* Updates result */
                    pstResult = orxNULL;
                  }
                }
                else
                {
                  /* Logs message */
                  orxDEBUG_PRINT(orxDEBUG_LEVEL_DISPLAY, "Invalid character size (%f, %f) for font (%s).", vCharacterSize.fX, vCharacterSize.fY, _zConfigID);

                  /* Deletes structure */
                  orxFont_Delete(pstResult);

                  /* Updates result */
                  pstResult = orxNULL;
                }
              }
              else
              {
                /* Logs message */
                orxDEBUG_PRINT(orxDEBUG_LEVEL_DISPLAY, "Couldn't find character size property for font (%s).", _zConfigID);

                /* Deletes structure */
                orxFont_Delete(pstResult);

                /* Updates result */
                pstResult = orxNULL;
              }
            }
            else
            {
              /* Logs message */
              orxDEBUG_PRINT(orxDEBUG_LEVEL_DISPLAY, "Couldn't link texture (%s) to font (%s).", zName, _zConfigID);

              /* Deletes structures */
              orxTexture_Delete(pstTexture);
              orxFont_Delete(pstResult);

              /* Updates result */
              pstResult = orxNULL;
            }
          }
          else
          {
            /* Logs message */
            orxDEBUG_PRINT(orxDEBUG_LEVEL_DISPLAY, "Couldn't create texture (%s) for font (%s).", zName, _zConfigID);

            /* Deletes structure */
            orxFont_Delete(pstResult);

            /* Updates result */
            pstResult = orxNULL;
          }
        }
        else
        {
          /* Logs message */
          orxDEBUG_PRINT(orxDEBUG_LEVEL_DISPLAY, "Couldn't find texture property for font (%s).", _zConfigID);

          /* Deletes structure */
          orxFont_Delete(pstResult);

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
      orxDEBUG_PRINT(orxDEBUG_LEVEL_DISPLAY, "Couldn't find config section named (%s).", _zConfigID);

      /* Updates result */
      pstResult = orxNULL;
    }
  }

  /* Done! */
  return pstResult;
}

/** Deletes a font
 * @param[in]   _pstFont      Concerned font
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxFont_Delete(orxFONT *_pstFont)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstFont.u32Flags & orxFONT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstFont);

  /* Not referenced? */
  if(orxStructure_GetRefCounter(_pstFont) == 0)
  {
    /* Removes texture */
    orxFont_SetTexture(_pstFont, orxNULL);

    /* Deletes map */
    orxBank_Free(sstFont.pstMapBank, _pstFont->pstMap);

    /* Is referenced? */
    if(orxStructure_TestFlags(_pstFont, orxFONT_KU32_FLAG_REFERENCED) != orxFALSE)
    {
      /* Removes it from reference table */
      orxHashTable_Remove(sstFont.pstReferenceTable, orxString_ToCRC(_pstFont->zReference));
    }

    /* Has reference? */
    if(_pstFont->zReference != orxNULL)
    {
      /* Unprotects it */
      orxConfig_ProtectSection(_pstFont->zReference, orxFALSE);
    }

    /* Deletes structure */
    orxStructure_Delete(_pstFont);
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_DISPLAY, "Tried to delete font object when it was still referenced.");

    /* Referenced by others */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Gets default font
 * @return      Default font / orxNULL
 */
const orxFONT *orxFASTCALL orxFont_GetDefaultFont()
{
  orxFONT *pstResult;

  /* Checks */
  orxASSERT(sstFont.u32Flags & orxFONT_KU32_STATIC_FLAG_READY);

  /* Updates result */
  pstResult = sstFont.pstDefaultFont;

  /* Done ! */
  return pstResult;
}

/** Sets font's texture
 * @param[in]   _pstFont      Concerned font
 * @param[in]   _pstTexture   Texture to set
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxFont_SetTexture(orxFONT *_pstFont, orxTEXTURE *_pstTexture)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstFont.u32Flags & orxFONT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstFont);

  /* Had previous texture? */
  if(_pstFont->pstTexture != orxNULL)
  {
    /* Updates structure reference counter */
    orxStructure_DecreaseCounter(_pstFont->pstTexture);

    /* Internally handled? */
    if(orxStructure_TestFlags(_pstFont, orxFONT_KU32_FLAG_INTERNAL))
    {
      /* Deletes it */
      orxTexture_Delete(_pstFont->pstTexture);

      /* Updates flags */
      orxStructure_SetFlags(_pstFont, orxFONT_KU32_FLAG_NONE, orxFONT_KU32_FLAG_INTERNAL);
    }

    /* Cleans reference */
    _pstFont->pstTexture = orxNULL;

    /* Clears origin & size */
    _pstFont->fTop = _pstFont->fLeft = _pstFont->fWidth = _pstFont->fHeight = orxFLOAT_0;
  }

  /* New texture? */
  if(_pstTexture != orxNULL)
  {
    /* Stores it */
    _pstFont->pstTexture = _pstTexture;

    /* Updates its reference counter */
    orxStructure_IncreaseCounter(_pstTexture);

    /* Updates font's size */
    orxTexture_GetSize(_pstTexture, &(_pstFont->fWidth), &(_pstFont->fHeight));
  }

  /* Updates font's map */
  orxFont_UpdateMap(_pstFont);

  /* Done! */
  return eResult;
}

/** Sets font's character list
 * @param[in]   _pstFont      Concerned font
 * @param[in]   _zList        Character list
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxFont_SetCharacterList(orxFONT *_pstFont, const orxSTRING _zList)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstFont.u32Flags & orxFONT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstFont);

  /* Had a character list? */
  if(_pstFont->zCharacterList != orxSTRING_EMPTY)
  {
    /* Deletes it */
    orxString_Delete((orxSTRING)_pstFont->zCharacterList);

    /* Cleans its reference */
    _pstFont->zCharacterList = orxSTRING_EMPTY;
  }
  
  /* Valid? */
  if((_zList != orxNULL) && (_zList != orxSTRING_EMPTY))
  {
    /* Stores it */
    _pstFont->zCharacterList = orxString_Duplicate(_zList);
  }
  else
  {
    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Updates font's map */
  orxFont_UpdateMap(_pstFont);

  /* Done! */
  return eResult;
}

/** Sets font's character size
 * @param[in]   _pstFont      Concerned font
 * @param[in]   _pvSize       Character's size
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxFont_SetCharacterSize(orxFONT *_pstFont, const orxVECTOR *_pvSize)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstFont.u32Flags & orxFONT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstFont);
  orxASSERT(_pvSize != orxNULL);

  /* Valid? */
  if((_pvSize->fX > orxFLOAT_0) && (_pvSize->fY > orxFLOAT_0))
  {
    /* Stores it */
    orxVector_Set(&(_pstFont->vCharacterSize), _pvSize->fX, _pvSize->fY, orxFLOAT_0);

    /* Updates font's map */
    orxFont_UpdateMap(_pstFont);
  }
  else
  {
    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Sets font's origin
 * @param[in]   _pstFont      Concerned font
 * @param[in]   _pvOrigin     Font's origin
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxFont_SetOrigin(orxFONT *_pstFont, const orxVECTOR *_pvOrigin)
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Checks */
  orxASSERT(sstFont.u32Flags & orxFONT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstFont);
  orxASSERT(_pvOrigin != orxNULL);

  /* Has texture? */
  if(_pstFont->pstTexture)
  {
    orxFLOAT fWidth, fHeight;

    /* Gets its size */
    orxTexture_GetSize(_pstFont->pstTexture, &fWidth, &fHeight);

    /* Valid? */
    if((_pvOrigin->fX >= orxFLOAT_0)
    && (_pvOrigin->fX < fWidth)
    && (_pvOrigin->fY >= orxFLOAT_0)
    && (_pvOrigin->fY < fHeight))
    {
      /* Stores it */
      _pstFont->fLeft = _pvOrigin->fX;
      _pstFont->fTop  = _pvOrigin->fY;

      /* Updates font's map */
      orxFont_UpdateMap(_pstFont);

      /* Updates result */
      eResult = orxSTATUS_SUCCESS;
    }
  }

  /* Done! */
  return eResult;
}

/** Sets font's size
 * @param[in]   _pstFont      Concerned font
 * @param[in]   _pvSize       Font's size
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxFont_SetSize(orxFONT *_pstFont, const orxVECTOR *_pvSize)
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Checks */
  orxASSERT(sstFont.u32Flags & orxFONT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstFont);
  orxASSERT(_pvSize != orxNULL);

  /* Has texture? */
  if(_pstFont->pstTexture)
  {
    orxFLOAT fWidth, fHeight;

    /* Gets its size */
    orxTexture_GetSize(_pstFont->pstTexture, &fWidth, &fHeight);

    /* Valid? */
    if((_pvSize->fX > orxFLOAT_0)
    && (_pvSize->fX <= fWidth)
    && (_pvSize->fY > orxFLOAT_0)
    && (_pvSize->fY <= fHeight))
    {
      /* Stores it */
      _pstFont->fWidth  = _pvSize->fX;
      _pstFont->fHeight = _pvSize->fY;

      /* Updates font's map */
      orxFont_UpdateMap(_pstFont);

      /* Updates result */
      eResult = orxSTATUS_SUCCESS;
    }
  }

  /* Done! */
  return eResult;
}

/** Gets font's texture
 * @param[in]   _pstFont      Concerned font
 * @return      Font texture / orxNULL
 */
orxTEXTURE *orxFASTCALL orxFont_GetTexture(const orxFONT *_pstFont)
{
  orxTEXTURE *pstResult;

  /* Checks */
  orxASSERT(sstFont.u32Flags & orxFONT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstFont);

  /* Updates result */
  pstResult = _pstFont->pstTexture;

  /* Done! */
  return pstResult;
}

/** Gets font's character list
 * @param[in]   _pstFont      Concerned font
 * @return      Font's character list / orxNULL
 */
const orxSTRING orxFASTCALL orxFont_GetCharacterList(const orxFONT *_pstFont)
{
  const orxSTRING zResult;

  /* Checks */
  orxASSERT(sstFont.u32Flags & orxFONT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstFont);

  /* Updates result */
  zResult = _pstFont->zCharacterList;

  /* Done! */
  return zResult;
}

/** Gets font's character size
 * @param[in]   _pstFont      Concerned font
 * @param[out]  _pvSize       Character's size
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxVECTOR *orxFASTCALL orxFont_GetCharacterSize(const orxFONT *_pstFont, orxVECTOR *_pvSize)
{
  orxVECTOR *pvResult = _pvSize;

  /* Checks */
  orxASSERT(sstFont.u32Flags & orxFONT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstFont);
  orxASSERT(_pvSize != orxNULL);

  /* Updates result */
  orxVector_Copy(pvResult, &(_pstFont->vCharacterSize));

  /* Done! */
  return pvResult;
}

/** Gets font's origin
 * @param[in]   _pstFont      Concerned font
 * @param[out]  _pvOrigin     Font's origin
 * @return      orxDISPLAY_FONT / orxNULL
 */
orxVECTOR *orxFASTCALL orxFont_GetOrigin(const orxFONT *_pstFont, orxVECTOR *_pvOrigin)
{
  orxVECTOR *pvResult = _pvOrigin;

  /* Checks */
  orxASSERT(sstFont.u32Flags & orxFONT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstFont);
  orxASSERT(_pvOrigin != orxNULL);

  /* Updates result */
  orxVector_Set(pvResult, _pstFont->fLeft, _pstFont->fTop, orxFLOAT_0);

  /* Done! */
  return pvResult;
}

/** Gets font's size
 * @param[in]   _pstFont      Concerned font
 * @param[out]  _pvSize       Font's size
 * @return      orxDISPLAY_FONT / orxNULL
 */
orxVECTOR *orxFASTCALL orxFont_GetSize(const orxFONT *_pstFont, orxVECTOR *_pvSize)
{
  orxVECTOR *pvResult = _pvSize;

  /* Checks */
  orxASSERT(sstFont.u32Flags & orxFONT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstFont);
  orxASSERT(_pvSize != orxNULL);

  /* Updates result */
  orxVector_Set(pvResult, _pstFont->fWidth, _pstFont->fHeight, orxFLOAT_0);

  /* Done! */
  return pvResult;
}

/** Gets font's map
 * @param[in]   _pstFont      Concerned font
 * @return      orxCHARACTER_MAP / orxNULL
 */
const orxCHARACTER_MAP *orxFASTCALL orxFont_GetMap(const orxFONT *_pstFont)
{
  const orxCHARACTER_MAP *pstResult;

  /* Checks */
  orxASSERT(sstFont.u32Flags & orxFONT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstFont);

  /* Updates result */
  pstResult = _pstFont->pstMap;

  /* Done! */
  return pstResult;
}

/** Gets font name
 * @param[in]   _pstFont      Concerned font
 * @return      Font name / orxSTRING_EMPTY
 */
const orxSTRING orxFASTCALL orxFont_GetName(const orxFONT *_pstFont)
{
  const orxSTRING zResult;

  /* Checks */
  orxASSERT(sstFont.u32Flags & orxFONT_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstFont);

  /* Updates result */
  zResult = (_pstFont->zReference != orxNULL) ? _pstFont->zReference : orxSTRING_EMPTY;

  /* Done! */
  return zResult;
}
