/***************************************************************************
 orxTree.c
 Tree module
 
 begin                : 07/04/2005
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


#include "utils/orxTree.h"

#include "memory/orxMemory.h"


/*
 * Platform independant defines
 */

#define orxTREE_KU32_FLAG_NONE                    0x00000000
#define orxTREE_KU32_FLAG_READY                   0x00000001


/*
 * Static structure
 */
typedef struct __orxTREE_STATIC_t
{
  /* Control flags */
  orxU32 u32Flags;

} orxTREE_STATIC;

/*
 * Static data
 */
orxSTATIC orxTREE_STATIC sstTree;


/***************************************************************************
 ***************************************************************************
 ******                       LOCAL FUNCTIONS                         ******
 ***************************************************************************
 ***************************************************************************/

/***************************************************************************
 orxTree_PrivateRemove
 Removes a node from its tree.

 returns: orxSTATUS_SUCCESS/orxSTATUS_FAILED
 ***************************************************************************/
orxINLINE orxSTATUS orxTree_PrivateRemove(orxTREE_NODE *_pstNode, orxBOOL _bKeepRef)
{
  orxREGISTER orxTREE *pstTree;
  orxREGISTER orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(_pstNode != orxNULL);

  /* Gets tree */
  pstTree = _pstNode->pstTree;

  /* Keep refs? */
  if(_bKeepRef != orxFALSE)
  {
    /* Is root? */
    if(pstTree->pstRoot == _pstNode)
    {
      /* !!! MSG !!! */
        
      /* Can't process */
      eResult = orxSTATUS_FAILED;
    }
    else
    {
      /* Was firt child? */
      if(_pstNode->pstParent->pstChild == _pstNode)
      {
        /* Udpates parent */
        _pstNode->pstParent->pstChild = _pstNode->pstSibling;
      }
      else
      {
        orxREGISTER orxTREE_NODE *pstChild;

        /* Finds left sibling */
        for(pstChild = _pstNode->pstParent->pstChild;
            pstChild->pstSibling != _pstNode;
            pstChild = pstChild->pstSibling);
    
        /* Updates it */
        pstChild->pstSibling = _pstNode->pstSibling;
      }

      /* Updates node */
      _pstNode->pstParent   = orxNULL;
      _pstNode->pstSibling  = orxNULL;
    }
  }
  /* Cleans all related */
  else
  {
    /* Is root? */
    if(pstTree->pstRoot == _pstNode)
    {
      /* Is the last node in tree? */
      if(pstTree->u32Counter == 1)
      {
        /* !!! TODO !!! */
      }
      else
      {
        /* !!! MSG !!! */

        /* Can't process */
        eResult = orxSTATUS_FAILED;
      }
    }
  }

  /* Done! */
  return eResult;
}  


/***************************************************************************
 ***************************************************************************
 ******                       PUBLIC FUNCTIONS                        ******
 ***************************************************************************
 ***************************************************************************/

/***************************************************************************
 orxTree_Init
 Inits the link list system.

 returns: orxSTATUS_SUCCESS/orxSTATUS_FAILED
 ***************************************************************************/
orxSTATUS orxTree_Init()
{
  /* Already Initialized? */
  if(sstTree.u32Flags & orxTREE_KU32_FLAG_READY)
  {
    /* !!! MSG !!! */

    return orxSTATUS_FAILED;
  }

  /* Cleans static controller */
  orxMemory_Set(&sstTree, 0, sizeof(orxTREE_STATIC));

  /* Inits ID Flags */
  sstTree.u32Flags = orxTREE_KU32_FLAG_READY;

  /* Done! */
  return orxSTATUS_SUCCESS;
}

/***************************************************************************
 orxTree_Exit
 Exits from the link list system.

 returns: orxVOID
 ***************************************************************************/
orxVOID orxTree_Exit()
{
  /* Not initialized? */
  if((sstTree.u32Flags & orxTREE_KU32_FLAG_READY) == orxTREE_KU32_FLAG_NONE)
  {
    /* !!! MSG !!! */

    return;
  }

  /* Updates flags */
  sstTree.u32Flags &= ~orxTREE_KU32_FLAG_READY;

  return;
}

/***************************************************************************
 orxTree_Clean
 Cleans a link list.

 returns: orxSTATUS_SUCCESS/orxSTATUS_FAILED
 ***************************************************************************/
orxSTATUS orxTree_Clean(orxTREE *_pstTree)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;
    
  /* Checks */
  orxASSERT(sstTree.u32Flags & orxTREE_KU32_FLAG_READY);
  orxASSERT(_pstTree != orxNULL);

  /* Non empty? */
  while((_pstTree->u32Counter != 0) && (eResult == orxSTATUS_SUCCESS))
  {
    /* Removes root node */
    eResult = orxTree_Remove(_pstTree->pstRoot);
  }

  /* Successful? */
  if(eResult == orxSTATUS_SUCCESS)
  {
    /* Cleans tree */
    orxMemory_Set(_pstTree, 0, sizeof(orxTREE));
  }

  /* Done! */
  return eResult;
}

/***************************************************************************
 orxTree_AddRoot
 Adds a new node at the root of the corresponding tree.

 returns: orxSTATUS_SUCCESS/orxSTATUS_FAILED
 ***************************************************************************/
orxSTATUS orxTree_AddRoot(orxTREE *_pstTree, orxTREE_NODE *_pstNode)
{
  orxREGISTER orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstTree.u32Flags & orxTREE_KU32_FLAG_READY);
  orxASSERT(_pstTree != orxNULL);
  orxASSERT(_pstNode != orxNULL);

  /* Not already used in a list? */
  if(_pstNode->pstTree == orxNULL)
  {
    /* Has a root? */
    if(_pstTree->pstRoot != orxNULL)
    {
      /* Adds as parent of the current root */
      eResult = orxTree_AddParent(_pstTree->pstRoot, _pstNode);
    }
    else
    {
        /* Checks there are no node right now */
        orxASSERT(_pstTree->u32Counter == 0);

        /* Stores it as root */
        _pstTree->pstRoot = _pstNode;

        /* Cleans it */
        orxMemory_Set(_pstNode, 0, sizeof(orxTREE_NODE));
        
        /* Stores tree pointer */
        _pstNode->pstTree = _pstTree;

        /* Updates counter */
        _pstTree->u32Counter++;
    }
  }
  else
  {
    /* !!! MSG !!! */

    /* Not linked */
    eResult = orxSTATUS_FAILED;
  }

  /* Done! */
  return eResult;
}

/***************************************************************************
 orxTree_AddParent
 Adds a new node as parent of another one.

 returns: orxSTATUS_SUCCESS/orxSTATUS_FAILED
 ***************************************************************************/
orxSTATUS orxTree_AddParent(orxTREE_NODE *_pstRefNode, orxTREE_NODE *_pstNode)
{
  orxREGISTER orxSTATUS eResult = orxSTATUS_SUCCESS;
  orxREGISTER orxTREE *pstTree;

  /* Checks */
  orxASSERT(sstTree.u32Flags & orxTREE_KU32_FLAG_READY);
  orxASSERT(_pstRefNode != orxNULL);
  orxASSERT(_pstNode != orxNULL);

  /* Isn't already linked? */
  if(_pstNode->pstTree == orxNULL)
  {
    /* Gets tree */
    pstTree = _pstRefNode->pstTree;
  
    /* Valid? */
    if(pstTree != orxNULL)
    {
      /* Adds it in the tree */
      _pstNode->pstChild    = _pstRefNode;
      _pstNode->pstParent   = _pstRefNode->pstParent;
      _pstNode->pstTree     = pstTree;
      _pstNode->pstSibling  = _pstRefNode->pstSibling;

      /* Updates parent? */
      if(_pstRefNode->pstParent != orxNULL)
      {
        /* Was first child? */
        if(_pstRefNode->pstParent->pstChild == _pstRefNode)
        {
            /* Updates parent */
            _pstRefNode->pstParent->pstChild = _pstNode;
        }
        else
        {
          orxREGISTER orxTREE_NODE *pstSibling;

          /* Finds left sibling */
          for(pstSibling = _pstRefNode->pstParent->pstChild;
              pstSibling->pstSibling != _pstRefNode;
              pstSibling = pstSibling->pstSibling);

          /* Updates sibling */
          pstSibling->pstSibling = _pstNode;
        }
      }
      else
      {
        /* Checks node was the root */
        orxASSERT(pstTree->pstRoot == _pstRefNode);

        /* Updates new root */
        pstTree->pstRoot = _pstNode;
      }

      /* Updates ref node */
      _pstRefNode->pstParent  = _pstNode;
      _pstRefNode->pstSibling = orxNULL;

      /* Updates counter */
      pstTree->u32Counter++;
    }
    else
    {
      /* !!! MSG !!! */
  
      /* No list found */
      eResult = orxSTATUS_FAILED;
    }
  }
  else
  {
    /* !!! MSG !!! */
    
    /* Already linked */
    eResult = orxSTATUS_FAILED;
  }

  /* Done! */
  return eResult;
}

/***************************************************************************
 orxTree_AddChild
 Adds a new node as a child of another one.

 returns: orxSTATUS_SUCCESS/orxSTATUS_FAILED
 ***************************************************************************/
orxSTATUS orxTree_AddChild(orxTREE_NODE *_pstRefNode, orxTREE_NODE *_pstNode)
{
  orxREGISTER orxSTATUS eResult = orxSTATUS_SUCCESS;
  orxREGISTER orxTREE *pstTree;

  /* Checks */
  orxASSERT(sstTree.u32Flags & orxTREE_KU32_FLAG_READY);
  orxASSERT(_pstRefNode != orxNULL);
  orxASSERT(_pstNode != orxNULL);

  /* Isn't already linked? */
  if(_pstNode->pstTree == orxNULL)
  {
    /* Gets tree */
    pstTree = _pstRefNode->pstTree;
  
    /* Valid? */
    if(pstTree != orxNULL)
    {
      /* Adds it in the tree */
      _pstNode->pstParent   = _pstRefNode;
      _pstNode->pstSibling  = _pstRefNode->pstChild;
      _pstNode->pstTree     = pstTree;
      _pstNode->pstChild    = orxNULL;

      /* Updates ref node */
      _pstRefNode->pstChild = _pstNode;

      /* Updates counter */
      pstTree->u32Counter++;
    }
    else
    {
      /* !!! MSG !!! */
  
      /* No list found */
      eResult = orxSTATUS_FAILED;
    }
  }
  else
  {
    /* !!! MSG !!! */
    
    /* Already linked */
    eResult = orxSTATUS_FAILED;
  }

  /* Done! */
  return eResult;
}

/***************************************************************************
 orxTree_MoveAsChild
 Moves as a child of another node of the same tree.

 returns: orxSTATUS_SUCCESS/orxSTATUS_FAILED
 ***************************************************************************/
orxSTATUS orxTree_MoveAsChild(orxTREE_NODE *_pstRefNode, orxTREE_NODE *_pstNode)
{
  orxREGISTER orxSTATUS eResult = orxSTATUS_SUCCESS;
  orxREGISTER orxTREE *pstTree;

  /* Checks */
  orxASSERT(sstTree.u32Flags & orxTREE_KU32_FLAG_READY);
  orxASSERT(_pstRefNode != orxNULL);
  orxASSERT(_pstNode != orxNULL);

  /* Gets tree */
  pstTree = _pstRefNode->pstTree;

  /* Is already in the tree? */
  if(_pstNode->pstTree == pstTree)
  {
    orxREGISTER orxTREE_NODE *pstTest;

    /* Checks for preventing from turning into graph */
    for(pstTest = _pstRefNode; pstTest != orxNULL; pstTest = pstTest->pstParent)
    {
      /* Bad request? */
      if(pstTest == _pstNode)
      {
        break;
      }
    }

    /* No graph cycle found? */
    if(pstTest == orxNULL)
    {
      /* Removes it from its place */
      eResult = orxTree_PrivateRemove(_pstNode, orxTRUE);
    
      /* Success? */
      if(eResult == orxSTATUS_SUCCESS)
      {
        /* Adds it at new place */
        _pstNode->pstParent   = _pstRefNode;
        _pstNode->pstSibling  = _pstRefNode->pstChild;

        /* Updates ref node */
        _pstRefNode->pstChild = _pstNode;
      }
      else
      {
        /* !!! MSG !!! */
      }
    }
    else
    {
      /* !!! MSG !!! */

      /* Can't process */
      eResult = orxSTATUS_FAILED;
    }
  }
  else
  {
    /* !!! MSG !!! */
    
    /* Not already in the tree */
    eResult = orxSTATUS_FAILED;
  }

  /* Done! */
  return eResult;
}

/***************************************************************************
 orxTree_Remove
 Removes a node from its tree.

 returns: orxSTATUS_SUCCESS/orxSTATUS_FAILED
 ***************************************************************************/
orxSTATUS orxTree_Remove(orxTREE_NODE *_pstNode)
{
  orxREGISTER orxTREE *pstTree;
  orxREGISTER orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstTree.u32Flags & orxTREE_KU32_FLAG_READY);
  orxASSERT(_pstNode != orxNULL);

  /* Gets tree */
  pstTree = _pstNode->pstTree;

  /* Valid? */
  if(pstTree != orxNULL)
  {
    /* Checks tree is non empty */
    orxASSERT(pstTree->u32Counter != 0);

    /* Removes it */
    eResult = orxTree_PrivateRemove(_pstNode, orxFALSE);
  }
  else
  {
    /* !!! MSG !!! */

    /* Failed */
    eResult = orxSTATUS_FAILED;
  }

  /* Done! */
  return eResult;
}
