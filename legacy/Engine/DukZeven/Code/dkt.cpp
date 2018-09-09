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

/* TCE (c) All rights reserved */


#include "dkti.h"
#include <math.h>
#include <sys/stat.h>
#if defined(_DEBUG) && defined(USE_VLD)
#include <vld.h>
#endif




//
// Les trucs static
//
char *CDkt::lastErrorString = 0;
std::vector<CTexture*> CDkt::textures;
int CDkt::checkingUpdate = 0;



//
// Pour blurer la texture
//
void			 dktBlurTexture(unsigned int textureID, int nbPass)
{
#ifndef _DX_
	// On passe toute nos textures en loop
	for (int k=0;k<(int)CDkt::textures.size();k++)
	{
		CTexture *texture = CDkt::textures.at(k);

		if (texture->oglID == textureID)
		{
			unsigned char * imageData = new unsigned char [texture->size[0] * texture->size[1] * texture->bpp];
			glBindTexture(GL_TEXTURE_2D, textureID);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);

			int fw = texture->size[0]*texture->bpp;
			int fh = texture->size[1];
			//int px = 0;
			//int py = 0;

			int filter = 4;

			// On va blurer la texture
			for (int p=0;p<nbPass;p++)
			{
				for (int j=1;j<fh+1;j++)
				{
					for (int i=1;i<fw+1;i++)
					{
						imageData[((j)%fh)*fw + ((i)%fw)] = 
							char((
							imageData[((j  )%fh)*fw + ((i  )%fw)]*filter + 
							imageData[((j  )%fh)*fw + ((i+1)%fw)] +
							imageData[((j+1)%fh)*fw + ((i+1)%fw)] +
							imageData[((j+1)%fh)*fw + ((i  )%fw)] +
							imageData[((j  )%fh)*fw + ((i-1)%fw)] +
							imageData[((j-1)%fh)*fw + ((i-1)%fw)] +
							imageData[((j-1)%fh)*fw + ((i  )%fw)] +
							imageData[((j-1)%fh)*fw + ((i+1)%fw)] +
							imageData[((j+1)%fh)*fw + ((i-1)%fw)])/(8+filter));
					}
				}
			}

			// On construit les mipmaps
			GLint level;
			if (texture->bpp == 1) level = GL_LUMINANCE;
			if (texture->bpp == 3) level = GL_RGB;
			if (texture->bpp == 4) level = GL_RGBA;
			gluBuild2DMipmaps(GL_TEXTURE_2D, texture->bpp, texture->size[0], texture->size[1],
							  GL_RGB, GL_UNSIGNED_BYTE, imageData);

			// On efface le data temporaire
			delete [] imageData;
		}
	}
#endif
}



//
// Pour changer de filter en cours de route
//
void			 dktChangeFilter(int filter)
{
#ifndef _DX_
	// On passe toute nos textures en loop
	for (int i=0;i<(int)CDkt::textures.size();i++)
	{
		CTexture *texture = CDkt::textures.at(i);

		if (texture)
		{
			glBindTexture(GL_TEXTURE_2D, texture->oglID);

			// On set le filter
			switch (filter)
			{
			case DKT_FILTER_NEAREST: // Nearest
				{
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
					break;
				}
			case DKT_FILTER_LINEAR: // Linear
				{
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
					break;
				}
			case DKT_FILTER_BILINEAR: // Bilinear
				{
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
					break;
				}
			case DKT_FILTER_TRILINEAR: // Trilinear
				{
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
					break;
				}
			default: // Nearest default
				{
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_NEAREST);
					break;
				}
			}
		}
	}
#endif
}



//
// Pour simplement reloader un TGA
//
void reloadTGA(CTexture * texture)
{
#ifndef _DX_
	// Les variables utilisé pour tenir l'information loadé du Targa
	unsigned char TGAcompare[12];
	unsigned char header[6];
	unsigned int bytesPerPixel;
	unsigned int imageSize;
	unsigned char temp;
	unsigned char *imageData;
	unsigned int bpp;
	unsigned int width;
	unsigned int height;

	// On ouvre le fichier targa
	FILE *file = fopen(texture->filename.s, "rb");

	// Si ça marche pas, oups, on returne 0 comme texture.
	if (file == NULL) 
	{
		// on écris l'erreur dans le log
		CDkt::updateLastError(CString("ERROR > Can not read file : \"%s\"", texture->filename.s).s);
		return;
	}

	// On li le header du fichier (12 premiers byte)
	fread(TGAcompare,1,sizeof(TGAcompare),file);

	// On li la suite du header
	fread(header,1,sizeof(header),file);

	// On prend le width et le height du header
	width  = header[1] * 256 + header[0];
	height = header[3] * 256 + header[2];
	texture->size.set(width, height);

	// On prend le bbp (24bit ou 32bit)
	bpp	= header[4];

	// On le divise par 8 pour avoir 3 ou 4 bytes (RGB ou RGBA)
	bytesPerPixel	= bpp/8;
	texture->bpp = bytesPerPixel;

	// On calcul la grandeur de l'image en byte
	imageSize		= width * height * bytesPerPixel;

	// On alou alors autant de bytes qu'il faut pour tenir l'image
	imageData = new unsigned char [imageSize];

	// On li maintenant le gros bloc de données
	fread(imageData, 1, imageSize, file);

	// On défini si c'est RGB ou RGBA
	GLint Level = (bytesPerPixel == 3) ? GL_RGB : GL_RGBA;

	// Ici c'est con, mais faut switcher le rouge avec le bleu
	for(unsigned int i=0; i<imageSize; i+=bytesPerPixel){

		temp=imageData[i];
		imageData[i] = imageData[i + 2];
		imageData[i + 2] = temp;
	}

	// On ferme maintenant le fichier
	fclose (file);

	// On bind cette texture au context
	glBindTexture(GL_TEXTURE_2D, texture->oglID);

	// On construit les mipmaps
	gluBuild2DMipmaps(GL_TEXTURE_2D, bytesPerPixel, width, height,
					  Level, GL_UNSIGNED_BYTE, imageData);

	// On delete notre Data qu'on n'a pus besoin
	delete [] imageData;
#endif
}



//
// Loader un TGA
//
unsigned int createTextureTGA(char * filename, int filter){

#ifndef _DX_
	// Notre texture ID de ogl
	unsigned int Texture = 0;

		// On check quelle n'existe pas déjà
		for (int l=0;l<(int)CDkt::textures.size();l++)
		{
			CTexture *texture = CDkt::textures.at(l);
			if (texture->filename == filename)
			{
				texture->nbInstance++;
				return texture->oglID;
			}
		}

		// Les variables utilisé pour tenir l'information loadé du Targa
		unsigned char TGAcompare[12];
		unsigned char header[6];
		unsigned int bytesPerPixel;
		unsigned int imageSize;
		unsigned char temp;
		unsigned char *imageData;
		unsigned int bpp;
		unsigned int width;
		unsigned int height;

		// On ouvre le fichier targa
		FILE *file = fopen(filename, "rb");

		// Si ça marche pas, oups, on returne 0 comme texture.
		if (file == NULL) 
		{
			// on écris l'erreur dans le log
            printf("Cannot open texutre file: %s\n", filename);
			CDkt::updateLastError(CString("ERROR > Can not read file : \"%s\"", filename).s);
			return 0;
		}

		// On li le header du fichier (12 premiers byte)
		fread(TGAcompare,1,sizeof(TGAcompare),file);

		// On li la suite du header
		fread(header,1,sizeof(header),file);

		// On prend le width et le height du header
		width  = header[1] * 256 + header[0];
		height = header[3] * 256 + header[2];

		// On prend le bbp (24bit ou 32bit)
		bpp	= header[4];

		// On le divise par 8 pour avoir 3 ou 4 bytes (RGB ou RGBA)
		bytesPerPixel	= bpp/8;

		// On calcul la grandeur de l'image en byte
		imageSize		= width * height * bytesPerPixel;

		// On alou alors autant de bytes qu'il faut pour tenir l'image
		imageData = new unsigned char [imageSize];

		// On li maintenant le gros bloc de données
		fread(imageData, 1, imageSize, file);

		// On défini si c'est RGB ou RGBA
		GLint Level = (bytesPerPixel == 3) ? GL_RGB : GL_RGBA;

		// Ici c'est con, mais faut switcher le rouge avec le bleu
		for(unsigned int i=0; i<imageSize; i+=bytesPerPixel){

			temp=imageData[i];
			imageData[i] = imageData[i + 2];
			imageData[i + 2] = temp;
		}

		// On ferme maintenant le fichier
		fclose (file);

		// On génère une texture
		glGenTextures(1, &Texture);

		// On bind cette texture au context
		glBindTexture(GL_TEXTURE_2D, Texture);

		// On set le filter
		switch (filter)
		{
		case DKT_FILTER_NEAREST: // Nearest
			{
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
				break;
			}
		case DKT_FILTER_LINEAR: // Linear
			{
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
				break;
			}
		case DKT_FILTER_BILINEAR: // Bilinear
			{
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
				break;
			}
		case DKT_FILTER_TRILINEAR: // Trilinear
			{
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
				break;
			}
		default: // Nearest default
			{
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_NEAREST);
				break;
			}
		}

		// On construit les mipmaps
		gluBuild2DMipmaps(GL_TEXTURE_2D, bytesPerPixel, width, height,
						  Level, GL_UNSIGNED_BYTE, imageData);

		// On delete notre Data qu'on n'a pus besoin
		delete [] imageData;

		// On se cré notre nouvelle texture
		CTexture *texture = new CTexture;
		texture->filename = filename;
		texture->nbInstance = 1;
		texture->size.set(width, height);
		texture->bpp = bytesPerPixel;
		texture->oglID = Texture;

		struct stat attrib;
		stat(filename, &attrib);
		texture->modifDate = INT4(attrib.st_mtime);

		CDkt::textures.push_back(texture);

	// On retourne l'index de la texture
	return Texture;
#else
	return 0;
#endif
}



//
// Pour créer une texture vide
//
unsigned int	 dktCreateEmptyTexture(int w, int h, int bpp, int filter)
{
#ifndef _DX_
	unsigned int textureID=0;
	
	// On se cré notre nouvelle texture
	CTexture *texture = new CTexture;
	texture->filename = "Custom";
	texture->nbInstance = 1;
	CDkt::textures.push_back(texture);
	texture->size.set(w,h);

	// On cré notre array
	int totalSize = w*h*bpp;
	unsigned char *buffer = new unsigned char[w*h*bpp];
	for (int i=0;i<totalSize;buffer[i++] = 255);

	// On cré une texture ogl et on la bind
	glGenTextures(1, &textureID);
	texture->oglID = textureID;
	glBindTexture(GL_TEXTURE_2D, textureID);

	// On check le level
	GLuint level = 0;
	if (bpp == 1) level = GL_LUMINANCE;
	if (bpp == 3) level = GL_RGB;
	if (bpp == 4) level = GL_RGBA;
	texture->bpp = bpp;

	// On set le filter
	switch (filter)
	{
	case DKT_FILTER_NEAREST: // Nearest
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
			break;
		}
	case DKT_FILTER_LINEAR: // Linear
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			break;
		}
	case DKT_FILTER_BILINEAR: // Bilinear
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
			break;
		}
	case DKT_FILTER_TRILINEAR: // Trilinear
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			break;
		}
	default: // Nearest default
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_NEAREST);
			break;
		}
	}

	// On construit la texture et ses mipmap
	gluBuild2DMipmaps(GL_TEXTURE_2D, bpp, w, h,
					  level, GL_UNSIGNED_BYTE, buffer);

	delete [] buffer;

	return textureID;
#else
	return 0;
#endif
}



//
// Pour créer une texture à partir d'un buffer
//
void			 dktCreateTextureFromBuffer(unsigned int *textureID, unsigned char *buffer, int w, int h, int bpp, int filter)
{
#ifndef _DX_
	// On delete l'ancienne (elle DOIT exister)
	dktDeleteTexture(textureID);

	// On se cré notre nouvelle texture
	CTexture *texture = new CTexture;
	texture->filename = "Custom";
	texture->nbInstance = 1;
	CDkt::textures.push_back(texture);

	// On cré une texture ogl et on la bind
	glGenTextures(1, textureID);
	texture->oglID = *textureID;
	glBindTexture(GL_TEXTURE_2D, *textureID);

	// On set le filter
	switch (filter)
	{
	case DKT_FILTER_NEAREST: // Nearest
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
			break;
		}
	case DKT_FILTER_LINEAR: // Linear
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			break;
		}
	case DKT_FILTER_BILINEAR: // Bilinear
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
			break;
		}
	case DKT_FILTER_TRILINEAR: // Trilinear
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			break;
		}
	default: // Nearest default
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_NEAREST);
			break;
		}
	}

	// On check le level
	GLuint level = 0;
	if (bpp == 1) level = GL_LUMINANCE;
	if (bpp == 3) level = GL_RGB;
	if (bpp == 4) level = GL_RGBA;

	texture->size.set(w,h);
	texture->bpp = bpp;

	// On construit la texture et ses mipmap
	gluBuild2DMipmaps(GL_TEXTURE_2D, bpp, w, h,
					  level, GL_UNSIGNED_BYTE, buffer);
#endif
}



//
// Pour créer une texture à partir d'une image
//
unsigned int	 dktCreateTextureFromFile(char *mFilename, int filter)
{
	if (strnicmp(&(mFilename[strlen(mFilename)-3]), "TGA", 3) == 0)

	{
		return createTextureTGA(mFilename, filter);
	}
	else
	{
		// Doit obligatoirement être un TGA
		CDkt::updateLastError("DKT : The image is not a TGA");
		return 0;
	}
	return 0;
}



//
// On update la dernière erreur
//
void CDkt::updateLastError(char *error)
{
	if (lastErrorString)
	{
		delete [] lastErrorString;
		lastErrorString = 0;
	}
	if(error)
	{
		lastErrorString = new char [strlen(error)+1];
		strcpy(lastErrorString, error);
	}
}



//
// Pour effacer une texture du stack
//
void			 dktDeleteTexture(unsigned int *textureID)
{
#ifndef _DX_
	for (int i=0;i<(int)CDkt::textures.size();i++)
	{
		CTexture *texture = CDkt::textures.at(i);
		if (texture->oglID == *textureID) 
		{
			texture->nbInstance--;
			if (texture->nbInstance <= 0)
			{
				CDkt::textures.erase(CDkt::textures.begin() + i);
				delete texture;
			}
			break;
		}
	}

	*textureID = 0;
#endif
}



//
// Pour obtenir la dernière erreur
//
char*			 dktGetLastError()
{
	return CDkt::lastErrorString;
}



//
// Obtenir le byte per pixel d'une texture
//
int				 dktGetTextureBytePerPixel(unsigned int textureID)
{
#ifndef _DX_
	for (int i=0;i<(int)CDkt::textures.size();i++)
	{
		CTexture *texture = CDkt::textures.at(i);
		if (texture->oglID == textureID) 
		{
			return texture->bpp;
		}
	}
#endif
	return 0; // La texture n'est pas trouvé
}



//
// Pour obtenir le data d'une texture ogl
//
void			 dktGetTextureData(unsigned int textureID, unsigned char * data)
{
#ifndef _DX_
	for (int i=0;i<(int)CDkt::textures.size();i++)
	{
		CTexture *texture = CDkt::textures.at(i);
		if (texture->oglID == textureID) 
		{
			glBindTexture(GL_TEXTURE_2D, textureID);
			glGetTexImage(GL_TEXTURE_2D, 0, (texture->bpp==3)?GL_RGB:GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
	}
#endif
	data = 0;
}



//
// Pour obtenir la grandeur de la texture
//
CVector2i		 dktGetTextureSize(unsigned int textureID)
{
#ifndef _DX_
	for (int i=0;i<(int)CDkt::textures.size();i++)
	{
		CTexture *texture = CDkt::textures.at(i);
		if (texture->oglID == textureID) 
		{
			return texture->size;
		}
	}
#endif
	return CVector2i();
}



//
// Pour initialiser dkt
//
void			 dktInit()
{
	// On a enlevé DevIL
//	ilInit();
}



//
// Pour renderer sur une texture
//
void			 dktRenderToTexture(unsigned int textureID, int x, int y, int w, int h, unsigned int internalFormat)
{
#ifndef _DX_
	glBindTexture(GL_TEXTURE_2D, textureID);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, internalFormat, x, y, w, h, 0);
#endif
}



//
// Pour effacer le tout à la fin
//
void			 dktShutDown()
{
	for (int i=0;i<(int)CDkt::textures.size();i++)
	{
		CTexture *texture = CDkt::textures.at(i);
		if (texture) delete texture;
	}
	CDkt::textures.clear();
	CDkt::updateLastError(0);
}



//
// Pour updater, ça check si une texture a été modifié à l'extérieur du programme pis ça la replace si cest le cas
//
void			 dktUpdate()
{
/*	if (CDkt::checkingUpdate >= (int)CDkt::textures.size()) CDkt::checkingUpdate = 0;

	if (CDkt::textures.size() > 0)
	{
		CTexture *texture = CDkt::textures.at(CDkt::checkingUpdate);

		struct stat attrib;
		stat(texture->filename.s, &attrib);
		INT4 tmpModifDate = INT4(attrib.st_mtime);

		// Ça fonctionne juste avec les .tga notre affaire
		if (texture->filename.checkExtension("tga"))
		{
			reloadTGA(texture);
		}

		texture->modifDate = tmpModifDate;
	}

	CDkt::checkingUpdate++;*/
}
