#ifndef CTKOTH_H_INCLUDED
#define CTKOTH_H_INCLUDED

#include "AMod.h"


#define POSITION_REQUEST_INTERVAL	0.5f
#define BLUE_TEAM_ID	0
#define RED_TEAM_ID		1

// Struct to represent a player
struct SPlayer
{
public:

	int			m_id;
	float		m_x;
	float		m_y;
	int			m_teamId;
	wxString	m_playerName;
	float		m_timeInZone;
    
	SPlayer( const int & in_id )
	{
		m_id = in_id;
		m_x = -999.0f;
		m_y = -999.0f;
		m_teamId = -1;
		m_timeInZone = 0;
	}

	~SPlayer()
	{
	}
};

// struct to represent the zone (the hill)
struct SZone
{
public:

	float	m_top;
	float	m_bottom;
	float	m_left;
	float	m_right;

	SZone()
	{
		Reset();
	}
	~SZone(){}

	void Reset()
	{
		m_top = -999;
		m_bottom = 999;
		m_left = 999;
		m_right = -999;
	}
};


//
//--- Class representing the Team KOTH mod
//
class CTKOTH : public AMod
{
private:

	float	m_goalTime;
	int		m_goalScore;

	// keep vector of players
	std::vector<SPlayer *>	m_players;

	// keep vector of player in the zone
	std::vector<SPlayer *>	m_zonePlayers;

	// keep the zone
	SZone					m_zone;

	// timer before asking server for all player pos
	float	m_posTimer;

	// time blue/red team has been capturing the zone
	float	m_blueTimer;
	float	m_redTimer;

	// keep if we warned about a certain time
	bool	m_blueWarnings[6];
	bool	m_redWarnings[6];

	// keep if last frame blue/red team was captuing
	bool	m_blueWasCapturing;
	bool	m_redWasCapturing;

	// helper function to know if a player is in the zone
	bool	IsInTheZone( SPlayer * in_player );

	// hleper funciton to reset warnings
	void	ResetWarnings( const int & in_teamId );



public:

	CTKOTH( const float & in_goalTime, const int & in_goalScore );
	~CTKOTH();


	virtual void OnInit();
	virtual void OnUpdate( const float & in_delta );
	virtual void OnMapChange( const char * in_mapName );

	virtual void OnBlueSpawnPointInfo( const int & in_spawnId, const float & in_x, const float & in_y );
	virtual void OnPlayerSpawn( const int & in_playerId, const int & in_teamId, const int & in_weaponId, const int & in_secondaryId, const CVector2f & in_postition );
	virtual void OnPlayerKill( const int & in_victimPlayerId, const int & in_killerPlayerId, const int & in_weaponId );
	virtual void OnPlayerDisconnect( const int & in_playerId );
	virtual void OnPlayerJoin( const int & in_playerId, const char * in_playerName, const char * in_playerIP );
	virtual void OnPlayerPosition( const int & in_playerId, const float & in_x, const float & in_y );
	virtual void OnPlayerJoinSpectators( const int & in_playerId );

};


#endif

