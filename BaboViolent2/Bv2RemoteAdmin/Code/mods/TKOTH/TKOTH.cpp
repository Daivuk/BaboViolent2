#include "TKOTH.h"



CTKOTH::CTKOTH( const float & in_goalTime, const int & in_goalScore )
{
	m_goalTime = in_goalTime;
	m_goalScore = in_goalScore;
}

CTKOTH::~CTKOTH()
{
}


void CTKOTH::OnInit()
{
	m_zone.Reset();

	// send commands to server
	wxString command;
	command.Printf("set sv_winLimit %i", m_goalScore);
    SendCommand( (const char*)command.c_str() );

	command.Printf("listbluespawns");
	SendCommand( (const char*)command.c_str() );

	command.Printf("moveid -1 -1");
	SendCommand( (const char*)command.c_str() );

	command.Printf("sayall \x6 Team KOTH mod :\x2 Enabled");
	SendCommand( (const char*)command.c_str() );

	command.Printf("set sv_sendJoinMessage false");
	SendCommand( (const char*)command.c_str() );

	command.Printf("set sv_showKills true");
	SendCommand( (const char*)command.c_str() );

	command.Printf("set sv_gameType 2");
	SendCommand( (const char*)command.c_str() );

	command.Printf("set sv_timeToSpawn 1");
	SendCommand( (const char*)command.c_str() );

	command.Printf("set sv_forceRespawn true");
	SendCommand( (const char*)command.c_str() );

	command.Printf("set sv_autoBalanceTime 1");
	SendCommand( (const char*)command.c_str() );


	
	for( unsigned int i=0; i<m_players.size(); i++ )
	{
		m_players[i]->m_teamId = -1;
		m_players[i]->m_x = -999.0f;
		m_players[i]->m_y = -999.0f;
	}
	m_zonePlayers.clear();
	m_posTimer = 0;
	m_blueTimer = 0;
	m_redTimer = 0;
	m_blueWasCapturing = false;
	m_redWasCapturing = false;
	ResetWarnings( BLUE_TEAM_ID );
	ResetWarnings( RED_TEAM_ID );
}

void CTKOTH::OnUpdate( const float & in_delta )
{
	m_posTimer += in_delta;
	if( m_posTimer >= POSITION_REQUEST_INTERVAL )
	{
		m_posTimer = 0;
		SendCommand("allplayerpos");
	}

	unsigned int i,j;
	for( i=0; i<m_players.size(); i++ )
	{
		if( IsInTheZone( m_players[i] ) )
		{
			m_players[i]->m_timeInZone += in_delta;

			//make sure he's part of the zone vector
			bool inVector = false;
			for( j=0; j<m_zonePlayers.size(); j++ )
			{
				if( m_zonePlayers[j] == m_players[i] )
				{
					// the guy is in the vector already
					inVector = true;
					break;
				}
			}
			if( !inVector )
			{
				// add him
				m_zonePlayers.push_back( m_players[i] );

				// tell him he's now in the zone
				wxString command;
				command.Printf("Sayid %i \x6 You are on the HILL, hold it!!", m_players[i]->m_id);
				SendCommand((const char*)command.c_str());
			}
		}
		else
		{
			// make sure he's not part of the zone vector
			for( j=0; j<m_zonePlayers.size(); j++ )
			{
				if( m_zonePlayers[j] == m_players[i] )
				{
					// the guy is in the vector, remove it
					m_zonePlayers[j]->m_timeInZone = 0;
					m_zonePlayers.erase( m_zonePlayers.begin() + j );
					break;
				}
			}
		}
	}

	// now check if a team is actually capturing the zone
	bool blueCapturing = false;
	bool redCapturing = false;
	for( i=0; i<m_zonePlayers.size(); i++ )
	{
		if( m_zonePlayers[i]->m_teamId == BLUE_TEAM_ID )
		{
			blueCapturing = true;
		}
		else if(  m_zonePlayers[i]->m_teamId == RED_TEAM_ID )
		{
			redCapturing = true;
		}
	}

	if( blueCapturing && redCapturing )
	{
		// both teams are in the zone, do nothing
		m_blueWasCapturing = false;
		m_redWasCapturing = false;
	}
	else if( blueCapturing && !redCapturing )
	{
		// blue team is actually capturing it!
		m_blueTimer += in_delta;
		m_redTimer = 0;
		ResetWarnings( RED_TEAM_ID );

		m_redWasCapturing = false;
		if( !m_blueWasCapturing )
		{
			m_blueWasCapturing = true;
			wxString command;
			command.Printf("sayall \x1 Blue Team\x6 is holding the HILL!!");
			SendCommand((const char*)command.c_str());
		}

		// check if we need to emit a warning
		int remaningTime = (int)m_goalTime - (int)m_blueTimer;
		if( remaningTime <= 5 && remaningTime )
		{
			if( !m_blueWarnings[remaningTime] )
			{
				m_blueWarnings[remaningTime] = true;
				wxString command;
				command.Printf("sayall \x9 %i\x6 seconds before\x1 Blue Team\x6 scores!", remaningTime);
				SendCommand((const char*)command.c_str());
			}
		}
	}
	else if( !blueCapturing && redCapturing )
	{
		// red team is actually capturing it!
		m_redTimer += in_delta;
		m_blueTimer = 0;
		ResetWarnings( BLUE_TEAM_ID );

		m_blueWasCapturing = false;
		if( !m_redWasCapturing )
		{
			m_redWasCapturing = true;
			wxString command;
			command.Printf("sayall \x4 Red Team\x6 is holding the HILL!!");
			SendCommand((const char*)command.c_str());
		}

		// check if we need to emit a warning
		int remaningTime = (int)m_goalTime - (int)m_redTimer;
		if( remaningTime <= 5 && remaningTime )
		{
			if( !m_redWarnings[remaningTime] )
			{
				m_redWarnings[remaningTime] = true;
				wxString command;
				command.Printf("sayall \x9 %i\x6 seconds before\x4 Red Team\x6 scores!", remaningTime);
				SendCommand((const char*)command.c_str());
			}
		}
	}
	else
	{
		// no one in the zone
		m_blueWasCapturing = false;
		m_redWasCapturing = false;
		m_blueTimer = 0;
		m_redTimer = 0;
		ResetWarnings( BLUE_TEAM_ID );
		ResetWarnings( RED_TEAM_ID );
	}

	// check if a team has captured the room
	if( m_blueTimer >= m_goalTime )
	{
		// blue team scores!
		// get the player with the most zone time in blue team
		float maxZoneTime = -1;
		int maxId = -1;
		for( i=0; i<m_zonePlayers.size(); i++ )
		{
			if( m_zonePlayers[i]->m_timeInZone > maxZoneTime && m_zonePlayers[i]->m_teamId == BLUE_TEAM_ID )
			{
				maxZoneTime = m_zonePlayers[i]->m_timeInZone;
				maxId =  m_zonePlayers[i]->m_id;
			}
		}
		m_zonePlayers.clear();

		// send the scoring events
		wxString command;
		command.Printf("blueTeamScore %i", maxId);
		SendCommand((const char*)command.c_str());

		command.Printf("sayall \x1 BLUE TEAM SCORES!!!");
		SendCommand((const char*)command.c_str());

		command.Printf("sayall \x6 New round starting...", maxId);
		SendCommand((const char*)command.c_str());

		// force everyone to bench then respawn
		for( i=0; i<m_players.size(); i++ )
		{
			if( m_players[i]->m_teamId != -1 )
			{
				m_players[i]->m_x = -999;
				m_players[i]->m_y = -999;
				command.Printf("moveid %i %i", m_players[i]->m_teamId == 1 ? 0 : 1, m_players[i]->m_id);
				SendCommand((const char*)command.c_str());	
				command.Printf("moveid %i %i", m_players[i]->m_teamId, m_players[i]->m_id);
				SendCommand((const char*)command.c_str());	
			}
		}

		m_blueTimer = 0;
		m_redTimer = 0;
	}
	else if( m_redTimer >= m_goalTime )
	{
		// red team scores!
		// get the player with the most zone time in red team
		float maxZoneTime = -1;
		int maxId = -1;
		for( i=0; i<m_zonePlayers.size(); i++ )
		{
			if( m_zonePlayers[i]->m_timeInZone > maxZoneTime && m_zonePlayers[i]->m_teamId == RED_TEAM_ID )
			{
				maxZoneTime = m_zonePlayers[i]->m_timeInZone;
				maxId =  m_zonePlayers[i]->m_id;
			}
		}
		m_zonePlayers.clear();

		// send the scoring events
		wxString command;
		command.Printf("redTeamScore %i", maxId);
		SendCommand((const char*)command.c_str());

		command.Printf("sayall \x4 RED TEAM SCORES!!!");
		SendCommand((const char*)command.c_str());

		command.Printf("sayall \x6 New round starting...", maxId);
		SendCommand((const char*)command.c_str());

		// force everyone to bench then respawn
		for( i=0; i<m_players.size(); i++ )
		{
			if( m_players[i]->m_teamId != -1 )
			{
				m_players[i]->m_x = -999;
				m_players[i]->m_y = -999;
				command.Printf("moveid %i %i", m_players[i]->m_teamId == 1 ? 0 : 1, m_players[i]->m_id);
				SendCommand((const char*)command.c_str());	
				command.Printf("moveid %i %i", m_players[i]->m_teamId, m_players[i]->m_id);
				SendCommand((const char*)command.c_str());	
			}
		}

		m_blueTimer = 0;
		m_redTimer = 0;
	}
	

}

void CTKOTH::OnPlayerJoinSpectators( const int & in_playerId )
{
	for( unsigned int i=0; i<m_players.size(); i++ )
	{
		if( m_players[i]->m_id == in_playerId )
		{
			m_players[i]->m_teamId = -1;
			return;
		}
	}
}

void CTKOTH::ResetWarnings( const int & in_teamId )
{
	if( in_teamId == BLUE_TEAM_ID )
	{
		for( int i=0; i<6; i++ )
		{
			m_blueWarnings[i] = false;
		}
		return;
	}

	if( in_teamId == RED_TEAM_ID )
	{
		for( int i=0; i<6; i++ )
		{
			m_redWarnings[i] = false;
		}
		return;
	}
}

bool CTKOTH::IsInTheZone( SPlayer * in_player )
{
	if( in_player->m_x > m_zone.m_left - 0.5f )
	{
		if( in_player->m_x < m_zone.m_right + 0.5f )
		{
			if( in_player->m_y < m_zone.m_top + 0.5f )
			{
				if( in_player->m_y > m_zone.m_bottom - 0.5f )
				{
					return true;
				}
			}
		}
	}
	return false;
}

void CTKOTH::OnBlueSpawnPointInfo( const int & in_spawnId, const float & in_x, const float & in_y )
{
	// update our zone with this info
	if( in_x < m_zone.m_left )		m_zone.m_left = in_x;
	if( in_x > m_zone.m_right )		m_zone.m_right = in_x;

	if( in_y < m_zone.m_bottom )	m_zone.m_bottom = in_y;
	if( in_y > m_zone.m_top )		m_zone.m_top = in_y;
}

void CTKOTH::OnPlayerSpawn( const int & in_playerId, const int & in_teamId, const int & in_weaponId, const int & in_secondaryId, const CVector2f & in_postition )
{
	for( unsigned int i=0; i<m_players.size(); i++ )
	{
		if( m_players[i]->m_id == in_playerId )
		{
			m_players[i]->m_teamId = in_teamId;
			m_players[i]->m_timeInZone = 0;

			// just making sure he's not part of the zone
			for( unsigned int j=0; j<m_zonePlayers.size(); j++ )
			{
				if( m_zonePlayers[j]->m_id == in_playerId )
				{
					m_zonePlayers.erase( m_zonePlayers.begin() + j );
					return;
				}
			}
			return;
		}
	}
}


void CTKOTH::OnPlayerKill( const int & in_victimPlayerId, const int & in_killerPlayerId, const int & in_weaponId )
{
	// just making sure he's not part of the zone
	for( unsigned int j=0; j<m_zonePlayers.size(); j++ )
	{
		if( m_zonePlayers[j]->m_id == in_victimPlayerId )
		{
			m_zonePlayers[j]->m_x = -999;
			m_zonePlayers[j]->m_y = -999;
			m_zonePlayers.erase( m_zonePlayers.begin() + j );
			return;
		}
	}
}

void CTKOTH::OnPlayerDisconnect( const int & in_playerId )
{
	// just making sure he's not part of the zone
	unsigned int j;
	for( j=0; j<m_zonePlayers.size(); j++ )
	{
		if( m_zonePlayers[j]->m_id == in_playerId )
		{
			m_zonePlayers.erase( m_zonePlayers.begin() + j );
		}
	}

	// delete the player from the player array
	for( j=0; j<m_players.size(); j++ )
	{
		if( m_players[j]->m_id == in_playerId )
		{
			delete m_players[j];
			m_players.erase( m_players.begin() + j );
			return;
		}
	}
}

void CTKOTH::OnPlayerJoin( const int & in_playerId, const char * in_playerName, const char * in_playerIP )
{
	// create a new player
	m_players.push_back( new SPlayer(in_playerId) );
	m_players[m_players.size()-1]->m_playerName = in_playerName;

	// make sure he's not part of the zone
	unsigned int j;
	for( j=0; j<m_zonePlayers.size(); j++ )
	{
		if( m_zonePlayers[j]->m_id == in_playerId )
		{
			m_zonePlayers.erase( m_zonePlayers.begin() + j );
		}
	}

	// greet the new player in 2 seconds
	wxString command;
	command.Printf("Sayid %i \x6 Welcome\x4 %s\x6 to a Team KOTH server", in_playerId, in_playerName);
	SendDelayedCommand( (char*)command.c_str(), 2.0f );
	command.Printf("Sayid %i \x6 Your team needs to hold the HILL for\x9 %i seconds\x6 to score!", in_playerId, (int)m_goalTime);
	SendDelayedCommand( (char*)command.c_str(), 3.0f );
}

void CTKOTH::OnPlayerPosition( const int & in_playerId, const float & in_x, const float & in_y )
{
	unsigned int j;
	for( j=0; j<m_players.size(); j++ )
	{
		if( m_players[j]->m_id == in_playerId )
		{
			m_players[j]->m_x = in_x;
			m_players[j]->m_y = in_y;
			return;
		}
	}
}

void CTKOTH::OnMapChange( const char * in_mapName )
{
	OnInit();
}
