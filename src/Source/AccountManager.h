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

#ifndef ACCOUNTMANAGER_H_INCLUDED
#define ACCOUNTMANAGER_H_INCLUDED

class AccountManager
{
public:
	static const short loginMaxLength;
	static const short passMaxLength;
	static const short nickMaxLength;
	static const short emailMaxLength;
	static const short clanNameMaxLength;
	static const short clanTagMaxLength;
	static const short websiteMaxLength;

	// account management methods
	virtual bool createAccount(char* login, char* password, char* nick, char* email) = 0;
	virtual bool deleteAccount(int userID, char* password) = 0;
	virtual bool updateAccount(char* login, char* password, char* nick, char* email) = 0;
	virtual bool changePassword(int userID, char* oldPass, char* newPass) = 0;
	virtual bool recoverPassword(char* login) = 0;
	virtual bool loginAccount(char* login, char* password) = 0;
	virtual bool logoutAccount(char* login, char* password) = 0;
	virtual bool userStatusUpdate(int userID, char* password, char* serverName, char* ip, short port) = 0;
	virtual bool registerClan(int userID, char* password, char* name, char* tag, char* email, char* website) = 0;
	virtual bool removeClan(int userID, char* password) = 0;
	virtual bool changeClanPermissions(int userID, char* password, int userIDChanging, char permissions) = 0;
	virtual bool joinClanRequest(int userID, char* password, int userIDDest) = 0;
	virtual bool joinClanAccept(int userID, char* password, int userIDFrom) = 0;
	virtual bool leaveClan(int userID, char* password) = 0;
	virtual bool requestFriend(int userID, char* password, int userIDDest, char* groupName) = 0;
	virtual bool acceptFriend(int userID, char* password, int userIDDest, char* groupName) = 0;
	virtual bool removeFriend(int userID, char* password, int userIDDest) = 0;
	virtual bool moveFriend(int userID, char* password, int userIDDest, char* groupName) = 0;
	virtual bool statsUpdate() = 0;
};

#endif
