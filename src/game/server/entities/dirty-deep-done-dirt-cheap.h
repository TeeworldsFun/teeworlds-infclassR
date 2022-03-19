// DIRTY DEEPS DONE DIRT CHEAP
// D 4 C
#ifndef GAME_SERVER_ENTITIES_D4C_H
#define GAME_SERVER_ENTITIES_D4C_H

#include <game/server/entity.h>

class CD4C : public CEntity
{

public:
	CD4C(CGameWorld *pGameWorld, vec2 StartPos, vec2 EndPos);

	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);

private:
	vec2 m_StartPos;
	vec2 m_EndPos;
	bool m_LaserFired;

};

#endif
