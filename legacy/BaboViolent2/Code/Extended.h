#ifndef EXTENDED_H
#define EXTENDED_H


#include "ControlListener.h"
#include "Menu.h"

#include "Button.h"
#include "Write.h"
#include "Choice.h"
#include "Label.h"
#include "Key.h"


class Extended : public ControlListener
{
public:
	// The main buttons
	Button * btn_apply;
	Button * btn_cancel;

	//--- Les controls
	Button * btn_nextSkin;
	Button * btn_previousSkin;

	//--- Les couleurs
	Button * btn_redDecal[6];
	Button * btn_greenDecal[6];
	Button * btn_blueDecal[6];

	// Pour dessiner notre sphere
	GLUquadricObj* qObj;

	//--- Animer la sphere
	float animSphere;

	//--- Notre skin
	unsigned int tex_skin;
	unsigned int tex_skinOriginal;
	int currentSkin;
	std::vector<CString> skins;

	//--- Les couleurs custom du babo
	CColor3f redDecal;
	CColor3f greenDecal;
	CColor3f blueDecal;

	// La font
	unsigned int font;

	// Son menu maitre
	Menu * rootMenu;

public:
	// Constructor
	Extended(Menu * pRootMenu);

	// Destructor
	virtual ~Extended();

	// Si on clic sur un bouton
	void onClick(Control * control);
	void updateUnique(float delay);
	void renderUnique();

	//--- Pour updater les couleurs sur la texture
	void updateSkinColor();

	// Pour mettre les valeur config
	void resetToConfigValue();
	void setConfigValue(bool resetAfter);
};


#endif

