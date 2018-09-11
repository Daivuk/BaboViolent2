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


#define BREBIS_GL_IMPLEMENTATION
#include "dkgli.h"

static SDL_GLContext glContext = nullptr;


#include <algorithm>
#include <memory.h>

#define TORAD 0.01745329251994329576923690768489f

static void createViewMatrix(const float position[3], const float target[3], const float up[3], float out[4][4])
{
    // Normalized direction
    float R2[3] = {
        target[0] - position[0],
        target[1] - position[1],
        target[2] - position[2]
    };

    float R0[3] = {
        up[1] * R2[2] - up[2] * R2[1],
        up[2] * R2[0] - up[0] * R2[2],
        up[0] * R2[1] - up[1] * R2[0]
    };
    float len = std::sqrtf(R0[0] * R0[0] + R0[1] * R0[1]);
    R0[0] /= len;
    R0[1] /= len;

    float R1[3] = {
        R2[1] * R0[2] - R2[2] * R0[1],
        R2[2] * R0[0] - R2[0] * R0[2],
        R2[0] * R0[1] - R2[1] * R0[0]
    };

    auto D0 = R0[0] * -position[0] + R0[1] * -position[1] + R0[2] * -position[2];
    auto D1 = R1[0] * -position[0] + R1[1] * -position[1] + R1[2] * -position[2];
    auto D2 = R2[0] * -position[0] + R2[1] * -position[1] + R2[2] * -position[2];

    out[0][0] = R0[0];
    out[0][1] = R1[0];
    out[0][2] = R2[0];
    out[0][3] = 0.0f;

    out[1][0] = R0[1];
    out[1][1] = R1[1];
    out[1][2] = R2[1];
    out[1][3] = 0.0f;

    out[2][0] = R0[2];
    out[2][1] = R1[2];
    out[2][2] = R2[2];
    out[2][3] = 0.0f;

    out[3][0] = D0;
    out[3][1] = D1;
    out[3][2] = D2;
    out[3][3] = 1.0f;
}

static void createViewMatrix(const float position[3], float angleX, float angleZ, float out[4][4])
{
    // Normalized direction
    float R2[3] = {
        -std::sinf(angleZ * TORAD) * std::cosf(angleX * TORAD),
        -std::cosf(angleZ * TORAD) * std::cosf(angleX * TORAD),
        -std::sinf(angleX * TORAD)
    };

    float R0[3] = {
        -1 * R2[1],
        1 * R2[0],
        0
    };
    float len = std::sqrtf(R0[0] * R0[0] + R0[1] * R0[1]);
    R0[0] /= len;
    R0[1] /= len;

    float R1[3] = {
        R2[1] * R0[2] - R2[2] * R0[1],
        R2[2] * R0[0] - R2[0] * R0[2],
        R2[0] * R0[1] - R2[1] * R0[0]
    };

    auto D0 = R0[0] * -position[0] + R0[1] * -position[1] + R0[2] * -position[2];
    auto D1 = R1[0] * -position[0] + R1[1] * -position[1] + R1[2] * -position[2];
    auto D2 = R2[0] * -position[0] + R2[1] * -position[1] + R2[2] * -position[2];

    out[0][0] = R0[0];
    out[0][1] = R1[0];
    out[0][2] = R2[0];
    out[0][3] = 0.0f;

    out[1][0] = R0[1];
    out[1][1] = R1[1];
    out[1][2] = R2[1];
    out[1][3] = 0.0f;

    out[2][0] = R0[2];
    out[2][1] = R1[2];
    out[2][2] = R2[2];
    out[2][3] = 0.0f;

    out[3][0] = D0;
    out[3][1] = D1;
    out[3][2] = D2;
    out[3][3] = 1.0f;
}

static void createPerspectiveFieldOfView(float fov, float aspectRatio, float nearPlane, float farPlane, float out[4][4])
{
    float CosFov = std::cos(0.5f * fov * TORAD);
    float SinFov = std::sin(0.5f * fov * TORAD);

    float Height = CosFov / SinFov;
    float Width = Height / aspectRatio;
    float fRange = farPlane / (nearPlane - farPlane);

    out[0][0] = Width;
    out[0][1] = 0.0f;
    out[0][2] = 0.0f;
    out[0][3] = 0.0f;

    out[1][0] = 0.0f;
    out[1][1] = Height;
    out[1][2] = 0.0f;
    out[1][3] = 0.0f;

    out[2][0] = 0.0f;
    out[2][1] = 0.0f;
    out[2][2] = fRange;
    out[2][3] = -1.0f;

    out[3][0] = 0.0f;
    out[3][1] = 0.0f;
    out[3][2] = fRange * nearPlane;
    out[3][3] = 0.0f;
}

static void mulMatrix(float a[4][4], float b[4][4], float out[4][4])
{
    memcpy(out, a, sizeof(out) * 16);
    // Cache the invariants in registers
    float x = out[0][0];
    float y = out[0][1];
    float z = out[0][2];
    float w = out[0][3];
    // Perform the operation on the first row
    out[0][0] = (b[0][0] * x) + (b[1][0] * y) + (b[2][0] * z) + (b[3][0] * w);
    out[0][1] = (b[0][1] * x) + (b[1][1] * y) + (b[2][1] * z) + (b[3][1] * w);
    out[0][2] = (b[0][2] * x) + (b[1][2] * y) + (b[2][2] * z) + (b[3][2] * w);
    out[0][3] = (b[0][3] * x) + (b[1][3] * y) + (b[2][3] * z) + (b[3][3] * w);
    // Repeat for all the other rows
    x = out[1][0];
    y = out[1][1];
    z = out[1][2];
    w = out[1][3];
    out[1][0] = (b[0][0] * x) + (b[1][0] * y) + (b[2][0] * z) + (b[3][0] * w);
    out[1][1] = (b[0][1] * x) + (b[1][1] * y) + (b[2][1] * z) + (b[3][1] * w);
    out[1][2] = (b[0][2] * x) + (b[1][2] * y) + (b[2][2] * z) + (b[3][2] * w);
    out[1][3] = (b[0][3] * x) + (b[1][3] * y) + (b[2][3] * z) + (b[3][3] * w);
    x = out[2][0];
    y = out[2][1];
    z = out[2][2];
    w = out[2][3];
    out[2][0] = (b[0][0] * x) + (b[1][0] * y) + (b[2][0] * z) + (b[3][0] * w);
    out[2][1] = (b[0][1] * x) + (b[1][1] * y) + (b[2][1] * z) + (b[3][1] * w);
    out[2][2] = (b[0][2] * x) + (b[1][2] * y) + (b[2][2] * z) + (b[3][2] * w);
    out[2][3] = (b[0][3] * x) + (b[1][3] * y) + (b[2][3] * z) + (b[3][3] * w);
    x = out[3][0];
    y = out[3][1];
    z = out[3][2];
    w = out[3][3];
    out[3][0] = (b[0][0] * x) + (b[1][0] * y) + (b[2][0] * z) + (b[3][0] * w);
    out[3][1] = (b[0][1] * x) + (b[1][1] * y) + (b[2][1] * z) + (b[3][1] * w);
    out[3][2] = (b[0][2] * x) + (b[1][2] * y) + (b[2][2] * z) + (b[3][2] * w);
    out[3][3] = (b[0][3] * x) + (b[1][3] * y) + (b[2][3] * z) + (b[3][3] * w);
}

void dkglEnableVsync(bool vsync)
{
    SDL_GL_SetSwapInterval(vsync ? 1 : 0);
}

//
// Pour vérifier une extension de la carte
//
bool dkglCheckExtension(char * extension)
{
    return false;
}


//
// Pour créer le context openGL (rendering context)
//
int dkglCreateContext(SDL_GLContext mDC, int colorDepth)
{
    glContext = mDC;
	return 1;
}

//
// Pour dessiner le system de coordonnée
//
void dkglDrawCoordSystem()
{
#ifndef _DX_
	glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT);
		glLineWidth(2);
		glDisable(GL_CULL_FACE);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glColor3f(1,0,0);
		glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3f(1,0,0);

			glVertex3f(1,0,0);
			glVertex3f(.9f,.1f,.1f);
			glVertex3f(1,0,0);
			glVertex3f(.9f,-.1f,.1f);
			glVertex3f(1,0,0);
			glVertex3f(.9f,-.1f,-.1f);
			glVertex3f(1,0,0);
			glVertex3f(.9f,.1f,-.1f);
		glEnd();
		glColor3f(0,1,0);
		glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3f(0,1,0);

			glVertex3f(0,1,0);
			glVertex3f(.1f,.9f,.1f);
			glVertex3f(0,1,0);
			glVertex3f(-.1f,.9f,.1f);
			glVertex3f(0,1,0);
			glVertex3f(-.1f,.9f,-.1f);
			glVertex3f(0,1,0);
			glVertex3f(.1f,.9f,-.1f);
		glEnd();
		glColor3f(0,0,1);
		glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3f(0,0,1);

			glVertex3f(0,0,1);
			glVertex3f(.1f,.1f,.9f);
			glVertex3f(0,0,1);
			glVertex3f(-.1f,.1f,.9f);
			glVertex3f(0,0,1);
			glVertex3f(-.1f,-.1f,.9f);
			glVertex3f(0,0,1);
			glVertex3f(.1f,-.1f,.9f);
		glEnd();
	glPopAttrib();
#endif
}



//
// Draw wire cube
//
void dkglDrawWireCube()
{
#ifndef _DX_
	glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT);
		glLineWidth(2);
		glDisable(GL_CULL_FACE);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glColor3f(1,1,0);
		glBegin(GL_LINE_LOOP);
			glVertex3f(-1,-1,-1);
			glVertex3f(-1,1,-1);
			glVertex3f(1,1,-1);
			glVertex3f(1,-1,-1);
		glEnd();
		glBegin(GL_LINE_LOOP);
			glVertex3f(-1,1,1);
			glVertex3f(-1,-1,1);
			glVertex3f(1,-1,1);
			glVertex3f(1,1,1);
		glEnd();
		glBegin(GL_LINES);
			glVertex3f(-1,1,1);
			glVertex3f(-1,1,-1);
			glVertex3f(-1,-1,1);
			glVertex3f(-1,-1,-1);
			glVertex3f(1,-1,1);
			glVertex3f(1,-1,-1);
			glVertex3f(1,1,1);
			glVertex3f(1,1,-1);
		glEnd();
	glPopAttrib();
#endif
}



//
// Pour revenir en vue 3D
//
void dkglPopOrtho()
{
#ifndef _DX_
	// On pop nos matrice
			glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	// On pop nos attribs
	glPopAttrib();
#endif
}



//
// Pour setter la vue en 2D
//
void dkglPushOrtho(float mWidth, float mHeight)
{
#ifndef _DX_
	// On push les attribs pour certaines modifications
	glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);
//	glCullFace(GL_BACK);

	// En mode 2D on ne veux pas de z-buffer
	glDisable(GL_DEPTH_TEST);

	// On push nos matrice et on set la matrice ortho
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		glOrtho(0,mWidth,mHeight,0,-9999,9999);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();
#endif
}



//
// Pour setter la fonction de blending
//
void dkglSetBlendingFunc(int blending)
{
#ifndef _DX_
	switch (blending)
	{
	case DKGL_BLENDING_ADD_SATURATE:
		glBlendFunc(GL_ONE, GL_ONE);
		break;
	case DKGL_BLENDING_ADD:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		break;
	case DKGL_BLENDING_MULTIPLY:
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		break;
	case DKGL_BLENDING_ALPHA:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	}
#endif
}



//
// Pour rapidement créer une lumière dans votre scene (très basic)
//
void dkglSetPointLight(int ID, float x, float y, float z, float r, float g, float b)
{
#ifndef _DX_
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0 + ID);
	float pos[] = {x,y,z,1};
	float amb[] = {r/6,g/6,b/6,1};
	float diff[] = {r,g,b,1};
	float spec[] = {r,g,b,1};

	glLightfv(GL_LIGHT0 + ID, GL_POSITION, pos);
	glLightfv(GL_LIGHT0 + ID, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0 + ID, GL_DIFFUSE, diff);
	glLightfv(GL_LIGHT0 + ID, GL_SPECULAR, spec);
#endif
}



//
// On set la vue de la perspective là
//
void dkglSetProjection(float mFieldOfView, float mNear, float mFar, float mWidth, float mHeight)
{
#ifndef _DX_
	// On met la matrice de projection pour ce créer une vue perspective
	glMatrixMode(GL_PROJECTION);

	// On remet cette matrice à identity
	glLoadIdentity();

	// On ajuste la matrice de projection
    float proj[4][4];
    createPerspectiveFieldOfView(mFieldOfView, mWidth / mHeight, mNear, mFar, proj);
    glMultMatrixf(&proj[0][0]);

	// On remet cette de model view (qui est celle de la position et l'orientation)
	glMatrixMode(GL_MODELVIEW);

	// La model view à identity
	glLoadIdentity();
#endif
}

int glhInvertMatrixf2(float *m, float *out);

int glhProjectf(float objx, float objy, float objz, float *modelview, float *projection, int *viewport, float *windowCoordinate)
{
    // Transformation vectors
    float fTempo[8];
    // Modelview transform
    fTempo[0] = modelview[0] * objx + modelview[4] * objy + modelview[8] * objz + modelview[12]; // w is always 1
    fTempo[1] = modelview[1] * objx + modelview[5] * objy + modelview[9] * objz + modelview[13];
    fTempo[2] = modelview[2] * objx + modelview[6] * objy + modelview[10] * objz + modelview[14];
    fTempo[3] = modelview[3] * objx + modelview[7] * objy + modelview[11] * objz + modelview[15];
    // Projection transform, the final row of projection matrix is always [0 0 -1 0]
    // so we optimize for that.
    fTempo[4] = projection[0] * fTempo[0] + projection[4] * fTempo[1] + projection[8] * fTempo[2] + projection[12] * fTempo[3];
    fTempo[5] = projection[1] * fTempo[0] + projection[5] * fTempo[1] + projection[9] * fTempo[2] + projection[13] * fTempo[3];
    fTempo[6] = projection[2] * fTempo[0] + projection[6] * fTempo[1] + projection[10] * fTempo[2] + projection[14] * fTempo[3];
    fTempo[7] = -fTempo[2];
    // The result normalizes between -1 and 1
    if (fTempo[7] == 0.0f) // The w value
        return 0;
    fTempo[7] = 1.0f / fTempo[7];
    // Perspective division
    fTempo[4] *= fTempo[7];
    fTempo[5] *= fTempo[7];
    fTempo[6] *= fTempo[7];
    // Window coordinates
    // Map x, y to range 0-1
    windowCoordinate[0] = (fTempo[4] * 0.5f + 0.5f)*viewport[2] + viewport[0];
    windowCoordinate[1] = (fTempo[5] * 0.5f + 0.5f)*viewport[3] + viewport[1];
    // This is only correct when glDepthRange(0.0, 1.0)
    windowCoordinate[2] = (1.0f + fTempo[6])*0.5f;	// Between 0 and 1
    return 1;
}

void MultiplyMatrices4by4OpenGL_FLOAT(float *result, float *matrix1, float *matrix2);
void MultiplyMatrixByVector4by4OpenGL_FLOAT(float *resultvector, const float *matrix, const float *pvector);

int glhUnProjectf(float winx, float winy, float winz, float *modelview, float *projection, int *viewport, float *objectCoordinate)
{
    // Transformation matrices
    float m[16], A[16];
    float in[4], out[4];
    // Calculation for inverting a matrix, compute projection x modelview
    // and store in A[16]
    MultiplyMatrices4by4OpenGL_FLOAT(A, projection, modelview);
    // Now compute the inverse of matrix A
    if (glhInvertMatrixf2(A, m) == 0)
        return 0;
    // Transformation of normalized coordinates between -1 and 1
    in[0] = (winx - (float)viewport[0]) / (float)viewport[2] * 2.0f - 1.0f;
    in[1] = (winy - (float)viewport[1]) / (float)viewport[3] * 2.0f - 1.0f;
    in[2] = 2.0f*winz - 1.0f;
    in[3] = 1.0f;
    // Objects coordinates
    MultiplyMatrixByVector4by4OpenGL_FLOAT(out, m, in);
    if (out[3] == 0.0f)
        return 0;
    out[3] = 1.0f / out[3];
    objectCoordinate[0] = out[0] * out[3];
    objectCoordinate[1] = out[1] * out[3];
    objectCoordinate[2] = out[2] * out[3];
    return 1;
}

void MultiplyMatrices4by4OpenGL_FLOAT(float *result, float *matrix1, float *matrix2)
{
    result[0] = matrix1[0] * matrix2[0] +
        matrix1[4] * matrix2[1] +
        matrix1[8] * matrix2[2] +
        matrix1[12] * matrix2[3];
    result[4] = matrix1[0] * matrix2[4] +
        matrix1[4] * matrix2[5] +
        matrix1[8] * matrix2[6] +
        matrix1[12] * matrix2[7];
    result[8] = matrix1[0] * matrix2[8] +
        matrix1[4] * matrix2[9] +
        matrix1[8] * matrix2[10] +
        matrix1[12] * matrix2[11];
    result[12] = matrix1[0] * matrix2[12] +
        matrix1[4] * matrix2[13] +
        matrix1[8] * matrix2[14] +
        matrix1[12] * matrix2[15];
    result[1] = matrix1[1] * matrix2[0] +
        matrix1[5] * matrix2[1] +
        matrix1[9] * matrix2[2] +
        matrix1[13] * matrix2[3];
    result[5] = matrix1[1] * matrix2[4] +
        matrix1[5] * matrix2[5] +
        matrix1[9] * matrix2[6] +
        matrix1[13] * matrix2[7];
    result[9] = matrix1[1] * matrix2[8] +
        matrix1[5] * matrix2[9] +
        matrix1[9] * matrix2[10] +
        matrix1[13] * matrix2[11];
    result[13] = matrix1[1] * matrix2[12] +
        matrix1[5] * matrix2[13] +
        matrix1[9] * matrix2[14] +
        matrix1[13] * matrix2[15];
    result[2] = matrix1[2] * matrix2[0] +
        matrix1[6] * matrix2[1] +
        matrix1[10] * matrix2[2] +
        matrix1[14] * matrix2[3];
    result[6] = matrix1[2] * matrix2[4] +
        matrix1[6] * matrix2[5] +
        matrix1[10] * matrix2[6] +
        matrix1[14] * matrix2[7];
    result[10] = matrix1[2] * matrix2[8] +
        matrix1[6] * matrix2[9] +
        matrix1[10] * matrix2[10] +
        matrix1[14] * matrix2[11];
    result[14] = matrix1[2] * matrix2[12] +
        matrix1[6] * matrix2[13] +
        matrix1[10] * matrix2[14] +
        matrix1[14] * matrix2[15];
    result[3] = matrix1[3] * matrix2[0] +
        matrix1[7] * matrix2[1] +
        matrix1[11] * matrix2[2] +
        matrix1[15] * matrix2[3];
    result[7] = matrix1[3] * matrix2[4] +
        matrix1[7] * matrix2[5] +
        matrix1[11] * matrix2[6] +
        matrix1[15] * matrix2[7];
    result[11] = matrix1[3] * matrix2[8] +
        matrix1[7] * matrix2[9] +
        matrix1[11] * matrix2[10] +
        matrix1[15] * matrix2[11];
    result[15] = matrix1[3] * matrix2[12] +
        matrix1[7] * matrix2[13] +
        matrix1[11] * matrix2[14] +
        matrix1[15] * matrix2[15];
}

void MultiplyMatrixByVector4by4OpenGL_FLOAT(float *resultvector, const float *matrix, const float *pvector)
{
    resultvector[0] = matrix[0] * pvector[0] + matrix[4] * pvector[1] + matrix[8] * pvector[2] + matrix[12] * pvector[3];
    resultvector[1] = matrix[1] * pvector[0] + matrix[5] * pvector[1] + matrix[9] * pvector[2] + matrix[13] * pvector[3];
    resultvector[2] = matrix[2] * pvector[0] + matrix[6] * pvector[1] + matrix[10] * pvector[2] + matrix[14] * pvector[3];
    resultvector[3] = matrix[3] * pvector[0] + matrix[7] * pvector[1] + matrix[11] * pvector[2] + matrix[15] * pvector[3];
}

#define SWAP_ROWS_DOUBLE(a, b) { double *_tmp = a; (a) = (b); (b) = _tmp; }
#define SWAP_ROWS_FLOAT(a, b) { float *_tmp = a; (a) = (b); (b) = _tmp; }
#define MAT(m, r, c) (m)[(c) * 4 + (r)]

// This code comes directly from GLU except that it is for float
int glhInvertMatrixf2(float *m, float *out)
{
    float wtmp[4][8];
    float m0, m1, m2, m3, s;
    float *r0, *r1, *r2, *r3;
    r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];
    r0[0] = MAT(m, 0, 0), r0[1] = MAT(m, 0, 1),
        r0[2] = MAT(m, 0, 2), r0[3] = MAT(m, 0, 3),
        r0[4] = 1.0, r0[5] = r0[6] = r0[7] = 0.0,
        r1[0] = MAT(m, 1, 0), r1[1] = MAT(m, 1, 1),
        r1[2] = MAT(m, 1, 2), r1[3] = MAT(m, 1, 3),
        r1[5] = 1.0, r1[4] = r1[6] = r1[7] = 0.0,
        r2[0] = MAT(m, 2, 0), r2[1] = MAT(m, 2, 1),
        r2[2] = MAT(m, 2, 2), r2[3] = MAT(m, 2, 3),
        r2[6] = 1.0, r2[4] = r2[5] = r2[7] = 0.0,
        r3[0] = MAT(m, 3, 0), r3[1] = MAT(m, 3, 1),
        r3[2] = MAT(m, 3, 2), r3[3] = MAT(m, 3, 3),
        r3[7] = 1.0, r3[4] = r3[5] = r3[6] = 0.0;
    /* choose pivot - or die */
    if (fabsf(r3[0]) > fabsf(r2[0]))
        SWAP_ROWS_FLOAT(r3, r2);
    if (fabsf(r2[0]) > fabsf(r1[0]))
        SWAP_ROWS_FLOAT(r2, r1);
    if (fabsf(r1[0]) > fabsf(r0[0]))
        SWAP_ROWS_FLOAT(r1, r0);
    if (0.0 == r0[0])
        return 0;
    /* eliminate first variable */
    m1 = r1[0] / r0[0];
    m2 = r2[0] / r0[0];
    m3 = r3[0] / r0[0];
    s = r0[1];
    r1[1] -= m1 * s;
    r2[1] -= m2 * s;
    r3[1] -= m3 * s;
    s = r0[2];
    r1[2] -= m1 * s;
    r2[2] -= m2 * s;
    r3[2] -= m3 * s;
    s = r0[3];
    r1[3] -= m1 * s;
    r2[3] -= m2 * s;
    r3[3] -= m3 * s;
    s = r0[4];
    if (s != 0.0) {
        r1[4] -= m1 * s;
        r2[4] -= m2 * s;
        r3[4] -= m3 * s;
    }
    s = r0[5];
    if (s != 0.0) {
        r1[5] -= m1 * s;
        r2[5] -= m2 * s;
        r3[5] -= m3 * s;
    }
    s = r0[6];
    if (s != 0.0) {
        r1[6] -= m1 * s;
        r2[6] -= m2 * s;
        r3[6] -= m3 * s;
    }
    s = r0[7];
    if (s != 0.0) {
        r1[7] -= m1 * s;
        r2[7] -= m2 * s;
        r3[7] -= m3 * s;
    }
    /* choose pivot - or die */
    if (fabsf(r3[1]) > fabsf(r2[1]))
        SWAP_ROWS_FLOAT(r3, r2);
    if (fabsf(r2[1]) > fabsf(r1[1]))
        SWAP_ROWS_FLOAT(r2, r1);
    if (0.0 == r1[1])
        return 0;
    /* eliminate second variable */
    m2 = r2[1] / r1[1];
    m3 = r3[1] / r1[1];
    r2[2] -= m2 * r1[2];
    r3[2] -= m3 * r1[2];
    r2[3] -= m2 * r1[3];
    r3[3] -= m3 * r1[3];
    s = r1[4];
    if (0.0 != s) {
        r2[4] -= m2 * s;
        r3[4] -= m3 * s;
    }
    s = r1[5];
    if (0.0 != s) {
        r2[5] -= m2 * s;
        r3[5] -= m3 * s;
    }
    s = r1[6];
    if (0.0 != s) {
        r2[6] -= m2 * s;
        r3[6] -= m3 * s;
    }
    s = r1[7];
    if (0.0 != s) {
        r2[7] -= m2 * s;
        r3[7] -= m3 * s;
    }
    /* choose pivot - or die */
    if (fabsf(r3[2]) > fabsf(r2[2]))
        SWAP_ROWS_FLOAT(r3, r2);
    if (0.0f == r2[2])
        return 0;
    /* eliminate third variable */
    m3 = r3[2] / r2[2];
    r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
        r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6], r3[7] -= m3 * r2[7];
    /* last check */
    if (0.0f == r3[3])
        return 0;
    s = 1.0f / r3[3];		/* now back substitute row 3 */
    r3[4] *= s;
    r3[5] *= s;
    r3[6] *= s;
    r3[7] *= s;
    m2 = r2[3];			/* now back substitute row 2 */
    s = 1.0f / r2[2];
    r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
        r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
    m1 = r1[3];
    r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
        r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
    m0 = r0[3];
    r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
        r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;
    m1 = r1[2];			/* now back substitute row 1 */
    s = 1.0f / r1[1];
    r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
        r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
    m0 = r0[2];
    r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
        r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;
    m0 = r0[1];			/* now back substitute row 0 */
    s = 1.0f / r0[0];
    r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
        r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);
    MAT(out, 0, 0) = r0[4];
    MAT(out, 0, 1) = r0[5], MAT(out, 0, 2) = r0[6];
    MAT(out, 0, 3) = r0[7], MAT(out, 1, 0) = r1[4];
    MAT(out, 1, 1) = r1[5], MAT(out, 1, 2) = r1[6];
    MAT(out, 1, 3) = r1[7], MAT(out, 2, 0) = r2[4];
    MAT(out, 2, 1) = r2[5], MAT(out, 2, 2) = r2[6];
    MAT(out, 2, 3) = r2[7], MAT(out, 3, 0) = r3[4];
    MAT(out, 3, 1) = r3[5], MAT(out, 3, 2) = r3[6];
    MAT(out, 3, 3) = r3[7];
    return 1;
}

//
// Pour bien fermer tout ça
//
void dkglShutDown()
{
}


//
// Pour projeter la mouse ou un point 2D quelconque
//
CVector3f dkglUnProject(CVector2i & pos2D, float zRange)
{
	float v[3];
#ifndef _DX_
	CVector3f pos((float)pos2D[0], (float)pos2D[1], zRange);
	GLfloat modelMatrix[16];
    GLfloat projMatrix[16];
	GLint    viewport[4];

	glGetFloatv(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetFloatv(GL_PROJECTION_MATRIX, projMatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);

    glhUnProjectf(
		pos[0], 
		pos[1], 
		pos[2], 
		modelMatrix, 
		projMatrix, 
		viewport, 
        v);
#endif

	return CVector3f(v[0], v[1], v[2]);
}


CVector3f dkglProject(CVector3f & pos3D)
{
    float v[3];
#ifndef _DX_
    GLfloat modelMatrix[16];
    GLfloat projMatrix[16];
	GLint    viewport[4];

    glGetFloatv(GL_MODELVIEW_MATRIX, modelMatrix);
    glGetFloatv(GL_PROJECTION_MATRIX, projMatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);

    glhProjectf(
		pos3D[0], 
		pos3D[1], 
		pos3D[2], 
		modelMatrix, 
		projMatrix, 
		viewport, 
        v);
#endif
    return CVector3f(v[0], v[1], v[2]);
}

static void lookAt(
    float eyex, float eyey, float eyez,
    float orgx, float orgy, float orgz,
    float  upx, float  upy, float  upz)
{
    //Calculate eye direction vector
    float vpnx = orgx - eyex;
    float vpny = orgy - eyey;
    float vpnz = orgz - eyez;

    //Normalize it
    float len = sqrt(vpnx * vpnx + vpny * vpny + vpnz * vpnz);
    vpnx /= len;
    vpny /= len;
    vpnz /= len;

    //Calculate right vector
    float rvx = vpny * upz - vpnz * upy;
    float rvy = vpnz * upx - vpnx * upz;
    float rvz = vpnx * upy - vpny * upx;

    //Calculate new up vector
    float nux = rvy * vpnz - rvz * vpny;
    float nuy = rvz * vpnx - rvx * vpnz;
    float nuz = rvx * vpny - rvy * vpnx;

    //Put it all in a pretty Matrix
    float mat[16] = {
        rvx, nux, -vpnx, 0,
        rvy, nuy, -vpny, 0,
        rvz, nuz, -vpnz, 0,
        0, 0, 0, 1
    };

    //Apply the matrix and translate to eyepoint
    glMultMatrixf(mat);
    glTranslatef(-eyex, -eyey, -eyez);
}

void gluLookAt(
    GLdouble eyex,
    GLdouble eyey,
    GLdouble eyez,
    GLdouble centerx,
    GLdouble centery,
    GLdouble centerz,
    GLdouble upx,
    GLdouble upy,
    GLdouble upz)
{
    lookAt((float)eyex, (float)eyey, (float)eyez, (float)centerx, (float)centery, (float)centerz, (float)upx, (float)upy, (float)upz);
}

#define PI2 6.283185307179586476925286766559f

// Very expensive call
void drawSphere(GLdouble radius, GLint slices, GLint stacks, GLenum topology)
{
    glBegin(topology);
    for (int j = 0; j < stacks; ++j)
    {
        float angleX0 = -(float)j / (float)stacks * PI + PI / 2;
        float angleX1 = -(float)(j + 1) / (float)stacks * PI + PI / 2;
        for (int i = 0; i < slices; ++i)
        {
            float angleZ0 = (float)i / (float)slices * PI2;
            float angleZ1 = (float)(i + 1) / (float)slices * PI2;

            glTexCoord2f((float)(i) / (float)slices, (float)(j) / (float)stacks);
            glNormal3f(
                std::cosf(angleZ0) * std::cosf(angleX0),
                std::sinf(angleZ0) * std::cosf(angleX0),
                std::sinf(angleX0)
            );
            glVertex3f(
                std::cosf(angleZ0) * std::cosf(angleX0) * (float)radius,
                std::sinf(angleZ0) * std::cosf(angleX0) * (float)radius,
                std::sinf(angleX0) * (float)radius);

            glTexCoord2f((float)(i) / (float)slices, (float)(j + 1) / (float)stacks);
            glNormal3f(
                std::cosf(angleZ0) * std::cosf(angleX1),
                std::sinf(angleZ0) * std::cosf(angleX1),
                std::sinf(angleX1)
            );
            glVertex3f(
                std::cosf(angleZ0) * std::cosf(angleX1) * (float)radius,
                std::sinf(angleZ0) * std::cosf(angleX1) * (float)radius,
                std::sinf(angleX1) * (float)radius);

            glTexCoord2f((float)(i + 1) / (float)slices, (float)(j + 1) / (float)stacks);
            glNormal3f(
                std::cosf(angleZ1) * std::cosf(angleX1),
                std::sinf(angleZ1) * std::cosf(angleX1),
                std::sinf(angleX1)
            );
            glVertex3f(
                std::cosf(angleZ1) * std::cosf(angleX1) * (float)radius,
                std::sinf(angleZ1) * std::cosf(angleX1) * (float)radius,
                std::sinf(angleX1) * (float)radius);

            glTexCoord2f((float)(i) / (float)slices, (float)(j) / (float)stacks);
            glNormal3f(
                std::cosf(angleZ0) * std::cosf(angleX0),
                std::sinf(angleZ0) * std::cosf(angleX0),
                std::sinf(angleX0)
            );
            glVertex3f(
                std::cosf(angleZ0) * std::cosf(angleX0) * (float)radius,
                std::sinf(angleZ0) * std::cosf(angleX0) * (float)radius,
                std::sinf(angleX0) * (float)radius);

            glTexCoord2f((float)(i + 1) / (float)slices, (float)(j + 1) / (float)stacks);
            glNormal3f(
                std::cosf(angleZ1) * std::cosf(angleX1),
                std::sinf(angleZ1) * std::cosf(angleX1),
                std::sinf(angleX1)
            );
            glVertex3f(
                std::cosf(angleZ1) * std::cosf(angleX1) * (float)radius,
                std::sinf(angleZ1) * std::cosf(angleX1) * (float)radius,
                std::sinf(angleX1) * (float)radius);

            glTexCoord2f((float)(i + 1) / (float)slices, (float)(j) / (float)stacks);
            glNormal3f(
                std::cosf(angleZ1) * std::cosf(angleX0),
                std::sinf(angleZ1) * std::cosf(angleX0),
                std::sinf(angleX0)
            );
            glVertex3f(
                std::cosf(angleZ1) * std::cosf(angleX0) * (float)radius,
                std::sinf(angleZ1) * std::cosf(angleX0) * (float)radius,
                std::sinf(angleX0) * (float)radius);
        }
    }
    glEnd();
}
