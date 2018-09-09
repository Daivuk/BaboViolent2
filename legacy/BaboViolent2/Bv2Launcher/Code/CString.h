/* RndLabs inc. (c) All rights reserved */



#ifndef CSTRING_H
#define CSTRING_H



/// \name Constantes
/// Nombre maximum de caract�es qu'un string peut contenir
//E.P Uhhhmmm... pas sr que j'aime �...
#define MAX_CARAC 512


#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#ifndef WIN32
	#include "LinuxHeader.h"
#endif


////////////////////////////////////////////////////////////////////////////////////////
/// \brief Classe utile pour g�er des cha�es de caract�es
///
/// Elle permet de cr�r des objets contenant des fonctions de recherche de caract�es,
///	d'insertion, de remplacement, bref toutes les op�ations qui peuvent �re utile lors
/// de la manipulation de cha�es de caract�es.
/// Classe d�iv� de CAttribute.
////////////////////////////////////////////////////////////////////////////////////////
class CString
{
public:
	/// Pointeur qui contient notre cha�e de caract�e
	char* s;

	// Constructeurs

	///Constructeur par d�aut. La string est initialis� avec un seul caract�e, le caract�e nul.
	CString(){s = new char[1]; s[0] = '\0';}
	CString(char* fmt, ...);

	/// Constructeur copie
	CString(const CString & objToCopy){s = new char[strlen(objToCopy.s)+1]; strcpy(s, objToCopy.s);}

	/// Destructeur
	virtual ~CString(){delete [] s;}

	// V�ifie l'extension d'une cha�e de caract�e
	bool checkExtension(char * extension);

	// Remplis la cha�e de caract�e par le chemin de l'application
	void fillWithAppPath();

	// On check si on a le string voulu inclu dedans
	bool find(CString string);
	bool find(char* string);
	bool find(CString string, char* strFound);
	bool find(char* string, char* strFound);
	bool find(CString string, int & index);
	bool find(char* string, int & index);
	bool find(CString string, char* strFound, int & index);
	bool find(char* string, char* strFound, int & index);
	
	///Obtenir le nom du fichier d'un chemin complet
	CString getFilename();

	// Prendre les tokens
	CString getFirstToken(int caracterSeparator);
	CString getNextToken(int caracterSeparator);

	///Obtenir le path du fichier (sans le fichier lui-m�e)
	CString getPath();

	///Obtenir le pointeur this (HEU??? sert �quoi???)
	CString * getThis(){return this;}

	///Obtenir le type de l'objet
	CString getType(){return "CString";}

	// Ins�er un string �une certaine position
	void insert(CString string, int index);
	void insert(char *, int index);

	///Retourne vrai si la string est vide
	bool isNull(){return (s[0] == '\0') ? true : false;}

	//	int len(){int len_=0; while(s[len_++]); return len_-1;}  // On peut tr� bien le faire nous m�e
	/// Grandeur de la string
	int len(){return (int)strlen(s);} // Probablement que strlen() est plus optimis�en _asm

	// Pour retirer un caract�e
	void remove(int index);

	/// Pour remplacer un caract�e par un autre dans tout le string
	void replace(char toReplace, char by){int len_=len(); for(int i=0;i<len_;i++) if(s[i] == toReplace) s[i]=by;}

	///Vide la cha�e
	void reset(){delete [] s; s = new char[1]; s[0] = '\0';}

	// Changer ses dimensions
	void resize(int newSize);
	void resizeInverse(int newSize);

	// Lui setter une nouvelle valeur
	void set(char* fmt, ...);

	// Pour le loader d'un fichier
	void loadFromFile(FILE *fic);

	/// Convertir en float
	float toFloat(){/*float tmp;sscanf(s, "%f", &tmp);*/return (float)atof(s)/*tmp*/;}
	///Convertir en int
	int toInt(){/*int tmp;sscanf(s, "%i", &tmp);*/return atoi(s)/* tmp*/;}

	/// Mettre la cha�e en minuscule
	void toLower(){int len_=len(); for(int i=0;i<len_;i++) if(s[i]>='A' && s[i]<='Z') s[i]+=32;}

	///Mettre la cha�e en majuscule
	void toUpper(){int len_=len(); for(int i=0;i<len_;i++) if(s[i]>='a' && s[i]<='z') s[i]-=32;}

	// Pour trimer
	void trim(char caracter);



	/// Acc�er �un caract�e du string
	char& operator[](const int i){return (i>=0 && i<len()) ? s[i] : ((i<0) ? s[0] : s[len()-1]);}

	///Copier �partir d'un pointeur
	void operator=(const char* string){delete [] s; s = new char[strlen(string)+1]; strcpy(s, string);}
	///Copier �partir de l'adresse de l'objet
	void operator=(const CString &objToCopy){delete [] s; s = new char[strlen(objToCopy.s)+1]; strcpy(s, objToCopy.s);}
	///Copier �partir d'un int
	void operator=(int value){set("%i", value);}
	///Copier �partir d'un flaot
	void operator=(float value){set("%f", value);}

	///Concat�ation �partir de l'adresse de la string (retourne une string)
	CString operator+(const CString& string){return CString("%s%s", s, string.s);}
	///Concat�ation �partir de l'adresse de la string (en affectant la valeur �la string courante)
	void operator+=(const CString &string){set("%s%s", s, string.s);}
	///Concat�ation �partir d'un pointeur sur une string (retourne une string)
	CString operator+(char* string){return CString("%s%s", s, string);}
	///Concat�ation �partir d'un pointeur sur une string (en affectant la valeur �la string courante)
	void operator+=(char* string){set("%s%s", s, string);}
	///Concat�ation �partir d'un int (retourne une string)
	CString operator+(int value){return CString("%s%i", s, value);}
	///Concat�ation �partir d'un int sur une string (en affectant la valeur �la string courante)
	void operator+=(int value){set("%s%i", s, value);}
	///Concat�ation �partir d'un float (retourne une string)
	CString operator+(float value){return CString("%s%f", s, value);}
	///Concat�ation �partir d'un float sur une string (en affectant la valeur �la string courante)
	void operator+=(float value){set("%s%f", s, value);}

	///Retourne vrai si les deux string sont identiques
	bool operator==(const CString &string){return (stricmp(s, string.s)==0);}
	///Retourne vrai si les deux string sont diff�entes
	bool operator!=(const CString &string){return !(stricmp(s, string.s)==0);}
	///Retourne vrai si les deux string sont identiques
	bool operator==(const char* string){return (stricmp(s, string)==0);}
	///Retourne vrai si les deux string sont diff�entes
	bool operator!=(const char* string){return !(stricmp(s, string)==0);}
	///Retourne vrai si la string est plus petite
	bool operator<(const CString& string)const{return (stricmp(s, string.s)<0);}
	///Retourne vrai si la string est plus grande
	bool operator>(const CString& string)const{return (stricmp(s, string.s)>0);}
	///Retourne vrai si la string est plus petite ou �ale
	bool operator<=(const CString& string){return (stricmp(s, string.s)<=0);}
	///Retourne vrai si la string est plus grande ou �ale
	bool operator>=(const CString& string){return (stricmp(s, string.s)>=0);}
};

/*
CString operator+(const char * string, const CString& string2);
CString operator+(const char * string, int & value);
CString operator+(const char * string, float & value);
CString operator+(const char * string1, const char* string2);
*/

#endif
