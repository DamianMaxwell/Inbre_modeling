/* 
A* -------------------------------------------------------------------
B* This file contains source code for the PyMOL computer program
C* copyright 1998-2000 by Warren Lyford Delano of DeLano Scientific. 
D* -------------------------------------------------------------------
E* It is unlawful to modify or remove this copyright notice.
F* -------------------------------------------------------------------
G* Please see the accompanying LICENSE file for further information. 
H* -------------------------------------------------------------------
I* Additional authors of this source file include:
-* 
-* 
-*
Z* -------------------------------------------------------------------
*/
#ifndef _H_Object
#define _H_Object

/* literally a 3-D object...also an object object */

#include"Base.h"
#include"Ray.h"
#include"Rep.h"
#include"Setting.h"

#define ObjNameMax        255
#define cObjectMolecule     1
#define cObjectMap          2
#define cObjectMesh         3
#define cObjectDist         4
#define cObjectCallback     5
#define cObjectCGO          6

/* 
   the object base class is in the process of being converted to support
   states explicitly (an unfortunate early omission), which will allow
   for simplified implementation of future multi-state objects.
 */

typedef struct Object {
  void (*fUpdate)(struct Object *I); /* update representations */
  void (*fRender)(struct Object *I,int frame,CRay *ray,Pickable **pick,int pass);
  void (*fFree)(struct Object *I);
  int  (*fGetNFrame)(struct Object *I);
  void (*fDescribeElement)(struct Object *I,int index,char *buffer);
  void (*fInvalidate)(struct Object *I,int rep,int level,int state);
  CSetting **(*fGetSettingHandle)(struct Object *I,int state);
  int type;
  char Name[ObjNameMax];
  int Color;
  int RepVis[cRepCnt]; /* currently used only by non atomic objects */
  float ExtentMin[3],ExtentMax[3];
  int ExtentFlag,TTTFlag;
  float TTT[16]; /* translate, transform, translate matrix */
  CSetting *Setting;
} Object;

void ObjectInit(Object *I);
void ObjectPurge(Object *I);
void ObjectSetName(Object *I,char *name);
void ObjectFree(Object *I);
void ObjectUseColor(Object *I);
void ObjectSetRepVis(Object *I,int rep,int state);
void ObjectPrepareContext(Object *I,CRay *ray);
void ObjectCombineTTT(Object *I,float *ttt);
void ObjectSetTTTOrigin(Object *I,float *origin);
void ObjectResetTTT(Object *I);

#endif



