/**************************************************************************************************
*                                                                                                 *
* This file is part of HPMPC.                                                                     *
*                                                                                                 *
* HPMPC -- Library for High-Performance implementation of solvers for MPC.                        *
* Copyright (C) 2014 by Technical University of Denmark. All rights reserved.                     *
*                                                                                                 *
* HPMPC is free software; you can redistribute it and/or                                          *
* modify it under the terms of the GNU Lesser General Public                                      *
* License as published by the Free Software Foundation; either                                    *
* version 2.1 of the License, or (at your option) any later version.                              *
*                                                                                                 *
* HPMPC is distributed in the hope that it will be useful,                                        *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                                  *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                                            *
* See the GNU Lesser General Public License for more details.                                     *
*                                                                                                 *
* You should have received a copy of the GNU Lesser General Public                                *
* License along with HPMPC; if not, write to the Free Software                                    *
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA                  *
*                                                                                                 *
* Author: Gianluca Frison, giaf (at) dtu.dk                                                       *
*                                                                                                 *
**************************************************************************************************/



// transposed of general matrices, read along panels, write across panels TODO test if it is the best way
void kernel_dgetr_4_lib4(int kmax, int kna, double *A, double *C, int sdc)
	{

	const int bs = 4;
	
	int k;

	k = 0;
	if(kna>0)
		{
		for( ; k<kna; k++)
			{
			C[0+bs*0] = A[0+bs*0];
			C[0+bs*1] = A[1+bs*0];
			C[0+bs*2] = A[2+bs*0];
			C[0+bs*3] = A[3+bs*0];

			C += 1;
			A += bs;
			}
		C += bs*(sdc-1);
		}
	
	for( ; k<kmax-3; k+=4)
		{
		C[0+bs*0] = A[0+bs*0];
		C[0+bs*1] = A[1+bs*0];
		C[0+bs*2] = A[2+bs*0];
		C[0+bs*3] = A[3+bs*0];

		C[1+bs*0] = A[0+bs*1];
		C[1+bs*1] = A[1+bs*1];
		C[1+bs*2] = A[2+bs*1];
		C[1+bs*3] = A[3+bs*1];

		C[2+bs*0] = A[0+bs*2];
		C[2+bs*1] = A[1+bs*2];
		C[2+bs*2] = A[2+bs*2];
		C[2+bs*3] = A[3+bs*2];

		C[3+bs*0] = A[0+bs*3];
		C[3+bs*1] = A[1+bs*3];
		C[3+bs*2] = A[2+bs*3];
		C[3+bs*3] = A[3+bs*3];

		C += bs*sdc;
		A += bs*bs;
		}
	for( ; k<kmax; k++)
		{
		C[0+bs*0] = A[0+bs*0];
		C[0+bs*1] = A[1+bs*0];
		C[0+bs*2] = A[2+bs*0];
		C[0+bs*3] = A[3+bs*0];

		C += 1;
		A += bs;
		}

	}



// transposed of general matrices, read along panels, write across panels TODO test if it is the best way
void kernel_dgetr_3_lib4(int kmax, int kna, double *A, double *C, int sdc)
	{

	const int bs = 4;
	
	int k;

	k = 0;
	if(kna>0)
		{
		for( ; k<kna; k++)
			{
			C[0+bs*0] = A[0+bs*0];
			C[0+bs*1] = A[1+bs*0];
			C[0+bs*2] = A[2+bs*0];

			C += 1;
			A += bs;
			}
		C += bs*(sdc-1);
		}
	
	for( ; k<kmax-3; k+=4)
		{
		C[0+bs*0] = A[0+bs*0];
		C[0+bs*1] = A[1+bs*0];
		C[0+bs*2] = A[2+bs*0];

		C[1+bs*0] = A[0+bs*1];
		C[1+bs*1] = A[1+bs*1];
		C[1+bs*2] = A[2+bs*1];

		C[2+bs*0] = A[0+bs*2];
		C[2+bs*1] = A[1+bs*2];
		C[2+bs*2] = A[2+bs*2];

		C[3+bs*0] = A[0+bs*3];
		C[3+bs*1] = A[1+bs*3];
		C[3+bs*2] = A[2+bs*3];

		C += bs*sdc;
		A += bs*bs;
		}
	for( ; k<kmax; k++)
		{
		C[0+bs*0] = A[0+bs*0];
		C[0+bs*1] = A[1+bs*0];
		C[0+bs*2] = A[2+bs*0];

		C += 1;
		A += bs;
		}

	}



// transposed of general matrices, read along panels, write across panels TODO test if it is the best way
void kernel_dgetr_2_lib4(int kmax, int kna, double *A, double *C, int sdc)
	{

	const int bs = 4;
	
	int k;

	k = 0;
	if(kna>0)
		{
		for( ; k<kna; k++)
			{
			C[0+bs*0] = A[0+bs*0];
			C[0+bs*1] = A[1+bs*0];

			C += 1;
			A += bs;
			}
		C += bs*(sdc-1);
		}
	
	for( ; k<kmax-3; k+=4)
		{
		C[0+bs*0] = A[0+bs*0];
		C[0+bs*1] = A[1+bs*0];

		C[1+bs*0] = A[0+bs*1];
		C[1+bs*1] = A[1+bs*1];

		C[2+bs*0] = A[0+bs*2];
		C[2+bs*1] = A[1+bs*2];

		C[3+bs*0] = A[0+bs*3];
		C[3+bs*1] = A[1+bs*3];

		C += bs*sdc;
		A += bs*bs;
		}
	for( ; k<kmax; k++)
		{
		C[0+bs*0] = A[0+bs*0];
		C[0+bs*1] = A[1+bs*0];

		C += 1;
		A += bs;
		}

	}



// transposed of general matrices, read along panels, write across panels TODO test if it is the best way
void kernel_dgetr_1_lib4(int kmax, int kna, double *A, double *C, int sdc)
	{

	const int bs = 4;
	
	int k;

	k = 0;
	if(kna>0)
		{
		for( ; k<kna; k++)
			{
			C[0+bs*0] = A[0+bs*0];

			C += 1;
			A += bs;
			}
		C += bs*(sdc-1);
		}
	
	for( ; k<kmax-3; k+=4)
		{
		C[0+bs*0] = A[0+bs*0];

		C[1+bs*0] = A[0+bs*1];

		C[2+bs*0] = A[0+bs*2];

		C[3+bs*0] = A[0+bs*3];

		C += bs*sdc;
		A += bs*bs;
		}
	for( ; k<kmax; k++)
		{
		C[0+bs*0] = A[0+bs*0];

		C += 1;
		A += bs;
		}

	}



void kernel_dtran_4_lib4(int kmax, int kna, double *A, int sda, double *C)
	{
	
	// kmax is at least 4 !!!
	
	int k;

	const int bs = 4;
	
	k=0;

	if(kna==0)
		{

		C[0+bs*0] = A[0+bs*0];

		C[0+bs*1] = A[1+bs*0];
		C[1+bs*1] = A[1+bs*1];
		
		C[0+bs*2] = A[2+bs*0];
		C[1+bs*2] = A[2+bs*1];
		C[2+bs*2] = A[2+bs*2];

		C[0+bs*3] = A[3+bs*0];
		C[1+bs*3] = A[3+bs*1];
		C[2+bs*3] = A[3+bs*2];
		C[3+bs*3] = A[3+bs*3];
		
		A += 4*sda;
		C += 4*bs;
		k += 4;
		
		}
	else if(kna==1)
		{
		
		// top 1x1 triangle
		C[0+bs*0] = A[0+bs*0];
		
		A += 1 + bs*(sda-1);
		C += bs;
		k += 1;

		// 4x4
		C[0+bs*0] = A[0+bs*0];
		C[1+bs*0] = A[0+bs*1];
		
		C[0+bs*1] = A[1+bs*0];
		C[1+bs*1] = A[1+bs*1];
		C[2+bs*1] = A[1+bs*2];
		
		C[0+bs*2] = A[2+bs*0];
		C[1+bs*2] = A[2+bs*1];
		C[2+bs*2] = A[2+bs*2];
		C[3+bs*2] = A[2+bs*3];
		
		if(kmax==4)
			return;

		C[0+bs*3] = A[3+bs*0];
		C[1+bs*3] = A[3+bs*1];
		C[2+bs*3] = A[3+bs*2];
		C[3+bs*3] = A[3+bs*3];

		A += bs*sda;
		C += 4*bs;
		k += 4;

		}
	else if(kna==2)
		{

		// top 2x2 triangle
		C[0+bs*0] = A[0+bs*0];

		C[0+bs*1] = A[1+bs*0];
		C[1+bs*1] = A[1+bs*1];

		A += 2 + bs*(sda-1);
		C += 2*bs;
		k += 2;

		C[0+bs*0] = A[0+bs*0];
		C[1+bs*0] = A[0+bs*1];
		C[2+bs*0] = A[0+bs*2];
		
		C[0+bs*1] = A[1+bs*0];
		C[1+bs*1] = A[1+bs*1];
		C[2+bs*1] = A[1+bs*2];
		C[3+bs*1] = A[1+bs*3];
		
		if(kmax==4)
			return;

		C[0+bs*2] = A[2+bs*0];
		C[1+bs*2] = A[2+bs*1];
		C[2+bs*2] = A[2+bs*2];
		C[3+bs*2] = A[2+bs*3];
		
		if(kmax==5)
			return;

		C[0+bs*3] = A[3+bs*0];
		C[1+bs*3] = A[3+bs*1];
		C[2+bs*3] = A[3+bs*2];
		C[3+bs*3] = A[3+bs*3];

		A += bs*sda;
		C += 4*bs;
		k += 4;

		}
	else // if(kna==3)
		{

		// top 1x1 triangle
		C[0+bs*0] = A[0+bs*0];

		// 2x2 square
		C[0+bs*1] = A[1+bs*0];
		C[0+bs*2] = A[2+bs*0];
		C[1+bs*1] = A[1+bs*1];
		C[1+bs*2] = A[2+bs*1];

		// low 1x1 triangle
		C[2+bs*2] = A[2+bs*2];

		A += 3 + bs*(sda-1);
		C += 3*bs;
		k += 3;

		}

	__builtin_prefetch( A );
#if defined(TARGET_CORTEX_A9)
	__builtin_prefetch( A+4 );
#endif		
	__builtin_prefetch( A+8 );
#if defined(TARGET_CORTEX_A9)
	__builtin_prefetch( A+12 );
#endif		

	for(; k<kmax-3; k+=4)
		{
		
		__builtin_prefetch( A+bs*sda );
#if defined(TARGET_CORTEX_A9)
		__builtin_prefetch( A+bs*sda+4 );
#endif		
		__builtin_prefetch( A+bs*sda+8 );
#if defined(TARGET_CORTEX_A9)
		__builtin_prefetch( A+bs*sda+12 );
#endif		

		C[0+bs*0] = A[0+bs*0];
		C[1+bs*0] = A[0+bs*1];
		C[2+bs*0] = A[0+bs*2];
		C[3+bs*0] = A[0+bs*3];
		
		C[0+bs*1] = A[1+bs*0];
		C[1+bs*1] = A[1+bs*1];
		C[2+bs*1] = A[1+bs*2];
		C[3+bs*1] = A[1+bs*3];
		
		C[0+bs*2] = A[2+bs*0];
		C[1+bs*2] = A[2+bs*1];
		C[2+bs*2] = A[2+bs*2];
		C[3+bs*2] = A[2+bs*3];
		
		C[0+bs*3] = A[3+bs*0];
		C[1+bs*3] = A[3+bs*1];
		C[2+bs*3] = A[3+bs*2];
		C[3+bs*3] = A[3+bs*3];
	
		A += bs*sda;
		C += 4*bs;

		}

	if(k==kmax)
		return;

	if(kmax-k==1)
		{
		
		C[0+bs*0] = A[0+bs*0];
		C[1+bs*0] = A[0+bs*1];
		C[2+bs*0] = A[0+bs*2];
		C[3+bs*0] = A[0+bs*3];

		}
	else if(kmax-k==2)
		{
		
		C[0+bs*0] = A[0+bs*0];
		C[1+bs*0] = A[0+bs*1];
		C[2+bs*0] = A[0+bs*2];
		C[3+bs*0] = A[0+bs*3];
		
		C[0+bs*1] = A[1+bs*0];
		C[1+bs*1] = A[1+bs*1];
		C[2+bs*1] = A[1+bs*2];
		C[3+bs*1] = A[1+bs*3];

		}
	else // if(kmax-k==3)
		{

		C[0+bs*0] = A[0+bs*0];
		C[1+bs*0] = A[0+bs*1];
		C[2+bs*0] = A[0+bs*2];
		C[3+bs*0] = A[0+bs*3];
		
		C[0+bs*1] = A[1+bs*0];
		C[1+bs*1] = A[1+bs*1];
		C[2+bs*1] = A[1+bs*2];
		C[3+bs*1] = A[1+bs*3];
		
		C[0+bs*2] = A[2+bs*0];
		C[1+bs*2] = A[2+bs*1];
		C[2+bs*2] = A[2+bs*2];
		C[3+bs*2] = A[2+bs*3];

		}

	return;
	
	}



void corner_dtran_3_lib4(int kna, double *A, int sda, double *C)
	{

	const int bs = 4;
	
	if(kna==0)
		{
		
		C[0+bs*0] = A[0+bs*0];
		
		C[0+bs*1] = A[1+bs*0];
		C[1+bs*1] = A[1+bs*1];
		
		C[0+bs*2] = A[2+bs*0];
		C[1+bs*2] = A[2+bs*1];
		C[2+bs*2] = A[2+bs*2];

		}
	else if(kna==1)
		{
		
		C[0+bs*0] = A[0+bs*0];
		
		A += 1 + bs*(sda-1);
		C += bs;

		C[0+bs*0] = A[0+bs*0];
		C[1+bs*0] = A[0+bs*1];
		
		C[0+bs*1] = A[1+bs*0];
		C[1+bs*1] = A[1+bs*1];
		C[2+bs*1] = A[1+bs*2];

		}
	else if(kna==2)
		{

		C[0+bs*0] = A[0+bs*0];
		
		C[0+bs*1] = A[1+bs*0];
		C[1+bs*1] = A[1+bs*1];

		A += 2 + bs*(sda-1);
		C += 2*bs;

		C[0+bs*0] = A[0+bs*0];
		C[1+bs*0] = A[0+bs*1];
		C[2+bs*0] = A[0+bs*2];

		}
	else // if(kna==3)
		{

		C[0+bs*0] = A[0+bs*0];
		
		C[0+bs*1] = A[1+bs*0];
		C[1+bs*1] = A[1+bs*1];
		
		C[0+bs*2] = A[2+bs*0];
		C[1+bs*2] = A[2+bs*1];
		C[2+bs*2] = A[2+bs*2];

		}

	}



void corner_dtran_2_lib4(int kna, double *A, int sda, double *C)
	{

	const int bs = 4;
	
	if(kna==1)
		{

		C[0+bs*0] = A[0+bs*0];
		
		A += 1 + bs*(sda-1);
		C += bs;

		C[0+bs*0] = A[0+bs*0];
		C[1+bs*0] = A[0+bs*1];

		}
	else // if(kna==3)
		{
		
		C[0+bs*0] = A[0+bs*0];
		
		C[0+bs*1] = A[1+bs*0];
		C[1+bs*1] = A[1+bs*1];

		}

	}




