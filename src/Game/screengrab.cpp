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
#if defined(_PRO_)
#include "screengrab.h"
#include "Zeven.h"
#include "GameVar.h"
#include "Game.h"
#include "Player.h"
#include "Scene.h"
#include <GL\gl.h>
#include <time.h>
#include <stdio.h>


extern Scene* scene;

void SaveBitmapToFile( BYTE* pBitmapBits, LONG lWidth, LONG lHeight,WORD wBitsPerPixel, LPCTSTR lpszFileName )
{
    BITMAPINFOHEADER bmpInfoHeader = {0};
    // Set the size
    bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    // Bit count
    bmpInfoHeader.biBitCount = wBitsPerPixel;
    // Use all colors
    bmpInfoHeader.biClrImportant = 0;
    // Use as many colors according to bits per pixel
    bmpInfoHeader.biClrUsed = 0;
    // Store as un Compressed
    bmpInfoHeader.biCompression = BI_RGB;
    // Set the height in pixels
    bmpInfoHeader.biHeight = lHeight;
    // Width of the Image in pixels
    bmpInfoHeader.biWidth = lWidth;
    // Default number of planes
    bmpInfoHeader.biPlanes = 1;
    // Calculate the image size in bytes
    bmpInfoHeader.biSizeImage = lWidth* lHeight * (wBitsPerPixel/8);

    BITMAPFILEHEADER bfh = {0};
    // This value should be values of BM letters i.e 0×4D42
    // 0×4D = M 0×42 = B storing in reverse order to match with endian
    bfh.bfType=0x4D42;
    /* or
    bfh.bfType = ‘B’+(’M’ << 8);
    // <<8 used to shift ‘M’ to end
    */
    // Offset to the RGBQUAD
    bfh.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
    // Total size of image including size of headers
    bfh.bfSize = bfh.bfOffBits + bmpInfoHeader.biSizeImage;
    // Create the file in disk to write
    HANDLE hFile = CreateFile( lpszFileName,GENERIC_WRITE, 0,NULL,

                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL);

    if( !hFile ) // return if error opening file
    {
        return;
    }

    DWORD dwWritten = 0;
    // Write the File header
    WriteFile( hFile, &bfh, sizeof(bfh), &dwWritten , NULL );
    // Write the bitmap info header
    WriteFile( hFile, &bmpInfoHeader, sizeof(bmpInfoHeader), &dwWritten, NULL );
    // Write the RGB Data
    WriteFile( hFile, pBitmapBits, bmpInfoHeader.biSizeImage, &dwWritten, NULL );
    // Close the file handle
    CloseHandle( hFile );
}

bool SaveScreenGrabAuto() 
{
   char path[512];
   time_t time;
   ::time(&time);
   sprintf(path, "SS_%d.bmp", time);
   return SaveScreenGrab(path);
}

bool SaveStatsAuto()
{
   char path[512];
   time_t time;
   ::time(&time);
   sprintf(path, "SS_%d.bmp", time);
   SaveScreenGrab(path);
   sprintf(path, "SS_%d.txt", time);

  FILE * pFile;
  pFile = fopen (path,"w");
  if (pFile!=NULL)
  {
/*
	CString mapName;

	// Le type de jeu et les scores
	int gameType;
   int spawnType;
   int subGameType;
	int blueScore;
	int redScore;
	int blueWin;
	int redWin;
   */
    Game* pGame = scene->client->game;


    fputs (pGame->mapName.s,pFile);
    fprintf(pFile, "\nBlue:%d\nRed:%d\n", pGame->blueScore, pGame->redScore);
   
	 for (int i=0;i<MAX_PLAYER;++i)
	 {
       if (pGame->players[i])
		 {         
       Player* pPlayer = pGame->players[i];       
       fprintf(pFile, "PlayerName:%s\n", textColorLess(pPlayer->name).s);       
       fprintf(pFile, "PlayerId:%d\nTeamId:%d\n", pPlayer->playerID, pPlayer->teamID);              
       fprintf(pFile, "Kills:%d\n", pPlayer->kills);
       fprintf(pFile, "Deaths:%d\n", pPlayer->deaths);
       fprintf(pFile, "Damage:%f\n", pPlayer->dmg);
       fprintf(pFile, "FlagAttempts:%d\n", pPlayer->flagAttempts);
       fprintf(pFile, "Returns:%d\n", pPlayer->returns);
       fprintf(pFile, "Score:%d\n", pPlayer->score);       
		 }
	 }
   
    fclose(pFile);
  }
 
   return true;
}



bool SaveScreenGrab(const char* filename) {

	// get some info about the screen size
   CVector2i res = dkwGetResolution();
   
   int sw           =  res.x();
	int sh           =  res.y();
   int bitdepth     =  32;
   GLenum   format  =  GL_BGRA_EXT;
   int bpp          =  4;
	//int bitdepth     =  gameVar.r_bitdepth;
	//GLenum   format  =  (bitdepth==32) ? GL_BGRA_EXT : GL_BGR_EXT;
   //int bpp          =  (bitdepth==32) ? 4 : 3;

	// allocate memory to store image data
	unsigned char* pdata = new unsigned char[sw*sh*bpp];
#ifndef _DX_
	// read from front buffer
	glReadBuffer(GL_FRONT);

	// read pixel data
	glReadPixels(0,0,sw,sh,format,GL_UNSIGNED_BYTE,pdata);

	// write data as a tga file
   SaveBitmapToFile(pdata,sw,sh,bitdepth,filename);
#endif

	// clean up
	delete [] pdata;

	// done
	return true;
}

#endif
#endif
