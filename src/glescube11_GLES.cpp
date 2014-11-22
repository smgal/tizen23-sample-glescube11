/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "glescube11.h"
#include <math.h>
#include <Elementary_GL_Helpers.h>

// #define USE_GLFIXED

extern const unsigned short IMAGE_565_128_128_1[];
extern const unsigned short IMAGE_4444_128_128_1[];

namespace // unnamed
{
#if defined(USE_GLFIXED)

const int MAX_INT = 65536.0f;

typedef int GlUnit;

GlUnit GetGlUnit(float vaule)
{
	return GlUnit(vaule * MAX_INT);
}

GlUnit GetGlUnit(int vaule)
{
	return GlUnit(vaule << 16);
}

#define glClearColorEx glClearColorx
#define glFrustum     glFrustumx
#define glFog        glFogx
#define glTranslate   glTranslatex
#define glRotate      glRotatex
#define GL_TFIXED      GL_FIXED

#else

typedef float GlUnit;

GlUnit GetGlUnit(float vaule)
{
	return vaule;
}

GlUnit GetGlUnit(int val)
{
	return float(val);
}

#define glClearColorEx glClearColor
#define glFrustum      glFrustumf
#define glFog          glFogf
#define glTranslate    glTranslatef
#define glRotate       glRotatef
#define GL_TFIXED      GL_FLOAT

#endif

const GlUnit ONEP = GetGlUnit(+1.0f);
const GlUnit ONEN = GetGlUnit(-1.0f);
const GlUnit ZERO = GetGlUnit( 0.0f);

void SetPerspective(Evas_Object *obj, GLfloat fovDegree, GLfloat aspect, GLfloat zNear,  GLfloat zFar)
{
	ELEMENTARY_GLVIEW_USE(obj);

	// tan(double(degree) * 3.1415962 / 180.0 / 2.0);
	static const float HALF_TAN_TABLE[91] =
	{
		0.00000f, 0.00873f, 0.01746f, 0.02619f, 0.03492f, 0.04366f, 0.05241f, 0.06116f, 0.06993f,
		0.07870f, 0.08749f, 0.09629f, 0.10510f, 0.11394f, 0.12278f, 0.13165f, 0.14054f, 0.14945f,
		0.15838f, 0.16734f, 0.17633f, 0.18534f, 0.19438f, 0.20345f, 0.21256f, 0.22169f, 0.23087f,
		0.24008f, 0.24933f, 0.25862f, 0.26795f, 0.27732f, 0.28675f, 0.29621f, 0.30573f, 0.31530f,
		0.32492f, 0.33460f, 0.34433f, 0.35412f, 0.36397f, 0.37389f, 0.38386f, 0.39391f, 0.40403f,
		0.41421f, 0.42448f, 0.43481f, 0.44523f, 0.45573f, 0.46631f, 0.47698f, 0.48773f, 0.49858f,
		0.50953f, 0.52057f, 0.53171f, 0.54296f, 0.55431f, 0.56577f, 0.57735f, 0.58905f, 0.60086f,
		0.61280f, 0.62487f, 0.63707f, 0.64941f, 0.66189f, 0.67451f, 0.68728f, 0.70021f, 0.71329f,
		0.72654f, 0.73996f, 0.75356f, 0.76733f, 0.78129f, 0.79544f, 0.80979f, 0.82434f, 0.83910f,
		0.85408f, 0.86929f, 0.88473f, 0.90041f, 0.91633f, 0.93252f, 0.94897f, 0.96569f, 0.98270f,
		1.00000f
	};

	int degree = static_cast<int>(fovDegree + 0.5f);

	degree = (degree >=  0) ? degree :  0;
	degree = (degree <= 90) ? degree : 90;

	GlUnit fxdYMax = GetGlUnit(zNear * HALF_TAN_TABLE[degree]);
	GlUnit fxdYMin = -fxdYMax;

#if defined(USE_GLFIXED)
	GlUnit fxdXMax = GetGlUnit(GLfloat(fxdYMax) * aspect / MAX_INT);
#else
	GlUnit fxdXMax = GetGlUnit(GLfloat(fxdYMax) * aspect);
#endif
	GlUnit fxdXMin = -fxdXMax;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glFrustum(fxdXMin, fxdXMax, fxdYMin, fxdYMax, GetGlUnit(zNear), GetGlUnit(zFar));
}

} // namespace

void
init_gles(Evas_Object *obj)
{
	ELEMENTARY_GLVIEW_USE(obj);
	appdata_s *ad = (appdata_s *)evas_object_data_get(obj, APPDATA_KEY);

	int width;
	int height;

	elm_glview_size_get(obj, &width, &height);

	ad->current_tex_index = 0;

	{
		glGenTextures(2, ad->tex_ids);

		glBindTexture(GL_TEXTURE_2D, ad->tex_ids[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, IMAGE_4444_128_128_1);
		glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, ad->tex_ids[1]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, IMAGE_565_128_128_1);
		glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	glShadeModel(GL_SMOOTH);

	glViewport(0, 0, width, height);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	{
		glEnable(GL_FOG);
		glFogx(GL_FOG_MODE,     GL_LINEAR);
		glFog(GL_FOG_DENSITY, GetGlUnit(0.25f));
		glFog(GL_FOG_START,   GetGlUnit(4.0f));
		glFog(GL_FOG_END,     GetGlUnit(6.5f));
		glHint(GL_FOG_HINT,     GL_DONT_CARE);
	}

	SetPerspective(obj, 60.0f, 1.0f * width / height, 1.0f, 400.0f);

	glClearColorEx(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void
destroy_gles(Evas_Object *obj)
{
   appdata_s *ad;

   ELEMENTARY_GLVIEW_USE(obj);
   ad = (appdata_s *)evas_object_data_get(obj, APPDATA_KEY);

   if (ad->tex_ids[0])
   {
      glDeleteTextures(1, &(ad->tex_ids[0]));
      ad->tex_ids[0] = 0;
   }

   if (ad->tex_ids[1])
   {
      glDeleteTextures(1, &(ad->tex_ids[1]));
      ad->tex_ids[1] = 0;
   }
}

void
resize_gl(Evas_Object *obj)
{
	ELEMENTARY_GLVIEW_USE(obj);

	int width;
	int height;

	elm_glview_size_get(obj, &width, &height);

	SetPerspective(obj, 60.0f, 1.0f * width / height, 1.0f, 400.0f);
}

static void
GlesCube11_DrawCube1(Evas_Object *obj)
{
	ELEMENTARY_GLVIEW_USE(obj);
	appdata_s *ad = (appdata_s *)evas_object_data_get(obj, APPDATA_KEY);

	static const GlUnit VERTICES[] =
	{
		ONEN, ONEP, ONEN, // 0
		ONEP, ONEP, ONEN, // 1
		ONEN, ONEN, ONEN, // 2
		ONEP, ONEN, ONEN, // 3
		ONEN, ONEP, ONEP, // 4
		ONEP, ONEP, ONEP, // 5
		ONEN, ONEN, ONEP, // 6
		ONEP, ONEN, ONEP  // 7
	};

	static const GlUnit VERTEX_COLOR[] =
	{
		ONEP, ZERO, ONEP, ONEP,
		ONEP, ONEP, ZERO, ONEP,
		ZERO, ONEP, ONEP, ONEP,
		ONEP, ZERO, ZERO, ONEP,
		ZERO, ZERO, ONEP, ONEP,
		ZERO, ONEP, ZERO, ONEP,
		ONEP, ONEP, ONEP, ONEP,
		ZERO, ZERO, ZERO, ONEP
	};

	static const unsigned short INDEX_BUFFER[] =
	{
		0, 1, 2, 2, 1, 3,
		1, 5, 3, 3, 5, 7,
		5, 4, 7, 7, 4, 6,
		4, 0, 6, 6, 0, 2,
		4, 5, 0, 0, 5, 1,
		2, 3, 6, 6, 3, 7
	};

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_TFIXED, 0, VERTICES);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_TFIXED, 0, VERTEX_COLOR);

	glMatrixMode(GL_MODELVIEW);
	{
		glLoadIdentity();
		glTranslate(0, GetGlUnit(-0.7f), GetGlUnit(-5.0f));

		{
			static int angle = 0;
			angle = (angle + 1) % (360 * 3);
			glRotate(GetGlUnit(angle) / 3, GetGlUnit(1.0f), 0, 0);
			glRotate(GetGlUnit(angle), 0, 0, GetGlUnit(1.0f));
		}
	}

	glDrawElements(GL_TRIANGLES, 6 * (3 * 2), GL_UNSIGNED_SHORT, &INDEX_BUFFER[0]);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

static void
GlesCube11_DrawCube2(Evas_Object *obj)
{
	ELEMENTARY_GLVIEW_USE(obj);
	appdata_s *ad = (appdata_s *)evas_object_data_get(obj, APPDATA_KEY);

	static const GlUnit VERTICES[] =
	{
		ONEN, ONEN, ONEP, ONEP, ONEN, ONEP, ONEN, ONEP, ONEP, ONEP, ONEP, ONEP,
		ONEN, ONEN, ONEN, ONEN, ONEP, ONEN, ONEP, ONEN, ONEN, ONEP, ONEP, ONEN,
		ONEN, ONEN, ONEP, ONEN, ONEP, ONEP, ONEN, ONEN, ONEN, ONEN, ONEP, ONEN,
		ONEP, ONEN, ONEN, ONEP, ONEP, ONEN, ONEP, ONEN, ONEP, ONEP, ONEP, ONEP,
		ONEN, ONEP, ONEP, ONEP, ONEP, ONEP, ONEN, ONEP, ONEN, ONEP, ONEP, ONEN,
		ONEN, ONEN, ONEP, ONEN, ONEN, ONEN, ONEP, ONEN, ONEP, ONEP, ONEN, ONEN
	};

	static const GlUnit TEXTURE_COORD[] =
	{
		ONEP, ZERO, ZERO, ZERO, ONEP, ONEP, ZERO, ONEP,
		ONEP, ZERO, ZERO, ZERO, ONEP, ONEP, ZERO, ONEP,
		ONEP, ZERO, ZERO, ZERO, ONEP, ONEP, ZERO, ONEP,
		ONEP, ZERO, ZERO, ZERO, ONEP, ONEP, ZERO, ONEP,
		ONEP, ZERO, ZERO, ZERO, ONEP, ONEP, ZERO, ONEP,
		ONEP, ZERO, ZERO, ZERO, ONEP, ONEP, ZERO, ONEP
	};

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_TFIXED, 0, VERTICES);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_TFIXED, 0, TEXTURE_COORD);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ad->tex_ids[ad->current_tex_index]);

	glMatrixMode(GL_MODELVIEW);
	{
		const  float Z_POS_INC = 0.01f;
		static float zPos      = -5.0f;
		static float zPosInc   = Z_POS_INC;

		zPos += zPosInc;

		if (zPos < -8.0f)
		{
			zPosInc = Z_POS_INC;
			ad->current_tex_index = 1 - ad->current_tex_index;
		}

		if (zPos > -5.0f)
		{
			zPosInc = -Z_POS_INC;
		}

		glLoadIdentity();
		glTranslate(0, GetGlUnit(1.2f), GetGlUnit(zPos));

		{
			static int angle = 0;
			angle = (angle + 1) % (360 * 3);
			glRotate(GetGlUnit(angle) / 3, 0, 0, GetGlUnit(1.0f));
			glRotate(GetGlUnit(angle), 0, GetGlUnit(1.0f), 0);
		}
	}

	for(int i = 0; i < 6; i++)
	{
		glDrawArrays(GL_TRIANGLE_STRIP, 4 * i, 4);
	}

	glDisable(GL_TEXTURE_2D);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void
draw_gl(Evas_Object *obj)
{
	ELEMENTARY_GLVIEW_USE(obj);

	struct
	{
		GLint x, y, width, height;
	} viewPort;

	glGetIntegerv(GL_VIEWPORT, (GLint*)&viewPort);

	{
		const  double PI  = 3.141592;
		static double hue = 0.0;

		float r = (1.0f + static_cast<float>(sin(hue - 2.0 * PI / 3.0))) / 3.0f;
		float g = (1.0f + static_cast<float>(sin(hue)                 )) / 3.0f;
		float b = (1.0f + static_cast<float>(sin(hue + 2.0 * PI / 3.0))) / 3.0f;

		GLfloat fogColor[4] =
		{
			r, g, b, 1.0f
		};

		glFogfv(GL_FOG_COLOR, fogColor);

		glClearColorEx(GetGlUnit(r), GetGlUnit(g), GetGlUnit(b), GetGlUnit(1.0f));

		hue += 0.03;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GlesCube11_DrawCube1(obj);
	GlesCube11_DrawCube2(obj);
}
