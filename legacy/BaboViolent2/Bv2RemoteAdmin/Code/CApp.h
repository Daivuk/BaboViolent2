#ifndef CAPP_H_INCLUDED
#define CAPP_H_INCLUDED


#include <wx/app.h>


class CApp: public wxApp
{
private:
public:
	//--- Constructor
	CApp();

	//--- Destructor
	virtual ~CApp();

	//--- Initialize
	bool OnInit();
};


#endif
