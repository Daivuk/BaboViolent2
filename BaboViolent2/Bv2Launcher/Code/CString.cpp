/* TCE (c) All rights reserved */



#include "CString.h"
//#include <windows.h>



//
// Constructeurs
//

////////////////////////////////////////////////////////////////////////////////////////
/// \brief Constructeur utilis�comme pour un printf
///
/// Ce constructeur fonctionne de la m�e mani�e qu'un printf avec un nombre inconnu
/// d'arguments. Les arguments sont pass� tour �tour et int�r� �la string...
///
////////////////////////////////////////////////////////////////////////////////////////
CString::CString(char* fmt, ...){

	if (!fmt)
	{
		s = new char [1];
		s[0] = '\0';
		return;
	}

	// On cr�un char de 512 pour contenir le string (pas besoin de plus que 512 je crois)
	char *mString = new char[MAX_CARAC];

	// Ici on passe tout les param (c comme un printf) pour les mettre dans le string
	va_list		ap;
	va_start(ap, fmt);
		vsprintf(mString, fmt, ap);
	va_end(ap);

	// On lui transfert la valeur finale
	s = new char [strlen(mString)+1];
	strcpy(s, mString);

	delete [] mString;
}



////////////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction qui permet de modifier la taille de la cha�e de caract�e
///
/// \param newSize : Nouvelle taille.
///
/// \see 
/// http://www.cplusplus.com/ref/cstdio/sprintf.html
////////////////////////////////////////////////////////////////////////////////////////
void CString::resize(int newSize)
{
	if (newSize > 0)
	{
		char tmp[10];
		sprintf(tmp, "%%.%is", newSize);
		CString newStr(tmp, s);
		set(newStr.s);
	}
	else
	{
		// On le remet �vide
		delete [] s;
		s = new char [1];
		s[0] = '\0';
	}
}

////////////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction qui permet de modifier la taille de la cha�e de caract�e et qui en
/// plus l'inverse
///
/// \param newSize : Nouvelle taille.
///
/// \return Aucun
///
////////////////////////////////////////////////////////////////////////////////////////
void CString::resizeInverse(int newSize){

	if (newSize > 0)
	{
		// On le resize
		int len_ = len();
		if (len_-newSize > 0)
		{
			CString newStr(&(s[len_-newSize]));
			set(newStr.s);
		}
	}
	else
	{
		// On le remet �vide
		delete [] s;
		s = new char [1];
		s[0] = '\0';
	}
}



////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction qui permet d'assigner une valeur �une cha�e existante de la m�e fa�n
///	qu'un printf
///
/// La fonction supprime d'abbord la cha�e existante et en cr� une nouvelle avec la valeur
/// d�ir�
////////////////////////////////////////////////////////////////////////////////////////////
void CString::set(char* fmt, ...){

	if (!fmt)
	{
		delete [] s;
		s = new char [1];
		s[0] = '\0';
		return;
	}

	// On cr�un char de 256 pour contenir le string (pas besoin de plus que 256 je crois)
	char *mString = new char[MAX_CARAC];

	// Ici on passe tout les param (c comme un printf) pour les mettre dans le string
	va_list		ap;
	va_start(ap, fmt);
		vsprintf(mString, fmt, ap);
	va_end(ap);

	// On lui transfert la valeur finale
	delete [] s;
	s = new char [strlen(mString)+1];
	strcpy(s, mString);

	delete [] mString;
}



////////////////////////////////////////////////////////////////////////////////////////////
/// \brief V�ifie l'extension d'une cha�e de caract�e
///
///	Fonction qui permet de d�erminer si la cha�e de caract�e poss�e l'extension
/// envoy� en param�re
///
/// \param extension : Extension qu'on veut v�ifier.
///
/// \return True : si l'extension est pr�ente\n
///			False : si l'extension n'est pas pr�ente
////////////////////////////////////////////////////////////////////////////////////////////
bool CString::checkExtension(char * extension)
{
	// On v�ifie que le string est bon
	if (!extension) 
	{
		return false;
	}

	// On check le nb de caract�e de notre extension
	int extensionLen = (int)strlen(extension);

	if (extensionLen > 0 && len() >= extensionLen)
	{
		//on compare sans tenir compte de majuscule/minuscule
		return (stricmp(&(s[len()-extensionLen]), extension) == 0);
	}
	else
	{
		return false;
	}
}



/////////////////////////////////////////////////////////////////////////
///Obtenir le path du fichier (sans le fichier lui-m�e)
/////////////////////////////////////////////////////////////////////////
CString CString::getPath()
{
	// On va chercher le dernier '\' ou '/' trouv�
	for (int i=len()-1;i>=0;i--)
	{
		if (s[i] == '\\' || s[i] == '/')
		{
			char tmp[10];
			sprintf(tmp, "%%.%is", i+1);
			return CString(tmp, s);
		}
	}

	return CString();
}



//////////////////////////////////////////////////////////////
///Obtenir le nom du fichier d'un chemin complet
//////////////////////////////////////////////////////////////
CString CString::getFilename()
{

	// On va chercher le dernier '\' ou '/' trouv�
	for (int i=len()-1;i>=0;i--)
	{
		if (s[i] == '\\' || s[i] == '/')
		{
			char tmp[10];
			sprintf(tmp, "%%.%is", len()-(i+1));
			return CString(tmp, &(s[i+1]));
		}
	}

	return CString();
}



////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Enlever des caract�es au d�ut et �la fin de la cha�e
///
///	Cette fonction supprime le caract�e recu en param�re au d�ut et �la fin de la string
///
/// \param caracter : Caract�e �trimmer
////////////////////////////////////////////////////////////////////////////////////////////
void CString::trim(char caracter)
{

	// En enl�e les caract�es au d�ut
	int len_ = len();
	int i=0;
	for (i=0;i<len_;i++)
	{
		if (s[i] != caracter)
		{
			break;
		}
	}
	CString newString(&(s[i]));

	// les dernier caract�e maintenant
	for (i=newString.len()-1;i>=0;i--)
	{
		if (newString.s[i] != caracter)
		{
			break;
		}
	}

	char tmp[10];
	sprintf(tmp, "%%.%is", i+1);
	CString newString2(tmp, newString.s);

	set(newString2.s);
}



////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Permet de prendre le token suivant
///
///	Cette fonction retourne les caract�es se trouvant �la droite du caract�e de s�aration
/// re� en param�re
///
/// \param caracter : Caract�e servant de s�aration
////////////////////////////////////////////////////////////////////////////////////////////
CString CString::getNextToken(int caracter)
{

	// On commence pa r trimmer le tout au cas
	trim(caracter);

	CString result;

	// Maintenant on pogne le dernier mot
	// On va chercher le dernier ' ' trouv�
	for (int i = len()-1; i >= -1; i--)
	{
		if (s[i] == caracter || i == -1)
		{
			result = &(s[i+1]);
			resize(i+1);
			break;
		}
	}

	// On trim
	trim(caracter);

	return result;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Permet de prendre le token �partir du d�ut
///
///	Cette fonction retourne les caract�es se trouvant �la gauche du caract�e de s�aration
/// re� en param�re. Cela peut �re pratique si nous voulons faire un switch sur la command
/// avant tout et passer le reste du string.
///
/// \param caracter : Caract�e servant de s�aration
////////////////////////////////////////////////////////////////////////////////////////////
CString CString::getFirstToken(int caracterSeparator)
{

	// On commence par trimmer le tout au cas
	trim(caracterSeparator);

	CString result;

	// Maintenant on pogne le dernier mot
	// On va chercher le dernier ' ' trouv�
	int len_ = len();
	for (int i=0;i<=len_;i++){
		if (s[i] == caracterSeparator || s[i] == '\0'){
			char tmp[10];
			sprintf(tmp, "%%.%is", i);
			result = CString(tmp, s);
			resizeInverse(len_-i);
			break;
		}
	}

	// On trim
	trim(caracterSeparator);

	return result;
}



////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Recherche de caract�es
///
///	Cette fonction utilise une cha�e de caract�e re�e en param�re et essaie de 
/// la trouver dans la cha�e (s). (Seule la premi�e occurence de la cha�e est trouv�)
///
/// \param string : Cha�e de caract�e �trouver.
///
/// \return True : si la cha�e est trouv�
///			False : si la cha�e n'est pas trouv�
///
///	\note Algorithmes plus performants existent si n�essaire.
////////////////////////////////////////////////////////////////////////////////////////////
bool CString::find(CString string)
{

	//longueur de notre cha�e (s)
	int len_1 = len();
	//longueur de la cha�e qu'on veut trouver
	int len_2 = string.len();

	//on parcours la cha�e pour essayer de trouver la sub-string
	for (int i=0; i < (len_1 - len_2 + 1); i++)
	{
		if (strnicmp(&(s[i]), string.s, len_2) == 0) 
		{
			return true;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Recherche de caract�es avec pointeur sur la string trouv�
///
///	Cette fonction utilise une cha�e de caract�e re�e en param�re et essaie de 
/// la trouver dans la cha�e (s). La fonction recoit aussi un pointeur en param�re qui
/// va pointer au d�ut de la cha�e trouv�. (Seule la premi�e occurence de la cha�e est trouv�)
///
/// \param	string : Cha�e de caract�e �trouver.
///	\param	strFound : Pointeur vers le b�ut de la cha�e si elle est trouv�
///
/// \return True : si la cha�e est trouv�
///			False : si la cha�e n'est pas trouv�
///
///	\note Algorithmes plus performants existent si n�essaire.
////////////////////////////////////////////////////////////////////////////////////////////
bool CString::find(CString string, char* strFound)
{

	int len_1 = len();
	int len_2 = string.len();

	for (int i=0; i < (len_1 - len_2 + 1) ;i++)
	{
		if (strnicmp(&(s[i]), string.s, len_2) == 0)
		{
			strFound = &(s[i]);
			return true;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Recherche de caract�es avec position de la string trouv�
///
///	Cette fonction utilise une cha�e de caract�e re�e en param�re et essaie de 
/// la trouver dans la cha�e (s). La position de la cha�e est aussi retourn�
/// (Seule la premi�e occurence de la cha�e est trouv�.)
///
/// \param	string : Cha�e de caract�e �trouver.
///	\param	index : Position de la cha�e si elle est trouv�.
///
/// \return True : si la cha�e est trouv�
///			False : si la cha�e n'est pas trouv�
///
///	\note Algorithmes plus performants existent si n�essaire.
////////////////////////////////////////////////////////////////////////////////////////////
bool CString::find(CString string, int & index)
{

	int len_1 = len();
	int len_2 = string.len();

	for (int i=0; i < (len_1 - len_2 + 1); i++)
	{
		if (strnicmp(&(s[i]), string.s, len_2) == 0)
		{
			index = i;
			return true;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Recherche de caract�es avec pointeur et position de la string trouv�
///
///	Cette fonction utilise une cha�e de caract�e re�e en param�re et essaie de 
/// la trouver dans la cha�e (s). La position de la cha�e est aussi retourn�
/// (Seule la premi�e occurence de la cha�e est trouv�.)
///
/// \param	string : Cha�e de caract�e �trouver.
///	\param	strFound : Pointeur vers le b�ut de la cha�e si elle est trouv�
///	\param	index : Position de la cha�e si elle est trouv�.
///
/// \return True : si la cha�e est trouv�
///			False : si la cha�e n'est pas trouv�
///
///	\note Algorithmes plus performants existent si n�essaire.
////////////////////////////////////////////////////////////////////////////////////////////
bool CString::find(CString string, char* strFound, int & index)
{

	int len_1 = len();
	int len_2 = string.len();

	for (int i=0;i < (len_1 - len_2 + 1); i++)
	{
		if (strnicmp(&(s[i]), string.s, len_2) == 0)
		{
			index = i;
			strFound = &(s[i]);
			return true;
		}
	}

	return false;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Recherche de caract�es
///
///	Cette fonction utilise une cha�e de caract�e re�e en param�re et essaie de 
/// la trouver dans la cha�e (s). (Seule la premi�e occurence de la cha�e est trouv�)
///
/// \param string_ : Cha�e de caract�e �trouver.
///
/// \return True : si la cha�e est trouv�
///			False : si la cha�e n'est pas trouv�
///
///	\note Algorithmes plus performants existent si n�essaire.
////////////////////////////////////////////////////////////////////////////////////////////
bool CString::find(char* string_)
{
	return find(CString(string_)); // On recall l'autre
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Recherche de caract�es avec pointeur sur la string trouv�
///
///	Cette fonction utilise une cha�e de caract�e re�e en param�re et essaie de 
/// la trouver dans la cha�e (s). La fonction recoit aussi un pointeur en param�re qui
/// va pointer au d�ut de la cha�e trouv�. (Seule la premi�e occurence de la cha�e est trouv�)
///
/// \param	string_ : Cha�e de caract�e �trouver.
///	\param	strFound : Pointeur vers le b�ut de la cha�e si elle est trouv�
///
/// \return True : si la cha�e est trouv�
///			False : si la cha�e n'est pas trouv�
///
///	\note Algorithmes plus performants existent si n�essaire.
////////////////////////////////////////////////////////////////////////////////////////////
bool CString::find(char* string_, char* strFound)
{
	return find(CString(string_), strFound); // On recall l'autre
}


////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Recherche de caract�es avec position de la string trouv�
///
///	Cette fonction utilise une cha�e de caract�e re�e en param�re et essaie de 
/// la trouver dans la cha�e (s). La position de la cha�e est aussi retourn�
/// (Seule la premi�e occurence de la cha�e est trouv�.)
///
/// \param	string_ : Cha�e de caract�e �trouver.
///	\param	index : Position de la cha�e si elle est trouv�.
///
/// \return True : si la cha�e est trouv�
///			False : si la cha�e n'est pas trouv�
///
///	\note Algorithmes plus performants existent si n�essaire.
////////////////////////////////////////////////////////////////////////////////////////////
bool CString::find(char* string_, int & index)
{
	return find(CString(string_), index); // On recall l'autre
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Recherche de caract�es avec pointeur et position de la string trouv�
///
///	Cette fonction utilise une cha�e de caract�e re�e en param�re et essaie de 
/// la trouver dans la cha�e (s). La position de la cha�e est aussi retourn�
/// (Seule la premi�e occurence de la cha�e est trouv�.)
///
/// \param	string_ : Cha�e de caract�e �trouver.
///	\param	strFound : Pointeur vers le b�ut de la cha�e si elle est trouv�
///	\param	index : Position de la cha�e si elle est trouv�.
///
/// \return True : si la cha�e est trouv�
///			False : si la cha�e n'est pas trouv�
///
///	\note Algorithmes plus performants existent si n�essaire.
////////////////////////////////////////////////////////////////////////////////////////////
bool CString::find(char* string_, char* strFound, int & index)
{
	return find(CString(string_), strFound, index); // On recall l'autre
}



////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Insertion de texte
///
///	Cette fonction insert le texte re� en param�re �la position d�ermin� par le 2e param�re
///
/// \param	string : Cha�e de caract�e �ins�er.
///	\param	index : Position de l'insertion dans la cha�e de base.
////////////////////////////////////////////////////////////////////////////////////////////
void CString::insert(CString string, int index){

	char tmp[10];
	sprintf(tmp, "%%.%is%%s%%s", index);
	CString newString(tmp, s, string.s, &(s[index]));
	set(newString.s);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Insertion de texte (avec pointeur)
///
///	Cette fonction insert le texte re� en param�re �la position d�ermin� par le 2e param�re
///
/// \param	string : Cha�e de caract�e �ins�er.
///	\param	index : Position de l'insertion dans la cha�e de base.
////////////////////////////////////////////////////////////////////////////////////////////
void CString::insert(char *string, int index){
	insert(CString(string), index);
}



////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Suppression de texte
///
///	Cette fonction supprime le texte de la cha�e de base situ�apr� la position re� en param�re
///
/// \param	index : Position de la suppression dans la cha�e de base.
////////////////////////////////////////////////////////////////////////////////////////////
void CString::remove(int index){

	char tmp[10];
	sprintf(tmp, "%%.%is%%s", index);
	CString newString(tmp, s, &(s[index+1]));
	set(newString.s);
}



////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Remplis la cha�e de caract�e par le chemin de l'application
///
///	La fonction trouve le chemin (path) de l'application et l'affecte �la cha�e
////////////////////////////////////////////////////////////////////////////////////////////
void CString::fillWithAppPath()
{

	char *appPath = new char[MAX_CARAC]; //E.P Utile?
	appPath[0] = '\0';					 //E.P Utile?
	set(appPath);						 //E.P Utile?

	CString result = getPath();
	set(result.s);

	delete [] appPath;					//E.P Utile?
}


////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Utilisation d'un fichier pour setter le texte
///
///	Cette fonction load le texte �partir du fichier et l'utilise pour setter la cha�e
///
/// \param	fic : fichier
////////////////////////////////////////////////////////////////////////////////////////////
void CString::loadFromFile(FILE *fic)
{
	char tmp[MAX_CARAC] = {0};

	for (int i=0;i<MAX_CARAC;i++)
	{
		fread(&(tmp[i]), 1, sizeof(char), fic);
		if (tmp[i] == 0) break;
	}

	set(tmp);
}



//
// Op�ateur sur des char*
//
/*
CString operator+(const char * string, const CString& string2)
{
	return CString("%s%s", string, string2.s);
}

CString operator+(const char * string, int & value)
{
	return CString("%s%i", string, value);
}

CString operator+(const char * string, float & value)
{
	return CString("%s%f", string, value);
}

CString operator+(const char * string1, char* string2)
{
	return CString("%s%s", string1, string2);
}*/
