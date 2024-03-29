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

#ifndef __glescube11_H__
#define __glescube11_H__

#include <Evas_GL.h>
#include <Elementary.h>
#include <efl_extension.h>
#include <dlog.h>

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "glescube11"

#define APPDATA_KEY "AppData"

struct _appdata_s
{
   Evas_Object *table, *bg;
   Evas_Object *win;
   Evas_Object *glview;
   Ecore_Animator *anim;
   Evas_Object *conform;

   GLuint tex_ids[2];
   int current_tex_index;
};
typedef struct _appdata_s appdata_s;

#if defined(__cplusplus)
extern "C" {
#endif
void init_gles(Evas_Object *obj);
void draw_gl(Evas_Object *obj);
void resize_gl(Evas_Object *obj);
void destroy_gles(Evas_Object *obj);
#if defined(__cplusplus)
}
#endif

#endif // __glescube11_H__
