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

#ifndef CONSOLE_H
#define CONSOLE_H

#ifndef WIN32
	#include "LinuxHeader.h"
#endif

#include "Zeven.h"

#ifndef DEDICATED_SERVER
#include "Writting.h"
#endif
#include <vector>


#ifndef DEDICATED_SERVER
#define CONSOLE_MAX_RECOGNITION_VAR 10
#endif


class Console
{
private:
	// Le fichier pour le output automatique
	CString m_outputFilename;

	// Toute notre texte
	std::vector<CString> m_eventMessages;
	std::vector<CString> m_chatMessages;

	// Max number of messages kept in m_messages
	const int m_maxMsgHistorySize;
	int m_visibleMsgOffset;

	// What to display in console
	// Do not set by hand! Use SetDisplayEvents(bool b)!
	bool displayEvents;

	// Buffer for past commands
	std::vector<CString> m_cmdHistory;

	// Max number of commands kept in m_cmdHistory
	const int m_maxCmdHistorySize;

	const int m_historyMod;

	int m_cmdHistoryID;

	// commands that are not saved in log file (sending log with admin pass is not good ;))
	std::vector<CString> m_excludeFromLog;

#ifndef DEDICATED_SERVER
	// Sa font
	unsigned int m_font;

	// Si la console est active ou non
	bool m_isActive;

	// Sa position verticale
	float m_vPos;

	// Le string courrant, qu'on est entrein de taper
	Writting * m_currentText;

	// Pour tenir les 10 premier trouv�dans les var recognition
	char ** recognitionVar;
	bool showRecognitionVar;
	CString lastRecognitionVar;
	int curRecognitionVar;
	CString m_lastToken;

	// Pour pas qu'on utilise la console (meton qu'on est apres chatter
	bool locked;
#endif

public:
	// Constructeurs
	Console();
	Console(CString outputFilename);

	// Destructeur
	virtual ~Console();

	// Pour ajouter une ligne
	void add(CString message, bool fromServer=false, bool isEvent=true);
    
	// Pour outputer dans un fichier (log meton)
	void output(CString filename);
#ifndef DEDICATED_SERVER
	// Pour afficher la console
	void render();
#endif
	// Pour updater la console (meton taper dedans, etc)
	void update(float delay);

	// Pour la reseter, efface son contenu
	void reset();

	// Pour initialiser la console
	void init();
#ifndef DEDICATED_SERVER
	// Pour savoir si la console est active ou non, si cest le cas, on block les inputs ailleur
	bool isActive() {return m_isActive;}
#endif
	// Pour envoyer une commande ala console
	void sendCommand(CString commandLine, bool admin=false, unsigned long bbnetID = -1);

	// Pour uniquement modifier une variable, c'est comme le set
	void svChange(CString command);

	// Pour outputer l'erreur de la BB
	void debugBBNET(bool client,bool server);
#ifndef DEDICATED_SERVER
	// Pour la locker
	void lock() {locked = true;}
	void unlock() {locked = false;}
#endif

	void SetDisplayEvents(bool b);

private:
	const std::vector<CString>& GetActiveMessages();
};


extern Console * console;


#endif

