
/* {{{ Copyright etc. */

/*  libss - collection of generic image processing functions

    Stephen Smith, FMRIB Image Analysis Group

    Copyright (C) 1999-2000 University of Oxford  */

/*  Part of FSL - FMRIB's Software Library
    WWW:      http://www.fmrib.ox.ac.uk/fsl
    Email:    fsl@@fmrib.ox.ac.uk
    
    Developed at FMRIB (Oxford Centre for Functional Magnetic Resonance
    Imaging of the Brain), Department of Clinical Neurology, Oxford
    University, Oxford, UK
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at
    your option) any later version.
    
    This program is distributed in the hope that it will be useful, but in
    order that the University as a charitable foundation protects its
    assets for the benefit of its educational and research purposes, the
    University makes clear that no condition is made or to be implied, nor
    is any warranty given or to be implied, as to the accuracy of FSL, or
    that it will be suitable for any particular purpose or for use under
    any specific conditions.  Furthermore, the University disclaims all
    responsibility for the use which is made of FSL.  See the GNU General
    Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
    USA */

/* }}} */

#include "libss.h"

/* {{{ 1D median, mean */

/* {{{ median */

#define CH(a,b) { FDT d=(a);(a)=(b);(b)=d; }

FDT median(double index, FDT *in, int inlen)
{
  unsigned int e=inlen-1,f,g=(unsigned int)(((double)inlen)*index),h,i=0,j;

  while (1) {
    if (e<i+2)
      {
	if ( (e==i+1) && (in[e]<in[i]) ) CH(in[i],in[e]);
	return in[g];
      }
    else
      {
	FDT c;

	j=(i+e) >> 1;
	CH(in[j],in[i+1]);
	if (in[i]>in[e])   CH(in[i],in[e]);
	if (in[i+1]>in[e]) CH(in[i+1],in[e]);
	if (in[i]>in[i+1]) CH(in[i],in[i+1]);
	f=i+1; h=e; c=in[i+1];
	while (1) {
	  do f++; while (in[f] < c);
	  do h--; while (in[h] > c);
	  if (h < f) break;
	  CH(in[f],in[h]);
	}
	in[i+1]=in[h]; in[h]=c;
	if (h >= g) e=h-1;
	if (h <= g) i=f;
      }
  }
}

#undef CH

/* }}} */
/* {{{ dmedian */

#define CH(a,b) { double d=(a);(a)=(b);(b)=d; }

double dmedian(double index, double *in, int inlen)
{
  unsigned int e=inlen-1,f,g=(unsigned int)(((double)inlen)*index),h,i=0,j;

  while (1) {
    if (e<i+2)
      {
	if ( (e==i+1) && (in[e]<in[i]) ) CH(in[i],in[e]);
	return in[g];
      }
    else
      {
	double c;

	j=(i+e) >> 1;
	CH(in[j],in[i+1]);
	if (in[i]>in[e])   CH(in[i],in[e]);
	if (in[i+1]>in[e]) CH(in[i+1],in[e]);
	if (in[i]>in[i+1]) CH(in[i],in[i+1]);
	f=i+1; h=e; c=in[i+1];
	while (1) {
	  do f++; while (in[f] < c);
	  do h--; while (in[h] > c);
	  if (h < f) break;
	  CH(in[f],in[h]);
	}
	in[i+1]=in[h]; in[h]=c;
	if (h >= g) e=h-1;
	if (h <= g) i=f;
      }
  }
}

#undef CH

/* }}} */
/* {{{ mean */

FDT mean(FDT *values, int count)
{
int k;
double sum=0;

  for(k=0; k<count; k++)
    sum += (double) values[k];

  sum /= (double) count;

  return((FDT)sum);
}

/* }}} */

/* }}} */
/* {{{ interpolation and resizing */

/* {{{ trilinear interpolation */

/* note that the doubles are fractions of voxels, not fractions of mm */

#define  TLI_I(x,y,z)  ( i.i[((z)*y_size+(y))*x_size+(x)] )
#define  TLI_I_2d(x,y) ( i.i[(y)*x_size+(x)] )

FDT TLI(image_struct i, double x, double y, double z)
{
  int    ix=x, iy=y, iz=z,
    x_size=i.x, y_size=i.y;
  double fx=x-ix, fy=y-iy, fz=z-iz;

  if (i.z==1) /* single slice */

    return ( (1-fx)*TLI_I_2d(ix,iy)     + fx*TLI_I_2d(ix+1,iy)     ) * (1-fy) +
           ( (1-fx)*TLI_I_2d(ix,iy+1)   + fx*TLI_I_2d(ix+1,iy+1)   ) * fy  ;

  else

    return ( ( (1-fx)*TLI_I(ix,iy,iz)     + fx*TLI_I(ix+1,iy,iz)     ) * (1-fy) +
	     ( (1-fx)*TLI_I(ix,iy+1,iz)   + fx*TLI_I(ix+1,iy+1,iz)   ) * fy       ) * (1-fz) +
           ( ( (1-fx)*TLI_I(ix,iy,iz+1)   + fx*TLI_I(ix+1,iy,iz+1)   ) * (1-fy) +
	     ( (1-fx)*TLI_I(ix,iy+1,iz+1) + fx*TLI_I(ix+1,iy+1,iz+1) ) * fy       ) * fz  ;
  
}

/* }}} */
/* {{{ COMMENT make_isometric */

#ifdef FoldingComment

int make_isometric (image_struct in, image_struct *out)
{
  double min_length=MIN(in.xv,MIN(in.yv,in.zv)),
    max_length=MAX(in.xv,MAX(in.yv,in.zv)),
    x_scaling, y_scaling, z_scaling,
    x_start, y_start, z_start;

  /* {{{ already very close to isometric? */

  if (max_length/min_length < 1.01)
    {
      *out=in;
      return(2); /* let's use this code to tell the caller that out isn't a new image */
    }

/* }}} */

  x_scaling = in.xv/min_length;
  y_scaling = in.yv/min_length;
  z_scaling = in.zv/min_length;

  /* if scaling close to 1, just set new dimension to old */
  if (x_scaling>1.00001) out->x=ceil((double)in.x*x_scaling); else out->x=in.x;
  if (y_scaling>1.00001) out->y=ceil((double)in.y*y_scaling); else out->y=in.y;
  if (z_scaling>1.00001) out->z=ceil((double)in.z*z_scaling); else out->z=in.z;

  init_image_struct(out);

  x_start = out->x*

}

#endif

/* }}} */

/* }}} */
/* {{{ co-ordinate transforms */

/* {{{ polar to cartesian */

int p2c(int x_size, int y_size, int z_size, double cgx, double cgy, double cgz, double r, double theta, double phi, double *x, double *y, double *z)
{
  *x = r * sin(theta) * cos(phi) + cgx;
  *y = r * sin(theta) * sin(phi) + cgy;
  *z = r * cos(theta) + cgz;

  if ( (*x>0) && (*x<=x_size-1) &&
       (*y>0) && (*y<=y_size-1) &&
       (*z>0) && (*z<=z_size-1) )
    return (1);
  else
    return (0);
}

FDT getp2c(image_struct im, double cgx, double cgy, double cgz, double r, double theta, double phi)
{
  int x_size=im.x, y_size=im.y, z_size=im.z;
  double x,y,z;

  if ( p2c(x_size,y_size,z_size,cgx,cgy,cgz,r,theta,phi,&x,&y,&z) )
    return ( TLI(im,x,y,z) );
  else
    return (0); /* this is dodgy ! should return some sort of null */
}

/* }}} */

/* }}} */
/* {{{ histogram and thresholds */

/* {{{ find_histogram */

/* set im->min=im->max to tell the proc to autocompute these */

int find_histogram (image_struct *im, int *hist, int bins)
{
  FDT *imagep, *image=im->i;
  int i, size=im->t*im->z*im->y*im->x, validsize=0;

  /* zero histogram */
  for(i=0; i<bins; i++)
    hist[i]=0;

  if (im->min==im->max) /* set min and max if not already set */
    {
      for(im->min=im->dtmax, im->max=im->dtmin, imagep=image, i=0; i<size; i++)
	{
	  if ( (*imagep>im->max) && (*imagep>=im->lthresh) && (*imagep<=im->uthresh) )
	    im->max=*imagep;
	  if ( (*imagep<im->min) && (*imagep>=im->lthresh) && (*imagep<=im->uthresh) )
	    im->min=*imagep;
	  imagep++;
	}
      if (im->min==im->dtmax) im->min=im->lthresh; /* ie if no data found */
      if (im->max==im->dtmin) im->max=im->uthresh; /* ie if no data found */
      /*printf("\n%f %f\n",im->min,im->max);*/
    }
  if (im->min==im->max)
    return -1;

  /* create histogram; the MIN is so that the maximum value falls in the last valid bin, not the (last+1) bin */
  for(imagep=image, i=0; i<size; i++)
    {
      if ( (*imagep>=im->lthresh) && (*imagep<=im->uthresh) )
	{
	  hist[MAX(0, MIN( (int)((((double)bins)*((double)(*imagep-im->min)))/(im->max-im->min)), bins-1) )]++;
	  validsize++;
	}
      imagep++;
    }

   /* {{{ COMMENT print histogram */

#ifdef FoldingComment

  printf("min=%f max=%f\n",im->min,im->max);
  for(i=0; i<bins; i++)
    if ( hist[i]!=0 )
      printf("%d %d\n",i,hist[i]);

#endif

/* }}} */

   return validsize;
}

/* }}} */
/* {{{ COMMENT find_roi_histogram */

#ifdef FoldingComment

/* set im->min=im->.max to tell the proc to autocompute these */

void find_roi_histogram (image_struct *im, int x_start, int y_start, int z_start, int x_roi_size, int y_roi_size, int z_roi_size, int *hist, int bins)
{
  FDT *image=im->i;
  int i,x,y,z,x_size=im->x, y_size=im->y;
;

  /* zero histogram */
  for(i=0; i<bins; i++)
    hist[i]=0;

  if (im->min==im->max)
    for(z=z_start; z<z_start+z_roi_size; z++)
      for(y=y_start; y<y_start+y_roi_size; y++)
	for(x=x_start; x<x_start+x_roi_size; x++)
	  {
	    if (image[(z*y_size+y)*x_size+x]>im->max)
	      im->max=image[(z*y_size+y)*x_size+x];
	    if (image[(z*y_size+y)*x_size+x]<im->min)
	      im->min=image[(z*y_size+y)*x_size+x];
	  }

  /* create histogram; the MIN is so that the maximum value falls in the last valid bin, not the (last+1) bin */
  for(z=z_start; z<z_start+z_roi_size; z++)
    for(y=y_start; y<y_start+y_roi_size; y++)
      for(x=x_start; x<x_start+x_roi_size; x++)
	  hist[MAX(0, MIN( (int)((((double)bins)*((double)(image[(z*y_size+y)*x_size+x]-im->min)))/(im->max-im->min)), bins-1) )]++;

  /*  printf("min=%f max=%f\n",im->min,im->max);
  for(i=0; i<bins; i++)
  printf("%d %d\n",i,hist[i]);*/
}

#endif

/* }}} */
/* {{{ find thresholds */

#define HISTOGRAM_BINS 1000
#define MAX_PASSES     10

void find_thresholds (image_struct *im, double fraction)
{
  int hist[HISTOGRAM_BINS], top_bin=0, bottom_bin=0, count, pass=1,
    lowest_bin=0, highest_bin=HISTOGRAM_BINS-1, validsize;
  double initmin=0, initmax=0;

  while ( (pass==1) ||
	  ( (im->thresh98 - im->thresh2) < (im->max - im->min) / 10 ) ) /* test for very long tails */
    /* {{{ find histogram and thresholds */

{
  FDT tmpmin;

  if (pass>1) /* redo histogram with new min and max */
    {
      /* increase range slightly from the 2-98% range found */
      bottom_bin=MAX(bottom_bin-1,0);           
      top_bin=MIN(top_bin+1,HISTOGRAM_BINS-1);

      /* now set new min and max on the basis of this new range */
      tmpmin = (FDT)( im->min + ((double)bottom_bin/(double)(HISTOGRAM_BINS))*(im->max-im->min) );
      im->max = (FDT)( im->min + ((double)(top_bin+1)/(double)(HISTOGRAM_BINS))*(im->max-im->min) );
      im->min=tmpmin;
    }

  if (pass==MAX_PASSES)  /* give up and revert to full range ... */
    im->min = im->max = MAX_PASSES;

  validsize = find_histogram (im,hist,HISTOGRAM_BINS);

  if (pass==MAX_PASSES)  /* ... _but_ ignore end bins */
    {
      validsize-= hist[lowest_bin] + hist[highest_bin];
      lowest_bin++;
      highest_bin--;
    }

  if (pass==1) /* for sensible histogramming we may alter min and max, so we must remember the initial values */
    {
      initmin=im->min;
      initmax=im->max;
    }

  if (validsize<0) /* ie zero range */
    {
      im->thresh2=im->thresh98=im->min;
      break;
    }

  for(count=0, bottom_bin=lowest_bin; count<validsize/50; bottom_bin++)
    count+=hist[bottom_bin];
  bottom_bin--;
  im->thresh2 = (double)( im->min + ((double)bottom_bin/(double)(HISTOGRAM_BINS))*(im->max-im->min) );

  for(count=0, top_bin=highest_bin; count<validsize/50; top_bin--)
    count+=hist[top_bin];
  top_bin++;
  im->thresh98 = im->min + ((double)(top_bin+1)/(double)HISTOGRAM_BINS)*(im->max - im->min);

  /*printf("lowest_bin=%d bottom_bin=%d top_bin=%d highest_bin=%d\n     hist_min=%.4e thresh2=%.4e thresh=%.4e hist_max=%.4e\n",
	 lowest_bin,bottom_bin,top_bin,highest_bin,
	 im->min,im->thresh2,im->thresh98,im->max);*/

  if (pass==MAX_PASSES)
    break;

  pass++;
}

/* }}} */

  im->min=initmin;
  im->max=initmax;
  im->thresh = (double) ( im->thresh2 + fraction * ( im->thresh98 - im->thresh2 ) );
}

/* }}} */

/* }}} */
/* {{{ CofG, radius */

/* {{{ c_of_g */

void c_of_g (image_struct im, double *cgx, double *cgy, double *cgz)
{
  FDT    *image=im.i;
  double value, total, thresh=im.thresh, thresh2=im.thresh2;
  int    x, y, z, x_size=im.x, y_size=im.y, z_size=im.z;

  thresh -= thresh2;
  total=*cgx=*cgy=*cgz=0;

  for(z=0; z<z_size; z++)
    for(y=0; y<y_size; y++)
      for(x=0; x<x_size; x++)
	{
	  value = ((double) image[z*x_size*y_size+y*x_size+x]) - thresh2;
	  if (value>thresh) {
	    total += value; *cgx += (double) x * value; *cgy += (double) y * value; *cgz += (double) z * value; }
	}
  *cgx /= total; *cgy /= total; *cgz /= total;
}

/* }}} */
/* {{{ find_radius */

/* if "scale" is voxel volume (mm^3) then output is in mm

   OR

   if cubels are being used, i.e., the smallest voxel dimension is set
   to 1 and the others rescaled accordingly, thus scale is set to
   xv*yv*zv where the minimum of these is 1, then radius is in units
   of this "1", ie the smallest voxel dimension
*/

double find_radius (image_struct im, double scale)
{
  FDT threshold=im.thresh, *image=im.i;
  int i, size=im.x*im.y*im.z;
  double radius, count=0;

  for (i=0; i<size; i++)
    if (*image++>threshold)
      count ++;

  radius = pow(0.75*count*scale/M_PI,1.0/3.0);

  return(radius);
}

/* }}} */

/* }}} */
/* {{{ invert_y */

void invert_y (image_struct im)
{
  FDT *image=im.i, tmp;
  int x, y, z, x_size=im.x, y_size=im.y, z_size=im.z;

  for(z=0; z<z_size; z++)
    for(y=0; y<y_size/2; y++)
      for(x=0; x<x_size; x++)
	{
	  tmp = image[(z*y_size+y)*x_size+x];
	  image[(z*y_size+y)*x_size+x] = image[(z*y_size+(y_size-1-y))*x_size+x];
	  image[(z*y_size+(y_size-1-y))*x_size+x] = tmp;
	}
}

/* }}} */
/* {{{ image_struct stuff */

/* call after setting image dimensions */

void print_image_struct(image_struct i)
{
  printf("dim=(%d,%d,%d,%d) voxel=(%f,%f,%f) voxel_original=(%f,%f,%f) origin=(%f,%f,%f)\norient=%d tr=%f min,max,thresh2,thresh98,thresh=(%f,%f,%f,%f,%f)\n",i.x,i.y,i.z,i.t,i.xv,i.yv,i.zv,i.xv0,i.yv0,i.zv0,i.xo,i.yo,i.zo,i.orient,i.tr,i.min,i.max,i.thresh2,i.thresh98,i.thresh);
}

void init_image_struct(image_struct *i)
{
  i->xv=1; i->yv=1; i->zv=1;
  i->xv0=1; i->yv0=1; i->zv0=1;
  i->xo=0; i->yo=0; i->zo=0;
  i->tr=1;
  i->orient=0;
  i->min = i->max = i->thresh2 = i->thresh98 = i->thresh = 0;

  if (i->t==0)
    i->t=1;

  if((i->i=(FDT*)malloc(sizeof(FDT)*i->x*i->y*i->z*i->t))==NULL)
     {
       fprintf(stderr,"Malloc failed\n");
       exit(1);
     }
}

/* }}} */
/* {{{ spatial and temporal filtering and intensity normalisation */

/* {{{ spatgauss 3D Gaussian filtering */

/* sigmas are in voxels not mm */

void spatgauss(FDT *in, int x_size, int y_size, int z_size, double x_sigma, double y_sigma, double z_sigma)
{
  FDT    *inp;
  int    x, y, z, lp_mask_size, i, i_size=x_size*y_size;
  double *lp_exp, *lp_exp_orig, *array, *arrayp;

  /* {{{ x filtering */

if ( (x_sigma>0) && (x_size>1) )
{
  /* {{{ find mask size and create lp_exp LUT */

array=(double *)malloc(sizeof(double)*x_size);

lp_mask_size=0.5+4*x_sigma;
lp_exp_orig=(double *)malloc(sizeof(double)*(2*lp_mask_size+1));
lp_exp=lp_exp_orig+lp_mask_size;

for(i=-lp_mask_size; i<=lp_mask_size; i++)
     lp_exp[i] = exp( -0.5 * ((double)(i*i)) / (x_sigma*x_sigma) );

/* }}} */

  for(z=0; z<z_size; z++)
    {
      for(y=0; y<y_size; y++)
	{
          /* {{{ read data from 3D input into 1D array */

inp=&IA(in,0,y,z);
arrayp=array;

for(x=0; x<x_size; x++)
{
  *arrayp++ = (double)*inp++;
  /*  inp += v_size;*/
}

/* }}} */
	  /* {{{ apply lowpass filter to 1D array */

inp=&IA(in,0,y,z);

for(x=0; x<x_size; x++)
{
  double sum=0, total=0;
  int ii,
    start=MAX(x-lp_mask_size,0),
    stop=MIN(x+lp_mask_size,x_size-1);

  for(ii=start; ii<=stop; ii++)
    {
      sum+=lp_exp[ii-x]*array[ii];
      total+=lp_exp[ii-x];
    }

  *inp++ = (FDT)(sum/total); /* maybe should bound this to data type limits.... */
}

/* }}} */
	}
    }

  free(array);
  free(lp_exp_orig);
}

/* }}} */
  /* {{{ y filtering */

if ( (y_sigma>0) && (y_size>1) )
{
  /* {{{ find mask size and create lp_exp LUT */

array=(double *)malloc(sizeof(double)*y_size);

lp_mask_size=0.5+4*y_sigma;
lp_exp_orig=(double *)malloc(sizeof(double)*(2*lp_mask_size+1));
lp_exp=lp_exp_orig+lp_mask_size;

for(i=-lp_mask_size; i<=lp_mask_size; i++)
     lp_exp[i] = exp( -0.5 * ((double)(i*i)) / (y_sigma*y_sigma) );

/* }}} */

  for(z=0; z<z_size; z++)
    {
      for(x=0; x<x_size; x++)
	{
          /* {{{ read data from 3D input into 1D array */

inp=&IA(in,x,0,z);
arrayp=array;

for(y=0; y<y_size; y++)
{
  *arrayp++ = (double)*inp;
  inp += x_size;
}

/* }}} */
	  /* {{{ apply lowpass filter to 1D array */

inp=&IA(in,x,0,z);

for(y=0; y<y_size; y++)
{
  double sum=0, total=0;
  int ii,
    start=MAX(y-lp_mask_size,0),
    stop=MIN(y+lp_mask_size,y_size-1);

  for(ii=start; ii<=stop; ii++)
    {
      sum+=lp_exp[ii-y]*array[ii];
      total+=lp_exp[ii-y];
    }

  *inp = (FDT)(sum/total); /* maybe should bound this to data type limits.... */
  inp += x_size;
}

/* }}} */
	}
    }

  free(array);
  free(lp_exp_orig);
}

/* }}} */
  /* {{{ z filtering */

if ( (z_sigma>0) && (z_size>1) )
{
  /* {{{ find mask size and create lp_exp LUT */

array=(double *)malloc(sizeof(double)*z_size);

lp_mask_size=0.5+4*z_sigma;
lp_exp_orig=(double *)malloc(sizeof(double)*(2*lp_mask_size+1));
lp_exp=lp_exp_orig+lp_mask_size;

for(i=-lp_mask_size; i<=lp_mask_size; i++)
     lp_exp[i] = exp( -0.5 * ((double)(i*i)) / (z_sigma*z_sigma) );

/* }}} */

  for(y=0; y<y_size; y++)
    {
      for(x=0; x<x_size; x++)
	{
          /* {{{ read data from 3D input into 1D array */

inp=&IA(in,x,y,0);
arrayp=array;

for(z=0; z<z_size; z++)
{
  *arrayp++ = (double)*inp;
  inp += i_size;
}

/* }}} */
	  /* {{{ apply lowpass filter to 1D array */

inp=&IA(in,x,y,0);

for(z=0; z<z_size; z++)
{
  double sum=0, total=0;
  int ii,
    start=MAX(z-lp_mask_size,0),
    stop=MIN(z+lp_mask_size,z_size-1);

  for(ii=start; ii<=stop; ii++)
    {
      sum+=lp_exp[ii-z]*array[ii];
      total+=lp_exp[ii-z];
    }

  *inp = (FDT)(sum/total); /* maybe should bound this to data type limits.... */
  inp += i_size;
}

/* }}} */
	}
    }

  free(array);
  free(lp_exp_orig);
}

/* }}} */
}

/* }}} */
/* {{{ spatfilt Gaussian (linear) lowpass spatial filter for 3D or 4D data */

/* sigma is HWHM of filter, in mm */

void spatfilt(image_struct im, double sigma)
{
  int t; 

  if (im.t<1) im.t=1; /* in case a 3D image doesn't have im.t set */

  for(t=0; t<im.t; t++)
    spatgauss(im.i+t*(im.x*im.y*im.z), im.x, im.y, im.z, sigma/im.xv, sigma/im.yv, sigma/im.zv);
}

/* }}} */

/* {{{ fmrimask */

void fmrimask(image_struct im, image_struct mask)
{
  FDT *inp;
  int size=im.x*im.y*im.z, i, t;
  unsigned char *mi=(unsigned char*)mask.i;

  /* {{{ create and apply mask */

for(i=0; i<size; i++)
  mi[i]=1;

for(t=0; t<im.t; t++)
{
  inp=im.i+t*size;
  for(i=0; i<size; i++)
    {
      if (*inp<im.thresh)
	mi[i]=0;
      inp++;
    }
}

for(t=0; t<im.t; t++)
{
  inp=im.i+t*size;
  for(i=0; i<size; i++)
    {
      if (mi[i]==0)
	*inp=0;
      inp++;
    }
}

/* }}} */
}

/* }}} */
/* {{{ intensity normalisation */

void intnorm(image_struct im, int grand_mean, double target_mean)
{
  FDT    *inp;
  int    t_size=im.t, v_size=im.x*im.y*im.z, i, t, count;
  double total, tmpf, factor;

  if (grand_mean==1) /* make the data look like one big volume - groovy hack! */
    {
      v_size*=t_size;
      t_size=1;
    }

  /* {{{ do per-volume re-meaning */

  for(t=0; t<t_size; t++)
    {
      count=0;
      total=0;

      inp=im.i+t*v_size;
      for(i=0; i<v_size; i++)
	{
	  if (*inp>=im.thresh)
	    {
	      total += *inp;
	      count++;
	    }
	  inp++;
	}

      factor = target_mean * count / total;
      /*printf("normalisation factor=%f\n",factor);*/

      inp=im.i+t*v_size;
      for(i=0; i<v_size; i++)
	{
	  tmpf=((double)*inp) * factor;
	  if (tmpf>im.dtmax) *inp=(FDT)im.dtmax;
	  else               *inp=(FDT)tmpf;
	  inp++;
	}
    }

/* }}} */
}

/* }}} */

/* {{{ tempfilt nonlinear highpass and linear lowpass temporal filter */

/* sigmas are HWHM of filters, in volumes not seconds; make either <0 to skip that filter */

void tempfilt(image_struct im, double hp_sigma, double lp_sigma)
{
  FDT   *in=im.i, *inp;
  int   x_size=im.x, y_size=im.y, z_size=im.z, t_size=im.t, v_size=im.x*im.y*im.z,
    x, y, z, t, hp_mask_size, lp_mask_size;
  double *hp_exp=NULL, *lp_exp=NULL, *arrayp, *array, *array2;

  /* {{{ mallocs */

hp_mask_size=hp_sigma*3.0;   /* this isn't a linear filter, so small hard cutoffs at ends don't matter */
if (hp_mask_size<0) hp_mask_size=0;

lp_mask_size=lp_sigma*5.0+2; /* this will be small, so we might as well be careful */
if (lp_mask_size<0) lp_mask_size=0;

array=(double *)malloc(sizeof(double)*(im.t+2*lp_mask_size));
array+=lp_mask_size;

array2=(double *)malloc(sizeof(double)*(im.t+2*lp_mask_size));
array2+=lp_mask_size;

/* }}} */
  /* {{{ create hp_exp LUT */

if (hp_sigma>0)
{
  hp_exp=(double*)malloc(sizeof(double)*(2*hp_mask_size+1));
  hp_exp+=hp_mask_size;

  for(t=-hp_mask_size; t<=hp_mask_size; t++)
    hp_exp[t] = exp( -0.5 * ((double)(t*t)) / (hp_sigma*hp_sigma) );
}

/* }}} */
  /* {{{ create lp_exp LUT */

if (lp_sigma>0)
{
  double total=0;

  lp_exp=(double*)malloc(sizeof(double)*(2*lp_mask_size+1));
  lp_exp+=lp_mask_size;

  for(t=-lp_mask_size; t<=lp_mask_size; t++)
    {
      lp_exp[t] = exp( -0.5 * ((double)(t*t)) / (lp_sigma*lp_sigma) );
      total += lp_exp[t];
    }

  for(t=-lp_mask_size; t<=lp_mask_size; t++)
    {
      lp_exp[t] /= total;
      /*printf("%f\n",lp_exp[t]);*/
    }
}

/* }}} */
  /* {{{ apply filters */

for(z=0; z<z_size; z++)
{
  for(y=0; y<y_size; y++)
    {
      for(x=0; x<x_size; x++)
	{
	  double c0=0;

          /* {{{ read time series from 4D input data into 1D array */

inp=&IT(in,x,y,z,0);
arrayp=array;

for(t=0; t<t_size; t++)
{
  *arrayp++ = (double)*inp;
  inp += v_size;
}

/* }}} */
	  /* {{{ apply highpass filter to 1D array */

if (hp_sigma>0)
{
  for(t=0; t<t_size; t++)
    {
      int tt;
      double c, w, A=0, B=0, C=0, D=0, N=0;

      /* {{{ calculate LSF sums */

for(tt=MAX(t-hp_mask_size,0); tt<=MIN(t+hp_mask_size,t_size-1); tt++)
{
  int dt=tt-t;
  
  w = hp_exp[dt];
  A += w * dt;
  B += w * array[tt];
  C += w * dt * dt;
  D += w * dt * array[tt];
  N += w;
}

/* }}} */

      c = (B*C-A*D) / (C*N-A*A);
      /* m = (D*N-B*A) / (C*N-A*A);*/ /* don't need this */

      if (t==0) c0=c;
      array2[t] = c0 + array[t] - c;
    }

  memcpy(array,array2,sizeof(double)*t_size);
}

/* }}} */
	  /* {{{ apply lowpass filter to 1D array */

if (lp_sigma>0)
{
  /* {{{ pad array at ends */

  for(t=1; t<=lp_mask_size; t++)
    {
      array[-t]=array[0];
      array[t_size-1+t]=array[t_size-1];
    }

/* }}} */

  for(t=0; t<t_size; t++)
    {
      double total=0;
      int tt;

      for(tt=t-lp_mask_size; tt<=t+lp_mask_size; tt++)
	total += array[tt] * lp_exp[tt-t];

      array2[t] = total;
    }

  memcpy(array,array2,sizeof(double)*t_size);
}

/* }}} */
	  /* {{{ write 1D array back to input 4D data */

inp=&IT(in,x,y,z,0);
arrayp=array;

for(t=0; t<t_size; t++)
{
  *inp = (FDT)*arrayp++;
  inp += v_size;
}

/* }}} */
	}
    }
}

/* }}} */
}

/* }}} */
/* {{{ COMMENT (variable sigma at ends) tempfilt nonlinear highpass and linear lowpass temporal filter */

#ifdef FoldingComment

/* sigmas are HWHM of filters, in volumes not seconds; make either <0 to skip that filter */

void tempfilt(image_struct im, double hp_sigma, double lp_sigma)
{
  FDT   *in=im.i, *inp;
  int   x_size=im.x, y_size=im.y, z_size=im.z, t_size=im.t, v_size=im.x*im.y*im.z,
    x, y, z, t, hp_mask_size, local_hp_mask_size, lp_mask_size;
  double *hp_exp, *lp_exp, *arrayp, *array, *array2, local_hp_sigma;

  /* {{{ mallocs */

hp_mask_size=hp_sigma*3.0;   /* this isn't a linear filter, so small hard cutoffs at ends don't matter */
if (hp_mask_size<0) hp_mask_size=0;

lp_mask_size=lp_sigma*5.0+2; /* this will be small, so we might as well be careful */
if (lp_mask_size<0) lp_mask_size=0;

array=(double *)malloc(sizeof(double)*(im.t+2*lp_mask_size));
array+=lp_mask_size;

array2=(double *)malloc(sizeof(double)*(im.t+2*lp_mask_size));
array2+=lp_mask_size;

/* }}} */
  /* {{{ create hp_exp LUT */

if (hp_sigma>0)
{
  hp_exp=(double*)malloc(sizeof(double)*(2*hp_mask_size+1));
  hp_exp+=hp_mask_size;

  for(t=-hp_mask_size; t<=hp_mask_size; t++)
    hp_exp[t] = exp( -0.5 * ((double)(t*t)) / (hp_sigma*hp_sigma) );
}

/* }}} */
  /* {{{ create lp_exp LUT */

if (lp_sigma>0)
{
  double total=0;

  lp_exp=(double*)malloc(sizeof(double)*(2*lp_mask_size+1));
  lp_exp+=lp_mask_size;

  for(t=-lp_mask_size; t<=lp_mask_size; t++)
    {
      lp_exp[t] = exp( -0.5 * ((double)(t*t)) / (lp_sigma*lp_sigma) );
      total += lp_exp[t];
    }

  for(t=-lp_mask_size; t<=lp_mask_size; t++)
    {
      lp_exp[t] /= total;
      /*printf("%f\n",lp_exp[t]);*/
    }
}

/* }}} */
  /* {{{ apply filters */

for(z=0; z<z_size; z++)
{
  for(y=0; y<y_size; y++)
    {
      for(x=0; x<x_size; x++)
	{
	  double c0;

          /* {{{ read time series from 4D input data into 1D array */

inp=&IT(in,x,y,z,0);
arrayp=array;

for(t=0; t<t_size; t++)
{
  *arrayp++ = (double)*inp;
  inp += v_size;
}

/* }}} */
	  /* {{{ apply highpass filter to 1D array */

if (hp_sigma>0)
{
  for(t=0; t<t_size; t++)
    {
      int tt, tmp;
      double c, w, A=0, B=0, C=0, D=0, N=0;

      /* {{{ calculate LSF sums */

tmp=t_size/2-abs(t-t_size/2);
if (tmp<hp_sigma) /* if close to ends increase sigma */
  local_hp_sigma=2.0*hp_sigma-tmp;
else
  local_hp_sigma=hp_sigma;
printf("%f ",local_hp_sigma);
local_hp_mask_size=3.0*local_hp_sigma;

for(tt=MAX(t-local_hp_mask_size,0); tt<=MIN(t+local_hp_mask_size,t_size-1); tt++)
{
  int dt=tt-t;
  
  w = exp( -0.5 * ((double)(dt*dt)) / (local_hp_sigma*local_hp_sigma) );
  A += w * dt;
  B += w * array[tt];
  C += w * dt * dt;
  D += w * dt * array[tt];
  N += w;
}

/* }}} */
      /* {{{ COMMENT ORIG calculate LSF sums */

#ifdef FoldingComment

for(tt=MAX(t-hp_mask_size,0); tt<=MIN(t+hp_mask_size,t_size-1); tt++)
{
  int dt=tt-t;
  
  w = hp_exp[dt];
  A += w * dt;
  B += w * array[tt];
  C += w * dt * dt;
  D += w * dt * array[tt];
  N += w;
}

#endif

/* }}} */

      c = (B*C-A*D) / (C*N-A*A);
      /* m = (D*N-B*A) / (C*N-A*A);*/ /* don't need this */

      if (t==0) c0=c;
      array2[t] = c0 + array[t] - c;
    }

  memcpy(array,array2,sizeof(double)*t_size);
}

/* }}} */
	  /* {{{ apply lowpass filter to 1D array */

if (lp_sigma>0)
{
  /* {{{ pad array at ends */

  for(t=1; t<=lp_mask_size; t++)
    {
      array[-t]=array[0];
      array[t_size-1+t]=array[t_size-1];
    }

/* }}} */

  for(t=0; t<t_size; t++)
    {
      double total=0;
      int tt;

      for(tt=t-lp_mask_size; tt<=t+lp_mask_size; tt++)
	total += array[tt] * lp_exp[tt-t];

      array2[t] = total;
    }

  memcpy(array,array2,sizeof(double)*t_size);
}

/* }}} */
	  /* {{{ write 1D array back to input 4D data */

inp=&IT(in,x,y,z,0);
arrayp=array;

for(t=0; t<t_size; t++)
{
  *inp = (FDT)*arrayp++;
  inp += v_size;
}

/* }}} */
	}
    }
}

/* }}} */
}

#endif

/* }}} */

/* {{{ resample (to new image) */

void resample(image_struct in, image_struct *out, float scale)
{
  int x, y, z, t;

  /* {{{ copy image and rescale parameters */

  *out=in;

  out->x=MAX(1,in.x*scale);
  out->y=MAX(1,in.y*scale);
  out->z=MAX(1,in.z*scale);

  out->xv/=scale;   out->xv0/=scale;
  out->yv/=scale;   out->yv0/=scale;
  out->zv/=scale;   out->zv0/=scale;

  out->xo*=scale;
  out->yo*=scale;
  out->zo*=scale;

/*
  printf("in  %d %d %d   %f %f %f %f %f %f   %f %f %f\n",
	 in.x,in.y,in.z,in.xv,in.yv,in.zv,in.xv0,in.yv0,in.zv0,in.xo,in.yo,in.zo);
  printf("out %d %d %d   %f %f %f %f %f %f   %f %f %f\n",
	 out->x,out->y,out->z,out->xv,out->yv,out->zv,out->xv0,out->yv0,out->zv0,out->xo,out->yo,out->zo);
*/

/* }}} */
  /* {{{ make copy of input and blur */

if (scale<1)
{
  FDT *tmp;

  tmp=malloc(in.x*in.y*in.z*in.t*sizeof(FDT));

  memcpy(tmp,in.i,in.x*in.y*in.z*in.t*sizeof(FDT));

  in.i=tmp;

  for(t=0; t<in.t; t++)
    spatgauss(tmp+t*(in.x*in.y*in.z), in.x, in.y, in.z, 0.425/scale, 0.425/scale, 0.425/scale);
}

/* }}} */
  /* {{{ malloc new image and fill */

out->i=malloc(out->x*out->y*out->z*out->t*sizeof(FDT));

for(t=0;t<out->t;t++)
  for(z=0;z<out->z;z++)
    for(y=0;y<out->y;y++)
      for(x=0;x<out->x;x++)
        out->i[t*out->z*out->y*out->x+z*out->y*out->x+y*out->x+x] = TLI(in,x/scale,y/scale,z/scale);

/* }}} */
}

/* }}} */
/* {{{ sample (from input image to existing image) */

void sample(image_struct in, image_struct out, float scale)
{
  int x, y, z, t;

  for(t=0;t<out.t;t++)
    for(z=0;z<out.z;z++)
      for(y=0;y<out.y;y++)
	for(x=0;x<out.x;x++)
	  {
	    float X=x/scale, Y=y/scale, Z=z/scale;

	    if ( (X>0) && (X<in.x-1) && (Y>0) && (Y<in.y-1) && (Z>0) && (Z<in.z-1) )
	      out.i[t*out.z*out.y*out.x+z*out.y*out.x+y*out.x+x] = TLI(in,X,Y,Z);
	  }
}

/* }}} */

/* }}} */


