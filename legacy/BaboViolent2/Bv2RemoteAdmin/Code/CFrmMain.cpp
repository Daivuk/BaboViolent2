#include "CFrmMain.h"
#include "baboNet.h"
#include "RemoteAdminPackets.h"
#include "FileIO.h"
#include "CString.h"

// Team KOTH Mod
#include "TKOTH.h"

#define TIME_DELTA 0.033f
#define LOG_FILENAME "log.txt"

CFrmMain::CFrmMain()
	: IFrmMain( NULL )
{
	Start( 33 );
	m_masterTimeout = 0;
	m_lastMessageTime = 0;
	m_nextMessageIndex = 0;

	statusBar->SetLabel( wxT("Not connected to any server") );

	m_mod = 0;
	m_connected = false;
	m_connTimeout = -1.0f;
	m_nbLogRows = 0;
	m_nbChatLogRows = 0;
	m_peerId = 0;
	m_requestingGames = false;
	m_master = new CMaster();
	m_master->SetLogger( lstLog );

	int rc = sqlite3_open("admin.db",&m_db);
	if(rc)
	{
		wxMessageBox( wxT("admin.db database not found, re-install this application might solve the issue."), wxT("Error"));
		m_db = 0;
	}

	chkBool->Hide();
	txtValue->Hide();

	FileIO fileIO( LOG_FILENAME, "w" );

	// get last inputed language slurs
	LoadFilters();

	// get last inputed messages
	LoadMessages();
}

void CFrmMain::LoadFilters()
{
	if( m_db )
	{
		//some infos to load the data
		char	*zErrMsg;		// holds error msg if any
		char	**azResult;		// contains the actual returned data
		int		nRow;			// number of record
		int		nColumn;		// number of column
		char	SQL[256];		// the query

		// Get filters previously entered
		sprintf( SQL,"Select Word From Filters" );
		sqlite3_get_table(m_db,SQL,&azResult,&nRow,&nColumn,&zErrMsg);

		if( nRow > 0 )
		{
			for( int i=1; i<nRow+1; i++ )
			{
				wxString word = azResult[i];
				m_lstFilters->AppendString( word );
			}
		}
		sqlite3_free_table( azResult );
	}
}

void CFrmMain::LoadMessages()
{
	if( m_db )
	{
		//some infos to load the data
		char	*zErrMsg;		// holds error msg if any
		char	**azResult;		// contains the actual returned data
		int		nRow;			// number of record
		int		nColumn;		// number of column
		char	SQL[256];		// the query

		// Get filters previously entered
		sprintf( SQL,"Select Message From Messages" );
		sqlite3_get_table(m_db,SQL,&azResult,&nRow,&nColumn,&zErrMsg);

		if( nRow > 0 )
		{
			for( int i=1; i<nRow+1; i++ )
			{
				wxString msg = azResult[i];
				m_lstSpam->AppendString( msg );
			}
		}
		sqlite3_free_table( azResult );
	}
}

CFrmMain::~CFrmMain()
{
	if( m_master )
	{
		delete m_master;
		m_master = 0;
	}

	unsigned int i;
	for (i=0;i<m_games.size();++i)
	{
		delete m_games[i];
	}
	m_games.clear();

	for (i=0;i<m_vars.size();++i)
	{
		delete m_vars[i];
	}
	m_vars.clear();

	for (i=0;i<m_users.size();++i)
	{
		delete m_users[i];
	}
	m_users.clear();

	for (i=0;i<m_warnings.size();++i)
	{
		delete m_warnings[i];
	}
	m_warnings.clear();

    if( m_db )
	{
		// save latest messages + filters
		char * SQL = new char[2048];
		sprintf(SQL,"Delete From Messages;");
		sqlite3_exec(m_db,SQL,0,0,0);
		sprintf(SQL,"Delete From Filters;");
		sqlite3_exec(m_db,SQL,0,0,0);

		int i;
		for( i=0; i<(int)m_lstFilters->GetCount(); i++ )
		{
			wxString filter = m_lstFilters->GetString( i );
			
			sprintf(SQL,"Insert into Filters Values('%s');", filter.c_str());
			sqlite3_exec(m_db,SQL,0,0,0);
		}

		for( i=0; i<(int)m_lstSpam->GetCount(); i++ )
		{
			wxString msg = m_lstSpam->GetString( i );
			
			sprintf(SQL,"Insert into Messages Values('%s');", msg .c_str());
			sqlite3_exec(m_db,SQL,0,0,0);
		}
		sqlite3_close(m_db);
		delete [] SQL;
	}

	if( m_mod )
	{
		delete m_mod;
	}

	Destroy();
}

// this is like the main Update of the program, called 30 times per second
void CFrmMain::Notify()
{
	// update p2p network module
	bool isNew;
	int result = bb_peerUpdate(TIME_DELTA, isNew);
	if (result == -1)
	{
		wxString id; id.Printf("%d", m_peerId);
		lstLog->AppendString(wxString("Error while updating p2p module for peerID ") + id);
		if( m_peerId > 0 )
		{
			bb_peerDelete(m_peerId,true);
			m_peerId = 0;
		}

		// reset p2p module
		bb_peerShutdown();

		// clear players, vars, and connection state
		lstPlayers->Clear();
		for (unsigned int i=0;i<m_vars.size();++i)
		{
			delete m_vars[i];
		}
		m_vars.clear();
		lstVars->Clear();

		lstLog->AppendString( L"Disconnected from server" );
		lstChatLog->AppendString( L"> Disconnected from server" );
		statusBar->SetLabel( "Not connected to any server" );
		m_connected = false;
		m_peerId = 0;
		m_connTimeout = -1.0f;
	}
	else if (isNew)
	{
		if (result != 0) bb_peerDelete(result);
	}

	
	// receive peers packet!
	if( m_peerId > 0 )
	{
		if( m_connected )
		{
			m_keepAliveTime -= TIME_DELTA;
			if( m_keepAliveTime < 0.0f )
			{
				m_keepAliveTime = 3.0f;
				bb_peerSend( m_peerId, 0, RA_KEEP_ALIVE, 0, true );
			}
		}

		ReceivePeerPackets();
	}

	// if a mod is present, update it
	if( m_mod )
	{
		m_mod->Update( TIME_DELTA );
	}

	// update master server
	if( m_master )
	{
		UpdateMaster( 0.033f );
	}

	// update connect button
	if( txtIP->GetValue().Len() > 5 && m_peerId == 0 && txtLogin->GetValue().Len() > 0 && txtPass->GetValue().Len() > 0 && m_connTimeout < 0.0f && !m_connected )
	{
		btnConnect->Enable();
	}
	else
	{
		btnConnect->Disable();
	}

	// update disconnect button
	if( m_connected )
	{
		btnDisconnect->Enable();
	}
	else
	{
		btnDisconnect->Disable();
		chkConsoleLog->SetValue( false );
	}



	// update connection timeout
	m_connTimeout -= TIME_DELTA;
	if( m_peerId > 0 && m_connTimeout < 0.0f && !m_connected )
	{
		// connection timed out
		lstLog->AppendString( wxString("Connection to server timed out") );
		bb_peerDelete( m_peerId, true );
		m_peerId = 0;
	}


	if( lstLog->GetCount() != m_nbLogRows )
	{
		if( chkScrollLog->GetValue() )
		{
			lstLog->ScrollLines( 99999 );
			m_nbLogRows = lstLog->GetCount();
		}
	}

	if( lstChatLog->GetCount() != m_nbChatLogRows )
	{
		if( chkAutoScroll->GetValue() )
		{
			lstChatLog->ScrollLines( 99999 );
			m_nbChatLogRows = lstChatLog->GetCount();
		}
	}

	// update warning kick/ban counts
	wxString temp = m_txtWarnings->GetValue();
	long kick;
	temp.ToLong(&kick);
	m_maxWarningsBeforeKick = (int)kick;
	temp = m_txtBans->GetValue();
	temp.ToLong(&kick);
	m_maxWarningsBeforeBan = (int)kick;

	// update warnings
	for( unsigned int i=0; i<m_warnings.size(); i++ )
	{
		if( m_warnings[i]->Update( TIME_DELTA ) )
		{
			// this warning has expired
			delete m_warnings[i];
			m_warnings.erase( m_warnings.begin() + i );
			i--;
		}
	}

	// update auto message
	if( m_connected )
	{
		if( m_lstSpam->GetCount() > 0 )
		{
			m_lastMessageTime += TIME_DELTA;
			wxString temp = m_txtSpamDelay->GetValue();
			double dbl;
			temp.ToDouble(&dbl);
			if( m_lastMessageTime >= (float)dbl )
			{
				m_lastMessageTime = 0;

				// emit next message if any
				wxString msg = m_lstSpam->GetString( m_nextMessageIndex );
				if( msg.Len() == 0 )
				{
					m_nextMessageIndex = 0;
					msg = m_lstSpam->GetString( m_nextMessageIndex );
				}
				
				wxString command;
				command.Printf("sayall %s",msg.c_str());

				command.Replace("^1","\x1");
				command.Replace("^2","\x2");
				command.Replace("^3","\x3");
				command.Replace("^4","\x4");
				command.Replace("^5","\x5");
				command.Replace("^6","\x6");
				command.Replace("^7","\x7");
				command.Replace("^8","\x8");
				command.Replace("^9","\x9");

				SendServerCommand( (const char*)command.c_str()) ;

				m_nextMessageIndex++;
			}
		}
	}


	// sleep a bit
	wxMilliSleep( 5 );
}

void CFrmMain::ReceivePeerPackets()
{
	char *buf=0;
	long peerID;
	int typeID;

	while( buf = bb_peerReceive( &peerID, &typeID ))
	{		
		switch( typeID )
		{
			case RA_LOGIN_REPLY:
			{
				net_ra_login_reply reply;
				memcpy( &reply , buf , sizeof(net_ra_login_reply));
				if( reply.result == 1 )
				{
					// logged in successfully!
					lstLog->AppendString("Remote login success!");
					wxString str;
					str.Printf("Connected to %s",txtIP->GetValue().c_str());
					statusBar->SetLabel( str );
					m_connected = true;
					m_keepAliveTime = 3.0f;

					// clear var list
					for( unsigned int i=0; i<m_vars.size(); i++ )
					{
						delete m_vars[i];
					}
					m_vars.clear();
					lstVars->Clear();
					// ask for var list
					bb_peerSend( m_peerId, 0, RA_VAR_LIST, 0, true );

					lstLog->AppendString( wxString("Receiving game vars from server...") );
				}
				else
				{
					// login failed
					lstLog->AppendString("Remote login failed");
					btnConnect->Enable();
					m_connected = false;
				}
				m_connTimeout = -1;
				return;
			}
			case RA_VAR:
			{
				// received a var from the remote server!
				net_ra_var var;
				memcpy( &var , buf , sizeof(net_ra_var));

				AddNewVar( var.var );
				return;
			}
			case RA_VAR_END:
			{
				// we received everything
				wxString log;
				log.Printf( "Received %i vars from the server",m_vars.size() );
				lstLog->AppendString( log );

				// now add them to the list box
				for( unsigned int i=0; i<m_vars.size(); i++ )
				{
					lstVars->AppendString( m_vars[i]->name );
				}

				chkBool->Hide();
				txtValue->Hide();

				// ask server to send player list
				lstLog->AppendString( L"Getting player list..." );
				bb_peerSend( m_peerId, 0, RA_PLAYER_LIST, 0, true );
				lstPlayers->Clear();

				// ask server to send available maps on the server
				lstLog->AppendString( L"Getting map list.." );
				bb_peerSend( m_peerId, 0, RA_MAP_LIST_REQUEST, 0, true );
				bb_peerSend( m_peerId, 0, RA_MAP_ROTATION_REQUEST, 0, true );
				cboChangeMap->Clear();
				cboAddMap->Clear();
				cboRemoveMap->Clear();

				return;
			}
			case RA_PLAYER_ENTRY:
			{
				net_ra_player_entry entry;
				memcpy( &entry , buf , sizeof(net_ra_player_entry));

				wxString strID;
				if( entry.id < 10 )
				{
					strID.Printf("[0%i]", entry.id );
				}
				else
				{
					strID.Printf("[%i]", entry.id );
				}

				wxString playerStr;
				playerStr.Printf("%s %s - [%s]",strID, entry.name,entry.ip);
				lstPlayers->AppendString( playerStr );

				wxString log;
				log.Printf("> Added player %s to chat",entry.name);
				lstChatLog->AppendString( log );

				AddUser( entry.name, entry.ip, entry.id );
				return;
			}
			case RA_NEW_PLAYER:
			{
				net_ra_new_player player;
				memcpy( &player , buf , sizeof(net_ra_new_player));

				
				wxString strID;
				if( player.id < 10 )
				{
					strID.Printf("[0%i]", player.id );
				}
				else
				{
					strID.Printf("[%i]", player.id );
				}

				wxString playerStr;
				playerStr.Printf("%s %s - [%s]",strID, player.name,player.ip);
				lstPlayers->AppendString( playerStr );

				wxString log;
				log.Printf("> Player %s joined the game",player.name);
				lstChatLog->AppendString( log );

				// if a mod is present tell him about the new player
				if( m_mod )
				{
					m_mod->OnPlayerJoin( (int)player.id, player.name, player.ip );
				}
				AddUser( player.name, player.ip, player.id );
				return;
			}
			case RA_PLAYER_DISCONNECTED:
			{
				net_ra_player_disconnected player;
				memcpy( &player , buf , sizeof(net_ra_player_disconnected));

				long playerID = player.id;
				char strID[3];
				if( playerID < 10 )
				{
					strID[0] = '0';
					sprintf( &(strID[1]),"%i",playerID );
				}
				else
				{
					sprintf( strID,"%i", playerID );
				}
				wxString ID;
				ID.Printf("[%s]",strID);
				
				int found = -1;
				for( unsigned int i=0; i<lstPlayers->GetCount(); i++ )
				{
					wxString p = lstPlayers->GetString( i );
					if( p.Find( ID ) != -1 )
					{
						wxString playerName = p.Remove(0, 4);
						wxString log;
						log.Printf("> Player %s disconnected",playerName.c_str());
						lstChatLog->AppendString( log );

						found = i;
						break;
					}
				}

				if( found != -1 )
				{
					lstPlayers->Delete( found );
				}

				// if a mod is present tell him about the player disconnect
				if( m_mod )
				{
					m_mod->PlayerDisconnect( (int)player.id );
				}

				RemoveUser( player.id );

				return;
			}
			case RA_CHAT:
			{
				net_ra_chat chat;
				memcpy( &chat , buf , sizeof(net_ra_chat));
				chat.message[127] = '\0';

				wxString log;
				log.Printf("%s",chat.message);
				lstChatLog->AppendString( log );

				// filter the chat message to issue warnings/kick/ban if word filter has any entry
				FilterMessage( (const char*)log.c_str(), chat.id );

				// permit the mod to extract the chat messages
				if( m_mod )
				{
					m_mod->ExtractChatLog( (const char*)log.c_str(), chat.id );
				}

				return;
			}
			case RA_CONSOLE_BROADCAST:
			{
				net_ra_console_broadcast bc;
				memcpy( &bc , buf , sizeof(net_ra_console_broadcast));

				bc.message[127] = '\0';
				wxString str = bc.message;
				lstLog->AppendString( str );

				// permit the mod to extract the chat messages
				if( m_mod )
				{
					m_mod->ExtractLog( (const char*)str.c_str() );
				}

				if( chkDumpLog->GetValue() == true )
				{
					// append to text file
					FileIO fileIO( LOG_FILENAME, "a" );
					if (fileIO.isValid())
					{
						// remove colors before logging consoel output to file
						int len = (int)str.Len();
						for (int i=0;i<len;++i)
						{
							if (str[i] >= '\x1' && str[i] <= '\x9')
							{
								str.Remove(i);
								--i;
								--len;
							}
						}
						fileIO.putLine(CString((char*)str.c_str()));
					}
				}

				return;
			}
			case RA_MAP_LIST_ENTRY:
			{
				net_ra_map_entry map;
				memcpy( &map , buf , sizeof(net_ra_map_entry));

				wxString mapName = map.mapName;
				cboChangeMap->AppendString( mapName );
				cboAddMap->AppendString( mapName );
				//cboRemoveMap->AppendString( mapName );

				return;
			}
			case RA_MAP_ROTATION_ENTRY:
			{
				net_ra_map_rotation_entry map;
				memcpy( &map , buf , sizeof(net_ra_map_rotation_entry));

				wxString mapName = map.mapName;
				cboRemoveMap->AppendString( mapName );

				// find the map name in cboAddMap and remove it
				for( unsigned int i=0; i<cboAddMap->GetCount(); i++ )
				{
					if( cboAddMap->GetString(i) == mapName )
					{
						cboAddMap->Delete(i);
						return;
					}
				}
				return;
			}
			case RA_PLAYER_POSITION:
			{
				// receving a player specific position
				net_ra_player_position pos;
				memcpy( &pos , buf , sizeof(net_ra_player_position));

				if( m_mod )
				{
					m_mod->OnPlayerPosition( (int)pos.playerId, (float)((float)pos.x / 100.0f), (float)((float)pos.y / 100.0f) );
				}

				return;
			}
		}
	}
}

void CFrmMain::FilterMessage( const char * in_message, const long in_playerId )
{
	wxString msg = in_message;
	msg.UpperCase();
	msg.Replace(" ", "");

	for( int i=0; i<(int)m_lstFilters->GetCount(); i++ )
	{
		wxString filter = m_lstFilters->GetString( i );
		filter.UpperCase();
		filter.Replace(" ", "");

		// check if it exists
		if( msg.Find( (const char*)filter.c_str() ) != -1 )
		{
			// issue a warning
			SUser * user = GetUserById( (int)in_playerId );
			if( user )
			{
				AddWarning( (const char*)user->m_ip.c_str() );
				return;
			}
		}
	}


}

void CFrmMain::AddUser( const char * in_name, const char * in_ip, const long & in_id )
{
	SUser * newUser = new SUser( in_name, in_ip, in_id );
	m_users.push_back( newUser );
}

void CFrmMain::RemoveUser( const long & in_id )
{
	for( unsigned int i=0; i<m_users.size(); i++ )
	{
		if( m_users[i]->m_id == in_id )
		{
			delete m_users[i];
			m_users.erase( m_users.begin() + i );
			return;
		}
	}
}

void CFrmMain::AddWarning( const char * in_ip )
{
	// check if a warning existed for that user
	SWarning * warning = GetWarningByIp( in_ip );
	if( !warning )
	{
		// no warning issued yet for this guy
		warning = new SWarning( in_ip );
		m_warnings.push_back( warning );
	}

	// this user already been warned, increment warn count
	warning->m_warningCount++;

	SUser * user = GetUserByIp( in_ip );

	// check if we need to kick/ban him
	if( warning->m_warningCount == m_maxWarningsBeforeKick + 1 )
	{
		// kick the player
		if( user )
		{
			wxString command;
			command.Printf("kickid %i", (int)user->m_id );
			SendServerCommand( (const char*)command.c_str() );

			command.Printf("sayall \x6 #Kicked player\x4 %s\x6 for innapropriate language/nickname#", user->m_name );
			SendServerCommand( (const char*)command.c_str() );	
		}


		return;
	}

	if( warning->m_warningCount == m_maxWarningsBeforeBan + 1 )
	{
		// ban the player
		if( user )
		{
			wxString command;
			command.Printf("banid %i", (int)user->m_id );
			SendServerCommand( (const char*)command.c_str() );

			command.Printf("sayall \x6 #Banned player\x4 %s\x6 for continuous innapropriate language/nickname#", user->m_name );
			SendServerCommand( (const char*)command.c_str() );	
		}

		return;
	}

	if( user )
	{
		wxString command;
		command.Printf("sayall \x6 #Issued Warning to player\x4 %s\x6 for innapropriate language/nickname#", user->m_name );
		SendServerCommand( (const char*)command.c_str() );	
	}
}

SWarning * CFrmMain::GetWarningByIp( const char * in_ip )
{
	wxString inIp = in_ip;
	for( unsigned int i=0; i<m_warnings.size(); i++ )
	{
		if( inIp == m_warnings[i]->m_playerIP )
		{
			return m_warnings[i];
		}
	}
	return 0;
}

// helper function to Retreive a user by id
SUser * CFrmMain::GetUserById( const int & in_id )
{
	for( unsigned int i=0; i<m_users.size(); i++ )
	{
		if( in_id == m_users[i]->m_id )
		{
			return m_users[i];
		}
	}
	return 0;
}

// helper function to Retreive a user by ip
SUser * CFrmMain::GetUserByIp( const char * in_ip )
{
	wxString inIp = in_ip;
	for( unsigned int i=0; i<m_users.size(); i++ )
	{
		if( inIp == m_users[i]->m_ip )
		{
			return m_users[i];
		}
	}
	return 0;
}


void CFrmMain::AddNewVar( char * in_varName )
{
	// extract name + value
	wxString name = in_varName;
	int space = name.Find(' ');
	name.Remove(space);

	wxString value = in_varName;
	value = value.AfterFirst(' ');

	//lstLog->AppendString( name + value );
	
	wxString desc = "";

	// get var infos from the db
	if( m_db )
	{
	//some infos to load the data
		char	*zErrMsg;		// holds error msg if any
		char	**azResult;		// contains the actual returned data
		int		nRow;			// number of record
		int		nColumn;		// number of column
		char	SQL[256];		// the query

		// Get infos of master servers and choose the one with the lowest Score
		sprintf( SQL,"Select Description From ServerVars where Name = '%s';", name.c_str() );
		sqlite3_get_table(m_db,SQL,&azResult,&nRow,&nColumn,&zErrMsg);

		if( nRow > 0 )
		{
			desc = azResult[1];
		}
		sqlite3_free_table( azResult );
	}

	if( desc.Len() < 1 )
	{
		desc = "No description entry for this var yet";
	}

	// add a server var!
	m_vars.push_back( new SServerVar( name.c_str(), value.c_str(), desc.c_str() ) );

}

void CFrmMain::UpdateMaster( const float & in_delay )
{
	if( m_master->IsValid() )
	{
		m_master->Update( in_delay );

		if( m_master->IsConnected() )
		{
			if( m_requestingGames )
			{
				btnRefresh->Disable();
				if( m_master->GetNbGameFound() > -1 && (int)m_games.size() == m_master->GetNbGameFound() )
				{
					m_requestingGames = false;
					m_master->DisconnectMaster();

					// for stats, log total number of players
					int nbPlayers = 0;
					for( unsigned int j=0; j<m_games.size(); j++ )
					{
						nbPlayers += m_games[j]->nbPlayer;
					}
					wxString log;
					log.Printf("Total number of player : %i", nbPlayers);
					lstLog->AppendString( log );

					lstGames->Clear();
					for( unsigned int i=0; i<m_games.size(); i++ )
					{
						wxString newGame = m_games[i]->serverName;

						for (int z=0;z<(int)newGame.Len();++z)
						{
							if ((unsigned char)newGame[z] < '\x10' && (unsigned char)newGame[z] != '\n')
							{
								newGame.Remove(z,1);
								--z;
							}
						}
						lstGames->AppendString( newGame );
					}
				}
			}
		}
		else
		{
			if( m_masterTimeout < 0.0f )
			{
				btnRefresh->SetLabel( "Refresh Server List" );
				btnRefresh->Enable();
			}
			else
			{
				m_masterTimeout -= in_delay;
				wxString lbl;
				lbl.Printf("Refresh Server List - %i", (int)m_masterTimeout);
				btnRefresh->SetLabel( lbl );
			}
		}
	}
	else
	{
		delete m_master;
		m_master = new CMaster();
		m_master->SetLogger( lstLog );
		btnRefresh->SetLabel( "Refresh Server List" );
		btnRefresh->Enable();
		m_masterTimeout = 0.0f;
	}
}

void CFrmMain::On_btnConnectClick( wxCommandEvent& event )
{
	if( m_peerId != 0 ) return;

	wxString str;
	str.Printf("Trying to connect to %s:%s",txtIP->GetValue(),txtPort->GetValue());
	lstLog->AppendString(str);
	
	long port=0;
	if( !txtPort->GetValue().ToLong( &port ) )
	{
		lstLog->AppendString("Invalid port specified, connection aborted");
		return;
	}
	
	// let's send the login packet to the specified ip/port
	net_ra_login loginInfos;
	sprintf( loginInfos.login, "%s", txtLogin->GetValue().c_str() );
	sprintf( loginInfos.pass, "%s", txtPass->GetValue().c_str() );
	
	wxString ipTxt = txtIP->GetValue();
	char ip[16];
	sprintf( ip,"%s",ipTxt.c_str() );
	m_peerId = bb_peerSend( ip, (short)port, (char*)&loginInfos, RA_LOGIN, sizeof(net_ra_login), true );

	// check for error
	if( m_peerId == -1 )
	{
		str = "An error occured while trying to connect to server";
		lstLog->AppendString( str );
		return;
	}
	btnConnect->Disable();
	m_connTimeout = 10.0f;
}

void CFrmMain::On_btnRefreshGames_Click( wxCommandEvent& event )
{
	// ask master for game list
	for (int i=0;i<(int)m_games.size();++i)
	{
		delete m_games[i];
	}
	m_games.clear();
	lstGames->Clear();

	m_master->RequestGames( &m_games );
	m_requestingGames = true;
	m_masterTimeout = 10.0f;

	txtLogin->SetLabel("");
	txtIP->SetLabel("");
	txtPort->SetLabel("");
	txtPass->SetLabel("");
}

void CFrmMain::On_lstGames_Selected( wxCommandEvent& event )
{
	lstInfos->Clear();

	// get data of selected game
	wxString game = lstGames->GetStringSelection();

	// find the right data
	for( unsigned int i=0; i<m_games.size(); i++ )
	{
		wxString svName = m_games[i]->serverName;
		for (int z=0;z<(int)svName.Len();++z)
		{
			if ((unsigned char)svName[z] < '\x10' && (unsigned char)svName[z] != '\n')
			{
				svName.Remove(z,1);
				--z;
			}
		}

		if( game == svName )
		{
			// we found it
			wxString str;
			str.Printf( "%s", m_games[i]->ip );
			txtIP->SetValue( str );

			str.Printf( "%i", m_games[i]->port );
			txtPort->SetValue( str );

			str.Printf( "Number of players : %i/%i", (int)m_games[i]->nbPlayer, (int)m_games[i]->maxPlayer );
			lstInfos->AppendString( str );

			str.Printf( "Current map : %s", m_games[i]->map );
			lstInfos->AppendString( str );

			str.Printf( "Password : %s", m_games[i]->password[0] != 0 ? "Yes" : "No" );
			lstInfos->AppendString( str );
			break;
		}
	}
}

void CFrmMain::On_btnDisconnectClick( wxCommandEvent& event )
{
	if( !m_peerId || !m_connected ) return;
	bb_peerSend( m_peerId, 0, RA_DISCONNECT, 0, true );
	bb_peerDelete( m_peerId, false );
	m_connected = false;
	m_peerId = 0;
	statusBar->SetLabel( wxString("Not connected to any server") );
}

void CFrmMain::On_lstVarsSelected( wxCommandEvent& event )
{
	int selectedVarIndex = -1;
	int selected = lstVars->GetSelection();
	wxString varSelected = lstVars->GetString( selected );

	// find the right var
	for( unsigned int i=0; i<m_vars.size(); i++ )
	{
		wxString varName = m_vars[i]->name;
		if( varName == varSelected )
		{
			selectedVarIndex = i;
			break;
		}
	}
	if( selectedVarIndex == -1 ) return;

	lblDescription->SetLabel( m_vars[selectedVarIndex]->description );
	lblDescription->Wrap( 400 );

	// hide all modifying controls
	chkBool->Hide();
	txtValue->Hide();

	wxString value = m_vars[selectedVarIndex]->value;

	// make the right one appear depending on data type
	if( value == "true" || value == "false" || value == "True" || value == "False" )
	{
		chkBool->Show();
		if( value == "true" )
		{
			chkBool->SetValue( true );
			chkBool->SetLabel( "True" );
		}
		else
		{
			chkBool->SetValue( false );
			chkBool->SetLabel( "False" );
		}
	}
	else
	{
		txtValue->Show();
		txtValue->SetLabel( m_vars[selectedVarIndex]->value );
	}
}

void CFrmMain::On_chkBoolChange( wxCommandEvent& event )
{
	bool checked = chkBool->GetValue();
	chkBool->SetLabel( checked ? "True" : "False" );

	// change value of the var itself
	int selectedVarIndex = -1;
	int selected = lstVars->GetSelection();
	wxString varSelected = lstVars->GetString( selected );
	// find the right var
	for( unsigned int i=0; i<m_vars.size(); i++ )
	{
		wxString varName = m_vars[i]->name;
		if( varName == varSelected )
		{
			selectedVarIndex = i;
			break;
		}
	}
	if( selectedVarIndex == -1 ) return;

	sprintf( m_vars[selectedVarIndex]->value, "%s", checked ? "true" : "false" );
	m_vars[selectedVarIndex]->changed = true;
}

void CFrmMain::On_btnUpdateVarClick( wxCommandEvent& event )
{
	if( !m_connected || m_peerId == 0 )
	{
		lstLog->AppendString( "Not connected, var update aborted..." );
		return;
	}
	lstLog->AppendString( "Updating changed vars on server..." );

	// check all vars marked as "changed" and send them to server with their new value
	for( unsigned int i=0; i<m_vars.size(); i++ )
	{
		if( m_vars[i]->changed )
		{
			// prepare command
			wxString command;
			command.Printf("set %s %s", m_vars[i]->name, m_vars[i]->value);
			net_ra_command comm;
			sprintf( comm.command, "%s", command.c_str() );

			bb_peerSend( m_peerId, (char*)&comm, RA_COMMAND, sizeof( net_ra_command ), true );
			m_vars[i]->changed = false;
		}
	}

	lstLog->AppendString( "Var update done!" );

}

void CFrmMain::On_txtValueKeyUp( wxKeyEvent& event )
{
	event.Skip();

	// change value of the var itself
	int selectedVarIndex = -1;
	int selected = lstVars->GetSelection();
	wxString varSelected = lstVars->GetString( selected );
	// find the right var
	for( unsigned int i=0; i<m_vars.size(); i++ )
	{
		wxString varName = m_vars[i]->name;
		if( varName == varSelected )
		{
			selectedVarIndex = i;
			break;
		}
	}
	if( selectedVarIndex == -1 ) return;

	sprintf( m_vars[selectedVarIndex]->value, "%s", txtValue->GetValue().c_str() );
	m_vars[selectedVarIndex]->changed = true;
}

void CFrmMain::On_txtChatEnter( wxCommandEvent& event )
{
	// send a say all command if theres some text in the textbox
	if( txtChat->GetValue().Len() > 0 )
	{
		if( !m_connected )
		{
			lstChatLog->AppendString("> Not connected to a server");
			return;
		}
		// prepare the command
		wxString command;
		command.Printf("sayall %s", txtChat->GetValue().c_str() );
		net_ra_command chatCommand;
		sprintf( chatCommand.command, "%s", command.c_str() );
		bb_peerSend( m_peerId, (char*)&chatCommand, RA_COMMAND, sizeof( net_ra_command ), true );

		wxString log;
		log.Printf("} console : %s", txtChat->GetValue().c_str() );
		lstChatLog->AppendString( log );
		txtChat->SetValue("");

	}
}

void CFrmMain::On_chkConsoleLogCheck( wxCommandEvent& event )
{
	if( !m_connected || m_peerId == 0 )
	{
		chkConsoleLog->SetValue( false );
		return;
	}
	bool checked = chkConsoleLog->GetValue();
	net_ra_console_broadcast_activate activate;
	activate.enabled = checked ? 1 : 0;
	bb_peerSend( m_peerId,(char*)&activate, RA_CONSOLE_BROADCAST_ACTIVATE, sizeof( net_ra_console_broadcast_activate ), true );

}

void CFrmMain::SendServerCommand( const char * in_command )
{
	if( !m_connected || m_peerId == 0 )
	{
		return;
	}

	// check if we are sending a AddMap or RemoveMap command, so we can update the combo boxes properly
	wxString strCommand = in_command;

	if( strCommand.Upper().Find("REMOVEMAP") != -1 )
	{
		wxString map = strCommand.AfterLast(' ');
		unsigned int i=0;
		for( i=0; i<cboRemoveMap->GetCount(); i++ )
		{
			if( cboRemoveMap->GetString(i) == map )
			{
				cboRemoveMap->Delete(i);
				cboAddMap->AppendString( map );
				break;
			}
		}
	}


	if( strCommand.Upper().Find("ADDMAP") != -1 )
	{
		wxString map = strCommand.AfterLast(' ');
		unsigned int i=0;
		for( i=0; i<cboAddMap->GetCount(); i++ )
		{
			if( cboAddMap->GetString(i) == map )
			{
				cboAddMap->Delete(i);
				cboRemoveMap->AppendString( map );
				break;
			}
		}
	}


	net_ra_command command;
	sprintf(command.command,"%s",in_command);

	bb_peerSend( m_peerId,(char*)&command, RA_COMMAND, sizeof( net_ra_command ), true );

	//wxString sentCommand;
	//sentCommand.Printf("Sent command to server : '%s'",command.command);
	//lstLog->AppendString( sentCommand );
}

void CFrmMain::On_btnKickClick( wxCommandEvent& event )
{
	int selected = lstPlayers->GetSelection();
	if( selected == wxNOT_FOUND )
	{
		lstLog->AppendString(L"Can't kick, no player selected");
		return;
	}

	wxString message;
	wxString playerName = lstPlayers->GetString( selected );
	message.Printf("Kick %s ?", playerName.c_str() );
	int answer = wxMessageBox( message, "Confirm kick", wxYES_NO | wxCENTRE );
	if( answer == wxNO ) return;

	// get the id of the selected player
	wxString player = lstPlayers->GetString( selected ).BeforeFirst(']');
	player = player.Remove(0,1);

	// build command we send to the server
	wxString command;
	command.Printf("kickid %s", player.c_str() );

	// send command to server
	SendServerCommand( (char*)command.c_str() );
}

void CFrmMain::On_btnBanClick( wxCommandEvent& event )
{
	int selected = lstPlayers->GetSelection();
	if( selected == wxNOT_FOUND )
	{
		lstLog->AppendString(L"Can't ban, no player selected");
		return;
	}

	wxString message;
	wxString playerName = lstPlayers->GetString( selected );
	message.Printf("Ban %s ?", playerName.c_str() );
	int answer = wxMessageBox( message, "Confirm ban", wxYES_NO | wxCENTRE );
	if( answer == wxNO ) return;

	// get the id of the selected player
	wxString player = lstPlayers->GetString( selected ).BeforeFirst(']');
	player = player.Remove(0,1);

	// build command we send to the server
	wxString command;
	command.Printf("banid %s", player.c_str() );

	// send command to server
	SendServerCommand( (char*)command.c_str() );
}

void CFrmMain::On_btnInputChangeMapClick( wxCommandEvent& event )
{
	wxString changeMap;
	int selected = cboChangeMap->GetSelection();
	if( selected == wxNOT_FOUND ) return;
	changeMap.Printf("changemap %s", cboChangeMap->GetString(selected));
	txtServerCommand->SetValue( changeMap );
}

void CFrmMain::On_btnSendCommandClick( wxCommandEvent& event )
{
	if( !m_connected || m_peerId == 0 ) return;
	if( txtServerCommand->GetValue().Len() < 5 ) return;
	SendServerCommand( (char*)txtServerCommand->GetValue().c_str() );
	txtServerCommand->SetValue("");
}

void CFrmMain::On_btnInputAddMapClick( wxCommandEvent& event )
{
	wxString addMap;
	int selected = cboAddMap->GetSelection();
	if( selected == wxNOT_FOUND ) return;
	addMap.Printf("addmap %s", cboAddMap->GetString(selected));
	txtServerCommand->SetValue( addMap );
}

void CFrmMain::On_btnInputRemoveMapClick( wxCommandEvent& event )
{
	wxString removeMap;
	int selected = cboRemoveMap->GetSelection();
	if( selected == wxNOT_FOUND ) return;
	removeMap.Printf("removemap %s", cboRemoveMap->GetString(selected));
	txtServerCommand->SetValue( removeMap );
}

void CFrmMain::On_btnUploadMapsClick( wxCommandEvent& event )
{
	if( !m_connected || m_peerId == 0 ) return;

	CDlgUploadMap * dlg = new CDlgUploadMap( &m_peerId );
	dlg->ShowModal();
	delete dlg;
}

void CFrmMain::On_btnSayAllClick( wxCommandEvent& event )
{
	wxCommandEvent evt;
	On_txtChatEnter( evt );
}

void CFrmMain::On_btnWhisperClick( wxCommandEvent& event )
{
	int selected = lstPlayers->GetSelection();
	if( selected == wxNOT_FOUND )
	{
		wxMessageBox( wxT("No player selected!"), wxT("Whisper failed"));
		return;
	}

	wxString player = lstPlayers->GetString( selected ).BeforeFirst(']');
	player = player.Remove(0,1);

	// build command we send to the server
	wxString command;
	command.Printf("sayid %s", player.c_str() );

	// send command to server
	SendServerCommand( (char*)command.c_str() );

	wxString log;
	log.Printf("} console whisper : %s", txtChat->GetValue().c_str() );
	lstChatLog->AppendString( log );
	txtChat->SetValue("");
}

void CFrmMain::On_fntPickerChanged( wxFontPickerEvent& event )
{
	lstChatLog->SetFont( event.GetFont() );
}

void CFrmMain::On_btnRemoveSpam_Click( wxCommandEvent& event )
{
	int selected = m_lstSpam->GetSelection();
	if( selected == wxNOT_FOUND ) return;
	m_lstSpam->Delete( selected );
}

void CFrmMain::On_btnAddSpam_Click( wxCommandEvent& event )
{
	wxString newSpam = m_txtNewMessage->GetValue();
	if( newSpam.Len() > 0 )
	{
		m_lstSpam->AppendString( newSpam );
		m_txtNewMessage->SetValue("");
	}
}

void CFrmMain::On_btnRemoveFilter_Click( wxCommandEvent& event )
{
	int selected = m_lstFilters->GetSelection();
	if( selected == wxNOT_FOUND ) return;
	m_lstFilters->Delete( selected );
}

void CFrmMain::On_btnAddFilter_Click( wxCommandEvent& event )
{
	wxString newFilter = m_txtNewFilter->GetValue();
	if( newFilter.Len() > 0 )
	{
		m_lstFilters->AppendString( newFilter );
		m_txtNewFilter->SetValue("");
	}
}


//////////// TKOTH Mod SPECIFIC CODE
void CFrmMain::On_btnStartTKOTH_Click( wxCommandEvent& event )
{
	// make sure we are connected to a server
	if( !m_connected || m_peerId == 0 )
	{
		wxMessageBox( "Not connected to any server, please connect first", "Can't start Team KOTH mod");
		return;
	}

	// now check if we need to trigger console log
	if( chkConsoleLog->GetValue() == false )
	{
		chkConsoleLog->SetValue(true);
		wxCommandEvent eventFake;
		On_chkConsoleLogCheck( eventFake );
	}

	// get Team Koth user settings
	wxString strGoalTime = txtTKOTHGoalTime->GetValue();
	double goalTime;
	strGoalTime.ToDouble(&goalTime);

	wxString strGoalScore = txtTKOTHGoalScore->GetValue();
	long goalScore;
	strGoalScore.ToLong(&goalScore);

	// start the mod!
	if( m_mod ) delete m_mod;
	m_mod = new CTKOTH((float)goalTime, (int)goalScore );
	m_mod->SetMainWindow( this );// this is mandatory

	chkConsoleLog->Disable();
	txtTKOTHStatus->SetLabel("Active");
	btnStopTKOTH->Enable();
	btnStartTKOTH->Disable();


}

void CFrmMain::On_btnStopTKOTH_Click( wxCommandEvent& event )
{
	if( m_mod ) delete m_mod;
	m_mod = 0;
	chkConsoleLog->Enable();
	txtTKOTHStatus->SetLabel("Idle");
	btnStopTKOTH->Disable();
	btnStartTKOTH->Enable();
}
/////////////////////////////// END TKOTH
