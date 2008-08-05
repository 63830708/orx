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


/** Public structure definition. */
typedef struct __orxVECTOR_t
{
  /** Coordinates : 12 */
  union
  {
    orxFLOAT fX;
    orxFLOAT fRho;
    orxFLOAT fR;
  };

  union
  {
    orxFLOAT fY;
    orxFLOAT fTheta;
    orxFLOAT fG;
  };

  union
  {
    orxFLOAT fZ;
    orxFLOAT fPhi;
    orxFLOAT fB;
  };

} orxVECTOR;

/** Public axis aligned box structure. */
typedef struct __orxAABOX_t
{
  /* Corners : 24 */
  orxVECTOR vTL, vBR;
} orxAABOX;


/* *** Vector inlined functions *** */


/** Sets vector x / y / z / w values. */
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

/** Sets value in all vector fields. */
orxSTATIC orxINLINE orxVECTOR *               orxVector_SetAll(orxVECTOR *_pvVec, orxFLOAT _fValue)
{
  /* Done ! */
  return(orxVector_Set(_pvVec, _fValue, _fValue, _fValue));
}

/** Copies vector values into another one. */
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

/** Adds vectors and stores result in a third one. */
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

/** Subs vectors and stores result in a third one. */
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

/** Muls a vector by an orxFLOAT and stores result in another one. */
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

/** Muls a vector by another vector and stores result in a third one. */
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

/** Divs a vector by an orxFLOAT and stores result in another one. */
orxSTATIC orxINLINE orxVECTOR *               orxVector_Divf(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp1, orxFLOAT _fOp2)
{
  orxREGISTER orxFLOAT fInvCoef;

  /* Checks */
  orxASSERT(_pvRes != orxNULL);
  orxASSERT(_pvOp1 != orxNULL);
  orxASSERT(_fOp2 != orxFLOAT_0);

  /* Gets coef */
  fInvCoef = orxFLOAT_1 / _fOp2;

  /* Muls all */
  _pvRes->fX = _pvOp1->fX * fInvCoef;
  _pvRes->fY = _pvOp1->fY * fInvCoef;
  _pvRes->fZ = _pvOp1->fZ * fInvCoef;

  /* Done! */
  return _pvRes;
}

/** Divs a vector by another vector and stores result in a third one. */
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

/** Lerps from one vector to another one using a coefficient */
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

/** Gets minimum between two vectors */
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

/** Gets maximum between two vectors */
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

/** Clamps a vector between two others */
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

/** Negates a vector and stores result in another one */
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

/** Reverses a vector and stores result in another one. */
orxSTATIC orxINLINE orxVECTOR *               orxVector_Inv(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp)
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

orxSTATIC orxINLINE orxVECTOR *               orxVector_Normalize(orxVECTOR *_pvRes, orxCONST orxVECTOR *_pvOp)
{
  orxREGISTER orxFLOAT fOp;

  /* Checks */
  orxASSERT(_pvRes != orxNULL);
  orxASSERT(_pvOp  != orxNULL);

  /* Gets squared size */
  fOp = (_pvOp->fX * _pvOp->fX) + (_pvOp->fY * _pvOp->fY) + (_pvOp->fZ * _pvOp->fZ);

  /* Valid? */
  if(fOp> orxFLOAT_0)
  {
    /* Gets invert size */
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


/* *** Vector constants *** */


extern orxDLLAPI orxCONST orxVECTOR orxVECTOR_X;
extern orxDLLAPI orxCONST orxVECTOR orxVECTOR_Y;
extern orxDLLAPI orxCONST orxVECTOR orxVECTOR_Z;

extern orxDLLAPI orxCONST orxVECTOR orxVECTOR_0;
extern orxDLLAPI orxCONST orxVECTOR orxVECTOR_WHITE;


/* *** AABox inlined functions */


/** Tests axis aligned box intersection given corners (if corners are not sorted, test won't work). */
orxSTATIC orxINLINE orxBOOL                   orxAABox_TestIntersection(orxCONST orxAABOX *_pstBox1, orxCONST orxAABOX *_pstBox2)
{
  orxREGISTER orxBOOL bResult = orxFALSE;

  /* Checks */
  orxASSERT(_pstBox1 != orxNULL);
  orxASSERT(_pstBox2 != orxNULL);

  /* Warning : Corners should be sorted beforehand! */

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

/** Tests axis aligned box intersection given corners (if corners are not sorted, test won't work). */
orxSTATIC orxINLINE orxBOOL                   orxAABox_Test2DIntersection(orxCONST orxAABOX *_pstBox1, orxCONST orxAABOX *_pstBox2)
{
  orxREGISTER orxBOOL bResult = orxFALSE;

  /* Checks */
  orxASSERT(_pstBox1 != orxNULL);
  orxASSERT(_pstBox2 != orxNULL);

  /* Warning : Corners should be sorted beforehand! */

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

/** Reorders AABox corners */
orxSTATIC orxINLINE orxVOID                   orxAABox_Reorder(orxAABOX *_pstBox)
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
  return;
}

orxSTATIC orxINLINE orxAABOX *                orxAABox_2DRotate(orxAABOX *_pstRes, orxCONST orxAABOX *_pstOp, orxFLOAT _fAngle)
{
  orxREGISTER orxFLOAT fSin, fCos;

  /* Checks */
  orxASSERT(_pstRes  != orxNULL);
  orxASSERT(_pstOp != orxNULL);

  /* Gets cos & sin of angle */
  fCos = orxMath_Cos(_fAngle);
  fSin = orxMath_Sin(_fAngle);

  /* Updates result */
  orxVector_Set(&(_pstRes->vTL), (fCos * _pstOp->vTL.fX) - (fSin * _pstOp->vTL.fY), (fSin * _pstOp->vTL.fX) + (fCos * _pstOp->vTL.fY), _pstOp->vTL.fZ);
  orxVector_Set(&(_pstRes->vBR), (fCos * _pstOp->vBR.fX) - (fSin * _pstOp->vBR.fY), (fSin * _pstOp->vBR.fX) + (fCos * _pstOp->vBR.fY), _pstOp->vBR.fZ);

  /* Done! */
  return _pstRes;
}

/** Copies AABox values into another one. */
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

#endif /* _orxVECTOR_H_ */

/** @} */
