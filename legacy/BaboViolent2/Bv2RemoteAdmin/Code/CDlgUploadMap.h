#ifndef DLGUPLOADMAP_H_
#define DLGUPLOADMAP_H_

#include "noname.h"
#include "RemoteAdminPackets.h"

#include <wx/timer.h>


class CDlgUploadMap : public dlgMapUpload, public wxTimer
{
private:

	// keep a pointer on our peer id
	int	*	m_peerId;

	// infos on the file we are sending
	FILE	*	m_currentFile;
	long		m_fileSize;
	long		m_fileSizeSent;

	// keep the same file chunk for all sending needs, so we dont new/delte all the time
	net_ra_map_file_chunk	m_chunk;

	// big buffer to hold it all
	char	*	m_chunkAll;

public:

	CDlgUploadMap( int * in_peerId );
	~CDlgUploadMap();

	void Notify();

	// derived from dlgMapUpload
	virtual void On_btnAddMap_Click( wxCommandEvent& event );
	virtual void On_btnRemoveMap_Click( wxCommandEvent& event );
	virtual void On_btnUploadAll_Click( wxCommandEvent& event );
};

#endif
