/* Orx - Portable Game Engine
 *
 * Copyright (c) 2008-2015 Orx-Project
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *    1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 *    2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 *    3. This notice may not be removed or altered from any source
 *    distribution.
 */

/**
 * @file orxParticleGroup.c
 * @date 16/02/2015
 * @author simons.philippe@gmail.com
 *
 */


#include "physics/orxParticleGroup.h"
#include "physics/orxPhysics.h"
#include "core/orxConfig.h"
#include "object/orxObject.h"
#include "object/orxFrame.h"
#include "utils/orxString.h"

#include "debug/orxDebug.h"
#include "debug/orxProfiler.h"
#include "memory/orxMemory.h"


/** ParticleGroup flags
 */
#define orxPARTICLEGROUP_KU32_FLAG_NONE                0x00000000  /**< No flags */

#define orxPARTICLEGROUP_KU32_FLAG_HAS_DATA            0x00000001  /**< Has data flag */

#define orxPARTICLEGROUP_KU32_MASK_ALL                 0xFFFFFFFF  /**< User all ID mask */


/** Module flags
 */
#define orxPARTICLEGROUP_KU32_STATIC_FLAG_NONE         0x00000000  /**< No flags */

#define orxPARTICLEGROUP_KU32_STATIC_FLAG_READY        0x10000000  /**< Ready flag */

#define orxPARTICLEGROUP_KU32_MASK_ALL                 0xFFFFFFFF  /**< All mask */


/** Misc defines
 */
#define orxBODY_KZ_CONFIG_INERTIA             "Inertia"
#define orxBODY_KZ_CONFIG_MASS                "Mass"
#define orxBODY_KZ_CONFIG_LINEAR_DAMPING      "LinearDamping"
#define orxBODY_KZ_CONFIG_ANGULAR_DAMPING     "AngularDamping"
#define orxBODY_KZ_CONFIG_CUSTOM_GRAVITY      "CustomGravity"
#define orxBODY_KZ_CONFIG_FIXED_ROTATION      "FixedRotation"
#define orxBODY_KZ_CONFIG_ALLOW_SLEEP         "AllowSleep"
#define orxBODY_KZ_CONFIG_ALLOW_GROUND_SLIDING "AllowGroundSliding"
#define orxBODY_KZ_CONFIG_ALLOW_MOVING        "AllowMoving"
#define orxBODY_KZ_CONFIG_HIGH_SPEED          "HighSpeed"
#define orxBODY_KZ_CONFIG_DYNAMIC             "Dynamic"
#define orxBODY_KZ_CONFIG_PART_LIST           "PartList"
#define orxBODY_KZ_CONFIG_FRICTION            "Friction"
#define orxBODY_KZ_CONFIG_RESTITUTION         "Restitution"
#define orxBODY_KZ_CONFIG_DENSITY             "Density"
#define orxBODY_KZ_CONFIG_SELF_FLAGS          "SelfFlags"
#define orxBODY_KZ_CONFIG_CHECK_MASK          "CheckMask"
#define orxBODY_KZ_CONFIG_TYPE                "Type"
#define orxBODY_KZ_CONFIG_SOLID               "Solid"
#define orxBODY_KZ_CONFIG_TOP_LEFT            "TopLeft"
#define orxBODY_KZ_CONFIG_BOTTOM_RIGHT        "BottomRight"
#define orxBODY_KZ_CONFIG_CENTER              "Center"
#define orxBODY_KZ_CONFIG_RADIUS              "Radius"
#define orxBODY_KZ_CONFIG_VERTEX_LIST         "VertexList"
#define orxBODY_KZ_CONFIG_PARENT_ANCHOR       "ParentAnchor"
#define orxBODY_KZ_CONFIG_CHILD_ANCHOR        "ChildAnchor"
#define orxBODY_KZ_CONFIG_COLLIDE             "Collide"
#define orxBODY_KZ_CONFIG_ROTATION            "Rotation"
#define orxBODY_KZ_CONFIG_MIN_ROTATION        "MinRotation"
#define orxBODY_KZ_CONFIG_MAX_ROTATION        "MaxRotation"
#define orxBODY_KZ_CONFIG_MOTOR_SPEED         "MotorSpeed"
#define orxBODY_KZ_CONFIG_MAX_MOTOR_FORCE     "MaxMotorForce"
#define orxBODY_KZ_CONFIG_MAX_MOTOR_TORQUE    "MaxMotorTorque"
#define orxBODY_KZ_CONFIG_MAX_FORCE           "MaxForce"
#define orxBODY_KZ_CONFIG_MAX_TORQUE          "MaxTorque"
#define orxBODY_KZ_CONFIG_TRANSLATION_AXIS    "TranslationAxis"
#define orxBODY_KZ_CONFIG_MIN_TRANSLATION     "MinTranslation"
#define orxBODY_KZ_CONFIG_MAX_TRANSLATION     "MaxTranslation"
#define orxBODY_KZ_CONFIG_LENGTH              "Length"
#define orxBODY_KZ_CONFIG_FREQUENCY           "Frequency"
#define orxBODY_KZ_CONFIG_DAMPING             "Damping"
#define orxBODY_KZ_CONFIG_PARENT_GROUND_ANCHOR "ParentGroundAnchor"
#define orxBODY_KZ_CONFIG_CHILD_GROUND_ANCHOR "ChildGroundAnchor"
#define orxBODY_KZ_CONFIG_PARENT_LENGTH       "ParentLength"
#define orxBODY_KZ_CONFIG_MAX_PARENT_LENGTH   "MaxParentLength"
#define orxBODY_KZ_CONFIG_CHILD_LENGTH        "ChildLength"
#define orxBODY_KZ_CONFIG_MAX_CHILD_LENGTH    "MaxChildLength"
#define orxBODY_KZ_CONFIG_LENGTH_RATIO        "LengthRatio"
#define orxBODY_KZ_CONFIG_JOINT_RATIO         "JointRatio"
#define orxBODY_KZ_CONFIG_PARENT_JOINT_NAME   "ParentJoint"
#define orxBODY_KZ_CONFIG_CHILD_JOINT_NAME    "ChildJoint"

#define orxBODY_KZ_FULL                       "full"
#define orxBODY_KZ_TYPE_SPHERE                "sphere"
#define orxBODY_KZ_TYPE_BOX                   "box"
#define orxBODY_KZ_TYPE_MESH                  "mesh"
#define orxBODY_KZ_TYPE_REVOLUTE              "revolute"
#define orxBODY_KZ_TYPE_PRISMATIC             "prismatic"
#define orxBODY_KZ_TYPE_SPRING                "spring"
#define orxBODY_KZ_TYPE_ROPE                  "rope"
#define orxBODY_KZ_TYPE_PULLEY                "pulley"
#define orxBODY_KZ_TYPE_SUSPENSION            "suspension"
#define orxBODY_KZ_TYPE_WELD                  "weld"
#define orxBODY_KZ_TYPE_FRICTION              "friction"
#define orxBODY_KZ_TYPE_GEAR                  "gear"

#define orxPARTICLEGROUP_KU32_BANK_SIZE       256         /**< Bank size */


/***************************************************************************
 * Structure declaration                                                   *
 ***************************************************************************/

/** ParticleGroup structure
 */
struct __orxPARTICLEGROUP_t
{
  orxSTRUCTURE                stStructure;                                /**< Public structure, first structure member : 32 */
  orxPHYSICS_PARTICLEGROUP   *pstData;                                    /**< Physics body data : 44 */
  const orxSTRUCTURE         *pstOwner;                                   /**< Owner structure : 48 */
  orxU32                      u32DefFlags;                                /**< Definition flags : 52 */
};

/** Static structure
 */
typedef struct __orxPARTICLEGROUP_STATIC_t
{
  orxU32            u32Flags;                                         /**< Control flags */

} orxPARTICLEGROUP_STATIC;


/***************************************************************************
 * Static variables                                                        *
 ***************************************************************************/

/** Static data
 */
static orxPARTICLEGROUP_STATIC sstParticleGroup;


/***************************************************************************
 * Private functions                                                       *
 ***************************************************************************/

/** Deletes all bodies
 */
static orxINLINE void orxParticleGroup_DeleteAll()
{
  register orxPARTICLEGROUP *pstParticleGroup;

  /* Gets first ParticleGroup */
  pstParticleGroup = orxPARTICLEGROUP(orxStructure_GetFirst(orxSTRUCTURE_ID_PARTICLEGROUP));

  /* Non empty? */
  while(pstParticleGroup != orxNULL)
  {
    /* Deletes ParticleGroup */
    orxParticleGroup_Delete(pstBody);

    /* Gets first ParticleGroup */
    pstParticleGroup = orxPARTICLEGROUP(orxStructure_GetFirst(orxSTRUCTURE_ID_PARTICLEGROUP));
  }

  return;
}

static orxINLINE orxU16 orxParticleGroup_GetCollisionFlag(const orxSTRING _zConfigID)
{
  orxU32  u32Value;
  orxU16  u16Result = 0;

  /* Gets its numerical value */
  u32Value = orxConfig_GetListU32(_zConfigID, 0);

  /* Is 0? */
  if(u32Value == 0)
  {
    orxU32 u32Counter, i;

    /* For all elements */
    for(i = 0, u32Counter = orxConfig_GetListCounter(_zConfigID); i < u32Counter; i++)
    {
      /* Updates result with numerical value */
      u16Result |= (orxU16)orxPhysics_GetCollisionFlagValue(orxConfig_GetListString(_zConfigID, i));
    }
  }
  else
  {
    /* Updates result */
    u16Result = (orxU16)u32Value;
  }

  /* Done! */
  return u16Result;
}


/***************************************************************************
 * Public functions                                                        *
 ***************************************************************************/

/** ParticleGroup module setup
 */
void orxFASTCALL orxParticleGroup_Setup()
{
  /* Adds module dependencies */
  orxModule_AddDependency(orxMODULE_ID_BODY, orxMODULE_ID_MEMORY);
  orxModule_AddDependency(orxMODULE_ID_BODY, orxMODULE_ID_STRING);
  orxModule_AddDependency(orxMODULE_ID_BODY, orxMODULE_ID_PROFILER);
  orxModule_AddDependency(orxMODULE_ID_BODY, orxMODULE_ID_STRUCTURE);
  orxModule_AddDependency(orxMODULE_ID_BODY, orxMODULE_ID_PHYSICS);
  orxModule_AddDependency(orxMODULE_ID_BODY, orxMODULE_ID_FRAME);
  orxModule_AddDependency(orxMODULE_ID_BODY, orxMODULE_ID_CONFIG);

  return;
}

/** Inits the ParticleGroup module
 */
orxSTATUS orxFASTCALL orxParticleGroup_Init()
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Not already Initialized? */
  if((sstParticleGroup.u32Flags & orxPARTICLEGROUP_KU32_STATIC_FLAG_READY) == orxPARTICLEGROUP_KU32_STATIC_FLAG_NONE)
  {
    /* Cleans static controller */
    orxMemory_Zero(&sstParticleGroup, sizeof(orxPARTICLEGROUP_STATIC));

    /* Registers structure type */
    eResult = orxSTRUCTURE_REGISTER(PARTICLEGROUP, orxSTRUCTURE_STORAGE_TYPE_LINKLIST, orxMEMORY_TYPE_MAIN, orxPARTICLEGROUP_KU32_BANK_SIZE, orxNULL);
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Tried to initialize particle group module when it is already initialized.");

    /* Already initialized */
    eResult = orxSTATUS_SUCCESS;
  }

  /* Initialized? */
  if(eResult != orxSTATUS_FAILURE)
  {
    /* Inits Flags */
    sstParticleGroup.u32Flags = orxPARTICLEGROUP_KU32_STATIC_FLAG_READY;
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Failed to register storage link list.");
  }

  /* Done! */
  return eResult;
}

/** Exits from the ParticleGroup module
 */
void orxFASTCALL orxParticleGroup_Exit()
{
  /* Initialized? */
  if(sstParticleGroup.u32Flags & orxPARTICLEGROUP_KU32_STATIC_FLAG_READY)
  {
    /* Deletes body list */
    orxParticleGroup_DeleteAll();

    /* Unregisters structure type */
    orxStructure_Unregister(orxSTRUCTURE_ID_PARTICLEGROUP);

    /* Updates flags */
    sstParticleGroup.u32Flags &= ~orxPARTICLEGROUP_KU32_STATIC_FLAG_READY;
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Tried to exit particle group module when it wasn't initialized.");
  }

  return;
}

/** Creates a particle group
 * @param[in]   _pstOwner                              ParticleGroup's owner used for collision callbacks (usually an orxOBJECT)
 * @param[in]   _pstParticleGroupDef                   ParticleGroup definition
 * @return      Created orxBODY / orxNULL
 */
orxPARTICLEGROUP *orxFASTCALL orxParticleGroup_Create(const orxSTRUCTURE *_pstOwner, const orxPARTICLEGROUP_DEF *_pstParticleGroupDef)
{
  orxPARTICLEGROUP    *pstParticleGroup;
  orxOBJECT           *pstObject;

  /* Checks */
  orxASSERT(sstParticleGroup.u32Flags & orxPARTICLEGROUP_KU32_STATIC_FLAG_READY);
  orxASSERT(orxOBJECT(_pstOwner));
  orxASSERT(_pstParticleGroupDef != orxNULL);

  /* Gets owner object */
  pstObject = orxOBJECT(_pstOwner);

  /* Creates body */
  pstParticleGroup = orxPARTICLEGROUP(orxStructure_Create(orxSTRUCTURE_ID_PARTICLEGROUP));

  /* Valid? */
  if(pstParticleGroup != orxNULL)
  {
    /* Creates physics body */
    pstParticleGroup->pstData = orxPhysics_CreateParticleGroup(pstParticleGroup, _pstParticleGroupDef);

    /* Valid? */
    if(pstParticleGroup->pstData != orxNULL)
    {
      /* Stores owner */
      pstParticleGroup->pstOwner = _pstOwner;

      /* Stores its definition flags */
      pstParticleGroup->u32DefFlags = _pstParticleGroupDef->u32Flags;

      /* Updates flags */
      orxStructure_SetFlags(pstParticleGroup, orxPARTICLEGROUP_KU32_FLAG_HAS_DATA, orxPARTICLEGROUP_KU32_FLAG_NONE);

      /* Increases counter */
      orxStructure_IncreaseCounter(pstParticleGroup);
    }
    else
    {
      /* Logs message */
      orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Failed to create particle group.");

      /* Deletes allocated structure */
      orxStructure_Delete(pstParticleGroup);
      pstParticleGroup = orxNULL;
    }
  }

  /* Done! */
  return pstParticleGroup;
}

/** Creates a particle group from config
 * @param[in]   _pstOwner                     ParticleGroup's owner used for collision callbacks (usually an orxOBJECT)
 * @param[in]   _zConfigID                    ParticleGroup config ID
 * @return      Created orxGRAPHIC / orxNULL
 */
orxPARTICLEGROUP *orxFASTCALL orxParticleGroup_CreateFromConfig(const orxSTRUCTURE *_pstOwner, const orxSTRING _zConfigID)
{
  orxPARTICLEGROUP *pstResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstOwner);
  orxASSERT((_zConfigID != orxNULL) && (_zConfigID != orxSTRING_EMPTY));

  /* Pushes section */
  if((orxConfig_HasSection(_zConfigID) != orxFALSE)
  && (orxConfig_PushSection(_zConfigID) != orxSTATUS_FAILURE))
  {
    orxPARTICLEGROUP_DEF stParticleGroupDef;

    /* Clears body definition */
    orxMemory_Zero(&stParticleGroupDef, sizeof(orxPARTICLEGROUP_DEF));

    /* Inits it */
    stBodyDef.fInertia            = orxConfig_GetFloat(orxBODY_KZ_CONFIG_INERTIA);
    stBodyDef.fMass               = orxConfig_GetFloat(orxBODY_KZ_CONFIG_MASS);
    stBodyDef.fLinearDamping      = orxConfig_GetFloat(orxBODY_KZ_CONFIG_LINEAR_DAMPING);
    stBodyDef.fAngularDamping     = orxConfig_GetFloat(orxBODY_KZ_CONFIG_ANGULAR_DAMPING);
    stBodyDef.u32Flags            = orxPARTICLEGROUP_DEF_KU32_FLAG_2D;
    if(orxConfig_GetBool(orxBODY_KZ_CONFIG_FIXED_ROTATION) != orxFALSE)
    {
      stBodyDef.u32Flags |= orxBODY_DEF_KU32_FLAG_FIXED_ROTATION;
    }
    if((orxConfig_HasValue(orxBODY_KZ_CONFIG_ALLOW_SLEEP) == orxFALSE) || (orxConfig_GetBool(orxBODY_KZ_CONFIG_ALLOW_SLEEP) != orxFALSE))
    {
      stBodyDef.u32Flags |= orxBODY_DEF_KU32_FLAG_ALLOW_SLEEP;
    }
    if((orxConfig_HasValue(orxBODY_KZ_CONFIG_ALLOW_GROUND_SLIDING) == orxFALSE) || (orxConfig_GetBool(orxBODY_KZ_CONFIG_ALLOW_GROUND_SLIDING) != orxFALSE))
    {
      stBodyDef.u32Flags |= orxBODY_DEF_KU32_FLAG_CAN_SLIDE;
    }
    if((orxConfig_HasValue(orxBODY_KZ_CONFIG_ALLOW_MOVING) == orxFALSE) || (orxConfig_GetBool(orxBODY_KZ_CONFIG_ALLOW_MOVING) != orxFALSE))
    {
      stBodyDef.u32Flags |= orxBODY_DEF_KU32_FLAG_CAN_MOVE;
    }
    if(orxConfig_GetBool(orxBODY_KZ_CONFIG_HIGH_SPEED) != orxFALSE)
    {
      stBodyDef.u32Flags |= orxBODY_DEF_KU32_FLAG_HIGH_SPEED;
    }
    if(orxConfig_GetBool(orxBODY_KZ_CONFIG_DYNAMIC) != orxFALSE)
    {
      stBodyDef.u32Flags |= orxBODY_DEF_KU32_FLAG_DYNAMIC;
    }

    /* Creates body */
    pstResult = orxParticleGroup_Create(_pstOwner, &stParticleGroupDef);

    /* Valid? */
    if(pstResult != orxNULL)
    {
      orxU32 i, u32SlotCounter;

      /* Gets number of declared slots */
      u32SlotCounter = orxConfig_GetListCounter(orxBODY_KZ_CONFIG_PART_LIST);

      /* For all parts */
      for(i = 0; i < u32SlotCounter; i++)
      {
        const orxSTRING zPartName;

        /* Gets its name */
        zPartName = orxConfig_GetListString(orxBODY_KZ_CONFIG_PART_LIST, i);

        /* Valid? */
        if((zPartName != orxNULL) && (zPartName != orxSTRING_EMPTY))
        {
          /* Adds part */
          if(orxBody_AddPartFromConfig(pstResult, zPartName) == orxNULL)
          {
            /* Logs message */
            orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "[%s]: Couldn't add part <%s> for this body: too many parts or invalid part.", _zConfigID, orxConfig_GetListString(orxBODY_KZ_CONFIG_PART_LIST, i));
          }
        }
        else
        {
          break;
        }
      }
    }

    /* Pops previous section */
    orxConfig_PopSection();
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Cannot find config section named (%s).", _zConfigID);

    /* Updates result */
    pstResult = orxNULL;
  }

  /* Done! */
  return pstResult;
}

/** Deletes a particle group
 * @param[in]   _pstParticleGroup     ParticleGroup to delete
 */
orxSTATUS orxFASTCALL orxParticleGroup_Delete(orxPARTICLEGROUP *_pstParticleGroup)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstParticleGroup.u32Flags & orxPARTICLEGROUP_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstParticleGroup);

  /* Decreases counter */
  orxStructure_DecreaseCounter(_pstParticleGroup);

  /* Not referenced? */
  if(orxStructure_GetRefCounter(_pstParticleGroup) == 0)
  {
    /* Has data? */
    if(orxStructure_TestFlags(_pstParticleGroup, orxPARTICLEGROUP_KU32_FLAG_HAS_DATA))
    {
      /* Deletes physics body */
      orxPhysics_DeleteParticleGroup(_pstParticleGroup->pstData);
    }

    /* Deletes structure */
    orxStructure_Delete(_pstParticleGroup);
  }
  else
  {
    /* Referenced by others */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Tests flags against body definition ones
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _u32Flags       Flags to test
 * @return      orxTRUE / orxFALSE
 */
orxBOOL orxFASTCALL orxBody_TestDefFlags(const orxBODY *_pstBody, orxU32 _u32Flags)
{
  orxBOOL bResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Updates result */
  bResult = orxFLAG_TEST(_pstBody->u32DefFlags, _u32Flags);

  /* Done! */
  return bResult;
}

/** Tests all flags against body definition ones
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _u32Flags       Flags to test
 * @return      orxTRUE / orxFALSE
 */
orxBOOL orxFASTCALL orxBody_TestAllDefFlags(const orxBODY *_pstBody, orxU32 _u32Flags)
{
  orxBOOL bResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Updates result */
  bResult = orxFLAG_TEST_ALL(_pstBody->u32DefFlags, _u32Flags);

  /* Done! */
  return bResult;
}

/** Gets body definition flags
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _u32Mask        Mask to use for getting flags
 * @return      orxU32
 */
orxU32 orxFASTCALL orxBody_GetDefFlags(const orxBODY *_pstBody, orxU32 _u32Mask)
{
  orxU32 u32Result;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Updates result */
  u32Result = orxFLAG_GET(_pstBody->u32DefFlags, _u32Mask);

  /* Done! */
  return u32Result;
}

/** Gets a body owner
 * @param[in]   _pstBody        Concerned body
 * @return      orxSTRUCTURE / orxNULL
 */
orxSTRUCTURE *orxFASTCALL orxBody_GetOwner(const orxBODY *_pstBody)
{
  orxSTRUCTURE *pstResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Updates result */
  pstResult = orxSTRUCTURE(_pstBody->pstOwner);

  /* Done! */
  return pstResult;
}

/** Adds a part to body
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _pstBodyPartDef Body part definition
 * @return      orxBODY_PART / orxNULL
 */
orxBODY_PART *orxFASTCALL orxBody_AddPart(orxBODY *_pstBody, const orxBODY_PART_DEF *_pstBodyPartDef)
{
  orxBODY_PART_DEF *pstLocalBodyPartDef;
  orxBODY_PART     *pstResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);
  orxASSERT(_pstBodyPartDef != orxNULL);

  /* Creates a body part & body part def */
  pstLocalBodyPartDef = (orxBODY_PART_DEF *)orxBank_Allocate(sstBody.pstPartDefBank);
  pstResult = (orxBODY_PART *)orxBank_Allocate(sstBody.pstPartBank);

  /* Valid? */
  if((pstLocalBodyPartDef != orxNULL) && (pstResult != orxNULL))
  {
    orxPHYSICS_BODY_PART *pstBodyPart;

    /* Clears it */
    orxMemory_Zero(pstResult, sizeof(orxBODY_PART));

    /* Creates physics part */
    pstBodyPart = orxPhysics_CreatePart(_pstBody->pstData, pstResult, _pstBodyPartDef);

    /* Valid? */
    if(pstBodyPart != orxNULL)
    {
      /* Stores its data */
      pstResult->pstData = pstBodyPart;

      /* Copies def */
      orxMemory_Copy(pstLocalBodyPartDef, _pstBodyPartDef, sizeof(orxBODY_PART_DEF));

      /* Stores def */
      pstResult->pstDef = pstLocalBodyPartDef;

      /* Stores body */
      pstResult->pstBody = _pstBody;

      /* Links it */
      orxLinkList_AddEnd(&(_pstBody->stPartList), &(pstResult->stNode));
    }
    else
    {
      /* Logs message */
      orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Failed to create body part.");

      /* Deletes part */
      orxBank_Free(sstBody.pstPartBank, pstResult);

      /* Updates result */
      pstResult = orxNULL;
    }
  }
  else
  {
    /* Deletes banks */
    if(pstLocalBodyPartDef != orxNULL)
    {
      /* Deletes it */
      orxBank_Free(sstBody.pstPartDefBank, pstLocalBodyPartDef);
      pstLocalBodyPartDef = orxNULL;
    }
    if(pstResult != orxNULL)
    {
      /* Deletes it */
      orxBank_Free(sstBody.pstPartBank, pstResult);
      pstResult = orxNULL;
    }
  }

  /* Done! */
  return pstResult;
}

/** Adds a part to body from config
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _zConfigID      Body part config ID
 * @return      orxBODY_PART / orxNULL
 */
orxBODY_PART *orxFASTCALL orxBody_AddPartFromConfig(orxBODY *_pstBody, const orxSTRING _zConfigID)
{
  orxBODY_PART *pstResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);
  orxASSERT((_zConfigID != orxNULL) && (_zConfigID != orxSTRING_EMPTY));

  /* Pushes section */
  if((orxConfig_HasSection(_zConfigID) != orxFALSE)
  && (orxConfig_PushSection(_zConfigID) != orxSTATUS_FAILURE))
  {
    const orxSTRING   zBodyPartType;
    orxBODY_PART_DEF  stBodyPartDef;
    orxBOOL           bSuccess = orxTRUE;

    /* Clears body part definition */
    orxMemory_Zero(&stBodyPartDef, sizeof(orxBODY_PART_DEF));

    /* Gets body part type */
    zBodyPartType = orxConfig_GetString(orxBODY_KZ_CONFIG_TYPE);

    /* Inits it */
    stBodyPartDef.fFriction     = orxConfig_GetFloat(orxBODY_KZ_CONFIG_FRICTION);
    stBodyPartDef.fRestitution  = orxConfig_GetFloat(orxBODY_KZ_CONFIG_RESTITUTION);
    stBodyPartDef.fDensity      = (orxConfig_HasValue(orxBODY_KZ_CONFIG_DENSITY) != orxFALSE) ? orxConfig_GetFloat(orxBODY_KZ_CONFIG_DENSITY) : orxFLOAT_1;
    stBodyPartDef.u16SelfFlags  = orxBody_GetCollisionFlag(orxBODY_KZ_CONFIG_SELF_FLAGS);
    stBodyPartDef.u16CheckMask  = orxBody_GetCollisionFlag(orxBODY_KZ_CONFIG_CHECK_MASK);
    orxVector_Copy(&(stBodyPartDef.vScale), &(_pstBody->vScale));
    if(orxConfig_GetBool(orxBODY_KZ_CONFIG_SOLID) != orxFALSE)
    {
      stBodyPartDef.u32Flags |= orxBODY_PART_DEF_KU32_FLAG_SOLID;
    }
    /* Sphere? */
    if(orxString_ICompare(zBodyPartType, orxBODY_KZ_TYPE_SPHERE) == 0)
    {
      /* Updates sphere specific info */
      stBodyPartDef.u32Flags |= orxBODY_PART_DEF_KU32_FLAG_SPHERE;
      if(((orxConfig_HasValue(orxBODY_KZ_CONFIG_CENTER) == orxFALSE)
       && (orxConfig_HasValue(orxBODY_KZ_CONFIG_RADIUS) == orxFALSE))
      || (orxString_ICompare(orxConfig_GetString(orxBODY_KZ_CONFIG_RADIUS), orxBODY_KZ_FULL) == 0)
      || (orxString_ICompare(orxConfig_GetString(orxBODY_KZ_CONFIG_CENTER), orxBODY_KZ_FULL) == 0))
      {
        orxVECTOR vPivot, vSize;

        /* Gets object size & pivot */
        orxObject_GetSize(orxOBJECT(_pstBody->pstOwner), &vSize);
        orxObject_GetPivot(orxOBJECT(_pstBody->pstOwner), &vPivot);

        /* Gets radius size */
        orxVector_Mulf(&vSize, &vSize, orx2F(0.5f));

        /* Inits body part def */
        orxVector_Set(&(stBodyPartDef.stSphere.vCenter), vSize.fX - vPivot.fX, vSize.fY - vPivot.fY, vSize.fZ - vPivot.fZ);
        stBodyPartDef.stSphere.fRadius = orxMAX(vSize.fX, orxMAX(vSize.fY, vSize.fZ));
      }
      else
      {
        orxConfig_GetVector(orxBODY_KZ_CONFIG_CENTER, &(stBodyPartDef.stSphere.vCenter));
        stBodyPartDef.stSphere.fRadius = orxConfig_GetFloat(orxBODY_KZ_CONFIG_RADIUS);
      }
    }
    /* Box? */
    else if(orxString_ICompare(zBodyPartType, orxBODY_KZ_TYPE_BOX) == 0)
    {
      /* Updates box specific info */
      stBodyPartDef.u32Flags |= orxBODY_PART_DEF_KU32_FLAG_BOX;
      if(((orxConfig_HasValue(orxBODY_KZ_CONFIG_TOP_LEFT) == orxFALSE)
       && (orxConfig_HasValue(orxBODY_KZ_CONFIG_BOTTOM_RIGHT) == orxFALSE))
      || (orxString_ICompare(orxConfig_GetString(orxBODY_KZ_CONFIG_TOP_LEFT), orxBODY_KZ_FULL) == 0)
      || (orxString_ICompare(orxConfig_GetString(orxBODY_KZ_CONFIG_BOTTOM_RIGHT), orxBODY_KZ_FULL) == 0))
      {
        orxVECTOR vPivot, vSize;

        /* Gets object size & pivot */
        orxObject_GetSize(orxOBJECT(_pstBody->pstOwner), &vSize);
        orxObject_GetPivot(orxOBJECT(_pstBody->pstOwner), &vPivot);

        /* Inits body part def */
        orxVector_Set(&(stBodyPartDef.stAABox.stBox.vTL), -vPivot.fX, -vPivot.fY, -vPivot.fZ);
        orxVector_Set(&(stBodyPartDef.stAABox.stBox.vBR), vSize.fX - vPivot.fX, vSize.fY - vPivot.fY, vSize.fZ - vPivot.fZ);
      }
      else
      {
        orxConfig_GetVector(orxBODY_KZ_CONFIG_TOP_LEFT, &(stBodyPartDef.stAABox.stBox.vTL));
        orxConfig_GetVector(orxBODY_KZ_CONFIG_BOTTOM_RIGHT, &(stBodyPartDef.stAABox.stBox.vBR));
      }
    }
    /* Mesh */
    else if(orxString_ICompare(zBodyPartType, orxBODY_KZ_TYPE_MESH) == 0)
    {
      /* Updates mesh specific info */
      stBodyPartDef.u32Flags |= orxBODY_PART_DEF_KU32_FLAG_MESH;
      if((orxConfig_HasValue(orxBODY_KZ_CONFIG_VERTEX_LIST) != orxFALSE)
      && ((stBodyPartDef.stMesh.u32VertexCounter = orxConfig_GetListCounter(orxBODY_KZ_CONFIG_VERTEX_LIST)) >= 3))
      {
        orxU32 i;

        /* Too many defined vertices? */
        if(stBodyPartDef.stMesh.u32VertexCounter > orxBODY_PART_DEF_KU32_MESH_VERTEX_NUMBER)
        {
          /* Logs message */
          orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Too many vertices in the list: %d. The maximum allowed is: %d. Using the first %d ones for the shape <%s>", stBodyPartDef.stMesh.u32VertexCounter, orxBODY_PART_DEF_KU32_MESH_VERTEX_NUMBER, orxBODY_PART_DEF_KU32_MESH_VERTEX_NUMBER, _zConfigID);

          /* Updates vertices number */
          stBodyPartDef.stMesh.u32VertexCounter = orxBODY_PART_DEF_KU32_MESH_VERTEX_NUMBER;
        }

        /* For all defined vertices */
        for(i = 0; i < stBodyPartDef.stMesh.u32VertexCounter; i++)
        {
          /* Gets its vector */
          orxConfig_GetListVector(orxBODY_KZ_CONFIG_VERTEX_LIST, i, &(stBodyPartDef.stMesh.avVertices[i]));
        }
      }
      else
      {
        /* Logs message */
        orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Vertex list for creating mesh body <%s> is invalid (missing or less than 3 vertices).", _zConfigID);

        /* Updates status */
        bSuccess = orxFALSE;
      }
    }
    /* Unknown */
    else
    {
      /* Logs message */
      orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "<%s> isn't a valid type for a body part.", zBodyPartType);

      /* Updates status */
      bSuccess = orxFALSE;
    }

    /* Valid? */
    if(bSuccess != orxFALSE)
    {
      /* Adds body part */
      pstResult = orxBody_AddPart(_pstBody, &stBodyPartDef);

      /* Valid? */
      if(pstResult != orxNULL)
      {
        /* Stores its reference */
        pstResult->zReference = orxConfig_GetCurrentSection();

        /* Protects it */
        orxConfig_ProtectSection(pstResult->zReference, orxTRUE);
      }
    }
    else
    {
      /* Logs message */
      orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Couldn't create part (%s)", _zConfigID);

      /* Updates result */
      pstResult = orxNULL;
    }

    /* Pops previous section */
    orxConfig_PopSection();
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Couldn't find config section named (%s)", _zConfigID);

    /* Updates result */
    pstResult = orxNULL;
  }

  /* Done! */
  return pstResult;
}

/** Removes a part using its config ID
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _zConfigID      Config ID of the part to remove
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_RemovePartFromConfig(orxBODY *_pstBody, const orxSTRING _zConfigID)
{
  orxU32        u32ID;
  orxBODY_PART *pstPart;
  orxSTATUS     eResult = orxSTATUS_FAILURE;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Gets part ID */
  u32ID = orxString_ToCRC(_zConfigID);

  /* For all parts */
  for(pstPart = orxBody_GetNextPart(_pstBody, orxNULL);
      pstPart != orxNULL;
      pstPart = orxBody_GetNextPart(_pstBody, pstPart))
  {
    /* Found? */
    if(orxString_ToCRC(orxBody_GetPartName(pstPart)) == u32ID)
    {
      /* Removes it */
      eResult = orxBody_RemovePart(pstPart);

      break;
    }
  }

  /* Done! */
  return eResult;
}

/** Gets next body part
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _pstBodyPart    Current body part (orxNULL to get the first one)
 * @return      orxBODY_PART / orxNULL
 */
orxBODY_PART *orxFASTCALL orxBody_GetNextPart(const orxBODY *_pstBody, const orxBODY_PART *_pstBodyPart)
{
  orxBODY_PART *pstResult = orxNULL;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* First one? */
  if(_pstBodyPart == orxNULL)
  {
    /* Updates result */
    pstResult = (orxBODY_PART *)orxLinkList_GetFirst(&(_pstBody->stPartList));
  }
  else
  {
    /* Updates result */
    pstResult = (orxBODY_PART *)orxLinkList_GetNext(&(_pstBodyPart->stNode));
  }

  /* Done! */
  return pstResult;
}

/** Gets a body part name
 * @param[in]   _pstBodyPart    Concerned body part
 * @return      orxSTRING / orxNULL
 */
const orxSTRING orxFASTCALL orxBody_GetPartName(const orxBODY_PART *_pstBodyPart)
{
  const orxSTRING zResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstBodyPart != orxNULL);

  /* Updates result */
  zResult = _pstBodyPart->zReference;

  /* Done! */
  return zResult;
}

/** Gets a body part definition (matching current part status)
 * @param[in]   _pstBodyPart    Concerned body part
 * @return      orxBODY_PART_DEF / orxNULL
 */
const orxBODY_PART_DEF *orxFASTCALL orxBody_GetPartDef(const orxBODY_PART *_pstBodyPart)
{
  orxBODY_PART_DEF *pstResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstBodyPart != orxNULL);

  /* Updates result */
  pstResult = (orxBODY_PART_DEF *)_pstBodyPart->pstDef;

  /* Updates its information */
  orxVector_Copy(&(pstResult->vScale), &(_pstBodyPart->pstBody->vScale));
  pstResult->u16CheckMask = orxBody_GetPartCheckMask(_pstBodyPart);
  pstResult->u16SelfFlags = orxBody_GetPartSelfFlags(_pstBodyPart);
  if(orxBody_IsPartSolid(_pstBodyPart) != orxFALSE)
  {
    orxFLAG_SET(pstResult->u32Flags, orxBODY_PART_DEF_KU32_FLAG_SOLID, orxBODY_PART_DEF_KU32_FLAG_NONE);
  }
  else
  {
    orxFLAG_SET(pstResult->u32Flags, orxBODY_PART_DEF_KU32_FLAG_NONE, orxBODY_PART_DEF_KU32_FLAG_SOLID);
  }

  /* Done! */
  return pstResult;
}

/** Gets a body part body (ie. owner)
 * @param[in]   _pstBodyPart    Concerned body part
 * @return      orxBODY / orxNULL
 */
orxBODY *orxFASTCALL orxBody_GetPartBody(const orxBODY_PART *_pstBodyPart)
{
  orxBODY *pstResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstBodyPart != orxNULL);

  /* Updates result */
  pstResult = (orxBODY *)_pstBodyPart->pstBody;

  /* Done! */
  return pstResult;
}

/** Removes a body part
 * @param[in]   _pstBodyPart    Concerned body part
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_RemovePart(orxBODY_PART *_pstBodyPart)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);

  /* Valid? */
  if(_pstBodyPart != orxNULL)
  {
    /* Unlinks it */
    orxLinkList_Remove(&(_pstBodyPart->stNode));

    /* Deletes its data */
    orxPhysics_DeletePart(_pstBodyPart->pstData);

    /* Has reference? */
    if(_pstBodyPart->zReference != orxNULL)
    {
      /* Unprotects it */
      orxConfig_ProtectSection(_pstBodyPart->zReference, orxFALSE);
    }

    /* Frees part def */
    orxBank_Free(sstBody.pstPartDefBank, _pstBodyPart->pstDef);

    /* Frees part */
    orxBank_Free(sstBody.pstPartBank, _pstBodyPart);

    /* Updates result */
    eResult = orxSTATUS_SUCCESS;
  }
  else
  {
    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Adds a joint to link two bodies together
 * @param[in]   _pstSrcBody       Concerned source body
 * @param[in]   _pstDstBody       Concerned destination body
 * @param[in]   _pstBodyJointDef  Body joint definition
 * @return      orxBODY_JOINT / orxNULL
 */
orxBODY_JOINT *orxFASTCALL orxBody_AddJoint(orxBODY *_pstSrcBody, orxBODY *_pstDstBody, const orxBODY_JOINT_DEF *_pstBodyJointDef)
{
  orxBODY_JOINT *pstResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSrcBody);
  orxSTRUCTURE_ASSERT(_pstDstBody);
  orxASSERT(_pstBodyJointDef != orxNULL);

  /* Creates a body part */
  pstResult = (orxBODY_JOINT *)orxBank_Allocate(sstBody.pstJointBank);

  /* Valid? */
  if(pstResult != orxNULL)
  {
    orxPHYSICS_BODY_JOINT *pstBodyJoint;

    /* Clears it */
    orxMemory_Zero(pstResult, sizeof(orxBODY_JOINT));

    /* Creates physics joint */
    pstBodyJoint = orxPhysics_CreateJoint(_pstSrcBody->pstData, _pstDstBody->pstData, pstResult, _pstBodyJointDef);

    /* Valid? */
    if(pstBodyJoint != orxNULL)
    {
      /* Stores its data */
      pstResult->pstData = pstBodyJoint;

      /* Links it */
      orxLinkList_AddEnd(&(_pstSrcBody->stSrcJointList), &(pstResult->stSrcNode));
      orxLinkList_AddEnd(&(_pstDstBody->stDstJointList), &(pstResult->stDstNode));
    }
    else
    {
      /* Logs message */
      orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Failed to create body joint.");

      /* Deletes part */
      orxBank_Free(sstBody.pstJointBank, pstResult);

      /* Updates result */
      pstResult = orxNULL;
    }
  }

  /* Done! */
  return pstResult;
}

/** Adds a joint from config to link two bodies together
 * @param[in]   _pstSrcBody     Concerned source body
 * @param[in]   _pstDstBody     Concerned destination body
 * @param[in]   _zConfigID      Body joint config ID
 * @return      orxBODY_JOINT / orxNULL
 */
orxBODY_JOINT *orxFASTCALL orxBody_AddJointFromConfig(orxBODY *_pstSrcBody, orxBODY *_pstDstBody, const orxSTRING _zConfigID)
{
  orxBODY_JOINT *pstResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSrcBody);
  orxSTRUCTURE_ASSERT(_pstDstBody);
  orxASSERT(_zConfigID != orxNULL);

  /* Pushes section */
  if((_zConfigID != orxSTRING_EMPTY)
  && (orxConfig_HasSection(_zConfigID) != orxFALSE)
  && (orxConfig_PushSection(_zConfigID) != orxSTATUS_FAILURE))
  {
    const orxSTRING   zBodyJointType;
    orxBODY_JOINT_DEF stBodyJointDef;
    orxBOOL           bSuccess = orxTRUE;

    /* Clears body part definition */
    orxMemory_Zero(&stBodyJointDef, sizeof(orxBODY_JOINT_DEF));

    /* Gets body joint type */
    zBodyJointType = orxConfig_GetString(orxBODY_KZ_CONFIG_TYPE);

    /* Inits it */
    orxVector_Copy(&(stBodyJointDef.vSrcScale), &(_pstSrcBody->vScale));
    orxVector_Copy(&(stBodyJointDef.vDstScale), &(_pstDstBody->vScale));
    orxConfig_GetVector(orxBODY_KZ_CONFIG_PARENT_ANCHOR, &(stBodyJointDef.vSrcAnchor));
    orxConfig_GetVector(orxBODY_KZ_CONFIG_CHILD_ANCHOR, &(stBodyJointDef.vDstAnchor));
    if(orxConfig_GetBool(orxBODY_KZ_CONFIG_COLLIDE) != orxFALSE)
    {
      stBodyJointDef.u32Flags = orxBODY_JOINT_DEF_KU32_FLAG_COLLIDE;
    }

    /* Revolute? */
    if(orxString_ICompare(zBodyJointType, orxBODY_KZ_TYPE_REVOLUTE) == 0)
    {
      /* Stores type */
      stBodyJointDef.u32Flags                    |= orxBODY_JOINT_DEF_KU32_FLAG_REVOLUTE;

      /* Stores default rotation */
      stBodyJointDef.stRevolute.fDefaultRotation  = orxConfig_HasValue(orxBODY_KZ_CONFIG_ROTATION) ? orxMATH_KF_DEG_TO_RAD * orxConfig_GetFloat(orxBODY_KZ_CONFIG_ROTATION) : orxObject_GetWorldRotation(orxOBJECT(orxBody_GetOwner(_pstDstBody))) - orxObject_GetWorldRotation(orxOBJECT(orxBody_GetOwner(_pstSrcBody)));

      /* Has rotation limits? */
      if((orxConfig_HasValue(orxBODY_KZ_CONFIG_MIN_ROTATION) != orxFALSE)
      && (orxConfig_HasValue(orxBODY_KZ_CONFIG_MAX_ROTATION) != orxFALSE))
      {
        /* Updates status */
        stBodyJointDef.u32Flags                  |= orxBODY_JOINT_DEF_KU32_FLAG_ROTATION_LIMIT;

        /* Stores them */
        stBodyJointDef.stRevolute.fMinRotation    = orxMATH_KF_DEG_TO_RAD * orxConfig_GetFloat(orxBODY_KZ_CONFIG_MIN_ROTATION);
        stBodyJointDef.stRevolute.fMaxRotation    = orxMATH_KF_DEG_TO_RAD * orxConfig_GetFloat(orxBODY_KZ_CONFIG_MAX_ROTATION);
      }

      /* Is a motor? */
      if((orxConfig_HasValue(orxBODY_KZ_CONFIG_MOTOR_SPEED) != orxFALSE)
      && (orxConfig_HasValue(orxBODY_KZ_CONFIG_MAX_MOTOR_TORQUE) != orxFALSE))
      {
        /* Stores motor values */
        stBodyJointDef.stRevolute.fMotorSpeed     = orxMATH_KF_DEG_TO_RAD * orxConfig_GetFloat(orxBODY_KZ_CONFIG_MOTOR_SPEED);
        stBodyJointDef.stRevolute.fMaxMotorTorque = orxConfig_GetFloat(orxBODY_KZ_CONFIG_MAX_MOTOR_TORQUE);

        /* Updates status */
        stBodyJointDef.u32Flags                  |= orxBODY_JOINT_DEF_KU32_FLAG_MOTOR;
      }
    }
    /* Prismatic? */
    else if(orxString_ICompare(zBodyJointType, orxBODY_KZ_TYPE_PRISMATIC) == 0)
    {
      /* Stores type */
      stBodyJointDef.u32Flags                    |= orxBODY_JOINT_DEF_KU32_FLAG_PRISMATIC;

      /* Stores default rotation */
      stBodyJointDef.stPrismatic.fDefaultRotation = orxConfig_HasValue(orxBODY_KZ_CONFIG_ROTATION) ? orxMATH_KF_DEG_TO_RAD * orxConfig_GetFloat(orxBODY_KZ_CONFIG_ROTATION) : orxObject_GetWorldRotation(orxOBJECT(orxBody_GetOwner(_pstDstBody))) - orxObject_GetWorldRotation(orxOBJECT(orxBody_GetOwner(_pstSrcBody)));

      /* Stores translation axis */
      orxConfig_GetVector(orxBODY_KZ_CONFIG_TRANSLATION_AXIS, &(stBodyJointDef.stPrismatic.vTranslationAxis));

      /* Has translation limits? */
      if((orxConfig_HasValue(orxBODY_KZ_CONFIG_MIN_TRANSLATION) != orxFALSE)
      && (orxConfig_HasValue(orxBODY_KZ_CONFIG_MAX_TRANSLATION) != orxFALSE))
      {
        /* Updates status */
        stBodyJointDef.u32Flags |= orxBODY_JOINT_DEF_KU32_FLAG_TRANSLATION_LIMIT;

        /* Stores them */
        stBodyJointDef.stPrismatic.fMinTranslation  = orxConfig_GetFloat(orxBODY_KZ_CONFIG_MIN_TRANSLATION);
        stBodyJointDef.stPrismatic.fMaxTranslation  = orxConfig_GetFloat(orxBODY_KZ_CONFIG_MAX_TRANSLATION);
      }

      /* Is a motor? */
      if((orxConfig_HasValue(orxBODY_KZ_CONFIG_MOTOR_SPEED) != orxFALSE)
      && (orxConfig_HasValue(orxBODY_KZ_CONFIG_MAX_MOTOR_FORCE) != orxFALSE))
      {
        /* Stores motor values */
        stBodyJointDef.stPrismatic.fMotorSpeed      = orxConfig_GetFloat(orxBODY_KZ_CONFIG_MOTOR_SPEED);
        stBodyJointDef.stPrismatic.fMaxMotorForce   = orxConfig_GetFloat(orxBODY_KZ_CONFIG_MAX_MOTOR_FORCE);

        /* Updates status */
        stBodyJointDef.u32Flags                    |= orxBODY_JOINT_DEF_KU32_FLAG_MOTOR;
      }
    }
    /* Spring? */
    else if(orxString_ICompare(zBodyJointType, orxBODY_KZ_TYPE_SPRING) == 0)
    {
      orxVECTOR vSrcPos, vDstPos;

      /* Stores type */
      stBodyJointDef.u32Flags |= orxBODY_JOINT_DEF_KU32_FLAG_SPRING;

      /* Stores length */
      stBodyJointDef.stSpring.fLength     = orxConfig_HasValue(orxBODY_KZ_CONFIG_LENGTH) ? orxConfig_GetFloat(orxBODY_KZ_CONFIG_LENGTH) : orxVector_GetDistance(orxObject_GetWorldPosition(orxOBJECT(orxBody_GetOwner(_pstSrcBody)), &vSrcPos), orxObject_GetWorldPosition(orxOBJECT(orxBody_GetOwner(_pstDstBody)), &vDstPos));;

      /* Stores frequency */
      stBodyJointDef.stSpring.fFrequency  = orxConfig_GetFloat(orxBODY_KZ_CONFIG_FREQUENCY);

      /* Stores damping */
      stBodyJointDef.stSpring.fDamping    = orxConfig_GetFloat(orxBODY_KZ_CONFIG_DAMPING);
    }
    /* Rope? */
    else if(orxString_ICompare(zBodyJointType, orxBODY_KZ_TYPE_ROPE) == 0)
    {
      orxVECTOR vSrcPos, vDstPos;

      /* Stores type */
      stBodyJointDef.u32Flags |= orxBODY_JOINT_DEF_KU32_FLAG_ROPE;

      /* Stores length */
      stBodyJointDef.stRope.fLength = orxConfig_HasValue(orxBODY_KZ_CONFIG_LENGTH) ? orxConfig_GetFloat(orxBODY_KZ_CONFIG_LENGTH) : orxVector_GetDistance(orxObject_GetWorldPosition(orxOBJECT(orxBody_GetOwner(_pstSrcBody)), &vSrcPos), orxObject_GetWorldPosition(orxOBJECT(orxBody_GetOwner(_pstDstBody)), &vDstPos));;
    }
    /* Pulley? */
    else if(orxString_ICompare(zBodyJointType, orxBODY_KZ_TYPE_PULLEY) == 0)
    {
      orxVECTOR vPos;

      /* Stores type */
      stBodyJointDef.u32Flags |= orxBODY_JOINT_DEF_KU32_FLAG_PULLEY;

      /* Stores ratio */
      stBodyJointDef.stPulley.fLengthRatio  = (orxConfig_HasValue(orxBODY_KZ_CONFIG_LENGTH_RATIO) != orxFALSE) ? orxConfig_GetFloat(orxBODY_KZ_CONFIG_LENGTH_RATIO) : orxFLOAT_1;

      /* Stores anchors */
      orxConfig_GetVector(orxBODY_KZ_CONFIG_PARENT_GROUND_ANCHOR, &(stBodyJointDef.stPulley.vSrcGroundAnchor));
      orxConfig_GetVector(orxBODY_KZ_CONFIG_CHILD_GROUND_ANCHOR, &(stBodyJointDef.stPulley.vDstGroundAnchor));

      /* Stores lengths */
      stBodyJointDef.stPulley.fSrcLength    = orxConfig_HasValue(orxBODY_KZ_CONFIG_PARENT_LENGTH) ? orxConfig_GetFloat(orxBODY_KZ_CONFIG_PARENT_LENGTH) : orxVector_GetDistance(orxObject_GetWorldPosition(orxOBJECT(orxBody_GetOwner(_pstSrcBody)), &vPos), &(stBodyJointDef.stPulley.vSrcGroundAnchor));
      stBodyJointDef.stPulley.fDstLength    = orxConfig_HasValue(orxBODY_KZ_CONFIG_CHILD_LENGTH) ? orxConfig_GetFloat(orxBODY_KZ_CONFIG_CHILD_LENGTH) : orxVector_GetDistance(orxObject_GetWorldPosition(orxOBJECT(orxBody_GetOwner(_pstDstBody)), &vPos), &(stBodyJointDef.stPulley.vDstGroundAnchor));
      stBodyJointDef.stPulley.fMaxSrcLength = orxConfig_HasValue(orxBODY_KZ_CONFIG_MAX_PARENT_LENGTH) ? orxConfig_GetFloat(orxBODY_KZ_CONFIG_MAX_PARENT_LENGTH) : stBodyJointDef.stPulley.fSrcLength + stBodyJointDef.stPulley.fLengthRatio * stBodyJointDef.stPulley.fDstLength;
      stBodyJointDef.stPulley.fMaxDstLength = orxConfig_HasValue(orxBODY_KZ_CONFIG_MAX_CHILD_LENGTH) ? orxConfig_GetFloat(orxBODY_KZ_CONFIG_MAX_CHILD_LENGTH) : stBodyJointDef.stPulley.fSrcLength + stBodyJointDef.stPulley.fDstLength;
    }
    /* Suspension? */
    else if(orxString_ICompare(zBodyJointType, orxBODY_KZ_TYPE_SUSPENSION) == 0)
    {
      /* Stores type */
      stBodyJointDef.u32Flags |= orxBODY_JOINT_DEF_KU32_FLAG_SUSPENSION;

      /* Stores translation axis */
      orxConfig_GetVector(orxBODY_KZ_CONFIG_TRANSLATION_AXIS, &(stBodyJointDef.stSuspension.vTranslationAxis));

      /* Has translation limits? */
      if((orxConfig_HasValue(orxBODY_KZ_CONFIG_MIN_TRANSLATION) != orxFALSE)
      && (orxConfig_HasValue(orxBODY_KZ_CONFIG_MAX_TRANSLATION) != orxFALSE))
      {
        /* Updates status */
        stBodyJointDef.u32Flags |= orxBODY_JOINT_DEF_KU32_FLAG_TRANSLATION_LIMIT;

        /* Stores them */
        stBodyJointDef.stSuspension.fMinTranslation  = orxConfig_GetFloat(orxBODY_KZ_CONFIG_MIN_TRANSLATION);
        stBodyJointDef.stSuspension.fMaxTranslation  = orxConfig_GetFloat(orxBODY_KZ_CONFIG_MAX_TRANSLATION);
      }

      /* Is a motor? */
      if((orxConfig_HasValue(orxBODY_KZ_CONFIG_MOTOR_SPEED) != orxFALSE)
      && (orxConfig_HasValue(orxBODY_KZ_CONFIG_MAX_MOTOR_FORCE) != orxFALSE))
      {
        /* Stores motor values */
        stBodyJointDef.stSuspension.fMotorSpeed     = orxConfig_GetFloat(orxBODY_KZ_CONFIG_MOTOR_SPEED);
        stBodyJointDef.stSuspension.fMaxMotorForce  = orxConfig_GetFloat(orxBODY_KZ_CONFIG_MAX_MOTOR_FORCE);

        /* Updates status */
        stBodyJointDef.u32Flags                    |= orxBODY_JOINT_DEF_KU32_FLAG_MOTOR;
      }
    }
    /* Weld? */
    else if(orxString_ICompare(zBodyJointType, orxBODY_KZ_TYPE_WELD) == 0)
    {
      /* Stores type */
      stBodyJointDef.u32Flags                |= orxBODY_JOINT_DEF_KU32_FLAG_WELD;

      /* Stores default rotation */
      stBodyJointDef.stWeld.fDefaultRotation  = orxConfig_HasValue(orxBODY_KZ_CONFIG_ROTATION) ? orxMATH_KF_DEG_TO_RAD * orxConfig_GetFloat(orxBODY_KZ_CONFIG_ROTATION) : orxObject_GetWorldRotation(orxOBJECT(orxBody_GetOwner(_pstDstBody))) - orxObject_GetWorldRotation(orxOBJECT(orxBody_GetOwner(_pstSrcBody)));
    }
    /* Friction? */
    else if(orxString_ICompare(zBodyJointType, orxBODY_KZ_TYPE_FRICTION) == 0)
    {
      /* Stores type */
      stBodyJointDef.u32Flags |= orxBODY_JOINT_DEF_KU32_FLAG_FRICTION;

      /* Stores max force & torque values */
      stBodyJointDef.stFriction.fMaxForce   = orxConfig_GetFloat(orxBODY_KZ_CONFIG_MAX_FORCE);
      stBodyJointDef.stFriction.fMaxTorque  = orxConfig_GetFloat(orxBODY_KZ_CONFIG_MAX_TORQUE);
    }
    /* Gear? */
    else if(orxString_ICompare(zBodyJointType, orxBODY_KZ_TYPE_GEAR) == 0)
    {
      /* Stores type */
      stBodyJointDef.u32Flags |= orxBODY_JOINT_DEF_KU32_FLAG_GEAR;

      /* Stores joint names */
      stBodyJointDef.stGear.zSrcJointName = orxConfig_GetString(orxBODY_KZ_CONFIG_PARENT_JOINT_NAME);
      stBodyJointDef.stGear.zDstJointName = orxConfig_GetString(orxBODY_KZ_CONFIG_CHILD_JOINT_NAME);

      /* Stores joint ratio */
      stBodyJointDef.stGear.fJointRatio   = (orxConfig_HasValue(orxBODY_KZ_CONFIG_JOINT_RATIO) != orxFALSE) ? orxConfig_GetFloat(orxBODY_KZ_CONFIG_JOINT_RATIO) : orxFLOAT_1;
    }
    /* Unknown */
    else
    {
      /* Logs message */
      orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Can't create body joint: <%s> isn't a valid type.", zBodyJointType);

      /* Updates result */
      pstResult = orxNULL;

      /* Updates status */
      bSuccess = orxFALSE;
    }

    /* Valid? */
    if(bSuccess != orxFALSE)
    {
      /* Adds body part */
      pstResult = orxBody_AddJoint(_pstSrcBody, _pstDstBody, &stBodyJointDef);

      /* Valid? */
      if(pstResult != orxNULL)
      {
        /* Stores its reference */
        pstResult->zReference = orxConfig_GetCurrentSection();

        /* Protects it */
        orxConfig_ProtectSection(pstResult->zReference, orxTRUE);
      }
    }

    /* Pops previous section */
    orxConfig_PopSection();
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Couldn't find config section named (%s)", _zConfigID);

    /* Updates result */
    pstResult = orxNULL;
  }

  /* Done! */
  return pstResult;
}

/** Gets next body joint
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _pstBodyJoint   Current body joint (orxNULL to get the first one)
 * @return      orxBODY_JOINT / orxNULL
 */
orxBODY_JOINT *orxFASTCALL orxBody_GetNextJoint(const orxBODY *_pstBody, const orxBODY_JOINT *_pstBodyJoint)
{
  orxBODY_JOINT *pstResult = orxNULL;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* First one? */
  if(_pstBodyJoint == orxNULL)
  {
    /* Gets first source joint */
    pstResult = orxBODY_GET_FIRST_JOINT_FROM_SRC_LIST(_pstBody);

    /* Invalid? */
    if(pstResult == orxNULL)
    {
      /* Gets first destination joint */
      pstResult = orxBODY_GET_FIRST_JOINT_FROM_DST_LIST(_pstBody);
    }
  }
  else
  {
    /* Is a source joint? */
    if(_pstBodyJoint->stSrcNode.pstList == &(_pstBody->stSrcJointList))
    {
      /* Is last? */
      if(&(_pstBodyJoint->stSrcNode) == orxLinkList_GetLast(&(_pstBody->stSrcJointList)))
      {
        /* Gets first destination joint */
        pstResult = orxBODY_GET_FIRST_JOINT_FROM_DST_LIST(_pstBody);
      }
      else
      {
        /* Gets next source joint */
        pstResult = orxBODY_GET_NEXT_JOINT_FROM_SRC_LIST(_pstBodyJoint);
      }
    }
    else
    {
      /* Checks */
      orxASSERT(_pstBodyJoint->stDstNode.pstList == &(_pstBody->stDstJointList));

      /* Gets next destination joint */
      pstResult = orxBODY_GET_NEXT_JOINT_FROM_DST_LIST(_pstBodyJoint);
    }
  }

  /* Done! */
  return pstResult;
}

/** Gets a body joint name
 * @param[in]   _pstBodyJoint   Concerned body joint
 * @return      orxSTRING / orxNULL
 */
const orxSTRING orxFASTCALL orxBody_GetJointName(const orxBODY_JOINT *_pstBodyJoint)
{
  const orxSTRING zResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstBodyJoint != orxNULL);

  /* Updates result */
  zResult = _pstBodyJoint->zReference;

  /* Done! */
  return zResult;
}

/** Removes a body joint
 * @param[in]   _pstBodyJoint   Concerned body joint
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_RemoveJoint(orxBODY_JOINT *_pstBodyJoint)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);

  /* Valid? */
  if(_pstBodyJoint != orxNULL)
  {
    /* Unlinks it */
    orxLinkList_Remove(&(_pstBodyJoint->stSrcNode));
    orxLinkList_Remove(&(_pstBodyJoint->stDstNode));

    /* Deletes its data */
    orxPhysics_DeleteJoint(_pstBodyJoint->pstData);

    /* Has reference? */
    if(_pstBodyJoint->zReference != orxNULL)
    {
      /* Unprotects it */
      orxConfig_ProtectSection(_pstBodyJoint->zReference, orxFALSE);
    }

    /* Frees joint */
    orxBank_Free(sstBody.pstJointBank, _pstBodyJoint);

    /* Updates result */
    eResult = orxSTATUS_SUCCESS;
  }
  else
  {
    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Enables a (revolute) body joint motor
 * @param[in]   _pstBodyJoint   Concerned body joint
 * @param[in]   _bEnable        Enable / Disable
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_EnableMotor(orxBODY_JOINT *_pstBodyJoint, orxBOOL _bEnable)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);

  /* Valid? */
  if(_pstBodyJoint != orxNULL)
  {
    orxPhysics_EnableMotor(_pstBodyJoint->pstData, _bEnable);

    /* Updates result */
    eResult = orxSTATUS_SUCCESS;
  }
  else
  {
    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Sets a (revolute) body joint motor speed
 * @param[in]   _pstBodyJoint   Concerned body joint
 * @param[in]   _fSpeed         Speed
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_SetJointMotorSpeed(orxBODY_JOINT *_pstBodyJoint, orxFLOAT _fSpeed)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);

  /* Valid? */
  if(_pstBodyJoint != orxNULL)
  {
    orxPhysics_SetJointMotorSpeed(_pstBodyJoint->pstData, _fSpeed);

    /* Updates result */
    eResult = orxSTATUS_SUCCESS;
  }
  else
  {
    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Sets a (revolute) body joint maximum motor torque
 * @param[in]   _pstBodyJoint   Concerned body joint
 * @param[in]   _fMaxTorque     Maximum motor torque
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_SetJointMaxMotorTorque(orxBODY_JOINT *_pstBodyJoint, orxFLOAT _fMaxTorque)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);

  /* Valid? */
  if(_pstBodyJoint != orxNULL)
  {
    orxPhysics_SetJointMaxMotorTorque(_pstBodyJoint->pstData, _fMaxTorque);

    /* Updates result */
    eResult = orxSTATUS_SUCCESS;
  }
  else
  {
    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Gets the reaction force on the attached body at the joint anchor
 * @param[in]   _pstBodyJoint                         Concerned body joint
 * @param[out]  _pvForce                              Reaction force
 * @return      Reaction force in Newtons
 */
orxVECTOR *orxFASTCALL orxBody_GetJointReactionForce(const orxBODY_JOINT *_pstBodyJoint, orxVECTOR *_pvForce)
{
  orxVECTOR *pvResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxASSERT(_pvForce != orxNULL);

  /* Valid? */
  if(_pstBodyJoint != orxNULL)
  {
    /* Updates result */
    pvResult = orxPhysics_GetJointReactionForce(_pstBodyJoint->pstData, _pvForce);
  }
  else
  {
    /* Updates result */
    pvResult = orxNULL;
  }

  /* Done! */
  return pvResult;
}

/** Gets the reaction torque on the attached body
 * @param[in]   _pstBodyJoint                         Concerned body joint
 * @return      Reaction torque
 */
orxFLOAT orxFASTCALL orxBody_GetJointReactionTorque(const orxBODY_JOINT *_pstBodyJoint)
{
  orxFLOAT fResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);

  /* Valid? */
  if(_pstBodyJoint != orxNULL)
  {
    /* Updates result */
    fResult = orxPhysics_GetJointReactionTorque(_pstBodyJoint->pstData);
  }
  else
  {
    /* Updates result */
    fResult = orxFLOAT_0;
  }

  /* Done! */
  return fResult;
}

/** Sets a body position
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _pvPosition     Position to set
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_SetPosition(orxBODY *_pstBody, const orxVECTOR *_pvPosition)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);
  orxASSERT(_pvPosition != orxNULL);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Updates its position */
    eResult = orxPhysics_SetPosition(_pstBody->pstData, _pvPosition);
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Structure does not have data.");

    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Sets a body rotation
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _fRotation      Rotation to set (radians)
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_SetRotation(orxBODY *_pstBody, orxFLOAT _fRotation)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Updates its position */
    eResult = orxPhysics_SetRotation(_pstBody->pstData, _fRotation);
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Structure does not have data.");

    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Sets a body scale
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _pvScale        Scale to set
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_SetScale(orxBODY *_pstBody, const orxVECTOR *_pvScale)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);
  orxASSERT(_pvScale != orxNULL);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Is new scale different? */
    if(orxVector_AreEqual(_pvScale, &(_pstBody->vScale)) == orxFALSE)
    {
      orxBODY_PART   *pstBodyPart;
      orxBODY_JOINT  *pstBodyJoint;
      orxU32          u32Counter;

      /* Stores it */
      orxVector_Copy(&(_pstBody->vScale), _pvScale);

      /* For all parts */
      for(u32Counter = orxLinkList_GetCounter(&(_pstBody->stPartList)), pstBodyPart = (orxBODY_PART *)orxLinkList_GetFirst(&(_pstBody->stPartList));
          u32Counter > 0;
          u32Counter--, pstBodyPart = (orxBODY_PART *)orxLinkList_GetFirst(&(_pstBody->stPartList)))
      {
        /* Has reference? */
        if(pstBodyPart->zReference != orxNULL)
        {
          const orxSTRING zReference;

          /* Stores it locally */
          zReference = pstBodyPart->zReference;

          /* Removes part */
          orxBody_RemovePart(pstBodyPart);

          /* Creates new part */
          orxBody_AddPartFromConfig(_pstBody, zReference);
        }
        else
        {
          /* Logs message */
          orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "[%s]: Scaling of body part with no config reference is unsupported. Please scale only bodies that contain parts created from config.", (_pstBody->pstOwner != orxNULL) ? orxObject_GetName(orxOBJECT(_pstBody->pstOwner)) : "UNDEFINED");
        }
      }

      /* For all source joints */
      for(u32Counter = orxLinkList_GetCounter(&(_pstBody->stSrcJointList)), pstBodyJoint = orxBODY_GET_FIRST_JOINT_FROM_SRC_LIST(_pstBody);
          u32Counter > 0;
          u32Counter--, pstBodyJoint = orxBODY_GET_FIRST_JOINT_FROM_SRC_LIST(_pstBody))
      {
        /* Has reference? */
        if(pstBodyJoint->zReference != orxNULL)
        {
          const orxSTRING zReference;
          orxBODY        *pstDstBody;

          /* Stores it locally */
          zReference = pstBodyJoint->zReference;

          /* Gets destination body */
          pstDstBody = orxSTRUCT_GET_FROM_FIELD(orxBODY, stDstJointList, orxLinkList_GetList(&(pstBodyJoint->stDstNode)));

          /* Removes part */
          orxBody_RemoveJoint(pstBodyJoint);

          /* Creates new joint */
          orxBody_AddJointFromConfig(_pstBody, pstDstBody, zReference);
        }
        else
        {
          /* Logs message */
          orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "[%s]: Scaling of body joint with no config reference is unsupported. Please scale only bodies that contain joints created from config.", (_pstBody->pstOwner != orxNULL) ? orxObject_GetName(orxOBJECT(_pstBody->pstOwner)) : "UNDEFINED");
        }
      }

      /* For all destination joints */
      for(u32Counter = orxLinkList_GetCounter(&(_pstBody->stDstJointList)), pstBodyJoint = orxBODY_GET_FIRST_JOINT_FROM_DST_LIST(_pstBody);
          u32Counter > 0;
          u32Counter--, pstBodyJoint = orxBODY_GET_FIRST_JOINT_FROM_DST_LIST(_pstBody))
      {
        /* Has reference? */
        if(pstBodyJoint->zReference != orxNULL)
        {
          const orxSTRING zReference;
          orxBODY        *pstSrcBody;

          /* Stores it locally */
          zReference = pstBodyJoint->zReference;

          /* Gets source body */
          pstSrcBody = orxSTRUCT_GET_FROM_FIELD(orxBODY, stSrcJointList, orxLinkList_GetList(&(pstBodyJoint->stSrcNode)));

          /* Removes part */
          orxBody_RemoveJoint(pstBodyJoint);

          /* Creates new joint */
          orxBody_AddJointFromConfig(pstSrcBody, _pstBody, zReference);
        }
        else
        {
          /* Logs message */
          orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "[%s]: Scaling of body joint with no config reference is unsupported. Please scale only bodies that contain joints created from config.", (_pstBody->pstOwner != orxNULL) ? orxObject_GetName(orxOBJECT(_pstBody->pstOwner)) : "UNDEFINED");
        }
      }
    }

    /* Updates result */
    eResult = orxSTATUS_SUCCESS;
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Structure does not have data.");

    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Sets a body speed
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _pvSpeed        Speed to set
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_SetSpeed(orxBODY *_pstBody, const orxVECTOR *_pvSpeed)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);
  orxASSERT(_pvSpeed != orxNULL);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Stores it */
    orxVector_Copy(&(_pstBody->vSpeed), _pvSpeed);

    /* Updates result */
    eResult = orxSTATUS_SUCCESS;
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Structure does not have data.");

    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Sets a body angular velocity
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _fVelocity      Angular velocity to set (radians/seconds)
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_SetAngularVelocity(orxBODY *_pstBody, orxFLOAT _fVelocity)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Stores it */
    _pstBody->fAngularVelocity = _fVelocity;

    /* Updates result */
    eResult = orxSTATUS_SUCCESS;
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Structure does not have data.");

    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Sets a body custom gravity
 * @param[in]   _pstBody          Concerned body
 * @param[in]   _pvCustomGravity  Custom gravity to set / orxNULL to remove it
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_SetCustomGravity(orxBODY *_pstBody, const orxVECTOR *_pvCustomGravity)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Has gravity? */
    if(_pvCustomGravity != orxNULL)
    {
      /* Stores it */
      orxVector_Copy(&(_pstBody->vGravity), _pvCustomGravity);

      /* Updates flags */
      orxStructure_SetFlags(_pstBody, orxBODY_KU32_FLAG_HAS_GRAVITY, orxBODY_KU32_FLAG_NONE);
    }
    else
    {
      /* Clears gravity */
      orxVector_Copy(&(_pstBody->vGravity), &orxVECTOR_0);

      /* Updates flags */
      orxStructure_SetFlags(_pstBody, orxBODY_KU32_FLAG_NONE, orxBODY_KU32_FLAG_HAS_GRAVITY);
    }

    /* Updates result */
    eResult = orxSTATUS_SUCCESS;
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Structure does not have data.");

    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Sets a body fixed rotation
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _bFixed         Fixed / not fixed
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_SetFixedRotation(orxBODY *_pstBody, orxBOOL _bFixed)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Updates physics body fixed rotation */
  eResult = orxPhysics_SetFixedRotation(_pstBody->pstData, _bFixed);

  /* Done! */
  return eResult;
}

/** Gets a body position
 * @param[in]   _pstBody        Concerned body
 * @param[out]  _pvPosition     Position to get
 * @return      Body position / orxNULL
 */
orxVECTOR *orxFASTCALL orxBody_GetPosition(const orxBODY *_pstBody, orxVECTOR *_pvPosition)
{
  orxVECTOR *pvResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);
  orxASSERT(_pvPosition != orxNULL);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Updates result */
    pvResult = orxPhysics_GetPosition(_pstBody->pstData, _pvPosition);
  }
  else
  {
    /* Updates result */
    pvResult = orxNULL;
  }

  /* Done! */
  return pvResult;
}

/** Gets a body rotation
 * @param[in]   _pstBody        Concerned body
 * @return      Body rotation (radians)
 */
orxFLOAT orxFASTCALL orxBody_GetRotation(const orxBODY *_pstBody)
{
  orxFLOAT fResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Updates result */
    fResult = orxPhysics_GetRotation(_pstBody->pstData);
  }
  else
  {
    /* Updates result */
    fResult = orxFLOAT_0;
  }

  /* Done! */
  return fResult;
}

/** Gets a body speed
 * @param[in]   _pstBody        Concerned body
 * @param[out]   _pvSpeed       Speed to get
 * @return      Body speed / orxNULL
 */
orxVECTOR *orxFASTCALL orxBody_GetSpeed(const orxBODY *_pstBody, orxVECTOR *_pvSpeed)
{
  orxVECTOR *pvResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);
  orxASSERT(_pvSpeed != orxNULL);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Updates result */
    pvResult = orxVector_Copy(_pvSpeed, &(_pstBody->vSpeed));
  }
  else
  {
    /* Updates result */
    pvResult = orxNULL;
  }

  /* Done! */
  return pvResult;
}

/** Gets a body speed at a specified world point
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _pvPosition     Concerned world position
 * @param[out]  _pvSpeed        Speed to get
 * @return      Body speed / orxNULL
 */
orxVECTOR *orxFASTCALL orxBody_GetSpeedAtWorldPosition(const orxBODY *_pstBody, const orxVECTOR *_pvPosition, orxVECTOR *_pvSpeed)
{
  orxVECTOR *pvResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);
  orxASSERT(_pvPosition != orxNULL);
  orxASSERT(_pvSpeed != orxNULL);

  /* Updates result */
  pvResult = orxPhysics_GetSpeedAtWorldPosition(_pstBody->pstData, _pvPosition, _pvSpeed);

  /* Done! */
  return pvResult;
}

/** Gets a body angular velocity
 * @param[in]   _pstBody        Concerned body
 * @return      Body angular velocity (radians/seconds)
 */
orxFLOAT orxFASTCALL orxBody_GetAngularVelocity(const orxBODY *_pstBody)
{
  orxFLOAT fResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Updates result */
    fResult = _pstBody->fAngularVelocity;
  }
  else
  {
    /* Updates result */
    fResult = orxFLOAT_0;
  }

  /* Done! */
  return fResult;
}

/** Gets a body custom gravity
 * @param[in]   _pstBody          Concerned body
 * @param[out]  _pvCustomGravity  Custom gravity to get
 * @return      Body custom gravity / orxNULL is object doesn't have any
 */
orxVECTOR *orxFASTCALL orxBody_GetCustomGravity(const orxBODY *_pstBody, orxVECTOR *_pvCustomGravity)
{
  orxVECTOR *pvResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);
  orxASSERT(_pvCustomGravity != orxNULL);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Has gravity? */
    if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_GRAVITY))
    {
      /* Updates result */
      pvResult = orxVector_Copy(_pvCustomGravity, &(_pstBody->vGravity));
    }
    else
    {
      /* Updates result */
      pvResult = orxNULL;
    }
  }
  else
  {
    /* Updates result */
    pvResult = orxNULL;
  }

  /* Done! */
  return pvResult;
}

/** Is a body using a fixed rotation
 * @param[in]   _pstBody        Concerned body
 * @return      orxTRUE if fixed rotation, orxFALSE otherwise
 */
orxBOOL orxFASTCALL orxBody_IsFixedRotation(const orxBODY *_pstBody)
{
  orxBOOL bResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Updates result */
  bResult = orxPhysics_IsFixedRotation(_pstBody->pstData);

  /* Done! */
  return bResult;
}

/** Gets a body mass
 * @param[in]   _pstBody        Concerned body
 * @return      Body mass
 */
orxFLOAT orxFASTCALL orxBody_GetMass(const orxBODY *_pstBody)
{
  orxFLOAT fResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Gets mass */
    fResult = orxPhysics_GetMass(_pstBody->pstData);
  }
  else
  {
    /* Updates result */
    fResult = orxFLOAT_0;
  }

  /* Done! */
  return fResult;
}

/** Gets a body center of mass (object space)
 * @param[in]   _pstBody        Concerned body
 * @param[out]  _pvMassCenter   Mass center to get
 * @return      Mass center / orxNULL
 */
orxVECTOR *orxFASTCALL orxBody_GetMassCenter(const orxBODY *_pstBody, orxVECTOR *_pvMassCenter)
{
  orxVECTOR *pvResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);
  orxASSERT(_pvMassCenter != orxNULL);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Valid scale? */
    if((_pstBody->vScale.fX != orxFLOAT_0)
    && (_pstBody->vScale.fY != orxFLOAT_0))
    {
      /* Gets mass center */
      pvResult = orxPhysics_GetMassCenter(_pstBody->pstData, _pvMassCenter);

      /* Removes scale */
      pvResult->fX /= _pstBody->vScale.fX;
      pvResult->fY /= _pstBody->vScale.fY;
    }
    else
    {
      /* Updates result */
      pvResult = orxVector_Copy(_pvMassCenter, &orxVECTOR_0);
    }
  }
  else
  {
    /* Updates result */
    pvResult = orxNULL;
  }

  /* Done! */
  return pvResult;
}

/** Sets a body linear damping
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _fDamping       Linear damping to set
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_SetLinearDamping(orxBODY *_pstBody, orxFLOAT _fDamping)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Sets linear damping */
  eResult = orxPhysics_SetLinearDamping(_pstBody->pstData, _fDamping);

  /* Done! */
  return eResult;
}

/** Sets a body angular damping
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _fDamping       Angular damping to set
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_SetAngularDamping(orxBODY *_pstBody, orxFLOAT _fDamping)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Sets angular damping */
  eResult = orxPhysics_SetAngularDamping(_pstBody->pstData, _fDamping);

  /* Done! */
  return eResult;
}

/** Gets a body linear damping
 * @param[in]   _pstBody        Concerned body
 * @return      Body's linear damping
 */
orxFLOAT orxFASTCALL orxBody_GetLinearDamping(const orxBODY *_pstBody)
{
  orxFLOAT fResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Gets linear damping */
  fResult = orxPhysics_GetLinearDamping(_pstBody->pstData);

  /* Done! */
  return fResult;
}

/** Gets a body angular damping
 * @param[in]   _pstBody        Concerned body
 * @return      Body's angular damping
 */
orxFLOAT orxFASTCALL orxBody_GetAngularDamping(const orxBODY *_pstBody)
{
  orxFLOAT fResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Gets angular damping */
  fResult = orxPhysics_GetAngularDamping(_pstBody->pstData);

  /* Done! */
  return fResult;
}

/** Applies a torque
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _fTorque        Torque to apply
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_ApplyTorque(orxBODY *_pstBody, orxFLOAT _fTorque)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Applies torque */
    eResult = orxPhysics_ApplyTorque(_pstBody->pstData, _fTorque);
  }
  else
  {
    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Applies a force
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _pvForce        Force to apply
 * @param[in]   _pvPoint        Point (world coordinates) where the force will be applied, if orxNULL, center of mass will be used
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_ApplyForce(orxBODY *_pstBody, const orxVECTOR *_pvForce, const orxVECTOR *_pvPoint)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);
  orxASSERT(_pvForce != orxNULL);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Applies force */
    eResult = orxPhysics_ApplyForce(_pstBody->pstData, _pvForce, _pvPoint);
  }
  else
  {
    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Applies an impulse
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _pvImpulse      Impulse to apply
 * @param[in]   _pvPoint        Point (world coordinates) where the impulse will be applied, if orxNULL, center of mass will be used
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_ApplyImpulse(orxBODY *_pstBody, const orxVECTOR *_pvImpulse, const orxVECTOR *_pvPoint)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);
  orxASSERT(_pvImpulse != orxNULL);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Enforces body speed */
    orxPhysics_SetSpeed(_pstBody->pstData, &(_pstBody->vSpeed));

    /* Applies impulse */
    eResult = orxPhysics_ApplyImpulse(_pstBody->pstData, _pvImpulse, _pvPoint);

    /* Success? */
    if(eResult != orxSTATUS_FAILURE)
    {
      /* Updates body speed (so as to not override the effect of impulse during the physics update) */
      orxPhysics_GetSpeed(_pstBody->pstData, &(_pstBody->vSpeed));
    }
  }
  else
  {
    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Sets self flags of a physical body part
 * @param[in]   _pstBodyPart    Concerned physical body part
 * @param[in]   _u16SelfFlags   Self flags to set
 * @return orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_SetPartSelfFlags(orxBODY_PART *_pstBodyPart, orxU16 _u16SelfFlags)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstBodyPart != orxNULL);

  /* Sets self flags */
  eResult = orxPhysics_SetPartSelfFlags(_pstBodyPart->pstData, _u16SelfFlags);

  /* Done! */
  return eResult;
}

/** Sets check mask of a physical body part
 * @param[in]   _pstBodyPart    Concerned physical body part
 * @param[in]   _u16CheckMask   Check mask to set
 * @return orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_SetPartCheckMask(orxBODY_PART *_pstBodyPart, orxU16 _u16CheckMask)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstBodyPart != orxNULL);

  /* Sets check mask */
  eResult = orxPhysics_SetPartCheckMask(_pstBodyPart->pstData, _u16CheckMask);

  /* Done! */
  return eResult;
}

/** Gets self flags of a physical body part
 * @param[in]   _pstBodyPart    Concerned physical body part
 * @return Self flags of the physical body part
 */
orxU16 orxFASTCALL orxBody_GetPartSelfFlags(const orxBODY_PART *_pstBodyPart)
{
  orxU16 u16Result;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstBodyPart != orxNULL);

  /* Gets self flags */
  u16Result = orxPhysics_GetPartSelfFlags(_pstBodyPart->pstData);

  /* Done! */
  return u16Result;
}

/** Gets check mask of a physical body part
 * @param[in]   _pstBodyPart    Concerned physical body part
 * @return Check mask of the physical body part
 */
orxU16 orxFASTCALL orxBody_GetPartCheckMask(const orxBODY_PART *_pstBodyPart)
{
  orxU16 u16Result;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstBodyPart != orxNULL);

  /* Gets check mask */
  u16Result = orxPhysics_GetPartCheckMask(_pstBodyPart->pstData);

  /* Done! */
  return u16Result;
}

/** Is a body part solid?
 * @param[in]   _pstBodyPart    Concerned body part
 * @return      orxTRUE / orxFALSE
 */
orxBOOL orxFASTCALL orxBody_IsPartSolid(const orxBODY_PART *_pstBodyPart)
{
  orxBOOL bResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstBodyPart != orxNULL);

  /* Updates result */
  bResult = orxPhysics_IsPartSolid(_pstBodyPart->pstData);

  /* Done! */
  return bResult;
}

/** Sets a body part solid
 * @param[in]   _pstBodyPart    Concerned body part
 * @param[in]   _bSolid         Solid or sensor?
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_SetPartSolid(orxBODY_PART *_pstBodyPart, orxBOOL _bSolid)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstBodyPart != orxNULL);

  /* Updates result */
  eResult = orxPhysics_SetPartSolid(_pstBodyPart->pstData, _bSolid);

  /* Done! */
  return eResult;
}

/** Issues a raycast to test for potential bodies in the way
 * @param[in]   _pvStart        Start of raycast
 * @param[in]   _pvEnd          End of raycast
 * @param[in]   _u16SelfFlags   Selfs flags used for filtering (0xFFFF for no filtering)
 * @param[in]   _u16CheckMask   Check mask used for filtering (0xFFFF for no filtering)
 * @param[in]   _bEarlyExit     Should stop as soon as an object has been hit (which might not be the closest)
 * @param[in]   _pvContact      If non-null and a contact is found it will be stored here
 * @param[in]   _pvNormal       If non-null and a contact is found, its normal will be stored here
 * @return Colliding orxBODY / orxNULL
 */
orxBODY *orxFASTCALL orxBody_Raycast(const orxVECTOR *_pvStart, const orxVECTOR *_pvEnd, orxU16 _u16SelfFlags, orxU16 _u16CheckMask, orxBOOL _bEarlyExit, orxVECTOR *_pvContact, orxVECTOR *_pvNormal)
{
  orxHANDLE hRaycastResult;
  orxBODY  *pstResult = orxNULL;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxASSERT(_pvStart != orxNULL);
  orxASSERT(_pvEnd != orxNULL);

  /* Issues raycast */
  hRaycastResult = orxPhysics_Raycast(_pvStart, _pvEnd, _u16SelfFlags, _u16CheckMask, _bEarlyExit, _pvContact, _pvNormal);

  /* Found? */
  if(hRaycastResult != orxHANDLE_UNDEFINED)
  {
    /* Updates result */
    pstResult = orxBODY(hRaycastResult);
  }

  /* Done! */
  return pstResult;
}
