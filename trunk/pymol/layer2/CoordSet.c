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
#include<stdlib.h>
#include<math.h>
#include<stdio.h>

#include"Base.h"
#include"OOMac.h"
#include"MemoryDebug.h"
#include"Err.h"
#include"Scene.h"
#include"CoordSet.h"
#include"Color.h"

#include"RepWireBond.h"
#include"RepCylBond.h"
#include"RepDot.h"
#include"RepMesh.h"
#include"RepSphere.h"
#include"RepRibbon.h"

void CoordSetUpdate(CoordSet *I);

void CoordSetFree(CoordSet *I);
void CoordSetRender(CoordSet *I,CRay *ray,Pickable **pick);
void CoordSetEnumIndices(CoordSet *I);
void CoordSetStrip(CoordSet *I);
void CoordSetInvalidateRep(CoordSet *I,int type,int level);
void CoordSetExtendIndices(CoordSet *I,int nAtom);
void CoordSetAppendIndices(CoordSet *I,int offset);

/*========================================================================*/
void CoordSetInvalidateRep(CoordSet *I,int type,int level)
{
  int a;

  if(level>=cRepInvColor)
	 VLAFreeP(I->Color);

  if(type>=0) {
	 if(type<I->NRep)	{
		SceneChanged();
		
		if(I->Rep[type]) {
		  I->Rep[type]->fFree(I->Rep[type]);
		  I->Rep[type] = NULL;
		}
	 }
  } else {
	 for(a=0;a<I->NRep;a++)	{
		SceneChanged();
		
		if(I->Rep[a]) {
		  I->Rep[a]->fFree(I->Rep[a]);
		  I->Rep[a] = NULL;
		}
	 }
  }
}
/*========================================================================*/
void CoordSetUpdate(CoordSet *I)
{
  int a;
  int i;
  if(!I->Color) /* colors invalidated */
	 {
		I->Color=VLAlloc(int,I->NIndex);
		if(I->Color) {
		  for(a=0;a<I->Obj->NAtom;a++)
			 {
				i=I->AtmToIdx[a];
				if(i>=0) 
				  I->Color[i]=I->Obj->AtomInfo[a].color;
			 }
		}
	 }
  for(a=0;a<I->NRep;a++)
	 {
		OrthoBusyFast(a,I->NRep);
	 	if(!I->Rep[cRepLine]) {
		  I->Rep[cRepLine]=RepWireBondNew(I);
		  SceneDirty();
		}
		if(!I->Rep[cRepCyl]) {
		  I->Rep[cRepCyl]=RepCylBondNew(I);
		  SceneDirty();
		  }
		if(!I->Rep[cRepDot]) {
		  I->Rep[cRepDot]=RepDotNew(I,0);
		  SceneDirty();
		}
		if(!I->Rep[cRepMesh]) {
		  I->Rep[cRepMesh]=RepMeshNew(I);
		  SceneDirty();
		}
	 	if(!I->Rep[cRepSphere]) {
		  I->Rep[cRepSphere]=RepSphereNew(I);
		  SceneDirty();
		  }
	 	if(!I->Rep[cRepRibbon]) {
		  I->Rep[cRepRibbon]=RepRibbonNew(I);
		  SceneDirty();
		  }
	 }
  OrthoBusyFast(1,1);
}
/*========================================================================*/
void CoordSetRender(CoordSet *I,CRay *ray,Pickable **pick)
{
  int a;
  for(a=0;a<I->NRep;a++)
	 if(I->Rep[a]) 
		{
		  if(!ray) {
			 SceneResetNormal();
			 ObjectUseColor((Object*)I->Obj);
		  } else {
			 ray->fColor3fv(ray,ColorGet(I->Obj->Obj.Color));
		  }
			 
		  I->Rep[a]->fRender(I->Rep[a],ray,pick);
		}

}
/*========================================================================*/
CoordSet *CoordSetNew(void)
{
  int a;
  OOAlloc(CoordSet);

  I->fFree=CoordSetFree;
  I->fRender=CoordSetRender;
  I->fUpdate=CoordSetUpdate;
  I->fEnumIndices=CoordSetEnumIndices;
  I->fExtendIndices=CoordSetExtendIndices;
  I->fAppendIndices=CoordSetAppendIndices;
  I->fInvalidateRep=CoordSetInvalidateRep;
  I->NIndex=0;
  I->Coord = NULL;
  I->Color = NULL;
  I->AtmToIdx = NULL;
  I->IdxToAtm = NULL;
  I->TmpBond = NULL;
  I->Rep=VLAlloc(Rep*,10);
  I->NRep=cRepCnt;

  for(a=0;a<I->NRep;a++)
	 I->Rep[a] = NULL;
  return(I);
}
/*========================================================================*/
void CoordSetExtendIndices(CoordSet *I,int nAtom)
{
  int a;
  if(I->NAtIndex<nAtom)
	 {
		if(I->AtmToIdx) {
		  I->AtmToIdx = Realloc(I->AtmToIdx,int,nAtom);
		  ErrChkPtr(I->AtmToIdx);
		  for(a=I->NAtIndex;a<nAtom;a++)
			 I->AtmToIdx[a]=-1;
		  I->NAtIndex = nAtom;
		} else {
		  I->AtmToIdx = Alloc(int,nAtom);
		  for(a=0;a<nAtom;a++)
			 I->AtmToIdx[a]=-1;
		  I->NAtIndex = nAtom;
		}
	 }
}
/*========================================================================*/
void CoordSetAppendIndices(CoordSet *I,int offset) 
	  /* this is going to get impractical down the road...
		  we need to revise the index system */
{
  int a;
  I->AtmToIdx = Alloc(int,I->NIndex+offset);
  I->IdxToAtm = Alloc(int,I->NIndex);
  ErrChkPtr(I->AtmToIdx);
  ErrChkPtr(I->IdxToAtm);
  for(a=0;a<offset;a++)
	 I->AtmToIdx[a]=-1;
  for(a=0;a<I->NIndex;a++) {
	 I->AtmToIdx[a+offset]=a;
	 I->IdxToAtm[a]=a+offset;
  }
  I->NAtIndex = I->NIndex + offset;
}
/*========================================================================*/
void CoordSetEnumIndices(CoordSet *I)
{
  /* set up for simple case where 1 = 1, etc. */
  int a;
  I->AtmToIdx = Alloc(int,I->NIndex);
  I->IdxToAtm = Alloc(int,I->NIndex);
  ErrChkPtr(I->AtmToIdx);
  ErrChkPtr(I->IdxToAtm);
  for(a=0;a<I->NIndex;a++)
	 {
	 I->AtmToIdx[a]=a;
	 I->IdxToAtm[a]=a;
	 }
  I->NAtIndex = I->NIndex;
}
/*========================================================================*/
void CoordSetStrip(CoordSet *I)
{
  int a;
  for(a=0;a<I->NRep;a++)
	 if(I->Rep[a])
		I->Rep[a]->fFree(I->Rep[a]);
  I->NRep=0;
}
/*========================================================================*/
void CoordSetFree(CoordSet *I)
{
  int a;
  for(a=0;a<I->NRep;a++)
	 if(I->Rep[a])
		I->Rep[a]->fFree(I->Rep[a]);
  if(I) 
	 {
	 OOFreeP(I->AtmToIdx);
	 OOFreeP(I->IdxToAtm);
	 VLAFreeP(I->Color);
	 VLAFreeP(I->Coord);
	 VLAFreeP(I->Rep);
	 VLAFreeP(I->TmpBond);
	 OOFreeP(I);
	 }
}


