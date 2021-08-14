// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Custom implementation of the Condition system
// Author(s): KaydemonLP
//
#pragma once

// include your game's shared defs or wherever you defineed the conditions
#include "of_shareddefs.h"

// Define your main player classes here!
#ifdef GAME_DLL
#define CCondPlayer COFPlayer
#else
#define CCondPlayer C_OFPlayer
#endif

// HAS to be the same as your condition count!
#define COND_THINK_COUNT TF_COND_LAST

// Client specific.
#ifdef CLIENT_DLL
EXTERN_RECV_TABLE( DT_CondManager );
// Server specific.
#else
EXTERN_SEND_TABLE( DT_CondManager );
#endif

#define OF_MAX_COND_LENGHT 64
#define PERMANENT_LENGHT -1

// I'm really unsure of this is the best way of handling these
// But for now it seems to be that way \_(o~o)_/
// You can add custom stacking modes for specific conds if needed - Kay
enum EOFCondStackingMode
{
	OF_COND_STACK_NONE,
	OF_COND_STACK_ONE_PER_PROVIDER,
	OF_COND_STACK_LONGER_TIME,
	OF_COND_STACK_ALL,
};

class COFCondManager;

class COFCondition
{
public:
	DECLARE_EMBEDDED_NETWORKVAR();
	DECLARE_CLASS_NOBASE(COFCondition);

	// Constructors
	// Empty constructor needed for CUtlVector Functions - Kay
	COFCondition();
	COFCondition( ETFCond nCond, float flDuration, CCondPlayer *hOuter, int iSeed, EOFCondStackingMode nCanStack = OF_COND_STACK_NONE, CBaseEntity *hProvider = NULL, char *szVars = NULL );
	~COFCondition();

	ETFCond GetID() { return (ETFCond)m_iID.Get(); };

	void	SetDuration(float flDuration){ m_flDuration = flDuration; };
	float	GetRemainingDuration(){ return m_flDuration; };

	// Accessors for the var
	// Only use it if the var stores a single value - Kay
	int GetInt(void);
	bool GetBool(void);
	float GetFloat(void);

	// Returns the var string
	// May be used in conjuction with CCommand to create a list of vars ( See the Burning Cond as an example ) - Kay
	char *GetString(void);

	void SetVars(char *szVars){ Q_strncpy(m_iszVars.GetForModify(), szVars ? szVars : "", OF_MAX_COND_LENGHT); }
	void SetVars(const char *szVars){ Q_strncpy(m_iszVars.GetForModify(), szVars ? szVars : "", OF_MAX_COND_LENGHT); }

	CBaseEntity	*GetProvider(){ return m_hProvider; }
	void SetProvider(CBaseEntity *pProvider){ m_hProvider = pProvider; };
	CCondPlayer *GetOuter() { return m_hOuter; }
	int GetSeed() const { return m_iSeed; }
	EOFCondStackingMode CanStack(){ return m_nCanStack; }
	void Think(void);

	// Needed for CUtlVector's find function
	// This assumes the outer on both conditions is the same
	// If for god knows what reason we need to check through multiple players
	// && other.GetOuter() == m_hOuter should be added here - Kay
	bool operator==(COFCondition const & other) const {
		return other.GetSeed() == m_iSeed;
	}

public:
	COFCondManager *m_pManager;
// Scrapped atm, formerly assigned to a think function when the Cond was created
// Currently replaced by just referencing the func array itself - Kay
//private:
//	void(*pfnThink)(COFCondition *pCond);
private:
	CNetworkVar(int, m_iID); // Cond ID
	CNetworkVar(float, m_flDuration); // How much longer the cond is applied
	CNetworkVar(int, m_iSeed); // Cond seed to differentiate it from other instances
	CNetworkHandle(CBaseEntity, m_hProvider); // The Entity that applied this condition onto us
	CNetworkHandle(CCondPlayer, m_hOuter); // Us, the player, who is inflicted

	// TODO?: Perhaps create a custom networking prop
	// That sends and recieves a CCommand
	// So we don't need to manually tokenize it in the think function - Kay
	CNetworkString(m_iszVars, OF_MAX_COND_LENGHT);
	EOFCondStackingMode m_nCanStack;
};
// Has to be the same lenght as the condition count
// Imagine it similar to weapons and Damage Types
extern void (*g_fnCondThink[COND_THINK_COUNT])( COFCondition *pCond );

class COFCondManager
{
public:
	DECLARE_EMBEDDED_NETWORKVAR();
	DECLARE_CLASS_NOBASE(COFCondManager);

	void	RunThink( void );

	void	AddCond( ETFCond nCond, float flDuration = PERMANENT_LENGHT, EOFCondStackingMode nCanStack = OF_COND_STACK_NONE, CBaseEntity *hProvider = NULL, char *szVars = NULL );
	void	RemoveCond( ETFCond nCond, CBaseEntity *pProvider = NULL );
	void	RemoveCondSeed( int iSeed );

	COFCondition *GetCond( ETFCond nCond, CBaseEntity *pProvider = NULL, COFCondition *pPrevious = NULL );
	bool		InCond( ETFCond nCond );
	void		RemoveAllCond( CBaseEntity *pProvider = NULL );
	float		GetConditionDuration( ETFCond nCond );

	// OF-RE: Possibly move OnConditionAdded and OnConditionRemoved into here?

#ifdef CLIENT_DLL
	virtual void OnPreDataChanged( void );
	virtual void OnDataChanged( void );
#endif

private:
	CCondPlayer *m_pOuter; // Player who we manage
	CUtlVector<COFCondition> m_hPlayerCond;
	CNetworkVar(int, m_iCondSeed);
#ifdef CLIENT_DLL
	CUtlVector<COFCondition> m_hOldPlayerCond;
#endif
	friend class CCondPlayer;
	friend class COFPlayerShared;
};