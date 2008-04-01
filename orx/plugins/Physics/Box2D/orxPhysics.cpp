/**
 * @file orxPhysics.cpp
 * 
 * Box2D physics plugin
 * 
 */
 
 /***************************************************************************
 orxPhysics.cpp
 Box2D physics plugin
 
 begin                : 24/03/2008
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

extern "C"
{
  #include "orxInclude.h"

  #include "core/orxConfig.h"
  #include "core/orxClock.h" 
  #include "plugin/orxPluginUser.h"

  #include "physics/orxPhysics.h"
}

#include <Box2D/Box2D.h>


/** Module flags
 */
#define orxPHYSICS_KU32_STATIC_FLAG_NONE        0x00000000 /**< No flags */

#define orxPHYSICS_KU32_STATIC_FLAG_READY       0x00000001 /**< Ready flag */

#define orxPHYSICS_KU32_STATIC_MASK_ALL         0xFFFFFFFF /**< All mask */


orxSTATIC orxCONST orxU32   su32DefaultIterations   = 10;
orxSTATIC orxCONST orxFLOAT sfDefaultFrequency      = orx2F(60.0f);
orxSTATIC orxCONST orxFLOAT sfDefaultDimensionRatio = orx2F(0.1f);


/***************************************************************************
 * Structure declaration                                                   *
 ***************************************************************************/

/** Static structure
 */
typedef struct __orxPHYSICS_STATIC_t
{
  orxU32            u32Flags;                   /**< Control flags */
  orxU32            u32Iterations;              /**< Simulation iterations per step */
  orxFLOAT          fDimensionRatio;            /**< Dimension ratio */
  orxCLOCK         *pstClock;                   /**< Simulation clock */
  b2World          *poWorld;                    /**< World */
  b2Body           *poGround;                   /**< Ground */

} orxPHYSICS_STATIC;


/***************************************************************************
 * Static variables                                                        *
 ***************************************************************************/

/** Static data
 */
orxSTATIC orxPHYSICS_STATIC sstPhysics;


/***************************************************************************
 * Private functions                                                       *
 ***************************************************************************/

/** Update (callback to register on a clock)
 * @param[in]   _pstClockInfo   Clock info of the clock used upon registration
 * @param[in]   _pstContext     Context sent when registering callback to the clock
 */
orxVOID orxFASTCALL orxPhysics_Update(orxCONST orxCLOCK_INFO *_pstClockInfo, orxVOID *_pstContext)
{
  /* Checks */
  orxASSERT(sstPhysics.u32Flags & orxPHYSICS_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstClockInfo != orxNULL);

  /* Updates world simulation */
  sstPhysics.poWorld->Step(_pstClockInfo->fDT, (orxU32)_pstContext);

  return;
}

extern "C" orxPHYSICS_BODY *orxPhysics_Box2D_CreateBody(orxCONST orxBODY_DEF *_pstBodyDef)
{
  b2Body     *poResult = 0;
  b2BodyDef   stBodyDef;

  /* Checks */
  orxASSERT(sstPhysics.u32Flags & orxPHYSICS_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstBodyDef != orxNULL);

  /* 2D? */
  if(orxFLAG_TEST(_pstBodyDef->u32Flags, orxBODY_DEF_KU32_FLAG_2D))
  {
    /* Inits body definition */
    stBodyDef.angle           = _pstBodyDef->fAngle;
    stBodyDef.linearDamping   = _pstBodyDef->fLinearDamping;
    stBodyDef.angularDamping  = _pstBodyDef->fAngularDamping;
    stBodyDef.isBullet        = orxFLAG_TEST(_pstBodyDef->u32Flags, orxBODY_DEF_KU32_FLAG_HIGH_SPEED);
    stBodyDef.fixedRotation   = orxFLAG_TEST(_pstBodyDef->u32Flags, orxBODY_DEF_KU32_FLAG_NO_ROTATION);
    stBodyDef.position.Set(_pstBodyDef->vPosition.fX, _pstBodyDef->vPosition.fY);

    /* Is dynamic? */
    if(orxFLAG_TEST(_pstBodyDef->u32Flags, orxBODY_DEF_KU32_FLAG_DYNAMIC))
    {
      /* Creates dynamic body */
      poResult = sstPhysics.poWorld->CreateDynamicBody(&stBodyDef);
    }
    else
    {
      /* Creates static body */
      poResult = sstPhysics.poWorld->CreateStaticBody(&stBodyDef);
    }
  }

  /* Done! */
  return (orxPHYSICS_BODY *)poResult;
}

extern "C" orxVOID orxPhysics_Box2D_DeleteBody(orxPHYSICS_BODY *_pstBody)
{
  /* Checks */
  orxASSERT(sstPhysics.u32Flags & orxPHYSICS_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstBody != orxNULL);

  /* Deletes it */
  sstPhysics.poWorld->DestroyBody((b2Body *)_pstBody);

  return;
}

extern "C" orxPHYSICS_BODY_PART *orxPhysics_Box2D_CreateBodyPart(orxPHYSICS_BODY *_pstBody, orxCONST orxBODY_PART_DEF *_pstBodyPartDef)
{
  b2Body       *poBody;
  b2Shape      *poResult = 0;
  b2ShapeDef   *pstShapeDef;
  b2CircleDef   stCircleDef;
  b2PolygonDef  stPolygonDef;

  /* Checks */
  orxASSERT(sstPhysics.u32Flags & orxPHYSICS_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstBody != orxNULL);
  orxASSERT(_pstBodyPartDef != orxNULL);
  orxASSERT(orxFLAG_TEST(_pstBodyPartDef->u32Flags, orxBODY_PART_DEF_KU32_FLAG_BOX | orxBODY_PART_DEF_KU32_FLAG_SPHERE));

  /* Gets body */
  poBody = (b2Body *)_pstBody;

  /* Circle? */
  if(orxFLAG_TEST(_pstBodyPartDef->u32Flags, orxBODY_PART_DEF_KU32_FLAG_SPHERE))
  {
    /* Gets def reference */
    pstShapeDef = &stCircleDef;

    /* Updates shape type */
    stCircleDef.type = e_circleShape;

    /* Stores its coordinates */
    stCircleDef.localPosition.Set(sstPhysics.fDimensionRatio * _pstBodyPartDef->stSphere.vCenter.fX, sstPhysics.fDimensionRatio * _pstBodyPartDef->stSphere.vCenter.fY);
    stCircleDef.radius = sstPhysics.fDimensionRatio * _pstBodyPartDef->stSphere.fRadius;
  }
  /* Polygon */
  else
  {
    /* Gets def reference */
    pstShapeDef = &stPolygonDef;

    /* Updates shape type */
    stPolygonDef.type = e_polygonShape;

    /* Stores its coordinates */
    stPolygonDef.vertexCount = 4;
    stPolygonDef.vertices[0].Set(sstPhysics.fDimensionRatio * _pstBodyPartDef->stAABox.stBox.vTL.fX, sstPhysics.fDimensionRatio * _pstBodyPartDef->stAABox.stBox.vTL.fY);
    stPolygonDef.vertices[1].Set(sstPhysics.fDimensionRatio * _pstBodyPartDef->stAABox.stBox.vTL.fX, sstPhysics.fDimensionRatio * _pstBodyPartDef->stAABox.stBox.vBR.fY);
    stPolygonDef.vertices[2].Set(sstPhysics.fDimensionRatio * _pstBodyPartDef->stAABox.stBox.vBR.fX, sstPhysics.fDimensionRatio * _pstBodyPartDef->stAABox.stBox.vBR.fY);
    stPolygonDef.vertices[3].Set(sstPhysics.fDimensionRatio * _pstBodyPartDef->stAABox.stBox.vBR.fX, sstPhysics.fDimensionRatio * _pstBodyPartDef->stAABox.stBox.vTL.fY);
  }

  /* Inits shape definition */
  pstShapeDef->friction     = _pstBodyPartDef->fFriction;
  pstShapeDef->restitution  = _pstBodyPartDef->fRestitution;
  pstShapeDef->density      = _pstBodyPartDef->fDensity;
  pstShapeDef->categoryBits = _pstBodyPartDef->u16SelfFlags;
  pstShapeDef->maskBits     = _pstBodyPartDef->u16CheckMask;
  pstShapeDef->groupIndex   = (orxU16)(sstPhysics.fDimensionRatio * _pstBodyPartDef->stAABox.stBox.vTL.fZ);
  pstShapeDef->isSensor     = orxFLAG_TEST(_pstBodyPartDef->u32Flags, orxBODY_PART_DEF_KU32_FLAG_RIGID) == orxFALSE;

  /* Creates it */
  poResult = poBody->CreateShape(pstShapeDef); 

  /* Done! */
  return (orxPHYSICS_BODY_PART *)poResult;
}

extern "C" orxVOID orxPhysics_Box2D_DeleteBodyPart(orxPHYSICS_BODY_PART *_pstBodyPart)
{
  b2Shape  *poShape;

  /* Checks */
  orxASSERT(sstPhysics.u32Flags & orxPHYSICS_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstBodyPart != orxNULL);

  /* Gets shape */
  poShape = (b2Shape *)_pstBodyPart;

  /* Deletes its part */
  poShape->GetBody()->DestroyShape(poShape);

  return;
}

extern "C" orxSTATUS orxPhysics_Box2D_SetPosition(orxPHYSICS_BODY *_pstBody, orxCONST orxVECTOR *_pvPosition)
{
  b2Body   *poBody;
  b2Vec2    vPosition;
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstPhysics.u32Flags & orxPHYSICS_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstBody != orxNULL);
  orxASSERT(_pvPosition != orxNULL);

  /* Gets body */
  poBody = (b2Body *)_pstBody;

  /* Sets position vector */
  vPosition.Set(_pvPosition->fX, _pvPosition->fY);

  /* Updates its position */
  eResult = (poBody->SetXForm(vPosition, poBody->GetAngle()) != false) ? orxSTATUS_SUCCESS : orxSTATUS_FAILURE;

  /* Done! */
  return eResult;
}

extern "C" orxSTATUS orxPhysics_Box2D_SetRotation(orxPHYSICS_BODY *_pstBody, orxFLOAT _fRotation)
{
  b2Body   *poBody;
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstPhysics.u32Flags & orxPHYSICS_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstBody != orxNULL);

  /* Gets body */
  poBody = (b2Body *)_pstBody;

  /* Updates its rotation */
  eResult = (poBody->SetXForm(poBody->GetPosition(), _fRotation) != false) ? orxSTATUS_SUCCESS : orxSTATUS_FAILURE;

  /* Done! */
  return eResult;
}

extern "C" orxSTATUS orxPhysics_Box2D_SetSpeed(orxPHYSICS_BODY *_pstBody, orxCONST orxVECTOR *_pvSpeed)
{
  b2Body   *poBody;
  b2Vec2    vSpeed;
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstPhysics.u32Flags & orxPHYSICS_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstBody != orxNULL);
  orxASSERT(_pvSpeed != orxNULL);

  /* Gets body */
  poBody = (b2Body *)_pstBody;

  /* Sets speed vector */
  vSpeed.Set(_pvSpeed->fX, _pvSpeed->fY);

  /* Updates its speed */
  poBody->SetLinearVelocity(vSpeed);

  /* Done! */
  return eResult;
}

extern "C" orxSTATUS orxPhysics_Box2D_SetAngularVelocity(orxPHYSICS_BODY *_pstBody, orxFLOAT _fVelocity)
{
  b2Body   *poBody;
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstPhysics.u32Flags & orxPHYSICS_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstBody != orxNULL);

  /* Gets body */
  poBody = (b2Body *)_pstBody;

  /* Updates its angular velocity */
  poBody->SetAngularVelocity(_fVelocity);

  /* Done! */
  return eResult;
}

extern "C" orxVECTOR *orxPhysics_Box2D_GetPosition(orxPHYSICS_BODY *_pstBody, orxVECTOR *_pvPosition)
{
  b2Body   *poBody;
  b2Vec2    vPosition;
  orxVECTOR *pvResult;

  /* Checks */
  orxASSERT(sstPhysics.u32Flags & orxPHYSICS_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstBody != orxNULL);
  orxASSERT(_pvPosition != orxNULL);

  /* Gets body */
  poBody = (b2Body *)_pstBody;

  /* Gets its position */
  vPosition = poBody->GetPosition();

  /* Updates result */
  pvResult      = _pvPosition;
  pvResult->fX  = vPosition.x;
  pvResult->fY  = vPosition.y;

  /* Done! */
  return pvResult;
}

extern "C" orxFLOAT orxPhysics_Box2D_GetRotation(orxPHYSICS_BODY *_pstBody)
{
  b2Body   *poBody;
  orxFLOAT fResult;

  /* Checks */
  orxASSERT(sstPhysics.u32Flags & orxPHYSICS_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstBody != orxNULL);

  /* Gets body */
  poBody = (b2Body *)_pstBody;

  /* Gets its rotation */
  fResult = poBody->GetAngle();

  /* Done! */
  return fResult;
}

extern "C" orxVECTOR *orxPhysics_Box2D_GetSpeed(orxPHYSICS_BODY *_pstBody, orxVECTOR *_pvSpeed)
{
  b2Body   *poBody;
  b2Vec2    vSpeed;
  orxVECTOR *pvResult;

  /* Checks */
  orxASSERT(sstPhysics.u32Flags & orxPHYSICS_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstBody != orxNULL);
  orxASSERT(_pvSpeed != orxNULL);

  /* Gets body */
  poBody = (b2Body *)_pstBody;

  /* Gets its speed */
  vSpeed = poBody->GetLinearVelocity();

  /* Updates result */
  pvResult      = _pvSpeed;
  pvResult->fX  = vSpeed.x;
  pvResult->fY  = vSpeed.y;

  /* Done! */
  return pvResult;
}

extern "C" orxFLOAT orxPhysics_Box2D_GetAngularVelocity(orxPHYSICS_BODY *_pstBody)
{
  b2Body   *poBody;
  orxFLOAT fResult;

  /* Checks */
  orxASSERT(sstPhysics.u32Flags & orxPHYSICS_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstBody != orxNULL);

  /* Gets body */
  poBody = (b2Body *)_pstBody;

  /* Gets its rotation */
  fResult = poBody->GetAngularVelocity();

  /* Done! */
  return fResult;
}

extern "C" orxSTATUS orxPhysics_Box2D_Init()
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Was not already initialized? */
  if(!(sstPhysics.u32Flags & orxPHYSICS_KU32_STATIC_FLAG_READY))
  {
    orxBOOL   bAllowSleep;
    orxVECTOR vGravity, vLower, vUpper;
    b2AABB    stWorldAABB;
    b2Vec2    vWorldGravity;

    /* Cleans static controller */
    orxMemory_Set(&sstPhysics, 0, sizeof(orxPHYSICS_STATIC));

    /* Gets gravity & allow sleep from config */
    orxConfig_SelectSection(orxPHYSICS_KZ_CONFIG_SECTION);
    orxConfig_GetVector(orxPHYSICS_KZ_CONFIG_GRAVITY, &vGravity);
    bAllowSleep = orxConfig_GetBool(orxPHYSICS_KZ_CONFIG_ALLOW_SLEEP);

    /* Gets world corners from config */
    orxConfig_GetVector(orxPHYSICS_KZ_CONFIG_WORLD_LOWER, &vLower);
    orxConfig_GetVector(orxPHYSICS_KZ_CONFIG_WORLD_UPPER, &vUpper);

    /* Inits world AABB */
    stWorldAABB.lowerBound.Set(vLower.fX, vLower.fY);
    stWorldAABB.upperBound.Set(vUpper.fX, vUpper.fY);

    /* Inits world gravity */
    vWorldGravity.Set(vGravity.fX, vGravity.fY);

    /* Creates world */
    sstPhysics.poWorld = new b2World(stWorldAABB, vWorldGravity, bAllowSleep);

    /* Success? */
    if(sstPhysics.poWorld != orxNULL)
    {
      b2BodyDef stGroundDef;

      /* Inits ground definition */
      stGroundDef.position.SetZero();

      /* Creates ground */
      sstPhysics.poGround = sstPhysics.poWorld->CreateStaticBody(&stGroundDef);

      /* Success? */
      if(sstPhysics.poGround != orxNULL)
      {
        orxFLOAT  fFrequency, fTickSize, fRatio;
        orxS32    s32IterationsPerStep;

        /* Gets dimension ratio */
        orxConfig_GetFloat(orxPHYSICS_KZ_CONFIG_RATIO);

        /* Valid? */
        if(fRatio > orxFLOAT_0)
        {
          /* Stores it */
          sstPhysics.fDimensionRatio = fRatio;
        }
        else
        {
          /* Stores default one */
          sstPhysics.fDimensionRatio = sfDefaultDimensionRatio;
        }

        /* Gets iteration per step number from config */
        orxConfig_GetS32(orxPHYSICS_KZ_CONFIG_ITERATIONS);

        /* Valid? */
        if(s32IterationsPerStep > 0)
        {
          /* Stores it */
          sstPhysics.u32Iterations = (orxU32)s32IterationsPerStep;
        }
        else
        {
          /* Uses default value */
          sstPhysics.u32Iterations = su32DefaultIterations;
        }

        /* Gets frequency */
        fFrequency = orxConfig_GetFloat(orxPHYSICS_KZ_CONFIG_FREQUENCY);

        /* Valid? */
        if(fFrequency <= orxFLOAT_0)
        {
          /* Gets tick size */
          fTickSize = orxFLOAT_1 / fFrequency;
        }
        else
        {
          /* Gets default tick size */
          fTickSize = orxFLOAT_1 / sfDefaultFrequency;
        }

        /* Creates physics clock */
        sstPhysics.pstClock = orxClock_Create(fTickSize, orxCLOCK_TYPE_PHYSICS);

        /* Valid? */
        if(sstPhysics.pstClock != orxNULL)
        {
          /* Registers rendering function */
          eResult = orxClock_Register(sstPhysics.pstClock, orxPhysics_Update, (orxVOID *)sstPhysics.u32Iterations, orxMODULE_ID_PHYSICS);

          /* Valid? */
          if(eResult != orxSTATUS_FAILURE)
          {
            /* Updates status */
            sstPhysics.u32Flags |= orxPHYSICS_KU32_STATIC_FLAG_READY;
          }
          else
          {
            /* Deletes world */
            delete sstPhysics.poWorld;

            /* Updates result */
            eResult = orxSTATUS_FAILURE;
          }
        }
        else
        {
          /* Deletes world */
          delete sstPhysics.poWorld;

          /* Updates result */
          eResult = orxSTATUS_FAILURE;
        }
      }
      else
      {
        /* Deletes world */
        delete sstPhysics.poWorld;

        /* Updates result */
        eResult = orxSTATUS_FAILURE;
      }
    }
    else
    {
      /* Updates result */
      eResult = orxSTATUS_FAILURE;
    }
  }

  /* Done! */
  return eResult;  
}

extern "C" orxVOID orxPhysics_Box2D_Exit()
{
  /* Was initialized? */
  if(sstPhysics.u32Flags & orxPHYSICS_KU32_STATIC_FLAG_READY)
  {
    /* Deletes ground */
    sstPhysics.poWorld->DestroyBody(sstPhysics.poGround);

    /* Deletes world */
    delete sstPhysics.poWorld;

    /* Cleans static controller */
    orxMemory_Set(&sstPhysics, 0, sizeof(orxPHYSICS_STATIC));
  }

  return;
}


/********************
 *  Plugin Related  *
 ********************/

orxPLUGIN_USER_CORE_FUNCTION_START(PHYSICS);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxPhysics_Box2D_Init, PHYSICS, INIT);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxPhysics_Box2D_Exit, PHYSICS, EXIT);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxPhysics_Box2D_CreateBody, PHYSICS, CREATE_BODY);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxPhysics_Box2D_DeleteBody, PHYSICS, DELETE_BODY);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxPhysics_Box2D_CreateBodyPart, PHYSICS, CREATE_BODY_PART);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxPhysics_Box2D_DeleteBodyPart, PHYSICS, DELETE_BODY_PART);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxPhysics_Box2D_SetPosition, PHYSICS, SET_POSITION);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxPhysics_Box2D_SetRotation, PHYSICS, SET_ROTATION);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxPhysics_Box2D_SetSpeed, PHYSICS, SET_SPEED);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxPhysics_Box2D_SetAngularVelocity, PHYSICS, SET_ANGULAR_VELOCITY);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxPhysics_Box2D_GetPosition, PHYSICS, GET_POSITION);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxPhysics_Box2D_GetRotation, PHYSICS, GET_ROTATION);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxPhysics_Box2D_GetSpeed, PHYSICS, GET_SPEED);
orxPLUGIN_USER_CORE_FUNCTION_ADD(orxPhysics_Box2D_GetAngularVelocity, PHYSICS, GET_ANGULAR_VELOCITY);
orxPLUGIN_USER_CORE_FUNCTION_END();
