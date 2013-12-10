/* Modified version of John M. Boone's code */
/* His code mentions no license whatsoever, so will assume it is in the public domain */
/* Tom Schoonjans 2013 */


/* ------------------------------------------------------------
   File Name:   mamspec.c
   Author:      John M. Boone, Ph.D.
   Date:        Dec 15, 1997
   Description: Mammography Spectrum Program
   
Note:  This code is supplied as is and you should always check your
       calculated results.  This program was created for ease of use, 
       and was not optimized for memory efficiency or operational speed.
       Good luck.
==================================================================
   Parameters passed to subroutine:  MAMSPEC( itube,xkv,en,spec )
INPUTS:
   itube: (integer)
              1 = molybdenum anode
              2 = rhodium anode
              3 = tungsten anode
   xkv:  (floating point)
              needs to be in the range 18.0 to 42.0
OUTPUTS:
   en:   (floating point array, at least 100 elements long)
              lists the energies of the output spectrum              
   spec: (floating point array, al least 100 elements long)
              lists the fluence values of the output spectrum
------------------------------------------------------------ */

#include <math.h>
#include <xraylib.h>
#include "xmi_data_structs.h"
#include "xmi_boone.h"
#include <stdio.h>
#include <stdlib.h>

/* ---------------------------------------------
     File: moly.h
     Date: Mar 13, 1997 (04:58 pm)
     Data for creating XASMIP spectra
     This file for creating MOLYBDENUM spectra
--------------------------------------------- */ 
static int mnn[150]={
0,0,0,0,0,0,0,2,2,4,4,4,4,4,4,
4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
4,4,4,4,4,4,4,4,3,3,4,4,4,4,4,
4,4,4,3,3,3,3,3,4,3,3,3,3,3,3,
3,3,3,3,2,3,3,3,2,3,3,3,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

static double menergy[150]={
 0.50, 1.00, 1.50, 2.00, 2.50, 3.00, 3.50, 4.00, 4.50, 5.00,
 5.50, 6.00, 6.50, 7.00, 7.50, 8.00, 8.50, 9.00, 9.50,10.00,
10.50,11.00,11.50,12.00,12.50,13.00,13.50,14.00,14.50,15.00,
15.50,16.00,16.50,17.00,17.50,18.00,18.50,19.00,19.50,20.00,
20.50,21.00,21.50,22.00,22.50,23.00,23.50,24.00,24.50,25.00,
25.50,26.00,26.50,27.00,27.50,28.00,28.50,29.00,29.50,30.00,
30.50,31.00,31.50,32.00,32.50,33.00,33.50,34.00,34.50,35.00,
35.50,36.00,36.50,37.00,37.50,38.00,38.50,39.00,39.50,40.00,
40.50,41.00,41.50,42.00,42.50,43.00,43.50,44.00,44.50,45.00,
45.50, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00};

static double maa[151][4]={
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{-1.444468e+005,+1.505384e+004,+0.000000e+000,+0.000000e+000},
{-2.157610e+004,+1.756251e+004,+0.000000e+000,+0.000000e+000},
{-1.069419e+006,+1.569301e+005,-4.043083e+003,+3.914996e+001},
{-3.062868e+006,+4.184705e+005,-1.177408e+004,+1.196206e+002},
{-4.916993e+006,+6.789471e+005,-1.904982e+004,+1.963841e+002},
{-6.925022e+006,+9.227469e+005,-2.431684e+004,+2.485349e+002},
{-9.240879e+006,+1.197607e+006,-3.016570e+004,+2.894177e+002},
{-1.196409e+007,+1.506369e+006,-3.734140e+004,+3.358694e+002},
{-1.433777e+007,+1.725903e+006,-4.149616e+004,+4.076118e+002},
{-1.346037e+007,+1.602853e+006,-3.417208e+004,+3.197254e+002},
{-1.703271e+007,+1.992354e+006,-4.714275e+004,+4.732125e+002},
{-1.735496e+007,+1.964368e+006,-4.346886e+004,+4.179785e+002},
{-1.806311e+007,+2.014349e+006,-4.465826e+004,+4.502301e+002},
{-1.680020e+007,+1.840469e+006,-3.830591e+004,+3.793459e+002},
{-2.131791e+007,+2.320385e+006,-5.571021e+004,+5.947650e+002},
{-1.837808e+007,+1.941591e+006,-4.164300e+004,+4.369608e+002},
{-1.984031e+007,+2.051260e+006,-4.550193e+004,+4.825815e+002},
{-1.868657e+007,+1.856300e+006,-3.785170e+004,+3.944230e+002},
{-2.294681e+007,+2.290842e+006,-5.356084e+004,+5.760982e+002},
{-2.231694e+007,+2.181380e+006,-5.004156e+004,+5.412009e+002},
{-2.281241e+007,+2.182148e+006,-4.982511e+004,+5.395803e+002},
{-2.223463e+007,+2.055223e+006,-4.472059e+004,+4.753687e+002},
{-2.138391e+007,+1.910704e+006,-3.956225e+004,+4.187863e+002},
{-2.183426e+007,+1.905475e+006,-3.938379e+004,+4.207844e+002},
{-2.210670e+007,+1.880798e+006,-3.838533e+004,+4.106319e+002},
{-2.341731e+007,+1.966213e+006,-4.140524e+004,+4.524618e+002},
{+4.410687e+006,-1.228638e+006,+5.956400e+004,-1.509388e+002},
{+2.393001e+008,-2.799296e+007,+9.214276e+005,-5.792732e+003},
{-1.687220e+007,+1.111509e+006,-1.417979e+004,+2.537656e+002},
{-2.685072e+007,+2.144921e+006,-4.691376e+004,+4.874623e+002},
{-3.181105e+007,+2.602798e+006,-6.209036e+004,+6.590199e+002},
{+3.735285e+006,-1.520292e+006,+6.688979e+004,+0.000000e+000},
{-7.801074e+006,-3.623279e+004,+2.136239e+004,+0.000000e+000},
{-2.607519e+007,+1.984240e+006,-4.278393e+004,+3.803342e+002},
{-3.235224e+007,+2.581614e+006,-6.219627e+004,+5.856532e+002},
{-2.883215e+007,+2.182060e+006,-4.876334e+004,+4.424922e+002},
{-2.762892e+007,+2.061974e+006,-4.578730e+004,+4.210210e+002},
{-3.111678e+007,+2.317619e+006,-5.247238e+004,+4.795377e+002},
{-2.692913e+007,+1.845917e+006,-3.656864e+004,+3.073399e+002},
{-3.602581e+007,+2.706274e+006,-6.425582e+004,+5.998871e+002},
{-2.948700e+007,+2.084902e+006,-4.552940e+004,+4.157864e+002},
{-1.634090e+007,+7.942274e+005,-4.991084e+003,+0.000000e+000},
{-1.891984e+007,+9.291174e+005,-6.896883e+003,+0.000000e+000},
{-1.813234e+007,+8.642746e+005,-5.958818e+003,+0.000000e+000},
{-1.772540e+007,+8.280173e+005,-5.458039e+003,+0.000000e+000},
{-1.908858e+007,+8.867837e+005,-6.205095e+003,+0.000000e+000},
{-5.870978e+007,+4.326653e+006,-1.055765e+005,+9.471267e+002},
{-1.951479e+007,+8.763217e+005,-5.994696e+003,+0.000000e+000},
{-1.873200e+007,+8.200074e+005,-5.262214e+003,+0.000000e+000},
{-1.988762e+007,+8.638795e+005,-5.760048e+003,+0.000000e+000},
{-1.989074e+007,+8.507146e+005,-5.634714e+003,+0.000000e+000},
{-2.091310e+007,+8.944657e+005,-6.235000e+003,+0.000000e+000},
{-1.976925e+007,+8.214290e+005,-5.284905e+003,+0.000000e+000},
{-2.486637e+007,+1.079714e+006,-8.691732e+003,+0.000000e+000},
{-2.916641e+007,+1.297188e+006,-1.158454e+004,+0.000000e+000},
{-2.523419e+007,+1.069977e+006,-8.528449e+003,+0.000000e+000},
{-2.576746e+007,+1.095557e+006,-8.985011e+003,+0.000000e+000},
{-1.388773e+007,+4.327039e+005,+0.000000e+000,+0.000000e+000},
{-2.630931e+007,+1.076203e+006,-8.433643e+003,+0.000000e+000},
{-3.190475e+007,+1.368707e+006,-1.241636e+004,+0.000000e+000},
{-2.139757e+007,+7.921584e+005,-4.691571e+003,+0.000000e+000},
{-1.498973e+007,+4.408404e+005,+0.000000e+000,+0.000000e+000},
{-3.664991e+007,+1.552772e+006,-1.441075e+004,+0.000000e+000},
{-3.575070e+007,+1.494224e+006,-1.368656e+004,+0.000000e+000},
{-2.285521e+007,+8.235004e+005,-5.143063e+003,+0.000000e+000},
{-1.491104e+007,+4.134720e+005,+0.000000e+000,+0.000000e+000},
{-1.569037e+007,+4.286760e+005,+0.000000e+000,+0.000000e+000},
{-1.571175e+007,+4.220360e+005,+0.000000e+000,+0.000000e+000},
{-1.559272e+007,+4.143320e+005,+0.000000e+000,+0.000000e+000},
{-1.597894e+007,+4.182650e+005,+0.000000e+000,+0.000000e+000},
{-1.646647e+007,+4.259945e+005,+0.000000e+000,+0.000000e+000},
{-1.809636e+007,+4.597110e+005,+0.000000e+000,+0.000000e+000},
{-1.676175e+007,+4.227545e+005,+0.000000e+000,+0.000000e+000},
{-1.980247e+007,+4.895522e+005,+0.000000e+000,+0.000000e+000},
{-2.153375e+007,+5.258515e+005,+0.000000e+000,+0.000000e+000},
{-2.594940e+007,+6.260341e+005,+0.000000e+000,+0.000000e+000},
{-1.435279e+008,+3.420986e+006,+0.000000e+000,+0.000000e+000},
{-1.435279e+008,+3.420986e+006,+0.000000e+000,+0.000000e+000},
{-1.435279e+008,+3.420986e+006,+0.000000e+000,+0.000000e+000},
{-1.435279e+008,+3.420986e+006,+0.000000e+000,+0.000000e+000},
{-1.435279e+008,+3.420986e+006,+0.000000e+000,+0.000000e+000},
{-1.435279e+008,+3.420986e+006,+0.000000e+000,+0.000000e+000},
{-1.435279e+008,+3.420986e+006,+0.000000e+000,+0.000000e+000},
{-1.435279e+008,+3.420986e+006,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
};

/* ---------------------------------------------
     File: rhodium.h
     Date: Mar 13, 1997 (04:58 pm)
     Data for creating XASMIP spectra
     This file for creating RHODIUM spectra
--------------------------------------------- */ 
static int rnn[150]={
0,0,0,0,0,0,0,0,3,3,3,4,4,4,4,
4,4,4,4,3,4,4,4,4,4,3,4,3,4,3,
4,4,4,4,4,4,4,3,4,3,4,4,4,4,3,
4,3,3,3,3,4,3,3,3,3,3,3,3,3,3,
3,3,3,2,3,2,3,3,2,3,2,3,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

static double renergy[150]={
 0.20, 0.70, 1.20, 1.70, 2.20, 2.70, 3.20, 3.70, 4.20, 4.70,
 5.20, 5.70, 6.20, 6.70, 7.20, 7.70, 8.20, 8.70, 9.20, 9.70,
10.20,10.70,11.20,11.70,12.20,12.70,13.20,13.70,14.20,14.70,
15.20,15.70,16.20,16.70,17.20,17.70,18.20,18.70,19.20,19.70,
20.20,20.70,21.20,21.70,22.20,22.70,23.20,23.70,24.20,24.70,
25.20,25.70,26.20,26.70,27.20,27.70,28.20,28.70,29.20,29.70,
30.20,30.70,31.20,31.70,32.20,32.70,33.20,33.70,34.20,34.70,
35.20,35.70,36.20,36.70,37.20,37.70,38.20,38.70,39.20,39.70,
40.20,40.70,41.20,41.70,42.20,42.70,43.20,43.70,44.20,44.70,
45.20, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00};

static double raa[151][4]={
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{-1.192045e+005,+1.367483e+004,-1.778029e+002,+0.000000e+000},
{-1.530890e+005,+2.993084e+004,-3.532862e+002,+0.000000e+000},
{-4.149003e+005,+8.345416e+004,-1.083803e+003,+0.000000e+000},
{-2.110957e+006,+3.086459e+005,-6.845642e+003,+5.278299e+001},
{-2.895965e+006,+4.286116e+005,-8.011144e+003,+5.426242e+001},
{-5.555697e+006,+7.440265e+005,-1.642142e+004,+1.359430e+002},
{-6.230149e+006,+8.173703e+005,-1.611456e+004,+1.222256e+002},
{-6.721657e+006,+8.681462e+005,-1.532193e+004,+1.103395e+002},
{-9.068231e+006,+1.096678e+006,-2.023126e+004,+1.549885e+002},
{-8.560903e+006,+1.021060e+006,-1.534351e+004,+9.531149e+001},
{-9.932459e+006,+1.131318e+006,-1.756427e+004,+1.235573e+002},
{-8.539150e+006,+9.150594e+005,-7.607820e+003,+0.000000e+000},
{-1.230498e+007,+1.299176e+006,-2.007492e+004,+1.400943e+002},
{-1.258838e+007,+1.281049e+006,-1.888631e+004,+1.459051e+002},
{-1.117718e+007,+1.123389e+006,-1.388007e+004,+9.606536e+001},
{-1.341985e+007,+1.322989e+006,-2.081445e+004,+1.790645e+002},
{-1.165702e+007,+1.092111e+006,-1.302265e+004,+9.547466e+001},
{-9.828931e+006,+8.537916e+005,-4.495113e+003,+0.000000e+000},
{-1.315013e+007,+1.166460e+006,-1.522815e+004,+1.211643e+002},
{-1.055732e+007,+8.516584e+005,-4.450458e+003,+0.000000e+000},
{-1.226238e+007,+9.997632e+005,-9.875705e+003,+6.619936e+001},
{-1.138493e+007,+8.582543e+005,-4.631944e+003,+0.000000e+000},
{-1.421375e+007,+1.145459e+006,-1.533291e+004,+1.259442e+002},
{-9.209721e+006,+5.087933e+005,+8.598628e+003,-1.587797e+002},
{-1.549092e+007,+1.206348e+006,-1.766176e+004,+1.532736e+002},
{-1.932713e+007,+1.607190e+006,-3.258084e+004,+3.306037e+002},
{-1.655464e+007,+1.245415e+006,-2.068765e+004,+2.442210e+002},
{-1.425125e+007,+9.396121e+005,-9.776435e+003,+1.238981e+002},
{-1.815094e+007,+1.364727e+006,-2.403874e+004,+2.285077e+002},
{-1.216617e+007,+7.156276e+005,-2.695818e+003,+0.000000e+000},
{-2.145288e+007,+1.616216e+006,-3.195334e+004,+3.094406e+002},
{-2.414743e+006,-3.452748e+005,+2.379756e+004,+0.000000e+000},
{+3.003396e+008,-3.075391e+007,+9.339644e+005,-7.017946e+003},
{+5.462775e+006,-1.213200e+006,+5.677435e+004,-4.994289e+002},
{-2.351651e+007,+1.690655e+006,-3.414655e+004,+3.187491e+002},
{-2.467444e+007,+1.753499e+006,-3.541516e+004,+3.220338e+002},
{-1.450545e+007,+7.268329e+005,-3.060708e+003,+0.000000e+000},
{+3.578927e+007,-4.382693e+006,+1.475727e+005,-1.066008e+003},
{-1.096621e+007,+3.159859e+005,+6.759924e+003,+0.000000e+000},
{-1.482286e+007,+7.697687e+005,-5.920930e+003,+0.000000e+000},
{-1.353926e+007,+6.892985e+005,-4.922390e+003,+0.000000e+000},
{-1.585640e+007,+8.193523e+005,-6.915169e+003,+0.000000e+000},
{-2.091730e+007,+1.281929e+006,-2.166006e+004,+1.561249e+002},
{-1.682020e+007,+8.387164e+005,-7.058292e+003,+0.000000e+000},
{-1.371139e+007,+6.412199e+005,-4.135089e+003,+0.000000e+000},
{-1.317428e+007,+5.910486e+005,-3.312845e+003,+0.000000e+000},
{-1.527150e+007,+7.058618e+005,-5.108053e+003,+0.000000e+000},
{-1.893962e+007,+8.966009e+005,-7.635881e+003,+0.000000e+000},
{-1.501063e+007,+6.731629e+005,-4.715857e+003,+0.000000e+000},
{-1.677653e+007,+7.439650e+005,-5.437238e+003,+0.000000e+000},
{-1.554349e+007,+6.616319e+005,-4.284524e+003,+0.000000e+000},
{-2.030067e+007,+9.246585e+005,-8.056714e+003,+0.000000e+000},
{-2.059148e+007,+9.300864e+005,-8.139347e+003,+0.000000e+000},
{-1.812410e+007,+7.722229e+005,-5.847742e+003,+0.000000e+000},
{-1.698671e+007,+7.030718e+005,-4.949130e+003,+0.000000e+000},
{-1.091269e+007,+3.485818e+005,+0.000000e+000,+0.000000e+000},
{-1.862237e+007,+7.666374e+005,-5.758571e+003,+0.000000e+000},
{-1.053842e+007,+3.292848e+005,+0.000000e+000,+0.000000e+000},
{-2.136943e+007,+8.979092e+005,-7.560643e+003,+0.000000e+000},
{-1.962580e+007,+7.851751e+005,-5.951071e+003,+0.000000e+000},
{-1.086196e+007,+3.225121e+005,+0.000000e+000,+0.000000e+000},
{-2.630957e+007,+1.132011e+006,-1.068638e+004,+0.000000e+000},
{-1.119069e+007,+3.212166e+005,+0.000000e+000,+0.000000e+000},
{-1.796286e+007,+6.417951e+005,-3.830313e+003,+0.000000e+000},
{-1.129583e+007,+3.160500e+005,+0.000000e+000,+0.000000e+000},
{-1.254071e+007,+3.441640e+005,+0.000000e+000,+0.000000e+000},
{-1.238993e+007,+3.357620e+005,+0.000000e+000,+0.000000e+000},
{-1.180338e+007,+3.155950e+005,+0.000000e+000,+0.000000e+000},
{-9.228471e+006,+2.500255e+005,+0.000000e+000,+0.000000e+000},
{-1.315735e+007,+3.420035e+005,+0.000000e+000,+0.000000e+000},
{-1.271111e+007,+3.264835e+005,+0.000000e+000,+0.000000e+000},
{-1.352841e+007,+3.425655e+005,+0.000000e+000,+0.000000e+000},
{-1.338335e+007,+3.333319e+005,+0.000000e+000,+0.000000e+000},
{-1.414444e+007,+3.479542e+005,+0.000000e+000,+0.000000e+000},
{-1.537484e+007,+3.736247e+005,+0.000000e+000,+0.000000e+000},
{-1.716400e+007,+4.120892e+005,+0.000000e+000,+0.000000e+000},
{-1.716400e+007,+4.120892e+005,+0.000000e+000,+0.000000e+000},
{-1.716400e+007,+4.120892e+005,+0.000000e+000,+0.000000e+000},
{-1.716400e+007,+4.120892e+005,+0.000000e+000,+0.000000e+000},
{-1.716400e+007,+4.120892e+005,+0.000000e+000,+0.000000e+000},
{-1.716400e+007,+4.120892e+005,+0.000000e+000,+0.000000e+000},
{-1.716400e+007,+4.120892e+005,+0.000000e+000,+0.000000e+000},
{-1.716400e+007,+4.120892e+005,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
};

/* ---------------------------------------------
     File: tungsten.h
     Date: Mar 13, 1997 (04:58 pm)
     Data for creating XASMIP spectra
     This file for creating TUNGSTEN spectra
--------------------------------------------- */ 
static int tnn[150]={
0,0,0,0,0,0,0,0,0,0,0,4,4,4,4,
4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
4,4,4,4,4,4,4,3,3,3,3,3,3,3,4,
3,3,3,3,3,3,3,3,2,3,3,3,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

static double tenergy[150]={
 0.50, 1.00, 1.50, 2.00, 2.50, 3.00, 3.50, 4.00, 4.50, 5.00,
 5.50, 6.00, 6.50, 7.00, 7.50, 8.00, 8.50, 9.00, 9.50,10.00,
10.50,11.00,11.50,12.00,12.50,13.00,13.50,14.00,14.50,15.00,
15.50,16.00,16.50,17.00,17.50,18.00,18.50,19.00,19.50,20.00,
20.50,21.00,21.50,22.00,22.50,23.00,23.50,24.00,24.50,25.00,
25.50,26.00,26.50,27.00,27.50,28.00,28.50,29.00,29.50,30.00,
30.50,31.00,31.50,32.00,32.50,33.00,33.50,34.00,34.50,35.00,
35.50,36.00,36.50,37.00,37.50,38.00,38.50,39.00,39.50,40.00,
40.50,41.00,41.50,42.00,42.50,43.00,43.50,44.00,44.50,45.00,
45.50, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00};

static double taa[151][4]={
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{-6.873489e+007,+7.187753e+006,-2.159918e+005,+2.221631e+003},
{-6.070034e+007,+6.345849e+006,-1.857021e+005,+1.845997e+003},
{-8.743849e+007,+9.280132e+006,-2.839093e+005,+2.922466e+003},
{-1.026951e+008,+1.081317e+007,-3.267757e+005,+3.355058e+003},
{-2.469562e+008,+2.591247e+007,-8.003514e+005,+8.518917e+003},
{-4.706627e+008,+4.614004e+007,-1.283757e+006,+1.301409e+004},
{-1.560684e+008,+1.572514e+007,-4.455923e+005,+4.439223e+003},
{-3.606819e+008,+3.634391e+007,-1.070835e+006,+1.140357e+004},
{-3.068500e+008,+2.869390e+007,-7.291643e+005,+7.155739e+003},
{-1.276861e+008,+1.288114e+007,-3.660513e+005,+3.644392e+003},
{-1.257411e+008,+1.297512e+007,-3.854823e+005,+3.942138e+003},
{-1.363437e+008,+1.364892e+007,-3.889844e+005,+3.882684e+003},
{-1.029163e+008,+1.055863e+007,-3.115667e+005,+3.143828e+003},
{-8.470663e+007,+8.705748e+006,-2.556132e+005,+2.545929e+003},
{-8.670866e+007,+8.952853e+006,-2.672010e+005,+2.712157e+003},
{-8.561442e+007,+8.769771e+006,-2.593430e+005,+2.613102e+003},
{-8.318715e+007,+8.460735e+006,-2.485027e+005,+2.499402e+003},
{-7.335074e+007,+7.279689e+006,-2.054119e+005,+2.011454e+003},
{-7.627260e+007,+7.608005e+006,-2.192303e+005,+2.200747e+003},
{-7.756584e+007,+7.662825e+006,-2.195321e+005,+2.191898e+003},
{-7.462895e+007,+7.294175e+006,-2.069436e+005,+2.060917e+003},
{-7.097482e+007,+6.859915e+006,-1.925960e+005,+1.912045e+003},
{-7.072362e+007,+6.763036e+006,-1.891083e+005,+1.882858e+003},
{-6.219645e+007,+5.788333e+006,-1.555985e+005,+1.516050e+003},
{-5.502960e+007,+4.926868e+006,-1.247321e+005,+1.166320e+003},
{-6.065437e+007,+5.395190e+006,-1.389085e+005,+1.311125e+003},
{-7.070298e+007,+6.342366e+006,-1.699210e+005,+1.646678e+003},
{-6.271249e+007,+5.382715e+006,-1.357499e+005,+1.262038e+003},
{-5.812485e+007,+4.840952e+006,-1.172847e+005,+1.060619e+003},
{-6.740821e+007,+5.686414e+006,-1.441134e+005,+1.340266e+003},
{-6.699264e+007,+5.464481e+006,-1.336550e+005,+1.206979e+003},
{-7.662571e+007,+6.357553e+006,-1.627368e+005,+1.518921e+003},
{-6.756454e+007,+5.414589e+006,-1.327032e+005,+1.210007e+003},
{-7.591042e+007,+6.173340e+006,-1.570560e+005,+1.469012e+003},
{-8.531624e+007,+6.865103e+006,-1.744268e+005,+1.609156e+003},
{-8.404450e+007,+6.628089e+006,-1.650829e+005,+1.498164e+003},
{-8.668162e+007,+6.854728e+006,-1.730579e+005,+1.590682e+003},
{-6.495929e+007,+4.651735e+006,-1.020722e+005,+8.478419e+002},
{-1.070874e+008,+8.469419e+006,-2.174418e+005,+1.997402e+003},
{-9.403950e+007,+7.084522e+006,-1.716615e+005,+1.508351e+003},
{-8.754474e+007,+6.488524e+006,-1.551625e+005,+1.359698e+003},
{-3.529739e+007,+1.744532e+006,-1.503964e+004,+0.000000e+000},
{-3.813298e+007,+1.870898e+006,-1.668576e+004,+0.000000e+000},
{-4.450025e+007,+2.221201e+006,-2.179242e+004,+0.000000e+000},
{-4.272417e+007,+2.092002e+006,-1.996507e+004,+0.000000e+000},
{-4.416447e+007,+2.137812e+006,-2.045019e+004,+0.000000e+000},
{-3.691062e+007,+1.668331e+006,-1.338276e+004,+0.000000e+000},
{-3.907555e+007,+1.774209e+006,-1.498448e+004,+0.000000e+000},
{-1.195746e+008,+8.396072e+006,-1.961298e+005,+1.641245e+003},
{-4.365839e+007,+1.975074e+006,-1.762596e+004,+0.000000e+000},
{-4.651813e+007,+2.091356e+006,-1.891025e+004,+0.000000e+000},
{-5.022499e+007,+2.257714e+006,-2.101889e+004,+0.000000e+000},
{-4.846810e+007,+2.147030e+006,-1.960649e+004,+0.000000e+000},
{-4.732332e+007,+2.059328e+006,-1.836286e+004,+0.000000e+000},
{-5.582990e+007,+2.475062e+006,-2.368736e+004,+0.000000e+000},
{-6.325837e+007,+2.830780e+006,-2.814664e+004,+0.000000e+000},
{-6.455889e+007,+2.905371e+006,-2.948107e+004,+0.000000e+000},
{-2.215968e+007,+6.551200e+005,+0.000000e+000,+0.000000e+000},
{-5.679439e+007,+2.407766e+006,-2.232638e+004,+0.000000e+000},
{-8.115400e+007,+3.604377e+006,-3.716713e+004,+0.000000e+000},
{-7.735838e+007,+3.421578e+006,-3.521819e+004,+0.000000e+000},
{-2.213949e+007,+6.210240e+005,+0.000000e+000,+0.000000e+000},
{-2.489510e+007,+6.823500e+005,+0.000000e+000,+0.000000e+000},
{-2.619137e+007,+7.038040e+005,+0.000000e+000,+0.000000e+000},
{-2.658188e+007,+7.053680e+005,+0.000000e+000,+0.000000e+000},
{-2.316312e+007,+6.122530e+005,+0.000000e+000,+0.000000e+000},
{-2.999583e+007,+7.719365e+005,+0.000000e+000,+0.000000e+000},
{-2.840658e+007,+7.219850e+005,+0.000000e+000,+0.000000e+000},
{-2.741783e+007,+6.895365e+005,+0.000000e+000,+0.000000e+000},
{-3.342120e+007,+8.262318e+005,+0.000000e+000,+0.000000e+000},
{-3.816755e+007,+9.320471e+005,+0.000000e+000,+0.000000e+000},
{-3.830466e+007,+9.241070e+005,+0.000000e+000,+0.000000e+000},
{-2.052079e+008,+4.891130e+006,+0.000000e+000,+0.000000e+000},
{-2.052079e+008,+4.891130e+006,+0.000000e+000,+0.000000e+000},
{-2.052079e+008,+4.891130e+006,+0.000000e+000,+0.000000e+000},
{-2.052079e+008,+4.891130e+006,+0.000000e+000,+0.000000e+000},
{-2.052079e+008,+4.891130e+006,+0.000000e+000,+0.000000e+000},
{-2.052079e+008,+4.891130e+006,+0.000000e+000,+0.000000e+000},
{-2.052079e+008,+4.891130e+006,+0.000000e+000,+0.000000e+000},
{-2.052079e+008,+4.891130e+006,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
{+0.000000e+000,+0.000000e+000,+0.000000e+000,+0.000000e+000},
};

static int mamspec(int itube,double xkv,double en[],double aspec[])
{
	double arg,dsum,sum1,sum2;
	double *aa[151],mr,xval,e1,e2,energy,w1,w2;
	double aenergy[150];
	double x,y,z,xmax,xmin,sum,factor,t0,t1,t2,t3,t4,t5,t6;
	int ien,nterms,nt,*nn,iemax,kv,ikv,genflag,iflag;
	int i,j,k,l,m,n,ians,ival,jj,j1,j2,npts;
/* ----------------------------------------------------
	If input range is outside limits, reject
---------------------------------------------------- */
	if( xkv < 18. || xkv > 42. ) {
		fprintf(stderr, "X-ray tube voltage must be between 18 and 42 keV\n");
		return 0;
	} 
/* ----------------------------------------------------
	Transfer Coefficients for Molybdenum Anode (itube=1)
	into working arrays
---------------------------------------------------- */
	if( itube==XMI_TUBE_BOONE_MOLYBDENUM ) { 
		for( i=0; i<90; ++i ) {
			en[i] = menergy[i];
			aa[i] = maa[i];
		}
		nn = mnn;
	}
/* ----------------------------------------------------
	Transfer Coefficients for Rhodium Anode (itube=2)
	into working arrays
---------------------------------------------------- */
	else if( itube==XMI_TUBE_BOONE_RHODIUM ) { 
		for( i=0; i<90; ++i ) {
			en[i] = renergy[i];
			aa[i] = raa[i];
		}
		nn = rnn;
	}
/* ----------------------------------------------------
	Transfer Coefficients for Tungsten Anode (itube=3)
	into working arrays
---------------------------------------------------- */
	else if( itube==XMI_TUBE_BOONE_TUNGSTEN ) { 
		for( i=0; i<90; ++i ) {
			en[i] = tenergy[i];
			aa[i] = taa[i];
		}
		nn = tnn;
	}
	else {
		fprintf(stderr, "Unknown tube type detected\n");
		return 0;
	}
/* ----------------------------------------------------
	If input range is outside limits, reject
---------------------------------------------------- */
	npts = 90;
	for( n=0; n<npts; ++n ) {
		energy = en[n];
		aspec[n] = 0.0;
		if( energy==0.0 || energy > xkv )
			continue;
		arg = xkv;
		dsum = aa[n][0];
		for( j=1; j<4; ++j ) {
			dsum += aa[n][j] * arg;
			arg = arg * xkv;
			}
		if( dsum < 0.0 ) dsum = 0.0;
		aspec[n] = dsum;
	}	
	return 1;
}

int xmi_tube_boone_1863(int tube_type, struct xmi_layer *tube_window,
		struct xmi_layer *tube_filter, double tube_voltage,
		  double tube_current, double tube_solid_angle, 
		  struct xmi_excitation **boone_spectrum
		) {
		
	double *spec, *en;
	struct xmi_excitation *boone_spectrum_local = NULL;
	int i;
	const double mm_square_to_sterad = 4.0 * atan(1.0/2000.0/sqrt(4.0*1000.0*1000.0 + 1.0 + 1.0));
	double intensity;

	spec = malloc(sizeof(double)*90);
	en = malloc(sizeof(double)*90);

	if (mamspec(tube_type, tube_voltage, en, spec) == 0)
		return 0;
		
	
	boone_spectrum_local = (struct xmi_excitation *) malloc(sizeof(struct xmi_excitation));

	boone_spectrum_local->n_discrete = 0;
	boone_spectrum_local->discrete = NULL;
	boone_spectrum_local->n_continuous= 0;
	boone_spectrum_local->continuous = NULL;

	for (i = 0 ; i < 90 ; i++) {
		intensity = spec[i];
		if (intensity == 0.0)
			continue;

		intensity /= mm_square_to_sterad;
		intensity /= 4.0;
		intensity *= tube_solid_angle;
		intensity *= tube_current;
		if( tube_type==XMI_TUBE_BOONE_MOLYBDENUM) { 
			intensity /= 26;
		}
		else if( tube_type==XMI_TUBE_BOONE_RHODIUM) { 
			intensity /= 24;
		}
		else if( tube_type==XMI_TUBE_BOONE_TUNGSTEN) { 
			intensity /= 37;
		}

		if (tube_window) {
			intensity *= exp(-1.0 * tube_window->density * CS_Total_Kissel(tube_window->Z[0], (float) en[i]) * tube_window->thickness);
		}

		if (tube_filter) {
			intensity *= exp(-1.0 * tube_filter->density * CS_Total_Kissel(tube_filter->Z[0], (float) en[i]) * tube_filter->thickness);
		}

		if (intensity <= 0.0)
			continue;

		boone_spectrum_local->continuous = realloc(boone_spectrum_local->continuous, sizeof(struct xmi_energy_continuous)*++boone_spectrum_local->n_continuous);
		boone_spectrum_local->continuous[boone_spectrum_local->n_continuous-1].energy = en[i];
		boone_spectrum_local->continuous[boone_spectrum_local->n_continuous-1].horizontal_intensity = 
		boone_spectrum_local->continuous[boone_spectrum_local->n_continuous-1].vertical_intensity = intensity/2.0;
		boone_spectrum_local->continuous[boone_spectrum_local->n_continuous-1].sigma_x = 
		boone_spectrum_local->continuous[boone_spectrum_local->n_continuous-1].sigma_xp = 
		boone_spectrum_local->continuous[boone_spectrum_local->n_continuous-1].sigma_y = 
		boone_spectrum_local->continuous[boone_spectrum_local->n_continuous-1].sigma_yp = 0.0;

	}

	free(en);
	free(spec);

	*boone_spectrum = boone_spectrum_local;
	return 1;


}

