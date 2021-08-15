// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: "Cry some more!"
// Author(s): Cherry!
//

#include "cbase.h"
#include "of_shareddefs.h"
#include "of_weapon_minigun.h"
#include "in_buttons.h"
#include "soundenvelope.h"

#ifdef CLIENT_DLL
	#include "of_clientmode.h"
	#include "c_of_player.h"
#else
	#include "of_player.h"
#endif

#define OF_MINIGUN_SPINUP_TIME 0.75

// ----------------------------------------------------------------------------- //
// Data tables
// ----------------------------------------------------------------------------- //

IMPLEMENT_NETWORKCLASS_ALIASED(OFMinigun, DT_OFWeaponMinigun)

BEGIN_NETWORK_TABLE(COFMinigun, DT_OFWeaponMinigun)
#ifdef CLIENT_DLL
	RecvPropInt(RECVINFO(m_iWeaponState)),
	RecvPropBool(RECVINFO(m_bCritShot)),
#else
	SendPropInt(SENDINFO(m_iWeaponState), 4, SPROP_UNSIGNED | SPROP_CHANGES_OFTEN),
	SendPropBool(SENDINFO(m_bCritShot)),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(COFMinigun)
	DEFINE_FIELD(m_iWeaponState,FIELD_INTEGER),
END_PREDICTION_DATA()
#endif

#ifdef GAME_DLL
BEGIN_DATADESC(COFMinigun)
END_DATADESC()
#endif

LINK_ENTITY_TO_CLASS(tf_weapon_minigun, COFMinigun);

COFMinigun::COFMinigun()
{
	#ifdef CLIENT_DLL

	m_pCurrentSound = NULL;

	m_pBrassEffect = NULL;

	m_pMuzzleEffect = NULL;
	m_hMuzzleWeapon = NULL;
	m_iMuzzleAttachPoint = -1;

	#endif

	WeaponReset();
}

COFMinigun::~COFMinigun()
{
	WeaponReset();
}

void COFMinigun::PrimaryAttack()
{
	SharedAttack();
}

void COFMinigun::SecondaryAttack()
{
	// ???
	if (GetOFPlayerOwner())
	{
		SharedAttack();
	}
}

bool COFMinigun::Lower()
{
	if (OF_MINIGUNSTATE_SPINUP < m_iWeaponState)
	{
		SpinDown();
	}

	return BaseClass::Lower();
}

bool COFMinigun::CanHolster() const
{
	// attributes...
	// "mod_minigun_can_holster_while_spinning"

	COFPlayer *pPlayer = GetOFPlayerOwner();
	if (pPlayer)
	{
		// OFTODO: conditionsss
		if (pPlayer->m_Shared.InCond(OF_COND_MELEE_ONLY))
		{
			return true;
		}
	}

	//if ("mod_minigun_can_holster_while_spinning")

	if (m_iWeaponState > OF_MINIGUNSTATE_SPINUP) return false;

	// 1721 = ACT_MELEE_ALLCLASS_VM_STUN - this aint right
	if (GetActivity() == ACT_MP_ATTACK_STAND_POSTFIRE || GetActivity() == ACT_PRIMARY_ATTACK_STAND_POSTFIRE)
	{
		if (!IsViewModelSequenceFinished()) return false;
	}

	// else
	// if (m_iWeaponState == 1 || m_iWeaponState == 2)
	// return false

	return BaseClass::CanHolster();
}

void COFMinigun::HandleFireOnEmpty()
{
	if (m_iWeaponState == OF_MINIGUNSTATE_FIRE || m_iWeaponState == OF_MINIGUNSTATE_SPIN)
	{
		SetWeaponState(OF_MINIGUNSTATE_DRYFIRE);
		SendWeaponAnim(ACT_VM_SECONDARYATTACK);

		if (m_iWeaponMode == OF_WEAPON_MODE_SECONDARY)
		{
			SetWeaponState(OF_MINIGUNSTATE_SPIN);
		}
	}
}

void COFMinigun::SetWeaponState(MinigunState_t iState)
{
	if (m_iWeaponState != iState)
	{
		/* p sure this is just for the ammo usage when firing the huo long heater, that fire minigun
		if ((uVar6 < 5) && ((0x13U >> (uVar6 & 0x1f) & 1) != 0)) {
			fVar2 = (*PTR__gpGlobals_00e34080)->curtime;
			*(float *)(this + 0x8a4) = fVar2;
			*(float *)(this + 0x89c) = fVar2 + 0.5;
		}
		*/

		m_iWeaponState = iState;
	}
}

bool COFMinigun::SendWeaponAnim(int iActivity)
{
	#ifdef CLIENT_DLL

	if (iActivity == ACT_MP_ATTACK_STAND_PRIMARYFIRE || iActivity == ACT_MP_ATTACK_STAND_PREFIRE)
	{
		m_flBarrelSpinTarget = 20.0;
	}
	else if (iActivity == ACT_MP_ATTACK_STAND_POSTFIRE)
	{
		m_flBarrelSpinTarget = 0.0;
	}

	#endif

	if (iActivity == ACT_VM_PRIMARYATTACK)
	{
		if (GetActivity() == ACT_VM_PRIMARYATTACK) return true;

		return BaseClass::SendWeaponAnim(ACT_VM_PRIMARYATTACK);
	}

	return BaseClass::SendWeaponAnim(iActivity);
}

void COFMinigun::SharedAttack()
{
	COFPlayer *pPlayer = GetOFPlayerOwner();

	if (!pPlayer) return;

	if (!CanAttack())
	{
		WeaponIdle();
		return;
	}

	if (pPlayer->m_nButtons & IN_ATTACK)
	{
		m_iWeaponMode = OF_WEAPON_MODE_PRIMARY;
	}
	else if (pPlayer->m_nButtons & IN_ATTACK2)
	{
		m_iWeaponMode = OF_WEAPON_MODE_SECONDARY;
	}

	// 0x6a8 = 2152 = m_iWeaponState
	switch (m_iWeaponState)
	{
	default:
	case OF_MINIGUNSTATE_SPINUP:

		SpinUp(); // previously called WindUp, just renamed for consistancy

		// attributes
		//"mult_minigun_spinup_time"

		if (pPlayer->GetViewModel(0))
		{
			pPlayer->GetViewModel(0)->SetPlaybackRate(OF_MINIGUN_SPINUP_TIME); // divide by fVar17, the multiplier
		}

		if (pPlayer->GetViewModel(1))
		{
			pPlayer->GetViewModel(1)->SetPlaybackRate(OF_MINIGUN_SPINUP_TIME); // divide by fVar17, the multiplier
		}

		field_0x874 = -1.0;

		m_flNextSecondaryAttack = m_flNextPrimaryAttack = m_flTimeWeaponIdle = gpGlobals->curtime + OF_MINIGUN_SPINUP_TIME;

		pPlayer->DoAnimationEvent(PLAYERANIMEVENT_ATTACK_PRE);

		break;

	case OF_MINIGUNSTATE_BEGINFIRE:

		// ignore LAB_003c9882, it leads to ring of fire stuff aka the huo long heater
		if (gpGlobals->curtime >= m_flNextPrimaryAttack)
		{
			if (m_iWeaponMode == OF_WEAPON_MODE_SECONDARY)
			{
				SetWeaponState(OF_MINIGUNSTATE_SPIN);
			}
			else
			{
				SetWeaponState(OF_MINIGUNSTATE_FIRE);
			}

			#ifdef GAME_DLL

			if (m_iWeaponState == OF_MINIGUNSTATE_SPIN)
			{
				//pPlayer->SpeakWeaponFire(3);
			}
			else
			{
				//pPlayer->SpeakWeaponFire(4);
			}

			#endif

			//m_flTimeWeaponIdle = gpGlobals->curtime + 0.1;
			//m_flNextPrimaryAttack = gpGlobals->curtime + 0.1;
			//m_flNextSecondaryAttack = gpGlobals->curtime + 0.1;

			m_flNextSecondaryAttack = m_flNextPrimaryAttack = m_flTimeWeaponIdle = gpGlobals->curtime + 0.1;
		}

		break;

	case OF_MINIGUNSTATE_FIRE:

		if (m_iWeaponMode == OF_WEAPON_MODE_SECONDARY)
		{
			SetWeaponState(OF_MINIGUNSTATE_SPIN);
		}

		if (m_iWeaponState == OF_MINIGUNSTATE_SPIN)
		{
			#ifdef GAME_DLL

			//pPlayer->ClearWeaponFireScene();
			//pPlayer->SpeakWeaponFire(3);

			#endif

			m_flNextSecondaryAttack = m_flNextPrimaryAttack = m_flTimeWeaponIdle = gpGlobals->curtime + 0.1;
		}
		else if (pPlayer->GetAmmoCount(m_iPrimaryAmmoType) <= 0)
		{
			SetWeaponState(OF_MINIGUNSTATE_DRYFIRE);
		}
		else
		{
			if (field_0x874 < 0.0)
			{
				field_0x874 = gpGlobals->curtime;
			}

			#ifdef GAME_DLL

			if (field_0x870 < gpGlobals->curtime)
			{
				field_0x870 = gpGlobals->curtime + 5.0;
				pPlayer->SpeakConceptIfAllowed(MP_CONCEPT_MINIGUN_FIREWEAPON);
			}

			#endif

			BaseClass::PrimaryAttack();
			CalcIsAttackCritical();

			m_bCritShot = m_bAttackCritical;

			pPlayer->DoAnimationEvent(PLAYERANIMEVENT_ATTACK_PRIMARY);

			// attributes here be

			m_flTimeWeaponIdle = gpGlobals->curtime + 0.2;
		}

		break;

	case OF_MINIGUNSTATE_SPIN:

		field_0x874 = -1.0;

		if (m_iWeaponMode == OF_WEAPON_MODE_PRIMARY)
		{
			if (0 < pPlayer->GetAmmoCount(m_iPrimaryAmmoType))
			{
				//pPlayer->ClearWeaponFireScene();
				//pPlayer->SpeakWeaponFire(4);

				SetWeaponState(OF_MINIGUNSTATE_FIRE);
			}
			else
			{
				SetWeaponState(OF_MINIGUNSTATE_DRYFIRE);
			}
		}

		SendWeaponAnim(ACT_VM_SECONDARYATTACK);

		break;

	case OF_MINIGUNSTATE_DRYFIRE:

		field_0x874 = -1.0;
		m_flLastStartSpinupTime = -1.0;

		if (0 < pPlayer->GetAmmoCount(m_iPrimaryAmmoType))
		{
			SetWeaponState(OF_MINIGUNSTATE_FIRE);
		}
		else if (m_iWeaponMode == OF_WEAPON_MODE_SECONDARY)
		{
			SetWeaponState(OF_MINIGUNSTATE_SPIN);
		}

		SendWeaponAnim(ACT_VM_SECONDARYATTACK);

		break;
	}
}

#ifdef CLIENT_DLL
void COFMinigun::OnDataChanged(DataUpdateType_t updateType)
{
	if (m_iWeaponState == OF_MINIGUNSTATE_FIRE && !m_pBrassEffect)
	{
		StartBrassEffect();
	}
	else if (m_iWeaponState != OF_MINIGUNSTATE_FIRE && m_pBrassEffect)
	{
		StopBrassEffect();
	}
	
	if (m_iWeaponState == OF_MINIGUNSTATE_FIRE && !m_pMuzzleEffect)
	{
		StartMuzzleEffect();
	}
	else if (m_iWeaponState != OF_MINIGUNSTATE_FIRE && m_pMuzzleEffect)
	{
		StopMuzzleEffect();
	}

	BaseClass::OnDataChanged(updateType);
	WeaponSoundUpdate();
}

void COFMinigun::StartBrassEffect()
{
	StopBrassEffect();

	m_hBrassWeapon = GetWeaponForEffect();

	if (!m_hBrassWeapon) return;

	if (UsingViewModel() && !g_pClientMode->ShouldDrawViewModel()) return;

	if (m_iBrassAttachPoint < 1)
	{
		m_iBrassAttachPoint = m_hBrassWeapon->LookupAttachment("eject_brass");
	}

	if (m_iBrassAttachPoint > 0 && !m_pBrassEffect)
	{
		m_pBrassEffect = m_hBrassWeapon->ParticleProp()->Create("eject_minigunbrass", PATTACH_POINT_FOLLOW, m_iBrassAttachPoint);
	}
}

void COFMinigun::StopBrassEffect()
{
	if (m_hBrassWeapon && m_pBrassEffect)
	{
		m_hBrassWeapon->ParticleProp()->StopEmission(m_pBrassEffect);
		m_hBrassWeapon = NULL;
		m_pBrassEffect = NULL;
	}
}

void COFMinigun::StartMuzzleEffect()
{
	StopMuzzleEffect();

	m_hMuzzleWeapon = GetWeaponForEffect();

	if (!m_hMuzzleWeapon) return;

	if (UsingViewModel() && !g_pClientMode->ShouldDrawViewModel()) return;

	if (m_iMuzzleAttachPoint < 1)
	{
		m_iMuzzleAttachPoint = m_hMuzzleWeapon->LookupAttachment("muzzle");
	}

	if (m_iMuzzleAttachPoint > 0 && !m_pMuzzleEffect)
	{
		m_pMuzzleEffect = m_hMuzzleWeapon->ParticleProp()->Create("muzzle_minigun_constant", PATTACH_POINT_FOLLOW, m_iMuzzleAttachPoint);
	}
}

void COFMinigun::StopMuzzleEffect()
{
	if (m_hMuzzleWeapon && m_pMuzzleEffect)
	{
		m_hMuzzleWeapon->ParticleProp()->StopEmission(m_pMuzzleEffect);
		m_hMuzzleWeapon = NULL;
		m_pMuzzleEffect = NULL;
	}
}

void COFMinigun::ItemPreFrame()
{
	UpdateBarrelSpin();

	BaseClass::ItemPreFrame();
}

void COFMinigun::ItemPostFrame()
{
	m_flNextEmptySoundTime = gpGlobals->curtime + 1.0;

	BaseClass::ItemPostFrame();
}

void COFMinigun::SetDormant(bool bDormant)
{
	if (!IsCarriedByLocalPlayer())
	{
		if (!IsDormant() && bDormant && m_iWeaponState >= OF_MINIGUNSTATE_FIRE)
		{
			WeaponSoundUpdate();
		}

		if (!IsDormant() && bDormant && m_iWeaponState != OF_MINIGUNSTATE_SPINUP)
		{
			StopMuzzleEffect();
			StopBrassEffect();
		}
	}

	C_BaseEntity::SetDormant(bDormant);
}

void COFMinigun::WeaponSoundUpdate()
{
	int iSound = -1;
	switch (m_iWeaponState)
	{
	case OF_MINIGUNSTATE_SPINUP:

		// attribute "minigun_no_spin_sounds"

		if (0.0 < m_flBarrelSpinSpeed)
		{
			iSound = SPECIAL2;
			if (0.0 < m_flBarrelSpinTarget)
			{
				m_flBarrelSpinTarget = 0.0;
			}
		}

		break;

	case OF_MINIGUNSTATE_BEGINFIRE:

		iSound = SPECIAL1;

		break;

	case OF_MINIGUNSTATE_FIRE:

		iSound = !m_bCritShot ? WPN_DOUBLE : BURST;

		break;

	case OF_MINIGUNSTATE_SPIN:

		// attribute "minigun_no_spin_sounds"

		iSound = SPECIAL3;

		break;

	case OF_MINIGUNSTATE_DRYFIRE:

		iSound = EMPTY;

		break;
	}

	/* whatever this is, i think its useless as it leads to applyfiredelay, and that's filled with mannpower stuff
	fVar8 = (float10)(**(code **)(*(int *)this + 0x768))(this, 0x3f800000);
	fVar9 = 1.0;
	if ((float)fVar8 != 1.0) {
		fVar9 = 1.5 - (float)fVar8;
		if (fVar9 <= 0.0) {
			fVar9 = 0.0;
		}
		if (1.0 <= fVar9) {
			fVar9 = 1.0;
		}
		fVar9 = fVar9 * 40.0 + 80.0;
	}
	if (this[0xc80] != (C_TFMinigun)0x0) {
		fVar9 = fVar9 * 0.6060606;
	}
	*/

	if (m_iCurrentSound == iSound) return; // && 0xc81 == 0xc80

	if (m_pCurrentSound)
	{
		CSoundEnvelopeController::GetController().SoundDestroy(m_pCurrentSound);
		m_pCurrentSound = NULL;
	}

	m_iCurrentSound = iSound;

	if (iSound != -1)
	{
		DevMsg("COFMinigun: New sound created! ,m_iCurrentSound: %i , iSound: %i\n", m_iCurrentSound, iSound);
		const char *pszSound = GetShootSound(iSound);
		CLocalPlayerFilter filter;

		CSoundPatch *pSound = CSoundEnvelopeController::GetController().SoundCreate(filter, entindex(), pszSound);
		m_pCurrentSound = pSound;

		CSoundEnvelopeController::GetController().Play(pSound, 1.0, 100.0);
		CSoundEnvelopeController::GetController().SoundChangeVolume(pSound, 1.0, 0.1);
	}
}

CStudioHdr *COFMinigun::OnNewModel()
{
	CStudioHdr *hdr = BaseClass::OnNewModel();

	field_0xc40 = LookupBone("barrel");

	if (IsToolRecording())
	{
		m_flBarrelAngle = 0.0;
		m_flBarrelSpinSpeed = 0.0;
		m_flBarrelSpinTarget = 0.0;
	}

	return hdr;
}

void COFMinigun::StandardBlendingRules(CStudioHdr *hdr, Vector pos[], Quaternion q[], float currentTime, int boneMask)
{
	BaseClass::StandardBlendingRules(hdr, pos, q, currentTime, boneMask);

	if (field_0xc40 != -1)
	{
		UpdateBarrelSpin();

		AngleQuaternion(RadianEuler(0.0, 0.0, m_flBarrelAngle), q[field_0xc40]);
	}
}

void COFMinigun::UpdateBarrelSpin()
{
	if (m_flBarrelSpinSpeed != m_flBarrelSpinTarget)
	{
		m_flBarrelSpinSpeed = Approach(m_flBarrelSpinTarget, m_flBarrelSpinSpeed, 0.1);

		if (m_flBarrelSpinSpeed == 0.0)
		{
			WeaponSoundUpdate();
		}
	}

	m_flBarrelAngle += m_flBarrelSpinSpeed * gpGlobals->frametime;
}

#endif

void COFMinigun::SpinUp()
{
	COFPlayer *pPlayer = GetOFPlayerOwner();
	if (!pPlayer) return;

	SendWeaponAnim(ACT_MP_ATTACK_STAND_PREFIRE);
	SetWeaponState(OF_MINIGUNSTATE_BEGINFIRE);
	pPlayer->m_Shared.AddCond(OF_COND_AIMING);

	#ifdef CLIENT_DLL
		WeaponSoundUpdate();
	#else
		//this_00->field_0x21e8 = 0xbf800000; // OFTODO: uh, figure this out later
	#endif

	pPlayer->SetSpeedOF();

	if (m_flLastStartSpinupTime == -1.0)
	{
		m_flLastStartSpinupTime = gpGlobals->curtime;
	}
}

void COFMinigun::SpinDown()
{
	COFPlayer *pPlayer = GetOFPlayerOwner();
	if (!pPlayer) return;

	SendWeaponAnim(ACT_MP_ATTACK_STAND_POSTFIRE);
	SetWeaponState(OF_MINIGUNSTATE_SPINUP);
	pPlayer->m_Shared.RemoveCond(OF_COND_AIMING);

	#ifdef CLIENT_DLL
		WeaponSoundUpdate();
	#else
		//pPlayer->ClearWeaponFireScene();
	#endif

	m_flTimeWeaponIdle = gpGlobals->curtime + 2.0;
	pPlayer->SetSpeedOF();

	#ifdef CLIENT_DLL
		field_0xc3c = 0;
		// gameevent stuff here
	#endif

	m_flLastStartSpinupTime = -1.0;
}

void COFMinigun::WeaponIdle()
{
	if (m_flTimeWeaponIdle <= gpGlobals->curtime)
	{
		if (m_iWeaponState != OF_MINIGUNSTATE_SPINUP && GetOFPlayerOwner())
		{
			GetOFPlayerOwner()->DoAnimationEvent(PLAYERANIMEVENT_ATTACK_POST);

			SpinDown();
			return;
		}

		BaseClass::WeaponIdle();
		
		m_flTimeWeaponIdle = gpGlobals->curtime + 12.5;
	}
}

void COFMinigun::WeaponReset()
{
	BaseClass::WeaponReset();

	COFPlayer *pPlayer = GetOFPlayerOwner();
	if (pPlayer)
	{
		//pPlayer->m_Shared.RemoveCond(TF_COND_AIMING);
		pPlayer->SetSpeedOF();
		//pPlayer->ClearWeaponFireScene();
	}

	SetWeaponState(OF_MINIGUNSTATE_SPINUP);
	m_iWeaponMode = OF_WEAPON_MODE_PRIMARY;

	m_bCritShot = false;

	field_0x874 = -1.0;
	m_flLastStartSpinupTime = -1.0;
	field_0x870 = 0.0; // 0xc2c - client value

	#ifdef CLIENT_DLL

	m_flBarrelSpinSpeed = 0.0; // 0xc38
	m_flBarrelSpinTarget = 0.0; // 0xc3c
	m_flBarrelAngle = 0.0; // 0xc44

	if (m_pCurrentSound)
	{
		CSoundEnvelopeController::GetController().SoundDestroy(m_pCurrentSound);
		m_pCurrentSound = NULL;
	}

	m_iCurrentSound = -1;
	//*(undefined4 *)(this + 0xc50) = 0x3f800000; // unused

	StopMuzzleEffect();
	StopBrassEffect();

	#endif
}