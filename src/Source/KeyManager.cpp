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


#include <vector>

static const std::vector < std::pair<const char*, int> > KEY_NAMES = {
    {"Escape", SDLK_ESCAPE},
    {"1", SDLK_1},
    {"2", SDLK_2},
    {"3", SDLK_3},
    {"4", SDLK_4},
    {"5", SDLK_5},
    {"6", SDLK_6},
    {"7", SDLK_7},
    {"8", SDLK_8},
    {"9", SDLK_9},
    {"0", SDLK_0},
    {"-", SDLK_MINUS},    /* - on main keyboard */
    {"=", SDLK_EQUALS},
    {"Back", SDLK_BACKSPACE},    /* backspace */
    {"Tab", SDLK_TAB},
    {"Q", SDLK_q},
    {"W", SDLK_w},
    {"E", SDLK_e},
    {"R", SDLK_r},
    {"T", SDLK_t},
    {"Y", SDLK_y},
    {"U", SDLK_u},
    {"I", SDLK_i},
    {"O", SDLK_o},
    {"P", SDLK_p},
    {"[", SDLK_LEFTBRACKET},
    {"]", SDLK_RIGHTBRACKET},
    {"Enter", SDLK_RETURN},    /* Enter on main keyboard */
    {"LCtrl", SDLK_LCTRL},
    {"A", SDLK_a},
    {"S", SDLK_s},
    {"D", SDLK_d},
    {"F", SDLK_f},
    {"G", SDLK_g},
    {"H", SDLK_h},
    {"J", SDLK_j},
    {"K", SDLK_k},
    {"L", SDLK_l},
    {"Semi", SDLK_SEMICOLON},
    {"\'", SDLK_QUOTE},
    {"Grave", SDLK_BACKQUOTE},    /* accent grave */
    {"LShift", SDLK_LSHIFT},
    {"\\", SDLK_BACKSLASH},
    {"Z", SDLK_z},
    {"X", SDLK_x},
    {"C", SDLK_c},
    {"V", SDLK_v},
    {"B", SDLK_b},
    {"N", SDLK_n},
    {"M", SDLK_m},
    {",", SDLK_COMMA},
    {".", SDLK_PERIOD},    /* . on main keyboard */
    {"/", SDLK_SLASH},    /* / on main keyboard */
    {"RShift", SDLK_RSHIFT},
    {"Num *", SDLK_KP_MULTIPLY},    /* * on numeric keypad */
    {"LAlt", SDLK_LALT},    /* left Alt */
    {"Space", SDLK_SPACE},
    {"Caps", SDLK_CAPSLOCK},
    {"F1", SDLK_F1},
    {"F2", SDLK_F2},
    {"F3", SDLK_F3},
    {"F4", SDLK_F4},
    {"F5", SDLK_F5},
    {"F6", SDLK_F6},
    {"F7", SDLK_F7},
    {"F8", SDLK_F8},
    {"F9", SDLK_F9},
    {"F10", SDLK_F10},
    {"NumLock", SDLK_NUMLOCKCLEAR},
    {"Scroll", SDLK_SCROLLLOCK},    /* Scroll Lock */
    {"Num 7", SDLK_KP_7},
    {"Num 8", SDLK_KP_8},
    {"Num 9", SDLK_KP_9},
    {"Num -", SDLK_KP_MINUS},    /* - on numeric keypad */
    {"Num 4", SDLK_KP_4},
    {"Num 5", SDLK_KP_5},
    {"Num 6", SDLK_KP_6},
    {"Num +", SDLK_KP_PLUS},    /* + on numeric keypad */
    {"Num 1", SDLK_KP_1},
    {"Num 2", SDLK_KP_2},
    {"Num 3", SDLK_KP_3},
    {"Num 0", SDLK_KP_0},
    {"Num .", SDLK_KP_DECIMAL},    /* . on numeric keypad */
                                                    //{"OEM 102", SDLK_OEM_102},    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
    {"F11", SDLK_F11},
    {"F12", SDLK_F12},
    {"F13", SDLK_F13},    /*                     (NEC PC98) */
    {"F14", SDLK_F14},    /*                     (NEC PC98) */
    {"F15", SDLK_F15},    /*                     (NEC PC98) */
                                           //{"Kana", SDLK_KANA},    /* (Japanese keyboard)            */
                                           //{"ABNT C1", SDLK_ABNT_C1},    /* /? on Brazilian keyboard */
                                           //{"Conv", SDLK_CONVERT},    /* (Japanese keyboard)            */
                                           //{"Noconv", SDLK_NOCONVERT},    /* (Japanese keyboard)            */
                                           //{"Yen", SDLK_YEN},    /* (Japanese keyboard)            */
                                           //{"ABNT C2", SDLK_ABNT_C2},    /* Numpad . on Brazilian keyboard */
    {"Num =", SDLK_KP_EQUALS},    /* = on numeric keypad (NEC PC98) */
                                                   //{"Prev", SDLK_PREVTRACK},    /* Previous Track (SDLK_CIRCUMFLEX on Japanese keyboard) */
    {"At", SDLK_AT},    /*                     (NEC PC98) */
    {"Colon", SDLK_COLON},    /*                     (NEC PC98) */
                                               //{"UnderLn", SDLK_UNDERLINE},    /*                     (NEC PC98) */
                                               //{"Kanji", SDLK_KANJI},    /* (Japanese keyboard)            */
    {"Stop", SDLK_STOP},    /*                     (NEC PC98) */
                                             //{"Ax", SDLK_AX},    /*                     (Japan AX) */
                                             //{"Unlbl", SDLK_UNLABELED},    /*                        (J3100) */
                                             //{"Next", SDLK_NEXTTRACK},    /* Next Track */
    {"NEnter", SDLK_KP_ENTER},    /* Enter on numeric keypad */
    {"RCtrl", SDLK_RCTRL},
    {"Mute", SDLK_MUTE},    /* Mute */
    {"Calc", SDLK_CALCULATOR},    /* Calculator */
    {"Play", SDLK_AUDIOPLAY},    /* Play / Pause */
    {"MStop", SDLK_AUDIOSTOP},    /* Media Stop */
    {"Vol-", SDLK_VOLUMEDOWN},    /* Volume - */
    {"Vol+", SDLK_VOLUMEUP},    /* Volume + */
    {"Web", SDLK_AC_HOME},    /* Web home */
    {"Num ,", SDLK_KP_COMMA},    /* , on numeric keypad (NEC PC98) */
    {"Num /", SDLK_KP_DIVIDE},    /* / on numeric keypad */
                                                   //{"Sys RQ", SDLK_SYSRQ},
    {"RAlt", SDLK_RALT},    /* right Alt */
    {"Pause", SDLK_PAUSE},    /* Pause */
    {"Home", SDLK_HOME},    /* Home on arrow keypad */
    {"Up", SDLK_UP},    /* UpArrow on arrow keypad */
    {"PgUp", SDLK_PRIOR},    /* PgUp on arrow keypad */
    {"Left", SDLK_LEFT},    /* LeftArrow on arrow keypad */
    {"Right", SDLK_RIGHT},    /* RightArrow on arrow keypad */
    {"End", SDLK_END},    /* End on arrow keypad */
    {"Down", SDLK_DOWN},    /* DownArrow on arrow keypad */
    {"PgDn", SDLK_PAGEDOWN},    /* PgDn on arrow keypad */
    {"Insert", SDLK_INSERT},    /* Insert on arrow keypad */
    {"Delete", SDLK_DELETE},    /* Delete on arrow keypad */
                                                 //{"LWin", SDLK_LSU},    /* Left Windows key */
                                                 //{"RWin", SDLK_RWIN},    /* Right Windows key */
                                                 //{"Apps", SDLK_APPS},    /* AppMenu key */
    {"Power", SDLK_POWER},    /* System Power */
    {"Sleep", SDLK_SLEEP},    /* System Sleep */
                                               //{"Wake", SDLK_WAKE},    /* System Wake */
                                               //{"Search", SDLK_WEBSEARCH},    /* Web Search */
                                               //{"Fav", SDLK_WEBFAVORITES},    /* Web Favorites */
                                               //{"Refresh", SDLK_WEBREFRESH},    /* Web Refresh */
                                               //{"WebStop", SDLK_WEBSTOP},    /* Web Stop */
                                               //{"Forward", SDLK_WEBFORWARD},    /* Web Forward */
                                               //{"WebBack", SDLK_WEBBACK},    /* Web Back */
                                               //{"MyComp", SDLK_MYCOMPUTER},    /* My Computer */
    {"Mail", SDLK_MAIL},    /* Mail */
    {"MSelect", SDLK_MEDIASELECT},    /* Media Select */
};


//
// Pour obtenir une touche à partir du nom
//
int KeyManager::getKeyByName(CString name)
{
    for (const auto& p : KEY_NAMES)
    {
        if (name == p.first) return p.second;
    }

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
    for (const auto& p : KEY_NAMES)
    {
        if (p.second == key) return (char*)p.first;
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

