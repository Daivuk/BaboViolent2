#include "CMaster.h"
#include "baboNet.h"

CMaster::CMaster()
{
	m_uniqueClientID = 0;
	m_isConnected = false;
	m_nbGameFound = -1;
	m_logger = 0;
	m_games = 0;
	m_valid = GetMasterInfos();
}


CMaster::~CMaster()
{
	DisconnectMaster();
}


//
//--- Connecting the master
//
void CMaster::ConnectToMaster()
{
	if( m_logger ) m_logger->AppendString(wxString("Establishing connection to master server..."));
	m_uniqueClientID = bb_clientConnect(m_IP, m_Port);
	m_isConnected = false;
}



//
//--- Kill it
//
void CMaster::DisconnectMaster()
{
	if( m_logger ) m_logger->AppendString(wxString("Disconnected from master server..."));
	if (m_uniqueClientID) bb_clientDisconnect(m_uniqueClientID);
	m_uniqueClientID = 0;
	m_isConnected = false;
}


//
//--- Updating the master client
//
int CMaster::Update(float in_delay)
{
	// Update the master client
	if (m_uniqueClientID > 0)
	{
		int result = bb_clientUpdate(m_uniqueClientID, in_delay, UPDATE_SEND_RECV);

		if (result == 1)
		{
			// An error, stop everything
			if( m_logger ) m_logger->AppendString(wxString("Master Server error... disconnected"));
			m_uniqueClientID = 0;
			m_isConnected = false;
			m_valid = false;
			return 0;
		}
		else if (result == 2)
		{
			// Server disconnected us
			if( m_logger ) m_logger->AppendString(wxString("Master server broke connection with us :'("));
			m_uniqueClientID = 0;
			m_isConnected = false;
			m_valid = false;
			return 0;
		}
		else if (result == 3)
		{
			//--- We are connected.
			if( m_logger ) m_logger->AppendString(wxString("Connection to master server successfull..."));
			m_valid = true;
			m_isConnected = true;
		}
	}

	// if we are connected to master server
	if (m_uniqueClientID > 0)
	{
		// receive messages
		char * buffer;
		int messageID;
		while (buffer = bb_clientReceive(m_uniqueClientID, &messageID))
		{
			// receive packets from master
			RecvPacket(buffer, messageID);
		}
	}
	return 1;
}




//
//--- To handle the incomming packets
//
void CMaster::RecvPacket(const char * buffer, int typeID)
{
	switch (typeID)
	{
		case MASTER_INFO:
		{
			stMasterInfo masterInfo;
			memcpy(&masterInfo, buffer, sizeof(stMasterInfo));
			if (masterInfo.NbGames == 0)
			{
				m_nbGameFound = 0;
				//DisconnectMaster();
				//console->add(CString("\x3> No games found"));
			}
			else if (masterInfo.NbGames == -1)
			{
				m_nbGameFound = 0;
				//DisconnectMaster();
				//console->add(CString("\x4> Outdated version"));
			}
			else
			{
				m_nbGameFound = masterInfo.NbGames;
				//console->add(CString("\x3> Games found : %i", m_nbGameFound));
				if( m_logger )
				{
					wxString log;
					log.Printf( "Receiving %i game(s) from master", m_nbGameFound );
					m_logger->AppendString( log );
				}
			}
			break;
		}
		case BV2_ROW:
		{
			stBV2row* bv2Row = new stBV2row();
			memcpy(bv2Row, buffer, sizeof(stBV2row));
			m_games->push_back(bv2Row);

			//if (gameVar.c_debug) console->add(CString("\x9> Received game %i/%i", (int)m_games.size(), m_nbGameFound));
			break;
		}
		default:
		{
			//console->add("Unhandled message by CMaster");
			break;
		}
	}
}



//
//--- Request games
//
void CMaster::RequestGames( std::vector<stBV2row*> * in_vector )
{
	m_games = in_vector;
	m_nbGameFound = -1;

	if( m_uniqueClientID == 0 )
	{
		ConnectToMaster();
	}

	stBV2list bv2List;
	strcpy(bv2List.Version, m_CurrentVersion );
	bb_clientSend( m_uniqueClientID, (char*)(&bv2List), sizeof(stBV2list), BV2_LIST );
	if( m_logger ) m_logger->AppendString( wxString("Requesting game list from master...") );
}

//
//--- Get Master Server IP / Port and our current game version from the database
//
bool CMaster::GetMasterInfos()
{
	//connect to database
	sqlite3 *db = 0;
	
	int rc = sqlite3_open("admin.db",&db);
	if(rc)
	{
		wxMessageBox( wxT("admin.db database not found"), wxT("Master Server Error"));
		return false;
	}
	
	//some infos to load the data
	char	*zErrMsg;		// holds error msg if any
	char	**azResult;		// contains the actual returned data
	int		nRow;			// number of record
	int		nColumn;		// number of column
	char	SQL[256];		// the query

	// Get infos of master servers and choose the one with the lowest Score
	sprintf(SQL,"Select * From MasterServers;");
	sqlite3_get_table(db,SQL,&azResult,&nRow,&nColumn,&zErrMsg);

	
	int i,best=9999,bestIndex=0;
	for(i=0;i<nRow;i++)
	{
		if( atoi(azResult[MASTER_SCORE]) < best )
		{
			best = atoi(azResult[MASTER_SCORE]);
			bestIndex = i;
		}
	}
	i = bestIndex;
	sprintf( m_IP , "%s", azResult[MASTER_IP]);
	m_Port = atoi(azResult[MASTER_PORT]) - 1000;
	sqlite3_free_table(azResult);
	sqlite3_close( db );

	sprintf( m_CurrentVersion , "2.11" );
	

	return true;
}
