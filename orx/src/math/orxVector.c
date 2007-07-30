/***************************************************************************
 orxVector.c
 Vector module
 
 begin                : 27/04/2005
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


#include "math/orxVector.h"


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
 orxVector_Rot
 Rotates a coord using a orxFLOAT angle (RAD), an axis and stores result in another one.

 returns: Rotated vector
 ***************************************************************************/
orxVECTOR *orxFASTCALL orxVector_Rot(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp, orxCONST orxVECTOR *_pvAxis, orxFLOAT _fAngle)
{
  /* Checks */
  orxASSERT(_pvRes  != orxNULL);
  orxASSERT(_pvAxis != orxNULL);

  /* !!! TODO !!! */

  return _pvRes;
}

/***************************************************************************
 orxVector_ReorderAABox
 Reorders axis aligned box corners (result is real upper left & bottom right corners).

 returns: Nothing
 ***************************************************************************/
orxVOID orxFASTCALL orxVector_ReorderAABox(orxVECTOR *_pvULBox, orxVECTOR *_pvBRBox)
{
  /* Checks */
  orxASSERT(_pvULBox != orxNULL);
  orxASSERT(_pvBRBox != orxNULL);

/* Reorders coordinates so as to have upper left & bottom right box corners */

  /* Z coord */
  if(_pvULBox->fZ > _pvBRBox->fZ)
  {
    /* Swaps */
    orxSWAP32(_pvULBox->fZ, _pvBRBox->fZ);
  }

  /* Y coord */
  if(_pvULBox->fY > _pvBRBox->fY)
  {
    /* Swaps */
    orxSWAP32(_pvULBox->fY, _pvBRBox->fY);
  }

  /* X coord */
  if(_pvULBox->fX > _pvBRBox->fX)
  {
    /* Swaps */
    orxSWAP32(_pvULBox->fX, _pvBRBox->fX);
  }

  /* Done! */
  return;
}

/***************************************************************************
 orxVector_TestAABoxIntersection
 Tests axis aligned box intersection given corners (if corners are not sorted, test won't work).

 returns: Nothing
 ***************************************************************************/
orxBOOL orxFASTCALL orxVector_TestAABoxIntersection(orxCONST orxVECTOR *_pvULBox1, orxCONST orxVECTOR *_pvBRBox1, orxCONST orxVECTOR *_pvULBox2, orxCONST orxVECTOR *_pvBRBox2)
{
  orxREGISTER orxBOOL bResult = orxFALSE;

  /* Checks */
  orxASSERT(_pvULBox1 != orxNULL);
  orxASSERT(_pvBRBox1 != orxNULL);
  orxASSERT(_pvULBox2 != orxNULL);
  orxASSERT(_pvBRBox2 != orxNULL);

  /* Warning : Corners should be sorted otherwise test won't work! */

  /* Z intersected? */
  if((_pvBRBox2->fZ >= _pvULBox1->fZ)
  && (_pvULBox2->fZ <= _pvBRBox1->fZ))
  {
    /* X intersected? */
    if((_pvBRBox2->fX >= _pvULBox1->fX)
    && (_pvULBox2->fX <= _pvBRBox1->fX))
    {
      /* Y intersected? */
      if((_pvBRBox2->fY >= _pvULBox1->fY)
      && (_pvULBox2->fY <= _pvBRBox1->fY))
      {
        /* Intersects */
        bResult = orxTRUE;
      }
    }
  }

  /* Done! */
  return bResult;
}
