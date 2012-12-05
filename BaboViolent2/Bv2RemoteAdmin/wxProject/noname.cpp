///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "noname.h"

///////////////////////////////////////////////////////////////////////////

IFrmMain::IFrmMain( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 600,600 ), wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_notebook2 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_notebook2->SetMinSize( wxSize( 581,-1 ) );
	
	pnlServers = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );
	
	bSizer14->SetMinSize( wxSize( 500,-1 ) ); 
	btnRefresh = new wxButton( pnlServers, wxID_ANY, wxT("Refresh Server List"), wxDefaultPosition, wxDefaultSize, 0 );
	btnRefresh->SetMinSize( wxSize( 150,-1 ) );
	
	bSizer14->Add( btnRefresh, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	lstGames = new wxListBox( pnlServers, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE|wxLB_SORT ); 
	bSizer14->Add( lstGames, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	lstInfos = new wxListBox( pnlServers, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer14->Add( lstInfos, 0, wxALL|wxEXPAND, 5 );
	
	bSizer2->Add( bSizer14, 1, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	bSizer5->SetMinSize( wxSize( 200,-1 ) ); 
	lblServerIp = new wxStaticText( pnlServers, wxID_ANY, wxT("Server IP "), wxDefaultPosition, wxDefaultSize, 0 );
	lblServerIp->Wrap( -1 );
	bSizer5->Add( lblServerIp, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	txtIP = new wxTextCtrl( pnlServers, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	txtIP->SetMaxLength( 15 ); 
	bSizer5->Add( txtIP, 0, wxALL, 5 );
	
	m_staticText9 = new wxStaticText( pnlServers, wxID_ANY, wxT("         Port"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	bSizer5->Add( m_staticText9, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	txtPort = new wxTextCtrl( pnlServers, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	txtPort->SetMaxLength( 5 ); 
	txtPort->SetMinSize( wxSize( 50,-1 ) );
	
	bSizer5->Add( txtPort, 0, wxALL, 5 );
	
	bSizer16->Add( bSizer5, 0, wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText10 = new wxStaticText( pnlServers, wxID_ANY, wxT("        Login"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	bSizer17->Add( m_staticText10, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	txtLogin = new wxTextCtrl( pnlServers, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	txtLogin->SetMaxLength( 20 ); 
	bSizer17->Add( txtLogin, 0, wxALL, 5 );
	
	m_staticText11 = new wxStaticText( pnlServers, wxID_ANY, wxT("Password"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	bSizer17->Add( m_staticText11, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	txtPass = new wxTextCtrl( pnlServers, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
	txtPass->SetMaxLength( 20 ); 
	bSizer17->Add( txtPass, 0, wxALL, 5 );
	
	bSizer16->Add( bSizer17, 1, wxEXPAND, 5 );
	
	bSizer2->Add( bSizer16, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	btnConnect = new wxButton( pnlServers, wxID_ANY, wxT("Connect"), wxDefaultPosition, wxDefaultSize, 0 );
	btnConnect->Enable( false );
	
	bSizer6->Add( btnConnect, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	btnDisconnect = new wxButton( pnlServers, wxID_ANY, wxT("Disconnect"), wxDefaultPosition, wxDefaultSize, 0 );
	btnDisconnect->Enable( false );
	
	bSizer6->Add( btnDisconnect, 0, wxALL, 5 );
	
	bSizer2->Add( bSizer6, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	bSizer3->Add( bSizer2, 1, wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	pnlServers->SetSizer( bSizer3 );
	pnlServers->Layout();
	bSizer3->Fit( pnlServers );
	m_notebook2->AddPage( pnlServers, wxT("Server List"), true );
	pnlServerVars = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	bSizer9->SetMinSize( wxSize( 200,-1 ) ); 
	lblVars = new wxStaticText( pnlServerVars, wxID_ANY, wxT("Available Variables"), wxDefaultPosition, wxDefaultSize, 0 );
	lblVars->Wrap( -1 );
	lblVars->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 90, true, wxEmptyString ) );
	
	bSizer9->Add( lblVars, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	lstVars = new wxListBox( pnlServerVars, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE|wxLB_SORT ); 
	bSizer9->Add( lstVars, 1, wxALL|wxEXPAND, 5 );
	
	bSizer8->Add( bSizer9, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	
	bSizer11->Add( 0, 20, 0, 0, 5 );
	
	lblDescriptionTag = new wxStaticText( pnlServerVars, wxID_ANY, wxT("Selected variable description"), wxDefaultPosition, wxDefaultSize, 0 );
	lblDescriptionTag->Wrap( -1 );
	lblDescriptionTag->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 90, true, wxEmptyString ) );
	
	bSizer11->Add( lblDescriptionTag, 0, wxALL, 5 );
	
	lblDescription = new wxStaticText( pnlServerVars, wxID_ANY, wxT("This var has no description yet"), wxDefaultPosition, wxDefaultSize, 0 );
	lblDescription->Wrap( 400 );
	lblDescription->SetMinSize( wxSize( 400,-1 ) );
	lblDescription->SetMaxSize( wxSize( 400,-1 ) );
	
	bSizer11->Add( lblDescription, 0, wxALL, 5 );
	
	bSizer13->Add( bSizer11, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxVERTICAL );
	
	lblChangeValue = new wxStaticText( pnlServerVars, wxID_ANY, wxT("Change this variable's value"), wxDefaultPosition, wxDefaultSize, 0 );
	lblChangeValue->Wrap( -1 );
	lblChangeValue->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 90, true, wxEmptyString ) );
	
	bSizer12->Add( lblChangeValue, 0, wxALL, 5 );
	
	chkBool = new wxCheckBox( pnlServerVars, wxID_ANY, wxT("True"), wxDefaultPosition, wxDefaultSize, 0 );
	
	chkBool->SetMinSize( wxSize( 100,-1 ) );
	
	bSizer12->Add( chkBool, 0, wxALL, 5 );
	
	txtValue = new wxTextCtrl( pnlServerVars, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	txtValue->SetMaxLength( 30 ); 
	txtValue->SetMinSize( wxSize( 300,-1 ) );
	
	bSizer12->Add( txtValue, 0, wxALL, 5 );
	
	bSizer13->Add( bSizer12, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxVERTICAL );
	
	btnUpdateVar = new wxButton( pnlServerVars, wxID_ANY, wxT("Update Vars On Server"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer18->Add( btnUpdateVar, 0, wxALL, 5 );
	
	bSizer13->Add( bSizer18, 0, wxEXPAND, 5 );
	
	bSizer10->Add( bSizer13, 1, wxEXPAND, 5 );
	
	bSizer8->Add( bSizer10, 1, wxEXPAND, 5 );
	
	bSizer7->Add( bSizer8, 1, wxEXPAND, 5 );
	
	pnlServerVars->SetSizer( bSizer7 );
	pnlServerVars->Layout();
	bSizer7->Fit( pnlServerVars );
	m_notebook2->AddPage( pnlServerVars, wxT("Server Variables"), false );
	pnlServerCommands = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer221;
	bSizer221 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer25;
	bSizer25 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText111 = new wxStaticText( pnlServerCommands, wxID_ANY, wxT("Change current map"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText111->Wrap( -1 );
	bSizer25->Add( m_staticText111, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	cboChangeMap = new wxComboBox( pnlServerCommands, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY|wxCB_SORT ); 
	bSizer25->Add( cboChangeMap, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	btnInputChangemap = new wxButton( pnlServerCommands, wxID_ANY, wxT("Input"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer25->Add( btnInputChangemap, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer23->Add( bSizer25, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer26;
	bSizer26 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText12 = new wxStaticText( pnlServerCommands, wxID_ANY, wxT("Add map to rotaion"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	bSizer26->Add( m_staticText12, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	cboAddMap = new wxComboBox( pnlServerCommands, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY|wxCB_SORT ); 
	bSizer26->Add( cboAddMap, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	btnInputAddMap = new wxButton( pnlServerCommands, wxID_ANY, wxT("Input"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer26->Add( btnInputAddMap, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer23->Add( bSizer26, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer27;
	bSizer27 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText13 = new wxStaticText( pnlServerCommands, wxID_ANY, wxT("Remove map from rotation"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13->Wrap( -1 );
	bSizer27->Add( m_staticText13, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	cboRemoveMap = new wxComboBox( pnlServerCommands, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	bSizer27->Add( cboRemoveMap, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	btnInputRemoveMap = new wxButton( pnlServerCommands, wxID_ANY, wxT("Input"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer27->Add( btnInputRemoveMap, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer23->Add( bSizer27, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer28;
	bSizer28 = new wxBoxSizer( wxHORIZONTAL );
	
	btnUploadMaps = new wxButton( pnlServerCommands, wxID_ANY, wxT("Upload new map(s) to server"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer28->Add( btnUploadMaps, 0, wxALL, 5 );
	
	bSizer23->Add( bSizer28, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	bSizer221->Add( bSizer23, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText101 = new wxStaticText( pnlServerCommands, wxID_ANY, wxT("Server Command"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText101->Wrap( -1 );
	bSizer24->Add( m_staticText101, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	txtServerCommand = new wxTextCtrl( pnlServerCommands, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	txtServerCommand->SetMaxLength( 70 ); 
	bSizer24->Add( txtServerCommand, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	btnSendCommand = new wxButton( pnlServerCommands, wxID_ANY, wxT("Send"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSendCommand->SetDefault(); 
	bSizer24->Add( btnSendCommand, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer221->Add( bSizer24, 0, wxEXPAND, 5 );
	
	pnlServerCommands->SetSizer( bSizer221 );
	pnlServerCommands->Layout();
	bSizer221->Fit( pnlServerCommands );
	m_notebook2->AddPage( pnlServerCommands, wxT("Server Commands"), false );
	pnlServerTools = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer401;
	bSizer401 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer411;
	bSizer411 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText19 = new wxStaticText( pnlServerTools, wxID_ANY, wxT("Auto Message"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText19->Wrap( -1 );
	bSizer411->Add( m_staticText19, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_lstSpam = new wxListBox( pnlServerTools, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer411->Add( m_lstSpam, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer43;
	bSizer43 = new wxBoxSizer( wxHORIZONTAL );
	
	m_btnRemoveMessage = new wxButton( pnlServerTools, wxID_ANY, wxT("Remove Selected"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer43->Add( m_btnRemoveMessage, 0, wxALL, 5 );
	
	
	bSizer43->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText21 = new wxStaticText( pnlServerTools, wxID_ANY, wxT("Delay between messages(in seconds):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	bSizer43->Add( m_staticText21, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_txtSpamDelay = new wxTextCtrl( pnlServerTools, wxID_ANY, wxT("7"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer43->Add( m_txtSpamDelay, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer411->Add( bSizer43, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer42;
	bSizer42 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText20 = new wxStaticText( pnlServerTools, wxID_ANY, wxT("New Message:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText20->Wrap( -1 );
	bSizer42->Add( m_staticText20, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_txtNewMessage = new wxTextCtrl( pnlServerTools, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer42->Add( m_txtNewMessage, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_btnAddMessage = new wxButton( pnlServerTools, wxID_ANY, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer42->Add( m_btnAddMessage, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer411->Add( bSizer42, 0, wxEXPAND, 5 );
	
	
	bSizer411->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( pnlServerTools, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer411->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	
	bSizer411->Add( 0, 30, 1, wxEXPAND, 5 );
	
	bSizer401->Add( bSizer411, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer44;
	bSizer44 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText22 = new wxStaticText( pnlServerTools, wxID_ANY, wxT("Word Filter"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText22->Wrap( -1 );
	bSizer44->Add( m_staticText22, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_lstFilters = new wxListBox( pnlServerTools, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer44->Add( m_lstFilters, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer45;
	bSizer45 = new wxBoxSizer( wxHORIZONTAL );
	
	m_btnRemoveFilter = new wxButton( pnlServerTools, wxID_ANY, wxT("Remove Selected"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer45->Add( m_btnRemoveFilter, 0, wxALL, 5 );
	
	
	bSizer45->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText23 = new wxStaticText( pnlServerTools, wxID_ANY, wxT("Number of warnings before kick:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	bSizer45->Add( m_staticText23, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_txtWarnings = new wxTextCtrl( pnlServerTools, wxID_ANY, wxT("2"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtWarnings->SetMaxLength( 1 ); 
	bSizer45->Add( m_txtWarnings, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText25 = new wxStaticText( pnlServerTools, wxID_ANY, wxT("before ban:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText25->Wrap( -1 );
	bSizer45->Add( m_staticText25, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_txtBans = new wxTextCtrl( pnlServerTools, wxID_ANY, wxT("3"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtBans->SetMaxLength( 1 ); 
	bSizer45->Add( m_txtBans, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer44->Add( bSizer45, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer46;
	bSizer46 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText24 = new wxStaticText( pnlServerTools, wxID_ANY, wxT("New Filter:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText24->Wrap( -1 );
	bSizer46->Add( m_staticText24, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_txtNewFilter = new wxTextCtrl( pnlServerTools, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer46->Add( m_txtNewFilter, 1, wxALL, 5 );
	
	m_btnAddFilter = new wxButton( pnlServerTools, wxID_ANY, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer46->Add( m_btnAddFilter, 0, wxALL, 5 );
	
	bSizer44->Add( bSizer46, 0, wxEXPAND, 5 );
	
	bSizer401->Add( bSizer44, 1, wxEXPAND, 5 );
	
	pnlServerTools->SetSizer( bSizer401 );
	pnlServerTools->Layout();
	bSizer401->Fit( pnlServerTools );
	m_notebook2->AddPage( pnlServerTools, wxT("Server Tools"), false );
	pnlPlayers = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer38;
	bSizer38 = new wxBoxSizer( wxVERTICAL );
	
	lstChatLog = new wxListBox( pnlPlayers, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_HSCROLL|wxHSCROLL|wxVSCROLL ); 
	bSizer38->Add( lstChatLog, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer39;
	bSizer39 = new wxBoxSizer( wxHORIZONTAL );
	
	fntPicker = new wxFontPickerCtrl( pnlPlayers, wxID_ANY, wxNullFont, wxDefaultPosition, wxDefaultSize, 0 );
	fntPicker->SetMaxPointSize( 100 ); 
	bSizer39->Add( fntPicker, 0, wxALL, 5 );
	
	
	bSizer39->Add( 0, 0, 1, 0, 5 );
	
	chkAutoScroll = new wxCheckBox( pnlPlayers, wxID_ANY, wxT("Auto Scroll"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer39->Add( chkAutoScroll, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	bSizer38->Add( bSizer39, 0, wxEXPAND, 5 );
	
	bSizer20->Add( bSizer38, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer31;
	bSizer31 = new wxBoxSizer( wxHORIZONTAL );
	
	txtChat = new wxTextCtrl( pnlPlayers, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	txtChat->SetMaxLength( 70 ); 
	bSizer31->Add( txtChat, 1, wxALL, 5 );
	
	btnSayAll = new wxButton( pnlPlayers, wxID_ANY, wxT("SayAll"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer31->Add( btnSayAll, 0, wxALL, 5 );
	
	btnWhisper = new wxButton( pnlPlayers, wxID_ANY, wxT("SayId"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer31->Add( btnWhisper, 0, wxALL, 5 );
	
	bSizer20->Add( bSizer31, 0, wxEXPAND, 5 );
	
	bSizer19->Add( bSizer20, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxVERTICAL );
	
	bSizer21->SetMinSize( wxSize( 100,200 ) ); 
	lstPlayers = new wxListBox( pnlPlayers, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_HSCROLL ); 
	bSizer21->Add( lstPlayers, 1, wxALL|wxEXPAND, 5 );
	
	btnKick = new wxButton( pnlPlayers, wxID_ANY, wxT("Kick Selected Player"), wxDefaultPosition, wxDefaultSize, 0 );
	btnKick->SetMinSize( wxSize( 130,-1 ) );
	
	bSizer21->Add( btnKick, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	btnBan = new wxButton( pnlPlayers, wxID_ANY, wxT("Ban Selected Player"), wxDefaultPosition, wxDefaultSize, 0 );
	btnBan->SetMinSize( wxSize( 130,-1 ) );
	
	bSizer21->Add( btnBan, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	bSizer19->Add( bSizer21, 0, wxEXPAND, 5 );
	
	pnlPlayers->SetSizer( bSizer19 );
	pnlPlayers->Layout();
	bSizer19->Fit( pnlPlayers );
	m_notebook2->AddPage( pnlPlayers, wxT("Players"), false );
	pnlTKOTH = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer32;
	bSizer32 = new wxBoxSizer( wxVERTICAL );
	
	m_bitmap1 = new wxStaticBitmap( pnlTKOTH, wxID_ANY, wxBitmap( wxT("Images/KOTH_header.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER );
	bSizer32->Add( m_bitmap1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer33;
	bSizer33 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText15 = new wxStaticText( pnlTKOTH, wxID_ANY, wxT("Team KOTH"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	bSizer33->Add( m_staticText15, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	
	bSizer33->Add( 0, 20, 0, 0, 5 );
	
	wxBoxSizer* bSizer34;
	bSizer34 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer34->Add( 20, 0, 1, wxEXPAND, 5 );
	
	m_staticText17 = new wxStaticText( pnlTKOTH, wxID_ANY, wxT("Goal Time(in seconds)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	m_staticText17->SetToolTip( wxT("How much time a team has to hold the zone to get a cap") );
	
	bSizer34->Add( m_staticText17, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	txtTKOTHGoalTime = new wxTextCtrl( pnlTKOTH, wxID_ANY, wxT("15.0"), wxDefaultPosition, wxDefaultSize, wxTE_RIGHT );
	txtTKOTHGoalTime->SetToolTip( wxT("How much time a team has to hold the zone to get a cap") );
	
	bSizer34->Add( txtTKOTHGoalTime, 0, wxALL, 5 );
	
	bSizer33->Add( bSizer34, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer40;
	bSizer40 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer40->Add( 72, 0, 1, wxEXPAND, 5 );
	
	m_staticText211 = new wxStaticText( pnlTKOTH, wxID_ANY, wxT("Goal Score"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText211->Wrap( -1 );
	m_staticText211->SetToolTip( wxT("How many times a team has to score to win the match") );
	
	bSizer40->Add( m_staticText211, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	txtTKOTHGoalScore = new wxTextCtrl( pnlTKOTH, wxID_ANY, wxT("7"), wxDefaultPosition, wxDefaultSize, wxTE_RIGHT );
	txtTKOTHGoalScore->SetToolTip( wxT("How many times a team has to score to win the match") );
	
	bSizer40->Add( txtTKOTHGoalScore, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer33->Add( bSizer40, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer36;
	bSizer36 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer36->Add( 0, 20, 0, 0, 5 );
	
	bSizer33->Add( bSizer36, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer41;
	bSizer41 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText18 = new wxStaticText( pnlTKOTH, wxID_ANY, wxT("Status"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18->Wrap( -1 );
	bSizer41->Add( m_staticText18, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	txtTKOTHStatus = new wxTextCtrl( pnlTKOTH, wxID_ANY, wxT("Idle"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE|wxTE_READONLY );
	bSizer41->Add( txtTKOTHStatus, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	btnStartTKOTH = new wxButton( pnlTKOTH, wxID_ANY, wxT("Start Team KOTH"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer41->Add( btnStartTKOTH, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	btnStopTKOTH = new wxButton( pnlTKOTH, wxID_ANY, wxT("Stop Team Koth"), wxDefaultPosition, wxDefaultSize, 0 );
	btnStopTKOTH->Enable( false );
	
	bSizer41->Add( btnStopTKOTH, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer33->Add( bSizer41, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	bSizer32->Add( bSizer33, 1, wxEXPAND, 5 );
	
	pnlTKOTH->SetSizer( bSizer32 );
	pnlTKOTH->Layout();
	bSizer32->Fit( pnlTKOTH );
	m_notebook2->AddPage( pnlTKOTH, wxT("MOD : Team KOTH"), false );
	
	bSizer1->Add( m_notebook2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );
	
	bSizer15->SetMinSize( wxSize( 590,100 ) ); 
	wxBoxSizer* bSizer22;
	bSizer22 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText8 = new wxStaticText( this, wxID_ANY, wxT("Log"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	bSizer22->Add( m_staticText8, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	bSizer22->Add( 20, 0, 0, 0, 5 );
	
	chkConsoleLog = new wxCheckBox( this, wxID_ANY, wxT("Show log from remote console"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer22->Add( chkConsoleLog, 0, wxALL, 5 );
	
	chkScrollLog = new wxCheckBox( this, wxID_ANY, wxT("Auto Scroll"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer22->Add( chkScrollLog, 0, wxALL, 5 );
	
	
	bSizer22->Add( 20, 0, 1, wxEXPAND, 5 );
	
	chkDumpLog = new wxCheckBox( this, wxID_ANY, wxT("Dump console log to text file"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer22->Add( chkDumpLog, 0, wxALL, 5 );
	
	bSizer15->Add( bSizer22, 0, wxEXPAND, 5 );
	
	lstLog = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxSize( 580,80 ), 0, NULL, 0 ); 
	lstLog->SetMinSize( wxSize( 580,80 ) );
	lstLog->SetMaxSize( wxSize( -1,80 ) );
	
	bSizer15->Add( lstLog, 0, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	bSizer1->Add( bSizer15, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	statusBar = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
	
	// Connect Events
	btnRefresh->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IFrmMain::On_btnRefreshGames_Click ), NULL, this );
	lstGames->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( IFrmMain::On_lstGames_Selected ), NULL, this );
	btnConnect->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IFrmMain::On_btnConnectClick ), NULL, this );
	btnDisconnect->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IFrmMain::On_btnDisconnectClick ), NULL, this );
	lstVars->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( IFrmMain::On_lstVarsSelected ), NULL, this );
	chkBool->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( IFrmMain::On_chkBoolChange ), NULL, this );
	txtValue->Connect( wxEVT_KEY_UP, wxKeyEventHandler( IFrmMain::On_txtValueKeyUp ), NULL, this );
	btnUpdateVar->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IFrmMain::On_btnUpdateVarClick ), NULL, this );
	btnInputChangemap->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IFrmMain::On_btnInputChangeMapClick ), NULL, this );
	btnInputAddMap->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IFrmMain::On_btnInputAddMapClick ), NULL, this );
	btnInputRemoveMap->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IFrmMain::On_btnInputRemoveMapClick ), NULL, this );
	btnUploadMaps->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IFrmMain::On_btnUploadMapsClick ), NULL, this );
	btnSendCommand->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IFrmMain::On_btnSendCommandClick ), NULL, this );
	m_btnRemoveMessage->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IFrmMain::On_btnRemoveSpam_Click ), NULL, this );
	m_btnAddMessage->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IFrmMain::On_btnAddSpam_Click ), NULL, this );
	m_btnRemoveFilter->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IFrmMain::On_btnRemoveFilter_Click ), NULL, this );
	m_btnAddFilter->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IFrmMain::On_btnAddFilter_Click ), NULL, this );
	fntPicker->Connect( wxEVT_COMMAND_FONTPICKER_CHANGED, wxFontPickerEventHandler( IFrmMain::On_fntPickerChanged ), NULL, this );
	txtChat->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( IFrmMain::On_txtChatEnter ), NULL, this );
	btnSayAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IFrmMain::On_btnSayAllClick ), NULL, this );
	btnWhisper->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IFrmMain::On_btnWhisperClick ), NULL, this );
	btnKick->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IFrmMain::On_btnKickClick ), NULL, this );
	btnBan->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IFrmMain::On_btnBanClick ), NULL, this );
	btnStartTKOTH->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IFrmMain::On_btnStartTKOTH_Click ), NULL, this );
	btnStopTKOTH->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IFrmMain::On_btnStopTKOTH_Click ), NULL, this );
	chkConsoleLog->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( IFrmMain::On_chkConsoleLogCheck ), NULL, this );
}

dlgMapUpload::dlgMapUpload( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer29;
	bSizer29 = new wxBoxSizer( wxVERTICAL );
	
	
	bSizer29->Add( 0, 30, 0, 0, 5 );
	
	wxBoxSizer* bSizer31;
	bSizer31 = new wxBoxSizer( wxHORIZONTAL );
	
	btnAddMap = new wxButton( this, wxID_ANY, wxT("Add Map"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer31->Add( btnAddMap, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	btnRemoveMap = new wxButton( this, wxID_ANY, wxT("Remove Selected Map"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer31->Add( btnRemoveMap, 0, wxALL, 5 );
	
	bSizer29->Add( bSizer31, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	lstMaps = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxSize( 700,200 ), 0, NULL, 0 ); 
	bSizer29->Add( lstMaps, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	btnUploadAll = new wxButton( this, wxID_ANY, wxT("Upload all maps!"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer29->Add( btnUploadAll, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	pbUpload = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxSize( 400,-1 ), wxGA_HORIZONTAL );
	bSizer29->Add( pbUpload, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	lblDone = new wxStaticText( this, wxID_ANY, wxT("Done!"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE|wxST_NO_AUTORESIZE );
	lblDone->Wrap( -1 );
	bSizer29->Add( lblDone, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer29 );
	this->Layout();
	
	// Connect Events
	btnAddMap->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( dlgMapUpload::On_btnAddMap_Click ), NULL, this );
	btnRemoveMap->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( dlgMapUpload::On_btnRemoveMap_Click ), NULL, this );
	btnUploadAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( dlgMapUpload::On_btnUploadAll_Click ), NULL, this );
}
