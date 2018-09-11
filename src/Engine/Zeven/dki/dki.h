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

/// \brief Module de gestion des controles (clavier, souris, manette de jeu, joystick)
///
/// \file dki.h
/// Ce module prend en charge la gestion des �v�nements syst�me relevant de l'�tat de chaque touche du clavier, de la position de la souris, de chaque bouton de la souris et possiblement, la position de chaque axe d'un joystick ou d'une manette de jeu et de chaque boutons d'un joystick ou d'une manette de jeu.
/// Ceci comprend:
/// 	- une fonction d'initialisation du module
/// 	- une fonction de terminaison du module
/// 	- une fonction de mise � jour du module
/// 	- des fonctions permettant d'obtenir l'�tat de chaque touche, boutons, axe ou position d'un clavier, d'un joystick ou d'une souris.
///
/// \note Ce module n�cessite Direct Input pour fonctionner
/// \author David St-Louis (alias Daivuk)
/// \author Louis Poirier (� des fins de documentation seulement)
///


#ifndef DKI_H
#define DKI_H


#include "CVector.h"

#include <SDL.h>


// Les �tats des touches
/// \name �tats des touches et boutons possibles
/// Dans le cas normal, la s�quence d'�tats pour une touche qui est appuy� pendant un certain temps sera :
/// ..., DKI_NOTHING, DKI_NOTHING,(l'�v�nement d'enfoncement de la touche se produit ici) DKI_DOWN, DKI_HOLD, DKI_HOLD, ..., DKI_HOLD, DKI_HOLD, (l'�v�nement de relachement de la touche se produit ici)DKI_UP, DKI_NOTHING, DKI_NOTHING,...
//@{
/// utilis�e pour d�signer qu'aucune touche n'a encore �t� pes� (voir dkiGetFirstDown())
const int DKI_NOKEY = -1;
/// utilis�e pour d�signer qu'une touche n'est pas appuy�e
const int DKI_NOTHING = 0;
/// utilis�e pour d�signer qu'une touche vient d'�tre enfonc�e. Cet �tat est pr�sent seulement une fois pour toute la dur�e de l'enfoncement d'une touche.
const int DKI_DOWN = 1;
/// utilis�e pour d�signer qu'une touche est maintenue enfonc�e. Cet �tat est pr�sent tant que la touche reste enfonc�.
const int DKI_HOLD = 2;
/// utilis�e pour d�signer qu'une touche vient d'�tre relach�e. Cet �tat est pr�sent seulement une fois d�s que la touche a �t� relach�e.
const int DKI_UP = 3;
/// non utilis�
const int DKI_CLIC = 3;
/// non utilis�
const int DKI_DBL_CLIC = 4;
//@}

enum DkiKey : int
{
    None = 0,

#if 1 // SDL2 scancode map
    KeyEscape = 41,
    Key1 = 30,
    Key2 = 31,
    Key3 = 32,
    Key4 = 33,
    Key5 = 34,
    Key6 = 35,
    Key7 = 36,
    Key8 = 37,
    Key9 = 38,
    Key0 = 39,
    KeyMinus = 45, /* - on main keyboard */
    KeyEquals = 46,
    KeyBackspace = 42, /* backspace */
    KeyTab = 43,
    KeyQ = 20,
    KeyW = 26,
    KeyE = 8,
    KeyR = 21,
    KeyT = 23,
    KeyY = 28,
    KeyU = 24,
    KeyI = 12,
    KeyO = 18,
    KeyP = 19,
    KeyLeftBracket = 47,
    KeyRightBracket = 48,
    KeyEnter = 40, /* Enter on main keyboard */
    KeyLeftControl = 224,
    KeyA = 4,
    KeyS = 22,
    KeyD = 7,
    KeyF = 9,
    KeyG = 10,
    KeyH = 11,
    KeyJ = 13,
    KeyK = 14,
    KeyL = 15,
    KeySemiColon = 51,
    KeyApostrophe = 52,
    KeyGrave = 53,    /* accent grave */
    KeyLeftShift = 225,
    KeyBackslash = 49,
    KeyZ = 29,
    KeyX = 27,
    KeyC = 6,
    KeyV = 25,
    KeyB = 5,
    KeyN = 17,
    KeyM = 16,
    KeyComma = 54,
    KeyPeriod = 55, /* . on main keyboard */
    KeySlash = 56, /* / on main keyboard */
    KeyRightShift = 229,
    KeyMultiply = 85, /* * on numeric keypad */
    KeyLeftAlt = 226, /* left Alt */
    KeySpaceBar = 44,
    KeyCapsLock = 57,
    KeyF1 = 58,
    KeyF2 = 59,
    KeyF3 = 60,
    KeyF4 = 61,
    KeyF5 = 62,
    KeyF6 = 63,
    KeyF7 = 64,
    KeyF8 = 65,
    KeyF9 = 66,
    KeyF10 = 67,
    KeyNumLock = 83,
    KeyScrollLock = 71, /* Scroll Lock */
    KeyNumPad7 = 95,
    KeyNumPad8 = 96,
    KeyNumPad9 = 97,
    KeyNumPadMinus = 86, /* - on numeric keypad */
    KeyNumPad4 = 92,
    KeyNumPad5 = 93,
    KeyNumPad6 = 94,
    KeyNumPadAdd = 87, /* + on numeric keypad */
    KeyNumPad1 = 89,
    KeyNumPad2 = 90,
    KeyNumPad3 = 91,
    KeyNumPad0 = 98,
    KeyNumPadPeriod = 99, /* . on numeric keypad */
    KeyF11 = 68,
    KeyF12 = 69,
    KeyF13 = 104, /* (NEC PC98) */
    KeyF14 = 105, /* (NEC PC98) */
    KeyF15 = 106, /* (NEC PC98) */
    KeyNumPadEquals = 103, /* = on numeric keypad (NEC PC98) */
    KeyPreviousTrack = 259, /* Previous Track (OINPUT_CIRCUMFLEX on Japanese keyboard) */
    KeyAt = 206, /* (NEC PC98) */
    KeyColon = 203, /* (NEC PC98) */
    KeyStop = 120, /* (NEC PC98) */
    KeyNextTrack = 258, /* Next Track */
    KeyNumPadEnter = 88, /* Enter on numeric keypad */
    KeyRightControl = 228,
    KeyMute = 262, /* Mute */
    KeyCalculator = 226, /* Calculator */
    KeyPlayPause = 261, /* Play / Pause */
    KeyMediaStop = 260, /* Media Stop */
    KeyVolumeDown = 129, /* Volume - */
    KeyVolumeUp = 128, /* Volume + */
    KeyWebHome = 269, /* Web home */
    KeyNumPadComma = 133, /* , on numeric keypad (NEC PC98) */
    KeyNumPadDivide = 84, /* / on numeric keypad */
    Key_SYSRQ = 154,
    KeyRightAlt = 230, /* right Alt */
    KeyAltCar = KeyRightAlt, /* right Alt */
    KeyPause = 72, /* Pause */
    KeyHome = 74, /* Home on arrow keypad */
    KeyUp = 82, /* UpArrow on arrow keypad */
    KeyPageUp = 75, /* PgUp on arrow keypad */
    KeyLeft = 80, /* LeftArrow on arrow keypad */
    KeyRight = 79, /* RightArrow on arrow keypad */
    KeyEnd = 77, /* End on arrow keypad */
    KeyDown = 81, /* DownArrow on arrow keypad */
    KeyPageDown = 78, /* PgDn on arrow keypad */
    KeyInsert = 73, /* Insert on arrow keypad */
    KeyDelete = 76, /* Delete on arrow keypad */
    KeyLeftWindows = 227, /* Left Windows key */
    KeyRightWindows = 231, /* Right Windows key */
    KeyAppMenu = 118, /* AppMenu key */
    KeyPower = 102, /* System Power */
    KeySleep = 282, /* System Sleep */
    KeyWebSearch = 268, /* Web Search */
    KeyWebFavorites = 112, /* Web Favorites */
    KeyWebRefresh = 273, /* Web Refresh */
    KeyWebStop = 272, /* Web Stop */
    KeyWebForward = 271, /* Web Forward */
    KeyWebBack = 270, /* Web Back */
    KeyMyComputer = 267, /* My Computer */
    KeyMail = 265, /* Mail */
    KeyMediaSelect = 263, /* Media Select */

                          /*
                          *  Alternate names for keys originally not used on US keyboards.
                          */
    KeyCircomflex = KeyPreviousTrack, /* Japanese keyboard */

                                      /*
                                      * X-Arcade
                                      */
    XArcadeLeftPaddle = Key3, /* Key3 */
    XArcadeRightPaddle = Key4, /* Key4 */

    XArcade1Player = Key1, /* Key1 */
    XArcade2Player = Key2, /* Key2 */

    XArcadeLJoyLeft = KeyNumPad4, /* KeyNumPad4 */
    XArcadeLJoyRight = KeyNumPad6, /* KeyNumPad6 */
    XArcadeLJoyUp = KeyNumPad8, /* KeyNumPad8 */
    XArcadeLJoyDown = KeyNumPad2, /* KeyNumPad2 */

    XArcadeRJoyLeft = KeyD, /* KeyD */
    XArcadeRJoyRight = KeyG, /* KeyG */
    XArcadeRJoyUp = KeyR, /* KeyR */
    XArcadeRJoyDown = KeyF, /* KeyF */

    XArcadeLButton1 = KeyLeftControl, /* KeyLeftControl */
    XArcadeLButton2 = KeyLeftAlt, /* KeyLeftAlt */
    XArcadeLButton3 = KeySpaceBar, /* KeySpaceBar */
    XArcadeLButton4 = KeyLeftShift, /* KeyLeftShift */
    XArcadeLButton5 = KeyZ, /* KeyZ */
    XArcadeLButton6 = KeyX, /* KeyX */
    XArcadeLButton7 = KeyC, /* KeyC */
    XArcadeLButton8 = Key5, /* Key5 */

    XArcadeRButton1 = KeyA, /* KeyA */
    XArcadeRButton2 = KeyS, /* KeyS */
    XArcadeRButton3 = KeyQ, /* KeyQ */
    XArcadeRButton4 = KeyW, /* KeyW */
    XArcadeRButton5 = KeyE, /* KeyE */
    XArcadeRButton6 = KeyLeftBracket, /* KeyLeftBracket */
    XArcadeRButton7 = KeyRightBracket, /* KeyRightBracket */
    XArcadeRButton8 = Key6 /* Key6 */

#else
    KeyEscape = 0x01,
    Key1 = 0x02,
    Key2 = 0x03,
    Key3 = 0x04,
    Key4 = 0x05,
    Key5 = 0x06,
    Key6 = 0x07,
    Key7 = 0x08,
    Key8 = 0x09,
    Key9 = 0x0A,
    Key0 = 0x0B,
    KeyMinus = 0x0C, /* - on main keyboard */
    KeyEquals = 0x0D,
    KeyBackspace = 0x0E, /* backspace */
    KeyTab = 0x0F,
    KeyQ = 0x10,
    KeyW = 0x11,
    KeyE = 0x12,
    KeyR = 0x13,
    KeyT = 0x14,
    KeyY = 0x15,
    KeyU = 0x16,
    KeyI = 0x17,
    KeyO = 0x18,
    KeyP = 0x19,
    KeyLeftBracket = 0x1A,
    KeyRightBracket = 0x1B,
    KeyEnter = 0x1C, /* Enter on main keyboard */
    KeyLeftControl = 0x1D,
    KeyA = 0x1E,
    KeyS = 0x1F,
    KeyD = 0x20,
    KeyF = 0x21,
    KeyG = 0x22,
    KeyH = 0x23,
    KeyJ = 0x24,
    KeyK = 0x25,
    KeyL = 0x26,
    KeySemiColon = 0x27,
    KeyApostrophe = 0x28,
    KeyGrave = 0x29,    /* accent grave */
    KeyLeftShift = 0x2A,
    KeyBackslash = 0x2B,
    KeyZ = 0x2C,
    KeyX = 0x2D,
    KeyC = 0x2E,
    KeyV = 0x2F,
    KeyB = 0x30,
    KeyN = 0x31,
    KeyM = 0x32,
    KeyComma = 0x33,
    KeyPeriod = 0x34, /* . on main keyboard */
    KeySlash = 0x35, /* / on main keyboard */
    KeyRightShift = 0x36,
    KeyMultiply = 0x37, /* * on numeric keypad */
    KeyLeftAlt = 0x38, /* left Alt */
    KeySpaceBar = 0x39,
    KeyCapsLock = 0x3A,
    KeyF1 = 0x3B,
    KeyF2 = 0x3C,
    KeyF3 = 0x3D,
    KeyF4 = 0x3E,
    KeyF5 = 0x3F,
    KeyF6 = 0x40,
    KeyF7 = 0x41,
    KeyF8 = 0x42,
    KeyF9 = 0x43,
    KeyF10 = 0x44,
    KeyNumLock = 0x45,
    KeyScrollLock = 0x46, /* Scroll Lock */
    KeyNumPad7 = 0x47,
    KeyNumPad8 = 0x48,
    KeyNumPad9 = 0x49,
    KeyNumPadMinus = 0x4A, /* - on numeric keypad */
    KeyNumPad4 = 0x4B,
    KeyNumPad5 = 0x4C,
    KeyNumPad6 = 0x4D,
    KeyNumPadAdd = 0x4E, /* + on numeric keypad */
    KeyNumPad1 = 0x4F,
    KeyNumPad2 = 0x50,
    KeyNumPad3 = 0x51,
    KeyNumPad0 = 0x52,
    KeyNumPadPeriod = 0x53, /* . on numeric keypad */
    KeyOEM102 = 0x56, /* <> or \| on RT 102-key keyboard (Non-U.S.) */
    KeyF11 = 0x57,
    KeyF12 = 0x58,
#if defined(__rpi__)
    KeyF13 = 183, /* (NEC PC98) */
    KeyF14 = 184, /* (NEC PC98) */
    KeyF15 = 185, /* (NEC PC98) */
    KeyKana = 90, /* (Japanese keyboard) */
    KeyAbntC1 = 0x73, /* /? on Brazilian keyboard */
    KeyConvert = 0x79, /* (Japanese keyboard) */
    KeyNoConvert = 0x7B, /* (Japanese keyboard) */
    KeyYen = 124, /* (Japanese keyboard) */
    KeyAbntC2 = 0x7E, /* Numpad . on Brazilian keyboard */
    KeyNumPadEquals = 117, /* = on numeric keypad (NEC PC98) */
    KeyPreviousTrack = 165, /* Previous Track (OINPUT_CIRCUMFLEX on Japanese keyboard) */
    KeyAt = 0x91, /* (NEC PC98) */
    KeyColon = 0x92, /* (NEC PC98) */
    KeyUnderline = 0x93, /* (NEC PC98) */
    KeyKanji = 0x94, /* (Japanese keyboard) */
    KeyStop = 128, /* (NEC PC98) */
    KeyAx = 0x96, /* (Japan AX) */
    KeyUnlabeled = 0x97, /* (J3100) */
    KeyNextTrack = 163, /* Next Track */
    KeyNumPadEnter = 96, /* Enter on numeric keypad */
    KeyRightControl = 97,
    KeyMute = 113, /* Mute */
    KeyCalculator = 140, /* Calculator */
    KeyPlayPause = 164, /* Play / Pause */
    KeyMediaStop = 166, /* Media Stop */
    KeyVolumeDown = 114, /* Volume - */
    KeyVolumeUp = 115, /* Volume + */
    KeyWebHome = 172, /* Web home */
    KeyNumPadComma = 121, /* , on numeric keypad (NEC PC98) */
    KeyNumPadDivide = 98, /* / on numeric keypad */
    Key_SYSRQ = 99,
    KeyRightAlt = 100, /* right Alt */
    KeyAltCar = KeyRightAlt, /* right Alt */
    KeyPause = 119, /* Pause */
    KeyHome = 102, /* Home on arrow keypad */
    KeyUp = 103, /* UpArrow on arrow keypad */
    KeyPageUp = 104, /* PgUp on arrow keypad */
    KeyLeft = 105, /* LeftArrow on arrow keypad */
    KeyRight = 106, /* RightArrow on arrow keypad */
    KeyEnd = 107, /* End on arrow keypad */
    KeyDown = 108, /* DownArrow on arrow keypad */
    KeyPageDown = 109, /* PgDn on arrow keypad */
    KeyInsert = 110, /* Insert on arrow keypad */
    KeyDelete = 111, /* Delete on arrow keypad */
    KeyLeftWindows = 0xDB, /* Left Windows key */
    KeyRightWindows = 0xDC, /* Right Windows key */
    KeyAppMenu = 0xDD, /* AppMenu key */
    KeyPower = 116, /* System Power */
    KeySleep = 142, /* System Sleep */
    KeyWake = 143, /* System Wake */
    KeyWebSearch = 217, /* Web Search */
    KeyWebFavorites = 0x16c, /* Web Favorites */
    KeyWebRefresh = 173, /* Web Refresh */
    KeyWebStop = 0xE8, /* Web Stop */
    KeyWebForward = 0x115, /* Web Forward */
    KeyWebBack = 0x116, /* Web Back */
    KeyMyComputer = 0xEB, /* My Computer */
    KeyMail = 155, /* Mail */
    KeyMediaSelect = 0xED, /* Media Select */

                           /*
                           *  Alternate names for keys originally not used on US keyboards.
                           */
    KeyCircomflex = KeyPreviousTrack, /* Japanese keyboard */
#else // __rpi__
    KeyF13 = 0x64, /* (NEC PC98) */
    KeyF14 = 0x65, /* (NEC PC98) */
    KeyF15 = 0x66, /* (NEC PC98) */
    KeyKana = 0x70, /* (Japanese keyboard) */
    KeyAbntC1 = 0x73, /* /? on Brazilian keyboard */
    KeyConvert = 0x79, /* (Japanese keyboard) */
    KeyNoConvert = 0x7B, /* (Japanese keyboard) */
    KeyYen = 0x7D, /* (Japanese keyboard) */
    KeyAbntC2 = 0x7E, /* Numpad . on Brazilian keyboard */
    KeyNumPadEquals = 0x8D, /* = on numeric keypad (NEC PC98) */
    KeyPreviousTrack = 0x90, /* Previous Track (OINPUT_CIRCUMFLEX on Japanese keyboard) */
    KeyAt = 0x91, /* (NEC PC98) */
    KeyColon = 0x92, /* (NEC PC98) */
    KeyUnderline = 0x93, /* (NEC PC98) */
    KeyKanji = 0x94, /* (Japanese keyboard) */
    KeyStop = 0x95, /* (NEC PC98) */
    KeyAx = 0x96, /* (Japan AX) */
    KeyUnlabeled = 0x97, /* (J3100) */
    KeyNextTrack = 0x99, /* Next Track */
    KeyNumPadEnter = 0x9C, /* Enter on numeric keypad */
    KeyRightControl = 0x9D,
    KeyMute = 0xA0, /* Mute */
    KeyCalculator = 0xA1, /* Calculator */
    KeyPlayPause = 0xA2, /* Play / Pause */
    KeyMediaStop = 0xA4, /* Media Stop */
    KeyVolumeDown = 0xAE, /* Volume - */
    KeyVolumeUp = 0xB0, /* Volume + */
    KeyWebHome = 0xB2, /* Web home */
    KeyNumPadComma = 0xB3, /* , on numeric keypad (NEC PC98) */
    KeyNumPadDivide = 0xB5, /* / on numeric keypad */
    Key_SYSRQ = 0xB7,
    KeyRightAlt = 0xB8, /* right Alt */
    KeyAltCar = KeyRightAlt, /* right Alt */
    KeyPause = 0xC5, /* Pause */
    KeyHome = 0xC7, /* Home on arrow keypad */
    KeyUp = 0xC8, /* UpArrow on arrow keypad */
    KeyPageUp = 0xC9, /* PgUp on arrow keypad */
    KeyLeft = 0xCB, /* LeftArrow on arrow keypad */
    KeyRight = 0xCD, /* RightArrow on arrow keypad */
    KeyEnd = 0xCF, /* End on arrow keypad */
    KeyDown = 0xD0, /* DownArrow on arrow keypad */
    KeyPageDown = 0xD1, /* PgDn on arrow keypad */
    KeyInsert = 0xD2, /* Insert on arrow keypad */
    KeyDelete = 0xD3, /* Delete on arrow keypad */
    KeyLeftWindows = 0xDB, /* Left Windows key */
    KeyRightWindows = 0xDC, /* Right Windows key */
    KeyAppMenu = 0xDD, /* AppMenu key */
    KeyPower = 0xDE, /* System Power */
    KeySleep = 0xDF, /* System Sleep */
    KeyWake = 0xE3, /* System Wake */
    KeyWebSearch = 0xE5, /* Web Search */
    KeyWebFavorites = 0xE6, /* Web Favorites */
    KeyWebRefresh = 0xE7, /* Web Refresh */
    KeyWebStop = 0xE8, /* Web Stop */
    KeyWebForward = 0xE9, /* Web Forward */
    KeyWebBack = 0xEA, /* Web Back */
    KeyMyComputer = 0xEB, /* My Computer */
    KeyMail = 0xEC, /* Mail */
    KeyMediaSelect = 0xED, /* Media Select */

                           /*
                           *  Alternate names for keys originally not used on US keyboards.
                           */
    KeyCircomflex = KeyPreviousTrack, /* Japanese keyboard */
#endif // else __rpi__

                                      /*
                                      * X-Arcade
                                      */
    XArcadeLeftPaddle = 4, /* Key3 */
    XArcadeRightPaddle = 5, /* Key4 */

    XArcade1Player = 2, /* Key1 */
    XArcade2Player = 3, /* Key2 */

    XArcadeLJoyLeft = 75, /* KeyNumPad4 */
    XArcadeLJoyRight = 77, /* KeyNumPad6 */
    XArcadeLJoyUp = 72, /* KeyNumPad8 */
    XArcadeLJoyDown = 80, /* KeyNumPad2 */

    XArcadeRJoyLeft = 32, /* KeyD */
    XArcadeRJoyRight = 34, /* KeyG */
    XArcadeRJoyUp = 19, /* KeyR */
    XArcadeRJoyDown = 33, /* KeyF */

    XArcadeLButton1 = 29, /* KeyLeftControl */
    XArcadeLButton2 = 56, /* KeyLeftAlt */
    XArcadeLButton3 = 57, /* KeySpaceBar */
    XArcadeLButton4 = 42, /* KeyLeftShift */
    XArcadeLButton5 = 44, /* KeyZ */
    XArcadeLButton6 = 45, /* KeyX */
    XArcadeLButton7 = 46, /* KeyC */
    XArcadeLButton8 = 6, /* Key5 */

    XArcadeRButton1 = 30, /* KeyA */
    XArcadeRButton2 = 31, /* KeyS */
    XArcadeRButton3 = 16, /* KeyQ */
    XArcadeRButton4 = 17, /* KeyW */
    XArcadeRButton5 = 18, /* KeyE */
    XArcadeRButton6 = 26, /* KeyLeftBracket */
    XArcadeRButton7 = 27, /* KeyRightBracket */
    XArcadeRButton8 = 7, /* Key6 */
#endif // __APPLE__
};

// Les boutons de la mouse
/// \name Constantes d�signants les diff�rents boutons d'une souris
//@{
const int DKI_MOUSE_BUTTON1 = 256;
const int DKI_MOUSE_BUTTON2 = 257;
const int DKI_MOUSE_BUTTON3 = 258;
const int DKI_MOUSE_BUTTON4 = 259;
const int DKI_MOUSE_BUTTON5 = 260;
const int DKI_MOUSE_BUTTON6 = 261;
const int DKI_MOUSE_BUTTON7 = 262;
const int DKI_MOUSE_BUTTON8 = 263;
//@}


// Les boutons du joystick
/// \name Constante d�signant les diff�rents boutons d'un joystick
/// Il y a une limite maximum de 128 buttons d�tectables. Pour atteindre le n i�me bouton, on n'a qu'� faire : DKI_JOY_BUTTON1 + n
//@{
const int DKI_JOY_BUTTON1 = 264; // 128 Buttons. DKI_JOY_BUTTON1 + n
//@}


// Les fonction du DKI

/// \brief retourne la constante d�signant la premi�re touche ayant �t� appuy�e
///
/// Cette fonction permet de connaitre la touche ou le bouton qui a �t� appuy� en premier. La fonction effectue la v�rification pour un instant seulement (lors de l'appel). Un appel pour chaque cycle d'ex�cution est donc n�cessaire afin de savoir si une touche a �t� pes� dans un certain intervalle de temps.
///
/// \return l'index repr�sentant la touche ou le bouton qui a �t� appuy� en premier
int				dkiGetFirstDown();


/// \brief retourne la vitesse � laquelle la roulette de la souris est d�plac�e
///
/// Cette fonction retourne la vitesse � laquelle la roulette de la souris est d�plac�e en nombre de clic
/// -2 signifirait que la wheel a �t� d�cendu 2 fois. (normalement on s'en tien � -1,0 ou 1)
/// Exemple d'utilisation : if (dkiGetMouseWheelVel() < 0) zoomOut();
///
/// \return retourne la vitesse � laquelle la roulette de la souris est d�plac�e
int				dkiGetMouseWheelVel();



/// \brief retourne la position actuelle de la souris
///
/// Cette fonction retourne la position actuelle de la souris. Cette position est en pixel et l'origine est le coin sup�rieur gauche de l'�cran.
///
/// \return retourne la position actuelle de la souris en pixel
CVector2i		dkiGetMouse();



/// \brief retourne la vitesse � laquelle se d�place la souris
///
/// Cette fonction retourne le d�placement effectu� par la souris en pixels depuis le dernier appel a dkiUpdate().
/// Le d�placement retourn� n'est pas d�pendant de la grandeur de la fen�tre (il y aura un certain d�placement retourn� m�me si on d�place la souris vers la gauche et que la derni�re position �tait (0,0).
/// Parfait pour les jeux de style FPS
///
/// \return retourne la vitesse � laquelle se d�place la souris
CVector2i		dkiGetMouseVel();



/// \brief retourne l'�tat d'une touche ou d'un bouton
///
/// Cette fonction retourne l'�tat d'une touche ou d'un bouton.
///
/// \param inputID identifiant unique de la touche ou du bouton
/// \return �tat de la touche ou du bouton
int				dkiGetState(int inputID);



/// \brief retourne la position de chaque axe d'un joystick ou d'une manette de jeu
///
/// Cette fonction retourne la position de chaque axe d'un joystick ou d'une manette de jeu. La position au repos �tant 0 et les extr�mes �tant -1 et 1.
/// Aucune "dead zone" et courbe de progression est d�fini ici. C'est au client de le faire.
///
/// \return position de chaque axe
CVector3f		dkiGetJoy();
CVector3f		dkiGetJoyR();



/// \brief retourne la vitesse � laquelle se d�place chacun des axes d'un joystick ou d'une manette de jeu
///
/// Cette fonction retourne la vitesse � laquelle se d�place chacun des axes d'un joystick ou d'une manette de jeu par rapport au dernier appel � dkiUpdate().
/// Si la derni�re position d'un axe �tait de -1 et que sa position est � 1 lors de l'appel, la valeur 2 sera retourn� pour cet axe.
///
/// \return vitesse de chaque axe
CVector3f		dkiGetJoyVel();



/// \brief initialise le module pour usage
///
/// Cette fonction initialise le module afin que son utilisation puisse d�buter. Cette fonction DOIT �tre appel� avant tout autres appels � des fonctions de ce module.
///
/// \param appInstance pointeur vers l'instance de l'application
/// \param appHandle pointeur vers le lien logique de l'application
/// \return 0 s'il y a un �chec, 1 si tout s'est bien pass�
int				dkiInit(SDL_Window* appHandle);



/// \brief termine l'utilisation du module
///
/// Cette fonction lib�re le syst�me des ressources allou�es qui �taient n�cessaires au fonctionnement du module et termine sont utilisation. Cette fonction de doit pas �tre appel� plusieurs fois de suite : pour chaque appel � cette fonction, un appel � dkiInit() correspondant doit pr�c�demment avoir �t� fait.
///
void			dkiShutDown();



/// \brief mise � jour des �tats des p�riph�riques d'entr�es
///
/// Cette fonction effectue la mise � jour des �tats des p�riph�riques d'entr�es (clavier, souris, joystick). Elle doit �tre appel�e une fois par cycle d'ex�cution.
/// On doit sp�cifier la dimension de la fen�tre dans laquelle le pointeur de la souris ne pourra pas exc�der.
///
/// \param elapsef non utilis�
/// \param width dimension en pixel de la fen�tre
/// \param height dimension en pixel de la fen�tre
void			dkiUpdate(float elapsef, int width, int height);


// Setter la position du cursor
void			dkiSetMouse(CVector2i & mousePos);

#endif
