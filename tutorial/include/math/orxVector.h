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
 * @file orxVector.h
 * @date 30/03/2005
 * @author iarwain@orx-project.org
 *
 * @todo
 * - Extract box code into box module
 * - Adds rotate function with matrix module, when it's done
 * - Gets it intrinsic depending on platform.
 */

/**
 * @addtogroup orxVector
 * 
 * Vector module
 * Module that handles vectors and basic structures based on them
 *
 * @{
 */


#ifndef _orxVECTOR_H_
#define _orxVECTOR_H_

#include "orxInclude.h"

#include "debug/orxDebug.h"
#include "memory/orxMemory.h"
#include "math/orxMath.h"


/** Public structure definition
 */
typedef struct __orxVECTOR_t
{
  /** Coordinates : 12 */
  union
  {
    orxFLOAT fX;              /**< First coordinate in the cartesian space */
    orxFLOAT fRho;            /**< First coordinate in the spherical space */
    orxFLOAT fR;              /**< First coordinate in the RGB color space */
  };

  union
  {
    orxFLOAT fY;              /**< Second coordinate in the cartesian space */
    orxFLOAT fTheta;          /**< Second coordinate in the spherical space */
    orxFLOAT fG;              /**< Second coordinate in the RGB color space */
  };

  union
  {
    orxFLOAT fZ;              /**< Third coordinate in the cartesian space */
    orxFLOAT fPhi;            /**< Third coordinate in the spherical space */
    orxFLOAT fB;              /**< Third coordinate in the RGB color space */
  };

} orxVECTOR;

/** Public axis aligned box structure
 */
typedef struct __orxAABOX_t
{
  orxVECTOR vTL; /**< Top left corner vector : 12 */
  orxVECTOR vBR; /**< Bottom right corner vector : 24 */

} orxAABOX;

/** Public oriented box structure
*/
typedef struct __orxOBOX_t
{
    orxVECTOR vPosition;/**< Position vector  : 12 */
    orxVECTOR vPivot;   /**< Pivot vector     : 24 */
    orxVECTOR vX;       /**< X axis vector    : 36 */
    orxVECTOR vY;       /**< Y axis vector    : 48 */
    orxVECTOR vZ;       /**< Z axis vector    : 60 */

} orxOBOX;


/* *** Vector inlined functions *** */


/** Sets vector XYZ values (also work for other coordinate system)
 * @param[in]   _pvVec                        Concerned vector
 * @param[in]   _fX                           First coordinate value
 * @param[in]   _fY                           Second coordinate value
 * @param[in]   _fZ                           Third coordinate value
 * @return      Vector
 */
orxSTATIC orxINLINE orxVECTOR *               orxVector_Set(orxVECTOR *_pvVec, orxFLOAT _fX, orxFLOAT _fY, orxFLOAT _fZ)
{
  /* Checks */
  orxASSERT(_pvVec != orxNULL);

  /* Stores values */
  _pvVec->fX = _fX;
  _pvVec->fY = _fY;
  _pvVec->fZ = _fZ;

  /* Done ! */
  return _pvVec;
}

/** Sets all the vector coordinates with the given value
 * @param[in]   _pvVec                        Concerned vector
 * @param[in]   _fValue                       Value to set
 * @return      Vector
 */
orxSTATIC orxINLINE orxVECTOR *               orxVector_SetAll(orxVECTOR *_pvVec, orxFLOAT _fValue)
{
  /* Done ! */
  return(orxVector_Set(_pvVec, _fValue, _fValue, _fValue));
}

/** Copies a vector onto another one
 * @param[in]   _pvDst                        Vector to copy to (destination)
 * @param[in]   _pvSrc                        Vector to copy from (source)
 * @return      Destination vector
 */
orxSTATIC orxINLINE orxVECTOR *               orxVector_Copy(orxVECTOR *_pvDst, orxCONST orxVECTOR *_pvSrc)
{
  /* Checks */
  orxASSERT(_pvDst != orxNULL);
  orxASSERT(_pvSrc != orxNULL);

  /* Copies it */
  orxMemory_Copy(_pvDst, _pvSrc, sizeof(orxVECTOR));

  /* Done! */
  return _pvDst;
}

/** Adds vectors and stores result in a third one
 * @param[out]   _pvRes                       Vector where to store result (can be one of the two operands)
 * @param[in]   _pvOp1                        First operand
 * @param[in]   _pvOp2                        Second operand
 * @return      Resulting vector (Op1 + Op2)
 */
orxSTATIC orxINLINE orxVECTOR *               orxVector_Add(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp1, orxCONST orxVECTOR *_pvOp2)
{
  /* Checks */
  orxASSERT(_pvRes != orxNULL);
  orxASSERT(_pvOp1 != orxNULL);
  orxASSERT(_pvOp2 != orxNULL);

  /* Adds all */
  _pvRes->fX = _pvOp1->fX + _pvOp2->fX;
  _pvRes->fY = _pvOp1->fY + _pvOp2->fY;
  _pvRes->fZ = _pvOp1->fZ + _pvOp2->fZ;

  /* Done! */
  return _pvRes;
}

/** Substracts vectors and stores result in a third one
 * @param[out]   _pvRes                       Vector where to store result (can be one of the two operands)
 * @param[in]   _pvOp1                        First operand
 * @param[in]   _pvOp2                        Second operand
 * @return      Resulting vector (Op1 - Op2)
 */
orxSTATIC orxINLINE orxVECTOR *               orxVector_Sub(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp1, orxCONST orxVECTOR *_pvOp2)
{
  /* Checks */
  orxASSERT(_pvRes != orxNULL);
  orxASSERT(_pvOp1 != orxNULL);
  orxASSERT(_pvOp2 != orxNULL);

  /* Adds all */
  _pvRes->fX = _pvOp1->fX - _pvOp2->fX;
  _pvRes->fY = _pvOp1->fY - _pvOp2->fY;
  _pvRes->fZ = _pvOp1->fZ - _pvOp2->fZ;

  /* Done! */
  return _pvRes;
}

/** Multiplies a vector by an orxFLOAT and stores result in another one
 * @param[out]   _pvRes                       Vector where to store result (can be the operand)
 * @param[in]   _pvOp1                        First operand
 * @param[in]   _fOp2                         Second operand
 * @return      Resulting vector
 */
orxSTATIC orxINLINE orxVECTOR *               orxVector_Mulf(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp1, orxFLOAT _fOp2)
{
  /* Checks */
  orxASSERT(_pvRes != orxNULL);
  orxASSERT(_pvOp1 != orxNULL);

  /* Muls all */
  _pvRes->fX = _pvOp1->fX * _fOp2;
  _pvRes->fY = _pvOp1->fY * _fOp2;
  _pvRes->fZ = _pvOp1->fZ * _fOp2;

  /* Done! */
  return _pvRes;
}

/** Multiplies a vector by another vector and stores result in a third one
 * @param[out]   _pvRes                       Vector where to store result (can be one of the two operands)
 * @param[in]   _pvOp1                        First operand
 * @param[in]   _pvOp2                        Second operand
 * @return      Resulting vector (Op1 * Op2)
 */
orxSTATIC orxINLINE orxVECTOR *               orxVector_Mul(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp1, orxCONST orxVECTOR *_pvOp2)
{
  /* Checks */
  orxASSERT(_pvRes != orxNULL);
  orxASSERT(_pvOp1 != orxNULL);
  orxASSERT(_pvOp2 != orxNULL);

  /* Muls all */
  _pvRes->fX = _pvOp1->fX * _pvOp2->fX;
  _pvRes->fY = _pvOp1->fY * _pvOp2->fY;
  _pvRes->fZ = _pvOp1->fZ * _pvOp2->fZ;

  /* Done! */
  return _pvRes;
}

/** Divides a vector by an orxFLOAT and stores result in another one
 * @param[out]   _pvRes                       Vector where to store result (can be the operand)
 * @param[in]   _pvOp1                        First operand
 * @param[in]   _fOp2                         Second operand
 * @return      Resulting vector
 */
orxSTATIC orxINLINE orxVECTOR *               orxVector_Divf(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp1, orxFLOAT _fOp2)
{
  orxREGISTER orxFLOAT fRecCoef;

  /* Checks */
  orxASSERT(_pvRes != orxNULL);
  orxASSERT(_pvOp1 != orxNULL);
  orxASSERT(_fOp2 != orxFLOAT_0);

  /* Gets reciprocal coef */
  fRecCoef = orxFLOAT_1 / _fOp2;

  /* Muls all */
  _pvRes->fX = _pvOp1->fX * fRecCoef;
  _pvRes->fY = _pvOp1->fY * fRecCoef;
  _pvRes->fZ = _pvOp1->fZ * fRecCoef;

  /* Done! */
  return _pvRes;
}

/** Divides a vector by another vector and stores result in a third one
 * @param[out]   _pvRes                       Vector where to store result (can be one of the two operands)
 * @param[in]   _pvOp1                        First operand
 * @param[in]   _pvOp2                        Second operand
 * @return      Resulting vector (Op1 / Op2)
 */
orxSTATIC orxINLINE orxVECTOR *               orxVector_Div(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp1, orxCONST orxVECTOR *_pvOp2)
{
  /* Checks */
  orxASSERT(_pvRes != orxNULL);
  orxASSERT(_pvOp1 != orxNULL);
  orxASSERT(_pvOp2 != orxNULL);
  orxASSERT(_pvOp2->fX != orxFLOAT_0);
  orxASSERT(_pvOp2->fY != orxFLOAT_0);
  orxASSERT(_pvOp2->fZ != orxFLOAT_0);

  /* Divs all */
  _pvRes->fX = _pvOp1->fX / _pvOp2->fX;
  _pvRes->fY = _pvOp1->fY / _pvOp2->fY;
  _pvRes->fZ = _pvOp1->fZ / _pvOp2->fZ;

  /* Done! */
  return _pvRes;
}

/** Lerps from one vector to another one using a coefficient
 * @param[out]   _pvRes                       Vector where to store result (can be one of the two operands)
 * @param[in]   _pvOp1                        First operand
 * @param[in]   _pvOp2                        Second operand
 * @param[in]   _fOp                          Lerp coefficien parameter
 * @return      Resulting vector
 */
orxSTATIC orxINLINE orxVECTOR *               orxVector_Lerp(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp1, orxCONST orxVECTOR *_pvOp2, orxFLOAT _fOp)
{
  /* Checks */
  orxASSERT(_pvRes != orxNULL);
  orxASSERT(_pvOp1 != orxNULL);
  orxASSERT(_pvOp2 != orxNULL);
  orxASSERT((_fOp >= orxFLOAT_0) && (_fOp <= orxFLOAT_1));

  /* Lerps all*/
  _pvRes->fX = orxLERP(_pvOp1->fX, _pvOp2->fX, _fOp);
  _pvRes->fY = orxLERP(_pvOp1->fY, _pvOp2->fY, _fOp);
  _pvRes->fZ = orxLERP(_pvOp1->fZ, _pvOp2->fZ, _fOp);

  /* Done! */
  return _pvRes;
}

/** Gets minimum between two vectors
 * @param[out]   _pvRes                       Vector where to store result (can be one of the two operands)
 * @param[in]   _pvOp1                        First operand
 * @param[in]   _pvOp2                        Second operand
 * @return      Resulting vector MIN(Op1, Op2)
 */
orxSTATIC orxINLINE orxVECTOR *               orxVector_Min(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp1, orxCONST orxVECTOR *_pvOp2)
{
  /* Checks */
  orxASSERT(_pvRes != orxNULL);
  orxASSERT(_pvOp1 != orxNULL);
  orxASSERT(_pvOp2 != orxNULL);

  /* Gets all mins */
  _pvRes->fX = orxMIN(_pvOp1->fX, _pvOp2->fX);
  _pvRes->fY = orxMIN(_pvOp1->fY, _pvOp2->fY);
  _pvRes->fZ = orxMIN(_pvOp1->fZ, _pvOp2->fZ);

  /* Done! */
  return _pvRes;
}

/** Gets maximum between two vectors
 * @param[out]   _pvRes                       Vector where to store result (can be one of the two operands)
 * @param[in]   _pvOp1                        First operand
 * @param[in]   _pvOp2                        Second operand
 * @return      Resulting vector MAX(Op1, Op2)
 */
orxSTATIC orxINLINE orxVECTOR *               orxVector_Max(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp1, orxCONST orxVECTOR *_pvOp2)
{
  /* Checks */
  orxASSERT(_pvRes != orxNULL);
  orxASSERT(_pvOp1 != orxNULL);
  orxASSERT(_pvOp2 != orxNULL);

  /* Gets all maxs */
  _pvRes->fX = orxMAX(_pvOp1->fX, _pvOp2->fX);
  _pvRes->fY = orxMAX(_pvOp1->fY, _pvOp2->fY);
  _pvRes->fZ = orxMAX(_pvOp1->fZ, _pvOp2->fZ);

  /* Done! */
  return _pvRes;
}

/** Clamps a vector between two others
 * @param[out]   _pvRes                       Vector where to store result (can be the operand)
 * @param[in]   _pvOp                         Vector to clamp
 * @param[in]   _pvMin                        Minimum boundary
 * @param[in]   _pvMax                        Maximum boundary
 * @return      Resulting vector CLAMP(Op, MIN, MAX)
 */
orxSTATIC orxINLINE orxVECTOR *               orxVector_Clamp(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp, orxCONST orxVECTOR *_pvMin, orxCONST orxVECTOR *_pvMax)
{
  /* Checks */
  orxASSERT(_pvRes != orxNULL);
  orxASSERT(_pvOp != orxNULL);
  orxASSERT(_pvMin != orxNULL);
  orxASSERT(_pvMax != orxNULL);

  /* Gets all clamped values */
  _pvRes->fX = orxCLAMP(_pvOp->fX, _pvMin->fX, _pvMax->fX);
  _pvRes->fY = orxCLAMP(_pvOp->fY, _pvMin->fY, _pvMax->fY);
  _pvRes->fZ = orxCLAMP(_pvOp->fZ, _pvMin->fZ, _pvMax->fZ);

  /* Done! */
  return _pvRes;
}

/** Negates a vector and stores result in another one
 * @param[out]   _pvRes                       Vector where to store result (can be the operand)
 * @param[in]   _pvOp                         Vector to negates
 * @return      Resulting vector (-Op)
 */
orxSTATIC orxINLINE orxVECTOR *               orxVector_Neg(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp)
{
  /* Checks */
  orxASSERT(_pvRes != orxNULL);
  orxASSERT(_pvOp  != orxNULL);

  /* Negates all */
  _pvRes->fX = -(_pvOp->fX);
  _pvRes->fY = -(_pvOp->fY);
  _pvRes->fZ = -(_pvOp->fZ);

  /* Done! */
  return _pvRes;
}

/** Gets reciprocal (1.0 /) vector and stores the result in another one
 * @param[out]   _pvRes                       Vector where to store result (can be the operand)
 * @param[in]   _pvOp                         Input value
 * @return      Resulting vector (1 / Op)
 */

orxSTATIC orxINLINE orxVECTOR *               orxVector_Rec(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp)
{
  /* Checks */
  orxASSERT(_pvRes != orxNULL);
  orxASSERT(_pvOp  != orxNULL);

  /* Reverts all */
  _pvRes->fX = orxFLOAT_1 / _pvOp->fX;
  _pvRes->fY = orxFLOAT_1 / _pvOp->fY;
  _pvRes->fZ = orxFLOAT_1 / _pvOp->fZ;

  /* Done! */
  return _pvRes;
}

/** Gets vector squared size
 * @param[in]   _pvOp                         Input vector
 * @return      Vector's squared size
 */
orxSTATIC orxINLINE orxFLOAT                  orxVector_GetSquareSize(orxCONST orxVECTOR *_pvOp)
{
  orxREGISTER orxFLOAT fResult;

  /* Checks */
  orxASSERT(_pvOp  != orxNULL);

  /* Updates result */
  fResult = (_pvOp->fX * _pvOp->fX) + (_pvOp->fY * _pvOp->fY) + (_pvOp->fZ * _pvOp->fZ);

  /* Done! */
  return fResult;
}

/** Gets vector size
 * @param[in]   _pvOp                         Input vector
 * @return      Vector's size
 */
orxSTATIC orxINLINE orxFLOAT                  orxVector_GetSize(orxCONST orxVECTOR *_pvOp)
{
  orxREGISTER orxFLOAT fResult;

  /* Checks */
  orxASSERT(_pvOp  != orxNULL);

  /* Updates result */
  fResult = orxMath_Sqrt((_pvOp->fX * _pvOp->fX) + (_pvOp->fY * _pvOp->fY) + (_pvOp->fZ * _pvOp->fZ));

  /* Done! */
  return fResult;
}

/** Gets squared distance between 2 positions
 * @param[in]   _pvOp1                        First position
 * @param[in]   _pvOp2                        Second position
 * @return      Squared distance
 */
orxSTATIC orxINLINE orxFLOAT                  orxVector_GetSquareDistance(orxCONST orxVECTOR *_pvOp1, orxCONST orxVECTOR *_pvOp2)
{
  orxVECTOR   vTemp;
  orxREGISTER orxFLOAT fResult;

  /* Checks */
  orxASSERT(_pvOp1 != orxNULL);
  orxASSERT(_pvOp2  != orxNULL);

  /* Gets distance vector */
  orxVector_Sub(&vTemp, _pvOp2, _pvOp1);

  /* Updates result */
  fResult = orxVector_GetSquareSize(&vTemp);

  /* Done! */
  return fResult;
}

/** Gets distance between 2 positions
 * @param[in]   _pvOp1                        First position
 * @param[in]   _pvOp2                        Second position
 * @return      Distance
 */
orxSTATIC orxINLINE orxFLOAT                  orxVector_GetDistance(orxCONST orxVECTOR *_pvOp1, orxCONST orxVECTOR *_pvOp2)
{
  orxVECTOR   vTemp;
  orxREGISTER orxFLOAT fResult;

  /* Checks */
  orxASSERT(_pvOp1 != orxNULL);
  orxASSERT(_pvOp2  != orxNULL);

  /* Gets distance vector */
  orxVector_Sub(&vTemp, _pvOp2, _pvOp1);

  /* Updates result */
  fResult = orxVector_GetSize(&vTemp);

  /* Done! */
  return fResult;
}

/** Normalizes a vector
 * @param[out]   _pvRes                       Vector where to store result (can be the operand)
 * @param[in]   _pvOp                         Vector to normalize
 * @return      Normalized vector
 */
orxSTATIC orxINLINE orxVECTOR *               orxVector_Normalize(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp)
{
  orxREGISTER orxFLOAT fOp;

  /* Checks */
  orxASSERT(_pvRes != orxNULL);
  orxASSERT(_pvOp  != orxNULL);

  /* Gets squared size */
  fOp = (_pvOp->fX * _pvOp->fX) + (_pvOp->fY * _pvOp->fY) + (_pvOp->fZ * _pvOp->fZ);

  /* Valid? */
  if(fOp > orxFLOAT_0)
  {
    /* Gets reciprocal size */
    fOp = orxFLOAT_1 / fOp;

    /* Updates result */
    _pvRes->fX = fOp * _pvOp->fX;
    _pvRes->fY = fOp * _pvOp->fY;
    _pvRes->fZ = fOp * _pvOp->fZ;
  }
  else
  {
    /* Clears result */
    orxMemory_Zero(_pvRes, sizeof(orxVECTOR));
  }

  /* Done! */
  return _pvRes;
}

/** Rotates a 2D vector (along Z-axis)
 * @param[out]   _pvRes                       Vector where to store result (can be the operand)
 * @param[in]   _pvOp                         Vector to rotate
 * @param[in]   _fAngle                       Angle of rotation (radians)
 * @return      Rotated vector
 */
orxSTATIC orxINLINE orxVECTOR *               orxVector_2DRotate(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp, orxFLOAT _fAngle)
{
  orxREGISTER orxFLOAT fSin, fCos;

  /* Checks */
  orxASSERT(_pvRes  != orxNULL);
  orxASSERT(_pvOp != orxNULL);

  /* Gets cos & sin of angle */
  fCos = orxMath_Cos(_fAngle);
  fSin = orxMath_Sin(_fAngle);

  /* Updates result */
  orxVector_Set(_pvRes, (fCos * _pvOp->fX) - (fSin * _pvOp->fY), (fSin * _pvOp->fX) + (fCos * _pvOp->fY), _pvOp->fZ);

  /* Done! */
  return _pvRes;
}

/** Is vector null?
 * @param[in]   _pvOp                         Vector to test
 * @return      orxTRUE if vector's null, orxFALSE otherwise
 */
orxSTATIC orxINLINE orxBOOL                   orxVector_IsNull(orxCONST orxVECTOR *_pvOp)
{
  orxBOOL bResult;

  /* Checks */
  orxASSERT(_pvOp  != orxNULL);

  /* Updates result */
  bResult = ((_pvOp->fX == orxFLOAT_0) && (_pvOp->fY == orxFLOAT_0) && (_pvOp->fZ == orxFLOAT_0)) ? orxTRUE : orxFALSE;

  /* Done! */
  return bResult;
}

/** Are vectors equal?
 * @param[in]   _pvOp1                        First vector to compare
 * @param[in]   _pvOp2                        Second vector to compare
 * @return      orxTRUE if both vectors are equal, orxFALSE otherwise
 */
orxSTATIC orxINLINE orxBOOL                   orxVector_AreEqual(orxCONST orxVECTOR *_pvOp1, orxCONST orxVECTOR *_pvOp2)
{
  orxBOOL bResult;

  /* Checks */
  orxASSERT(_pvOp1  != orxNULL);
  orxASSERT(_pvOp2  != orxNULL);

  /* Updates result */
  bResult = ((_pvOp1->fX == _pvOp2->fX) && (_pvOp1->fY == _pvOp2->fY) && (_pvOp1->fZ == _pvOp2->fZ)) ? orxTRUE : orxFALSE;

  /* Done! */
  return bResult;
}

/** Transforms a cartesian vector into a spherical one
 * @param[out]   _pvRes                       Vector where to store result (can be the operand)
 * @param[in]   _pvOp                         Vector to transform
 * @return      Transformed vector
 */
orxSTATIC orxINLINE orxVECTOR *               orxVector_FromCartesianToSpherical(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp)
{
  /* Checks */
  orxASSERT(_pvRes != orxNULL);
  orxASSERT(_pvOp  != orxNULL);

  /* Is operand vector null? */
  if((_pvOp->fX == orxFLOAT_0)
  && (_pvOp->fY == orxFLOAT_0)
  && (_pvOp->fZ == orxFLOAT_0))
  {
    /* Updates result vector */
    _pvRes->fRho = _pvRes->fTheta = _pvRes->fPhi = orxFLOAT_0;
  }
  else
  {
    orxFLOAT fRho, fTheta, fPhi;

    /* Z = 0? */
    if(_pvOp->fZ == orxFLOAT_0)
    {
      /* X = 0? */
      if(_pvOp->fX == orxFLOAT_0)
      {
        /* Sets rho */
        fRho = _pvOp->fY;
      }
      /* X != 0 and Y = 0? */
      else if(_pvOp->fY == orxFLOAT_0)
      {
        /* Sets rho */
        fRho = _pvOp->fX;
      }
      /* X != 0 and Y != 0 */
      else
      {
        /* Computes rho */
        fRho = orxMath_Sqrt((_pvOp->fX * _pvOp->fX) + (_pvOp->fY * _pvOp->fY));
      }

      /* Sets phi */
      fPhi = orxMATH_KF_PI_BY_2;
    }
    else
    {
      /* X = 0 and Y = 0? */
      if((_pvOp->fX == orxFLOAT_0) && (_pvOp->fY == orxFLOAT_0))
      {
        /* Z < 0? */
        if(*(orxU32 *)&(_pvOp->fZ) & (orxU32)0x80000000)
        {
          orxU32              u32Temp;
          orxREGISTER orxU32 *pu32Temp;

          /* Gets absolute value */
          u32Temp = *(orxU32 *)&(_pvOp->fZ) & (orxU32)0x7FFFFFFF;

          /* Sets rho */
          pu32Temp  = &u32Temp;
          fRho      = *(orxFLOAT *)pu32Temp;

          /* Sets phi */
          fPhi = orxMATH_KF_PI;
        }
        else
        {
          /* Sets rho */
          fRho = _pvOp->fZ;

          /* Sets phi */
          fPhi = orxFLOAT_0;
        }
      }
      else
      {
        /* Computes rho */
        fRho = orxMath_Sqrt(orxVector_GetSquareSize(_pvOp));

        /* Computes phi */
        fPhi = orxMath_ACos(_pvOp->fZ / fRho);
      }
    }

    /* Computes theta */
    fTheta = orxMath_ATan(_pvOp->fY, _pvOp->fX);

    /* Updates result */
    _pvRes->fRho    = fRho;
    _pvRes->fTheta  = fTheta;
    _pvRes->fPhi    = fPhi;
  }

  /* Done! */
  return _pvRes;
}

/** Transforms a spherical vector into a cartesian one
 * @param[out]   _pvRes                       Vector where to store result (can be the operand)
 * @param[in]   _pvOp                         Vector to transform
 * @return      Transformed vector
 */
orxSTATIC orxINLINE orxVECTOR *               orxVector_FromSphericalToCartesian(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp)
{
  orxFLOAT fSinPhi, fCosPhi, fSinTheta, fCosTheta;

  /* Checks */
  orxASSERT(_pvRes != orxNULL);
  orxASSERT(_pvOp  != orxNULL);

  /* Gets sine & cosine */
  fSinTheta = orxMath_Sin(_pvOp->fTheta);
  fCosTheta = orxMath_Cos(_pvOp->fTheta);
  fSinPhi   = orxMath_Sin(_pvOp->fPhi);
  fCosPhi   = orxMath_Cos(_pvOp->fPhi);

  /* Updates result */
  _pvRes->fX = _pvOp->fRho * fCosTheta * fSinPhi;
  _pvRes->fY = _pvOp->fRho * fSinTheta * fSinPhi;
  _pvRes->fZ = _pvOp->fRho * fCosPhi;

  /* Done! */
  return _pvRes;
}

/** Gets dot product of two vectors
 * @param[in]   _pvOp1                      First operand
 * @param[in]   _pvOp2                      Second operand
 * @return      Dot product
 */
orxSTATIC orxINLINE orxFLOAT                  orxVector_Dot(orxCONST orxVECTOR *_pvOp1, orxCONST orxVECTOR *_pvOp2)
{
  orxFLOAT fResult;

  /* Checks */
  orxASSERT(_pvOp1 != orxNULL);
  orxASSERT(_pvOp2  != orxNULL);

  /* Updates result */
  fResult = (_pvOp1->fX * _pvOp2->fX) + (_pvOp1->fY * _pvOp2->fY) + (_pvOp1->fZ * _pvOp2->fZ);

  /* Done! */
  return fResult;
}

/** Gets 2D dot product of two vectors
 * @param[in]   _pvOp1                      First operand
 * @param[in]   _pvOp2                      Second operand
 * @return      2D dot product
 */
orxSTATIC orxINLINE orxFLOAT                  orxVector_2DDot(orxCONST orxVECTOR *_pvOp1, orxCONST orxVECTOR *_pvOp2)
{
  orxFLOAT fResult;

  /* Checks */
  orxASSERT(_pvOp1 != orxNULL);
  orxASSERT(_pvOp2  != orxNULL);

  /* Updates result */
  fResult = (_pvOp1->fX * _pvOp2->fX) + (_pvOp1->fY * _pvOp2->fY);

  /* Done! */
  return fResult;
}

/** Gets cross product of two vectors
 * @param[out]  _pvRes                      Vector where to store result
 * @param[in]   _pvOp1                      First operand
 * @param[in]   _pvOp2                      Second operand
 * @return      Cross product orxVECTOR / orxNULL
 */
orxSTATIC orxINLINE orxVECTOR *               orxVector_Cross(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp1, orxCONST orxVECTOR *_pvOp2)
{
  orxFLOAT fTemp1, fTemp2;

  /* Checks */
  orxASSERT(_pvRes != orxNULL);
  orxASSERT(_pvOp1 != orxNULL);
  orxASSERT(_pvOp2  != orxNULL);

  /* Computes cross product */
  fTemp1      = (_pvOp1->fY * _pvOp2->fZ) - (_pvOp1->fZ * _pvOp2->fY);
  fTemp2      = (_pvOp1->fZ * _pvOp2->fX) - (_pvOp1->fX * _pvOp2->fZ);
  _pvRes->fZ  = (_pvOp1->fX * _pvOp2->fY) - (_pvOp1->fY * _pvOp2->fX);
  _pvRes->fY  = fTemp2;
  _pvRes->fX  = fTemp1;

  /* Done! */
  return _pvRes;
}


/* *** Vector constants *** */


extern orxDLLAPI orxCONST orxVECTOR orxVECTOR_X;      /**< X-Axis unit vector */
extern orxDLLAPI orxCONST orxVECTOR orxVECTOR_Y;      /**< Y-Axis unit vector */
extern orxDLLAPI orxCONST orxVECTOR orxVECTOR_Z;      /**< Z-Axis unit vector */

extern orxDLLAPI orxCONST orxVECTOR orxVECTOR_0;      /**< Null vector */
extern orxDLLAPI orxCONST orxVECTOR orxVECTOR_1;      /**< Vector filled with 1s */

extern orxDLLAPI orxCONST orxVECTOR orxVECTOR_RED;    /**< Red color vector */
extern orxDLLAPI orxCONST orxVECTOR orxVECTOR_GREEN;  /**< Green color vector */
extern orxDLLAPI orxCONST orxVECTOR orxVECTOR_BLUE;   /**< Blue color vector */
extern orxDLLAPI orxCONST orxVECTOR orxVECTOR_WHITE;  /**< White color vector */


/* *** AABox inlined functions */


/** Reorders AABox corners
 * @param[in]   _pstBox                       Box to reorder
 * @return      Reordered AABox
 */
orxSTATIC orxINLINE orxAABOX *                orxAABox_Reorder(orxAABOX *_pstBox)
{
  /* Checks */
  orxASSERT(_pstBox != orxNULL);

  /* Reorders coordinates so as to have upper left & bottom right box corners */

  /* Z coord */
  if(_pstBox->vTL.fZ > _pstBox->vBR.fZ)
  {
    /* Swaps */
    orxSWAP32(_pstBox->vTL.fZ, _pstBox->vBR.fZ);
  }

  /* Y coord */
  if(_pstBox->vTL.fY > _pstBox->vBR.fY)
  {
    /* Swaps */
    orxSWAP32(_pstBox->vTL.fY, _pstBox->vBR.fY);
  }

  /* X coord */
  if(_pstBox->vTL.fX > _pstBox->vBR.fX)
  {
    /* Swaps */
    orxSWAP32(_pstBox->vTL.fX, _pstBox->vBR.fX);
  }

  /* Done! */
  return _pstBox;
}

/** Sets axis aligned box values
 * @param[out]  _pstRes                       AABox to set
 * @param[in]   _pvTL                         Top left corner
 * @param[in]   _pvBR                         Bottom right corner
 * @return      orxAABOX / orxNULL
 */
orxSTATIC orxINLINE orxAABOX *                orxAABox_Set(orxAABOX *_pstRes, orxCONST orxVECTOR *_pvTL, orxCONST orxVECTOR *_pvBR)
{
  /* Checks */
  orxASSERT(_pstRes != orxNULL);
  orxASSERT(_pvTL != orxNULL);
  orxASSERT(_pvBR != orxNULL);

  /* Sets values */
  orxVector_Copy(&(_pstRes->vTL), _pvTL);
  orxVector_Copy(&(_pstRes->vBR), _pvBR);

  /* Reorders corners */
  orxAABox_Reorder(_pstRes);

  /* Done! */
  return _pstRes;
}

/** Is position inside axis aligned box test
 * @param[in]   _pstBox                       Box to test against position
 * @param[in]   _pvPosition                   Position to test against the box
 * @return      orxTRUE if position is inside the box, orxFALSE otherwise
 */
orxSTATIC orxINLINE orxBOOL                   orxAABox_IsInside(orxCONST orxAABOX *_pstBox, orxCONST orxVECTOR *_pvPosition)
{
  orxREGISTER orxBOOL bResult = orxFALSE;

  /* Checks */
  orxASSERT(_pstBox != orxNULL);
  orxASSERT(_pvPosition != orxNULL);

  /* Z intersected? */
  if((_pvPosition->fZ >= _pstBox->vTL.fZ)
  && (_pvPosition->fZ <= _pstBox->vBR.fZ))
  {
    /* X intersected? */
    if((_pvPosition->fX >= _pstBox->vTL.fX)
    && (_pvPosition->fX <= _pstBox->vBR.fX))
    {
      /* Y intersected? */
      if((_pvPosition->fY >= _pstBox->vTL.fY)
      && (_pvPosition->fY <= _pstBox->vBR.fY))
      {
        /* Intersects */
        bResult = orxTRUE;
      }
    }
  }

  /* Done! */
  return bResult;
}

/** Tests axis aligned box intersection
 * @param[in]   _pstBox1                      First box operand
 * @param[in]   _pstBox2                      Second box operand
 * @return      orxTRUE if boxes intersect, orxFALSE otherwise
 */
orxSTATIC orxINLINE orxBOOL                   orxAABox_TestIntersection(orxCONST orxAABOX *_pstBox1, orxCONST orxAABOX *_pstBox2)
{
  orxREGISTER orxBOOL bResult = orxFALSE;

  /* Checks */
  orxASSERT(_pstBox1 != orxNULL);
  orxASSERT(_pstBox2 != orxNULL);

  /* Z intersected? */
  if((_pstBox2->vBR.fZ >= _pstBox1->vTL.fZ)
  && (_pstBox2->vTL.fZ <= _pstBox1->vBR.fZ))
  {
    /* X intersected? */
    if((_pstBox2->vBR.fX >= _pstBox1->vTL.fX)
    && (_pstBox2->vTL.fX <= _pstBox1->vBR.fX))
    {
      /* Y intersected? */
      if((_pstBox2->vBR.fY >= _pstBox1->vTL.fY)
      && (_pstBox2->vTL.fY <= _pstBox1->vBR.fY))
      {
        /* Intersects */
        bResult = orxTRUE;
      }
    }
  }

  /* Done! */
  return bResult;
}

/** Tests axis aligned box 2D intersection (no Z-axis test)
 * @param[in]   _pstBox1                      First box operand
 * @param[in]   _pstBox2                      Second box operand
 * @return      orxTRUE if boxes intersect in 2D, orxFALSE otherwise
 */
orxSTATIC orxINLINE orxBOOL                   orxAABox_Test2DIntersection(orxCONST orxAABOX *_pstBox1, orxCONST orxAABOX *_pstBox2)
{
  orxREGISTER orxBOOL bResult = orxFALSE;

  /* Checks */
  orxASSERT(_pstBox1 != orxNULL);
  orxASSERT(_pstBox2 != orxNULL);

  /* X intersected? */
  if((_pstBox2->vBR.fX >= _pstBox1->vTL.fX)
  && (_pstBox2->vTL.fX <= _pstBox1->vBR.fX))
  {
    /* Y intersected? */
    if((_pstBox2->vBR.fY >= _pstBox1->vTL.fY)
    && (_pstBox2->vTL.fY <= _pstBox1->vBR.fY))
    {
      /* Intersects */
      bResult = orxTRUE;
    }
  }

  /* Done! */
  return bResult;
}

/** Copies an AABox onto another one
 * @param[out]   _pstDst                      AABox to copy to (destination)
 * @param[in]   _pstSrc                       AABox to copy from (destination)
 * @return      Destination AABox
 */
orxSTATIC orxINLINE orxAABOX *                orxAABox_Copy(orxAABOX *_pstDst, orxCONST orxAABOX *_pstSrc)
{
  /* Checks */
  orxASSERT(_pstDst != orxNULL);
  orxASSERT(_pstSrc != orxNULL);

  /* Copies it */
  orxMemory_Copy(_pstDst, _pstSrc, sizeof(orxAABOX));

  /* Done! */
  return _pstDst;
}

/** Moves an AABox
 * @param[out]  _pstRes                       AABox where to store result
 * @param[in]   _pstOp                        AABox to move
 * @param[in]   _pvMove                       Move vector
 * @return      Moved AABox
 */
orxSTATIC orxINLINE orxAABOX *                orxAABox_Move(orxAABOX *_pstRes, orxCONST orxAABOX *_pstOp, orxCONST orxVECTOR *_pvMove)
{
  /* Checks */
  orxASSERT(_pstRes != orxNULL);
  orxASSERT(_pstOp != orxNULL);
  orxASSERT(_pvMove != orxNULL);

  /* Updates result */
  orxVector_Add(&(_pstRes->vTL), &(_pstOp->vTL), _pvMove);
  orxVector_Add(&(_pstRes->vBR), &(_pstOp->vBR), _pvMove);

  /* Done! */
  return _pstRes;
}

/** Gets AABox center position
 * @param[in]   _pstOp                        Concerned AABox
 * @param[out]  _pvRes                        Center position
 * @return      Center position vector
 */
orxSTATIC orxINLINE orxVECTOR *               orxAABox_GetCenter(orxCONST orxAABOX *_pstOp, orxVECTOR *_pvRes)
{
  /* Checks */
  orxASSERT(_pstOp != orxNULL);
  orxASSERT(_pvRes != orxNULL);

  /* Gets box center */
  orxVector_Add(_pvRes, &(_pstOp->vTL), &(_pstOp->vBR));
  orxVector_Mulf(_pvRes, _pvRes, orx2F(0.5f));

  /* Done! */
  return _pvRes;
}


/* *** AABox inlined functions */


/** Sets 2D oriented box values
 * @param[out]  _pstRes                       OBox to set
 * @param[in]   _pvWorldPosition              World space position vector
 * @param[in]   _pvPivot                      Pivot vector
 * @param[in]   _pvSize                       Size vector
 * @param[in]   _fAngle                       Z-axis angle
 * @return      orxOBOX / orxNULL
 */
orxSTATIC orxINLINE orxOBOX *                 orxOBox_2DSet(orxOBOX *_pstRes, orxCONST orxVECTOR *_pvWorldPosition, orxCONST orxVECTOR *_pvPivot, orxCONST orxVECTOR *_pvSize, orxFLOAT _fAngle)
{
  orxFLOAT fCos, fSin;

  /* Checks */
  orxASSERT(_pstRes != orxNULL);
  orxASSERT(_pvWorldPosition != orxNULL);
  orxASSERT(_pvPivot != orxNULL);

  /* Gets cosine and sine */
  fCos = orxMath_Cos(_fAngle);
  fSin = orxMath_Sin(_fAngle);

  /* Sets axis */
  orxVector_Set(&(_pstRes->vX), fCos * _pvSize->fX, fSin * _pvSize->fX, orxFLOAT_0);
  orxVector_Set(&(_pstRes->vY), -fSin * _pvSize->fY, fCos * _pvSize->fY, orxFLOAT_0);
  orxVector_Set(&(_pstRes->vZ), orxFLOAT_0, orxFLOAT_0, _pvSize->fZ);

  /* Sets pivot */
  orxVector_Set(&(_pstRes->vPivot), (fCos * _pvPivot->fX) + (fSin * _pvPivot->fY), (fCos * _pvPivot->fY) - (fSin * _pvPivot->fX), _pvPivot->fZ);

  /* Sets box position */
  orxVector_Copy(&(_pstRes->vPosition), _pvWorldPosition);

  /* Done! */
  return _pstRes;
}

/** Copies an OBox onto another one
 * @param[out]  _pstDst                       OBox to copy to (destination)
 * @param[in]   _pstSrc                       OBox to copy from (destination)
 * @return      Destination OBox
 */
orxSTATIC orxINLINE orxOBOX *                 orxOBox_Copy(orxOBOX *_pstDst, orxCONST orxOBOX *_pstSrc)
{
  /* Checks */
  orxASSERT(_pstDst != orxNULL);
  orxASSERT(_pstSrc != orxNULL);

  /* Copies it */
  orxMemory_Copy(_pstDst, _pstSrc, sizeof(orxOBOX));

  /* Done! */
  return _pstDst;
}

/** Gets OBox center position
 * @param[in]   _pstOp                        Concerned OBox
 * @param[out]  _pvRes                        Center position
 * @return      Center position vector
 */
orxSTATIC orxINLINE orxVECTOR *               orxOBox_GetCenter(orxCONST orxOBOX *_pstOp, orxVECTOR *_pvRes)
{
  /* Checks */
  orxASSERT(_pstOp != orxNULL);
  orxASSERT(_pvRes != orxNULL);

  /* Gets box center */
  orxVector_Add(_pvRes, orxVector_Add(_pvRes, &(_pstOp->vX), &(_pstOp->vY)), &(_pstOp->vZ));
  orxVector_Mulf(_pvRes, _pvRes, orx2F(0.5f));
  orxVector_Sub(_pvRes, orxVector_Add(_pvRes, _pvRes, &(_pstOp->vPosition)), &(_pstOp->vPivot));

  /* Done! */
  return _pvRes;
}

/** Moves an OBox
 * @param[out]  _pstRes                       OBox where to store result
 * @param[in]   _pstOp                        OBox to move
 * @param[in]   _pvMove                       Move vector
 * @return      Moved OBox
 */
orxSTATIC orxINLINE orxOBOX *                 orxOBox_Move(orxOBOX *_pstRes, orxCONST orxOBOX *_pstOp, orxCONST orxVECTOR *_pvMove)
{
  /* Checks */
  orxASSERT(_pstRes != orxNULL);
  orxASSERT(_pstOp != orxNULL);
  orxASSERT(_pvMove != orxNULL);

  /* Updates result */
  orxVector_Add(&(_pstRes->vPosition), &(_pstOp->vPosition), _pvMove);

  /* Done! */
  return _pstRes;
}

/** Rotates in 2D an OBox
 * @param[out]  _pstRes                       OBox where to store result
 * @param[in]   _pstOp                        OBox to rotate (its Z-axis vector will be unchanged)
 * @param[in]   _fAngle                       Z-axis rotation angle
 * @return      Rotated OBox
 */
orxSTATIC orxINLINE orxOBOX *                 orxOBox_2DRotate(orxOBOX *_pstRes, orxCONST orxOBOX *_pstOp, orxFLOAT _fAngle)
{
  orxREGISTER orxFLOAT fSin, fCos;

  /* Checks */
  orxASSERT(_pstRes != orxNULL);
  orxASSERT(_pstOp != orxNULL);

  /* Gets cos & sin of angle */
  fCos = orxMath_Cos(_fAngle);
  fSin = orxMath_Sin(_fAngle);

  /* Updates axis */
  orxVector_Set(&(_pstRes->vX), (fCos * _pstRes->vX.fX) - (fSin * _pstRes->vX.fY), (fSin * _pstRes->vX.fX) + (fCos * _pstRes->vX.fY), _pstRes->vX.fZ);
  orxVector_Set(&(_pstRes->vY), (fCos * _pstRes->vY.fX) - (fSin * _pstRes->vY.fY), (fSin * _pstRes->vY.fX) + (fCos * _pstRes->vY.fY), _pstRes->vY.fZ);

  /* Updates pivot */
  orxVector_Set(&(_pstRes->vPivot), (fCos * _pstRes->vPivot.fX) - (fSin * _pstRes->vPivot.fY), (fSin * _pstRes->vPivot.fX) + (fCos * _pstRes->vPivot.fY), _pstRes->vPivot.fZ);

  /* Done! */
  return _pstRes;
}

/** Is position inside oriented box test
 * @param[in]   _pstBox                       Box to test against position
 * @param[in]   _pvPosition                   Position to test against the box
 * @return      orxTRUE if position is inside the box, orxFALSE otherwise
 */
orxSTATIC orxINLINE orxBOOL                   orxOBox_IsInside(orxCONST orxOBOX *_pstBox, orxCONST orxVECTOR *_pvPosition)
{
  orxREGISTER orxBOOL bResult = orxFALSE;
  orxFLOAT            fProj;
  orxVECTOR           vToPos;

  /* Checks */
  orxASSERT(_pstBox != orxNULL);
  orxASSERT(_pvPosition != orxNULL);

  /* Gets origin to position vector */
  orxVector_Sub(&vToPos, _pvPosition, orxVector_Sub(&vToPos, &(_pstBox->vPosition), &(_pstBox->vPivot)));

  /* Z-axis test */
  if(((fProj = orxVector_Dot(&vToPos, &(_pstBox->vZ))) >= orxFLOAT_0)
  && (fProj <= orxVector_GetSquareSize(&(_pstBox->vZ))))
  {
    /* X-axis test */
    if(((fProj = orxVector_Dot(&vToPos, &(_pstBox->vX))) >= orxFLOAT_0)
    && (fProj <= orxVector_GetSquareSize(&(_pstBox->vX))))
    {
      /* Y-axis test */
      if(((fProj = orxVector_Dot(&vToPos, &(_pstBox->vY))) >= orxFLOAT_0)
      && (fProj <= orxVector_GetSquareSize(&(_pstBox->vY))))
      {
        /* Updates result */
        bResult = orxTRUE;
      }
    }
  }

  /* Done! */
  return bResult;
}

/** Tests oriented 2D box intersection (simple Z-axis test, to use with Z-axis aligned orxOBOX)
 * @param[in]   _pstBox1                      First box operand
 * @param[in]   _pstBox2                      Second box operand
 * @return      orxTRUE if boxes intersect, orxFALSE otherwise
 */
orxSTATIC orxINLINE orxBOOL                   orxOBox_2DTestIntersection(orxCONST orxOBOX *_pstBox1, orxCONST orxOBOX *_pstBox2)
{
  orxREGISTER orxBOOL bResult;

  /* Checks */
  orxASSERT(_pstBox1 != orxNULL);
  orxASSERT(_pstBox2 != orxNULL);
  orxASSERT((_pstBox1->vZ.fX == orxFLOAT_0) && (_pstBox1->vZ.fX == orxFLOAT_0));
  orxASSERT((_pstBox1->vZ.fY == orxFLOAT_0) && (_pstBox1->vZ.fY == orxFLOAT_0));
  orxASSERT((_pstBox1->vZ.fZ >= orxFLOAT_0) && (_pstBox1->vZ.fZ >= orxFLOAT_0));

  /* Z intersected? */
  if((_pstBox2->vPosition.fZ + _pstBox2->vZ.fZ >= _pstBox1->vPosition.fZ)
  && (_pstBox2->vPosition.fZ <= _pstBox1->vPosition.fZ + _pstBox1->vZ.fZ))
  {
    orxU32            i;
    orxVECTOR         vOrigin1, vOrigin2, *pvOrigin1 = &vOrigin1, *pvOrigin2 = &vOrigin2, *pvTemp;
    orxCONST orxOBOX *pstBox1 = _pstBox1, *pstBox2 = _pstBox2, *pstTemp;

    /* Computes boxes origins */
    vOrigin1.fX = _pstBox1->vPosition.fX - pstBox1->vPivot.fX;
    vOrigin1.fY = _pstBox1->vPosition.fY - pstBox1->vPivot.fY;
    vOrigin2.fX = _pstBox2->vPosition.fX - pstBox2->vPivot.fX;
    vOrigin2.fY = _pstBox2->vPosition.fY - pstBox2->vPivot.fY;

    /* Test each box against the other */
    for(i = 2, bResult = orxTRUE;
        i != 0;
        i--, pstTemp = pstBox1, pstBox1 = pstBox2, pstBox2 = pstTemp, pvTemp = pvOrigin1, pvOrigin1 = pvOrigin2, pvOrigin2 = pvTemp)
    {
      orxVECTOR           vToCorner[4];
      orxCONST orxVECTOR *pvAxis;
      orxU32              j;

      /* Gets to-corner vectors */
      vToCorner[0].fX = pvOrigin2->fX - pvOrigin1->fX;
      vToCorner[0].fY = pvOrigin2->fY - pvOrigin1->fY;
      vToCorner[1].fX = vToCorner[0].fX + pstBox2->vX.fX;
      vToCorner[1].fY = vToCorner[0].fY + pstBox2->vX.fY;
      vToCorner[2].fX = vToCorner[1].fX + pstBox2->vY.fX;
      vToCorner[2].fY = vToCorner[1].fY + pstBox2->vY.fY;
      vToCorner[3].fX = vToCorner[0].fX + pstBox2->vY.fX;
      vToCorner[3].fY = vToCorner[0].fY + pstBox2->vY.fY;

      /* For both axis */
      for(j = 2, pvAxis = &(pstBox1->vX);
          j != 0;
          j--, pvAxis++)
      {
        orxFLOAT  fMin, fMax, fProj;
        orxU32    k;

        /* Gets initial projected values */
        fMin = fMax = fProj = orxVector_2DDot(&vToCorner[0], pvAxis);

        /* For all remaining corners */
        for(k = 1; k < 4; k++)
        {
          /* Gets projected value */
          fProj = orxVector_2DDot(&vToCorner[k], pvAxis);

          /* Updates extrema */
          if(fProj > fMax)
          {
            fMax = fProj;
          }
          else if(fProj < fMin)
          {
            fMin = fProj;
          }
        }

        /* Not intersecting? */
        if((fMax < orxFLOAT_0)
        || (fMin > orxVector_GetSquareSize(pvAxis)))
        {
          /* Updates result */
          bResult = orxFALSE;
          break;
        }
      }
    }
  }
  else
  {
    // Updates result
    bResult = orxFALSE;
  }

  /* Done! */
  return bResult;
}

#endif /* _orxVECTOR_H_ */

/** @} */
