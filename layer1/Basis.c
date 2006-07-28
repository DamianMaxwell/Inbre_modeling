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
-*   Larry Coopet (various optimizations)
-* 
-*
Z* -------------------------------------------------------------------
*/

#ifndef _PYMOL_INLINE

#include"os_predef.h"
#include"os_std.h"

#include"MemoryDebug.h"
#include"Base.h"
#include"Basis.h"
#include"Err.h"
#include"Feedback.h"
#include"Util.h"
#include"MemoryCache.h"
#include"Character.h"

static const float kR_SMALL4 = 0.0001F;
#define EPSILON 0.000001

/*========================================================================*/
#ifdef _PYMOL_INLINE
__inline__
#endif
static int ZLineToSphere(float *base, float *point,float *dir,float radius,float maxial,
                  float *sphere,float *asum,float *pre)
{
   /* Strategy - find an imaginary sphere that lies at the correct point on
     the line segment, then treat as a sphere reflection */
   
   float  intra_p0,intra_p1,intra_p2;
   float radial,axial,axial_sum,dangle,ab_dangle,axial_perp;
   float radialsq,tan_acos_dangle;
   float vradial0,vradial1,vradial2;
   const float _0   = 0.0f;
   float point0 = point[0];
   float point1 = point[1];
   float point2 = point[2];
   register float perpAxis0 = pre[0]; /* was cross_product(MinusZ,dir,perpAxis),normalize */
   register float perpAxis1 = pre[1];
   float intra0 = point0 - base[0];
   float intra1 = point1 - base[1];
   const float dir0   = dir[0];
   const float dir1   = dir[1];
   const float dir2   = dir[2];
   register float   dot, perpDist;
   
   /* the perpAxis defines a perp-plane which includes the cyl-axis */
   
   /* get minimum distance between the lines */
   
   perpDist = intra0*perpAxis0 + intra1*perpAxis1;
   /* was dot_product3f(intra,perpAxis); */

   if((perpDist < _0 ? -perpDist : perpDist) > radius)
      return 0;
      
   /*if(fabs(perpDist) > radius)   return 0; */
   
   dangle   = -dir2; /* was dot(MinusZ,dir) */
   ab_dangle   = (float)fabs(dangle);
   if(ab_dangle > (1.0f - kR_SMALL4))
   {
      if(dangle > _0)
      {
         sphere[0] = point0;
         sphere[1] = point1;
         sphere[2] = point2;
      }
      else
      {
         sphere[0]=dir0 * maxial + point0;
         sphere[1]=dir1 * maxial + point1;
         sphere[2]=dir2 * maxial + point2;
      }
      return(1);
   }

   if(ab_dangle > kR_SMALL4)
      tan_acos_dangle = (float)(sqrt1d(1.0-dangle*dangle) / dangle);
   else
      tan_acos_dangle = MAXFLOAT;
         
   /* now we need to define the triangle in the perp-plane  
     to figure out where the projected line intersection point is */

   intra_p2=point2-base[2];
   
   /* first, compute radial distance in the perp-plane between the two starting points */
   
   dot = intra0*perpAxis0 + intra1*perpAxis1;
   intra_p0=intra0-perpAxis0*dot;
   intra_p1=intra1-perpAxis1*dot;
      
   dot = intra_p0*dir0 + intra_p1*dir1 + intra_p2*dir2;
   vradial0=intra_p0-dir0*dot;
   vradial1=intra_p1-dir1*dot;
   vradial2=intra_p2-dir2*dot;  
   
   radialsq   = ((vradial0*vradial0) + (vradial1*vradial1) + (vradial2*vradial2));
   
   /* now figure out the axial distance along the cyl-line that will give us
     the point of closest approach */

   if(ab_dangle < kR_SMALL4)
      axial_perp   = _0;
   else
      axial_perp   = (float)(sqrt1f(radialsq)/tan_acos_dangle);
  
   axial = (float) sqrt1f( ( (intra_p0*intra_p0) + (intra_p1*intra_p1) + (intra_p2*intra_p2)) - radialsq );
   
   if( (intra_p0*dir0 + intra_p1*dir1 + intra_p2*dir2 ) >= _0 )
      axial = axial_perp - axial;
   else
      axial = axial_perp + axial;
   
   /* now we have to think about where the vector will actually strike the cylinder*/
   
   /* by definition, it must be perpdist away from the perp-plane becuase the perp-plane
     is parallel to the line, so we can compute the radial component to this point */
   
   radial = radius*radius - perpDist*perpDist;
   radial = (float)sqrt1f(radial);
   
   /* now the trick is figuring out how to adjust the axial distance to get the actual
     position along the cyl line which will give us a representative sphere */
   
   if(ab_dangle > kR_SMALL4)
      axial_sum = axial - radial/tan_acos_dangle;
   else
      axial_sum = axial;
   /*
    printf("radial2 %8.3f \n",radial);*/

   if(axial_sum < _0)
      axial_sum = _0;
   else if(axial_sum > maxial)
      axial_sum = maxial;
   
   sphere[0] = dir0 * axial_sum + point0;
   sphere[1] = dir1 * axial_sum + point1;
   sphere[2] = dir2 * axial_sum + point2;
   
   *asum = axial_sum;
   /*  printf("==>%8.3f sphere %8.3f %8.3f %8.3f\n",base[1],sphere[1],axial_perp,axial);*/
   return(1);
}


#ifdef _PYMOL_INLINE
__inline__
#endif
static int LineToSphere(float *base, float *ray, float *point,float *dir,float radius,float maxial,
                  float *sphere,float *asum)
{
   /* Strategy - find an imaginary sphere that lies at the correct point on
     the line segment, then treat as a sphere reflection */

  register float perpDist,radial,axial,axial_sum,dangle,ab_dangle,axial_perp;
  register float radialsq,tan_acos_dangle;
  register float perpAxis0, perpAxis1, perpAxis2;
  register float intra0, intra1, intra2;
  register float intra_p0, intra_p1, intra_p2;
  register float vradial0, vradial1, vradial2;
  register float dir0 = dir[0], dir1 = dir[1], dir2 = dir[2];
  register float ray0 = ray[0], ray1 = ray[1], ray2 = ray[2];
  float point0 = point[0], point1 = point[1], point2 = point[2];
  register float dot;
  
  const float _0  = 0.0F;
  const float _1  = 1.0F;

   /*    cross_product3f(ray,dir,perpAxis); */

   perpAxis0 = ray1*dir2 - ray2*dir1;
   perpAxis1 = ray2*dir0 - ray0*dir2;
   perpAxis2 = ray0*dir1 - ray1*dir0;
  
   /* subtract3f(point,base,intra); */
   intra0 = point0 - base[0];

   /* normalize3f(perpAxis) */
   {
     register float len = (float)sqrt1d((perpAxis0 * perpAxis0) + (perpAxis1 * perpAxis1) + (perpAxis2 * perpAxis2));
	  intra1 = point1 - base[1];
    /* subtract3f(point,base,intra); */
     if(len>R_SMALL8) {
       len = _1 / len;
	   intra2 = point2 - base[2];
       perpAxis0 *= len;
       perpAxis1 *= len;
       perpAxis2 *= len;
     } else {
	   intra2 = point2 - base[2];
	 }
   }
   /* the perpAxis defines a perp-plane which includes the cyl-axis */
   
   /* get minimum distance between the lines */

   /* dot_product3f(intra, perpAxis); */
   perpDist =  intra0 * perpAxis0;
   perpDist += intra1 * perpAxis1 + intra2 * perpAxis2 ;

   /*if(fabs(perpDist) > radius)   return 0; */

   dangle = ray0 * dir0;
   
   if((perpDist < _0 ? -perpDist : perpDist) > radius)
      return 0;
   
   /* dangle  = dot_product3f(ray, dir);*/
   dangle  += ray1 * dir1 + ray2 * dir2;

   ab_dangle   = (float)fabs(dangle);

   if(ab_dangle > (1.0f - kR_SMALL4))
   {
      if(dangle > _0)
      {
         sphere[0] = point0;
         sphere[1] = point1;
         sphere[2] = point2;
      }
      else
      {
         sphere[0]=dir0 * maxial + point0;
         sphere[1]=dir1 * maxial + point1;
         sphere[2]=dir2 * maxial + point2;
      }
      return(1);
   }

   if(ab_dangle > kR_SMALL4)
      tan_acos_dangle = (float)(sqrt1d(1.0-dangle*dangle) / dangle);
   else
      tan_acos_dangle = MAXFLOAT;
         
   /* now we need to define the triangle in the perp-plane  
     to figure out where the projected line intersection point is */
   
   /* first, compute radial distance in the perp-plane between the two starting points */
   
   /* dot = dot_product3f(intra,perpAxis); */
   dot = intra0 * perpAxis0 + intra1 * perpAxis1 + intra2 * perpAxis2 ;
   
   intra_p0 = intra0-perpAxis0*dot;
   intra_p1 = intra1-perpAxis1*dot;
   intra_p2 = intra2-perpAxis2*dot;
   
   /* dot = dot_product3f(intra_p, dir); */
   dot = intra_p0 * dir0 +  intra_p1 * dir1 +  intra_p2 * dir2;

   vradial0=intra_p0-dir0*dot;
   vradial1=intra_p1-dir1*dot;
   vradial2=intra_p2-dir2*dot;  
   
   radialsq   = ((vradial0*vradial0) + (vradial1*vradial1) + (vradial2*vradial2));
   
   /* now figure out the axial distance along the cyl-line that will give us
     the point of closest approach */

   if(ab_dangle < kR_SMALL4)
      axial_perp   = _0;
   else
      axial_perp   = (float)(sqrt1f(radialsq)/tan_acos_dangle);
  
   axial = (float) sqrt1f( ( (intra_p0*intra_p0) + 
                             (intra_p1*intra_p1) + 
                             (intra_p2*intra_p2)) - radialsq );
   
   if( (intra_p0*dir0 + intra_p1*dir1 + intra_p2*dir2 ) >= _0 )
      axial = axial_perp - axial;
   else
      axial = axial_perp + axial;
   
   /* now we have to think about where the vector will actually strike the cylinder*/
   
   /* by definition, it must be perpdist away from the perp-plane becuase the perp-plane
     is parallel to the line, so we can compute the radial component to this point */
   
   radial = radius*radius - perpDist*perpDist;
   radial = (float)sqrt1f(radial);
   
   /* now the trick is figuring out how to adjust the axial distance to get the actual
     position along the cyl line which will give us a representative sphere */
   
   if(ab_dangle > kR_SMALL4)
      axial_sum = axial - radial/tan_acos_dangle;
   else
      axial_sum = axial;
   /*
    printf("radial2 %8.3f \n",radial);*/

   if(axial_sum < _0)
      axial_sum = _0;
   else if(axial_sum > maxial)
      axial_sum = maxial;
   
   sphere[0] = dir0 * axial_sum + point0;
   sphere[1] = dir1 * axial_sum + point1;
   sphere[2] = dir2 * axial_sum + point2;
   
   *asum = axial_sum;
   /*  printf("==>%8.3f sphere %8.3f %8.3f %8.3f\n",base[1],sphere[1],axial_perp,axial);*/
   return(1);
}


#ifdef _PYMOL_INLINE
__inline__
#endif
static int FrontToInteriorSphere(float *front,
                                      float *point,
                                      float *dir,
                                      float radius,
                                      float radius2,
                                      float maxial)
{
  float intra_p[3];
  float axial;
  float intra[3],axis[3];
  float sphere[3];

  subtract3f(point,front,intra);
  remove_component3f(intra,dir,intra_p);
  add3f(front,intra_p,intra_p);
  subtract3f(point,intra_p,axis);
  axial = -dot_product3f(axis,dir);

  if(axial<0.0F) axial=0.0F;
  else if(axial>maxial) axial = maxial;

  sphere[0]=axial*dir[0]+point[0];
  sphere[1]=axial*dir[1]+point[1];
  sphere[2]=axial*dir[2]+point[2];

  return (diffsq3f(sphere,front)<=radius2);
}


/*========================================================================*/
#ifdef _PYMOL_INLINE
__inline__
#endif
static int ZLineToSphereCapped(float *base,float *point,float *dir,float radius,float maxial,
                  float *sphere,float *asum,int cap1,int cap2,float *pre)
{
  /* Strategy - find an imaginary sphere that lies at the correct point on
     the line segment, then treat as a sphere reflection */

  float perpAxis[3],intra_p[3];
  float perpDist,radial,axial,axial_sum,dangle,ab_dangle,axial_perp;
  float radialsq,tan_acos_dangle;
  float len_proj;
  float intra[3],vradial[3];
  float diff[3],fpoint[3];
  float proj[3];


  perpAxis[0] = pre[0]; /* was cross_product(MinusZ,dir,perpAxis),normalize */
  perpAxis[1] = pre[1];

 /* the perpAxis defines a perp-plane which includes the cyl-axis */
  
  intra[0]=point[0]-base[0];
  intra[1]=point[1]-base[1];

  /* get minimum distance between the lines */

  perpDist = intra[0]*perpAxis[0] + intra[1]*perpAxis[1];
  /* was dot_product3f(intra,perpAxis); */

  if(fabs(perpDist)>radius) {
    return(0);
  }

  perpAxis[2] = 0.0;
  intra[2]=point[2]-base[2];

  dangle = -dir[2]; /* was dot(MinusZ,dir) */
  ab_dangle= (float)fabs(dangle);
  if(ab_dangle>(1-kR_SMALL4)) /* vector inline with light ray... */
    {
      vradial[0]=point[0]-base[0];
      vradial[1]=point[1]-base[1];
      vradial[2]=0.0F;
      radial = (float)length3f(vradial);
      if(radial>radius)
        return 0;
      if(dangle>0.0) {
        switch(cap1) {
        case cCylCapFlat:
          sphere[0]=base[0];
          sphere[1]=base[1];
          sphere[2]=point[2]-radius;
          break;
        case cCylCapRound:
          sphere[0]=point[0];
          sphere[1]=point[1];
          sphere[2]=point[2];
        }
        return(1);
      } else {
        switch(cap1) {
        case cCylCapFlat:
          sphere[0]=base[0];
          sphere[1]=base[1];
          sphere[2]=dir[2]*maxial+point[2]-radius;
          break;
        case cCylCapRound:
          sphere[0]=dir[0]*maxial+point[0];
          sphere[1]=dir[1]*maxial+point[1];
          sphere[2]=dir[2]*maxial+point[2];
        }
        return(1);
      }
    }
  
  /*tan_acos_dangle = tan(acos(dangle));*/
  tan_acos_dangle = (float)sqrt1f(1-dangle*dangle)/dangle;

  /*
  printf("perpDist %8.3f\n",perpDist);
  printf("dir %8.3f %8.3f %8.3f\n",dir[0],dir[1],dir[2]);
  printf("base %8.3f %8.3f %8.3f\n",base[0],base[1],base[2]);
  printf("point %8.3f %8.3f %8.3f\n",point[0],point[1],point[2]);
  printf("intra %8.3f %8.3f %8.3f\n",intra[0],intra[1],intra[2]);
  printf("perpAxis %8.3f %8.3f %8.3f\n",perpAxis[0],perpAxis[1],perpAxis[2]);
  */

  /* now we need to define the triangle in the perp-plane  
     to figure out where the projected line intersection point is */

  /* first, compute radial distance in the perp-plane between the two starting points */

  remove_component3f(intra,perpAxis,intra_p);
  remove_component3f(intra_p,dir,vradial);

  radialsq = lengthsq3f(vradial);

  /* now figure out the axial distance along the cyl-line that will give us
     the point of closest approach */

  if(ab_dangle<kR_SMALL4)
    axial_perp=0;
  else
    axial_perp = (float)sqrt1f(radialsq)/tan_acos_dangle;
  
  axial = (float)lengthsq3f(intra_p)-radialsq;
  axial = (float)sqrt1f(axial);

  /*
  printf("radial %8.3f\n",radial);
  printf("vradial %8.3f %8.3f %8.3f\n",vradial[0],vradial[1],vradial[2]);
  printf("radial %8.3f\n",radial);
  printf("dangle %8.3f \n",dangle);
  printf("axial_perp %8.3f \n",axial_perp);
  printf("axial1 %8.3f \n",axial);
  printf("%8.3f\n",dot_product3f(intra_p,dir));
  */

  if(dot_product3f(intra_p,dir)>=0.0)
    axial = axial_perp - axial;
  else
    axial = axial_perp + axial;

  /*
  printf("axial2 %8.3f\n",axial);
  */
  
  /* now we have to think about where the vector will actually strike the cylinder*/

  /* by definition, it must be perpdist away from the perp-plane becuase the perp-plane
     is parallel to the line, so we can compute the radial component to this point */

  radial = radius*radius-perpDist*perpDist;
  radial = (float)sqrt1f(radial);

  /* now the trick is figuring out how to adjust the axial distance to get the actual
     position along the cyl line which will give us a representative sphere */

  if(ab_dangle > kR_SMALL4)
    axial_sum = axial - radial/tan_acos_dangle;
  else
    axial_sum = axial;

  /*    printf("ab_dangle %8.3f \n",ab_dangle);
 
    printf("axial_sum %8.3f \n",axial_sum);
  */
  if(axial_sum<0) {
    switch(cap1) {
    case cCylCapFlat:
      subtract3f(point,base,diff);
      project3f(diff,dir,proj);
      len_proj = (float)length3f(proj);
      dangle = -proj[2]/len_proj;
      if(fabs(dangle)<kR_SMALL4) 
        return 0;
      sphere[0]=base[0];
      sphere[1]=base[1];
      sphere[2]=base[2]-len_proj/dangle;
      if(diff3f(sphere,point)>radius)
        return 0; 
      sphere[0]+=dir[0]*radius;
      sphere[1]+=dir[1]*radius;
      sphere[2]+=dir[2]*radius;
      *asum=0;
      break;
    case cCylCapRound:
      axial_sum=0;
      sphere[0]=point[0];
      sphere[1]=point[1];
      sphere[2]=point[2];
      *asum = axial_sum;
      break;
    case cCylCapNone:
    default:
      return 0;
      break;
    }
  } else if(axial_sum>maxial) {
    switch(cap2) {

    case cCylCapFlat:
      scale3f(dir,maxial,fpoint);
      add3f(fpoint,point,fpoint);
      subtract3f(fpoint,base,diff);
      project3f(diff,dir,proj);
      len_proj = (float)length3f(proj);
      dangle = -proj[2]/len_proj;
      if(fabs(dangle)<kR_SMALL4) 
        return 0;
      sphere[0]=base[0];
      sphere[1]=base[1];
      sphere[2]=base[2]-len_proj/dangle;
      if(diff3f(sphere,fpoint)>radius)
        return 0; 
      sphere[0]-=dir[0]*radius;
      sphere[1]-=dir[1]*radius;
      sphere[2]-=dir[2]*radius;
      *asum=maxial;
      break;
    case cCylCapRound:
      axial_sum=maxial;
      sphere[0]=dir[0]*axial_sum+point[0];
      sphere[1]=dir[1]*axial_sum+point[1];
      sphere[2]=dir[2]*axial_sum+point[2];
      *asum = axial_sum;
      break;
    case cCylCapNone:
    default:
      return 0;
      break;
    }
  } else {
    sphere[0]=dir[0]*axial_sum+point[0];
    sphere[1]=dir[1]*axial_sum+point[1];
    sphere[2]=dir[2]*axial_sum+point[2];
  
    *asum = axial_sum;

    /*  printf("==>%8.3f sphere %8.3f %8.3f %8.3f\n",base[1],sphere[1],axial_perp,axial);*/
  }
  return(1);
  
}



#ifdef _PYMOL_INLINE
__inline__
#endif
static int LineToSphereCapped(float *base, float *ray,
                       float *point,float *dir,float radius,float maxial,
                              float *sphere,float *asum,int cap1,int cap2)
{
  /* Strategy - find an imaginary sphere that lies at the correct point on
     the line segment, then treat as a sphere reflection */

  float perpAxis[3],intra_p[3];
  float perpDist,radial,axial,axial_sum,dangle,ab_dangle,axial_perp;
  float radialsq,tan_acos_dangle;
  float len_proj;
  float intra[3],vradial[3];
  float diff[3],fpoint[3];
  float proj[3];


   subtract3f(point,base,intra);

   cross_product3f(ray,dir,perpAxis);

   normalize3f(perpAxis);

   /* the perpAxis defines a perp-plane which includes the cyl-axis */
   
   /* get minimum distance between the lines */

   perpDist = dot_product3f(intra, perpAxis);

  /* was dot_product3f(intra,perpAxis); */

  if(fabs(perpDist)>radius) {
    return(0);
  }

  dangle   = dot_product3f(ray, dir);
  ab_dangle   = (float)fabs(dangle);
  
  if(ab_dangle>(1-kR_SMALL4)) /* vector inline with light ray... */
    {
      vradial[0]=point[0]-base[0];
      vradial[1]=point[1]-base[1];
      vradial[2]=point[2]-base[2];
      radial = (float)length3f(vradial);
      if(radial>radius)
        return 0;
      if(dangle>0.0) {
        switch(cap1) {
        case cCylCapFlat:
          sphere[0]=dir[0]*radius+point[0];
          sphere[1]=dir[1]*radius+point[1];
          sphere[2]=dir[2]*radius+point[2];
          break;
        case cCylCapRound:
          sphere[0]=point[0];
          sphere[1]=point[1];
          sphere[2]=point[2];
        }
        return(1);
      } else {
        switch(cap1) {
        case cCylCapFlat:
          maxial-=radius;
          break;
        }
        sphere[0]=dir[0]*maxial+point[0];
        sphere[1]=dir[1]*maxial+point[1];
        sphere[2]=dir[2]*maxial+point[2];
        return(1);
      }
    }
  
  /*tan_acos_dangle = tan(acos(dangle));*/
  tan_acos_dangle = (float)sqrt1f(1-dangle*dangle)/dangle;

  /*
  printf("perpDist %8.3f\n",perpDist);
  printf("dir %8.3f %8.3f %8.3f\n",dir[0],dir[1],dir[2]);
  printf("base %8.3f %8.3f %8.3f\n",base[0],base[1],base[2]);
  printf("point %8.3f %8.3f %8.3f\n",point[0],point[1],point[2]);
  printf("intra %8.3f %8.3f %8.3f\n",intra[0],intra[1],intra[2]);
  printf("perpAxis %8.3f %8.3f %8.3f\n",perpAxis[0],perpAxis[1],perpAxis[2]);
  */

  /* now we need to define the triangle in the perp-plane  
     to figure out where the projected line intersection point is */

  /* first, compute radial distance in the perp-plane between the two starting points */

  remove_component3f(intra,perpAxis,intra_p);
  remove_component3f(intra_p,dir,vradial);

  radialsq = lengthsq3f(vradial);

  /* now figure out the axial distance along the cyl-line that will give us
     the point of closest approach */

  if(ab_dangle<kR_SMALL4)
    axial_perp=0;
  else
    axial_perp = (float)sqrt1f(radialsq)/tan_acos_dangle;
  
  axial = (float)lengthsq3f(intra_p)-radialsq;
  axial = (float)sqrt1f(axial);

  /*
  printf("radial %8.3f\n",radial);
  printf("vradial %8.3f %8.3f %8.3f\n",vradial[0],vradial[1],vradial[2]);
  printf("radial %8.3f\n",radial);
  printf("dangle %8.3f \n",dangle);
  printf("axial_perp %8.3f \n",axial_perp);
  printf("axial1 %8.3f \n",axial);
  printf("%8.3f\n",dot_product3f(intra_p,dir));
  */

  if(dot_product3f(intra_p,dir)>=0.0)
    axial = axial_perp - axial;
  else
    axial = axial_perp + axial;

  /*
  printf("axial2 %8.3f\n",axial);
  */
  
  /* now we have to think about where the vector will actually strike the cylinder*/

  /* by definition, it must be perpdist away from the perp-plane becuase the perp-plane
     is parallel to the line, so we can compute the radial component to this point */

  radial = radius*radius-perpDist*perpDist;
  radial = (float)sqrt1f(radial);

  /* now the trick is figuring out how to adjust the axial distance to get the actual
     position along the cyl line which will give us a representative sphere */

  if(ab_dangle > kR_SMALL4)
    axial_sum = axial - radial/tan_acos_dangle;
  else
    axial_sum = axial;

  /*    printf("ab_dangle %8.3f \n",ab_dangle);
 
    printf("axial_sum %8.3f \n",axial_sum);
  */
  if(axial_sum<0) {
    switch(cap1) {
    case cCylCapFlat:
      subtract3f(point,base,diff);
      project3f(diff,dir,proj);
      len_proj = (float)length3f(proj);
      dangle = dot_product3f(proj,ray)/len_proj;
      if(fabs(dangle)<kR_SMALL4) 
        return 0;
      len_proj /= dangle;
      sphere[0]=base[0]+ray[0]*len_proj;
      sphere[1]=base[1]+ray[1]*len_proj;
      sphere[2]=base[2]+ray[2]*len_proj;
      if(diff3f(sphere,point)>radius)
        return 0; 
      sphere[0]+=dir[0]*radius;
      sphere[1]+=dir[1]*radius;
      sphere[2]+=dir[2]*radius;
      *asum=0;
      break;
    case cCylCapRound:
      axial_sum=0;
      sphere[0]=point[0];
      sphere[1]=point[1];
      sphere[2]=point[2];
      *asum = axial_sum;
      break;
    case cCylCapNone:
    default:
      return 0;
      break;
    }
  } else if(axial_sum>maxial) {
    switch(cap2) {

    case cCylCapFlat:
      scale3f(dir,maxial,fpoint);
      add3f(fpoint,point,fpoint);
      subtract3f(fpoint,base,diff);
      project3f(diff,dir,proj);
      len_proj = (float)length3f(proj);
      dangle = dot_product3f(proj,ray)/len_proj;
      if(fabs(dangle)<kR_SMALL4) 
        return 0;
      len_proj /= dangle;
      sphere[0]=base[0]+ray[0]*len_proj;
      sphere[1]=base[1]+ray[1]*len_proj;
      sphere[2]=base[2]+ray[2]*len_proj;
      if(diff3f(sphere,fpoint)>radius)
        return 0; 
      sphere[0]-=dir[0]*radius;
      sphere[1]-=dir[1]*radius;
      sphere[2]-=dir[2]*radius;
      *asum=maxial;
      break;
    case cCylCapRound:
      axial_sum=maxial;
      sphere[0]=dir[0]*axial_sum+point[0];
      sphere[1]=dir[1]*axial_sum+point[1];
      sphere[2]=dir[2]*axial_sum+point[2];
      *asum = axial_sum;
      break;
    case cCylCapNone:
    default:
      return 0;
      break;
    }
  } else {
    sphere[0]=dir[0]*axial_sum+point[0];
    sphere[1]=dir[1]*axial_sum+point[1];
    sphere[2]=dir[2]*axial_sum+point[2];
  
    *asum = axial_sum;

    /*  printf("==>%8.3f sphere %8.3f %8.3f %8.3f\n",base[1],sphere[1],axial_perp,axial);*/
  }
  return(1);
  
}



#ifdef _PYMOL_INLINE
__inline__
#endif
static int FrontToInteriorSphereCapped(float *front,
                                            float *point,
                                            float *dir,
                                            float radius,
                                            float radius2,
                                            float maxial,
                                            int cap1,
                                            int cap2)
{
  float intra_p[3];
  float axial;
  float intra[3],axis[3];
  float sphere[3];

  subtract3f(point,front,intra);
  remove_component3f(intra,dir,intra_p);
  add3f(front,intra_p,intra_p);
  subtract3f(point,intra_p,axis);
  axial = -dot_product3f(axis,dir);

  if(axial<0.0F) return 0;
  else if(axial>maxial) return 0;

  sphere[0]=axial*dir[0]+point[0];
  sphere[1]=axial*dir[1]+point[1];
  sphere[2]=axial*dir[2]+point[2];

  return (diffsq3f(sphere,front)<radius2);
  
}


/*========================================================================*/
#ifdef _PYMOL_INLINE
__inline__
#endif
static float ZLineClipPoint(float *base,float *point,float *alongNormalSq,float cutoff)
{
   float hyp0, hyp1, hyp2;
   float result   = MAXFLOAT;
   
   /* this routine determines whether or not a vector starting at "base"
     heading in the direction "normal" intersects a sphere located at "point".

     It returns how far along the vector the intersection with the plane is or
     MAXFLOAT if there isn't a relevant intersection

     NOTE: this routine has been optimized for normals along Z
     Optimizes-out vectors that are more than "cutoff" from "point" in x,y plane 
   */

   hyp0 = point[0] - base[0];
   if(fabs(hyp0) > cutoff)
      return result;
      
   hyp1 = point[1] - base[1];
   if(fabs(hyp1) > cutoff)
      return result;
      
   hyp2 = point[2] - base[2];

   if(hyp2 < 0.0)
   {
      (*alongNormalSq) = (hyp2 * hyp2);
      result   = (hyp0 * hyp0) + (hyp1 * hyp1);
   }
   return result;
}

#ifdef _PYMOL_INLINE
__inline__
#endif
static int LineClipPoint(float *base,float *ray, 
                           float *point, float *dist,
                           float cutoff, float cutoff2)
{
   register float hyp0, hyp1, hyp2;
   register float opp0, opp1, opp2;
   register float adj0, adj1, adj2;
   register float ray0, ray1, ray2;
   register float proj;
   register double dcutoff = (double)cutoff;
   register float opp_len_sq;

   
   /* this routine determines whether or not a vector starting at "base"
     heading in the direction "ray" intersects a sphere located at "point".

     It returns how far along the vector the intersection with the plane is or
     MAXFLOAT if there isn't a relevant intersection

     NOTE: this routine has been optimized for normals along Z
     Optimizes-out vectors that are more than "cutoff" from "point" 
   */

   /* compute the hypo */
   
   hyp2 = point[2] - base[2]; 
   hyp1 = point[1] - base[1];
   hyp0 = point[0] - base[0];

   ray0 = ray[0];
   ray1 = ray[1];
   ray2 = ray[2];
     
   /* compute the adjacent edge (dot-projection) */
   
   proj = (ray0 * hyp0) + (ray1 * hyp1) + (ray2 * hyp2);
   
   adj0 = ray0 * proj;
   adj1 = ray1 * proj;
   opp0 = hyp0 - adj0;
   adj2 = ray2 * proj;
   if(fabs(opp0) > dcutoff)
     return 0;
   opp1 = hyp1 - adj1;
   opp2 = hyp2 - adj2;
   if(fabs(opp1) > dcutoff)
     return 0;
   if(fabs(opp2) > dcutoff)
     return 0;
   
   opp_len_sq = (opp0 * opp0) + (opp1 * opp1) + (opp2 * opp2);
   
   if(opp_len_sq<=cutoff2) {
     (*dist= proj - (float)sqrt1f(cutoff2-opp_len_sq));
     return 1;
   }
   
   return 0;
}

/*========================================================================*/
void BasisSetupMatrix(CBasis *I)
{
  float oldZ[3] = { 0.0,0.0,1.0 };
  float newY[3];
  float dotgle,angle;

  cross_product3f(oldZ,I->LightNormal,newY);

  dotgle=dot_product3f(oldZ,I->LightNormal);
  
  if((1.0-fabs(dotgle))<kR_SMALL4)
    {
      dotgle=(float)(dotgle/fabs(dotgle));
      newY[0]=0.0;
      newY[1]=1.0;
      newY[2]=0.0;
    }
  
  normalize3f(newY);

  angle=(float)(-acos(dotgle));
  
  /* now all we gotta do is effect a rotation about the new Y axis to line up new Z with Z */
  
  rotation_to_matrix33f(newY,angle,I->Matrix);

  /*
  printf("%8.3f %8.3f %8.3f %8.3f\n",angle*180.0/cPI,newY[0],newY[1],newY[2]);
  
  matrix_transform33f3f(I->Matrix,newY,test);
  printf("   %8.3f %8.3f %8.3f\n",test[0],test[1],test[2]);

  printf("   %8.3f %8.3f %8.3f\n",I->LightNormal[0],I->LightNormal[1],I->LightNormal[2]);
  matrix_transform33f3f(I->Matrix,I->LightNormal,test);
  printf("   %8.3f %8.3f %8.3f\n",test[0],test[1],test[2]);

  printf(">%8.3f %8.3f %8.3f\n",I->Matrix[0][0],I->Matrix[0][1],I->Matrix[0][2]);
  printf(">%8.3f %8.3f %8.3f\n",I->Matrix[1][0],I->Matrix[1][1],I->Matrix[1][2]);
  printf(">%8.3f %8.3f %8.3f\n",I->Matrix[2][0],I->Matrix[2][1],I->Matrix[2][2]);
  */
}

/*========================================================================*/
void BasisGetTriangleFlatDotgle(CBasis *I,RayInfo *r,int i)
{
  float   *n0 = I->Normal + (3 * I->Vert2Normal[i]); 
  r->flat_dotgle = n0[2];
}
void BasisGetTriangleFlatDotglePerspective(CBasis *I,RayInfo *r,int i)
{
  float   *n0 = I->Normal + (3 * I->Vert2Normal[i]); 
  r->flat_dotgle = -dot_product3f(r->dir,n0);
}

/*========================================================================*/
void BasisGetTriangleNormal(CBasis *I,RayInfo *r,int i,float *fc,int perspective) 
{
   float *n0,w2,fc0,fc1,fc2;
   float vt1[3];
   CPrimitive   *lprim   = r->prim;
   
   if(perspective) {
     r->impact[0] = r->base[0]+r->dir[0]*r->dist; 
     r->impact[1] = r->base[1]+r->dir[1]*r->dist; 
     r->impact[2] = r->base[2]+r->dir[2]*r->dist;
   } else {
     r->impact[0] = r->base[0]; 
     r->impact[1] = r->base[1]; 
     r->impact[2] = r->base[2]-r->dist;
   }

   n0 = I->Normal + (3 * I->Vert2Normal[i]) + 3; /* skip triangle normal */
   w2 = 1.0F - (r->tri1 + r->tri2);
   /*  printf("%8.3f %8.3f\n",r->tri[1],r->tri[2]);*/

   fc0 = (lprim->c2[0]*r->tri1)+(lprim->c3[0]*r->tri2)+(lprim->c1[0]*w2);
   fc1 = (lprim->c2[1]*r->tri1)+(lprim->c3[1]*r->tri2)+(lprim->c1[1]*w2);
   fc2 = (lprim->c2[2]*r->tri1)+(lprim->c3[2]*r->tri2)+(lprim->c1[2]*w2);

   r->trans = (lprim->tr[1]*r->tri1)+(lprim->tr[2]*r->tri2)+(lprim->tr[0]*w2);

   scale3f(n0+3, r->tri1, r->surfnormal);
   scale3f(n0+6, r->tri2, vt1);
   add3f(vt1, r->surfnormal, r->surfnormal);

   scale3f(n0,w2,vt1);
   add3f(vt1,r->surfnormal,r->surfnormal);

   normalize3f(r->surfnormal);
   
   fc[0] = fc0;
   fc[1] = fc1;
   fc[2] = fc2;
}

/*========================================================================*/
static void BasisGetTriangleImpactColor(CBasis *I,RayInfo *r,int i,float *fc)
{
}



#ifdef PROFILE_BASIS
int n_cells = 0;
int n_prims = 0;
int n_triangles = 0;
int n_spheres = 0;
int n_cylinders = 0;
int n_sausages = 0;
int n_skipped = 0;
#endif


#ifdef _PYMOL_INLINE
__inline__
#endif
int BasisHitPerspective(BasisCallRec *BC)
{
  register CBasis *BI = BC->Basis;
  register MapType *map = BI->Map;
  register int iMin0 = map->iMin[0];
  register int iMin1 = map->iMin[1];
  register int iMin2 = map->iMin[2];
  register int iMax0 = map->iMax[0];
  register int iMax1 = map->iMax[1];
  register int iMax2 = map->iMax[2];
  register int a,b,c;

  register float	iDiv	= map->recipDiv;
  register float base0, base1, base2;

  register float min0 = map->Min[0] * iDiv;
  register float min1 = map->Min[1] * iDiv;
  register float min2 = map->Min[2] * iDiv;

  int new_ray = !BC->pass;
  RayInfo *r = BC->rr;

  MapCache *cache = &BC->cache;   
  int *cache_cache = cache->Cache;
  int *cache_CacheLink = cache->CacheLink;

  CPrimitive *r_prim = NULL;  

  if(new_ray) { /* see if we can eliminate this ray right away using the mask */

    base0 = (r->base[0] * iDiv) - min0;
    base1 = (r->base[1] * iDiv) - min1;
    
    a = (int)base0;
    b = (int)base1;
    a += MapBorder;
    b += MapBorder;
    if(a < iMin0) a = iMin0; 
    else if(a > iMax0) a = iMax0; 
    if(b < iMin1) b = iMin1;
    else if(b > iMax1) b = iMax1;
    
    if(!*(map->EMask + a * map->Dim[1] + b)) 
      return -1;
  }

  {
    register int last_a = -1, last_b = -1, last_c = -1;
    register int allow_break;
    register int minIndex=-1;

    register float step0, step1, step2;
    register float back_dist = BC->back_dist;
    
    const float   _0   = 0.0F, _1 = 1.0F;
    float r_tri1=_0, r_tri2=_0, r_dist, dist; /* zero inits to suppress compiler warnings */
    float r_sphere0=_0,r_sphere1=_0,r_sphere2=_0;
    int  h,*ip;
    int      excl_trans_flag;
    int      *elist, local_iflag = false;
    int terminal = -1;
    int *ehead = map->EHead;
    int d1d2 = map->D1D2;
    int d2 = map->Dim[2];
    const int *vert2prim = BC->vert2prim;
    const float excl_trans = BC->excl_trans;
    const float BasisFudge0 = BC->fudge0;
    const float BasisFudge1 = BC->fudge1;
    int v2p;
    int i,ii;
    int n_vert = BI->NVertex, n_eElem = map->NEElem;
    int except = BC->except;
    int check_interior_flag   = BC->check_interior;
    float   sph[3],vt[3],tri1,tri2; 
    register CPrimitive *BC_prim = BC->prim;
    register int *BI_Vert2Normal = BI->Vert2Normal;
    register float *BI_Vertex = BI->Vertex;
    register float *BI_Precomp = BI->Precomp;
    float *BI_Normal = BI->Normal;
    float *BI_Radius = BI->Radius;
    float *BI_Radius2 = BI->Radius2;
    copy3f(r->base, vt);

    elist   = map->EList;

    r_dist = MAXFLOAT;
  
    excl_trans_flag   = (excl_trans != _0);       
  
    if(except >= 0)
      except   = vert2prim[except];

    MapCacheReset(cache);


    { /* take steps with a Z-size equil to the grid spacing */
      register float div = iDiv * (-MapGetDiv(BI->Map)/r->dir[2]);
      step0 = r->dir[0]*div;
      step1 = r->dir[1]*div;
      step2 = r->dir[2]*div;
    }

    base0 = (r->skip[0] * iDiv) - min0;
    base1 = (r->skip[1] * iDiv) - min1;
    base2 = (r->skip[2] * iDiv) - min2;
    
    allow_break = false;
    while(1) {
      int inside_code;
      register int clamped;

      a	= ((int)base0);
      b	= ((int)base1);
      c	= ((int)base2);

      inside_code = 1;
      clamped = false;
      
      a += MapBorder;
      b += MapBorder;
      c += MapBorder;
#define EDGE_ALLOWANCE 1

      if(a < iMin0)  {
        if(((iMin0 - a) > EDGE_ALLOWANCE) && allow_break)
          break;
        else {
          a = iMin0; 
          clamped = true;
        }
      } else if(a > iMax0) { 
        if(((a - iMax0) > EDGE_ALLOWANCE) && allow_break)
          break;
        else {
          a = iMax0; 
          clamped = true;
        }
      }
      if(b < iMin1) { 
        if(((iMin1 - b) > EDGE_ALLOWANCE) && allow_break)
          break;
        else {
          b = iMin1;
          clamped = true;
        }
      } else if(b > iMax1) {
        if(((b - iMax1) > EDGE_ALLOWANCE) && allow_break)
          break;
        else {
          b = iMax1;
          clamped = true;
        }
      }

      if(c < iMin2) { 
        if((iMin2 - c) > EDGE_ALLOWANCE) 
          break;
        else {
          c = iMin2;
          clamped = true;
        }
      } else if(c > iMax2)  {
        if((c - iMax2) > EDGE_ALLOWANCE)
          inside_code = 0;
        else {
          c = iMax2;
          clamped = true;
        }
      }
      if(inside_code && (((a!=last_a)||(b!=last_b)||(c!=last_c)))) {
        register int new_min_index;   
        h  = *(ehead + (a * d1d2) + (b * d2) + c);
        
        new_min_index = -1;     
        
        if(!clamped) /* don't discard a ray until it has hit the objective at least once */
          allow_break = true;
        
        if((terminal>0)&&(last_c!=c)) {
          if(!terminal--)
            break;
        }
        if((h>0) && (h<n_eElem)) {
          int do_loop;
          
          ip = elist + h;
          last_a = a;
          i = *(ip++);
          last_b = b;
          do_loop = ((i>=0)&&(i<n_vert));
          last_c = c;
          
          while(do_loop) /* n_vert checking is a bug workaround */
            {
              ii = *(ip++);
              v2p = vert2prim[i];
              do_loop = ((ii>=0)&&(ii<n_vert));
              if((v2p != except) && (!MapCached(cache,v2p))) 
                {
                  register CPrimitive *prm = BC_prim + v2p;
                  int prm_type;
                  
                  /*MapCache(cache,v2p);*/
                  cache_cache[v2p] = 1;
                  prm_type = prm->type;
                  cache_CacheLink[v2p] = cache->CacheStart;
                  cache->CacheStart = v2p;
                  
                  switch(prm_type)  {
                  case cPrimTriangle:
                  case cPrimCharacter:
                    {
                      float *dir = r->dir;
                      float *d10 = BI_Precomp + BI_Vert2Normal[i] * 3;
                      float *d20 = d10+3;
                      register float det, inv_det;
                      register float pvec0,pvec1, pvec2;
                      register float dir0 = dir[0], dir1 = dir[1], dir2 = dir[2];
                      register float d20_0 = d20[0], d20_1 = d20[1], d20_2 = d20[2];
                      register float d10_0 = d10[0], d10_1 = d10[1], d10_2 = d10[2];
                      
                      /* cross_product3f(dir, d20, pvec); */
                      
                      pvec0 = dir1*d20_2 - dir2*d20_1;
                      pvec1 = dir2*d20_0 - dir0*d20_2;
                      pvec2 = dir0*d20_1 - dir1*d20_0;

                      /* det = dot_product3f(pvec, d10); */
                      
                      det = pvec0 * d10_0 + pvec1 * d10_1 + pvec2 * d10_2;
                      
                      if(fabs(det) >= EPSILON) {
                        float *v0 = BI_Vertex + prm->vert*3;
                        register float tvec0, tvec1, tvec2;
                        register float qvec0, qvec1, qvec2;
                        
                        inv_det = _1/det;
                        
                        /* subtract3f(vt,v0,tvec); */
                        
                        tvec0 = vt[0] - v0[0];
                        tvec1 = vt[1] - v0[1];
                        tvec2 = vt[2] - v0[2];
                        
                        /* dot_product3f(tvec,pvec) * inv_det; */
                        tri1 = (tvec0 * pvec0 + tvec1 * pvec1 + tvec2 * pvec2) * inv_det;
                        
                        /* cross_product3f(tvec,d10,qvec); */
                        
                        qvec0 = tvec1*d10_2 - tvec2*d10_1;
                        qvec1 = tvec2*d10_0 - tvec0*d10_2;

                        if( (tri1>= BasisFudge0) && (tri1<=BasisFudge1)) {
                          qvec2 = tvec0*d10_1 - tvec1*d10_0;
                      
                          /* dot_product3f(dir, qvec) * inv_det; */
                          tri2 = (dir0 * qvec0 + dir1 * qvec1 + dir2 * qvec2) * inv_det;
                      
                          /* dot_product3f(d20, qvec) * inv_det; */
                          dist = (d20_0 * qvec0 + d20_1 * qvec1 + d20_2 * qvec2) * inv_det;

                          if( (tri2 >= BasisFudge0) && (tri2 <= BasisFudge1) && ((tri1 + tri2) <= BasisFudge1))
                            {
                              if( (dist < r_dist) && (dist >= _0) && 
                                  (dist <= back_dist) && (prm->trans != _1) ) 
                                {
                                  new_min_index   = prm->vert;
                                  r_tri1      = tri1;
                                  r_tri2      = tri2;
                                  r_dist      = dist;
                                }
                            }
                        }
                      }
                    }
                    break;
                  case cPrimSphere:
                    {

                      if(LineClipPoint( r->base, r->dir, 
                                        BI_Vertex + i*3, &dist, 
                                        BI_Radius[i] , BI_Radius2[i]))
                        {
                          if((dist < r_dist) && (prm->trans != _1))
                            {
                              if((dist >= _0) && (dist <= back_dist)) {
                                new_min_index = prm->vert;
                                r_dist      = dist;
                              } else if(check_interior_flag && (dist<=back_dist))  {
                                if(diffsq3f(vt,BI_Vertex+i*3) < BI_Radius2[i]) {
                              
                                  local_iflag   = true;
                                  r_prim      = prm;
                                  r_dist    = _0;
                                  new_min_index   = prm->vert;
                                }
                              }
                            }
                        }
                    }
                    break;
                
                  case cPrimCylinder:
                    if(LineToSphereCapped(r->base,r->dir,BI_Vertex+i*3, 
                                          BI_Normal+BI_Vert2Normal[i]*3,
                                          BI_Radius[i], prm->l1,sph,&tri1,
                                          prm->cap1,prm->cap2))
                      {
                        if(LineClipPoint(r->base,r->dir,sph,&dist,BI_Radius[i],BI_Radius2[i]))
                          {
                            if((dist < r_dist) && (prm->trans != _1))
                              {
                                if((dist >= _0) && (dist <= back_dist)) 
                                  {
                                    if(prm->l1 > kR_SMALL4)
                                      r_tri1   = tri1 / prm->l1;
                                
                                    r_sphere0   = sph[0];
                                    r_sphere1   = sph[1];                              
                                    r_sphere2   = sph[2];
                                    new_min_index   = prm->vert;
                                    r_dist      = dist;
                                  }
                                else if(check_interior_flag && (dist<=back_dist))
                                  {
                                    if(FrontToInteriorSphereCapped(vt,
                                                                   BI_Vertex+i*3,
                                                                   BI_Normal+BI_Vert2Normal[i]*3,
                                                                   BI_Radius[i],
                                                                   BI_Radius2[i],
                                                                   prm->l1,
                                                                   prm->cap1,
                                                                   prm->cap2)) 
                                      {
                                        local_iflag   = true;
                                        r_prim      = prm;
                                        r_dist      = _0;
                                    
                                        new_min_index   = prm->vert;
                                      }
                                  }
                              }
                          }
                      }
                    break;
                
                  case cPrimSausage:
                    if(LineToSphere(r->base,r->dir, 
                                    BI_Vertex+i*3,BI_Normal+BI_Vert2Normal[i]*3,
                                    BI_Radius[i],prm->l1,sph,&tri1))
                      {

                        if(LineClipPoint(r->base,r->dir,sph,&dist,BI_Radius[i],BI_Radius2[i])) 
                          {


                            int   tmp_flag = false;
                            if((dist<r_dist)  && (prm->trans != _1) )
                              {
                                if((dist >= _0) && (dist <= back_dist)) 
                                  {
                                    tmp_flag = true;
                                    if(excl_trans_flag) 
                                      {
                                        if( (prm->trans > _0) && (dist < excl_trans) )
                                          tmp_flag = false;
                                      }
                                    if(tmp_flag) 
                                      {

                                        if(prm->l1 > kR_SMALL4)
                                          r_tri1   = tri1 / prm->l1;
                                    
                                        r_sphere0   = sph[0];
                                        r_sphere1   = sph[1];                              
                                        r_sphere2   = sph[2];
                                        new_min_index      = prm->vert;
                                        r_dist         = dist;

                                    
                                      }
                                  }
                                else if(check_interior_flag &&( dist<=back_dist) ) 
                                  {
                                    if(FrontToInteriorSphere(vt, BI_Vertex+i*3, 
                                                             BI_Normal+BI_Vert2Normal[i]*3,
                                                             BI_Radius[i], BI_Radius2[i], 
                                                             prm->l1)) 
                                      {
                                        local_iflag   = true;
                                        r_prim      = prm;
                                        r_dist      = _0;
                                        new_min_index   = prm->vert;
                                      }
                                  }
                              }
                          }
                      }
                    break;
                  }   /* end of switch */
                }   /* end of if */
          
              i = ii;
          
            } /* end of while */

          if(local_iflag) {
            r->prim = r_prim;
            r->dist = r_dist;
          
            break;
          }

          if( new_min_index > -1 ) {
        
            minIndex = new_min_index;
        
            r_prim = BC_prim + vert2prim[minIndex];
        
            if(r_prim->type == cPrimSphere) 
              {
                const float   *vv   = BI->Vertex + minIndex * 3;
                r_sphere0   = vv[0];
                r_sphere1   = vv[1];
                r_sphere2   = vv[2];
              }
        
            BC->interior_flag = local_iflag;   
            r->tri1 = r_tri1;
            r->tri2 = r_tri2;
            r->prim = r_prim;
            r->dist = r_dist;
            r->sphere[0] = r_sphere0;
            r->sphere[1] = r_sphere1;
            r->sphere[2] = r_sphere2;
          }       
        } /* if -- h valid */
      } /* end of if */   
    
      if(minIndex > -1) {
        if(terminal<0)
          terminal = EDGE_ALLOWANCE+1;
      }
  
      base0+=step0;
      base1+=step1;
      base2+=step2;
      /* advance through the map one block at a time -- note that this is a crappy way to walk through the map... */
    }

    BC->interior_flag = local_iflag;
    return(minIndex);
  }
}



#ifdef _PYMOL_INLINE
__inline__
#endif
int BasisHitNoShadow(BasisCallRec *BC)
{
  float   oppSq,dist,sph[3],vt[3],tri1,tri2; 
  int      a,b,c,h,*ip;
  int      excl_trans_flag;
  int      check_interior_flag;
  int      *elist, local_iflag = false;
  const float   _0   = 0.0F, _1 = 1.0F;
   
  CBasis *BI = BC->Basis;
  RayInfo *r = BC->rr;
   
  if( MapInsideXY(BI->Map, r->base, &a, &b, &c) )  {
    register int      minIndex=-1;
    int     v2p;
    int     i,ii;
    int      *xxtmp;
    int do_loop;
    int except = BC->except;
    int    n_vert = BI->NVertex, n_eElem = BI->Map->NEElem;
    const int *vert2prim = BC->vert2prim;
    const float front = BC->front;
    const float back = BC->back;
    const float excl_trans = BC->excl_trans;
    const float BasisFudge0 = BC->fudge0;
    const float BasisFudge1 = BC->fudge1;

    MapCache *cache = &BC->cache;
      
    float r_tri1=_0, r_tri2=_0, r_dist=_0; /* zero inits to suppress compiler warnings */
    float r_sphere0=_0,r_sphere1=_0,r_sphere2=_0;
    CPrimitive *r_prim = NULL;
      
    check_interior_flag   = BC->check_interior;
      
    /* assumption: always heading in the negative Z direction with our vector... */
    vt[0]         = r->base[0];
    vt[1]         = r->base[1];
    vt[2]         = r->base[2] - front;
      
    if(except >= 0)
      except   = vert2prim[except];
         
    excl_trans_flag   = (excl_trans != _0);
      
    r_dist = MAXFLOAT;

    xxtmp   = BI->Map->EHead + (a * BI->Map->D1D2) + (b * BI->Map->Dim[2]) + c;

    MapCacheReset(cache);

    elist   = BI->Map->EList;
   
    while(c >= MapBorder) {
      h   = *xxtmp;      
      if((h>0) && (h<n_eElem)) {
        ip   = elist + h;
        i   = *(ip++);
        do_loop = ((i>=0)&&(i<n_vert));
        while(do_loop) {
          ii = *(ip++);
          v2p = vert2prim[i];
          do_loop = ((ii>=0)&&(ii<n_vert));
              
          if((v2p != except) && (!MapCached(cache,v2p)))  {
            CPrimitive *prm = BC->prim + v2p;
            MapCache(cache,v2p);
                  
            switch(prm->type) {
            case cPrimTriangle:
            case cPrimCharacter:
              if(!prm->cull) {
                float   *pre   = BI->Precomp + BI->Vert2Normal[i] * 3;
                           
                if( pre[6] ) {
                  float   *vert0   = BI->Vertex + prm->vert * 3;
                              
                  float   tvec0   = vt[0] - vert0[0];
                  float   tvec1   = vt[1] - vert0[1];
                              
                  tri1      = (tvec0 * pre[4] - tvec1 * pre[3]) * pre[7];
                  tri2      = -(tvec0 * pre[1] - tvec1 * pre[0]) * pre[7];
                              
                  if( !( (tri1 < BasisFudge0) || (tri2 < BasisFudge0) || 
                         (tri1 > BasisFudge1) || ((tri1 + tri2) > BasisFudge1) ) ) {
                    dist   = (r->base[2] - (tri1*pre[2]) - (tri2*pre[5]) - vert0[2]);

                    if( (dist < r_dist) && (dist >= front) && 
                        (dist <= back) && (prm->trans != _1) )   {
                      minIndex   = prm->vert;
                      r_tri1      = tri1;
                      r_tri2      = tri2;
                      r_dist      = dist;
                    }
                  }
                }
              }
              break;
                     
            case cPrimSphere:
              oppSq = ZLineClipPoint( r->base, BI->Vertex + i*3, &dist, BI->Radius[i] );
              if(oppSq <= BI->Radius2[i])      {
                dist   = (float)(sqrt1f(dist) - sqrt1f((BI->Radius2[i]-oppSq)));

                if((dist < r_dist) && (prm->trans != _1))  {
                  if((dist >= front) && (dist <= back))  {
                    minIndex   = prm->vert;
                    r_dist      = dist;
                  }
                  else if(check_interior_flag)   {
                    if(diffsq3f(vt,BI->Vertex+i*3) < BI->Radius2[i])    {
                      local_iflag   = true;
                      r_prim      = prm;
                      r_dist    = front;
                      minIndex   = prm->vert;
                    }
                  }
                }
              }
              break;
                     
            case cPrimCylinder:
              if(ZLineToSphereCapped(r->base,BI->Vertex+i*3, 
                                     BI->Normal+BI->Vert2Normal[i]*3,
                                     BI->Radius[i], prm->l1,sph,&tri1,prm->cap1,prm->cap2,
                                     BI->Precomp + BI->Vert2Normal[i] * 3))  {
                oppSq = ZLineClipPoint(r->base,sph,&dist,BI->Radius[i]);
                if(oppSq<=BI->Radius2[i])      {
                  dist   = (float)(sqrt1f(dist)-sqrt1f((BI->Radius2[i]-oppSq)));

                  if((dist < r_dist) && (prm->trans != _1)) {
                    if((dist >= front) && (dist <= back))   {
                      if(prm->l1 > kR_SMALL4)
                        r_tri1   = tri1 / prm->l1;
                                       
                      r_sphere0   = sph[0];
                      r_sphere1   = sph[1];                              
                      r_sphere2   = sph[2];
                      minIndex   = prm->vert;
                      r_dist      = dist;
                    } else if(check_interior_flag) {
                      if(FrontToInteriorSphereCapped(vt,
                                                     BI->Vertex+i*3,
                                                     BI->Normal+BI->Vert2Normal[i]*3,
                                                     BI->Radius[i],
                                                     BI->Radius2[i],
                                                     prm->l1,
                                                     prm->cap1,
                                                     prm->cap2)) {
                        local_iflag   = true;
                        r_prim      = prm;
                        r_dist      = front;
                        minIndex   = prm->vert;
                      }
                    }
                  }
                }
              }
              break;
                     
            case cPrimSausage:
              if(ZLineToSphere(r->base,BI->Vertex+i*3,BI->Normal+BI->Vert2Normal[i]*3,
                               BI->Radius[i],prm->l1,sph,&tri1,
                               BI->Precomp + BI->Vert2Normal[i] * 3))  {
                oppSq = ZLineClipPoint(r->base,sph,&dist,BI->Radius[i]);
                if(oppSq <= BI->Radius2[i])  {
                  int   tmp_flag = false;
                              
                  dist   = (float)(sqrt1f(dist)-sqrt1f((BI->Radius2[i]-oppSq)));
                  if((dist<r_dist)  && (prm->trans != _1) ) {
                    if((dist >= front) && (dist <= back))   {
                      tmp_flag = true;
                      if(excl_trans_flag)      {
                        if( (prm->trans > _0) && (dist < excl_trans) )
                          tmp_flag = false;
                      }
                      if(tmp_flag)  {
                        if(prm->l1 > kR_SMALL4)
                          r_tri1   = tri1 / prm->l1;
                                          
                        r_sphere0   = sph[0];
                        r_sphere1   = sph[1];                              
                        r_sphere2   = sph[2];
                        minIndex      = prm->vert;
                        r_dist         = dist;
                      }
                    }
                    else if(check_interior_flag)  {
                      if(FrontToInteriorSphere(vt, BI->Vertex+i*3, BI->Normal+BI->Vert2Normal[i]*3,
                                               BI->Radius[i], BI->Radius2[i], prm->l1))  {
                        local_iflag   = true;
                        r_prim      = prm;
                        r_dist      = front;
                        minIndex   = prm->vert;
                      }
                    }
                  }
                }
              }
              break;
            }   /* end of switch */
          }   /* end of if */
               
          i = ii;
               
        } /* end of while */
      }

      /* and of course stop when we hit the edge of the map */
         
      if(local_iflag)
        break;
         
      /* we've processed all primitives associated with this voxel, 
         so if an intersection has been found which occurs in front of
         the next voxel, then we can stop */
         
      if( minIndex > -1 )  {
        int   aa,bb,cc;
            
        vt[2]   = r->base[2] - r_dist;
        MapLocus(BI->Map,vt,&aa,&bb,&cc);
        if(cc > c) 
          break;
        else
          vt[2]   = r->base[2] - front;
      }
         
      c--;
      xxtmp--;
            
    } /* end of while */
      
    if( minIndex > -1 ) {
      r_prim = BC->prim + vert2prim[minIndex];
         
      if(r_prim->type == cPrimSphere) {
        const float   *vv   = BI->Vertex + minIndex * 3;
        r_sphere0   = vv[0];
        r_sphere1   = vv[1];
        r_sphere2   = vv[2];
      }
    }
      
    BC->interior_flag = local_iflag;   
    r->tri1 = r_tri1;
    r->tri2 = r_tri2;
    r->prim = r_prim;
    r->dist = r_dist;
    r->sphere[0] = r_sphere0;
    r->sphere[1] = r_sphere1;
    r->sphere[2] = r_sphere2;
    return(minIndex);
  } /* end of if */   
  BC->interior_flag = local_iflag;
   return(-1);
}



#ifdef _PYMOL_INLINE
__inline__
#endif
int BasisHitShadow(BasisCallRec *BC)
{
  float   oppSq,dist,tri1,tri2; 
  float sph[3],vt[3];
  register int      h,*ip;
  int a,b,c;
  int      excl_trans_flag;
  int      check_interior_flag;
  int      *elist, local_iflag = false;
  const float   _0   = 0.0F;
  const float   _1   = 1.0F;
  /* local copies (eliminate these extra copies later on) */
  
  CBasis   *BI   = BC->Basis;
  RayInfo   *r   = BC->rr;
  
  if( MapInsideXY(BI->Map,r->base, &a, &b, &c) ) {
      register int      minIndex=-1;
      register int     v2p;
      register int     i,ii;
      int      *xxtmp;
      
      int n_vert = BI->NVertex, n_eElem = BI->Map->NEElem;
      register int except = BC->except;
      const int *vert2prim = BC->vert2prim;
      const int trans_shadows = BC->trans_shadows;
      const int nearest_shadow = BC->nearest_shadow;
      const float excl_trans = BC->excl_trans;
      const float BasisFudge0 = BC->fudge0;
      const float BasisFudge1 = BC->fudge1;
      const int label_shadow_mode = BC->label_shadow_mode;
      register MapCache *cache = &BC->cache;
      int *cache_cache = cache->Cache;
      int *cache_CacheLink = cache->CacheLink;
      register CPrimitive *BC_prim = BC->prim;
    
      register float r_tri1=_0, r_tri2=_0, r_dist;  /* zero inits to suppress compiler warnings */
      register float r_sphere0=_0,r_sphere1=_0,r_sphere2=_0;
      register float r_trans = _0;
      CPrimitive *r_prim = NULL;
	  
      check_interior_flag   = BC->check_interior;
      
      /* assumption: always heading in the negative Z direction with our vector... */
      vt[0]         = r->base[0];
      vt[1]         = r->base[1];
      
      if(except >= 0)
         except   = vert2prim[except];
         
      excl_trans_flag   = (excl_trans != _0);
      
      r_trans = _1;
      r_dist = MAXFLOAT;

      xxtmp   = BI->Map->EHead + (a * BI->Map->D1D2) + (b * BI->Map->Dim[2]) + c;

      MapCacheReset(cache);

      elist   = BI->Map->EList;
   
      while(c >= MapBorder)   {
         h   = *xxtmp;
         if((h>0) && (h<n_eElem)) {
           int do_loop;
           ip   = elist + h;
           i   = *(ip++);
           do_loop = ((i>=0)&&(i<n_vert));
           while(do_loop) {
             ii = *(ip++);
             v2p = vert2prim[i];
             do_loop = ((ii>=0)&&(ii<n_vert));
             if( (v2p != except) && !MapCached(cache,v2p) ) {
               register CPrimitive *prm = BC_prim + v2p;
               int prm_type;
               
               /*MapCache(cache,v2p);*/
               cache_cache[v2p] = 1;
               prm_type = prm->type;
               cache_CacheLink[v2p] = cache->CacheStart;
               cache->CacheStart = v2p; 
               
               switch(prm_type)       {
               case cPrimCharacter: /* will need special handling for character shadows */
                 if(label_shadow_mode&0x2) { /* if labels case shadows... */
                   float   *pre   = BI->Precomp + BI->Vert2Normal[i] * 3;
                   
                   if( pre[6] )
                     {
                       float   *vert0   = BI->Vertex + prm->vert * 3;
                       
                       float   tvec0   = vt[0] - vert0[0];
                       float   tvec1   = vt[1] - vert0[1];
                       
                       tri1      = (tvec0 * pre[4] - tvec1 * pre[3]) * pre[7];
                       tri2      = -(tvec0 * pre[1] - tvec1 * pre[0]) * pre[7];
                       
                       if( !( (tri1 < BasisFudge0) || 
                              (tri2 < BasisFudge0) || 
                              (tri1 > BasisFudge1) || 
                              ((tri1 + tri2) > BasisFudge1) ) )  {
                         dist   = (r->base[2] - (tri1*pre[2]) - (tri2*pre[5]) - vert0[2]);
                         
                         {
                           float fc[3];
                           float trans;

                           r->tri1 = tri1;
                           r->tri2 = tri2;
                           r->dist = dist;
                           r->prim = prm;

                           {
                             float w2;
                             w2 = 1.0F - (r->tri1 + r->tri2);
                             
                             fc[0] = (prm->c2[0]*r->tri1)+(prm->c3[0]*r->tri2)+(prm->c1[0]*w2);
                             fc[1] = (prm->c2[1]*r->tri1)+(prm->c3[1]*r->tri2)+(prm->c1[1]*w2);
                             fc[2] = (prm->c2[2]*r->tri1)+(prm->c3[2]*r->tri2)+(prm->c1[2]*w2);
                           }

                           trans = CharacterInterpolate(BI->G,prm->char_id,fc);

                           if(trans == _0)  { /* opaque? return immed. */
                             if(dist > -kR_SMALL4) {
                               if(nearest_shadow) {
                                 if(dist < r_dist) {
                                   minIndex   = prm->vert;
                                   r_tri1      = tri1;
                                   r_tri2      = tri2;
                                   r_dist      = dist;
                                   r_trans = (r->trans = trans);
                                 }
                               } else {
                                 r->prim = prm;
                                 r->trans = _0;
                                 r->dist = dist;
                                 return(1);
                               }
                             }
                           } else if(trans_shadows)  {
                             if((dist > -kR_SMALL4) &&
                                (( r_trans > trans) ||
                                 (nearest_shadow && (dist<r_dist) && ( r_trans >= trans)))) {
                               minIndex   = prm->vert;
                               r_tri1      = tri1;
                               r_tri2      = tri2;
                               r_dist      = dist;
                               r_trans = (r->trans = trans);
                             }
                           }
                         }
                       }
                     }
                 }
                 break;
                    
               case cPrimTriangle:
                 {
                   float   *pre   = BI->Precomp + BI->Vert2Normal[i] * 3;
                        
                   if( pre[6] )
                     {
                       float   *vert0   = BI->Vertex + prm->vert * 3;
                           
                       float   tvec0   = vt[0] - vert0[0];
                       float   tvec1   = vt[1] - vert0[1];
                           
                       tri1      = (tvec0 * pre[4] - tvec1 * pre[3]) * pre[7];
                       tri2      = -(tvec0 * pre[1] - tvec1 * pre[0]) * pre[7];
                           
                       if( !( (tri1 < BasisFudge0) ||
                              (tri2 < BasisFudge0) || 
                              (tri1 > BasisFudge1) || 
                              ((tri1 + tri2) > BasisFudge1) ) )
                         {
                           dist   = (r->base[2] - (tri1*pre[2]) - (tri2*pre[5]) - vert0[2]);

                           if(prm->trans == _0) {
                             if(dist > -kR_SMALL4) {
                               if(nearest_shadow) { /* do we need the nearest shadow? */
                                 if(dist < r_dist) {
                                   minIndex   = prm->vert;
                                   r_tri1      = tri1;
                                   r_tri2      = tri2;
                                   r_dist = dist;
                                   r_trans = (r->trans = prm->trans);
                                 }
                               } else {
                                 r->prim = prm;
                                 r->trans = _0;
                                 r->dist = dist;
                                 return(1);
                               }
                             }
                           } else if(trans_shadows) {
                             if((dist > -kR_SMALL4) &&
                                (( r_trans > prm->trans) ||
                                 (nearest_shadow && (dist<r_dist) && ( r_trans >= prm->trans)))) {
                               minIndex   = prm->vert;
                               r_tri1      = tri1;
                               r_tri2      = tri2;
                               r_dist = dist;
                               r_trans = (r->trans = prm->trans);
                             }
                           }
                         }
                     }
                 }
                 break;
                     
               case cPrimSphere:
                      
                 oppSq = ZLineClipPoint( r->base, BI->Vertex + i*3, &dist, BI->Radius[i] );
                 if(oppSq <= BI->Radius2[i])
                   {
                     dist   = (float)(sqrt1f(dist) - sqrt1f((BI->Radius2[i]-oppSq)));

                     if(prm->trans == _0) {
                       if(dist > -kR_SMALL4) {
                         if(nearest_shadow) {
                           if(dist < r_dist) {
                             minIndex   = prm->vert;
                             r_dist = dist;
                             r_trans = (r->trans = prm->trans);
                           }
                         } else {
                           r->prim = prm;
                           r->trans = prm->trans;
                           r->dist = dist;
                           return(1);
                         }
                       }
                     } else if(trans_shadows) {
                       if((dist > -kR_SMALL4) && 
                          (( r_trans > prm->trans) ||
                           (nearest_shadow && (dist<r_dist) && ( r_trans >= prm->trans)))) {
                         minIndex   = prm->vert;
                         r_dist = dist;
                         r_trans = (r->trans = prm->trans);
                       }
                     }
                   }
                 break;
                        
               case cPrimCylinder:
                 if(ZLineToSphereCapped(r->base,BI->Vertex+i*3, 
                                        BI->Normal+BI->Vert2Normal[i]*3,
                                        BI->Radius[i], prm->l1,sph,&tri1,prm->cap1,prm->cap2,
                                        BI->Precomp + BI->Vert2Normal[i] * 3))
                   {
                     oppSq = ZLineClipPoint(r->base,sph,&dist,BI->Radius[i]);
                     if(oppSq <= BI->Radius2[i])
                       {
                         dist=(float)(sqrt1f(dist)-sqrt1f((BI->Radius2[i]-oppSq)));

                         if(prm->trans == _0) {
                           if(dist > -kR_SMALL4) {
                             if(nearest_shadow) {
                               if(dist < r_dist) {
                                 if(prm->l1 > kR_SMALL4)
                                   r_tri1   = tri1 / prm->l1;
                                 r_sphere0   = sph[0];
                                 r_sphere1   = sph[1];                              
                                 r_sphere2   = sph[2];
                                 minIndex   = prm->vert;
                                 r->trans = prm->trans;
                                 r_dist = dist;
                                 r_trans = (r->trans = prm->trans);}
                             } else {
                               r->prim = prm;
                               r->trans = prm->trans;
                               r->dist = dist;
                               return(1);
                             }
                           }
                         } else if(trans_shadows) {
                           if((dist > -kR_SMALL4) && 
                              (( r_trans > prm->trans) ||
                               (nearest_shadow && (dist<r_dist) && ( r_trans >= prm->trans)))) {
                             if(prm->l1 > kR_SMALL4)
                               r_tri1   = tri1 / prm->l1;
                             r_sphere0   = sph[0];
                             r_sphere1   = sph[1];                              
                             r_sphere2   = sph[2];
                             minIndex   = prm->vert;
                             r->trans = prm->trans;
                             r_dist = dist;
                             r_trans = (r->trans = prm->trans);
                           }
                         }
                       }
                   }
                 break;
                     
               case cPrimSausage:
                 if(ZLineToSphere(r->base,BI->Vertex+i*3,BI->Normal+BI->Vert2Normal[i]*3,
                                  BI->Radius[i],prm->l1,sph,&tri1,
                                  BI->Precomp + BI->Vert2Normal[i] * 3))
                   {
                     oppSq = ZLineClipPoint(r->base,sph,&dist,BI->Radius[i]);
                     if(oppSq <= BI->Radius2[i])
                       {
                         dist   = (float)(sqrt1f(dist) - sqrt1f((BI->Radius2[i]-oppSq)));
                              
                         if(prm->trans == _0) {
                           if(dist > -kR_SMALL4) {
                             if(nearest_shadow) {
                               if(dist < r_dist) {
                                 if(prm->l1 > kR_SMALL4)
                                   r_tri1   = tri1 / prm->l1;
                                 r_sphere0   = sph[0];
                                 r_sphere1   = sph[1];                              
                                 r_sphere2   = sph[2];
                                 minIndex   = prm->vert;
                                 r_dist = dist;
                                 r_trans = (r->trans = prm->trans);
                               }
                             } else {
                               r->prim = prm;
                               r->trans = prm->trans;
                               r->dist = dist;
                               return(1);
                             }
                           }
                         } else if(trans_shadows) {
                           if((dist > -kR_SMALL4) && 
                              (( r_trans > prm->trans) ||
                               (nearest_shadow && (dist<r_dist) && ( r_trans >= prm->trans)))) {
                             if(prm->l1 > kR_SMALL4)
                               r_tri1   = tri1 / prm->l1;
                                  
                             r_sphere0   = sph[0];
                             r_sphere1   = sph[1];                              
                             r_sphere2   = sph[2];
                             minIndex   = prm->vert;
                             r_dist = dist;
                             r_trans = (r->trans = prm->trans);
                           }
                         }
                       }
                   }
                 break;
               }   /* end of switch */
             }   /* end of if */

             i = ii;
           } /* end of while */
         }

         /* and of course stop when we hit the edge of the map */
         
         if(local_iflag)
           break;
         
         
         /* we've processed all primitives associated with this voxel, 
            so if an intersection has been found which occurs in front of
            the next voxel, then we can stop */
         
         /* this optimization invalid for transparent surfaces 

         if( minIndex > -1 ) 
         {
         int   aa,bb,cc;
                    
         vt[2]   = r->base[2] - r_dist;
         MapLocus(BI->Map,vt,&aa,&bb,&cc);
         if(cc > c) 
         break;
         }
         */
         
         c--;
         xxtmp--;
            
      } /* end of while */
      
      if( minIndex > -1 )  {
        r_prim = BC->prim + vert2prim[minIndex];
         
        if(r_prim->type == cPrimSphere)  {
          const float   *vv   = BI->Vertex + minIndex * 3;
          r_sphere0   = vv[0];
          r_sphere1   = vv[1];
          r_sphere2   = vv[2];
        }
      }
      
      BC->interior_flag = local_iflag;   
      r->tri1 = r_tri1;
      r->tri2 = r_tri2;
      r->prim = r_prim;
      r->dist = r_dist;
      r->sphere[0] = r_sphere0;
      r->sphere[1] = r_sphere1;
      r->sphere[2] = r_sphere2;
      return(minIndex);
  } /* end of if */   
  BC->interior_flag = local_iflag;
  return(-1);
}


#if 0
/*========================================================================*/
void BasisOptimizeMap(CBasis *I,float *vertex,int n,int *vert2prim);
void BasisOptimizeMap(CBasis *I,float *vertex,int n,int *vert2prim)
{
   int      *ip,*op;
   int      k;
   int     v2p;
   int     i,ii;
   MapCache cache;
   int a,b,c,aa,bb,cc;
   int h;
   int *xxtmp,*elist;
   MapCacheInit(&cache,I->Map);
   int cnt = 0,tcnt = 0;
   elist   = I->Map->EList;

     
#if 0
   for(k=0;k<n;k++) {
     if (MapExclLocus(I->Map,vertex + 3*k,&aa,&bb,&cc)) {
       for (a=aa-1;a<=aa+1;a++) {
         for (b=bb-1;b<=bb+1;b++) {
           xxtmp   = I->Map->EHead + (a * I->Map->D1D2) + (b * I->Map->Dim[2]);
           for(c=cc-1;c<=cc+1;c++) {
#else
{
  {
    for( a = I->Map->iMin[0]; a<=I->Map->iMax[0]; a++) {
      for(b = I->Map->iMin[1]; b<=I->Map->iMax[1]; b++) {
        xxtmp   = I->Map->EHead + (a * I->Map->D1D2) + (b * I->Map->Dim[2]);
        for( c = I->Map->iMin[2]; c <= I->Map->iMax[2]; c++)
          {
#endif

             h   = *(xxtmp + c);
             cnt = 0;
             tcnt = 0;
             if(h)
               {
                 op = ip = elist + h;
                 MapCacheReset(&cache);
                 
                 if(ip) {
                   i   = *(ip++);
                   while(i >= 0) 
                     {
                       tcnt++;
                       v2p = vert2prim[i];
                       ii = *(ip++);
                       /*    printf("%d\n",v2p);*/
                       if( !MapCached(&cache,v2p))
                         {
                           *(op++) = i; /* copy/store */
                           MapCache(&cache,v2p);
                         }   /* end of if */
                       else {
                         /* remove this vertex from the linked list -- we don't need it more than once */
                         cnt++;
                       }
                       i = ii;
                     } /* end of while */
                   *op = -1; /* terminate new list */
                 }
                 /*   printf("%6d %6d\n",tcnt,cnt);*/
               }
           
           }
         }
       }
     }
   }

}
#endif

/*========================================================================*/
void BasisMakeMap(CBasis *I,int *vert2prim,CPrimitive *prim,float *volume,
                  int group_id,int block_base,
                  int perspective,float front,float size_hint)
{
  register float *v;
  float ll;
  CPrimitive *prm;
  register int i;
  register int *tempRef;
  int n,h,q,x,y,z,j,k,l,e;
  int extra_vert = 0;
  float p[3],dd[3],*d1,*d2,vd[3],cx[3],cy[3];
  float *tempVertex;
  float xs,ys;
  int remapMode = true; /* remap mode means that some objects will span more
                         * than one voxel, so we have to worry about populating
                         * those voxels and also about eliminating duplicates 
                         * when traversing the neighbor lists */
  float min[3],max[3],extent[6];
  float sep;
  float diagonal[3];
  float l1,l2;
  float bh,ch;
  int n_voxel;
   
  const float _0   = 0.0;
  const float _p5   = 0.5;
   
  PRINTFD(I->G,FB_Ray)
    " BasisMakeMap: I->NVertex %d [(%8.3f, %8.3f, %8.3f),...]\n",I->NVertex,
    I->Vertex[0],I->Vertex[1],I->Vertex[2]
    ENDFD;
   
  sep = I->MinVoxel;
  if(sep == _0) {
    remapMode   = false;
    sep         = I->MaxRadius; /* also will imply no remapping of vertices */
  }
  /* we need to get a sense of the actual size in order to avoid sep being too small */
   
  v   = I->Vertex;
   
  min[0] = max[0] = v[0];
  min[1] = max[1] = v[1];
  min[2] = max[2] = v[2];
   
  v   += 3;

  {
    register int a;
    for(a = 1; a < I->NVertex; a++)
      {
        if(min[0] > v[0])      min[0]   = v[0];
        if(max[0] < v[0])      max[0]   = v[0];
        
        if(min[1] > v[1])      min[1]   = v[1];
        if(max[1] < v[1])      max[1]   = v[1];
        
        if(min[2] > v[2])      min[2]   = v[2];
        if(max[2] < v[2])      max[2]   = v[2];
        
        v   += 3;
      }
  }
   
  if(volume)
    {

      /*
        if(min[0] > volume[0])      min[0]   = volume[0];
        if(max[0] < volume[1])      max[0]   = volume[1];
        if(min[1] > volume[2])      min[1]   = volume[2];
        if(max[1] < volume[3])      max[1]   = volume[3];
        if(min[2] > (-volume[5]))   min[2]   = (-volume[5]);
        if(max[2] < (-volume[4]))   max[2]   = (-volume[4]);
      */

      if(min[0] < volume[0])      min[0]   = volume[0];
      if(max[0] > volume[1])      max[0]   = volume[1];
      if(min[1] < volume[2])      min[1]   = volume[2];
      if(max[1] > volume[3])      max[1]   = volume[3];
      if(min[2] > (-volume[5]))   min[2]   = (-volume[5]);
      if(max[2] < (-volume[4]))   max[2]   = (-volume[4]);

      PRINTFB(I->G,FB_Ray,FB_Debugging)
        " BasisMakeMap: (%8.3f,%8.3f),(%8.3f,%8.3f),(%8.3f,%8.3f)\n",
        volume[0],volume[1],volume[2],volume[3],volume[4],volume[5]
        ENDFB(I->G);
    }
   
  /* don't break up space unnecessarily if we only have a few vertices... */

  if(I->NVertex) {
    l1 = (float)fabs(max[0]-min[0]);
    l2 = (float)fabs(max[1]-min[1]);
    
    if(l1 < l2) l1 = l2;  
    
    l2 = (float)fabs(max[2]-min[2]);
    
    if(l1 < l2) l1 = l2;
    
    if(l1 < kR_SMALL4) l1 = 100.0;
    
    if(I->NVertex < (l1/sep))
      sep   = (l1/I->NVertex);
  }
  
  if(Feedback(I->G,FB_Ray,FB_Debugging)) {
    dump3f(min," BasisMakeMap: min");
    dump3f(max," BasisMakeMap: max");
    dump3f(I->Vertex," BasisMakeMap: I->Vertex");
    fflush(stdout);
  }

  sep = MapGetSeparation(I->G,sep,max,min,diagonal); /* this needs to be a minimum 
                                                      * estimate of the actual value */
  /* here we have to carry out a complicated work-around in order to
   * efficiently encode our primitives into the map in a way that doesn't
   * require expanding the map cutoff to the size of the largest object*/
  if(remapMode) {
    register int a,b,c;

    if(sep<size_hint) /* this keeps us from wasting time & memory on unnecessary subdivision */
      sep = size_hint;

    for(a = 0; a < I->NVertex; a++) {
      prm   = prim + vert2prim[a];
      
      switch(prm->type) {
      case cPrimTriangle:
      case cPrimCharacter:
        if(a == prm->vert) { /* only do this calculation for one of the three vertices */
          l1   = (float)length3f(I->Precomp+I->Vert2Normal[a]*3);
          l2   = (float)length3f(I->Precomp+I->Vert2Normal[a]*3+3);
          if((l1>=sep)||(l2>=sep)) {
            b   = (int)ceil(l1/sep)+1;
            c   = (int)ceil(l2/sep)+1;
            extra_vert += 4*b*c;
          }
        }
        break;
        
      case cPrimCylinder:
      case cPrimSausage:
        if((prm->l1+2*prm->r1)>=sep) {
          q = ((int)(2*(floor(prm->r1/sep)+1)))+1;
          q = q * q * ((int)ceil((prm->l1+2*prm->r1)/sep)+2);
          extra_vert+= q;
        }
        break;
        
      case cPrimSphere:
        if(prm->r1>=sep) {
          b = (int)(2*floor(prm->r1/sep)+1);
          extra_vert   += (b*b*b);
        }
        break;
      }
    }   /* for */
    
    extra_vert   += I->NVertex;
    tempVertex   = CacheAlloc(I->G,float,extra_vert*3,group_id,cCache_basis_tempVertex);
    tempRef      = CacheAlloc(I->G,int,extra_vert,group_id,cCache_basis_tempRef); 
    
    ErrChkPtr(I->G,tempVertex); /* can happen if extra vert is unreasonable */
    ErrChkPtr(I->G,tempRef);
    
    /* lower indexes->flags, top is ref->lower index*/

    {
      register float *vv,*d;    
      n   = I->NVertex;
      
      v   = tempVertex;
      vv   = I->Vertex;
      
      memcpy( v, vv, n * sizeof(float) * 3 );
      vv   += n * 3;
      v   += n * 3;
      
      for(a = 0; a < I->NVertex; a++) {
      
        prm   = prim + vert2prim[a];
      
        switch(prm->type)  {
        case cPrimTriangle:
        case cPrimCharacter:
          if(a == prm->vert) {
            /* only do this calculation for one of the three vertices */
            d1   = I->Precomp + I->Vert2Normal[a]*3;
            d2   = I->Precomp + I->Vert2Normal[a]*3+3;
            vv   = I->Vertex + a*3;
            l1   = (float)length3f(d1);
            l2   = (float)length3f(d2);
            if((l1>=sep)||(l2>=sep)) {
              b   = (int)floor(l1/sep)+1;
              c   = (int)floor(l2/sep)+1;
              extra_vert += b*c;
              bh   = (float)(b/2)+1;
              ch   = (float)(c/2)+1;
            
              for(x = 0; x < bh; x++) {
                const float   xb   = (float)x / b;
              
                for(y = 0; y < ch; y++)  {
                  const float   yc   = (float)y / c;
                
                  *(v++)   = vv[0] + (d1[0] * xb) + (d2[0] * yc);
                  *(v++)   = vv[1] + (d1[1] * xb) + (d2[1] * yc);
                  *(v++)   = vv[2] + (d1[2] * xb) + (d2[2] * yc);
                
                  tempRef[n++]   = a;
                }
              }
            
              for(x = 0; x < bh; x++)                {
                const float   xb   = (float)x / b;
              
                for(y = 0; y < ch; y++) {
                  const float   yc   = (float)y/c;
                
                  if( (xb + yc) < _p5 ) {
                    *(v++)   = vv[0] + d1[0] * (_p5 + xb) + (d2[0]*yc);
                    *(v++)   = vv[1] + d1[1] * (_p5 + xb) + (d2[1]*yc);
                    *(v++)   = vv[2] + d1[2] * (_p5 + xb) + (d2[2]*yc);
                  
                    tempRef[n++]   = a; 
                  
                    *(v++)   = vv[0] + (d1[0]*xb) + d2[0]*(_p5 + yc);
                    *(v++)   = vv[1] + (d1[1]*xb) + d2[1]*(_p5 + yc);
                    *(v++)   = vv[2] + (d1[2]*xb) + d2[2]*(_p5 + yc);
                  
                    tempRef[n++] = a;
                  }
                }
              }
            }
          }   /* if */
          break;
        case cPrimCylinder:
        case cPrimSausage:
            
          ll   = prm->l1+2*prm->r1;

          if(ll>=sep) {
            d   = I->Normal+3*I->Vert2Normal[a];
            vv   = I->Vertex+a*3;
            get_system1f3f(d,cx,cy); /* creates an orthogonal system about d */
                 
            p[0]   = vv[0]-d[0]*prm->r1;
            p[1]   = vv[1]-d[1]*prm->r1;
            p[2]   = vv[2]-d[2]*prm->r1;
            dd[0]   = d[0]*sep;
            dd[1]   = d[1]*sep;
            dd[2]   = d[2]*sep;
                 
            q   = (int)floor(prm->r1 / sep) + 1;
          
            while(1) {
              vd[0]   = (p[0] += dd[0]);
              vd[1]   = (p[1] += dd[1]);
              vd[2]   = (p[2] += dd[2]);
            
              for(x = -q; x <= q; x++) {
                for(y = -q; y <= q; y++) {
                  xs      = x*sep;
                  ys      = y*sep;
                  *(v++)   = vd[0] + xs*cx[0] + ys*cy[0];
                  *(v++)   = vd[1] + xs*cx[1] + ys*cy[1];
                  *(v++)   = vd[2] + xs*cx[2] + ys*cy[2];
                
                  tempRef[n++]   = a;
                }
              }
            
              if(ll <= _0)
                break;
              ll   -= sep;
            }
          }
          break;
          
        case cPrimSphere:
          if(prm->r1>=sep) {
            q   = (int)floor(prm->r1 / sep);
            vv   = I->Vertex + a*3;
                
            for(x = -q; x <= q; x++)  {
              for(y = -q; y <= q; y++)  {
                for( z = -q; z <= q; z++)  {
                  *(v++)   = vv[0] + x * sep;
                  *(v++)   = vv[1] + y * sep;
                  *(v++)   = vv[2] + z * sep;
                
                  tempRef[n++]   = a;
                }
              }
            }
          }
          break;
        }   /* end of switch */
      }
    }
    if(n > extra_vert) {
      printf("BasisMakeMap: %d>%d\n",n,extra_vert);
      ErrFatal(I->G,"BasisMakeMap","used too many extra vertices (this indicates a bug)...\n");
    }
    PRINTFB(I->G,FB_Ray,FB_Blather)
      " BasisMakeMap: %d total vertices\n",n
      ENDFB(I->G);
    
    if(volume) {
      v   = tempVertex;
      
      min[0]   = max[0]   = v[0];
      min[1]   = max[1]   = v[1];
      min[2]   = max[2]   = v[2];
      
      v += 3;
      
      if(Feedback(I->G,FB_Ray,FB_Debugging)) {
        dump3f(min," BasisMakeMap: remapped min");
        dump3f(max," BasisMakeMap: remapped max");
        fflush(stdout);
      }
      
      for(a = 1; a < n; a++) {
        if(min[0] > v[0])   min[0]   = v[0];
        if(max[0] < v[0])   max[0]   = v[0];
        
        if(min[1] > v[1])   min[1]   = v[1];
        if(max[1] < v[1])   max[1]   = v[1];
        
        if(min[2] > v[2])   min[2]   = v[2];
        if(max[2] < v[2])   max[2]   = v[2];
        v   += 3;
      }
      
      if(Feedback(I->G,FB_Ray,FB_Debugging)) {
        dump3f(min," BasisMakeMap: remapped min");
        dump3f(max," BasisMakeMap: remapped max");
        fflush(stdout);
      }
      if(min[0] < volume[0])      min[0]   = volume[0];
      if(max[0] > volume[1])      max[0]   = volume[1];
      if(min[1] < volume[2])      min[1]   = volume[2];
      if(max[1] > volume[3])      max[1]   = volume[3];
      
      if(min[2] < (-volume[5]))   min[2]   = (-volume[5]);
      if(max[2] > (-volume[4]))   max[2]   = (-volume[4]);
      
      extent[0]   = min[0];
      extent[1]   = max[0];
      extent[2]   = min[1];
      extent[3]   = max[1];
      extent[4]   = min[2];
      extent[5]   = max[2];
      PRINTFB(I->G,FB_Ray,FB_Blather)
        " BasisMakeMap: Extent [%8.2f %8.2f] [%8.2f %8.2f] [%8.2f %8.2f]\n",
        extent[0],extent[1],extent[2],extent[3],extent[4],extent[5]
        ENDFB(I->G);
      I->Map   = MapNewCached(I->G,-sep,tempVertex,n,extent,group_id,block_base);
      
    } else {
      I->Map   = MapNewCached(I->G,sep,tempVertex,n,NULL,group_id,block_base);
    }
    
    n_voxel = I->Map->Dim[0]*I->Map->Dim[1]*I->Map->Dim[2];
    
    if(perspective) {
      MapSetupExpressPerp(I->Map,tempVertex,front,n,true);
    } else if(n_voxel < (3*n)) {
      MapSetupExpressXY(I->Map,n,true);      
    } else {
      MapSetupExpressXYVert(I->Map,tempVertex,n,true);
    }
    
    {
      register MapType *map = I->Map;
      register int *sp,*ip,*ip0,ii;
      register int *elist = map->EList, *ehead = map->EHead;
      register int *elist_new = elist, *ehead_new = ehead;
      register int newelem = 0, neelem = -map->NEElem;
      register int i_nVertex = I->NVertex;
      const int iMin0=map->iMin[0];
      const int iMin1=map->iMin[1];
      const int iMin2=map->iMin[2];
      const int iMax0=map->iMax[0];
      const int iMax1=map->iMax[1];
      const int iMax2=map->iMax[2];
      
#if 0
      elist_new = VLACacheAlloc(I->G,int,map->NEElem,group_id,block_base + cCache_map_elist_new_offset);
      ehead_new = CacheCalloc(I->G,int,(map->Dim[0]*map->Dim[1]*map->Dim[2]),
                                            group_id,block_base + cCache_map_ehead_new_offset);
#endif
      /* now do a filter-reassignment pass to remap fake vertices
         to the original line vertex while deleting duplicate entries */
      
      memset( tempRef, 0, sizeof(int) * i_nVertex );

      if(n_voxel < (3*n)) { /* faster to traverse the entire map */
        int   *start;
        for(a = iMin0; a <= iMax0; a++) {
          for(b = iMin1; b <= iMax1; b++) {
            for(c = iMin2; c <= iMax2; c++) {
              start = MapEStart(map,a,b,c);
              h = *start;
              if((h<0)&&(h>=neelem)) {
                sp = elist-h;
                *(start) = -h; /* flip sign */
                i = *(sp++);
                if(ehead_new!=ehead) {
                  ehead_new[(start - ehead)-1] = newelem;
                  ip = ip0 = (elist_new+newelem);
                } else {
                  ip = ip0 = (sp-1);
                }
                  
                while(i>=0) {
                  register int ii = *(sp++);
                  if(i >= i_nVertex) /* reference -- remap */
                    i = tempRef[i];
                  if(!tempRef[i]) { /*eliminate duplicates */
                    tempRef[i]   = 1;
                    *(ip++) = i;
                  } 
                  i = ii;
                }
              
                *(ip++)   = -1; /* terminate list */
                newelem+= (ip-ip0);

                /* now reset flags efficiently */
                i = *(ip0++);
                while(i >= 0)  { /* unroll */
                  ii = *(ip0++);
                  tempRef[i]   = 0;
                  if((i=ii)>=0) {
                    ii = *(ip0++);
                    tempRef[i]   = 0;
                    if((i=ii)>=0) {
                      ii = *(ip0++);
                      tempRef[i]   = 0;
                      i=ii;
                    }
                  }                            
                }
              }
            }   /* for c */
          }   /* for b */
        }   /* for a */
      } else {
        int      *start;
        int jm1,jp1,km1,kp1,lm1,lp1;
        MapType   *map   = I->Map;
      
        v         = tempVertex;
      
        for(e = 0; e < n; e++) { 
        
          MapLocus(map,v, &j, &k, &l);
          jm1 = j-1; 
          jp1 = j+1;
          km1 = k-1;
          kp1 = k+1;
          lm1 = l-1;
          lp1 = l+1;
          
          if(perspective) { /* for normal maps */
            
            register int   *iPtr1   = map->EHead + ((j-1) * map->D1D2) + ((k-1) * map->Dim[2]) + (l-1);
            for(a = jm1; a <= jp1; a++) {
              register int   *iPtr2   = iPtr1;
              if( (a >= iMin0) && (a <= iMax0) ) {
                for(b = km1; b <= kp1; b++) {
                  register int   *iPtr3   = iPtr2;
                  if( (b >= iMin1) && (b <= iMax1) ) {
                    for(c = lm1; c <= lp1; c++) {
                      if( (c >= iMin2) && (c <= iMax2) ) {
                        
                        start   = iPtr3;
                        /*start   = MapEStart(map,a,b,c);*/
                        h      = *start;
                        if((h<0)&&(h>=neelem))  {
                          sp = elist - h;
                          (*start) = -h; /* no repeat visits */
                          i  = *(sp++);
                          if(ehead_new!=ehead) {
                            ehead_new[(start - ehead)] = newelem;
                            ip = ip0 = (elist_new+newelem);
                          } else {
                            ip = ip0 = (sp-1);
                          }
                          
                          while(i >= 0)  {
                            register int ii = *(sp++);
                            if(i >= i_nVertex)
                              i = tempRef[i];
                            if(!tempRef[i]) { /*eliminate duplicates */
                              tempRef[i]   = 1;
                              *(ip++) = i;
                            }
                            i = ii;
                          }
                          
                          *(ip++)   = -1; /* terminate list */
                          newelem+= (ip-ip0);
                          
                          /* now reset flags efficiently */
                          i = *(ip0++);
                          
                          while(i >= 0)  { /* unroll */
                            ii = *(ip0++);
                            tempRef[i]   = 0;
                            if((i=ii)>=0) {
                              ii = *(ip0++);
                              tempRef[i] = 0;
                              if((i=ii)>=0) {
                                ii = *(ip0++);
                                tempRef[i] = 0;
                                i=ii;
                              }
                            }                            
                          }
                        }   /* h > 0 */
                      }
                      iPtr3   ++;
                    }
                  }
                  iPtr2   += map->Dim[2];
                }
              }
              iPtr1   += map->D1D2;
            }
          } else { /* for XY maps... */
          
            c = l;
            {
              register int   *iPtr1   = ehead + ((j-1) * map->D1D2) + ((k-1) * map->Dim[2]) + c;
              if((c >= iMin2) && (c <= iMax2))  {
                
                for(a = jm1; a <= jp1; a++) {
                  register int   *iPtr2   = iPtr1;
                  if( (a >= iMin0) && (a <= iMax0) ) {
                    
                    for(b = km1; b <= kp1; b++)  {
                      if( (b >= iMin1) && (b <= iMax1) ) {
                        start   = iPtr2;
                        /*start   = MapEStart(map,a,b,c);*/
                        h      = *start;
                        if((h<0)&&(h>=neelem)) {

                          sp   = elist - h;
                          (*start) = -h; /* no repeat visits */
                          i = *(sp++);
                          if(ehead_new!=ehead) {
                            ehead_new[(start - ehead)] = newelem;
                            ip = ip0 = (elist_new+newelem);
                          } else {
                            ip = ip0 = sp-1;
                          }
                          
                          while(i >= 0)  {
                            register int ii = *(sp++);
                            
                            if(i >= i_nVertex)
                              i = tempRef[i];
                            
                            if(!tempRef[i]) { /*eliminate duplicates */
                              tempRef[i]  = 1;
                              *(ip++) = i;
                            }
                            i = ii;
                          }
                          
                          *(ip++)   = -1; /* terminate list */
                          newelem+= (ip-ip0);
                          
                          /* now reset flags efficiently */
                          i = *(ip0++);
                          while(i >= 0)  { /* unroll */
                            ii = *(ip0++);
                            tempRef[i]   = 0;
                            if((i=ii)>=0) {
                              ii = *(ip0++);
                              tempRef[i]   = 0;
                              if((i=ii)>=0) {
                                ii = *(ip0++);
                                tempRef[i]   = 0;
                                i = ii;
                              }
                            }                            
                          }
                        }   /* h > 0 */
                      }
                      iPtr2   += map->Dim[2];
                    }   /* for b */
                  }
                  iPtr1   += map->D1D2;
                }   /* for a */
              }
            }
          }
          v += 3;   /* happens for EVERY e! */
        }   /* for e */
      }
      if(ehead!=ehead_new) {
        CacheFreeP(I->G,map->EHead,group_id,block_base + cCache_map_ehead_offset,false);
        VLACacheFreeP(I->G,map->EList,group_id,block_base + cCache_map_elist_offset,false);    
        map->EList = elist_new;
        map->EHead = ehead_new;
        map->NEElem = newelem;
        MemoryCacheReplaceBlock(I->G, group_id, block_base + cCache_map_ehead_new_offset,
                                block_base + cCache_map_ehead_offset);
        MemoryCacheReplaceBlock(I->G, group_id, block_base + cCache_map_elist_new_offset, 
                                block_base + cCache_map_elist_offset);
        VLACacheSize(G,map->EList,int,map->NEElem,group_id,block_base + cCache_map_elist_offset);
      }
    }
      
    CacheFreeP(I->G,tempVertex,group_id,cCache_basis_tempVertex,false);
    CacheFreeP(I->G,tempRef,group_id,cCache_basis_tempRef,false);
      
  }  else    {
    /* simple sphere mode */
    I->Map   = MapNewCached(I->G,-sep,I->Vertex,I->NVertex,NULL,group_id,block_base);
    if(perspective) {
      MapSetupExpressPerp(I->Map,I->Vertex,front,I->NVertex,false);
    } else {
      MapSetupExpressXYVert(I->Map,I->Vertex,I->NVertex,false);
    }
  }
}

/*========================================================================*/
void BasisInit(PyMOLGlobals *G,CBasis *I,int group_id)
{
  I->G = G;
  I->Vertex = VLACacheAlloc(I->G,float,1,group_id,cCache_basis_vertex);
  I->Radius = VLACacheAlloc(I->G,float,1,group_id,cCache_basis_radius);
  I->Radius2 = VLACacheAlloc(I->G,float,1,group_id,cCache_basis_radius2);
  I->Normal = VLACacheAlloc(I->G,float,1,group_id,cCache_basis_normal);
  I->Vert2Normal = VLACacheAlloc(I->G,int,1,group_id,cCache_basis_vert2normal);
  I->Precomp = VLACacheAlloc(I->G,float,1,group_id,cCache_basis_precomp);
  I->Map=NULL;
  I->NVertex=0;
  I->NNormal=0;
}
/*========================================================================*/
void BasisFinish(CBasis *I,int group_id)
{
  if(I->Map) {
    MapFree(I->Map);
    I->Map=NULL;
  }  
  VLACacheFreeP(I->G,I->Radius2,group_id,cCache_basis_radius2,false);
  VLACacheFreeP(I->G,I->Radius,group_id,cCache_basis_radius,false);
  VLACacheFreeP(I->G,I->Vertex,group_id,cCache_basis_vertex,false);
  VLACacheFreeP(I->G,I->Vert2Normal,group_id,cCache_basis_vert2normal,false);
  VLACacheFreeP(I->G,I->Normal,group_id,cCache_basis_normal,false);
  VLACacheFreeP(I->G,I->Precomp,group_id,cCache_basis_precomp,false);
  I->Vertex=NULL;
}


/*========================================================================*/


void BasisTrianglePrecompute(float *v0,float *v1,float *v2,float *pre)
{
   float det;
   
   subtract3f(v1,v0,pre);
   subtract3f(v2,v0,pre+3);
   
   det = pre[0]*pre[4] - pre[1]*pre[3];
   
   if(fabs(det) < EPSILON)
      *(pre+6) = 0.0F;
   else 
   {
      *(pre+6) = 1.0F;
      *(pre+7) = 1.0F/det;
   }
}

void BasisTrianglePrecomputePerspective(float *v0,float *v1,float *v2,float *pre)
{
   subtract3f(v1,v0,pre);
   subtract3f(v2,v0,pre+3);
}


void BasisCylinderSausagePrecompute(float *dir,float *pre)
{
  float ln  = (float)(1.0f / sqrt1f(dir[1]*dir[1]+dir[0]*dir[0]));
  pre[0] = dir[1] * ln;
  pre[1] = -dir[0] * ln;
}

#else
typedef int this_file_is_no_longer_empty;

#endif