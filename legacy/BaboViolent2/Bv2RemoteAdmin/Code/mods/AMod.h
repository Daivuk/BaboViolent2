#ifndef AMOD_H_INCLUDED
#define AMOD_H_INCLUDED

#include "CFrmMain.h"
#include "CVector.h"


/*	use delayed commands if you need to send something that will trigger itself later
	example : when a new player joins, he's probably not able to receive a new chat message yet
	so you could queue with a 3 secondw delay "Hey! Welcome to my modded server"

	use function SendDelayedCommand instead of SendCommand to use this

*/

struct SDelayedCommand
{
	char			m_command[128];
	float			m_delay; //this is in seconds (1.0f = 1 second 0.5f = half a second etc)

	SDelayedCommand( const char * in_command, const float & in_delay )
	{
		wxString command = in_command;
		if( command.Len() > 127 )
		{
			command = command.Mid(0, 127);
		}
		m_delay = in_delay;
		sprintf( m_command, "%s", (char*)command.c_str() );
	}

	int Update( const float & in_delay )
	{
		m_delay -= in_delay;
		if( m_delay <= 0.0f )
		{
			// ask the mod to trigger this command!
			return 1;
		}

		// nothing to do
		return 0;
	}
};


//
//--- When you create a mod, inherit from this abstract class and implement the events you want
//

class AMod
{
private:

	std::vector<SDelayedCommand *>	m_delayedCommands;

protected:

	// keep a pointer on our main window for injecting commands!
	CFrmMain *	m_mainWindow;

	// keep id of red/blue flag holder
	int			m_redFlagHolderId;
	int			m_blueFlagHolderId;

public:

	//--- Override these functions in your mod to handle these events
	virtual void OnInit() {}
	virtual void OnUpdate( const float & in_delta ) {}
	virtual void OnConsoleLog( const char * in_log ) {}
	virtual void OnChatLog( const char * in_log, const int & in_playerId ) {}
	virtual void OnPlayerJoin( const int & in_playerId, const char * in_playerName, const char * in_playerIP ) {}
	virtual void OnPlayerDisconnect( const int & in_playerId ) {}
	virtual void OnPlayerCapturesBlueFlag( const int & in_playerId ) {}
	virtual void OnPlayerGrabsBlueFlag( const int & in_playerId ) {}
	virtual void OnPlayerDropsBlueFlag( const int & in_playerId ) {}
	virtual void OnPlayerCapturesRedFlag( const int & in_playerId ) {}
	virtual void OnPlayerGrabsRedFlag( const int & in_playerId ) {}
	virtual void OnPlayerDropsRedFlag( const int & in_playerId ) {}
	virtual void OnPlayerKill( const int & in_victimPlayerId, const int & in_killerPlayerId, const int & in_weaponId ) {}
	virtual void OnPlayerSpawn( const int & in_playerId, const int & in_teamId, const int & in_weaponId, const int & in_secondaryId, const CVector2f & in_postition ) {}
	virtual void OnPlayerJoinSpectators( const int & in_playerId ) {}
	virtual void OnPlayerJoinRedTeam( const int & in_playerId ) {}
	virtual void OnPlayerJoinBlueTeam( const int & in_playerId ) {}
	virtual void OnPlayerPosition( const int & in_playerId, const float & in_x, const float & in_y ){}

	// map related
	virtual void OnSpawnPointInfo( const int & in_spawnId, const float & in_x, const float & in_y ) {}
	virtual void OnBlueSpawnPointInfo( const int & in_spawnId, const float & in_x, const float & in_y ) {}
	virtual void OnRedSpawnPointInfo( const int & in_spawnId, const float & in_x, const float & in_y ) {}
	virtual void OnMapInfo( const char * in_mapName, const float & in_mapWidth, const float & in_mapHeight, const int & in_nbSpawn ) {}
	virtual void OnMapChange( const char * in_mapName ) {}


	//
	//--- A modder should never have to modify functions below, except if you know what you are doing :)
	//
	AMod()
	{
		m_redFlagHolderId = -1;
		m_blueFlagHolderId = -1;
	}

	virtual ~AMod()
	{
		// delete delayed commands if any
		for( unsigned int i=0; i<m_delayedCommands.size(); i++ )
		{
			delete m_delayedCommands[i];
		}
		m_delayedCommands.clear();

	}

	void SetMainWindow( CFrmMain * in_mainWindow )
	{
		m_mainWindow = in_mainWindow;

		// check list of player we already have
		wxListBox * lst = m_mainWindow->GetPlayerLst();
		if( lst )
		{
			for( unsigned int i=0; i<lst->GetCount(); i++ )
			{
				wxString str = lst->GetString(i);
				wxString player = str.BeforeFirst(']');
				player = player.Remove(0,1);

				wxString playerName = str.AfterFirst(']');
				wxString IP = str.AfterLast('[');
				IP = IP.Remove( IP.Len() - 2, IP.Len() - 1 );
				playerName = playerName.BeforeLast('[');
				playerName = playerName.Remove( playerName.Len() - 3, playerName.Len() - 1 );

				long id;
				player.ToLong(&id);

				OnPlayerJoin( (int)id, (const char*)playerName, (const char*)IP );
			}
		}

		// lets init the mod
		OnInit();
	}

	void SendDelayedCommand( char * in_command, const float & in_delay )
	{
		// add a delayed command to our vector
		m_delayedCommands.push_back( new SDelayedCommand(in_command,in_delay) );
	}

	void SendCommand( const char * in_command )
	{
		// make sure that, if we inject a command related to the flag, we update the flag holder id properly
		wxString command = in_command;
		if( command.Len() > 127 )
		{
			command = command.Mid(0, 127);
		}

		if( command.Find("redFlagReturn") != -1 )
		{
			m_redFlagHolderId = -1;
		}
		if( command.Find("blueFlagReturn") != -1 )
		{
			m_blueFlagHolderId = -1;
		}
		if( command.Find("redTeamScore") != -1 )
		{
			m_blueFlagHolderId = -1;
		}
		if( command.Find("blueTeamScore") != -1 )
		{
			m_redFlagHolderId = -1;
		}

		m_mainWindow->SendServerCommand( in_command );
	}

	void Update( const float & in_delay )
	{
		// update delayed commands
		for( unsigned int i=0; i<m_delayedCommands.size(); i++ )
		{
			if( m_delayedCommands[i]->Update( in_delay ) )
			{
				// trigger this command
				SendCommand( m_delayedCommands[i]->m_command );

				// delete it
				delete m_delayedCommands[i];
				m_delayedCommands.erase( m_delayedCommands.begin() + i );
				i--;
			}
		}

		// update the mod
		OnUpdate( in_delay );
	}

	void PlayerDisconnect( const int & in_playerId )
	{
		if( in_playerId == m_redFlagHolderId )
		{
			OnPlayerDropsRedFlag( in_playerId );
			m_redFlagHolderId = -1;
		}
		if( in_playerId == m_blueFlagHolderId )
		{
			OnPlayerDropsBlueFlag( in_playerId );
			m_blueFlagHolderId = -1;
		}
		OnPlayerDisconnect( in_playerId );
	}

	// this extracts outputs from a certain player
	void ExtractChatLog( const char * in_string, long in_playerId )
	{
		OnChatLog( in_string, (int)in_playerId );
	}

	// this extracts command comming from the console log
	void ExtractLog( const char * in_string )
	{
		// maybe some mods wanna do specific things with the strings
		OnConsoleLog( in_string );

		// this function will call callbacks function like "OnSomething" when a new string is printed in the log
		// wxString usage here to ease the pain
		wxString command = in_string;
		if( command.Find("spawned; ID:") != -1 )
		{
			// a player just spawned, extract the data
			int idIndex = command.Find("ID:");
			if( idIndex == -1 ) return;
            wxString pid = command.Mid(idIndex + 3, 2);
			long id;
			pid.ToLong(&id);

			int weaponIndex = command.Find("WeaponID:");
			if( weaponIndex == -1 ) return;
			wxString weaponStr = command.Mid(weaponIndex + 9, 2);
			long weaponId;
			weaponStr.ToLong( &weaponId );

			int secIndex = command.Find("SecondaryID:");
			if( secIndex == -1 ) return;
			wxString secStr = command.Mid(secIndex + 12, 2);
			long secId;
			secStr.ToLong( &secId );

			int posIndex = command.Find("Position:");
			if( posIndex == -1 ) return;
			wxString posStr = command.Mid(posIndex + 9, 18);
			wxString xStr = posStr.BeforeLast(',');
			wxString yStr = posStr.AfterLast(',');

			double x,y;
			xStr.ToDouble(&x);
			yStr.ToDouble(&y);


			int teamIndex = command.Find("teamID:");
			if( teamIndex == -1 ) return;
			wxString teamStr = command.Mid(teamIndex + 7, 2);
			long teamId;
			teamStr.ToLong( &teamId );
			
			CVector2f pos = CVector2f( (float)x, (float)y);

            OnPlayerSpawn( (int)id, (int)teamId, (int)weaponId, (int)secId, pos );
			return;
		}

		if( command.Find("killed player id:") != -1 )
		{
			// this is misleading because it is inversed on the server
			int idIndex = command.Find("Player id:");
			if( idIndex == -1 ) return;
            wxString pid = command.Mid(idIndex + 10, 2);
			long victimId;
			pid.ToLong(&victimId);

			int idIndex2 = command.Find("killed player id:");
			if( idIndex2 == -1 ) return;
            wxString pid2 = command.Mid(idIndex2 + 17, 2);
			long killerId;
			pid2.ToLong(&killerId);

			int idIndex3 = command.Find("with weapon id:");
			if( idIndex3 == -1 ) return;
            wxString pid3 = command.Mid(idIndex3 + 15, 2);
			long weaponId;
			pid3.ToLong(&weaponId);

			OnPlayerKill( (int) victimId, (int) killerId, (int) weaponId );

			// if the victim had a flag, make sure we drop it!
			if( m_redFlagHolderId == (int)victimId )
			{
				OnPlayerDropsRedFlag( m_redFlagHolderId );
				m_redFlagHolderId = -1;
			}

			if( m_blueFlagHolderId == (int)victimId )
			{
				OnPlayerDropsBlueFlag( m_blueFlagHolderId );
				m_blueFlagHolderId = -1;
			}
			return;
		}

		if( command.Find("took the blue flag ID:") != -1 )
		{
			wxString strId = command.AfterLast(':');
			long id;
			strId.ToLong( &id );
			m_blueFlagHolderId = (int)id;
			OnPlayerGrabsBlueFlag( (int)id );
			return;
		}

		if( command.Find("took the red flag ID:") != -1 )
		{
			wxString strId = command.AfterLast(':');
			long id;
			strId.ToLong( &id );
			m_redFlagHolderId = (int)id;
			OnPlayerGrabsRedFlag( (int)id );
			return;
		}

		if( command.Find("scores for the Blue team! ID:") != -1 )
		{
			wxString strId = command.AfterLast(':');
			long id;
			strId.ToLong( &id );
			m_redFlagHolderId = -1;
			OnPlayerCapturesRedFlag( (int)id );
			return;
		}

		if( command.Find("scores for the Red team! ID:") != -1 )
		{
			wxString strId = command.AfterLast(':');
			long id;
			strId.ToLong( &id );
			m_blueFlagHolderId = -1;
			OnPlayerCapturesBlueFlag( (int)id );
			return;
		}

		if( command.Find("Map Infos, name:") != -1 )
		{
			// extract the name and size of the map
			wxString mapName = command.AfterFirst(':');
			mapName = mapName.BeforeFirst(' ');
			wxString strNbSpawn = command.AfterLast(':');
			long nbSpawn;
			strNbSpawn.ToLong( &nbSpawn );


			wxString strX = command.AfterFirst(':');
			strX = strX.AfterFirst(':');
			strX = strX.BeforeFirst(' ');
			wxString strY = strX.AfterLast(',');
			strX = strX.BeforeFirst(',');
            
            double x,y;
			strX.ToDouble( &x );
			strY.ToDouble( &y );

			OnMapInfo( (const char*)mapName.c_str(), (float)x, (float)y, (int)nbSpawn );
			return;
		}

		if( command.Find("Map file Exist :") != -1 )
		{
			wxString map = command.AfterFirst(':');
			OnMapChange( (const char*)map.c_str() );
			return;
		}

		if( command.Find("joins red team ID:") != -1 )
		{
			wxString strId = command.AfterLast(':');
			long id;
			strId.ToLong( &id );

			OnPlayerJoinRedTeam( (int)id );
			return;
		}

		if( command.Find("joins blue team ID:") != -1 )
		{
			wxString strId = command.AfterLast(':');
			long id;
			strId.ToLong( &id );

			OnPlayerJoinBlueTeam( (int)id );
			return;
		}

		if( command.Find("goes spectator ID:") != -1 )
		{
			wxString strId = command.AfterLast(':');
			long id;
			strId.ToLong( &id );

			OnPlayerJoinSpectators( (int)id );
			return;
		}

		if( command.Find("Blue Spawn #") != -1 )
		{
			wxString strId = command.BeforeFirst(':');
			strId = strId.AfterFirst('#');
			long id;
			strId.ToLong( &id );

			wxString strX = command.AfterLast(':');
			wxString strY = strX.AfterLast(',');
			strX = strX.BeforeLast(',');

			double x,y;
			strX.ToDouble( &x );
			strY.ToDouble( &y );

			OnBlueSpawnPointInfo( (int)id, (float)x, (float)y );
			return;
		}

		if( command.Find("Red Spawn #") != -1 )
		{
			wxString strId = command.BeforeFirst(':');
			strId = strId.AfterFirst('#');
			long id;
			strId.ToLong( &id );

			wxString strX = command.AfterLast(':');
			wxString strY = strX.AfterLast(',');
			strX = strX.BeforeLast(',');

			double x,y;
			strX.ToDouble( &x );
			strY.ToDouble( &y );

			OnRedSpawnPointInfo( (int)id, (float)x, (float)y );
			return;
		}

		if( command.Find("Spawn ") != -1 )
		{
			// extract infos about this spawn point
			wxString strSpawnId = command.BeforeFirst(':');
			strSpawnId = strSpawnId.AfterFirst(' ');
			long spawnId;
			strSpawnId.ToLong( &spawnId );

			wxString strX = command.AfterLast(':');
			wxString strY = strX.AfterLast(',');
			strX = strX.BeforeFirst(',');

			double x,y;
			strX.ToDouble( &x );
			strY.ToDouble( &y );

			
			OnSpawnPointInfo( (int)spawnId, (float)x, (float)y );
			return;
		}
	}
};



#endif
