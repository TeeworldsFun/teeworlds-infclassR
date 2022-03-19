/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/server/gamecontext.h>
#include "love-train.h"
#include "growingexplosion.h"

CLoveTrain::CLoveTrain(CGameWorld* pGameWorld, vec2 Pos, float Radius, int Owner)
	: CEntity(pGameWorld, CGameWorld::ENTTYPE_D4C)
{
	m_Pos = Pos;
	GameWorld()->InsertEntity(this);
	m_Radius = Radius;
	m_StartTick = Server()->Tick();
    m_Owner = Owner;

	for (int i = 0; i < NUM_IDS; i++)
	{
		m_IDs[i] = Server()->SnapNewID();
	}
}

CLoveTrain::~CLoveTrain()
{
	for (int i = 0; i < NUM_IDS; i++)
	{
		Server()->SnapFreeID(m_IDs[i]);
	}
}

void CLoveTrain::Reset()
{
	GameServer()->m_World.DestroyEntity(this);
}


float CLoveTrain::GetRadius() const
{
	return m_Radius;
}

void CLoveTrain::SetRadius(float Radius)
{
	m_Radius = Radius;
}

int CLoveTrain::GetOwner() const
{
    return m_Owner;
}
void CLoveTrain::Snap(int SnappingClient)
{
	float Radius = m_Radius;

	int NumSide = CLoveTrain::NUM_SIDE;
	//if(Server()->GetClientAntiPing(SnappingClient))
	//	NumSide = std::min(6, NumSide);

	float AngleStep = 2.0f * pi / NumSide;

	for (int i = 0; i < NumSide; i++)
	{
		vec2 PartPosStart = m_Pos + vec2(Radius * cos(AngleStep * i), Radius * sin(AngleStep * i));
		vec2 PartPosEnd = m_Pos + vec2(Radius * cos(AngleStep * (i + 1)), Radius * sin(AngleStep * (i + 1)));

		CNetObj_Laser * pObj = static_cast<CNetObj_Laser*>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_IDs[i], sizeof(CNetObj_Laser)));
		if (!pObj)
			return;

		pObj->m_X = (int)PartPosStart.x;
		pObj->m_Y = (int)PartPosStart.y;
		pObj->m_FromX = (int)PartPosEnd.x;
		pObj->m_FromY = (int)PartPosEnd.y;
		pObj->m_StartTick = Server()->Tick();
	}
}

void CLoveTrain::Tick()
{
	if (IsMarkedForDestroy()) return;

    // Find other players
	bool DouJang = false;
	int DetonatedBy = -1;
	for(CCharacter *p = (CCharacter*) GameWorld()->FindFirst(CGameWorld::ENTTYPE_CHARACTER); p; p = (CCharacter *)p->TypeNext())
	{
		if(p->IsHuman() || p->GetClass() == PLAYERCLASS_JOESTER) continue;

		float Len = distance(p->m_Pos, m_Pos);
		if(Len < GetRadius())
		{
			DouJang = true;
			CPlayer *pDetonatedBy = p->GetPlayer();
			if (pDetonatedBy)
				DetonatedBy = pDetonatedBy->GetCID();
			else
				DetonatedBy = m_Owner;
		}

        if(DetonatedBy != -1 && DetonatedBy != GetOwner())
        {
            p->GenerateFPos(DetonatedBy);
        }
        break;
	}
}

void CLoveTrain::TickPaused()
{
	++m_StartTick;
}