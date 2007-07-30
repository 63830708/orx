/** 
 * \file orxVector.h
 * 
 * Vector module.
 * Handles vectors.
 * 
 * \todo
 * Extract box code into box module
 * Adds rotate function with matrix module, when it's done
 * Gets it intrinsic depending on platform.
 * All handling functions.
 */


/***************************************************************************
 orxVector.h
 Vector module
 
 begin                : 30/03/2005
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


#ifndef _orxVECTOR_H_
#define _orxVECTOR_H_

#include "orxInclude.h"

#include "debug/orxDebug.h"
#include "memory/orxMemory.h"


/** Public structure definition. */
typedef struct __orxVECTOR_t
{
  /** Coordinates : 16 */
  orxFLOAT fX, fY, fZ, fW;

} orxVECTOR;


/** Rotates a coord using a orxFLOAT angle (RAD), an axis and stores result in another one. */
extern orxDLLAPI orxVECTOR *orxFASTCALL       orxVector_Rot(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp, orxCONST orxVECTOR *_pvAxis, orxFLOAT _fAngle);

/** Reorders axis aligned box corners (result is real upper left & bottom right corners). */
extern orxDLLAPI orxVOID orxFASTCALL          orxVector_ReorderAABox(orxVECTOR *_pvULBox, orxVECTOR *_pvBRBox);

/** Tests axis aligned box intersection given corners (if corners are not sorted, test won't work). */
extern orxDLLAPI orxBOOL orxFASTCALL          orxVector_TestAABoxIntersection(orxCONST orxVECTOR *_pvULBox1, orxCONST orxVECTOR *_pvBRBox1, orxCONST orxVECTOR *_pvULBox2, orxCONST orxVECTOR *_pvBRBox2);


/* *** Vector inlined functions *** */


/** Sets vector x / y / z / w values. */
orxSTATIC orxINLINE orxVECTOR                *orxVector_Set4(orxVECTOR *_pvVec, orxFLOAT _fX, orxFLOAT _fY, orxFLOAT _fZ, orxFLOAT _fW)
{
  /* Checks */
  orxASSERT(_pvVec != orxNULL);

  /* Stores values */
  _pvVec->fX = _fX;
  _pvVec->fY = _fY;
  _pvVec->fZ = _fZ;
  _pvVec->fW = _fW;

  /* Done ! */
  return _pvVec;
}

/** Sets vector x / y / z values. */
orxSTATIC orxINLINE orxVECTOR                *orxVector_Set3(orxVECTOR *_pvVec, orxFLOAT _fX, orxFLOAT _fY, orxFLOAT _fZ)
{
  /* Done ! */
  return(orxVector_Set4(_pvVec, _fX, _fY, _fZ, orxFLOAT_0));
}

/** Sets value in all vector fields. */
orxSTATIC orxINLINE orxVECTOR                *orxVector_SetAll(orxVECTOR *_pvVec, orxFLOAT _fValue)
{
  /* Done ! */
  return(orxVector_Set4(_pvVec, _fValue, _fValue, _fValue, _fValue));
}

/** Copies vector values into another one. */
orxSTATIC orxINLINE orxVOID                   orxVector_Copy(orxVECTOR *_pvDst, orxCONST orxVECTOR *_pvSrc)
{
  /* Checks */
  orxASSERT(_pvDst != orxNULL);
  orxASSERT(_pvSrc != orxNULL);

  /* Copies it */
  orxMemory_Copy(_pvDst, _pvSrc, sizeof(orxVECTOR));

  /* Done! */
  return;
}

/** Adds vectors and stores result in a third one. */
orxSTATIC orxINLINE orxVECTOR                *orxVector_Add(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp1, orxCONST orxVECTOR *_pvOp2)
{
  /* Checks */
  orxASSERT(_pvRes != orxNULL);
  orxASSERT(_pvOp1 != orxNULL);
  orxASSERT(_pvOp2 != orxNULL);

  /* Adds all */
  _pvRes->fX = _pvOp1->fX + _pvOp2->fX;
  _pvRes->fY = _pvOp1->fY + _pvOp2->fY;
  _pvRes->fZ = _pvOp1->fZ + _pvOp2->fZ;
  _pvRes->fW = _pvOp1->fW + _pvOp2->fW;

  /* Done! */
  return _pvRes;
}

/** Subs vectors and stores result in a third one. */
orxSTATIC orxINLINE orxVECTOR                *orxVector_Sub(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp1, orxCONST orxVECTOR *_pvOp2)
{
  /* Checks */
  orxASSERT(_pvRes != orxNULL);
  orxASSERT(_pvOp1 != orxNULL);
  orxASSERT(_pvOp2 != orxNULL);

  /* Adds all */
  _pvRes->fX = _pvOp1->fX - _pvOp2->fX;
  _pvRes->fY = _pvOp1->fY - _pvOp2->fY;
  _pvRes->fZ = _pvOp1->fZ - _pvOp2->fZ;
  _pvRes->fW = _pvOp1->fW - _pvOp2->fW;

  /* Done! */
  return _pvRes;
}

/** Muls a vector by an orxFLOAT and stores result in another one. */
orxSTATIC orxINLINE orxVECTOR                *orxVector_Mul(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp1, orxFLOAT _fOp2)
{
  /* Checks */
  orxASSERT(_pvRes != orxNULL);
  orxASSERT(_pvOp1 != orxNULL);

  /* Muls all */
  _pvRes->fX = _pvOp1->fX * _fOp2;
  _pvRes->fY = _pvOp1->fY * _fOp2;
  _pvRes->fZ = _pvOp1->fZ * _fOp2;
  _pvRes->fW = _pvOp1->fW * _fOp2;

  /* Done! */
  return _pvRes;
}

/** Divs a vector by an orxFLOAT and stores result in another one. */
orxSTATIC orxINLINE orxVECTOR                *orxVector_Div(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp1, orxFLOAT _fOp2)
{
  orxREGISTER orxFLOAT fInvCoef;

  /* Checks */
  orxASSERT(_pvRes != orxNULL);
  orxASSERT(_pvOp1 != orxNULL);
  orxASSERT(_fOp2 != 0.0f);

  /* Gets coef */
  fInvCoef = orxFLOAT_1 / _fOp2;

  /* Muls all */
  _pvRes->fX = _pvOp1->fX * fInvCoef;
  _pvRes->fY = _pvOp1->fY * fInvCoef;
  _pvRes->fZ = _pvOp1->fZ * fInvCoef;
  _pvRes->fW = _pvOp1->fW * fInvCoef;

  /* Done! */
  return _pvRes;
}

/** Negates a vector and stores result in another one. */
orxSTATIC orxINLINE orxVECTOR                *orxVector_Neg(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp)
{
  /* Checks */
  orxASSERT(_pvRes != orxNULL);
  orxASSERT(_pvOp  != orxNULL);

  /* Negates all */
  _pvRes->fX = -(_pvOp->fX);
  _pvRes->fY = -(_pvOp->fY);
  _pvRes->fZ = -(_pvOp->fZ);
  _pvRes->fW = -(_pvOp->fW);

  /* Done! */
  return _pvRes;
}

/** Reverses a vector and stores result in another one. */
orxSTATIC orxINLINE orxVECTOR                *orxVector_Inv(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp)
{
  /* Checks */
  orxASSERT(_pvRes != orxNULL);
  orxASSERT(_pvOp  != orxNULL);

  /* Reverts all */
  _pvRes->fX = orxFLOAT_1 / _pvOp->fX;
  _pvRes->fY = orxFLOAT_1 / _pvOp->fY;
  _pvRes->fZ = orxFLOAT_1 / _pvOp->fZ;
  _pvRes->fW = orxFLOAT_1 / _pvOp->fW;

  /* Done! */
  return _pvRes;
}


/* *** Vector constants *** */


orxSTATIC orxCONST  orxVECTOR      orxVector_X    = {orx2F(1.0f), orx2F(0.0f), orx2F(0.0f), orx2F(0.0f)};
orxSTATIC orxCONST  orxVECTOR      orxVector_Y    = {orx2F(0.0f), orx2F(1.0f), orx2F(0.0f), orx2F(0.0f)};
orxSTATIC orxCONST  orxVECTOR      orxVector_Z    = {orx2F(0.0f), orx2F(0.0f), orx2F(1.0f), orx2F(0.0f)};

orxSTATIC orxCONST  orxVECTOR      orxVector_0    = {orx2F(0.0f), orx2F(0.0f), orx2F(0.0f), orx2F(0.0f)};


#endif /* _orxVECTOR_H_ */
