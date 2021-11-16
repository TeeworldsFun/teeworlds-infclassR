/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_FFS_POWER_H
#define GAME_SERVER_ENTITIES_FFS_POWER_H

#include <game/server/entity.h>
#include <base/tl/array.h>

class CFFSPower : public CEntity
{
public:
	CFFSPower(CGameWorld *pGameWorld, vec2 Pos, int Owner);
	virtual ~CFFSPower();

	virtual void Snap(int SnappingClient);
	virtual void Reset();
	virtual void TickPaused();
	void Explode();
	bool AddP();
	int GetNbP() { return m_nbP; }

private:
	int m_StartTick;
	array<int> m_IDP;
	int m_nbP;
	
public:
	float m_DetectionRadius;
	int m_Owner;
    int m_Damage;
};

#endif
