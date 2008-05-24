/***************************************************************************
 orxLinkList.c
 List module

 begin                : 06/04/2005
 author               : (C) Arcallians
 email                : iarwain@arcallians.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License           *
 *   as published by the Free Software Foundation; either version 2.1      *
 *   of the License, or (at your option) any later version.                *
 *                                                                         *
 ***************************************************************************/


#include "utils/orxLinkList.h"

#include "memory/orxMemory.h"


/*
 * Platform independant defines
 */

#define orxLINKLIST_KU32_STATIC_FLAG_NONE         0x00000000
#define orxLINKLIST_KU32_STATIC_FLAG_READY        0x00000001


/*
 * Static structure
 */
typedef struct __orxLINKLIST_STATIC_t
{
  /* Control flags */
  orxU32 u32Flags;

} orxLINKLIST_STATIC;

/*
 * Static data
 */
orxSTATIC orxLINKLIST_STATIC sstLinkList;


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
 orxLinkList_Setup
 LinkList module setup.

 returns: nothing
 ***************************************************************************/
orxVOID orxLinkList_Setup()
{
  /* Adds module dependencies */
  orxModule_AddDependency(orxMODULE_ID_LINKLIST, orxMODULE_ID_MEMORY);

  return;
}

/***************************************************************************
 orxLinkList_Init
 Inits the link list system.

 returns: orxSTATUS_SUCCESS/orxSTATUS_FAILURE
 ***************************************************************************/
orxSTATUS orxLinkList_Init()
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Already Initialized? */
  if(!(sstLinkList.u32Flags & orxLINKLIST_KU32_STATIC_FLAG_READY))
  {
    /* Cleans static controller */
    orxMemory_Zero(&sstLinkList, sizeof(orxLINKLIST_STATIC));

    /* Inits flags */
    sstLinkList.u32Flags = orxLINKLIST_KU32_STATIC_FLAG_READY;

    /* Success */
    eResult = orxSTATUS_SUCCESS;
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
 orxLinkList_Exit
 Exits from the link list system.

 returns: orxVOID
 ***************************************************************************/
orxVOID orxLinkList_Exit()
{
  /* Initialized? */
  if(sstLinkList.u32Flags & orxLINKLIST_KU32_STATIC_FLAG_READY)
  {
    /* Updates flags */
    sstLinkList.u32Flags &= ~orxLINKLIST_KU32_STATIC_FLAG_READY;
  }
  else
  {
    /* !!! MSG !!! */
  }

  return;
}

/***************************************************************************
 orxLinkList_Clean
 Cleans a link list.

 returns: orxSTATUS_SUCCESS/orxSTATUS_FAILURE
 ***************************************************************************/
orxSTATUS orxFASTCALL orxLinkList_Clean(orxLINKLIST *_pstList)
{
  /* Checks */
  orxASSERT(sstLinkList.u32Flags & orxLINKLIST_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstList != orxNULL);

  /* Non empty? */
  if(_pstList->u32Counter != 0)
  {
    orxREGISTER orxLINKLIST_NODE *pstNode, *pstNext;

    /* Gets first node */
    pstNode = _pstList->pstFirst;

    /* Clean all nodes */
    while(pstNode != orxNULL)
    {
      /* Backups next node */
      pstNext = pstNode->pstNext;

      /* Cleans current node */
      orxMemory_Zero(pstNode, sizeof(orxLINKLIST_NODE));

      /* Go to next node */
      pstNode = pstNext;
    }
  }

  /* Cleans list */
  orxMemory_Zero(_pstList, sizeof(orxLINKLIST));

  /* Done! */
  return orxSTATUS_SUCCESS;
}

/***************************************************************************
 orxLinkList_AddStart
 Adds a new node at the start of the corresponding list.

 returns: orxSTATUS_SUCCESS/orxSTATUS_FAILURE
 ***************************************************************************/
orxSTATUS orxFASTCALL orxLinkList_AddStart(orxLINKLIST *_pstList, orxLINKLIST_NODE *_pstNode)
{
  orxREGISTER orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstLinkList.u32Flags & orxLINKLIST_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstList != orxNULL);
  orxASSERT(_pstNode != orxNULL);

  /* Not already used in a list? */
  if(_pstNode->pstList == orxNULL)
  {
    /* Adds it at the start of the list */
    _pstNode->pstNext     = _pstList->pstFirst;
    _pstNode->pstPrevious = orxNULL;
    _pstNode->pstList     = _pstList;

    /* Updates old node if needed */
    if(_pstList->pstFirst != orxNULL)
    {
      _pstList->pstFirst->pstPrevious = _pstNode;
    }
    else
    {
      /* Updates last node */
      _pstList->pstLast   = _pstNode;
    }

    /* Stores node at the start of the list */
    _pstList->pstFirst    = _pstNode;

    /* Updates counter */
    _pstList->u32Counter++;
  }
  else
  {
    /* !!! MSG !!! */

    /* Not linked */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/***************************************************************************
 orxLinkList_AddEnd
 Adds a new node at the end of the corresponding list.

 returns: orxSTATUS_SUCCESS/orxSTATUS_FAILURE
 ***************************************************************************/
orxSTATUS orxFASTCALL orxLinkList_AddEnd(orxLINKLIST *_pstList, orxLINKLIST_NODE *_pstNode)
{
  orxREGISTER orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstLinkList.u32Flags & orxLINKLIST_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstList != orxNULL);
  orxASSERT(_pstNode != orxNULL);

  /* Not already used in a list? */
  if(_pstNode->pstList == orxNULL)
  {
    /* Adds it at the end of the list */
    _pstNode->pstPrevious = _pstList->pstLast;
    _pstNode->pstNext     = orxNULL;
    _pstNode->pstList     = _pstList;

    /* Updates old node if needed */
    if(_pstList->pstLast != orxNULL)
    {
      _pstList->pstLast->pstNext = _pstNode;
    }
    else
    {
      /* Updates first node */
      _pstList->pstFirst  = _pstNode;
    }

    /* Stores node at the end of the list */
    _pstList->pstLast     = _pstNode;

    /* Updates counter */
    _pstList->u32Counter++;
  }
  else
  {
    /* !!! MSG !!! */

    /* Not linked */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/***************************************************************************
 orxLinkList_AddBefore
 Adds a new node before another one.

 returns: orxSTATUS_SUCCESS/orxSTATUS_FAILURE
 ***************************************************************************/
orxSTATUS orxFASTCALL orxLinkList_AddBefore(orxLINKLIST_NODE *_pstRefNode, orxLINKLIST_NODE *_pstNode)
{
  orxREGISTER orxSTATUS eResult = orxSTATUS_SUCCESS;
  orxREGISTER orxLINKLIST *pstList;

  /* Checks */
  orxASSERT(sstLinkList.u32Flags & orxLINKLIST_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstRefNode != orxNULL);
  orxASSERT(_pstNode != orxNULL);

  /* Isn't already linked? */
  if(_pstNode->pstList == orxNULL)
  {
    /* Gets list */
    pstList = _pstRefNode->pstList;

    /* Valid? */
    if(pstList != orxNULL)
    {
      /* Adds it in the list */
      _pstNode->pstNext         = _pstRefNode;
      _pstNode->pstPrevious     = _pstRefNode->pstPrevious;
      _pstNode->pstList         = pstList;

      /* Updates previous? */
      if(_pstRefNode->pstPrevious != orxNULL)
      {
        /* Updates it */
        _pstRefNode->pstPrevious->pstNext = _pstNode;
      }
      else
      {
        /* Checks node was the first one */
        orxASSERT(pstList->pstFirst == _pstRefNode);

        /* Updates new first node */
        pstList->pstFirst = _pstNode;
      }

      /* Updates ref node */
      _pstRefNode->pstPrevious  = _pstNode;

      /* Updates counter */
      pstList->u32Counter++;
    }
    else
    {
      /* !!! MSG !!! */

      /* No list found */
      eResult = orxSTATUS_FAILURE;
    }
  }
  else
  {
    /* !!! MSG !!! */

    /* Already linked */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/***************************************************************************
 orxLinkList_AddAfter
 Adds a new node after another one.

 returns: orxSTATUS_SUCCESS/orxSTATUS_FAILURE
 ***************************************************************************/
orxSTATUS orxFASTCALL orxLinkList_AddAfter(orxLINKLIST_NODE *_pstRefNode, orxLINKLIST_NODE *_pstNode)
{
  orxREGISTER orxSTATUS eResult = orxSTATUS_SUCCESS;
  orxREGISTER orxLINKLIST *pstList;

  /* Checks */
  orxASSERT(sstLinkList.u32Flags & orxLINKLIST_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstRefNode != orxNULL);
  orxASSERT(_pstNode != orxNULL);

  /* Isn't already linked? */
  if(_pstNode->pstList == orxNULL)
  {
    /* Gets list */
    pstList = _pstRefNode->pstList;

    /* Valid? */
    if(pstList != orxNULL)
    {
      /* Adds it in the list */
      _pstNode->pstNext         = _pstRefNode->pstNext;
      _pstNode->pstPrevious     = _pstRefNode;
      _pstNode->pstList         = pstList;

      /* Updates next? */
      if(_pstRefNode->pstNext != orxNULL)
      {
        /* Updates it */
        _pstRefNode->pstNext->pstPrevious = _pstNode;
      }
      else
      {
        /* Checks node was the last one */
        orxASSERT(pstList->pstLast == _pstRefNode);

        /* Updates new last node */
        pstList->pstLast        = _pstNode;
      }

      /* Updates ref node */
      _pstRefNode->pstNext      = _pstNode;

      /* Updates counter */
      pstList->u32Counter++;
    }
    else
    {
      /* !!! MSG !!! */

      /* No list found */
      eResult = orxSTATUS_FAILURE;
    }
  }
  else
  {
    /* !!! MSG !!! */

    /* Already linked */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/***************************************************************************
 orxLinkList_Remove
 Removes a node from its list.

 returns: orxSTATUS_SUCCESS/orxSTATUS_FAILURE
 ***************************************************************************/
orxSTATUS orxFASTCALL orxLinkList_Remove(orxLINKLIST_NODE *_pstNode)
{
  orxREGISTER orxLINKLIST *pstList;
  orxREGISTER orxLINKLIST_NODE *pstPrevious, *pstNext;
  orxREGISTER orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstLinkList.u32Flags & orxLINKLIST_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstNode != orxNULL);

  /* Gets list */
  pstList = _pstNode->pstList;

  /* Valid? */
  if(pstList != orxNULL)
  {
    /* Checks list is non empty */
    orxASSERT(pstList->u32Counter != 0);

    /* Gets neighbours pointers */
    pstPrevious = _pstNode->pstPrevious;
    pstNext     = _pstNode->pstNext;

    /* Not at the start of the list? */
    if(pstPrevious != orxNULL)
    {
      /* Updates previous node */
      pstPrevious->pstNext  = pstNext;
    }
    else
    {
      /* Checks node was at the start of the list */
      orxASSERT(pstList->pstFirst == _pstNode);

      /* Updates list first pointer */
      pstList->pstFirst     = pstNext;
    }

    /* Not at the end of the list? */
    if(pstNext != orxNULL)
    {
      /* Updates previous node */
      pstNext->pstPrevious  = pstPrevious;
    }
    else
    {
      /* Checks node was at the end of the list */
      orxASSERT(pstList->pstLast == _pstNode);

      /* Updates list last pointer */
      pstList->pstLast      = pstPrevious;
    }

    /* Cleans node */
    orxMemory_Zero(_pstNode, sizeof(orxLINKLIST_NODE));

    /* Udpates counter */
    pstList->u32Counter--;
  }
  else
  {
    /* !!! MSG !!! */

    /* Failed */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}
