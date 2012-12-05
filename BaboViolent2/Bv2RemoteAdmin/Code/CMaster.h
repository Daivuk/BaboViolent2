#ifndef CMASTER_H_INCLUDED
#define CMASTER_H_INCLUDED

#include "cMSstruct.h"
#include "noname.h"
#include <wx/msgdlg.h>
#include <wx/timer.h>


// some macros to help fetch data after an sql query
#define MASTER_SCORE		i*nColumn+5
#define MASTER_ID			i*nColumn+6
#define MASTER_IP			i*nColumn+7
#define MASTER_LOCATION		i*nColumn+8
#define MASTER_PORT			i*nColumn+9


#include "sqlite3.h"
#include <vector>



class CMaster
{
private:

	// holds the master server IP
	char m_IP[16];

	// holds the master server Port
	unsigned short m_Port;

	// keep if master infos are valid
	bool	m_valid;

	// pointer on the list box to log infos
	wxListBox *	m_logger;

	// holds the current game version we have
	char m_CurrentVersion[5];

	unsigned long m_uniqueClientID;
	bool m_isConnected;
	int m_nbGameFound;
	// keep pointer on the vector the app wants us to put the games in
	std::vector<stBV2row*> * m_games;

	//--- To handle the incomming packets
	void RecvPacket(const char * buffer, int typeID);

	// to get IP/port of current master server in the sqlite3 database
	bool GetMasterInfos();

public:

	CMaster();
	~CMaster();

	//--- set a pointer on our log listbox
	void SetLogger( wxListBox * in_log ) { m_logger = in_log; }

	//--- Connecting the master
	void ConnectToMaster();

	//--- Kill it
	void DisconnectMaster();

	//--- Updating the master client, 0 = error, 1 = all good
	int Update(float in_delay);

	//--- Request games
	void RequestGames( std::vector<stBV2row*> * in_vector );

	//--- Are we connected to master
	bool IsConnected() const {return (m_uniqueClientID != 0);}

	//--- check if we are valid
	bool IsValid() const { return m_valid; }

	//--- Get number of game found
	int	GetNbGameFound() const { return m_nbGameFound;}


};

#endif

