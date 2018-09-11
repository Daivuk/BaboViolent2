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

#include "dki.h"

static const std::vector < std::pair<const char*, int> > KEY_NAMES = {
    {"Escape", KeyEscape},
    {"1", Key1},
    {"2", Key2},
    {"3", Key3},
    {"4", Key4},
    {"5", Key5},
    {"6", Key6},
    {"7", Key7},
    {"8", Key8},
    {"9", Key9},
    {"0", Key0},
    {"-", KeyMinus},    /* - on main keyboard */
    {"=", KeyEquals},
    {"Back", KeyBackspace},    /* backspace */
    {"Tab", KeyTab},
    {"Q", KeyQ},
    {"W", KeyW},
    {"E", KeyE},
    {"R", KeyR},
    {"T", KeyT},
    {"Y", KeyY},
    {"U", KeyU},
    {"I", KeyI},
    {"O", KeyO},
    {"P", KeyP},
    {"[", KeyLeftBracket},
    {"]", KeyRightBracket},
    {"Enter", KeyEnter},    /* Enter on main keyboard */
    {"LCtrl", KeyLeftControl},
    {"A", KeyA},
    {"S", KeyS},
    {"D", KeyD},
    {"F", KeyF},
    {"G", KeyG},
    {"H", KeyH},
    {"J", KeyJ},
    {"K", KeyK},
    {"L", KeyL},
    {"Semi", KeySemiColon},
    {"\'", KeyApostrophe},
    {"Grave", KeyGrave},    /* accent grave */
    {"LShift", KeyLeftShift},
    {"\\", KeyBackslash},
    {"Z", KeyZ},
    {"X", KeyX},
    {"C", KeyC},
    {"V", KeyV},
    {"B", KeyB},
    {"N", KeyN},
    {"M", KeyM},
    {",", KeyComma},
    {".", KeyPeriod},    /* . on main keyboard */
    {"/", KeySlash},    /* / on main keyboard */
    {"RShift", KeyRightShift},
    {"Num *", KeyMultiply},    /* * on numeric keypad */
    {"LAlt", KeyLeftAlt},    /* left Alt */
    {"Space", KeySpaceBar},
    {"Caps", KeyCapsLock},
    {"F1", KeyF1},
    {"F2", KeyF2},
    {"F3", KeyF3},
    {"F4", KeyF4},
    {"F5", KeyF5},
    {"F6", KeyF6},
    {"F7", KeyF7},
    {"F8", KeyF8},
    {"F9", KeyF9},
    {"F10", KeyF10},
    {"NumLock", KeyNumLock},
    {"Scroll", KeyScrollLock},    /* Scroll Lock */
    {"Num 7", KeyNumPad7},
    {"Num 8", KeyNumPad8},
    {"Num 9", KeyNumPad9},
    {"Num -", KeyNumPadMinus},    /* - on numeric keypad */
    {"Num 4", KeyNumPad4},
    {"Num 5", KeyNumPad5},
    {"Num 6", KeyNumPad6},
    {"Num +", KeyNumPadAdd},    /* + on numeric keypad */
    {"Num 1", KeyNumPad1},
    {"Num 2", KeyNumPad2},
    {"Num 3", KeyNumPad3},
    {"Num 0", KeyNumPad0},
    {"Num .", KeyNumPadPeriod},    /* . on numeric keypad */
                                                    //{"OEM 102", SDLK_OEM_102},    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
    {"F11", KeyF11},
    {"F12", KeyF12},
    {"F13", KeyF13},    /*                     (NEC PC98) */
    {"F14", KeyF14},    /*                     (NEC PC98) */
    {"F15", KeyF15},    /*                     (NEC PC98) */
                                           //{"Kana", SDLK_KANA},    /* (Japanese keyboard)            */
                                           //{"ABNT C1", SDLK_ABNT_C1},    /* /? on Brazilian keyboard */
                                           //{"Conv", SDLK_CONVERT},    /* (Japanese keyboard)            */
                                           //{"Noconv", SDLK_NOCONVERT},    /* (Japanese keyboard)            */
                                           //{"Yen", SDLK_YEN},    /* (Japanese keyboard)            */
                                           //{"ABNT C2", SDLK_ABNT_C2},    /* Numpad . on Brazilian keyboard */
    {"Num =", KeyNumPadEquals},    /* = on numeric keypad (NEC PC98) */
                                                   //{"Prev", SDLK_PREVTRACK},    /* Previous Track (SDLK_CIRCUMFLEX on Japanese keyboard) */
    {"At", KeyAt},    /*                     (NEC PC98) */
    {"Colon", KeyColon},    /*                     (NEC PC98) */
                                               //{"UnderLn", SDLK_UNDERLINE},    /*                     (NEC PC98) */
                                               //{"Kanji", SDLK_KANJI},    /* (Japanese keyboard)            */
    {"Stop", KeyStop},    /*                     (NEC PC98) */
                                             //{"Ax", SDLK_AX},    /*                     (Japan AX) */
                                             //{"Unlbl", SDLK_UNLABELED},    /*                        (J3100) */
                                             //{"Next", SDLK_NEXTTRACK},    /* Next Track */
    {"NEnter", KeyNumPadEnter},    /* Enter on numeric keypad */
    {"RCtrl", KeyRightControl},
    {"Mute", KeyMute},    /* Mute */
    {"Calc", KeyCalculator},    /* Calculator */
    {"Play", KeyPlayPause},    /* Play / Pause */
    {"MStop", KeyStop},    /* Media Stop */
    {"Vol-", KeyVolumeDown},    /* Volume - */
    {"Vol+", KeyVolumeUp},    /* Volume + */
    {"Web", KeyWebHome},    /* Web home */
    {"Num ,", KeyNumPadComma},    /* , on numeric keypad (NEC PC98) */
    {"Num /", KeyNumPadDivide},    /* / on numeric keypad */
                                                   //{"Sys RQ", SDLK_SYSRQ},
    {"RAlt", KeyRightAlt},    /* right Alt */
    {"Pause", KeyPause},    /* Pause */
    {"Home", KeyHome},    /* Home on arrow keypad */
    {"Up", KeyUp},    /* UpArrow on arrow keypad */
    {"PgUp", KeyPageUp},    /* PgUp on arrow keypad */
    {"Left", KeyLeft},    /* LeftArrow on arrow keypad */
    {"Right", KeyRight},    /* RightArrow on arrow keypad */
    {"End", KeyEnd},    /* End on arrow keypad */
    {"Down", KeyDown},    /* DownArrow on arrow keypad */
    {"PgDn", KeyPageDown},    /* PgDn on arrow keypad */
    {"Insert", KeyInsert},    /* Insert on arrow keypad */
    {"Delete", KeyDelete},    /* Delete on arrow keypad */
                                                 //{"LWin", SDLK_LSU},    /* Left Windows key */
                                                 //{"RWin", SDLK_RWIN},    /* Right Windows key */
                                                 //{"Apps", SDLK_APPS},    /* AppMenu key */
    {"Power", KeyPower},    /* System Power */
    {"Sleep", KeySleep},    /* System Sleep */
                                               //{"Wake", SDLK_WAKE},    /* System Wake */
                                               //{"Search", SDLK_WEBSEARCH},    /* Web Search */
                                               //{"Fav", SDLK_WEBFAVORITES},    /* Web Favorites */
                                               //{"Refresh", SDLK_WEBREFRESH},    /* Web Refresh */
                                               //{"WebStop", SDLK_WEBSTOP},    /* Web Stop */
                                               //{"Forward", SDLK_WEBFORWARD},    /* Web Forward */
                                               //{"WebBack", SDLK_WEBBACK},    /* Web Back */
                                               //{"MyComp", SDLK_MYCOMPUTER},    /* My Computer */
    {"Mail", KeyMail},    /* Mail */
    {"MSelect", KeyMediaSelect},    /* Media Select */
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

