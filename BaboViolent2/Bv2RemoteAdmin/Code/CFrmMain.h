#ifndef CFRMMAIN_H_INCLUDED
#define CFRMMAIN_H_INCLUDED

#include "noname.h"
#include "CDlgUploadMap.h"
#include "CMaster.h"


class AMod;


// this struct defines a server var
struct SServerVar
{
public:
	char name[80];
	char value[50];
	char description[514];
	bool changed;

	SServerVar()
	{
		sprintf(description,"This var has no description yet");
		sprintf(name,"NoName");
		sprintf(value,"NULL");
		changed = false;
	}

	SServerVar( const char * in_name, const char * in_value, const char * in_description )
	{
		if( in_description ) sprintf(description,"%s",in_description);
		if( in_name ) sprintf(name,"%s",in_name);
		if( in_value ) sprintf(value,"%s",in_value);
		changed = false;
	}
};

// struct to keep player Name/IP/ID
struct SUser
{
public:

	wxString	m_name;
	wxString	m_ip;
	long		m_id;

	SUser( const char * in_name, const char * in_ip, const long & in_id )
	{
		m_name = in_name;
		m_ip = in_ip;
		m_id = in_id;
	}

	~SUser(){}
};

// maximum life time of a struct
#define WARNING_LIFE	900.0f	// 15 minutes warnings should be enough
// this struct defines an issued warning
struct SWarning
{
public:
	
	wxString	m_playerIP;
	float		m_warningLife;
	int			m_warningCount;

	SWarning( const char * in_ip )
	{
		m_playerIP = in_ip;
		m_warningLife = WARNING_LIFE;
		m_warningCount = 0;
	}
	~SWarning(){}

	int Update( const float & in_delta )
	{
		m_warningLife -= in_delta;
		if( m_warningLife <= 0.0f )
		{
			// remove the warning
			return 1;
		}
		return 0;
	}
};

class CFrmMain : public IFrmMain, public wxTimer
{
private:

	// keep pointer on our loaded mod if any
	AMod *	m_mod;

	// keep pointer on sqlite3 database
	sqlite3 * m_db;

	// keep peer id with which we should communicate with our server
	int		m_peerId;

	// keep if we need to ask master a game request
	bool	m_requestingGames;

	// keep a 10 secondes timeout so we dontf lood master
	float m_masterTimeout;

	// keep connection process timeout to remote server
	float	m_connTimeout;

	// keep if a connection was established with a server
	bool	m_connected;

	// keep server vars
	std::vector<SServerVar *>	m_vars;

	// keep vector of emitted warnings
	std::vector<SWarning *>		m_warnings;

	// keep vector of users
	std::vector<SUser *>		m_users;


	// keep a pointer on our master server object
	CMaster	* m_master;

	// keep vector of bv2 games found
	std::vector<stBV2row *> m_games;

	// time before we send a keep alive packet to the server we are connected to
	float	m_keepAliveTime;

	// helper function to update master server
	void UpdateMaster( const float & in_delay );

	// keep last number of rows in the log box to make it scroll automatically!
	unsigned int		m_nbLogRows;
	unsigned int		m_nbChatLogRows;

	// helper function to receive peer packets
	void	ReceivePeerPackets();

	// helper function to add a new server var received from the server
	void	AddNewVar( char * in_varName );

	// helper function to Retreive a user by id
	SUser * GetUserById( const int & in_id );

	// helper function to Retreive a user by ip
	SUser * GetUserByIp( const char * in_ip );

	// helper function to add a user
	void	AddUser( const char * in_name, const char * in_ip, const long & in_id );
	// helper function to remove a user
	void	RemoveUser( const long & in_id );


	// keep maximum of warnings before a kick/ban
	int		m_maxWarningsBeforeKick;
	int		m_maxWarningsBeforeBan;

	// keep last time we emmitted an automessage
	float	m_lastMessageTime;
	int		m_nextMessageIndex;

	// helper function to add a warning
	void	AddWarning( const char * in_ip );

	// helper function to get a warning by ip
	SWarning * GetWarningByIp( const char * in_ip );

	// helper function to check for word filtering
	void FilterMessage( const char * in_message, const long in_playerId );

	// helper functions to load Messages/Filters
	void LoadFilters();
	void LoadMessages();

public:

	// public accessors for player listbox
	wxListBox * GetPlayerLst() { return lstPlayers; }

	// accessor to log infos
	void Log( const char * in_string )
	{
		lstLog->AppendString( in_string );
	}
	
	CFrmMain();
	~CFrmMain();

	// Overloaded function from wxTimer
	void	Notify();

	// helper function to send a command to the server
	void	SendServerCommand( const char * in_command );

	// Events derived from IFrmMain
	virtual void On_btnConnectClick( wxCommandEvent& event );
	virtual void On_btnRefreshGames_Click( wxCommandEvent& event );
	virtual void On_lstGames_Selected( wxCommandEvent& event );
	virtual void On_btnDisconnectClick( wxCommandEvent& event );
	virtual void On_lstVarsSelected( wxCommandEvent& event );
	virtual void On_btnUpdateVarClick( wxCommandEvent& event );
	virtual void On_chkBoolChange( wxCommandEvent& event );
	virtual void On_txtValueKeyUp( wxKeyEvent& event );
	virtual void On_txtChatEnter( wxCommandEvent& event );
	virtual void On_chkConsoleLogCheck( wxCommandEvent& event );
	virtual void On_btnKickClick( wxCommandEvent& event );
	virtual void On_btnBanClick( wxCommandEvent& event );
	virtual void On_btnInputChangeMapClick( wxCommandEvent& event );
	virtual void On_btnSendCommandClick( wxCommandEvent& event );
	virtual void On_btnInputAddMapClick( wxCommandEvent& event );
	virtual void On_btnInputRemoveMapClick( wxCommandEvent& event );
	virtual void On_btnUploadMapsClick( wxCommandEvent& event );
	virtual void On_btnSayAllClick( wxCommandEvent& event );
	virtual void On_btnWhisperClick( wxCommandEvent& event );
	virtual void On_fntPickerChanged( wxFontPickerEvent& event );
	
	virtual void On_btnRemoveSpam_Click( wxCommandEvent& event );
	virtual void On_btnAddSpam_Click( wxCommandEvent& event );
	virtual void On_btnRemoveFilter_Click( wxCommandEvent& event );
	virtual void On_btnAddFilter_Click( wxCommandEvent& event );

	// TKOTH specific events
	virtual void On_btnStartTKOTH_Click( wxCommandEvent& event );
	virtual void On_btnStopTKOTH_Click( wxCommandEvent& event );


};

#endif
