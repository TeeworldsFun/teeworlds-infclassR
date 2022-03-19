#include <game/server/entity.h>

class CLoveTrain : public CEntity
{
public:
	enum
	{
		NUM_SIDE = 12,
		NUM_PARTICLES = 12,
		NUM_IDS = NUM_SIDE + NUM_PARTICLES,
	};

public:
	CLoveTrain(CGameWorld* pGameWorld, vec2 Pos, float Radius, int Owner);
	virtual ~CLoveTrain();

	virtual void Snap(int SnappingClient);
	virtual void Reset();
	virtual void TickPaused();
	virtual void Tick();

	int GetOwner() const;
	float GetRadius() const;
	void SetRadius(float Radius);
private:
	int m_IDs[NUM_IDS];
	float m_Radius;

public:
	int m_StartTick;
	int m_Owner;
};