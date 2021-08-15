// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Implementation of CTFPlayerShared
// Author(s): Cherry!
//

#pragma once

#include "networkvar.h"
#include "of_condition.h"

#ifdef CLIENT_DLL
	#define COFPlayer C_OFPlayer
#endif

#ifdef CLIENT_DLL
	EXTERN_RECV_TABLE(DT_OFPlayerShared);
#else
	EXTERN_SEND_TABLE(DT_OFPlayerShared);
#endif

class COFPlayer;

class COFPlayerShared
{
public:

#ifdef CLIENT_DLL
	friend class C_OFPlayer;
	typedef C_OFPlayer OuterClass;
	DECLARE_PREDICTABLE();
#else
	friend class COFPlayer;
	typedef COFPlayer OuterClass;
#endif

	DECLARE_EMBEDDED_NETWORKVAR();
	DECLARE_CLASS_NOBASE( COFPlayerShared );

	COFPlayerShared();
	~COFPlayerShared();

	void Init(OuterClass *pOuter);

	void OnConditionAdded(EOFCond nCond);
	void OnConditionRemoved(EOFCond nCond);

	void ConditionThink(void);

	void SharedThink(void);

	virtual float GetCritMult();
	virtual bool CanAttack(int iFlags);
	virtual void AddCond(EOFCond nCond, float flDuration = PERMANENT_LENGTH, EOFCondStackingMode nCanStack = OF_COND_STACK_NONE, CBaseEntity *hProvider = NULL, char *szVars = NULL);
	virtual void RemoveCond(EOFCond nCond, CBaseEntity *pProvider = NULL);
	virtual bool InCond(EOFCond eCond);
	virtual bool IsAllowedToPickUpFlag() const;
	virtual void SetPlayerState(int iState) { m_nPlayerState = iState; }
	virtual int GetPlayerState() { return m_nPlayerState; }
	virtual bool IsLoser();

#ifdef CLIENT_DLL
	virtual void OnPreDataChanged(void);
	virtual void OnDataChanged(void);
#endif

public:
	CNetworkVarEmbedded(COFCondManager, m_Conds);
private:

	OuterClass *m_pOuter;

	CNetworkVar(int, m_iCritMult); // field_0x324
	bool m_bAllowedToPickUpFlag;

	CNetworkVar(int, m_nPlayerState); // offset 200 = 0xC8
	//CNetworkArray(bool, m_nPlayerCond, OF_COND_LAST);
	//float m_flPlayerCondDuration[OF_COND_LAST];
};