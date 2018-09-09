///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __noname__
#define __noname__

#include <wx/string.h>
#include <wx/button.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/statline.h>
#include <wx/fontpicker.h>
#include <wx/statbmp.h>
#include <wx/notebook.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
#include <wx/gauge.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class IFrmMain
///////////////////////////////////////////////////////////////////////////////
class IFrmMain : public wxFrame 
{
	private:
	
	protected:
		wxNotebook* m_notebook2;
		wxPanel* pnlServers;
		wxButton* btnRefresh;
		wxListBox* lstGames;
		wxListBox* lstInfos;
		wxStaticText* lblServerIp;
		wxTextCtrl* txtIP;
		wxStaticText* m_staticText9;
		wxTextCtrl* txtPort;
		wxStaticText* m_staticText10;
		wxTextCtrl* txtLogin;
		wxStaticText* m_staticText11;
		wxTextCtrl* txtPass;
		wxButton* btnConnect;
		wxButton* btnDisconnect;
		wxPanel* pnlServerVars;
		wxStaticText* lblVars;
		wxListBox* lstVars;
		
		wxStaticText* lblDescriptionTag;
		wxStaticText* lblDescription;
		wxStaticText* lblChangeValue;
		wxCheckBox* chkBool;
		wxTextCtrl* txtValue;
		wxButton* btnUpdateVar;
		wxPanel* pnlServerCommands;
		wxStaticText* m_staticText111;
		wxComboBox* cboChangeMap;
		wxButton* btnInputChangemap;
		wxStaticText* m_staticText12;
		wxComboBox* cboAddMap;
		wxButton* btnInputAddMap;
		wxStaticText* m_staticText13;
		wxComboBox* cboRemoveMap;
		wxButton* btnInputRemoveMap;
		wxButton* btnUploadMaps;
		wxStaticText* m_staticText101;
		wxTextCtrl* txtServerCommand;
		wxButton* btnSendCommand;
		wxPanel* pnlServerTools;
		wxStaticText* m_staticText19;
		wxListBox* m_lstSpam;
		wxButton* m_btnRemoveMessage;
		
		wxStaticText* m_staticText21;
		wxTextCtrl* m_txtSpamDelay;
		wxStaticText* m_staticText20;
		wxTextCtrl* m_txtNewMessage;
		wxButton* m_btnAddMessage;
		
		wxStaticLine* m_staticline1;
		
		wxStaticText* m_staticText22;
		wxListBox* m_lstFilters;
		wxButton* m_btnRemoveFilter;
		
		wxStaticText* m_staticText23;
		wxTextCtrl* m_txtWarnings;
		wxStaticText* m_staticText25;
		wxTextCtrl* m_txtBans;
		wxStaticText* m_staticText24;
		wxTextCtrl* m_txtNewFilter;
		wxButton* m_btnAddFilter;
		wxPanel* pnlPlayers;
		wxListBox* lstChatLog;
		wxFontPickerCtrl* fntPicker;
		
		wxCheckBox* chkAutoScroll;
		wxTextCtrl* txtChat;
		wxButton* btnSayAll;
		wxButton* btnWhisper;
		wxListBox* lstPlayers;
		wxButton* btnKick;
		wxButton* btnBan;
		wxPanel* pnlTKOTH;
		wxStaticBitmap* m_bitmap1;
		wxStaticText* m_staticText15;
		
		
		wxStaticText* m_staticText17;
		wxTextCtrl* txtTKOTHGoalTime;
		
		wxStaticText* m_staticText211;
		wxTextCtrl* txtTKOTHGoalScore;
		
		wxStaticText* m_staticText18;
		wxTextCtrl* txtTKOTHStatus;
		wxButton* btnStartTKOTH;
		wxButton* btnStopTKOTH;
		wxStaticText* m_staticText8;
		
		wxCheckBox* chkConsoleLog;
		wxCheckBox* chkScrollLog;
		
		wxCheckBox* chkDumpLog;
		wxListBox* lstLog;
		wxStatusBar* statusBar;
		
		// Virtual event handlers, overide them in your derived class
		virtual void On_btnRefreshGames_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void On_lstGames_Selected( wxCommandEvent& event ){ event.Skip(); }
		virtual void On_btnConnectClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void On_btnDisconnectClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void On_lstVarsSelected( wxCommandEvent& event ){ event.Skip(); }
		virtual void On_chkBoolChange( wxCommandEvent& event ){ event.Skip(); }
		virtual void On_txtValueKeyUp( wxKeyEvent& event ){ event.Skip(); }
		virtual void On_btnUpdateVarClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void On_btnInputChangeMapClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void On_btnInputAddMapClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void On_btnInputRemoveMapClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void On_btnUploadMapsClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void On_btnSendCommandClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void On_btnRemoveSpam_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void On_btnAddSpam_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void On_btnRemoveFilter_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void On_btnAddFilter_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void On_fntPickerChanged( wxFontPickerEvent& event ){ event.Skip(); }
		virtual void On_txtChatEnter( wxCommandEvent& event ){ event.Skip(); }
		virtual void On_btnSayAllClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void On_btnWhisperClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void On_btnKickClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void On_btnBanClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void On_btnStartTKOTH_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void On_btnStopTKOTH_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void On_chkConsoleLogCheck( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		IFrmMain( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Babo Violent 2 Remote Admin v3.0"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 600,600 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class dlgMapUpload
///////////////////////////////////////////////////////////////////////////////
class dlgMapUpload : public wxDialog 
{
	private:
	
	protected:
		
		wxButton* btnAddMap;
		wxButton* btnRemoveMap;
		wxListBox* lstMaps;
		wxButton* btnUploadAll;
		wxGauge* pbUpload;
		wxStaticText* lblDone;
		
		// Virtual event handlers, overide them in your derived class
		virtual void On_btnAddMap_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void On_btnRemoveMap_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void On_btnUploadAll_Click( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		dlgMapUpload( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Upload new map(s) to the server"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 800,420 ), long style = wxDEFAULT_DIALOG_STYLE );
	
};

#endif //__noname__
