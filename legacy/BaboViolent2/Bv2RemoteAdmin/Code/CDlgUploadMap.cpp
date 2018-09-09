#include "CDlgUploadMap.h"
#include "baboNet.h"

#include <wx/filedlg.h>
#include <wx/msgdlg.h>

#define FILE_CHUNK_SIZE	255

CDlgUploadMap::CDlgUploadMap( int * in_peerId )
	: dlgMapUpload( NULL )
{
	m_peerId = in_peerId;
	
	m_currentFile = 0;
	m_fileSize = 0;
	m_fileSizeSent = 0;

	m_chunk.chunkData = 0;
	m_chunk.chunkSize = 0;

	lblDone->SetLabel("");

	m_chunkAll = new char[1024];
}

CDlgUploadMap::~CDlgUploadMap()
{
	delete [] m_chunkAll;
	Destroy();
}

void CDlgUploadMap::On_btnAddMap_Click( wxCommandEvent& event )
{
	wxFileDialog * fd = new wxFileDialog( this, "Select map(s)", "", "", "*.bvm", wxMULTIPLE );
	fd->ShowModal();
	wxArrayString paths;
	fd->GetPaths( paths );
	for( unsigned int i=0; i<paths.Count(); i++ )
	{
		lstMaps->AppendString( paths[i] );
	}
	delete fd;
}

void CDlgUploadMap::On_btnRemoveMap_Click( wxCommandEvent& event )
{
	int selected = lstMaps->GetSelection();
	if( selected == wxNOT_FOUND ) return;
	lstMaps->Delete( selected );
}

void CDlgUploadMap::Notify()
{
	if( *m_peerId == 0 )
	{
		// oups connection problems ?
		wxMessageBox( wxT("Connection interrupted during transfert, aborting"), wxT("Error"));
		Stop();
		btnUploadAll->Enable();
		return;
	}

	// if no files are currently being sent, let's open the next showing on the list
	if( !m_currentFile )
	{
		if( lstMaps->GetCount() == 0 )
		{
			// we are finished sending map files
			Stop();
			btnUploadAll->Enable();
			lblDone->SetLabel("All maps successfully uploaded to server!");
			return;
		}
		else
		{
			// take the next file
			m_currentFile = fopen( lstMaps->GetString( 0 ).c_str(), "rb" );
			m_fileSizeSent = 0;

			fseek( m_currentFile, 0, SEEK_END );
			m_fileSize = ftell(m_currentFile);
			rewind(m_currentFile);

			// send the file header to the server
			net_ra_map_file_header header;
			wxString mapNameWithoutPath = lstMaps->GetString(0).AfterLast('\\');
			if( mapNameWithoutPath == lstMaps->GetString(0) )
			{
				// oups no '\' we're found try with '/'
				mapNameWithoutPath = lstMaps->GetString(0).AfterLast('/');
			}

			sprintf( header.fileName, "%s", mapNameWithoutPath.c_str() );
			header.fileSize = m_fileSize;

			bb_peerSend( *m_peerId, (char*)&header, RA_MAP_FILE_HEADER, sizeof(net_ra_map_file_header), true );

			// if we never allocated memory for our chunk, do it here
			if( !m_chunk.chunkData )
			{
				m_chunk.chunkData = new char[1024];
			}
		}
	}
	else
	{
		// already sending a file, send next chunk!
		int remaining = m_fileSizeSent + FILE_CHUNK_SIZE > m_fileSize ? m_fileSize - m_fileSizeSent : FILE_CHUNK_SIZE;

		// parse data
        m_chunk.chunkSize = (long)remaining;
		fread(m_chunk.chunkData, remaining, 1, m_currentFile);

		memcpy( m_chunkAll, &(m_chunk.chunkSize), 4 );
		memcpy( m_chunkAll+4, m_chunk.chunkData, remaining );

		// send the file chunk
		bb_peerSend( *m_peerId, m_chunkAll, RA_MAP_FILE_CHUNK, 4 + remaining, true );

		m_fileSizeSent += remaining;

		if( m_fileSizeSent == m_fileSize )
		{
			lblDone->SetLabel( lstMaps->GetString(0) + " Done!" );
			lstMaps->Delete(0);
			fclose(m_currentFile);
			m_currentFile = 0;
		}
		
	}

	// update progress bar!
	pbUpload->SetValue( (int)(((float)m_fileSizeSent / (float)m_fileSize) * 100.0f ) );

	// sleep a bit
	wxMilliSleep( 5 );
}

void CDlgUploadMap::On_btnUploadAll_Click( wxCommandEvent& event )
{
	if( lstMaps->GetCount() == 0 ) return;
	if( *m_peerId == 0 ) return;
	btnUploadAll->Disable();
	pbUpload->SetValue( 0 );
	pbUpload->Show();
    	
	// start timer at 30 fps!
	Start( 33 );

	
}
