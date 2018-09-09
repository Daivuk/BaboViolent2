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

#ifndef DEDICATED_SERVER
#include "KeyManager.h"



// notre module global
KeyManager keyManager;



//
// Pour obtenir une touche à partir du nom
//
int KeyManager::getKeyByName(CString name)
{
	if (name == "Escape") return DIK_ESCAPE;
	if (name == "1") return DIK_1;
	if (name == "2") return DIK_2;
	if (name == "3") return DIK_3;
	if (name == "4") return DIK_4;
	if (name == "5") return DIK_5;
	if (name == "6") return DIK_6;
	if (name == "7") return DIK_7;
	if (name == "8") return DIK_8;
	if (name == "9") return DIK_9;
	if (name == "0") return DIK_0;
	if (name == "-") return DIK_MINUS;    /* - on main keyboard */
	if (name == "=") return DIK_EQUALS;
	if (name == "Back") return DIK_BACK;    /* backspace */
	if (name == "Tab") return DIK_TAB;
	if (name == "Q") return DIK_Q;
	if (name == "W") return DIK_W;
	if (name == "E") return DIK_E;
	if (name == "R") return DIK_R;
	if (name == "T") return DIK_T;
	if (name == "Y") return DIK_Y;
	if (name == "U") return DIK_U;
	if (name == "I") return DIK_I;
	if (name == "O") return DIK_O;
	if (name == "P") return DIK_P;
	if (name == "[") return DIK_LBRACKET;
	if (name == "]") return DIK_RBRACKET;
	if (name == "Enter") return DIK_RETURN;    /* Enter on main keyboard */
	if (name == "LCtrl") return DIK_LCONTROL;
	if (name == "A") return DIK_A;
	if (name == "S") return DIK_S;
	if (name == "D") return DIK_D;
	if (name == "F") return DIK_F;
	if (name == "G") return DIK_G;
	if (name == "H") return DIK_H;
	if (name == "J") return DIK_J;
	if (name == "K") return DIK_K;
	if (name == "L") return DIK_L;
	if (name == "Semi") return DIK_SEMICOLON;
	if (name == "\'") return DIK_APOSTROPHE;
	if (name == "Grave") return DIK_GRAVE;    /* accent grave */
	if (name == "LShift") return DIK_LSHIFT;
	if (name == "\\") return DIK_BACKSLASH;
	if (name == "Z") return DIK_Z;
	if (name == "X") return DIK_X;
	if (name == "C") return DIK_C;
	if (name == "V") return DIK_V;
	if (name == "B") return DIK_B;
	if (name == "N") return DIK_N;
	if (name == "M") return DIK_M;
	if (name == ",") return DIK_COMMA;
	if (name == ".") return DIK_PERIOD;    /* . on main keyboard */
	if (name == "/") return DIK_SLASH;    /* / on main keyboard */
	if (name == "RShift") return DIK_RSHIFT;
	if (name == "Num *") return DIK_MULTIPLY;    /* * on numeric keypad */
	if (name == "LAlt") return DIK_LMENU;    /* left Alt */
	if (name == "Space") return DIK_SPACE;
	if (name == "Caps") return DIK_CAPITAL;
	if (name == "F1") return DIK_F1;
	if (name == "F2") return DIK_F2;
	if (name == "F3") return DIK_F3;
	if (name == "F4") return DIK_F4;
	if (name == "F5") return DIK_F5;
	if (name == "F6") return DIK_F6;
	if (name == "F7") return DIK_F7;
	if (name == "F8") return DIK_F8;
	if (name == "F9") return DIK_F9;
	if (name == "F10") return DIK_F10;
	if (name == "NumLock") return DIK_NUMLOCK;
	if (name == "Scroll") return DIK_SCROLL;    /* Scroll Lock */
	if (name == "Num 7") return DIK_NUMPAD7;
	if (name == "Num 8") return DIK_NUMPAD8;
	if (name == "Num 9") return DIK_NUMPAD9;
	if (name == "Num -") return DIK_SUBTRACT;    /* - on numeric keypad */
	if (name == "Num 4") return DIK_NUMPAD4;
	if (name == "Num 5") return DIK_NUMPAD5;
	if (name == "Num 6") return DIK_NUMPAD6;
	if (name == "Num +") return DIK_ADD;    /* + on numeric keypad */
	if (name == "Num 1") return DIK_NUMPAD1;
	if (name == "Num 2") return DIK_NUMPAD2;
	if (name == "Num 3") return DIK_NUMPAD3;
	if (name == "Num 0") return DIK_NUMPAD0;
	if (name == "Num .") return DIK_DECIMAL;    /* . on numeric keypad */
	if (name == "OEM 102") return DIK_OEM_102;    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
	if (name == "F11") return DIK_F11;
	if (name == "F12") return DIK_F12;
	if (name == "F13") return DIK_F13;    /*                     (NEC PC98) */
	if (name == "F14") return DIK_F14;    /*                     (NEC PC98) */
	if (name == "F15") return DIK_F15;    /*                     (NEC PC98) */
	if (name == "Kana") return DIK_KANA;    /* (Japanese keyboard)            */
	if (name == "ABNT C1") return DIK_ABNT_C1;    /* /? on Brazilian keyboard */
	if (name == "Conv") return DIK_CONVERT;    /* (Japanese keyboard)            */
	if (name == "Noconv") return DIK_NOCONVERT;    /* (Japanese keyboard)            */
	if (name == "Yen") return DIK_YEN;    /* (Japanese keyboard)            */
	if (name == "ABNT C2") return DIK_ABNT_C2;    /* Numpad . on Brazilian keyboard */
	if (name == "Num =") return DIK_NUMPADEQUALS;    /* = on numeric keypad (NEC PC98) */
	if (name == "Prev") return DIK_PREVTRACK;    /* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
	if (name == "At") return DIK_AT;    /*                     (NEC PC98) */
	if (name == "Colon") return DIK_COLON;    /*                     (NEC PC98) */
	if (name == "UnderLn") return DIK_UNDERLINE;    /*                     (NEC PC98) */
	if (name == "Kanji") return DIK_KANJI;    /* (Japanese keyboard)            */
	if (name == "Stop") return DIK_STOP;    /*                     (NEC PC98) */
	if (name == "Ax") return DIK_AX;    /*                     (Japan AX) */
	if (name == "Unlbl") return DIK_UNLABELED;    /*                        (J3100) */
	if (name == "Next") return DIK_NEXTTRACK;    /* Next Track */
	if (name == "NEnter") return DIK_NUMPADENTER;    /* Enter on numeric keypad */
	if (name == "RCtrl") return DIK_RCONTROL;
	if (name == "Mute") return DIK_MUTE;    /* Mute */
	if (name == "Calc") return DIK_CALCULATOR;    /* Calculator */
	if (name == "Play") return DIK_PLAYPAUSE;    /* Play / Pause */
	if (name == "MStop") return DIK_MEDIASTOP;    /* Media Stop */
	if (name == "Vol-") return DIK_VOLUMEDOWN;    /* Volume - */
	if (name == "Vol+") return DIK_VOLUMEUP;    /* Volume + */
	if (name == "Web") return DIK_WEBHOME;    /* Web home */
	if (name == "Num ,") return DIK_NUMPADCOMMA;    /* , on numeric keypad (NEC PC98) */
	if (name == "Num /") return DIK_DIVIDE;    /* / on numeric keypad */
	if (name == "Sys RQ") return DIK_SYSRQ;
	if (name == "RAlt") return DIK_RMENU;    /* right Alt */
	if (name == "Pause") return DIK_PAUSE;    /* Pause */
	if (name == "Home") return DIK_HOME;    /* Home on arrow keypad */
	if (name == "Up") return DIK_UP;    /* UpArrow on arrow keypad */
	if (name == "PgUp") return DIK_PRIOR;    /* PgUp on arrow keypad */
	if (name == "Left") return DIK_LEFT;    /* LeftArrow on arrow keypad */
	if (name == "Right") return DIK_RIGHT;    /* RightArrow on arrow keypad */
	if (name == "End") return DIK_END;    /* End on arrow keypad */
	if (name == "Down") return DIK_DOWN;    /* DownArrow on arrow keypad */
	if (name == "PgDn") return DIK_NEXT;    /* PgDn on arrow keypad */
	if (name == "Insert") return DIK_INSERT;    /* Insert on arrow keypad */
	if (name == "Delete") return DIK_DELETE;    /* Delete on arrow keypad */
	if (name == "LWin") return DIK_LWIN;    /* Left Windows key */
	if (name == "RWin") return DIK_RWIN;    /* Right Windows key */
	if (name == "Apps") return DIK_APPS;    /* AppMenu key */
	if (name == "Power") return DIK_POWER;    /* System Power */
	if (name == "Sleep") return DIK_SLEEP;    /* System Sleep */
	if (name == "Wake") return DIK_WAKE;    /* System Wake */
	if (name == "Search") return DIK_WEBSEARCH;    /* Web Search */
	if (name == "Fav") return DIK_WEBFAVORITES;    /* Web Favorites */
	if (name == "Refresh") return DIK_WEBREFRESH;    /* Web Refresh */
	if (name == "WebStop") return DIK_WEBSTOP;    /* Web Stop */
	if (name == "Forward") return DIK_WEBFORWARD;    /* Web Forward */
	if (name == "WebBack") return DIK_WEBBACK;    /* Web Back */
	if (name == "MyComp") return DIK_MYCOMPUTER;    /* My Computer */
	if (name == "Mail") return DIK_MAIL;    /* Mail */
	if (name == "MSelect") return DIK_MEDIASELECT;    /* Media Select */

	CString tmp = name;
	tmp.resize(5);
	if (tmp == "Mouse")
	{
		name.resizeInverse(name.len()-5);
		return DKI_MOUSE_BUTTON1 + (name.toInt()-1);
	}

	tmp = name;
	tmp.resize(3);
	if (tmp == "Joy")
	{
		name.resizeInverse(name.len()-3);
		return DKI_JOY_BUTTON1 + (name.toInt()-1);
	}

	return DKI_NOKEY;
}



//
// Pour obtenir le nom d'une touche
//
CString KeyManager::getKeyName(int key)
{
	switch (key)
	{
		case DIK_ESCAPE          :return "Escape";break;
		case DIK_1               :return "1";break;
		case DIK_2               :return "2";break;
		case DIK_3               :return "3";break;
		case DIK_4               :return "4";break;
		case DIK_5               :return "5";break;
		case DIK_6               :return "6";break;
		case DIK_7               :return "7";break;
		case DIK_8               :return "8";break;
		case DIK_9               :return "9";break;
		case DIK_0               :return "0";break;
		case DIK_MINUS           :return "-";break;    /* - on main keyboard */
		case DIK_EQUALS          :return "=";break;
		case DIK_BACK            :return "Back";break;    /* backspace */
		case DIK_TAB             :return "Tab";break;
		case DIK_Q               :return "Q";break;
		case DIK_W               :return "W";break;
		case DIK_E               :return "E";break;
		case DIK_R               :return "R";break;
		case DIK_T               :return "T";break;
		case DIK_Y               :return "Y";break;
		case DIK_U               :return "U";break;
		case DIK_I               :return "I";break;
		case DIK_O               :return "O";break;
		case DIK_P               :return "P";break;
		case DIK_LBRACKET        :return "[";break;
		case DIK_RBRACKET        :return "]";break;
		case DIK_RETURN          :return "Enter";break;    /* Enter on main keyboard */
		case DIK_LCONTROL        :return "LCtrl";break;
		case DIK_A               :return "A";break;
		case DIK_S               :return "S";break;
		case DIK_D               :return "D";break;
		case DIK_F               :return "F";break;
		case DIK_G               :return "G";break;
		case DIK_H               :return "H";break;
		case DIK_J               :return "J";break;
		case DIK_K               :return "K";break;
		case DIK_L               :return "L";break;
		case DIK_SEMICOLON       :return "Semi";break;
		case DIK_APOSTROPHE      :return "\'";break;
		case DIK_GRAVE           :return "Grave";break;    /* accent grave */
		case DIK_LSHIFT          :return "LShift";break;
		case DIK_BACKSLASH       :return "\\";break;
		case DIK_Z               :return "Z";break;
		case DIK_X               :return "X";break;
		case DIK_C               :return "C";break;
		case DIK_V               :return "V";break;
		case DIK_B               :return "B";break;
		case DIK_N               :return "N";break;
		case DIK_M               :return "M";break;
		case DIK_COMMA           :return ",";break;
		case DIK_PERIOD          :return ".";break;    /* . on main keyboard */
		case DIK_SLASH           :return "/";break;    /* / on main keyboard */
		case DIK_RSHIFT          :return "RShift";break;
		case DIK_MULTIPLY        :return "Num *";break;    /* * on numeric keypad */
		case DIK_LMENU           :return "LAlt";break;    /* left Alt */
		case DIK_SPACE           :return "Space";break;
		case DIK_CAPITAL         :return "Caps";break;
		case DIK_F1              :return "F1";break;
		case DIK_F2              :return "F2";break;
		case DIK_F3              :return "F3";break;
		case DIK_F4              :return "F4";break;
		case DIK_F5              :return "F5";break;
		case DIK_F6              :return "F6";break;
		case DIK_F7              :return "F7";break;
		case DIK_F8              :return "F8";break;
		case DIK_F9              :return "F9";break;
		case DIK_F10             :return "F10";break;
		case DIK_NUMLOCK         :return "NumLock";break;
		case DIK_SCROLL          :return "Scroll";break;    /* Scroll Lock */
		case DIK_NUMPAD7         :return "Num 7";break;
		case DIK_NUMPAD8         :return "Num 8";break;
		case DIK_NUMPAD9         :return "Num 9";break;
		case DIK_SUBTRACT        :return "Num -";break;    /* - on numeric keypad */
		case DIK_NUMPAD4         :return "Num 4";break;
		case DIK_NUMPAD5         :return "Num 5";break;
		case DIK_NUMPAD6         :return "Num 6";break;
		case DIK_ADD             :return "Num +";break;    /* + on numeric keypad */
		case DIK_NUMPAD1         :return "Num 1";break;
		case DIK_NUMPAD2         :return "Num 2";break;
		case DIK_NUMPAD3         :return "Num 3";break;
		case DIK_NUMPAD0         :return "Num 0";break;
		case DIK_DECIMAL         :return "Num .";break;    /* . on numeric keypad */
		case DIK_OEM_102         :return "OEM 102";break;    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
		case DIK_F11             :return "F11";break;
		case DIK_F12             :return "F12";break;
		case DIK_F13             :return "F13";break;    /*                     (NEC PC98) */
		case DIK_F14             :return "F14";break;    /*                     (NEC PC98) */
		case DIK_F15             :return "F15";break;    /*                     (NEC PC98) */
		case DIK_KANA            :return "Kana";break;    /* (Japanese keyboard)            */
		case DIK_ABNT_C1         :return "ABNT C1";break;    /* /? on Brazilian keyboard */
		case DIK_CONVERT         :return "Conv";break;    /* (Japanese keyboard)            */
		case DIK_NOCONVERT       :return "Noconv";break;    /* (Japanese keyboard)            */
		case DIK_YEN             :return "Yen";break;    /* (Japanese keyboard)            */
		case DIK_ABNT_C2         :return "ABNT C2";break;    /* Numpad . on Brazilian keyboard */
		case DIK_NUMPADEQUALS    :return "Num =";break;    /* = on numeric keypad (NEC PC98) */
		case DIK_PREVTRACK       :return "Prev";break;    /* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
		case DIK_AT              :return "At";break;    /*                     (NEC PC98) */
		case DIK_COLON           :return "Colon";break;    /*                     (NEC PC98) */
		case DIK_UNDERLINE       :return "UnderLn";break;    /*                     (NEC PC98) */
		case DIK_KANJI           :return "Kanji";break;    /* (Japanese keyboard)            */
		case DIK_STOP            :return "Stop";break;    /*                     (NEC PC98) */
		case DIK_AX              :return "Ax";break;    /*                     (Japan AX) */
		case DIK_UNLABELED       :return "Unlbl";break;    /*                        (J3100) */
		case DIK_NEXTTRACK       :return "Next";break;    /* Next Track */
		case DIK_NUMPADENTER     :return "NEnter";break;    /* Enter on numeric keypad */
		case DIK_RCONTROL        :return "RCtrl";break;
		case DIK_MUTE            :return "Mute";break;    /* Mute */
		case DIK_CALCULATOR      :return "Calc";break;    /* Calculator */
		case DIK_PLAYPAUSE       :return "Play";break;    /* Play / Pause */
		case DIK_MEDIASTOP       :return "MStop";break;    /* Media Stop */
		case DIK_VOLUMEDOWN      :return "Vol-";break;    /* Volume - */
		case DIK_VOLUMEUP        :return "Vol+";break;    /* Volume + */
		case DIK_WEBHOME         :return "Web";break;    /* Web home */
		case DIK_NUMPADCOMMA     :return "Num ,";break;    /* , on numeric keypad (NEC PC98) */
		case DIK_DIVIDE          :return "Num /";break;    /* / on numeric keypad */
		case DIK_SYSRQ           :return "Sys RQ";break;
		case DIK_RMENU           :return "RAlt";break;    /* right Alt */
		case DIK_PAUSE           :return "Pause";break;    /* Pause */
		case DIK_HOME            :return "Home";break;    /* Home on arrow keypad */
		case DIK_UP              :return "Up";break;    /* UpArrow on arrow keypad */
		case DIK_PRIOR           :return "PgUp";break;    /* PgUp on arrow keypad */
		case DIK_LEFT            :return "Left";break;    /* LeftArrow on arrow keypad */
		case DIK_RIGHT           :return "Right";break;    /* RightArrow on arrow keypad */
		case DIK_END             :return "End";break;    /* End on arrow keypad */
		case DIK_DOWN            :return "Down";break;    /* DownArrow on arrow keypad */
		case DIK_NEXT            :return "PgDn";break;    /* PgDn on arrow keypad */
		case DIK_INSERT          :return "Insert";break;    /* Insert on arrow keypad */
		case DIK_DELETE          :return "Delete";break;    /* Delete on arrow keypad */
		case DIK_LWIN            :return "LWin";break;    /* Left Windows key */
		case DIK_RWIN            :return "RWin";break;    /* Right Windows key */
		case DIK_APPS            :return "Apps";break;    /* AppMenu key */
		case DIK_POWER           :return "Power";break;    /* System Power */
		case DIK_SLEEP           :return "Sleep";break;    /* System Sleep */
		case DIK_WAKE            :return "Wake";break;    /* System Wake */
		case DIK_WEBSEARCH       :return "Search";break;    /* Web Search */
		case DIK_WEBFAVORITES    :return "Fav";break;    /* Web Favorites */
		case DIK_WEBREFRESH      :return "Refresh";break;    /* Web Refresh */
		case DIK_WEBSTOP         :return "WebStop";break;    /* Web Stop */
		case DIK_WEBFORWARD      :return "Forward";break;    /* Web Forward */
		case DIK_WEBBACK         :return "WebBack";break;    /* Web Back */
		case DIK_MYCOMPUTER      :return "MyComp";break;    /* My Computer */
		case DIK_MAIL            :return "Mail";break;    /* Mail */
		case DIK_MEDIASELECT     :return "MSelect";break;    /* Media Select */
	}

	if (key >= DKI_MOUSE_BUTTON1 && key <= DKI_MOUSE_BUTTON8)
	{
		return CString("Mouse%i", key-255);
	}

	if (key >= DKI_JOY_BUTTON1)
	{
		return CString("Joy%i", key - 263);
	}

	return "NOT SET";
}
#endif

