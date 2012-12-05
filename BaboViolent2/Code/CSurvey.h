/*
	Copyright 2012 bitHeads inc.

	This file is part of the BaboViolent 2 source code.

	The BaboViolent 2 source code is free software: you can redistribute it and/or 
	modify it under the terms of the GNU General Public License as published by the 
	Free Software Foundation, either version 3 of the License, or (at your option) 
	any later version.

	The BaboViolent 2 source code is distributed in the hope that it will be useful, 
	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
	FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with the 
	BaboViolent 2 source code. If not, see http://www.gnu.org/licenses/.
*/

#ifndef CONSOLE
#ifndef CSURVEY_H_INCLUDED
#define CSURVEY_H_INCLUDED


#include "CListener.h"


class CSurvey : public CListener
{
private:
	CControl * parent;

	//--- Is done
	bool m_isDone;

	//--- Time out
	float m_timeOut;

	// Le son pour quand on clic
	FSOUND_SAMPLE * m_sfxClic;

	// Le son pour quand on passe au dessus
	FSOUND_SAMPLE * m_sfxOver;

	//--- Controls
	CControl * pnl_main;
	CControl * pnl_sending;

	CControl * btn_notNow;
	CControl * btn_never;

	CControl * q_gender[2];
	CControl * q_age[4];
	CControl * q_continent[6]; CControl * q_country[1];
	CControl * q_gameGenre[3]; CControl * q_games[1];
	CControl * q_playingHours[5];
	CControl * q_magasine[1];
	CControl * q_console[7];

	CControl * q_baboPlayingHours[4];
	CControl * q_playingServer[2];
	CControl * q_learnedAboutBabo[1];
	CControl * q_whoYouPlayWith[3];
	CControl * q_whatYouLike[5]; CControl * q_youLikeOther[1];

	CControl * q_BVFuture[4]; CControl * q_futureOther[1];
	CControl * q_Ads[2];
	CControl * q_payAdFree[4];

	CControl * q_email[1];

	CControl * btn_submit;

public:
	//--- Constructor
	CSurvey(CControl * in_parent);

	//--- Destructor
	virtual ~CSurvey();

	//--- Gets
	bool IsDone() const {return m_isDone;}

	//--- Update
	void update(float delay);

	//--- Events
	void MouseEnter(CControl * control);
	void MouseLeave(CControl * control);
	void MouseDown(CControl * control);
	void MouseUp(CControl * control);
	void MouseMove(CControl * control);
	void Click(CControl * control);
};


#endif
#endif
