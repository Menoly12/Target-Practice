// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Implementation of CTFPlayerShared
// Author(s): Cherry!
//

#include "cbase.h"
#include "of_weapon_base.h"
#include "of_shareddefs.h"
#include "of_player_shared.h"
#include "of_gamerules.h"
#include "of_item.h"

#ifdef CLIENT_DLL
	#include "c_of_player.h"
#else
	#include "of_player.h"
#endif

// Data Tables
#ifdef CLIENT_DLL

BEGIN_PREDICTION_DATA_NO_BASE(COFPlayerShared)
	DEFINE_PRED_FIELD(m_nPlayerState, FIELD_INTEGER, FTYPEDESC_INSENDTABLE),
END_PREDICTION_DATA()

#else

#endif

BEGIN_NETWORK_TABLE_NOBASE( COFPlayerShared, DT_OFPlayerShared )
#ifdef CLIENT_DLL
	RecvPropInt( RECVINFO( m_nPlayerState ) ),
	RecvPropDataTable( RECVINFO_DT(m_Conds), 0, &REFERENCE_RECV_TABLE(DT_CondManager) ),
#else
	SendPropInt( SENDINFO( m_nPlayerState ) ),
	SendPropDataTable( SENDINFO_DT(m_Conds), &REFERENCE_SEND_TABLE(DT_CondManager) ),
#endif
END_NETWORK_TABLE()

//OFSTATUS: INCOMPLETE
COFPlayerShared::COFPlayerShared()
{
	field_0x324 = 0.0;
	m_bAllowedToPickUpFlag = true;
	m_nPlayerState = TF_STATE_ACTIVE;
}

//OFSTATUS: INCOMPLETE
COFPlayerShared::~COFPlayerShared()
{
}

//OFSTATUS: INCOMPLETE
float COFPlayerShared::GetCritMult()
{
	float flCritCalc = field_0x324 * 0.003921569;

	flCritCalc = clamp(flCritCalc, 0, 1.0);

	return (flCritCalc * 3.0) + 1.0;
}

//OFSTATUS: INCOMPLETE
// NOTENOTE: With the new cond system, you can have the same cond multiple times
// Every instance getting added will trigger this
// So make sure if you want something to trigger only once, for example burning particles
// Check if you have it already

// The same thing goes for OnConditionRemoved
void COFPlayerShared::OnConditionAdded( ETFCond nCond )
{
/*
  char cVar1;
  CEconItemSchema *this_00;
  CEconItemAttributeDefinition *pCVar2;
  CAttributeList *this_01;
  CBaseEntity *this_02;
  char *pcVar3;
  float fVar4;
  
  if ((int)param_1 < 0x31) {
    switch(param_1) {
    case 4:
switchD_002d95bc_caseD_40:
      OnAddStealthed(this);
      return;
    case 5:
switchD_002d9595_caseD_34:
      OnAddInvulnerable(this);
      return;
    case 7:
      OnAddTaunting(this);
      return;
    case 0xd:
      OnAddFeignDeath(this);
      return;
    case 0xe:
      UpdatePhaseEffects(this);
      goto LAB_002d96a1;
    case 0xf:
      OnAddStunned(this);
      return;
    case 0x11:
      OnAddShieldCharge(this);
      return;
    case 0x13:
      cVar1 = CTFPlayer::IsPlayerClass((CTFPlayer *)this->field_0x190,6);
      if ((cVar1 == '\0') &&
         (cVar1 = CTFPlayer::IsPlayerClass((CTFPlayer *)this->field_0x190,1), cVar1 == '\0')) {
        return;
      }
    case 0x20:
switchD_002d9595_caseD_31:
      CTFPlayer::TeamFortress_SetSpeed((CTFPlayer *)this->field_0x190);
      break;
    case 0x15:
      this->field_0x1a8 = 0;
      this->field_0x1ac = 0;
      this->field_0x1b4 = (*PTR__gpGlobals_00e34080)->curtime + 1.0;
      break;
    case 0x16:
      this_02 = (CBaseEntity *)this->field_0x190;
      pcVar3 = "Fire.Engulf";
LAB_002d9932:
      CBaseEntity::EmitSound(this_02,pcVar3,0.0,(float *)0x0);
      break;
    case 0x1b:
      OnAddMadMilk(this);
      return;
    case 0x1d:
      AddCond(this,0x20,-1.0,(CBaseEntity *)0x0);
      break;
    case 0x25:
      if (*(char *)&this->field_0x488 != '\x01') {
        (**(code **)(this->field_0x0 + 0x10))(this,&this->field_0x488);
        *(undefined *)&this->field_0x488 = 1;
      }
    }
  }
  else {
    if ((int)param_1 < 0x40) {
      switch(param_1) {
      case 0x31:
        goto switchD_002d9595_caseD_31;
      default:
        return;
      case 0x34:
      case 0x39:
        goto switchD_002d9595_caseD_34;
      case 0x35:
        if ((*(byte *)((int)&this->field_0xd4 + 2) & 4) == 0) {
          return;
        }
        this_00 = (CEconItemSchema *)GetItemSchema();
        pCVar2 = (CEconItemAttributeDefinition *)
                 CEconItemSchema::GetAttributeDefinitionByName(this_00,"head scale");
        this_01 = (CAttributeList *)
                  (**(code **)(*(int *)this->field_0x190 + 0x780))((int *)this->field_0x190);
        CAttributeList::RemoveAttribute(this_01,pCVar2);
        return;
      case 0x37:
        *(undefined *)((int)&this->field_0x1f4 + 1) = 1;
        return;
      }
    }
    if ((int)param_1 < 100) {
      switch(param_1) {
      case 0x40:
        goto switchD_002d95bc_caseD_40;
      default:
        return;
      case 0x47:
        cVar1 = (**(code **)(*(int *)this->field_0x190 + 0x6fc))((int *)this->field_0x190);
        if (cVar1 == '\0') {
          return;
        }
        fVar4 = 0.0;
        if ((*(byte *)&this->field_0xd4 & 0x80) != 0) {
          fVar4 = *(float *)(this->field_0x8 + 0x594);
        }
        StunPlayer(this,fVar4,1.0,0x23,(CTFPlayer *)0x0);
        return;
      case 0x48:
        OnAddHalloweenSpeedBoost(this);
        return;
      case 0x49:
        AddCond(this,0x1c,-1.0,(CBaseEntity *)0x0);
        Heal(this,(CBaseEntity *)this->field_0x190,(float)&DAT_41f00000,2.0,1.0,false,
             (CTFPlayer *)0x0);
        return;
      case 0x4a:
        OnAddHalloweenGiant(this);
        return;
      case 0x4b:
        OnAddHalloweenTiny(this);
        return;
      case 0x4d:
        OnAddHalloweenGhostMode(this);
        return;
      case 0x52:
        OnAddHalloweenKart(this);
        return;
      case 0x53:
        CBasePlayer::SetFOV((CBasePlayer *)this->field_0x190,
                            (CBaseEntity *)(CBasePlayer *)this->field_0x190,0x6e,1.0,0);
        CTFPlayer::DoAnimationEvent((CTFPlayer *)this->field_0x190,0x14,0x744);
        this_02 = (CBaseEntity *)this->field_0x190;
        pcVar3 = "BumperCar.SpeedBoostStart";
        break;
      case 0x54:
        OnAddBalloonHead(this);
        return;
      case 0x55:
        OnAddMeleeOnly(this);
        return;
      case 0x58:
        AddCond(this,0x57,-1.0,(CBaseEntity *)0x0);
        return;
      }
      goto LAB_002d9932;
    }
    if (0x6b < (int)param_1) {
      if (param_1 == 0x6c) {
        OnAddInPurgatory(this);
        return;
      }
      if (param_1 != 0x70) {
        return;
      }
      OnAddPlague(this);
      return;
    }
    if (param_1 == 100) {
      CTFPlayer::DoAnimationEvent((CTFPlayer *)this->field_0x190,0x14,0x750);
      return;
    }
    if (param_1 != 0x6a) {
      return;
    }
    if (*(char *)&this->field_0x180 == '\0') {
      AddPhaseEffects(this);
    }
LAB_002d96a1:
    (**(code **)(*(int *)this->field_0x190 + 0x740))
              ((int *)this->field_0x190,0x40,"started_dodging:1",0,0,0);
  }
  */
  return;
}


void COFPlayerShared::OnConditionRemoved( ETFCond nCond )
{
	/*
  CTFPlayer *pCVar1;
  char cVar2;
  int *piVar3;
  int iVar4;
  undefined4 *puVar5;
  
  if ((int)param_1 < 7) {
    switch(param_1) {
    case 1:
      CBasePlayer::SetFOV((CBasePlayer *)this->field_0x190,
                          (CBaseEntity *)(CBasePlayer *)this->field_0x190,0,0.1,0);
      return;
    case 2:
      if (this->field_0x120 == 0) {
        return;
      }
      puVar5 = &this->field_0x120;
      iVar4 = this->field_0x0;
      break;
    case 3:
      OnRemoveDisguised(this);
      return;
    case 4:
switchD_002d9d9b_caseD_4:
      OnRemoveStealthed(this);
      return;
    default:
      return;
    }
    goto LAB_002d9fb9;
  }
  if ((int)param_1 < 0x6a) {
    if ((int)param_1 < 0x48) {
      if ((int)param_1 < 0x31) {
        switch(param_1) {
        case 7:
          OnRemoveTaunting(this);
          return;
        default:
          return;
        case 0xc:
          this->field_0x234 = 0x3f800000;
          return;
        case 0xe:
          OnRemovePhase(this);
          return;
        case 0xf:
          OnRemoveStunned(this);
          return;
        case 0x11:
          RemovePhaseEffects(this);
          CTFPlayer::TeamFortress_SetSpeed((CTFPlayer *)this->field_0x190);
          *(undefined *)&this->field_0x3d0 = 1;
          this->field_0x3d4 = (*PTR__gpGlobals_00e34080)->curtime;
          if (this->field_0x288 == 0) {
            return;
          }
          puVar5 = &this->field_0x288;
          iVar4 = this->field_0x0;
          goto LAB_002d9fb9;
        case 0x13:
          cVar2 = CTFPlayer::IsPlayerClass((CTFPlayer *)this->field_0x190,6);
          if ((cVar2 == '\0') &&
             (cVar2 = CTFPlayer::IsPlayerClass((CTFPlayer *)this->field_0x190,1), cVar2 == '\0')) {
            return;
          }
          break;
        case 0x15:
          this->field_0x1a8 = 0;
          this->field_0x1ac = 0;
          return;
        case 0x16:
          this->field_0x1f8 = 0xffffffff;
          this->field_0x1fc = 0xffffffff;
          this->field_0x200 = 0xffffffff;
          pCVar1 = (CTFPlayer *)this->field_0x190;
          *(undefined4 *)&pCVar1->field_0x221c = 0;
          if (this->field_0xc8 == 0) {
            HatAndMiscEconEntities_OnOwnerKillEaterEventNoParter(pCVar1,0x53,1);
          }
          if ((*(byte *)((int)&this->field_0xd8 + 2) & 0x40) == 0) {
            return;
          }
          RemoveCond(this,0x76,false);
          return;
        case 0x18:
          OnRemoveUrine(this);
          return;
        case 0x19:
          this->field_0x218 = 0;
          return;
        case 0x1d:
          RemoveCond(this,0x20,false);
          return;
        case 0x20:
          break;
        case 0x25:
          if (*(char *)&this->field_0x488 == '\0') {
            return;
          }
          (**(code **)(this->field_0x0 + 0x10))(this,&this->field_0x488);
          *(undefined *)&this->field_0x488 = 0;
          return;
        }
switchD_002d9f54_caseD_31:
        CTFPlayer::TeamFortress_SetSpeed((CTFPlayer *)this->field_0x190);
        return;
      }
      if (0x3f < (int)param_1) {
        if (param_1 != 0x40) {
          return;
        }
        goto switchD_002d9d9b_caseD_4;
      }
      switch(param_1) {
      case 0x31:
        goto switchD_002d9f54_caseD_31;
      default:
        return;
      case 0x35:
        OnRemoveHalloweenBombHead(this);
        return;
      case 0x36:
        goto switchD_002d9f54_caseD_36;
      case 0x37:
        *(undefined *)((int)&this->field_0x1f4 + 1) = 0;
        if ((int)this->field_0x1e4 < 1) {
          return;
        }
        iVar4 = 0;
        do {
          piVar3 = (int *)UTIL_PlayerByIndex(*(int *)(this->field_0x1d8 + iVar4 * 4));
          if ((piVar3 != (int *)0x0) &&
             (cVar2 = (**(code **)(*piVar3 + 0x144))(piVar3), cVar2 != '\0')) {
            StopHealing((CTFPlayerShared *)(piVar3 + 0x66e),(CBaseEntity *)this->field_0x190);
          }
          iVar4 = iVar4 + 1;
        } while (iVar4 < (int)this->field_0x1e4);
        this->field_0x1e4 = 0;
        return;
      }
    }
    switch(param_1) {
    case 0x48:
      OnRemoveHalloweenSpeedBoost(this);
      return;
    case 0x49:
      RemoveCond(this,0x1c,false);
      goto switchD_002d9f54_caseD_36;
    case 0x4a:
      OnRemoveHalloweenGiant(this);
      return;
    case 0x4b:
      OnRemoveHalloweenTiny(this);
      return;
    case 0x4d:
      OnRemoveHalloweenGhostMode(this);
      return;
    case 0x52:
      OnRemoveHalloweenKart(this);
      return;
    case 0x53:
      CBasePlayer::SetFOV((CBasePlayer *)this->field_0x190,
                          (CBaseEntity *)(CBasePlayer *)this->field_0x190,0,1.0,0);
      CBaseEntity::EmitSound
                ((CBaseEntity *)this->field_0x190,"BumperCar.SpeedBoostStop",0.0,(float *)0x0);
      break;
    case 0x54:
      OnRemoveBalloonHead(this);
      return;
    case 0x55:
      OnRemoveMeleeOnly(this);
      return;
    case 0x56:
      AddCond(this,0x18,10.0,(CBaseEntity *)0x0);
    case 0x58:
      RemoveCond(this,0x57,false);
      DispatchParticleEffect("ghost_appearation",0,(CBaseEntity *)this->field_0x190,-1,false);
    }
  }
  else {
    if ((int)param_1 < 0x7c) {
      switch(param_1) {
      case 0x6a:
        if (*(char *)&this->field_0x180 != '\0') {
          RemovePhaseEffects(this);
        }
        (**(code **)(*(int *)this->field_0x190 + 0x740))
                  ((int *)this->field_0x190,0x40,"started_dodging:0",0,0,0);
        return;
      default:
        return;
      case 0x6c:
        OnRemoveInPurgatory(this);
        return;
      case 0x6f:
        goto switchD_002d9e24_caseD_6f;
      case 0x70:
        CBaseEntity::StopSound((CBaseEntity *)this->field_0x190,"Powerup.PickUpPlagueInfectedLoop");
        return;
      }
    }
    if (param_1 == 0x7c) {
      cVar2 = CTFPlayer::IsPlayerClass((CTFPlayer *)this->field_0x190,7);
      if (cVar2 == '\0') {
        return;
      }
      cVar2 = CTFConditionList::InCond((CTFConditionList *)&this->field_0xe0,0x16);
      if ((cVar2 == '\0') && ((*(byte *)((int)&this->field_0xcc + 2) & 0x40) == 0)) {
        return;
      }
      RemoveCond(this,0x16,false);
      return;
    }
    if (param_1 != 0x80) {
      return;
    }
switchD_002d9f54_caseD_36:
    StopHealing(this,(CBaseEntity *)this->field_0x190);
  }
  return;
switchD_002d9e24_caseD_6f:
  if (this->field_0x2c8 == 0) {
    return;
  }
  puVar5 = &this->field_0x2c8;
  iVar4 = this->field_0x0;
LAB_002d9fb9:
  (**(code **)(iVar4 + 0x10))(this,puVar5);
  *puVar5 = 0;
  return;
  */
}

// OFSTATUS: Incomplete?
// Possibly going to be replaced by running the condition think functions
// inside the actual conditions
// Though OFC we still need to RE Them so we know what they do
void COFPlayerShared::ConditionThink( void )
{
	// Do all the think functions for conditions
	m_Conds.RunThink();
	/*
  undefined4 *this_00;
  uint *puVar1;
  float *pfVar2;
  float fVar3;
  uint *puVar4;
  CMerasmus *this_01;
  code **ppcVar5;
  char cVar6;
  int *piVar7;
  CBaseEntity *pCVar8;
  int iVar9;
  int *piVar10;
  float *pfVar11;
  uint uVar12;
  byte bVar13;
  int iVar14;
  CTFPlayerShared **ppCVar15;
  uint uVar16;
  uint uVar17;
  float10 fVar18;
  uint *local_3c;
  int *local_38;
  int local_34;
  undefined4 *local_30;
  float local_2c;
  float local_28;
  float local_24;
  float local_20;
  float local_1c;
  uint local_14;
  
  iVar14 = 0x2e31ee;
  UpdateCloakMeter(this);
  UpdateRageBuffsAndRage(this);
  UpdateEnergyDrinkMeter(this);
  UpdateChargeMeter(this);
  DemoShieldChargeThink(this);
  if (((*(byte *)(this->field_0x190 + 300) & 1) != 0) &&
     ((*(byte *)((int)&this->field_0xd4 + 2) & 1) != 0)) {
    RemoveCond(this,0x50,false);
  }
  PulseMedicRadiusHeal(this);
  PulseKingRuneBuff(this);
  this_00 = &this->field_0xe0;
  local_30 = this_00;
  CTFConditionList::Think((CTFConditionList *)this_00);
  cVar6 = CTFConditionList::InCond((CTFConditionList *)this_00,0xf);
  if (((cVar6 != '\0') || ((*(byte *)((int)&this->field_0xcc + 1) & 0x80) != 0)) &&
     (iVar9 = this->field_0x380, -1 < iVar9)) {
    if ((((this->field_0x458 != iVar9 && iVar9 <= (int)this->field_0x458) &&
         (iVar9 * 0x18 + this->field_0x44c != 0)) &&
        ((cVar6 = CTFConditionList::InCond((CTFConditionList *)local_30,0xf), cVar6 != '\0' ||
         ((*(byte *)((int)&this->field_0xcc + 1) & 0x80) != 0)))) &&
       ((*(byte *)&this->field_0x37c & 2) != 0)) {
      CBaseEntity::SetAbsAngles
                ((CBaseEntity *)this->field_0x190,
                 (QAngle *)((CBaseEntity *)this->field_0x190 + 0x1e84));
      CBaseEntity::SetLocalAngles
                ((CBaseEntity *)this->field_0x190,
                 (QAngle *)((CBaseEntity *)this->field_0x190 + 0x1e84));
    }
    iVar9 = this->field_0x380;
    if (-1 < iVar9) {
      piVar7 = (int *)this->field_0x458;
      if ((int *)((int)piVar7 - iVar9) != (int *)0x0 && iVar9 <= (int)piVar7) {
        local_34 = 0x2e31ee;
        puVar4 = (uint *)this->field_0x44c;
        local_38 = piVar7;
        if ((puVar4 + iVar9 * 6 == (uint *)0x0) ||
           (local_3c = puVar4,
           (*PTR__gpGlobals_00e34080)->curtime < (float)puVar4[iVar9 * 6 + 2] ||
           (*PTR__gpGlobals_00e34080)->curtime == (float)puVar4[iVar9 * 6 + 2])) {
          iVar14 = 0x2e31ee;
          if (iVar9 * 0x18 + this->field_0x44c != 0) {
            CTFConditionList::InCond((CTFConditionList *)local_30,0xf);
          }
        }
        else {
          puVar1 = &this->field_0x380;
          iVar14 = (int)(int *)((int)piVar7 - iVar9) + -1;
          if (0 < iVar14) {
            __symbol_stub::_memmove(puVar4 + iVar9 * 6,puVar4 + iVar9 * 6 + 6,iVar14 * 0x18);
            local_38 = (int *)this->field_0x458;
          }
          piVar7 = (int *)((int)local_38 + -1);
          this->field_0x458 = piVar7;
          local_14 = 0xffffffff;
          if (*puVar1 != 0xffffffff) {
            (**(code **)(this->field_0x0 + 0x10))();
            this->field_0x380 = 0xffffffff;
            piVar7 = (int *)this->field_0x458;
          }
          if (piVar7 == (int *)0x0) {
            RemoveCond(this,0xf,false);
          }
          else {
            uVar16 = 0;
            if (1 < (int)piVar7) {
              iVar14 = this->field_0x44c;
              if (((int)piVar7 - 1U & 1) == 0) {
                uVar16 = 0;
                uVar12 = 1;
              }
              else {
                uVar16 = (uint)(*(float *)(iVar14 + 0x10) <= *(float *)(iVar14 + 0x28) &&
                               *(float *)(iVar14 + 0x28) != *(float *)(iVar14 + 0x10));
                uVar12 = 2;
              }
              local_38 = piVar7;
              if (piVar7 != (int *)((int)&mach_header_00000000.magic + 2)) {
                pfVar11 = (float *)(iVar14 + 0x28 + uVar12 * 0x18);
                do {
                  pfVar2 = (float *)(iVar14 + 0x10 + uVar16 * 0x18);
                  uVar17 = uVar12;
                  if (pfVar11[-6] < *pfVar2 || pfVar11[-6] == *pfVar2) {
                    uVar17 = uVar16;
                  }
                  pfVar2 = (float *)(iVar14 + 0x10 + uVar17 * 0x18);
                  uVar16 = uVar12 + 1;
                  if (*pfVar11 < *pfVar2 || *pfVar11 == *pfVar2) {
                    uVar16 = uVar17;
                  }
                  uVar12 = uVar12 + 2;
                  pfVar11 = pfVar11 + 0xc;
                } while ((int)uVar12 < (int)piVar7);
              }
            }
            local_3c = puVar1;
            local_14 = uVar16;
            if (*puVar1 == uVar16) {
              uVar16 = *puVar1;
            }
            else {
              (**(code **)(this->field_0x0 + 0x10))();
              *puVar1 = uVar16;
            }
            uVar16 = *(uint *)(this->field_0x44c + uVar16 * 0x18);
            pCVar8 = (CBaseEntity *)0x0;
            if (uVar16 != 0xffffffff) {
              iVar14 = (uVar16 & 0xfff) * 0x10;
              if (*(uint *)(**(int **)(local_34 + 0xb50e8e) + 8 + iVar14) == uVar16 >> 0xc) {
                pCVar8 = *(CBaseEntity **)(**(int **)(local_34 + 0xb50e8e) + 4 + iVar14);
              }
            }
            AddCond(this,0xf,-1.0,pCVar8);
            bVar13 = *(byte *)&this->field_0x374 + 1 & 3;
            if (*(byte *)&this->field_0x374 != bVar13) {
              (**(code **)(this->field_0x0 + 0x10))();
              *(byte *)&this->field_0x374 = bVar13;
            }
          }
          UpdateLegacyStunSystem(this);
          iVar14 = local_34;
        }
      }
    }
  }
  if ((*(byte *)((int)&this->field_0xd0 + 2) & 0x20) == 0) {
    local_14 = 0;
    ppCVar15 = (CTFPlayerShared **)&stack0xffffffb4;
    if (this->field_0x18c != 0) {
      (**(code **)(this->field_0x0 + 0x10))();
      this->field_0x18c = 0;
      ppCVar15 = (CTFPlayerShared **)&stack0xffffffb4;
    }
  }
  else {
    fVar3 = *(float *)(this->field_0x8 + 0x42c);
    iVar9 = 0;
    if ((((*(float *)("All lootlists must have names." + iVar14 + 0x14) <= fVar3) ||
         (iVar9 = 1, *(float *)("All lootlists must have names." + iVar14 + 0x1c) <= fVar3)) ||
        (iVar9 = 2, *(float *)("Duplicate lootlist name (%s) found!" + iVar14 + 5) <= fVar3)) &&
       (uVar16 = *(uint *)("All lootlists must have names." + iVar9 * 8 + iVar14 + 0x18),
       local_14 = uVar16, this->field_0x18c != uVar16)) {
      local_34 = iVar14;
      (**(code **)(this->field_0x0 + 0x10))();
      this->field_0x18c = uVar16;
      iVar14 = local_34;
    }
    piVar7 = *(int **)(iVar14 + 0xb50eba);
    iVar9 = *piVar7;
    ppCVar15 = (CTFPlayerShared **)&stack0xffffffb4;
    if (((iVar9 != 0) &&
        (ppCVar15 = (CTFPlayerShared **)&stack0xffffffb4, 0 < *(int *)(iVar9 + 0xca8))) &&
       (uVar16 = **(uint **)(iVar9 + 0xc9c), ppCVar15 = (CTFPlayerShared **)&stack0xffffffb4,
       uVar16 != 0xffffffff)) {
      local_38 = *(int **)(iVar14 + 0xb50e8e);
      iVar9 = (uVar16 & 0xfff) * 0x10;
      ppCVar15 = (CTFPlayerShared **)&stack0xffffffb4;
      if (((*(uint *)(*local_38 + 8 + iVar9) == uVar16 >> 0xc) &&
          (piVar10 = *(int **)(*local_38 + 4 + iVar9),
          ppCVar15 = (CTFPlayerShared **)&stack0xffffffb4, piVar10 != (int *)0x0)) &&
         ((iVar9 = (**(code **)(*piVar10 + 0x50c))(),
          ppCVar15 = (CTFPlayerShared **)&stack0xffffffb4, iVar9 == 3 &&
          (cVar6 = (**(code **)(*(int *)this->field_0x190 + 0x108))(),
          ppCVar15 = (CTFPlayerShared **)&stack0xffffffb4, cVar6 != '\0')))) {
        (**(code **)(*(int *)this->field_0x190 + 0x20c))();
        piVar10 = (int *)0x0;
        if ((0 < *(int *)(*piVar7 + 0xca8)) &&
           (uVar16 = **(uint **)(*piVar7 + 0xc9c), local_34 = iVar14, uVar16 != 0xffffffff)) {
          iVar9 = (uVar16 & 0xfff) * 0x10;
          if (*(uint *)(*local_38 + 8 + iVar9) == uVar16 >> 0xc) {
            piVar10 = *(int **)(*local_38 + 4 + iVar9);
          }
        }
        pfVar11 = (float *)(**(code **)(*piVar10 + 600))();
        ppCVar15 = (CTFPlayerShared **)&stack0xffffffb0;
        if ((((local_1c - pfVar11[2]) * (local_1c - pfVar11[2]) +
              (local_20 - pfVar11[1]) * (local_20 - pfVar11[1]) +
              (local_24 - *pfVar11) * (local_24 - *pfVar11) < *(float *)(iVar14 + 0x906dfe)) &&
            (ppCVar15 = (CTFPlayerShared **)&stack0xffffffb0, 0 < *(int *)(*piVar7 + 0xca8))) &&
           (uVar16 = **(uint **)(*piVar7 + 0xc9c), ppCVar15 = (CTFPlayerShared **)&stack0xffffffb0,
           uVar16 != 0xffffffff)) {
          iVar9 = (uVar16 & 0xfff) * 0x10;
          ppCVar15 = (CTFPlayerShared **)&stack0xffffffb0;
          if ((*(uint *)(*local_38 + 8 + iVar9) == uVar16 >> 0xc) &&
             (this_01 = *(CMerasmus **)(*local_38 + 4 + iVar9),
             ppCVar15 = (CTFPlayerShared **)&stack0xffffffb0, this_01 != (CMerasmus *)0x0)) {
            CMerasmus::AddStun(this_01,(CTFPlayer *)this->field_0x190);
            ppCVar15 = (CTFPlayerShared **)&stack0xffffffb0;
          }
        }
      }
    }
  }
  if ((*(int *)(**(int **)(iVar14 + 0xb50eba) + 0xd9c) == 2) &&
     ((*(byte *)((int)&this->field_0xd8 + 1) & 0x10) != 0)) {
    iVar9 = this->field_0x190;
    ppcVar5 = *(code ***)(iVar9 + 0x2338);
    *ppCVar15 = (CTFPlayerShared *)(iVar9 + 0x2338);
    ppCVar15[-1] = (CTFPlayerShared *)0x2e37b5;
    fVar18 = (float10)(**ppcVar5)();
    local_28 = (float)fVar18;
    if (*(float *)(iVar9 + 0x2340) <= local_28 && local_28 != *(float *)(iVar9 + 0x2340)) {
      iVar9 = this->field_0x190;
      *(int *)(iVar9 + 0x2344) = *(int *)(iVar9 + 0x2344) + 1;
      ppcVar5 = *(code ***)(iVar9 + 0x2338);
      *ppCVar15 = (CTFPlayerShared *)(iVar9 + 0x2338);
      ppCVar15[-1] = (CTFPlayerShared *)0x2e37e3;
      fVar18 = (float10)(**ppcVar5)();
      local_2c = (float)fVar18;
      *(float *)(iVar9 + 0x2340) = local_2c + *(float *)(iVar14 + 0x8fc8b2);
      *(undefined4 *)(iVar9 + 0x233c) = 0x41200000;
    }
  }
  *ppCVar15 = (CTFPlayerShared *)local_30;
  ppCVar15[1] = (CTFPlayerShared *)((int)&mach_header_00000000.magic + 2);
  ppCVar15[-1] = (CTFPlayerShared *)0x2e3818;
  cVar6 = CTFConditionList::InCond((CTFConditionList *)*ppCVar15,(ETFCond)ppCVar15[1]);
  if ((((cVar6 != '\0') || ((*(byte *)&this->field_0xcc & 4) != 0)) &&
      (0.0 < (float)this->field_0x230)) &&
     ((float)this->field_0x230 < *(float *)(**(int **)(iVar14 + 0xb50e92) + 0xc))) {
    *ppCVar15 = this;
    ppCVar15[-1] = (CTFPlayerShared *)0x2e384b;
    CompleteDisguise(*ppCVar15);
  }
  return;
  */
}

// OFSTATUS: Incomplete
void COFPlayerShared::SharedThink( void )
{
	//uint uVar1;
	//undefined4 uVar2;
	//int iVar3;
  
	ConditionThink();
	//InvisibilityThink();
	/*if (0.0 < (float)this->field_0x4a8) {
	uVar1 = this->field_0x4ac;
	uVar2 = 0;
	if (uVar1 != 0xffffffff) {
		iVar3 = (uVar1 & 0xfff) * 0x10;
		if (*(uint *)(*(int *)PTR__g_pEntityList_00e3407c + 8 + iVar3) == uVar1 >> 0xc) {
		uVar2 = *(undefined4 *)(*(int *)PTR__g_pEntityList_00e3407c + 4 + iVar3);
		}
	}
	(**(code **)(*(int *)this->field_0x190 + 0x430))((int *)this->field_0x190,uVar2,0);
	}*/
}

/*
bool COFPlayerShared::InCond(ETFCond eCond) const
{
	char cVar1;
	uint *puVar2;

	if ((int)eCond < 32) //0x20)
	{
		cVar1 = CTFConditionList::InCond((CTFConditionList *)&this->field_0xe0, eCond);
		if (cVar1 != '\0')
		{
			return true;
		}
		puVar2 = &this->field_0xcc;
	}
	else
	{
		if ((int)eCond < 0x80)
		{
			if ((int)eCond < 0x60)
			{
				if ((int)eCond < 0x40)
				{
					puVar2 = &this->field_0xd0;
					eCond = eCond - 0x20;
				}
				else
				{
					puVar2 = &this->field_0xd4;
					eCond = eCond - 0x40;
				}
			}
			else
			{
				puVar2 = &this->field_0xd8;
				eCond = eCond - 0x60;
			}
		}
		else
		{
			puVar2 = &this->field_0xdc;
			eCond = eCond - 0x80;
		}
	}
	return (*puVar2 >> (eCond & 0x1f) & 1) != 0;
}
*/

//OFSTATUS: INCOMPLETE
// OFTODO: condition system is needed to even finish this
bool COFPlayerShared::CanAttack(int iFlags)
{
	return true;
	/*
	int *piVar1;
	char cVar2;
	int iVar3;
	int iVar4;
	bool bVar5;
	byte local_11;

	if (this[0x23dc] == (CTFPlayer)0x0)
	{
		bVar5 = true;
		if (this[0x1e54] == (CTFPlayer)0x0)
		{
			piVar1 = *(int **)PTR__g_pGameRules_00e340a8;
			if ((*(float *)(this + 0x1c78) < (*PTR__gpGlobals_00e34080)->curtime ||
				*(float *)(this + 0x1c78) == (*PTR__gpGlobals_00e34080)->curtime) ||
				(((byte)this[0x1a8c] & 1) != 0)) {
				local_11 = 1;
				cVar2 = CTFConditionList::InCond((CTFConditionList *)(this + 0x1a98), 4);

				if (cVar2 == '\0')
				{
					local_11 = ((byte)this[0x1a84] & 0x10) >> 4;
				}

				if (((param_1 & 1U) == 0) && (local_11 != 0))
				{
					return false;
				}
			}
			else
			{
				if ((param_1 & 1U) == 0)
				{
					return false;
				}
			}
			if (this[0x1c24] == (CTFPlayer)0x0)
			{
				cVar2 = CTFConditionList::InCond((CTFConditionList *)(this + 0x1a98), 7);
				if (cVar2 == '\0')
				{
					if (((byte)this[0x1a84] & 0x80) == 0)
					{
						cVar2 = CTFConditionList::InCond((CTFConditionList *)(this + 0x1a98), 0xe);
						if (cVar2 == '\0')
						{
							if (((byte)this[0x1a85] & 0x40) == 0)
							{
								if (piVar1[0xdd] == 5) 
								{
									iVar3 = (**(code **)(*piVar1 + 700))(piVar1);
									iVar4 = CBaseEntity::GetTeamNumber((CBaseEntity *)this);
									if (iVar3 != iVar4)
									{
										return false;
									}
								}
								if (((byte)this[0x1a8e] & 4) == 0)
								{
									iVar3 = CAttributeManager::AttribHookValue<int>
										(0, "no_attack", (CBaseEntity *)this, (CUtlVector *)0x0, true);
									bVar5 = iVar3 == 0;
								}
								else
								{
									bVar5 = false;
								}
							}
							else
							{
								bVar5 = false;
							}
						}
						else {
							bVar5 = false;
						}
					}
					else {
						bVar5 = false;
					}
				}
				else {
					bVar5 = false;
				}
			}
			else {
				bVar5 = false;
			}
		}
	}
	else {
		bVar5 = false;
	}
	return bVar5;
	*/
}

// OFSTATUS: COMPLETE
bool COFPlayerShared::IsAllowedToPickUpFlag() const
{
	// attributes stuff
	return m_bAllowedToPickUpFlag;
}

//=========================================================
// Non-COFPlayerShared Functions
//=========================================================

void COFPlayer::SetItem(COFItem *pItem)
{
	m_hItem = pItem;

	#ifdef GAME_DLL

	if (!pItem)
	{
		//RemoveGlowEffect();
	}
	else
	{
		//AddGlowEffect();

		if (pItem->GetItemID() == OF_ITEM_FLAG)
		{
			//RemoveInvisibility();
		}
	}

	#endif
}

bool COFPlayer::HasItem() const
{
	return (m_hItem != NULL);
}

COFItem *COFPlayer::GetItem() const
{
	return m_hItem;
}

COFWeaponBase *COFPlayer::GetActiveOFWeapon() const
{
	return dynamic_cast< COFWeaponBase* >(GetActiveWeapon());
}

COFWeaponBase *COFPlayer::Weapon_OwnsThisID(int iWeaponID) const
{
	for (int i = 0; i < MAX_WEAPONS; i++)
	{
		COFWeaponBase* pWeapon = dynamic_cast<COFWeaponBase*>(GetWeapon(i));
		if (pWeapon && pWeapon->GetWeaponID() == iWeaponID)
		{
			return pWeapon;
		}
	}
	
	return NULL;
}

void COFPlayer::SetOffHandWeapon(COFWeaponBase *pWeapon)
{
	m_hOffHandWeapon = pWeapon; // Server: 0x20c8, Client: 0x1c0c

	if (m_hOffHandWeapon)
	{
		m_hOffHandWeapon->Deploy();
	}
}

void COFPlayer::HolsterOffHandWeapon()
{
	if (m_hOffHandWeapon)
	{
		m_hOffHandWeapon->Holster(NULL);
	}
}

// this is called "TeamFortress_SetSpeed" but cmon, who wants to type all that out
// filled with conditions, attributes, and my guess econ weapons, also passtime
// ive placed "TeamFortress_CalculateMaxSpeed in here as everything else in TeamFortress_SetSpeed is the above
void COFPlayer::SetSpeedOF()
{
	float flMaxSpeed = 0.0;

	// this will never happen as the game will just crash before this can even execute
	//if (!OFGameRules()) SetMaxSpeed(0.0);

	if (!OFGameRules()) return;

	// field_0xb0c = m_afPhysicsFlags
	if (IsObserver())
	{
		if (GetObserverMode() == OBS_MODE_ROAMING)
		{
			SetMaxSpeed(GetPlayerClassData(OF_CLASS_SCOUT)->m_flMaxSpeed);
		}

		SetMaxSpeed(flMaxSpeed);
		return;
	}

	// CanPlayerMove is filled with matchmaking
	// instead just check if we've restarted the round, if so, ya get frozen
	if (m_Class.m_iClass == OF_CLASS_UNDEFINED || OFGameRules()->InRoundRestart())
	{
		SetMaxSpeed(1.0);
		return;
	}

	flMaxSpeed = GetPlayerClassData(m_Class.m_iClass)->m_flMaxSpeed;

	// OFTODO: conditions + spy disguising
	/*
	if (m_Shared.InCond(TF_COND_DISGUISED) && !m_Shared.InCond(TF_COND_STEALTHED))
	{
		flMaxSpeed = GetPlayerClassData(m_nDisguiseClass)->m_flMaxSpeed; // 0x1abc - 6584 = 260 = m_nDisguiseClass
	}

	// mvm checks here, heck you
	if (m_Shared.InCond(TF_COND_AIMING))
	{
		int iClass = (m_Class.m_iClass);

		switch (iClass)
		{
		// minigun aiming/firing
		case OF_CLASS_HEAVY:
			flMaxSpeed = 110.0;
			break;
		// sniper rifle scope
		case OF_CLASS_SNIPER:
			flMaxSpeed = 80.0;
			break;
		}
	}
	*/

	SetMaxSpeed(flMaxSpeed);
}

// why is g_TFClassViewVectors hardcoded along a vector? and when only the z would be the only thing you'd be changing????
// instead ill add it to the class data
Vector &COFPlayer::GetClassEyeHeight()
{
	static Vector vecClassHeight = Vector(0.0, 0.0, GetPlayerClassData(m_Class.m_iClass)->m_iEyeHeight);
	return vecClassHeight;
}

void COFPlayer::RemoveDisguise()
{
	//if (m_Shared.InCond(TF_COND_DISGUISING) || m_Shared.InCond(TF_COND_DISGUISED))
	//{
	//	m_Shared.RemoveDisguise();
	//}
}

// empty on purpose
void COFPlayer::SetAnimation(PLAYER_ANIM playerAnim)
{
}

#ifdef CLIENT_DLL
// OFSTATUS: Incomplete
void COFPlayerShared::OnPreDataChanged( void )
{
	m_Conds.OnPreDataChanged();

	SharedThink();
}

// OFSTATUS: Incomplete
void COFPlayerShared::OnDataChanged( void )
{
	m_Conds.OnDataChanged();
}
#endif