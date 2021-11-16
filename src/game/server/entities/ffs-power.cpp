/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/server/gamecontext.h>
#include <engine/shared/config.h>
#include "ffs-power.h"
#include "growingexplosion.h"

CFFSPower::CFFSPower(CGameWorld *pGameWorld, vec2 Pos, int Owner)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_FFS_POWER)
{
	m_Pos = Pos;
	GameWorld()->InsertEntity(this);
	m_DetectionRadius = 100.0f;
	m_StartTick = Server()->Tick();
	m_Owner = Owner;
	m_nbP = g_Config.m_InfFFSPowers;
    m_Damage = 0;
	
	m_IDP.set_size(g_Config.m_InfFFSPowers);
	for(int i=0; i<m_IDP.size(); i++)
	{
		m_IDP[i] = Server()->SnapNewID();
	}
}

CFFSPower::~CFFSPower()
{
	for(int i=0; i<m_IDP.size(); i++)
		Server()->SnapFreeID(m_IDP[i]);
}

void CFFSPower::Reset()
{
	GameServer()->m_World.DestroyEntity(this);
}

void CFFSPower::Explode()
{
	CCharacter *OwnerChar = GameServer()->GetPlayerChar(m_Owner);
	if(!OwnerChar)
		return;
		
	vec2 dir = normalize(OwnerChar->m_Pos - m_Pos);
	m_Damage += 2;
	if(m_Damage > g_Config.m_InfFFSPowers)
		m_Damage = g_Config.m_InfFFSPowers;

	float Factor = static_cast<float>(m_Damage)/g_Config.m_InfFFSPowers;

	GameServer()->CreateSound(m_Pos, SOUND_GRENADE_EXPLODE);
	GameServer()->CreateExplosion(m_Pos, m_Owner, WEAPON_HAMMER, false, TAKEDAMAGEMODE_SELFHARM);
    if(m_Damage > 1)
	{
		GameServer()->CreateSound(m_Pos, SOUND_NINJA_FIRE);
		new CGrowingExplosion(GameWorld(), m_Pos, vec2(0.0, -1.0), m_Owner, 20.0f * Factor, GROWINGEXPLOSIONEFFECT_FFS_POWER);
	}
	for(int i=0; i<6; i++)
	{
		float angle = static_cast<float>(i)*2.0*pi/6.0;
		vec2 expPos = m_Pos + vec2(90.0*cos(angle), 90.0*sin(angle));
		GameServer()->CreateExplosion(expPos, m_Owner, WEAPON_HAMMER, false, TAKEDAMAGEMODE_SELFHARM);
	}
	for(int i=0; i<12; i++)
	{
		float angle = static_cast<float>(i)*2.0*pi/12.0;
		vec2 expPos = vec2(180.0*cos(angle), 190.0*sin(angle));
		if(dot(expPos, dir) <= 0)
		{
			GameServer()->CreateExplosion(m_Pos + expPos, m_Owner, WEAPON_HAMMER, false, TAKEDAMAGEMODE_SELFHARM);
		}
	}
	
	m_nbP--;
	
	if(m_nbP == 0)
	{
		GameServer()->m_World.DestroyEntity(this);
	}
}

void CFFSPower::Snap(int SnappingClient)
{
	float time = (Server()->Tick()-m_StartTick)/(float)Server()->TickSpeed();
	float angle = fmodf(time*pi/2, 2.0f*pi);
	
	for(int i=0; i<m_nbP; i++)
	{
		if(NetworkClipped(SnappingClient))
			return;
		
		float shiftedAngle = angle + 2.0*pi*static_cast<float>(i)/static_cast<float>(m_IDP.size());
		
		CNetObj_Projectile *pProj = static_cast<CNetObj_Projectile *>(Server()->SnapNewItem(NETOBJTYPE_PROJECTILE, m_IDP[i], sizeof(CNetObj_Projectile)));
		pProj->m_X = (int)(m_Pos.x + m_DetectionRadius*cos(shiftedAngle));
		pProj->m_Y = (int)(m_Pos.y + m_DetectionRadius*sin(shiftedAngle));
		pProj->m_VelX = (int)(0.0f);
		pProj->m_VelY = (int)(0.0f);
		pProj->m_StartTick = Server()->Tick();
		pProj->m_Type = WEAPON_RIFLE;
	}
}

void CFFSPower::TickPaused()
{
	++m_StartTick;
}

bool CFFSPower::AddP()
{
	if(m_nbP < m_IDP.size())
	{
		m_nbP++;
		return true;
	}
	else return false;
}
