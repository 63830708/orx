/***************************************************************************
 orxStructure.c
 Structure module
 
 begin                : 08/12/2003
 author               : (C) Arcallians
 email                : iarwain@arcallians.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "object/orxStructure.h"

#include "memory/orxBank.h"
#include "utils/orxLinkList.h"
#include "utils/orxTree.h"


/*
 * Platform independant defines
 */

#define orxSTRUCTURE_KU32_STATIC_FLAG_NONE    0x00000000
#define orxSTRUCTURE_KU32_STATIC_FLAG_READY   0x00000001

/* *** Misc *** */
#define orxSTRUCTURE_KU32_STORAGE_BANK_SIZE   256

#define orxSTRUCTURE_KU32_STRUCTURE_BANK_SIZE 32


/*
 * Internal storage structure
 */
typedef struct __orxSTRUCTURE_STORAGE_t
{
  /* Storage type : 4 */
  orxSTRUCTURE_STORAGE_TYPE eType;

  /* Associated node bank : 8 */
  orxBANK *pstNodeBank;

  /* Associated structure bank : 12 */
  orxBANK *pstStructureBank;

  /* Storage union : 24 */
  union
  {
    /* Link List */
    orxLINKLIST stLinkList;

    /* Tree */
    orxTREE stTree;
  };

} orxSTRUCTURE_STORAGE;

/*
 * Internal registration info
 */
typedef struct __orxSTRUCTURE_REGISTER_INFO_t
{
  /* Structure storage type : 4 */
  orxSTRUCTURE_STORAGE_TYPE eStorageType;

  /* Structure storage size : 8 */
  orxU32 u32Size;

  /* Structure storage memory type : 12 */
  orxMEMORY_TYPE eMemoryType;

  /* Structure update callbacks : 16 */
  orxSTRUCTURE_UPDATE_FUNCTION pfnUpdate;

} orxSTRUCTURE_REGISTER_INFO;

/*
 * Internal storage node
 */
typedef struct __orxSTRUCTURE_STORAGE_NODE_t
{
  /* Storage node union : 16 */
  union
  {    
    /* Link list node */
    orxLINKLIST_NODE stLinkListNode;

    /* Tree node */
    orxTREE_NODE stTreeNode;
  };

  /* Pointer to structure : 20 */
  orxSTRUCTURE *pstStructure;

  /* Storage type : 24 */
  orxSTRUCTURE_STORAGE_TYPE eType;

} orxSTRUCTURE_STORAGE_NODE;

/*
 * Static structure
 */
typedef struct __orxSTRUCTURE_STATIC_t
{
  /* Structure banks */
  orxSTRUCTURE_STORAGE astStorage[orxSTRUCTURE_ID_NUMBER];

  /* Structure info */
  orxSTRUCTURE_REGISTER_INFO astInfo[orxSTRUCTURE_ID_NUMBER];

  /* Control flags */
  orxU32 u32Flags;

} orxSTRUCTURE_STATIC;

/*
 * Static data
 */
orxSTATIC orxSTRUCTURE_STATIC sstStructure;


/***************************************************************************
 ***************************************************************************
 ******                       LOCAL FUNCTIONS                         ******
 ***************************************************************************
 ***************************************************************************/


/***************************************************************************
 ***************************************************************************
 ******                       PUBLIC FUNCTIONS                        ******
 ***************************************************************************
 ***************************************************************************/

/***************************************************************************
 orxStructure_Setup
 Structure module setup.

 returns: nothing
 ***************************************************************************/
orxVOID orxStructure_Setup()
{
  /* Adds module dependencies */
  orxModule_AddDependency(orxMODULE_ID_STRUCTURE, orxMODULE_ID_MEMORY);
  orxModule_AddDependency(orxMODULE_ID_STRUCTURE, orxMODULE_ID_BANK);
  orxModule_AddDependency(orxMODULE_ID_STRUCTURE, orxMODULE_ID_LINKLIST);
  orxModule_AddDependency(orxMODULE_ID_STRUCTURE, orxMODULE_ID_TREE);

  return;
}

/***************************************************************************
 orxStructure_Init
 Inits structure system.

 returns: orxSTATUS_SUCCESS/orxSTATUS_FAILURE
 ***************************************************************************/
orxSTATUS orxStructure_Init()
{
  orxU32 i;
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Not already Initialized? */
  if(!(sstStructure.u32Flags & orxSTRUCTURE_KU32_STATIC_FLAG_READY))
  {
    /* Cleans static controller */
    orxMemory_Set(&sstStructure, 0, sizeof(orxSTRUCTURE_STATIC));

    /* For all IDs */
    for(i = 0; i < orxSTRUCTURE_ID_NUMBER; i++)
    {
      /* Creates a bank */
      sstStructure.astStorage[i].pstNodeBank  = orxBank_Create(orxSTRUCTURE_KU32_STORAGE_BANK_SIZE, sizeof(orxSTRUCTURE_STORAGE_NODE), orxBANK_KU32_FLAG_NONE, orxMEMORY_TYPE_MAIN);

      /* Cleans storage type */
      sstStructure.astStorage[i].eType    = orxSTRUCTURE_STORAGE_TYPE_NONE;
    }

    /* All banks created? */
    if(i == orxSTRUCTURE_ID_NUMBER)
    {
      /* Inits Flags */
      sstStructure.u32Flags = orxSTRUCTURE_KU32_STATIC_FLAG_READY;

      /* Everything's ok */
      eResult = orxSTATUS_SUCCESS;
    }
    else
    {
      orxU32 j;

      /* !!! MSG !!! */

      /* For all created banks */
      for(j = 0; j < i; j++)
      {
        /* Deletes it */
        orxBank_Delete(sstStructure.astStorage[j].pstNodeBank);
      }
    }
  }
  else
  {
    /* !!! MSG !!! */

    /* Already initialized */
    eResult = orxSTATUS_SUCCESS;
  }

  /* Done! */
  return eResult;
}

/***************************************************************************
 orxStructure_Exit
 Exits from the structure system.

 returns: orxVOID
 ***************************************************************************/
orxVOID orxStructure_Exit()
{
  /* Initialized? */
  if(sstStructure.u32Flags & orxSTRUCTURE_KU32_STATIC_FLAG_READY)
  {
    orxU32 i;

    /* For all banks */
    for(i = 0; i < orxSTRUCTURE_ID_NUMBER; i++)
    {
      /* Depending on storage type */
      switch(sstStructure.astStorage[i].eType)
      {
        case orxSTRUCTURE_STORAGE_TYPE_LINKLIST:

        /* Empties list */
        orxLinkList_Clean(&(sstStructure.astStorage[i].stLinkList));

        break;
      
      case orxSTRUCTURE_STORAGE_TYPE_TREE:

        /* Empties tree */
        orxTree_Clean(&(sstStructure.astStorage[i].stTree));

        break;

      default:

        break;
      }

      /* Deletes banks */
      orxBank_Delete(sstStructure.astStorage[i].pstNodeBank);
      if(sstStructure.astStorage[i].pstStructureBank != orxNULL)
      {
        orxBank_Delete(sstStructure.astStorage[i].pstStructureBank);
      }
    }

    /* Updates flags */
    sstStructure.u32Flags &= ~orxSTRUCTURE_KU32_STATIC_FLAG_READY;
  }
  else
  {
    /* !!! MSG !!! */
  }

  return;
}

/***************************************************************************
 orxStructure_Register
 Registers a structure type.

 returns: orxSTATUS_SUCCESS/orxSTATUS_FAILURE
 ***************************************************************************/
orxSTATUS orxFASTCALL orxStructure_Register(orxSTRUCTURE_ID _eStructureID, orxSTRUCTURE_STORAGE_TYPE _eStorageType, orxMEMORY_TYPE _eMemoryType, orxU32 _u32Size, orxCONST orxSTRUCTURE_UPDATE_FUNCTION _pfnUpdate)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstStructure.u32Flags & orxSTRUCTURE_KU32_STATIC_FLAG_READY);
  orxASSERT(_eStructureID < orxSTRUCTURE_ID_NUMBER);
  orxASSERT(_u32Size != 0);
  orxASSERT(_eStorageType < orxSTRUCTURE_STORAGE_TYPE_NUMBER);
  orxASSERT(_eMemoryType < orxMEMORY_TYPE_NUMBER);

  /* Not already registered? */
  if(sstStructure.astInfo[_eStructureID].u32Size == 0)
  {
    /* Creates bank for structure storage */
    sstStructure.astStorage[_eStructureID].pstStructureBank = orxBank_Create(orxSTRUCTURE_KU32_STRUCTURE_BANK_SIZE, _u32Size, orxBANK_KU32_FLAG_NONE, _eMemoryType);

    /* Valid? */
    if(sstStructure.astStorage[_eStructureID].pstStructureBank != orxNULL)
    {
      /* Registers it */
      sstStructure.astInfo[_eStructureID].eStorageType  = _eStorageType;
      sstStructure.astInfo[_eStructureID].eMemoryType   = _eMemoryType;
      sstStructure.astInfo[_eStructureID].u32Size       = _u32Size;
      sstStructure.astInfo[_eStructureID].pfnUpdate     = _pfnUpdate;

      sstStructure.astStorage[_eStructureID].eType      = _eStorageType;
    }
    else
    {
      /* !!! MSG !!! */
    }
  }
  else
  {
    /* !!! MSG !!! */

    /* Already registered */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/***************************************************************************
 orxStructure_Unregister
 Unregisters a structure type.

 returns: orxVOID
 ***************************************************************************/
orxVOID orxFASTCALL orxStructure_Unregister(orxSTRUCTURE_ID _eStructureID)
{
  /* Checks */
  orxASSERT(sstStructure.u32Flags & orxSTRUCTURE_KU32_STATIC_FLAG_READY);
  orxASSERT(_eStructureID < orxSTRUCTURE_ID_NUMBER);

  /* Registered? */
  if(sstStructure.astInfo[_eStructureID].u32Size != 0)
  {
    /* Deletes structure storage bank */
    orxBank_Delete(sstStructure.astStorage[_eStructureID].pstStructureBank);

    /* Unregisters it */
    orxMemory_Set(&(sstStructure.astInfo[_eStructureID]), 0, sizeof(orxSTRUCTURE_REGISTER_INFO));
    sstStructure.astStorage[_eStructureID].pstStructureBank = orxNULL;
    sstStructure.astStorage[_eStructureID].eType            = orxSTRUCTURE_STORAGE_TYPE_NONE;
  }
  else
  {
    /* !!! MSG !!! */
  }

  return;
}

/***************************************************************************
 orxStructure_GetStorageType
 Gets structure storage type.

 returns: Structure storage ID
 ***************************************************************************/
orxSTRUCTURE_STORAGE_TYPE orxFASTCALL orxStructure_GetStorageType(orxSTRUCTURE_ID _eStructureID)
{
  /* Checks */
  orxASSERT(sstStructure.u32Flags & orxSTRUCTURE_KU32_STATIC_FLAG_READY);
  orxASSERT(_eStructureID < orxSTRUCTURE_ID_NUMBER);

  /* Returns it */
  return(sstStructure.astStorage[_eStructureID].eType);
}

/***************************************************************************
 orxStructure_GetNumber
 Gets given type structure number.

 returns: number / orxU32_UNDEFINED
 ***************************************************************************/
orxU32 orxFASTCALL orxStructure_GetNumber(orxSTRUCTURE_ID _eStructureID)
{
  orxREGISTER orxU32 u32Result = orxU32_UNDEFINED;

  /* Checks */
  orxASSERT(sstStructure.u32Flags & orxSTRUCTURE_KU32_STATIC_FLAG_READY);
  orxASSERT(_eStructureID < orxSTRUCTURE_ID_NUMBER);

  /* Dependig on type */
  switch(sstStructure.astStorage[_eStructureID].eType)
  {
  case orxSTRUCTURE_STORAGE_TYPE_LINKLIST:

    /* Gets counter */
    u32Result = orxLinkList_GetCounter(&(sstStructure.astStorage[_eStructureID].stLinkList));

    break;

  case orxSTRUCTURE_STORAGE_TYPE_TREE:

    /* Gets counter */
    u32Result = orxTree_GetCounter(&(sstStructure.astStorage[_eStructureID].stTree));

    break;

  default:

    /* !!! MSG !!! */

    break;
  }

  /* Done ! */
  return u32Result;
}

/***************************************************************************
 orxStructure_Create
 Creates a clean structure for given type.

 returns: orxSTRUCTURE pointer / orxNULL
 ***************************************************************************/
orxSTRUCTURE *orxFASTCALL orxStructure_Create(orxSTRUCTURE_ID _eStructureID)
{
  orxREGISTER orxSTRUCTURE_STORAGE_NODE *pstNode;
  orxREGISTER orxSTRUCTURE *pstStructure = orxNULL;

  /* Checks */
  orxASSERT(sstStructure.u32Flags & orxSTRUCTURE_KU32_STATIC_FLAG_READY);
  orxASSERT(_eStructureID < orxSTRUCTURE_ID_NUMBER);

  /* Is structure type registered? */
  if(sstStructure.astInfo[_eStructureID].u32Size != 0)
  {
    /* Creates structure */
    pstStructure = (orxSTRUCTURE *)orxBank_Allocate(sstStructure.astStorage[_eStructureID].pstStructureBank);

    /* Valid? */
    if(pstStructure != orxNULL)
    {
      /* Creates node */
      pstNode = (orxSTRUCTURE_STORAGE_NODE *)orxBank_Allocate(sstStructure.astStorage[_eStructureID].pstNodeBank);
  
      /* Valid? */
      if(pstNode != orxNULL)
      {
        orxSTATUS eResult;

        /* Cleans it */
        orxMemory_Set(pstNode, 0, sizeof(orxSTRUCTURE_STORAGE_NODE));

        /* Stores its type */
        pstNode->eType = sstStructure.astStorage[_eStructureID].eType;

        /* Dependig on type */
        switch(pstNode->eType)
        {
        case orxSTRUCTURE_STORAGE_TYPE_LINKLIST:
    
          /* Adds node to list */
          eResult = orxLinkList_AddStart(&(sstStructure.astStorage[_eStructureID].stLinkList), &(pstNode->stLinkListNode));

          break;

        case orxSTRUCTURE_STORAGE_TYPE_TREE:

          /* No root yet? */
          if(orxTree_GetRoot(&(sstStructure.astStorage[_eStructureID].stTree)) == orxNULL)
          {
            /* Adds root to tree */
            eResult = orxTree_AddRoot(&(sstStructure.astStorage[_eStructureID].stTree), &(pstNode->stTreeNode));
          }
          else
          {
            /* Adds node to tree */
            eResult = orxTree_AddChild(orxTree_GetRoot(&(sstStructure.astStorage[_eStructureID].stTree)), &(pstNode->stTreeNode));
          }

          break;
    
        default:
    
          /* !!! MSG !!! */
    
          /* Wrong type */
          eResult = orxSTATUS_FAILURE;
        }
        
        /* Succesful? */
        if(eResult == orxSTATUS_SUCCESS)
        {
          /* Cleans whole structure */
          orxMemory_Set(pstStructure, 0, sstStructure.astInfo[_eStructureID].u32Size);
          
          /* Stores ID with magic number */
          pstStructure->eID           = _eStructureID | orxSTRUCTURE_MAGIC_NUMBER;

          /* Stores storage handle */
          pstStructure->hStorageNode  = (orxHANDLE)pstNode;

          /* Stores structure pointer */
          pstNode->pstStructure       = pstStructure;
        }
        else
        {
          /* !!! MSG !!! */
    
          /* Frees allocated node & structure */
          orxBank_Free(sstStructure.astStorage[_eStructureID].pstNodeBank, pstNode);
          orxBank_Free(sstStructure.astStorage[_eStructureID].pstStructureBank, pstStructure);

          /* Not created */
          pstStructure = orxNULL;
        }        
      }
      else
      {
        /* !!! MSG !!! */
        
        /* Frees allocated structure */
        orxBank_Free(sstStructure.astStorage[_eStructureID].pstStructureBank, pstStructure);

        /* Not allocated */
        pstStructure = orxNULL;
      }
    }
    else
    {
      /* !!! MSG !!! */
    }
  }
  else
  {
    /* !!! MSG !!! */
  }

  /* Done! */
  return pstStructure;
}

/***************************************************************************
 orxStructure_Delete
 Deletes a structure (needs to be cleaned before).

 returns: orxVOID
 ***************************************************************************/
orxVOID orxFASTCALL orxStructure_Delete(orxHANDLE _phStructure)
{
  orxREGISTER orxSTRUCTURE_STORAGE_NODE *pstNode;

  /* Checks */
  orxASSERT(sstStructure.u32Flags & orxSTRUCTURE_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_phStructure);

  /* Gets storage node */
  pstNode = (orxSTRUCTURE_STORAGE_NODE *)((orxSTRUCTURE *)_phStructure)->hStorageNode;

  /* Valid? */
  if(pstNode != orxNULL)
  {
    /* Dependig on type */
    switch(sstStructure.astStorage[orxStructure_GetID(_phStructure)].eType)
    {
    case orxSTRUCTURE_STORAGE_TYPE_LINKLIST:

      /* Removes node from list */
      orxLinkList_Remove(&(pstNode->stLinkListNode));

    break;

    case orxSTRUCTURE_STORAGE_TYPE_TREE:

      /* Removes node from list */
      orxTree_Remove(&(pstNode->stTreeNode));

    break;

      default:
        /* !!! MSG !!! */
        break;
    }

    /* Deletes it */
    orxBank_Free(sstStructure.astStorage[orxStructure_GetID(_phStructure)].pstNodeBank, pstNode);

    /* Deletes structure */
    orxBank_Free(sstStructure.astStorage[orxStructure_GetID(_phStructure)].pstStructureBank, _phStructure);

    /* Cleans structure */
    orxMemory_Set(_phStructure, 0, sizeof(orxSTRUCTURE));
  }
  else
  {
    /* !!! MSG !!! */
  }

  return;
}

/***************************************************************************
 orxStructure_Update
 Updates structure if update function was registered for the structure type.

 returns: orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 ***************************************************************************/
orxSTATUS orxFASTCALL orxStructure_Update(orxHANDLE _phStructure, orxCONST orxHANDLE _phCaller, orxCONST orxCLOCK_INFO *_pstClockInfo)
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Checks */
  orxASSERT(sstStructure.u32Flags & orxSTRUCTURE_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_phStructure);
  orxASSERT(_pstClockInfo != orxNULL);

  /* Is structure registered? */
  if(sstStructure.astInfo[orxStructure_GetID(_phStructure)].u32Size != 0)
  {
    /* Is an update function registered? */
    if(sstStructure.astInfo[orxStructure_GetID(_phStructure)].pfnUpdate != orxNULL)
    {
      /* Calls it */
      eResult = sstStructure.astInfo[orxStructure_GetID(_phStructure)].pfnUpdate(((orxSTRUCTURE *)_phStructure), ((orxSTRUCTURE *)_phCaller), _pstClockInfo);
    }
    else
    {
      /* !!! MSG !!! */
    }
  }
  else
  {
    /* !!! MSG !!! */
  }

  /* Done! */
  return eResult;
}


/* *** Structure accessors *** */


/***************************************************************************
 orxStructure_GetFirst
 Gets first stored structure (first list cell or tree root depending on storage type).

 returns: first structure
 ***************************************************************************/
orxSTRUCTURE *orxFASTCALL orxStructure_GetFirst(orxSTRUCTURE_ID _eStructureID)
{
  orxREGISTER orxSTRUCTURE_STORAGE_NODE *pstNode;
  orxREGISTER orxSTRUCTURE *pstStructure = orxNULL;

  /* Checks */
  orxASSERT(sstStructure.u32Flags & orxSTRUCTURE_KU32_STATIC_FLAG_READY);
  orxASSERT(_eStructureID < orxSTRUCTURE_ID_NUMBER);

  /* Depending on type */
  switch(sstStructure.astStorage[_eStructureID].eType)
  {
  case orxSTRUCTURE_STORAGE_TYPE_LINKLIST:

    /* Gets node from list */
    pstNode = (orxSTRUCTURE_STORAGE_NODE *)orxLinkList_GetFirst(&(sstStructure.astStorage[_eStructureID].stLinkList));

    break;

  case orxSTRUCTURE_STORAGE_TYPE_TREE:

    /* Gets node from tree */
    pstNode = (orxSTRUCTURE_STORAGE_NODE *)orxTree_GetRoot(&(sstStructure.astStorage[_eStructureID].stTree));

    break;

  default:

    /* !!! MSG !!! */

    /* No node found */
    pstNode = orxNULL;

    break;
  }

  /* Node found? */
  if(pstNode != orxNULL)
  {
    /* Gets associated structure */
    pstStructure = pstNode->pstStructure;
  }

  /* Done! */
  return pstStructure;
}

/***************************************************************************
 orxStructure_GetParent
 Structure tree parent get accessor.

 returns: parent
 ***************************************************************************/
orxSTRUCTURE *orxFASTCALL orxStructure_GetParent(orxCONST orxHANDLE _phStructure)
{
  orxREGISTER orxSTRUCTURE_STORAGE_NODE *pstNode;
  orxREGISTER orxSTRUCTURE *pstStructure = orxNULL;

  /* Checks */
  orxASSERT(sstStructure.u32Flags & orxSTRUCTURE_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_phStructure);

  /* Gets node */
  pstNode = (orxSTRUCTURE_STORAGE_NODE *)((orxSTRUCTURE *)_phStructure)->hStorageNode;

  /* Valid? */
  if(pstNode != orxNULL)
  {
    /* Is storate type correct? */
    if(pstNode->eType == orxSTRUCTURE_STORAGE_TYPE_TREE)
    {
      /* Gets parent node */
      pstNode = (orxSTRUCTURE_STORAGE_NODE *)orxTree_GetParent(&(pstNode->stTreeNode));

      /* Valid? */
      if(pstNode != orxNULL)
      {
        /* Gets structure */
        pstStructure = pstNode->pstStructure;
      }
    }
    else
    {
      /* !!! MSG !!! */
    }
  }
  else
  {
    /* !!! MSG !!! */
  }

  /* Done! */
  return pstStructure;
}

/***************************************************************************
 orxStructure_GetChild
 Structure tree child get accessor.

 returns: child
 ***************************************************************************/
orxSTRUCTURE *orxFASTCALL orxStructure_GetChild(orxCONST orxHANDLE _phStructure)
{
  orxREGISTER orxSTRUCTURE_STORAGE_NODE *pstNode;
  orxREGISTER orxSTRUCTURE *pstStructure = orxNULL;

  /* Checks */
  orxASSERT(sstStructure.u32Flags & orxSTRUCTURE_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_phStructure);

  /* Gets node */
  pstNode = (orxSTRUCTURE_STORAGE_NODE *)((orxSTRUCTURE *)_phStructure)->hStorageNode;

  /* Valid? */
  if(pstNode != orxNULL)
  {
    /* Is storate type correct? */
    if(pstNode->eType == orxSTRUCTURE_STORAGE_TYPE_TREE)
    {
      /* Gets child node */
      pstNode = (orxSTRUCTURE_STORAGE_NODE *)orxTree_GetChild(&(pstNode->stTreeNode));

      /* Valid? */
      if(pstNode != orxNULL)
      {
        /* Gets structure */
        pstStructure = pstNode->pstStructure;
      }
    }
    else
    {
      /* !!! MSG !!! */
    }
  }
  else
  {
    /* !!! MSG !!! */
  }

  /* Done! */
  return pstStructure;
}

/***************************************************************************
 orxStructure_GetLeftSibling
 Structure tree left sibling get accessor.

 returns: left sibling
 ***************************************************************************/
orxSTRUCTURE *orxFASTCALL orxStructure_GetSibling(orxCONST orxHANDLE _phStructure)
{
  orxREGISTER orxSTRUCTURE_STORAGE_NODE *pstNode;
  orxREGISTER orxSTRUCTURE *pstStructure = orxNULL;

  /* Checks */
  orxASSERT(sstStructure.u32Flags & orxSTRUCTURE_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_phStructure);

  /* Gets node */
  pstNode = (orxSTRUCTURE_STORAGE_NODE *)((orxSTRUCTURE *)_phStructure)->hStorageNode;

  /* Valid? */
  if(pstNode != orxNULL)
  {
    /* Is storate type correct? */
    if(pstNode->eType == orxSTRUCTURE_STORAGE_TYPE_TREE)
    {
      /* Gets sibling node */
      pstNode = (orxSTRUCTURE_STORAGE_NODE *)orxTree_GetSibling(&(pstNode->stTreeNode));

      /* Valid? */
      if(pstNode != orxNULL)
      {
        /* Gets structure */
        pstStructure = pstNode->pstStructure;
      }
    }
    else
    {
      /* !!! MSG !!! */
    }
  }
  else
  {
    /* !!! MSG !!! */
  }

  /* Done! */
  return pstStructure;
}

/***************************************************************************
 orxStructure_GetPrevious
 Structure list previous get accessor.

 returns: previous
 ***************************************************************************/
orxSTRUCTURE *orxFASTCALL orxStructure_GetPrevious(orxCONST orxHANDLE _phStructure)
{
  orxREGISTER orxSTRUCTURE_STORAGE_NODE *pstNode;
  orxREGISTER orxSTRUCTURE *pstStructure = orxNULL;

  /* Checks */
  orxASSERT(sstStructure.u32Flags & orxSTRUCTURE_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_phStructure);

  /* Gets node */
  pstNode = (orxSTRUCTURE_STORAGE_NODE *)((orxSTRUCTURE *)_phStructure)->hStorageNode;

  /* Valid? */
  if(pstNode != orxNULL)
  {
    /* Is storate type correct? */
    if(pstNode->eType == orxSTRUCTURE_STORAGE_TYPE_LINKLIST)
    {
      /* Gets previous node */
      pstNode = (orxSTRUCTURE_STORAGE_NODE *)orxLinkList_GetPrevious(&(pstNode->stLinkListNode));

      /* Valid? */
      if(pstNode != orxNULL)
      {
        /* Gets structure */
        pstStructure = pstNode->pstStructure;
      }
    }
    else
    {
      /* !!! MSG !!! */
    }
  }
  else
  {
    /* !!! MSG !!! */
  }

  /* Done! */
  return pstStructure;
}

/***************************************************************************
 orxStructure_GetNext
 Structure list next get accessor.

 returns: next
 ***************************************************************************/
orxSTRUCTURE *orxFASTCALL orxStructure_GetNext(orxCONST orxHANDLE _phStructure)
{
  orxREGISTER orxSTRUCTURE_STORAGE_NODE *pstNode;
  orxREGISTER orxSTRUCTURE *pstStructure = orxNULL;

  /* Checks */
  orxASSERT(sstStructure.u32Flags & orxSTRUCTURE_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_phStructure);

  /* Gets node */
  pstNode = (orxSTRUCTURE_STORAGE_NODE *)((orxSTRUCTURE *)_phStructure)->hStorageNode;

  /* Valid? */
  if(pstNode != orxNULL)
  {
    /* Is storate type correct? */
    if(pstNode->eType == orxSTRUCTURE_STORAGE_TYPE_LINKLIST)
    {
      /* Gets next node */
      pstNode = (orxSTRUCTURE_STORAGE_NODE *)orxLinkList_GetNext(&(pstNode->stLinkListNode));

      /* Valid? */
      if(pstNode != orxNULL)
      {
        /* Gets structure */
        pstStructure = pstNode->pstStructure;
      }
    }
    else
    {
      /* !!! MSG !!! */
    }
  }
  else
  {
    /* !!! MSG !!! */
  }

  /* Done! */
  return pstStructure;
}

/***************************************************************************
 orxStructure_SetParent
 Structure tree parent set accessor.

 returns: orxVOID
 ***************************************************************************/
orxSTATUS orxFASTCALL orxStructure_SetParent(orxHANDLE _phStructure, orxHANDLE _phParent)
{
  orxREGISTER orxSTRUCTURE_STORAGE_NODE *pstNode, *pstParentNode;
  orxREGISTER orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstStructure.u32Flags & orxSTRUCTURE_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_phStructure);
  orxSTRUCTURE_ASSERT(_phParent);

  /* Gets nodes */
  pstNode       = (orxSTRUCTURE_STORAGE_NODE *)((orxSTRUCTURE *)_phStructure)->hStorageNode;
  pstParentNode = (orxSTRUCTURE_STORAGE_NODE *)((orxSTRUCTURE *)_phParent)->hStorageNode;

  /* Valid? */
  if((pstNode != orxNULL) && (pstParentNode != orxNULL))
  {
    /* Is storate type correct? */
    if(pstNode->eType == orxSTRUCTURE_STORAGE_TYPE_TREE)
    {
      /* Moves it */
      eResult = orxTree_MoveAsChild(&(pstParentNode->stTreeNode), &(pstNode->stTreeNode));
    }
    else
    {
      /* !!! MSG !!! */

      /* Not done */
      eResult = orxSTATUS_FAILURE;
    }
  }
  else
  {
    /* !!! MSG !!! */
    
    /* Not done */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}
