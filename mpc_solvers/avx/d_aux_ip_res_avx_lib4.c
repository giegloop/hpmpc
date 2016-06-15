/**************************************************************************************************
*                                                                                                 *
* This file is part of HPMPC.                                                                     *
*                                                                                                 *
* HPMPC -- Library for High-Performance implementation of solvers for MPC.                        *
* Copyright (C) 2014-2015 by Technical University of Denmark. All rights reserved.                *
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

#include <mmintrin.h>
#include <xmmintrin.h>  // SSE
#include <emmintrin.h>  // SSE2
#include <pmmintrin.h>  // SSE3
#include <smmintrin.h>  // SSE4
#include <immintrin.h>  // AVX

#include <math.h>
#include "../../include/blas_d.h"
#include "../../include/block_size.h"



void d_update_hessian_gradient_res_mpc_hard_tv(int N, int *nx, int *nu, int *nb, int *ng, double **res_d, double **res_m, double **t, double **lam, double **t_inv, double **Qx, double **qx)
	{
	
	// constants
	const int bs = D_MR;

	int nb0, pnb, ng0, png;
	
	double 
		*ptr_res_d, *ptr_Qx, *ptr_qx, *ptr_t, *ptr_lam, *ptr_res_m, *ptr_t_inv;
	
	__m256d
		v_ones,
		v_tmp0, v_tinv0, v_lam0, v_resm0, v_resd0,
		v_tmp1, v_tinv1, v_lam1, v_resm1, v_resd1;
	
	__m256i
		i_mask;
	
	v_ones = _mm256_set_pd( 1.0, 1.0, 1.0, 1.0 );

	int ii, jj, bs0;
	
	double ii_left;

	static double d_mask[4] = {0.5, 1.5, 2.5, 3.5};

	for(jj=0; jj<=N; jj++)
		{
		
		ptr_t     = t[jj];
		ptr_lam   = lam[jj];
		ptr_t_inv = t_inv[jj];
		ptr_res_d = res_d[jj];
		ptr_res_m = res_m[jj];
		ptr_Qx    = Qx[jj];
		ptr_qx    = qx[jj];

		// box constraints
		nb0 = nb[jj];
		if(nb0>0)
			{

			pnb  = (nb0+bs-1)/bs*bs; // simd aligned number of box constraints

			for(ii=0; ii<nb0-3; ii+=4)
				{

				v_tinv0 = _mm256_load_pd( &ptr_t[ii+0] );
				v_tinv1 = _mm256_load_pd( &ptr_t[ii+pnb] );
				v_tinv0 = _mm256_div_pd( v_ones, v_tinv0 );
				v_tinv1 = _mm256_div_pd( v_ones, v_tinv1 );
				v_lam0  = _mm256_load_pd( &ptr_lam[ii+0] );
				v_lam1  = _mm256_load_pd( &ptr_lam[ii+pnb] );
				v_resm0 = _mm256_load_pd( &ptr_res_m[ii+0] );
				v_resm1 = _mm256_load_pd( &ptr_res_m[ii+pnb] );
				v_resd0 = _mm256_load_pd( &ptr_res_d[ii+0] );
				v_resd1 = _mm256_load_pd( &ptr_res_d[ii+pnb] );
				v_tmp0  = _mm256_mul_pd( v_tinv0, v_lam0 );
				v_tmp1  = _mm256_mul_pd( v_tinv1, v_lam1 );
				_mm256_store_pd( &ptr_t_inv[ii+0], v_tinv0 );
				_mm256_store_pd( &ptr_t_inv[ii+pnb], v_tinv1 );
				v_tmp0  = _mm256_add_pd( v_tmp0, v_tmp1 );
				_mm256_store_pd( &ptr_Qx[ii+0], v_tmp0 );
				v_tmp0  = _mm256_mul_pd( v_lam0, v_resd0 );
				v_tmp1  = _mm256_mul_pd( v_lam1, v_resd1 );
				v_tmp0  = _mm256_sub_pd( v_resm0, v_tmp0 );
				v_tmp1  = _mm256_add_pd( v_resm1, v_tmp1 );
				v_tmp0  = _mm256_mul_pd( v_tmp0, v_tinv0 );
				v_tmp1  = _mm256_mul_pd( v_tmp1, v_tinv1 );
				v_tmp0  = _mm256_sub_pd( v_tmp0, v_tmp1 );
				_mm256_store_pd( &ptr_qx[ii+0], v_tmp0 );

				}
			if(ii<nb0)
				{

				ii_left = nb0-ii;
				i_mask  = _mm256_castpd_si256( _mm256_sub_pd( _mm256_loadu_pd( d_mask ), _mm256_broadcast_sd( &ii_left ) ) );

				v_tinv0 = _mm256_load_pd( &ptr_t[ii+0] );
				v_tinv1 = _mm256_load_pd( &ptr_t[ii+pnb] );
				v_tinv0 = _mm256_div_pd( v_ones, v_tinv0 );
				v_tinv1 = _mm256_div_pd( v_ones, v_tinv1 );
				v_lam0  = _mm256_load_pd( &ptr_lam[ii+0] );
				v_lam1  = _mm256_load_pd( &ptr_lam[ii+pnb] );
				v_resm0 = _mm256_load_pd( &ptr_res_m[ii+0] );
				v_resm1 = _mm256_load_pd( &ptr_res_m[ii+pnb] );
				v_resd0 = _mm256_load_pd( &ptr_res_d[ii+0] );
				v_resd1 = _mm256_load_pd( &ptr_res_d[ii+pnb] );
				v_tmp0  = _mm256_mul_pd( v_tinv0, v_lam0 );
				v_tmp1  = _mm256_mul_pd( v_tinv1, v_lam1 );
				_mm256_maskstore_pd( &ptr_t_inv[ii+0], i_mask, v_tinv0 );
				_mm256_maskstore_pd( &ptr_t_inv[ii+pnb], i_mask, v_tinv1 );
				v_tmp0  = _mm256_add_pd( v_tmp0, v_tmp1 );
				_mm256_maskstore_pd( &ptr_Qx[ii+0], i_mask, v_tmp0 );
				v_tmp0  = _mm256_mul_pd( v_lam0, v_resd0 );
				v_tmp1  = _mm256_mul_pd( v_lam1, v_resd1 );
				v_tmp0  = _mm256_sub_pd( v_resm0, v_tmp0 );
				v_tmp1  = _mm256_add_pd( v_resm1, v_tmp1 );
				v_tmp0  = _mm256_mul_pd( v_tmp0, v_tinv0 );
				v_tmp1  = _mm256_mul_pd( v_tmp1, v_tinv1 );
				v_tmp0  = _mm256_sub_pd( v_tmp0, v_tmp1 );
				_mm256_maskstore_pd( &ptr_qx[ii+0], i_mask, v_tmp0 );

				}

			ptr_t     += 2*pnb;
			ptr_lam   += 2*pnb;
			ptr_t_inv += 2*pnb;
			ptr_res_d += 2*pnb;
			ptr_res_m += 2*pnb;
			ptr_Qx    += pnb;
			ptr_qx    += pnb;

			}

		// general constraints
		ng0 = ng[jj];
		if(ng0>0)
			{

		
			png = (ng0+bs-1)/bs*bs; // simd aligned number of general constraints

			for(ii=0; ii<ng0-3; ii+=4)
				{

				v_tinv0 = _mm256_load_pd( &ptr_t[ii+0] );
				v_tinv1 = _mm256_load_pd( &ptr_t[ii+png] );
				v_tinv0 = _mm256_div_pd( v_ones, v_tinv0 );
				v_tinv1 = _mm256_div_pd( v_ones, v_tinv1 );
				v_lam0  = _mm256_load_pd( &ptr_lam[ii+0] );
				v_lam1  = _mm256_load_pd( &ptr_lam[ii+png] );
				v_resm0 = _mm256_load_pd( &ptr_res_m[ii+0] );
				v_resm1 = _mm256_load_pd( &ptr_res_m[ii+png] );
				v_resd0 = _mm256_load_pd( &ptr_res_d[ii+0] );
				v_resd1 = _mm256_load_pd( &ptr_res_d[ii+png] );
				v_tmp0  = _mm256_mul_pd( v_tinv0, v_lam0 );
				v_tmp1  = _mm256_mul_pd( v_tinv1, v_lam1 );
				_mm256_store_pd( &ptr_t_inv[ii+0], v_tinv0 );
				_mm256_store_pd( &ptr_t_inv[ii+png], v_tinv1 );
				v_tmp0  = _mm256_add_pd( v_tmp0, v_tmp1 );
				_mm256_store_pd( &ptr_Qx[ii+0], v_tmp0 );
				v_tmp0  = _mm256_mul_pd( v_lam0, v_resd0 );
				v_tmp1  = _mm256_mul_pd( v_lam1, v_resd1 );
				v_tmp0  = _mm256_sub_pd( v_resm0, v_tmp0 );
				v_tmp1  = _mm256_add_pd( v_resm1, v_tmp1 );
				v_tmp0  = _mm256_mul_pd( v_tmp0, v_tinv0 );
				v_tmp1  = _mm256_mul_pd( v_tmp1, v_tinv1 );
				v_tmp0  = _mm256_sub_pd( v_tmp0, v_tmp1 );
				_mm256_store_pd( &ptr_qx[ii+0], v_tmp0 );

				}
			if(ii<ng0)
				{

				ii_left = ng0-ii;
				i_mask  = _mm256_castpd_si256( _mm256_sub_pd( _mm256_loadu_pd( d_mask ), _mm256_broadcast_sd( &ii_left ) ) );

				v_tinv0 = _mm256_load_pd( &ptr_t[ii+0] );
				v_tinv1 = _mm256_load_pd( &ptr_t[ii+png] );
				v_tinv0 = _mm256_div_pd( v_ones, v_tinv0 );
				v_tinv1 = _mm256_div_pd( v_ones, v_tinv1 );
				v_lam0  = _mm256_load_pd( &ptr_lam[ii+0] );
				v_lam1  = _mm256_load_pd( &ptr_lam[ii+png] );
				v_resm0 = _mm256_load_pd( &ptr_res_m[ii+0] );
				v_resm1 = _mm256_load_pd( &ptr_res_m[ii+png] );
				v_resd0 = _mm256_load_pd( &ptr_res_d[ii+0] );
				v_resd1 = _mm256_load_pd( &ptr_res_d[ii+png] );
				v_tmp0  = _mm256_mul_pd( v_tinv0, v_lam0 );
				v_tmp1  = _mm256_mul_pd( v_tinv1, v_lam1 );
				_mm256_maskstore_pd( &ptr_t_inv[ii+0], i_mask, v_tinv0 );
				_mm256_maskstore_pd( &ptr_t_inv[ii+png], i_mask, v_tinv1 );
				v_tmp0  = _mm256_add_pd( v_tmp0, v_tmp1 );
				_mm256_maskstore_pd( &ptr_Qx[ii+0], i_mask, v_tmp0 );
				v_tmp0  = _mm256_mul_pd( v_lam0, v_resd0 );
				v_tmp1  = _mm256_mul_pd( v_lam1, v_resd1 );
				v_tmp0  = _mm256_sub_pd( v_resm0, v_tmp0 );
				v_tmp1  = _mm256_add_pd( v_resm1, v_tmp1 );
				v_tmp0  = _mm256_mul_pd( v_tmp0, v_tinv0 );
				v_tmp1  = _mm256_mul_pd( v_tmp1, v_tinv1 );
				v_tmp0  = _mm256_sub_pd( v_tmp0, v_tmp1 );
				_mm256_maskstore_pd( &ptr_qx[ii+0], i_mask, v_tmp0 );

				}

			}

		}

	}



void d_compute_alpha_res_mpc_hard_tv(int N, int *nx, int *nu, int *nb, int **idxb, int *ng, double **dux, double **t, double **t_inv, double **lam, double **pDCt, double **res_d, double **res_m, double **dt, double **dlam, double *ptr_alpha)
	{
	
	// constants
	const int bs = D_MR;
	const int ncl = D_NCL;

	int nu0, nx0, nb0, pnb, ng0, png, cng;

	double alpha = ptr_alpha[0];
	
	double
		*ptr_res_d, *ptr_res_m, *ptr_dux, *ptr_t, *ptr_t_inv, *ptr_dt, *ptr_lam, *ptr_dlam;
	
	int
		*ptr_idxb;
	
	int jj, ll;

	__m128d
		u_dux, u_alpha,
		u_resm0, u_resd0, u_dt0, u_dlam0, u_tmp0, u_tinv0, u_lam0, u_t0,
		u_resm1, u_resd1, u_dt1, u_dlam1, u_tmp1, u_tinv1, u_lam1, u_t1;
	
	__m256d
		v_dux, v_sign, v_alpha,
		v_resm0, v_resd0, v_dt0, v_dlam0, v_tmp0, v_tinv0, v_lam0, v_t0,
		v_resm1, v_resd1, v_dt1, v_dlam1, v_tmp1, v_tinv1, v_lam1, v_t1;
	
	__m128
		s_dlam, s_lam, s_mask0, s_tmp0,
		s_alpha0,
		s_alpha1;

	__m256
		t_dlam, t_dt, t_lam, t_t, t_sign, t_ones, t_zeros,
		t_mask0, t_tmp0, t_alpha0,
		t_mask1, t_tmp1, t_alpha1;
	
	__m256i
		i_mask;

	long long long_sign = 0x8000000000000000;
	v_sign = _mm256_broadcast_sd( (double *) &long_sign );

	int int_sign = 0x80000000;
	t_sign = _mm256_broadcast_ss( (float *) &int_sign );

	t_ones  = _mm256_set_ps( 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 );

	t_zeros = _mm256_setzero_ps( );

	// initialize alpha with 1.0
	t_alpha0 = _mm256_set_ps( 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 );
	t_alpha1 = _mm256_set_ps( 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 );

	for(jj=0; jj<=N; jj++)
		{

		ptr_res_d = res_d[jj];
		ptr_res_m = res_m[jj];
		ptr_dux   = dux[jj];
		ptr_t     = t[jj];
		ptr_t_inv = t_inv[jj];
		ptr_dt    = dt[jj];
		ptr_lam   = lam[jj];
		ptr_dlam  = dlam[jj];
		ptr_idxb  = idxb[jj];

		// box constraints
		nb0 = nb[jj];
		if(nb0>0)
			{

			pnb = (nb0+bs-1)/bs*bs;

			// box constraints
			ll = 0;
#if 1
			for(; ll<nb0-3; ll+=4)
				{

				u_tmp0  = _mm_load_sd( &ptr_dux[ptr_idxb[ll+0]] );
				u_tmp1  = _mm_load_sd( &ptr_dux[ptr_idxb[ll+2]] );
				u_tmp0  = _mm_loadh_pd( u_tmp0, &ptr_dux[ptr_idxb[ll+1]] );
				u_tmp1  = _mm_loadh_pd( u_tmp1, &ptr_dux[ptr_idxb[ll+3]] );
				v_dux   = _mm256_castpd128_pd256( u_tmp0 );
				v_dux   = _mm256_insertf128_pd( v_dux, u_tmp1, 0x1 );
				v_resd0 = _mm256_load_pd( &ptr_res_d[ll+0] );
				v_resd1 = _mm256_load_pd( &ptr_res_d[ll+pnb] );
				v_dt0   = _mm256_sub_pd( v_dux, v_resd0 );
				v_dt1   = _mm256_sub_pd( v_resd1, v_dux );
				_mm256_store_pd( &ptr_dt[ll+0], v_dt0 );
				_mm256_store_pd( &ptr_dt[ll+pnb], v_dt1 );

				v_lam0  = _mm256_load_pd( &ptr_lam[ll+0] );
				v_lam1  = _mm256_load_pd( &ptr_lam[ll+pnb] );
				v_tmp0  = _mm256_mul_pd( v_lam0, v_dt0 );
				v_tmp1  = _mm256_mul_pd( v_lam1, v_dt1 );
				v_resm0 = _mm256_load_pd( &ptr_res_m[ll+0] );
				v_resm1 = _mm256_load_pd( &ptr_res_m[ll+pnb] );
				v_tmp0  = _mm256_add_pd( v_tmp0, v_resm0 );
				v_tmp1  = _mm256_add_pd( v_tmp1, v_resm1 );
				v_tinv0 = _mm256_load_pd( &ptr_t_inv[ll+0] );
				v_tinv1 = _mm256_load_pd( &ptr_t_inv[ll+pnb] );
				v_tinv0 = _mm256_xor_pd( v_tinv0, v_sign );
				v_tinv1 = _mm256_xor_pd( v_tinv1, v_sign );
				v_dlam0  = _mm256_mul_pd( v_tinv0, v_tmp0 );
				v_dlam1  = _mm256_mul_pd( v_tinv1, v_tmp1 );
				_mm256_store_pd( &ptr_dlam[ll+0], v_dlam0 );
				_mm256_store_pd( &ptr_dlam[ll+pnb], v_dlam1 );

				t_dlam   = _mm256_permute2f128_ps( _mm256_castps128_ps256( _mm256_cvtpd_ps( v_dlam0 ) ), _mm256_castps128_ps256( _mm256_cvtpd_ps( v_dlam1 ) ), 0x20 );
				t_dt     = _mm256_permute2f128_ps( _mm256_castps128_ps256( _mm256_cvtpd_ps( v_dt0 ) ), _mm256_castps128_ps256( _mm256_cvtpd_ps( v_dt1 ) ), 0x20 );
				t_mask0  = _mm256_cmp_ps( t_dlam, t_zeros, 0x01 );
				t_mask1  = _mm256_cmp_ps( t_dt, t_zeros, 0x01 );
				v_t0  = _mm256_load_pd( &ptr_t[ll+0] );
				v_t1  = _mm256_load_pd( &ptr_t[ll+pnb] );
				t_lam    = _mm256_permute2f128_ps( _mm256_castps128_ps256( _mm256_cvtpd_ps( v_lam0 ) ), _mm256_castps128_ps256( _mm256_cvtpd_ps( v_lam1 ) ), 0x20 );
				t_t      = _mm256_permute2f128_ps( _mm256_castps128_ps256( _mm256_cvtpd_ps( v_t0 ) ), _mm256_castps128_ps256( _mm256_cvtpd_ps( v_t1 ) ), 0x20 );
				t_lam    = _mm256_xor_ps( t_lam, t_sign );
				t_t      = _mm256_xor_ps( t_t, t_sign );
				t_tmp0   = _mm256_div_ps( t_lam, t_dlam );
				t_tmp1   = _mm256_div_ps( t_t, t_dt );
				t_tmp0   = _mm256_blendv_ps( t_ones, t_tmp0, t_mask0 );
				t_tmp1   = _mm256_blendv_ps( t_ones, t_tmp1, t_mask1 );
				t_alpha0 = _mm256_min_ps( t_alpha0, t_tmp0 );
				t_alpha1 = _mm256_min_ps( t_alpha1, t_tmp1 );

				}
			if(ll<nb0)
				{
				
				if(nb0-ll==1)
					{

					u_dux    = _mm_load_sd( &ptr_dux[ptr_idxb[ll+0]] );
					u_resd0  = _mm_load_sd( &ptr_res_d[ll+0] );
					u_resd1  = _mm_load_sd( &ptr_res_d[ll+pnb] );
					u_dt0    = _mm_sub_pd( u_dux, u_resd0 );
					u_dt1    = _mm_sub_pd( u_resd1, u_dux );
					_mm_store_sd( &ptr_dt[ll+0], u_dt0 );
					_mm_store_sd( &ptr_dt[ll+pnb], u_dt1 );

					u_lam0   = _mm_load_sd( &ptr_lam[ll+0] );
					u_lam1   = _mm_load_sd( &ptr_lam[ll+pnb] );
					u_tmp0   = _mm_mul_pd( u_lam0, u_dt0 );
					u_tmp1   = _mm_mul_pd( u_lam1, u_dt1 );
					u_resm0  = _mm_load_sd( &ptr_res_m[ll+0] );
					u_resm1  = _mm_load_sd( &ptr_res_m[ll+pnb] );
					u_tmp0   = _mm_add_pd( u_tmp0, u_resm0 );
					u_tmp1   = _mm_add_pd( u_tmp1, u_resm1 );
					u_tinv0  = _mm_load_sd( &ptr_t_inv[ll+0] );
					u_tinv1  = _mm_load_sd( &ptr_t_inv[ll+pnb] );
					u_tinv0  = _mm_xor_pd( u_tinv0, _mm256_castpd256_pd128( v_sign ) );
					u_tinv1  = _mm_xor_pd( u_tinv1, _mm256_castpd256_pd128( v_sign ) );
					u_dlam0  = _mm_mul_pd( u_tinv0, u_tmp0 );
					u_dlam1  = _mm_mul_pd( u_tinv1, u_tmp1 );
					_mm_store_sd( &ptr_dlam[ll+0], u_dlam0 );
					_mm_store_sd( &ptr_dlam[ll+pnb], u_dlam1 );

					u_dt1    = _mm_movedup_pd( u_dt1 );
					u_dt0    = _mm_move_sd( u_dt1, u_dt0 );
					u_t1     = _mm_loaddup_pd( &ptr_t[ll+pnb] );
					u_t0     = _mm_load_sd( &ptr_t[ll+0] );
					u_t0     = _mm_move_sd( u_t1, u_t0 );
					u_dlam1  = _mm_movedup_pd( u_dlam1 );
					u_dlam0  = _mm_move_sd( u_dlam1, u_dlam0 );
					u_lam1   = _mm_movedup_pd( u_lam1 );
					u_lam0   = _mm_move_sd( u_lam1, u_lam0 );

					v_dlam0  = _mm256_castpd128_pd256( u_dlam0 );
					v_dlam0  = _mm256_insertf128_pd( v_dlam0, u_dt0, 0x1 );
					v_lam0   = _mm256_castpd128_pd256( u_lam0 );
					v_lam0   = _mm256_insertf128_pd( v_lam0, u_t0, 0x1 );

					s_dlam   = _mm256_cvtpd_ps( v_dlam0 );
					s_lam    = _mm256_cvtpd_ps( v_lam0 );
					s_mask0  = _mm_cmp_ps( s_dlam, _mm256_castps256_ps128( t_zeros ), 0x01 );
					s_lam    = _mm_xor_ps( s_lam, _mm256_castps256_ps128( t_sign ) );
					s_tmp0   = _mm_div_ps( s_lam, s_dlam );
					s_tmp0   = _mm_blendv_ps( _mm256_castps256_ps128( t_ones ), s_tmp0, s_mask0 );
					t_tmp0   = _mm256_blend_ps( t_ones, _mm256_castps128_ps256( s_tmp0 ), 0xf );
					t_alpha0 = _mm256_min_ps( t_alpha0, t_tmp0 );

					}
				else if(nb0-ll==2)
					{

					u_dux    = _mm_load_sd( &ptr_dux[ptr_idxb[ll+0]] );
					u_dux    = _mm_loadh_pd( u_dux, &ptr_dux[ptr_idxb[ll+1]] );
					u_resd0  = _mm_load_pd( &ptr_res_d[ll+0] );
					u_resd1  = _mm_load_pd( &ptr_res_d[ll+pnb] );
					u_dt0    = _mm_sub_pd( u_dux, u_resd0 );
					u_dt1    = _mm_sub_pd( u_resd1, u_dux );
					_mm_store_pd( &ptr_dt[ll+0], u_dt0 );
					_mm_store_pd( &ptr_dt[ll+pnb], u_dt1 );

					u_lam0   = _mm_load_pd( &ptr_lam[ll+0] );
					u_lam1   = _mm_load_pd( &ptr_lam[ll+pnb] );
					u_tmp0   = _mm_mul_pd( u_lam0, u_dt0 );
					u_tmp1   = _mm_mul_pd( u_lam1, u_dt1 );
					u_resm0  = _mm_load_pd( &ptr_res_m[ll+0] );
					u_resm1  = _mm_load_pd( &ptr_res_m[ll+pnb] );
					u_tmp0   = _mm_add_pd( u_tmp0, u_resm0 );
					u_tmp1   = _mm_add_pd( u_tmp1, u_resm1 );
					u_tinv0  = _mm_load_pd( &ptr_t_inv[ll+0] );
					u_tinv1  = _mm_load_pd( &ptr_t_inv[ll+pnb] );
					u_tinv0  = _mm_xor_pd( u_tinv0, _mm256_castpd256_pd128( v_sign ) );
					u_tinv1  = _mm_xor_pd( u_tinv1, _mm256_castpd256_pd128( v_sign ) );
					u_dlam0  = _mm_mul_pd( u_tinv0, u_tmp0 );
					u_dlam1  = _mm_mul_pd( u_tinv1, u_tmp1 );
					_mm_store_pd( &ptr_dlam[ll+0], u_dlam0 );
					_mm_store_pd( &ptr_dlam[ll+pnb], u_dlam1 );

					v_dt0    = _mm256_castpd128_pd256( u_dt0 );
					v_dt0    = _mm256_insertf128_pd( v_dt0, u_dt1, 0x1 );
					v_t0     = _mm256_castpd128_pd256( _mm_load_pd( &ptr_t[ll+0] ) );
					v_t0     = _mm256_insertf128_pd( v_t0, _mm_load_pd( &ptr_t[ll+pnb]), 0x1 );
					v_dlam0  = _mm256_castpd128_pd256( u_dlam0 );
					v_dlam0  = _mm256_insertf128_pd( v_dlam0, u_dlam1, 0x1 );
					v_lam0   = _mm256_castpd128_pd256( u_lam0 );
					v_lam0   = _mm256_insertf128_pd( v_lam0, u_lam1, 0x1 );

					t_dlam   = _mm256_permute2f128_ps( _mm256_castps128_ps256( _mm256_cvtpd_ps( v_dlam0 ) ), _mm256_castps128_ps256( _mm256_cvtpd_ps( v_dt0 ) ), 0x20 );
					t_mask0  = _mm256_cmp_ps( t_dlam, t_zeros, 0x01 );
					t_lam    = _mm256_permute2f128_ps( _mm256_castps128_ps256( _mm256_cvtpd_ps( v_lam0 ) ), _mm256_castps128_ps256( _mm256_cvtpd_ps( v_t0 ) ), 0x20 );
					t_lam    = _mm256_xor_ps( t_lam, t_sign );
					t_tmp0   = _mm256_div_ps( t_lam, t_dlam );
					t_tmp0   = _mm256_blendv_ps( t_ones, t_tmp0, t_mask0 );
					t_alpha0 = _mm256_min_ps( t_alpha0, t_tmp0 );

					}
				else // if(nb-ll==3)
					{

					i_mask = _mm256_castpd_si256( _mm256_set_pd( 1.0, -1.0, -1.0, -1.0 ) );

					u_tmp0  = _mm_load_sd( &ptr_dux[ptr_idxb[ll+0]] );
					u_tmp1  = _mm_load_sd( &ptr_dux[ptr_idxb[ll+2]] );
					u_tmp0  = _mm_loadh_pd( u_tmp0, &ptr_dux[ptr_idxb[ll+1]] );
					v_dux   = _mm256_castpd128_pd256( u_tmp0 );
					v_dux   = _mm256_insertf128_pd( v_dux, u_tmp1, 0x1 );
					v_resd0 = _mm256_load_pd( &ptr_res_d[ll+0] );
					v_resd1 = _mm256_load_pd( &ptr_res_d[ll+pnb] );
					v_dt0   = _mm256_sub_pd( v_dux, v_resd0 );
					v_dt1   = _mm256_sub_pd( v_resd1, v_dux );
					_mm256_maskstore_pd( &ptr_dt[ll+0], i_mask, v_dt0 );
					_mm256_maskstore_pd( &ptr_dt[ll+pnb], i_mask, v_dt1 );

					v_lam0  = _mm256_load_pd( &ptr_lam[ll+0] );
					v_lam1  = _mm256_load_pd( &ptr_lam[ll+pnb] );
					v_tmp0  = _mm256_mul_pd( v_lam0, v_dt0 );
					v_tmp1  = _mm256_mul_pd( v_lam1, v_dt1 );
					v_resm0 = _mm256_load_pd( &ptr_res_m[ll+0] );
					v_resm1 = _mm256_load_pd( &ptr_res_m[ll+pnb] );
					v_tmp0  = _mm256_add_pd( v_tmp0, v_resm0 );
					v_tmp1  = _mm256_add_pd( v_tmp1, v_resm1 );
					v_tinv0 = _mm256_load_pd( &ptr_t_inv[ll+0] );
					v_tinv1 = _mm256_load_pd( &ptr_t_inv[ll+pnb] );
					v_tinv0 = _mm256_xor_pd( v_tinv0, v_sign );
					v_tinv1 = _mm256_xor_pd( v_tinv1, v_sign );
					v_dlam0  = _mm256_mul_pd( v_tinv0, v_tmp0 );
					v_dlam1  = _mm256_mul_pd( v_tinv1, v_tmp1 );
					_mm256_maskstore_pd( &ptr_dlam[ll+0], i_mask, v_dlam0 );
					_mm256_maskstore_pd( &ptr_dlam[ll+pnb], i_mask, v_dlam1 );

					t_dlam   = _mm256_permute2f128_ps( _mm256_castps128_ps256( _mm256_cvtpd_ps( v_dlam0 ) ), _mm256_castps128_ps256( _mm256_cvtpd_ps( v_dlam1 ) ), 0x20 );
					t_dt     = _mm256_permute2f128_ps( _mm256_castps128_ps256( _mm256_cvtpd_ps( v_dt0 ) ), _mm256_castps128_ps256( _mm256_cvtpd_ps( v_dt1 ) ), 0x20 );
					t_mask0  = _mm256_cmp_ps( t_dlam, t_zeros, 0x01 );
					t_mask1  = _mm256_cmp_ps( t_dt, t_zeros, 0x01 );
					v_t0  = _mm256_load_pd( &ptr_t[ll+0] );
					v_t1  = _mm256_load_pd( &ptr_t[ll+pnb] );
					t_lam    = _mm256_permute2f128_ps( _mm256_castps128_ps256( _mm256_cvtpd_ps( v_lam0 ) ), _mm256_castps128_ps256( _mm256_cvtpd_ps( v_lam1 ) ), 0x20 );
					t_t      = _mm256_permute2f128_ps( _mm256_castps128_ps256( _mm256_cvtpd_ps( v_t0 ) ), _mm256_castps128_ps256( _mm256_cvtpd_ps( v_t1 ) ), 0x20 );
					t_lam    = _mm256_xor_ps( t_lam, t_sign );
					t_t      = _mm256_xor_ps( t_t, t_sign );
					t_tmp0   = _mm256_div_ps( t_lam, t_dlam );
					t_tmp1   = _mm256_div_ps( t_t, t_dt );
					t_mask0  = _mm256_blend_ps( t_zeros, t_mask0, 0x77 );
					t_mask1  = _mm256_blend_ps( t_zeros, t_mask1, 0x77 );
					t_tmp0   = _mm256_blendv_ps( t_ones, t_tmp0, t_mask0 );
					t_tmp1   = _mm256_blendv_ps( t_ones, t_tmp1, t_mask1 );
					t_alpha0 = _mm256_min_ps( t_alpha0, t_tmp0 );
					t_alpha1 = _mm256_min_ps( t_alpha1, t_tmp1 );

					}

				}

#else
			for(; ll<nb0; ll++)
				{
				
				ptr_dt[ll+0]   =   ptr_dux[ptr_idxb[ll]] - ptr_res_d[ll+0];
				ptr_dt[ll+pnb] = - ptr_dux[ptr_idxb[ll]] + ptr_res_d[ll+pnb];

				ptr_dlam[ll+0]   = - ptr_t_inv[ll+0]   * ( ptr_lam[ll+0]*ptr_dt[ll+0]     + ptr_res_m[ll+0] );
				ptr_dlam[ll+pnb] = - ptr_t_inv[ll+pnb] * ( ptr_lam[ll+pnb]*ptr_dt[ll+pnb] + ptr_res_m[ll+pnb] );

				if( -alpha*ptr_dlam[ll+0]>ptr_lam[ll+0] )
					{
					alpha = - ptr_lam[ll+0] / ptr_dlam[ll+0];
					}
				if( -alpha*ptr_dlam[ll+pnb]>ptr_lam[ll+pnb] )
					{
					alpha = - ptr_lam[ll+pnb] / ptr_dlam[ll+pnb];
					}
				if( -alpha*ptr_dt[ll+0]>ptr_t[ll+0] )
					{
					alpha = - ptr_t[ll+0] / ptr_dt[ll+0];
					}
				if( -alpha*ptr_dt[ll+pnb]>ptr_t[ll+pnb] )
					{
					alpha = - ptr_t[ll+pnb] / ptr_dt[ll+pnb];
					}

				}
#endif

			ptr_res_d += 2*pnb;
			ptr_res_m += 2*pnb;
			ptr_t     += 2*pnb;
			ptr_t_inv += 2*pnb;
			ptr_dt    += 2*pnb;
			ptr_lam   += 2*pnb;
			ptr_dlam  += 2*pnb;

			}

		// general constraints
		ng0 = ng[jj];
		if(ng0>0)
			{

			nu0 = nu[jj];
			nx0 = nx[jj];
			png = (ng0+bs-1)/bs*bs;
			cng = (ng0+ncl-1)/ncl*ncl;

			dgemv_t_lib(nx0+nu0, ng0, pDCt[jj], cng, ptr_dux, 0, ptr_dt, ptr_dt);

			ll = 0;
#if 1
			for(; ll<ng0-3; ll+=4)
				{

				v_tmp0  = _mm256_load_pd( &ptr_dt[ll+0] );
				v_resd0 = _mm256_load_pd( &ptr_res_d[ll+0] );
				v_resd1 = _mm256_load_pd( &ptr_res_d[ll+png] );
				v_dt0   = _mm256_sub_pd( v_tmp0, v_resd0 );
				v_dt1   = _mm256_sub_pd( v_resd1, v_tmp0 );
				_mm256_store_pd( &ptr_dt[ll+0], v_dt0 );
				_mm256_store_pd( &ptr_dt[ll+png], v_dt1 );

				v_lam0  = _mm256_load_pd( &ptr_lam[ll+0] );
				v_lam1  = _mm256_load_pd( &ptr_lam[ll+png] );
				v_tmp0  = _mm256_mul_pd( v_lam0, v_dt0 );
				v_tmp1  = _mm256_mul_pd( v_lam1, v_dt1 );
				v_resm0 = _mm256_load_pd( &ptr_res_m[ll+0] );
				v_resm1 = _mm256_load_pd( &ptr_res_m[ll+png] );
				v_tmp0  = _mm256_add_pd( v_tmp0, v_resm0 );
				v_tmp1  = _mm256_add_pd( v_tmp1, v_resm1 );
				v_tinv0 = _mm256_load_pd( &ptr_t_inv[ll+0] );
				v_tinv1 = _mm256_load_pd( &ptr_t_inv[ll+png] );
				v_tinv0 = _mm256_xor_pd( v_tinv0, v_sign );
				v_tinv1 = _mm256_xor_pd( v_tinv1, v_sign );
				v_dlam0  = _mm256_mul_pd( v_tinv0, v_tmp0 );
				v_dlam1  = _mm256_mul_pd( v_tinv1, v_tmp1 );
				_mm256_store_pd( &ptr_dlam[ll+0], v_dlam0 );
				_mm256_store_pd( &ptr_dlam[ll+png], v_dlam1 );

				t_dlam   = _mm256_permute2f128_ps( _mm256_castps128_ps256( _mm256_cvtpd_ps( v_dlam0 ) ), _mm256_castps128_ps256( _mm256_cvtpd_ps( v_dlam1 ) ), 0x20 );
				t_dt     = _mm256_permute2f128_ps( _mm256_castps128_ps256( _mm256_cvtpd_ps( v_dt0 ) ), _mm256_castps128_ps256( _mm256_cvtpd_ps( v_dt1 ) ), 0x20 );
				t_mask0  = _mm256_cmp_ps( t_dlam, t_zeros, 0x01 );
				t_mask1  = _mm256_cmp_ps( t_dt, t_zeros, 0x01 );
				v_t0  = _mm256_load_pd( &ptr_t[ll+0] );
				v_t1  = _mm256_load_pd( &ptr_t[ll+png] );
				t_lam    = _mm256_permute2f128_ps( _mm256_castps128_ps256( _mm256_cvtpd_ps( v_lam0 ) ), _mm256_castps128_ps256( _mm256_cvtpd_ps( v_lam1 ) ), 0x20 );
				t_t      = _mm256_permute2f128_ps( _mm256_castps128_ps256( _mm256_cvtpd_ps( v_t0 ) ), _mm256_castps128_ps256( _mm256_cvtpd_ps( v_t1 ) ), 0x20 );
				t_lam    = _mm256_xor_ps( t_lam, t_sign );
				t_t      = _mm256_xor_ps( t_t, t_sign );
				t_tmp0   = _mm256_div_ps( t_lam, t_dlam );
				t_tmp1   = _mm256_div_ps( t_t, t_dt );
				t_tmp0   = _mm256_blendv_ps( t_ones, t_tmp0, t_mask0 );
				t_tmp1   = _mm256_blendv_ps( t_ones, t_tmp1, t_mask1 );
				t_alpha0 = _mm256_min_ps( t_alpha0, t_tmp0 );
				t_alpha1 = _mm256_min_ps( t_alpha1, t_tmp1 );

				}
			if(ll<ng0)
				{
				
				if(ng0-ll==1)
					{

					u_tmp0  = _mm_load_pd( &ptr_dt[ll+0] );
					u_resd0 = _mm_load_pd( &ptr_res_d[ll+0] );
					u_resd1 = _mm_load_pd( &ptr_res_d[ll+png] );
					u_dt0   = _mm_sub_pd( u_tmp0, u_resd0 );
					u_dt1   = _mm_sub_pd( u_resd1, u_tmp0 );
					_mm_store_sd( &ptr_dt[ll+0], u_dt0 );
					_mm_store_sd( &ptr_dt[ll+png], u_dt1 );

					u_lam0   = _mm_load_sd( &ptr_lam[ll+0] );
					u_lam1   = _mm_load_sd( &ptr_lam[ll+png] );
					u_tmp0   = _mm_mul_pd( u_lam0, u_dt0 );
					u_tmp1   = _mm_mul_pd( u_lam1, u_dt1 );
					u_resm0  = _mm_load_sd( &ptr_res_m[ll+0] );
					u_resm1  = _mm_load_sd( &ptr_res_m[ll+png] );
					u_tmp0   = _mm_add_pd( u_tmp0, u_resm0 );
					u_tmp1   = _mm_add_pd( u_tmp1, u_resm1 );
					u_tinv0  = _mm_load_sd( &ptr_t_inv[ll+0] );
					u_tinv1  = _mm_load_sd( &ptr_t_inv[ll+png] );
					u_tinv0  = _mm_xor_pd( u_tinv0, _mm256_castpd256_pd128( v_sign ) );
					u_tinv1  = _mm_xor_pd( u_tinv1, _mm256_castpd256_pd128( v_sign ) );
					u_dlam0  = _mm_mul_pd( u_tinv0, u_tmp0 );
					u_dlam1  = _mm_mul_pd( u_tinv1, u_tmp1 );
					_mm_store_sd( &ptr_dlam[ll+0], u_dlam0 );
					_mm_store_sd( &ptr_dlam[ll+png], u_dlam1 );

					u_dt1    = _mm_movedup_pd( u_dt1 );
					u_dt0    = _mm_move_sd( u_dt1, u_dt0 );
					u_t1     = _mm_loaddup_pd( &ptr_t[ll+png] );
					u_t0     = _mm_load_sd( &ptr_t[ll+0] );
					u_t0     = _mm_move_sd( u_t1, u_t0 );
					u_dlam1  = _mm_movedup_pd( u_dlam1 );
					u_dlam0  = _mm_move_sd( u_dlam1, u_dlam0 );
					u_lam1   = _mm_movedup_pd( u_lam1 );
					u_lam0   = _mm_move_sd( u_lam1, u_lam0 );

					v_dlam0  = _mm256_castpd128_pd256( u_dlam0 );
					v_dlam0  = _mm256_insertf128_pd( v_dlam0, u_dt0, 0x1 );
					v_lam0   = _mm256_castpd128_pd256( u_lam0 );
					v_lam0   = _mm256_insertf128_pd( v_lam0, u_t0, 0x1 );

					s_dlam   = _mm256_cvtpd_ps( v_dlam0 );
					s_lam    = _mm256_cvtpd_ps( v_lam0 );
					s_mask0  = _mm_cmp_ps( s_dlam, _mm256_castps256_ps128( t_zeros ), 0x01 );
					s_lam    = _mm_xor_ps( s_lam, _mm256_castps256_ps128( t_sign ) );
					s_tmp0   = _mm_div_ps( s_lam, s_dlam );
					s_tmp0   = _mm_blendv_ps( _mm256_castps256_ps128( t_ones ), s_tmp0, s_mask0 );
					t_tmp0   = _mm256_blend_ps( t_ones, _mm256_castps128_ps256( s_tmp0 ), 0xf );
					t_alpha0 = _mm256_min_ps( t_alpha0, t_tmp0 );

					}
				else if(ng0-ll==2)
					{

					u_tmp0  = _mm_load_pd( &ptr_dt[ll+0] );
					u_resd0 = _mm_load_pd( &ptr_res_d[ll+0] );
					u_resd1 = _mm_load_pd( &ptr_res_d[ll+png] );
					u_dt0   = _mm_sub_pd( u_tmp0, u_resd0 );
					u_dt1   = _mm_sub_pd( u_resd1, u_tmp0 );
					_mm_store_pd( &ptr_dt[ll+0], u_dt0 );
					_mm_store_pd( &ptr_dt[ll+png], u_dt1 );

					u_lam0   = _mm_load_pd( &ptr_lam[ll+0] );
					u_lam1   = _mm_load_pd( &ptr_lam[ll+png] );
					u_tmp0   = _mm_mul_pd( u_lam0, u_dt0 );
					u_tmp1   = _mm_mul_pd( u_lam1, u_dt1 );
					u_resm0  = _mm_load_pd( &ptr_res_m[ll+0] );
					u_resm1  = _mm_load_pd( &ptr_res_m[ll+png] );
					u_tmp0   = _mm_add_pd( u_tmp0, u_resm0 );
					u_tmp1   = _mm_add_pd( u_tmp1, u_resm1 );
					u_tinv0  = _mm_load_pd( &ptr_t_inv[ll+0] );
					u_tinv1  = _mm_load_pd( &ptr_t_inv[ll+png] );
					u_tinv0  = _mm_xor_pd( u_tinv0, _mm256_castpd256_pd128( v_sign ) );
					u_tinv1  = _mm_xor_pd( u_tinv1, _mm256_castpd256_pd128( v_sign ) );
					u_dlam0  = _mm_mul_pd( u_tinv0, u_tmp0 );
					u_dlam1  = _mm_mul_pd( u_tinv1, u_tmp1 );
					_mm_store_pd( &ptr_dlam[ll+0], u_dlam0 );
					_mm_store_pd( &ptr_dlam[ll+png], u_dlam1 );

					v_dt0    = _mm256_castpd128_pd256( u_dt0 );
					v_dt0    = _mm256_insertf128_pd( v_dt0, u_dt1, 0x1 );
					v_t0     = _mm256_castpd128_pd256( _mm_load_pd( &ptr_t[ll+0] ) );
					v_t0     = _mm256_insertf128_pd( v_t0, _mm_load_pd( &ptr_t[ll+png]), 0x1 );
					v_dlam0  = _mm256_castpd128_pd256( u_dlam0 );
					v_dlam0  = _mm256_insertf128_pd( v_dlam0, u_dlam1, 0x1 );
					v_lam0   = _mm256_castpd128_pd256( u_lam0 );
					v_lam0   = _mm256_insertf128_pd( v_lam0, u_lam1, 0x1 );

					t_dlam   = _mm256_permute2f128_ps( _mm256_castps128_ps256( _mm256_cvtpd_ps( v_dlam0 ) ), _mm256_castps128_ps256( _mm256_cvtpd_ps( v_dt0 ) ), 0x20 );
					t_mask0  = _mm256_cmp_ps( t_dlam, t_zeros, 0x01 );
					t_lam    = _mm256_permute2f128_ps( _mm256_castps128_ps256( _mm256_cvtpd_ps( v_lam0 ) ), _mm256_castps128_ps256( _mm256_cvtpd_ps( v_t0 ) ), 0x20 );
					t_lam    = _mm256_xor_ps( t_lam, t_sign );
					t_tmp0   = _mm256_div_ps( t_lam, t_dlam );
					t_tmp0   = _mm256_blendv_ps( t_ones, t_tmp0, t_mask0 );
					t_alpha0 = _mm256_min_ps( t_alpha0, t_tmp0 );

					}
				else // if(ng-ll==3)
					{

					i_mask = _mm256_castpd_si256( _mm256_set_pd( 1.0, -1.0, -1.0, -1.0 ) );

					v_tmp0  = _mm256_load_pd( &ptr_dt[ll+0] );
					v_resd0 = _mm256_load_pd( &ptr_res_d[ll+0] );
					v_resd1 = _mm256_load_pd( &ptr_res_d[ll+png] );
					v_dt0   = _mm256_sub_pd( v_tmp0, v_resd0 );
					v_dt1   = _mm256_sub_pd( v_resd1, v_tmp0 );
					_mm256_maskstore_pd( &ptr_dt[ll+0], i_mask, v_dt0 );
					_mm256_maskstore_pd( &ptr_dt[ll+png], i_mask, v_dt1 );

					v_lam0  = _mm256_load_pd( &ptr_lam[ll+0] );
					v_lam1  = _mm256_load_pd( &ptr_lam[ll+png] );
					v_tmp0  = _mm256_mul_pd( v_lam0, v_dt0 );
					v_tmp1  = _mm256_mul_pd( v_lam1, v_dt1 );
					v_resm0 = _mm256_load_pd( &ptr_res_m[ll+0] );
					v_resm1 = _mm256_load_pd( &ptr_res_m[ll+png] );
					v_tmp0  = _mm256_add_pd( v_tmp0, v_resm0 );
					v_tmp1  = _mm256_add_pd( v_tmp1, v_resm1 );
					v_tinv0 = _mm256_load_pd( &ptr_t_inv[ll+0] );
					v_tinv1 = _mm256_load_pd( &ptr_t_inv[ll+png] );
					v_tinv0 = _mm256_xor_pd( v_tinv0, v_sign );
					v_tinv1 = _mm256_xor_pd( v_tinv1, v_sign );
					v_dlam0  = _mm256_mul_pd( v_tinv0, v_tmp0 );
					v_dlam1  = _mm256_mul_pd( v_tinv1, v_tmp1 );
					_mm256_maskstore_pd( &ptr_dlam[ll+0], i_mask, v_dlam0 );
					_mm256_maskstore_pd( &ptr_dlam[ll+png], i_mask, v_dlam1 );

					t_dlam   = _mm256_permute2f128_ps( _mm256_castps128_ps256( _mm256_cvtpd_ps( v_dlam0 ) ), _mm256_castps128_ps256( _mm256_cvtpd_ps( v_dlam1 ) ), 0x20 );
					t_dt     = _mm256_permute2f128_ps( _mm256_castps128_ps256( _mm256_cvtpd_ps( v_dt0 ) ), _mm256_castps128_ps256( _mm256_cvtpd_ps( v_dt1 ) ), 0x20 );
					t_mask0  = _mm256_cmp_ps( t_dlam, t_zeros, 0x01 );
					t_mask1  = _mm256_cmp_ps( t_dt, t_zeros, 0x01 );
					v_t0  = _mm256_load_pd( &ptr_t[ll+0] );
					v_t1  = _mm256_load_pd( &ptr_t[ll+png] );
					t_lam    = _mm256_permute2f128_ps( _mm256_castps128_ps256( _mm256_cvtpd_ps( v_lam0 ) ), _mm256_castps128_ps256( _mm256_cvtpd_ps( v_lam1 ) ), 0x20 );
					t_t      = _mm256_permute2f128_ps( _mm256_castps128_ps256( _mm256_cvtpd_ps( v_t0 ) ), _mm256_castps128_ps256( _mm256_cvtpd_ps( v_t1 ) ), 0x20 );
					t_lam    = _mm256_xor_ps( t_lam, t_sign );
					t_t      = _mm256_xor_ps( t_t, t_sign );
					t_tmp0   = _mm256_div_ps( t_lam, t_dlam );
					t_tmp1   = _mm256_div_ps( t_t, t_dt );
					t_mask0  = _mm256_blend_ps( t_zeros, t_mask0, 0x77 );
					t_mask1  = _mm256_blend_ps( t_zeros, t_mask1, 0x77 );
					t_tmp0   = _mm256_blendv_ps( t_ones, t_tmp0, t_mask0 );
					t_tmp1   = _mm256_blendv_ps( t_ones, t_tmp1, t_mask1 );
					t_alpha0 = _mm256_min_ps( t_alpha0, t_tmp0 );
					t_alpha1 = _mm256_min_ps( t_alpha1, t_tmp1 );

					}

				}

#else
			for(; ll<ng0; ll++)
				{

				ptr_dt[ll+png] = - ptr_dt[ll];

				ptr_dt[ll+0]   -= ptr_res_d[ll+0];
				ptr_dt[ll+png] += ptr_res_d[ll+png];

				ptr_dlam[ll+0]   = - ptr_t_inv[ll+0]   * ( ptr_lam[ll+0]*ptr_dt[ll+0]     + ptr_res_m[ll+0] );
				ptr_dlam[ll+png] = - ptr_t_inv[ll+png] * ( ptr_lam[ll+png]*ptr_dt[ll+png] + ptr_res_m[ll+png] );

				if( -alpha*ptr_dlam[ll+0]>ptr_lam[ll+0] )
					{
					alpha = - ptr_lam[ll+0] / ptr_dlam[ll+0];
					}
				if( -alpha*ptr_dlam[ll+png]>ptr_lam[ll+png] )
					{
					alpha = - ptr_lam[ll+png] / ptr_dlam[ll+png];
					}
				if( -alpha*ptr_dt[ll+0]>ptr_t[ll+0] )
					{
					alpha = - ptr_t[ll+0] / ptr_dt[ll+0];
					}
				if( -alpha*ptr_dt[ll+png]>ptr_t[ll+png] )
					{
					alpha = - ptr_t[ll+png] / ptr_dt[ll+png];
					}

				}
#endif

			}

		}		

	// reduce alpha
	t_alpha0 = _mm256_min_ps( t_alpha0, t_alpha1 );
	s_alpha0 = _mm256_extractf128_ps( t_alpha0, 0x1 );
	s_alpha1 = _mm256_castps256_ps128( t_alpha0 );
	s_alpha0 = _mm_min_ps( s_alpha0, s_alpha1 );
	
	v_alpha = _mm256_cvtps_pd( s_alpha0 );
	u_alpha = _mm256_extractf128_pd( v_alpha, 0x1 );
	u_alpha = _mm_min_pd( u_alpha, _mm256_castpd256_pd128( v_alpha ) );
	u_alpha = _mm_min_sd( u_alpha, _mm_permute_pd( u_alpha, 0x1 ) );
	u_alpha = _mm_min_sd( u_alpha, _mm_load_sd( &alpha ) );
//	u_alpha = _mm_min_sd( u_alpha, _mm_set_sd( 1.0 ) );
	_mm_store_sd( ptr_alpha, u_alpha );

	// store alpha
//	ptr_alpha[0] = alpha;

	return;
	
	}



void d_compute_dt_dlam_res_mpc_hard_tv(int N, int *nx, int *nu, int *nb, int **idxb, int *ng, double **dux, double **t, double **t_inv, double **lam, double **pDCt, double **res_d, double **res_m, double **dt, double **dlam)
	{
	
	// constants
	const int bs = D_MR;
	const int ncl = D_NCL;

	int nu0, nx0, nb0, pnb, ng0, png, cng;

	double
		*ptr_res_d, *ptr_res_m, *ptr_dux, *ptr_t, *ptr_t_inv, *ptr_dt, *ptr_lam, *ptr_dlam;
	
	int
		*ptr_idxb;
	
	__m128d
		u_tmp0,
		u_tmp1;
	
	__m256d
		v_dux, v_sign,
		v_resm0, v_resd0, v_dt0, v_dlam0, v_tmp0, v_tinv0, v_lam0, v_t0,
		v_resm1, v_resd1, v_dt1, v_dlam1, v_tmp1, v_tinv1, v_lam1, v_t1;

	long long long_sign = 0x8000000000000000;
	v_sign = _mm256_broadcast_sd( (double *) &long_sign );

	int jj, ll;

	for(jj=0; jj<=N; jj++)
		{

		ptr_res_d = res_d[jj];
		ptr_res_m = res_m[jj];
		ptr_dux   = dux[jj];
		ptr_t     = t[jj];
		ptr_t_inv = t_inv[jj];
		ptr_dt    = dt[jj];
		ptr_lam   = lam[jj];
		ptr_dlam  = dlam[jj];
		ptr_idxb  = idxb[jj];

		// box constraints
		nb0 = nb[jj];
		if(nb0>0)
			{

			pnb = (nb0+bs-1)/bs*bs;

			// box constraints
			ll = 0;
			for(; ll<nb0-3; ll+=4)
				{

				u_tmp0  = _mm_load_sd( &ptr_dux[ptr_idxb[ll+0]] );
				u_tmp1  = _mm_load_sd( &ptr_dux[ptr_idxb[ll+2]] );
				u_tmp0  = _mm_loadh_pd( u_tmp0, &ptr_dux[ptr_idxb[ll+1]] );
				u_tmp1  = _mm_loadh_pd( u_tmp1, &ptr_dux[ptr_idxb[ll+3]] );
				v_dux   = _mm256_castpd128_pd256( u_tmp0 );
				v_dux   = _mm256_insertf128_pd( v_dux, u_tmp1, 0x1 );
				v_resd0 = _mm256_load_pd( &ptr_res_d[ll+0] );
				v_resd1 = _mm256_load_pd( &ptr_res_d[ll+pnb] );
				v_dt0   = _mm256_sub_pd( v_dux, v_resd0 );
				v_dt1   = _mm256_sub_pd( v_resd1, v_dux );
				_mm256_store_pd( &ptr_dt[ll+0], v_dt0 );
				_mm256_store_pd( &ptr_dt[ll+pnb], v_dt1 );

				v_lam0  = _mm256_load_pd( &ptr_lam[ll+0] );
				v_lam1  = _mm256_load_pd( &ptr_lam[ll+pnb] );
				v_tmp0  = _mm256_mul_pd( v_lam0, v_dt0 );
				v_tmp1  = _mm256_mul_pd( v_lam1, v_dt1 );
				v_resm0 = _mm256_load_pd( &ptr_res_m[ll+0] );
				v_resm1 = _mm256_load_pd( &ptr_res_m[ll+pnb] );
				v_tmp0  = _mm256_add_pd( v_tmp0, v_resm0 );
				v_tmp1  = _mm256_add_pd( v_tmp1, v_resm1 );
				v_tinv0 = _mm256_load_pd( &ptr_t_inv[ll+0] );
				v_tinv1 = _mm256_load_pd( &ptr_t_inv[ll+pnb] );
				v_tinv0 = _mm256_xor_pd( v_tinv0, v_sign );
				v_tinv1 = _mm256_xor_pd( v_tinv1, v_sign );
				v_dlam0  = _mm256_mul_pd( v_tinv0, v_tmp0 );
				v_dlam1  = _mm256_mul_pd( v_tinv1, v_tmp1 );
				_mm256_store_pd( &ptr_dlam[ll+0], v_dlam0 );
				_mm256_store_pd( &ptr_dlam[ll+pnb], v_dlam1 );

				}
			for(; ll<nb0; ll++)
				{
				
				ptr_dt[ll+0]   =   ptr_dux[ptr_idxb[ll]] - ptr_res_d[ll+0];
				ptr_dt[ll+pnb] = - ptr_dux[ptr_idxb[ll]] + ptr_res_d[ll+pnb];

				ptr_dlam[ll+0]   = - ptr_t_inv[ll+0]   * ( ptr_lam[ll+0]*ptr_dt[ll+0]     + ptr_res_m[ll+0] );
				ptr_dlam[ll+pnb] = - ptr_t_inv[ll+pnb] * ( ptr_lam[ll+pnb]*ptr_dt[ll+pnb] + ptr_res_m[ll+pnb] );

				}

			ptr_res_d += 2*pnb;
			ptr_res_m += 2*pnb;
			ptr_t     += 2*pnb;
			ptr_t_inv += 2*pnb;
			ptr_dt    += 2*pnb;
			ptr_lam   += 2*pnb;
			ptr_dlam  += 2*pnb;

			}

		// general constraints
		ng0 = ng[jj];
		if(ng0>0)
			{

			nu0 = nu[jj];
			nx0 = nx[jj];
			png = (ng0+bs-1)/bs*bs;
			cng = (ng0+ncl-1)/ncl*ncl;

			dgemv_t_lib(nx0+nu0, ng0, pDCt[jj], cng, ptr_dux, 0, ptr_dt, ptr_dt);

			ll = 0;
			for(; ll<ng0-3; ll+=4)
				{

				v_tmp0  = _mm256_load_pd( &ptr_dt[ll+0] );
				v_resd0 = _mm256_load_pd( &ptr_res_d[ll+0] );
				v_resd1 = _mm256_load_pd( &ptr_res_d[ll+png] );
				v_dt0   = _mm256_sub_pd( v_tmp0, v_resd0 );
				v_dt1   = _mm256_sub_pd( v_resd1, v_tmp0 );
				_mm256_store_pd( &ptr_dt[ll+0], v_dt0 );
				_mm256_store_pd( &ptr_dt[ll+png], v_dt1 );

				v_lam0  = _mm256_load_pd( &ptr_lam[ll+0] );
				v_lam1  = _mm256_load_pd( &ptr_lam[ll+png] );
				v_tmp0  = _mm256_mul_pd( v_lam0, v_dt0 );
				v_tmp1  = _mm256_mul_pd( v_lam1, v_dt1 );
				v_resm0 = _mm256_load_pd( &ptr_res_m[ll+0] );
				v_resm1 = _mm256_load_pd( &ptr_res_m[ll+png] );
				v_tmp0  = _mm256_add_pd( v_tmp0, v_resm0 );
				v_tmp1  = _mm256_add_pd( v_tmp1, v_resm1 );
				v_tinv0 = _mm256_load_pd( &ptr_t_inv[ll+0] );
				v_tinv1 = _mm256_load_pd( &ptr_t_inv[ll+png] );
				v_tinv0 = _mm256_xor_pd( v_tinv0, v_sign );
				v_tinv1 = _mm256_xor_pd( v_tinv1, v_sign );
				v_dlam0  = _mm256_mul_pd( v_tinv0, v_tmp0 );
				v_dlam1  = _mm256_mul_pd( v_tinv1, v_tmp1 );
				_mm256_store_pd( &ptr_dlam[ll+0], v_dlam0 );
				_mm256_store_pd( &ptr_dlam[ll+png], v_dlam1 );

				}
			for(; ll<ng0; ll++)
				{

				ptr_dt[ll+png] = - ptr_dt[ll];

				ptr_dt[ll+0]   -= ptr_res_d[ll+0];
				ptr_dt[ll+png] += ptr_res_d[ll+png];

				ptr_dlam[ll+0]   = - ptr_t_inv[ll+0]   * ( ptr_lam[ll+0]*ptr_dt[ll+0]     + ptr_res_m[ll+0] );
				ptr_dlam[ll+png] = - ptr_t_inv[ll+png] * ( ptr_lam[ll+png]*ptr_dt[ll+png] + ptr_res_m[ll+png] );

				}

			}

		}		

	return;
	
	}



void d_update_var_res_mpc_hard_tv(int N, int *nx, int *nu, int *nb, int *ng, double alpha, double **ux, double **dux, double **pi, double **dpi, double **t, double **dt, double **lam, double **dlam)
	{

	// constants
	const int bs = D_MR;
	const int ncl = D_NCL;

	int nu0, nx0, nx1, nb0, pnb, ng0, png;

	int jj, ll;
	
	double
		*ptr_ux, *ptr_dux, *ptr_pi, *ptr_dpi, *ptr_t, *ptr_dt, *ptr_lam, *ptr_dlam;

	for(jj=0; jj<=N; jj++)
		{

		nx0 = nx[jj];
		nu0 = nu[jj];
		nb0 = nb[jj];
		pnb = bs*((nb0+bs-1)/bs); // cache aligned number of box constraints
		ng0 = ng[jj];
		png = bs*((ng0+bs-1)/bs); // cache aligned number of box constraints
		if(jj<N)
			nx1 = nx[jj+1];
		else
			nx1 = 0;
		
		// update inputs and states
		ptr_ux     = ux[jj];
		ptr_dux    = dux[jj];
		daxpy_lib(nu0+nx0, alpha, ptr_dux, ptr_ux);

		// update equality constrained multipliers
		ptr_pi     = pi[jj];
		ptr_dpi    = dpi[jj];
		daxpy_lib(nx1, alpha, ptr_dpi, ptr_pi);

		// box constraints
		ptr_t       = t[jj];
		ptr_dt      = dt[jj];
		ptr_lam     = lam[jj];
		ptr_dlam    = dlam[jj];
		daxpy_lib(nb0, alpha, &ptr_dlam[0], &ptr_lam[0]);
		daxpy_lib(nb0, alpha, &ptr_dlam[pnb], &ptr_lam[pnb]);
		daxpy_lib(nb0, alpha, &ptr_dt[0], &ptr_t[0]);
		daxpy_lib(nb0, alpha, &ptr_dt[pnb], &ptr_t[pnb]);

		// general constraints
		ptr_t       += 2*pnb;
		ptr_dt      += 2*pnb;
		ptr_lam     += 2*pnb;
		ptr_dlam    += 2*pnb;
		daxpy_lib(ng0, alpha, &ptr_dlam[0], &ptr_lam[0]);
		daxpy_lib(ng0, alpha, &ptr_dlam[png], &ptr_lam[png]);
		daxpy_lib(ng0, alpha, &ptr_dt[0], &ptr_t[0]);
		daxpy_lib(ng0, alpha, &ptr_dt[png], &ptr_t[png]);

		}

	return;
	
	}



void d_backup_update_var_res_mpc_hard_tv(int N, int *nx, int *nu, int *nb, int *ng, double alpha, double **ux_bkp, double **ux, double **dux, double **pi_bkp, double **pi, double **dpi, double **t_bkp, double **t, double **dt, double **lam_bkp, double **lam, double **dlam)
	{

	// constants
	const int bs = D_MR;
	const int ncl = D_NCL;

	int nu0, nx0, nx1, nb0, pnb, ng0, png;

	int jj, ll;
	
	double
		*ptr_ux_bkp, *ptr_ux, *ptr_dux, *ptr_pi_bkp, *ptr_pi, *ptr_dpi, *ptr_t_bkp, *ptr_t, *ptr_dt, *ptr_lam_bkp, *ptr_lam, *ptr_dlam;

	for(jj=0; jj<=N; jj++)
		{

		nx0 = nx[jj];
		nu0 = nu[jj];
		nb0 = nb[jj];
		pnb = bs*((nb0+bs-1)/bs); // cache aligned number of box constraints
		ng0 = ng[jj];
		png = bs*((ng0+bs-1)/bs); // cache aligned number of box constraints
		if(jj<N)
			nx1 = nx[jj+1];
		else
			nx1 = 0;
		
		// update inputs and states
		ptr_ux_bkp = ux_bkp[jj];
		ptr_ux     = ux[jj];
		ptr_dux    = dux[jj];
		daxpy_bkp_lib(nu0+nx0, alpha, ptr_dux, ptr_ux, ptr_ux_bkp);

		// update equality constrained multipliers
		ptr_pi_bkp = pi_bkp[jj];
		ptr_pi     = pi[jj];
		ptr_dpi    = dpi[jj];
		daxpy_bkp_lib(nx1, alpha, ptr_dpi, ptr_pi, ptr_pi_bkp);

		// box constraints
		ptr_t_bkp   = t_bkp[jj];
		ptr_t       = t[jj];
		ptr_dt      = dt[jj];
		ptr_lam_bkp = lam_bkp[jj];
		ptr_lam     = lam[jj];
		ptr_dlam    = dlam[jj];
		daxpy_bkp_lib(nb0, alpha, &ptr_dlam[0], &ptr_lam[0], &ptr_lam_bkp[0]);
		daxpy_bkp_lib(nb0, alpha, &ptr_dlam[pnb], &ptr_lam[pnb], &ptr_lam_bkp[pnb]);
		daxpy_bkp_lib(nb0, alpha, &ptr_dt[0], &ptr_t[0], &ptr_t_bkp[0]);
		daxpy_bkp_lib(nb0, alpha, &ptr_dt[pnb], &ptr_t[pnb], &ptr_t_bkp[pnb]);

		// general constraints
		ptr_t_bkp   += 2*pnb;
		ptr_t       += 2*pnb;
		ptr_dt      += 2*pnb;
		ptr_lam_bkp += 2*pnb;
		ptr_lam     += 2*pnb;
		ptr_dlam    += 2*pnb;
		daxpy_bkp_lib(ng0, alpha, &ptr_dlam[0], &ptr_lam[0], &ptr_lam_bkp[0]);
		daxpy_bkp_lib(ng0, alpha, &ptr_dlam[png], &ptr_lam[png], &ptr_lam_bkp[png]);
		daxpy_bkp_lib(ng0, alpha, &ptr_dt[0], &ptr_t[0], &ptr_t_bkp[0]);
		daxpy_bkp_lib(ng0, alpha, &ptr_dt[png], &ptr_t[png], &ptr_t_bkp[png]);

		}

	return;
	
	}



void d_compute_mu_res_mpc_hard_tv(int N, int *nx, int *nu, int *nb, int *ng, double alpha, double **lam, double **dlam, double **t, double **dt, double *ptr_mu, double mu_scal)
	{
	
	// constants
	const int bs = D_MR;

	int jj, ll, ll_bkp, ll_end;
	double ll_left;
	
	double d_mask[4] = {0.5, 1.5, 2.5, 3.5};
	
	__m128d
		u_mu0, u_tmp;

	__m256d
		v_alpha, v_mask, v_left, v_zeros,
		v_t0, v_dt0, v_lam0, v_dlam0, v_mu0, 
		v_t1, v_dt1, v_lam1, v_dlam1, v_mu1;
		
	double
		*ptr_t, *ptr_lam, *ptr_dt, *ptr_dlam;

	int nb0, pnb, ng0, png;
		
	v_alpha = _mm256_set_pd( alpha, alpha, alpha, alpha );
	
	v_zeros = _mm256_setzero_pd();
	v_mu0 = _mm256_setzero_pd();
	v_mu1 = _mm256_setzero_pd();

	for(jj=0; jj<=N; jj++)
		{
		
		ptr_t    = t[jj];
		ptr_lam  = lam[jj];
		ptr_dt   = dt[jj];
		ptr_dlam = dlam[jj];

		// box constraints
		nb0 = nb[jj];
		pnb = (nb0+bs-1)/bs*bs;
		for(ll=0; ll<nb0-3; ll+=4)
			{
			v_t0    = _mm256_load_pd( &ptr_t[0*pnb+ll] );
			v_t1    = _mm256_load_pd( &ptr_t[1*pnb+ll] );
			v_lam0  = _mm256_load_pd( &ptr_lam[0*pnb+ll] );
			v_lam1  = _mm256_load_pd( &ptr_lam[1*pnb+ll] );
			v_dt0   = _mm256_load_pd( &ptr_dt[0*pnb+ll] );
			v_dt1   = _mm256_load_pd( &ptr_dt[1*pnb+ll] );
			v_dlam0 = _mm256_load_pd( &ptr_dlam[0*pnb+ll] );
			v_dlam1 = _mm256_load_pd( &ptr_dlam[1*pnb+ll] );
#if defined(TARGET_X64_AVX2)
			v_t0    = _mm256_fmadd_pd( v_alpha, v_dt0, v_t0 );
			v_t1    = _mm256_fmadd_pd( v_alpha, v_dt1, v_t1 );
			v_lam0  = _mm256_fmadd_pd( v_alpha, v_dlam0, v_lam0 );
			v_lam1  = _mm256_fmadd_pd( v_alpha, v_dlam1, v_lam1 );
			v_mu0   = _mm256_fmadd_pd( v_lam0, v_t0, v_mu0 );
			v_mu1   = _mm256_fmadd_pd( v_lam1, v_t1, v_mu1 );
#endif
#if defined(TARGET_X64_AVX)
			v_dt0   = _mm256_mul_pd( v_alpha, v_dt0 );
			v_dt1   = _mm256_mul_pd( v_alpha, v_dt1 );
			v_dlam0 = _mm256_mul_pd( v_alpha, v_dlam0 );
			v_dlam1 = _mm256_mul_pd( v_alpha, v_dlam1 );
			v_t0    = _mm256_add_pd( v_t0, v_dt0 );
			v_t1    = _mm256_add_pd( v_t1, v_dt1 );
			v_lam0  = _mm256_add_pd( v_lam0, v_dlam0 );
			v_lam1  = _mm256_add_pd( v_lam1, v_dlam1 );
			v_lam0  = _mm256_mul_pd( v_lam0, v_t0 );
			v_lam1  = _mm256_mul_pd( v_lam1, v_t1 );
			v_mu0   = _mm256_add_pd( v_mu0, v_lam0 );
			v_mu1   = _mm256_add_pd( v_mu1, v_lam1 );
#endif
			}
		if(ll<nb0)
			{
			ll_left = nb0-ll;
			v_left  = _mm256_broadcast_sd( &ll_left );
			v_mask  = _mm256_loadu_pd( d_mask );
			v_mask  = _mm256_sub_pd( v_mask, v_left );

			v_t0    = _mm256_load_pd( &ptr_t[0*pnb+ll] );
			v_t1    = _mm256_load_pd( &ptr_t[1*pnb+ll] );
			v_lam0  = _mm256_load_pd( &ptr_lam[0*pnb+ll] );
			v_lam1  = _mm256_load_pd( &ptr_lam[1*pnb+ll] );
			v_dt0   = _mm256_load_pd( &ptr_dt[0*pnb+ll] );
			v_dt1   = _mm256_load_pd( &ptr_dt[1*pnb+ll] );
			v_dlam0 = _mm256_load_pd( &ptr_dlam[0*pnb+ll] );
			v_dlam1 = _mm256_load_pd( &ptr_dlam[1*pnb+ll] );
#if defined(TARGET_X64_AVX2)
			v_t0    = _mm256_fmadd_pd( v_alpha, v_dt0, v_t0 );
			v_t1    = _mm256_fmadd_pd( v_alpha, v_dt1, v_t1 );
			v_lam0  = _mm256_fmadd_pd( v_alpha, v_dlam0, v_lam0 );
			v_lam1  = _mm256_fmadd_pd( v_alpha, v_dlam1, v_lam1 );
			v_lam0  = _mm256_blendv_pd( v_zeros, v_lam0, v_mask );
			v_lam1  = _mm256_blendv_pd( v_zeros, v_lam1, v_mask );
			v_mu0   = _mm256_fmadd_pd( v_lam0, v_t0, v_mu0 );
			v_mu1   = _mm256_fmadd_pd( v_lam1, v_t1, v_mu1 );
#endif
#if defined(TARGET_X64_AVX)
			v_dt0   = _mm256_mul_pd( v_alpha, v_dt0 );
			v_dt1   = _mm256_mul_pd( v_alpha, v_dt1 );
			v_dlam0 = _mm256_mul_pd( v_alpha, v_dlam0 );
			v_dlam1 = _mm256_mul_pd( v_alpha, v_dlam1 );
			v_t0    = _mm256_add_pd( v_t0, v_dt0 );
			v_t1    = _mm256_add_pd( v_t1, v_dt1 );
			v_lam0  = _mm256_add_pd( v_lam0, v_dlam0 );
			v_lam1  = _mm256_add_pd( v_lam1, v_dlam1 );
			v_lam0  = _mm256_mul_pd( v_lam0, v_t0 );
			v_lam1  = _mm256_mul_pd( v_lam1, v_t1 );
			v_lam0  = _mm256_blendv_pd( v_zeros, v_lam0, v_mask );
			v_lam1  = _mm256_blendv_pd( v_zeros, v_lam1, v_mask );
			v_mu0   = _mm256_add_pd( v_mu0, v_lam0 );
			v_mu1   = _mm256_add_pd( v_mu1, v_lam1 );
#endif
			}

		ptr_t    += 2*pnb;
		ptr_lam  += 2*pnb;
		ptr_dt   += 2*pnb;
		ptr_dlam += 2*pnb;

		// general constraints
		ng0 = ng[jj];
		png = (ng0+bs-1)/bs*bs;
		for(ll=0; ll<ng0-3; ll+=4)
			{
			v_t0    = _mm256_load_pd( &ptr_t[ll] );
			v_t1    = _mm256_load_pd( &ptr_t[png+ll] );
			v_lam0  = _mm256_load_pd( &ptr_lam[ll] );
			v_lam1  = _mm256_load_pd( &ptr_lam[png+ll] );
			v_dt0   = _mm256_load_pd( &ptr_dt[ll] );
			v_dt1   = _mm256_load_pd( &ptr_dt[png+ll] );
			v_dlam0 = _mm256_load_pd( &ptr_dlam[ll] );
			v_dlam1 = _mm256_load_pd( &ptr_dlam[png+ll] );
#if defined(TARGET_X64_AVX2)
			v_t0    = _mm256_fmadd_pd( v_alpha, v_dt0, v_t0 );
			v_t1    = _mm256_fmadd_pd( v_alpha, v_dt1, v_t1 );
			v_lam0  = _mm256_fmadd_pd( v_alpha, v_dlam0, v_lam0 );
			v_lam1  = _mm256_fmadd_pd( v_alpha, v_dlam1, v_lam1 );
			v_mu0   = _mm256_fmadd_pd( v_lam0, v_t0, v_mu0 );
			v_mu1   = _mm256_fmadd_pd( v_lam1, v_t1, v_mu1 );
#endif
#if defined(TARGET_X64_AVX)
			v_dt0   = _mm256_mul_pd( v_alpha, v_dt0 );
			v_dt1   = _mm256_mul_pd( v_alpha, v_dt1 );
			v_dlam0 = _mm256_mul_pd( v_alpha, v_dlam0 );
			v_dlam1 = _mm256_mul_pd( v_alpha, v_dlam1 );
			v_t0    = _mm256_add_pd( v_t0, v_dt0 );
			v_t1    = _mm256_add_pd( v_t1, v_dt1 );
			v_lam0  = _mm256_add_pd( v_lam0, v_dlam0 );
			v_lam1  = _mm256_add_pd( v_lam1, v_dlam1 );
			v_lam0  = _mm256_mul_pd( v_lam0, v_t0 );
			v_lam1  = _mm256_mul_pd( v_lam1, v_t1 );
			v_mu0   = _mm256_add_pd( v_mu0, v_lam0 );
			v_mu1   = _mm256_add_pd( v_mu1, v_lam1 );
#endif
			}
		if(ll<ng0)
			{
			ll_left = ng0-ll;
			v_left  = _mm256_broadcast_sd( &ll_left );
			v_mask  = _mm256_loadu_pd( d_mask );
			v_mask  = _mm256_sub_pd( v_mask, v_left );

			v_t0    = _mm256_load_pd( &ptr_t[ll] );
			v_t1    = _mm256_load_pd( &ptr_t[png+ll] );
			v_lam0  = _mm256_load_pd( &ptr_lam[ll] );
			v_lam1  = _mm256_load_pd( &ptr_lam[png+ll] );
			v_dt0   = _mm256_load_pd( &ptr_dt[ll] );
			v_dt1   = _mm256_load_pd( &ptr_dt[png+ll] );
			v_dlam0 = _mm256_load_pd( &ptr_dlam[ll] );
			v_dlam1 = _mm256_load_pd( &ptr_dlam[png+ll] );
#if defined(TARGET_X64_AVX2)
			v_t0    = _mm256_fmadd_pd( v_alpha, v_dt0, v_t0 );
			v_t1    = _mm256_fmadd_pd( v_alpha, v_dt1, v_t1 );
			v_lam0  = _mm256_fmadd_pd( v_alpha, v_dlam0, v_lam0 );
			v_lam1  = _mm256_fmadd_pd( v_alpha, v_dlam1, v_lam1 );
			v_lam0  = _mm256_blendv_pd( v_zeros, v_lam0, v_mask );
			v_lam1  = _mm256_blendv_pd( v_zeros, v_lam1, v_mask );
			v_mu0   = _mm256_fmadd_pd( v_lam0, v_t0, v_mu0 );
			v_mu1   = _mm256_fmadd_pd( v_lam1, v_t1, v_mu1 );
#endif
#if defined(TARGET_X64_AVX)
			v_dt0   = _mm256_mul_pd( v_alpha, v_dt0 );
			v_dt1   = _mm256_mul_pd( v_alpha, v_dt1 );
			v_dlam0 = _mm256_mul_pd( v_alpha, v_dlam0 );
			v_dlam1 = _mm256_mul_pd( v_alpha, v_dlam1 );
			v_t0    = _mm256_add_pd( v_t0, v_dt0 );
			v_t1    = _mm256_add_pd( v_t1, v_dt1 );
			v_lam0  = _mm256_add_pd( v_lam0, v_dlam0 );
			v_lam1  = _mm256_add_pd( v_lam1, v_dlam1 );
			v_lam0  = _mm256_mul_pd( v_lam0, v_t0 );
			v_lam1  = _mm256_mul_pd( v_lam1, v_t1 );
			v_lam0  = _mm256_blendv_pd( v_zeros, v_lam0, v_mask );
			v_lam1  = _mm256_blendv_pd( v_zeros, v_lam1, v_mask );
			v_mu0   = _mm256_add_pd( v_mu0, v_lam0 );
			v_mu1   = _mm256_add_pd( v_mu1, v_lam1 );
#endif
			}

		}

	v_mu0 = _mm256_add_pd( v_mu0, v_mu1 );
	u_mu0 = _mm_add_pd( _mm256_castpd256_pd128( v_mu0 ), _mm256_extractf128_pd( v_mu0, 0x1 ) );
	u_mu0 = _mm_hadd_pd( u_mu0, u_mu0 );
	u_tmp = _mm_load_sd( &mu_scal );
	u_mu0 = _mm_mul_sd( u_mu0, u_tmp );
	_mm_store_sd( ptr_mu, u_mu0 );

	return;

	}



void d_compute_centering_correction_res_mpc_hard_tv(int N, int *nb, int *ng, double sigma_mu, double **dt, double **dlam, double **res_m)
	{

	const int bs = D_MR;

	int pnb, png;

	int ii, jj;

	double
		*ptr_res_m, *ptr_dt, *ptr_dlam;
	
	__m256d
		v_sigma_mu,
		v_dt0, v_dlam0, v_tmp0, v_resm0,
		v_dt1, v_dlam1, v_tmp1, v_resm1;
	
	__m256i
		i_mask;
	
	double ii_left;

	static double d_mask[4] = {0.5, 1.5, 2.5, 3.5};

	v_sigma_mu = _mm256_broadcast_sd( &sigma_mu );

	for(ii=0; ii<=N; ii++)
		{

		pnb = (nb[ii]+bs-1)/bs*bs;
		png = (ng[ii]+bs-1)/bs*bs;

		ptr_res_m = res_m[ii];
		ptr_dt    = dt[ii];
		ptr_dlam  = dlam[ii];

		for(jj=0; jj<nb[ii]-3; jj+=4)
			{
			v_dt0   = _mm256_load_pd( &ptr_dt[jj+0] );
			v_dt1   = _mm256_load_pd( &ptr_dt[jj+pnb] );
			v_dlam0 = _mm256_load_pd( &ptr_dlam[jj+0] );
			v_dlam1 = _mm256_load_pd( &ptr_dlam[jj+pnb] );
			v_tmp0  = _mm256_mul_pd( v_dt0, v_dlam0 );
			v_tmp1  = _mm256_mul_pd( v_dt1, v_dlam1 );
			v_tmp0  = _mm256_sub_pd( v_tmp0, v_sigma_mu );
			v_tmp1  = _mm256_sub_pd( v_tmp1, v_sigma_mu );
			v_resm0 = _mm256_load_pd( &ptr_res_m[jj+0] );
			v_resm1 = _mm256_load_pd( &ptr_res_m[jj+pnb] );
			v_resm0 = _mm256_add_pd( v_resm0, v_tmp0 );
			v_resm1 = _mm256_add_pd( v_resm1, v_tmp1 );
			_mm256_store_pd( &ptr_res_m[jj+0], v_resm0 );
			_mm256_store_pd( &ptr_res_m[jj+pnb], v_resm1 );
			}
		if(jj<nb[ii])
			{
			ii_left = nb[ii]-jj;
			i_mask  = _mm256_castpd_si256( _mm256_sub_pd( _mm256_loadu_pd( d_mask ), _mm256_broadcast_sd( &ii_left ) ) );

			v_dt0   = _mm256_load_pd( &ptr_dt[jj+0] );
			v_dt1   = _mm256_load_pd( &ptr_dt[jj+pnb] );
			v_dlam0 = _mm256_load_pd( &ptr_dlam[jj+0] );
			v_dlam1 = _mm256_load_pd( &ptr_dlam[jj+pnb] );
			v_tmp0  = _mm256_mul_pd( v_dt0, v_dlam0 );
			v_tmp1  = _mm256_mul_pd( v_dt1, v_dlam1 );
			v_tmp0  = _mm256_sub_pd( v_tmp0, v_sigma_mu );
			v_tmp1  = _mm256_sub_pd( v_tmp1, v_sigma_mu );
			v_resm0 = _mm256_load_pd( &ptr_res_m[jj+0] );
			v_resm1 = _mm256_load_pd( &ptr_res_m[jj+pnb] );
			v_resm0 = _mm256_add_pd( v_resm0, v_tmp0 );
			v_resm1 = _mm256_add_pd( v_resm1, v_tmp1 );
			_mm256_maskstore_pd( &ptr_res_m[jj+0], i_mask, v_resm0 );
			_mm256_maskstore_pd( &ptr_res_m[jj+pnb], i_mask, v_resm1 );
			}

		ptr_res_m += 2*pnb;
		ptr_dt    += 2*pnb;
		ptr_dlam  += 2*pnb;

		for(jj=0; jj<ng[ii]-3; jj+=4)
			{
			v_dt0   = _mm256_load_pd( &ptr_dt[jj+0] );
			v_dt1   = _mm256_load_pd( &ptr_dt[jj+png] );
			v_dlam0 = _mm256_load_pd( &ptr_dlam[jj+0] );
			v_dlam1 = _mm256_load_pd( &ptr_dlam[jj+png] );
			v_tmp0  = _mm256_mul_pd( v_dt0, v_dlam0 );
			v_tmp1  = _mm256_mul_pd( v_dt1, v_dlam1 );
			v_tmp0  = _mm256_sub_pd( v_tmp0, v_sigma_mu );
			v_tmp1  = _mm256_sub_pd( v_tmp1, v_sigma_mu );
			v_resm0 = _mm256_load_pd( &ptr_res_m[jj+0] );
			v_resm1 = _mm256_load_pd( &ptr_res_m[jj+png] );
			v_resm0 = _mm256_add_pd( v_resm0, v_tmp0 );
			v_resm1 = _mm256_add_pd( v_resm1, v_tmp1 );
			_mm256_store_pd( &ptr_res_m[jj+0], v_resm0 );
			_mm256_store_pd( &ptr_res_m[jj+png], v_resm1 );
			}
		if(jj<ng[ii])
			{
			ii_left = ng[ii]-jj;
			i_mask  = _mm256_castpd_si256( _mm256_sub_pd( _mm256_loadu_pd( d_mask ), _mm256_broadcast_sd( &ii_left ) ) );

			v_dt0   = _mm256_load_pd( &ptr_dt[jj+0] );
			v_dt1   = _mm256_load_pd( &ptr_dt[jj+png] );
			v_dlam0 = _mm256_load_pd( &ptr_dlam[jj+0] );
			v_dlam1 = _mm256_load_pd( &ptr_dlam[jj+png] );
			v_tmp0  = _mm256_mul_pd( v_dt0, v_dlam0 );
			v_tmp1  = _mm256_mul_pd( v_dt1, v_dlam1 );
			v_tmp0  = _mm256_sub_pd( v_tmp0, v_sigma_mu );
			v_tmp1  = _mm256_sub_pd( v_tmp1, v_sigma_mu );
			v_resm0 = _mm256_load_pd( &ptr_res_m[jj+0] );
			v_resm1 = _mm256_load_pd( &ptr_res_m[jj+png] );
			v_resm0 = _mm256_add_pd( v_resm0, v_tmp0 );
			v_resm1 = _mm256_add_pd( v_resm1, v_tmp1 );
			_mm256_maskstore_pd( &ptr_res_m[jj+0], i_mask, v_resm0 );
			_mm256_maskstore_pd( &ptr_res_m[jj+png], i_mask, v_resm1 );
			}

		}

	}



void d_update_gradient_res_mpc_hard_tv(int N, int *nx, int *nu, int *nb, int *ng, double **res_d, double **res_m, double **lam, double **t_inv, double **qx)
	{
	
	// constants
	const int bs = D_MR;

	int nb0, pnb, ng0, png;
	
	double temp0, temp1;
	
	double 
		*ptr_res_d, *ptr_Qx, *ptr_qx, *ptr_lam, *ptr_res_m, *ptr_t_inv;
	
	__m256d
		v_ones,
		v_tmp0, v_tinv0, v_lam0, v_resm0, v_resd0,
		v_tmp1, v_tinv1, v_lam1, v_resm1, v_resd1;
	
	__m256i
		i_mask;
	
	v_ones = _mm256_set_pd( 1.0, 1.0, 1.0, 1.0 );

	int ii, jj, bs0;
	
	double ii_left;

	static double d_mask[4] = {0.5, 1.5, 2.5, 3.5};

	for(jj=0; jj<=N; jj++)
		{
		
		ptr_lam   = lam[jj];
		ptr_t_inv = t_inv[jj];
		ptr_res_d = res_d[jj];
		ptr_res_m = res_m[jj];
		ptr_qx    = qx[jj];

		// box constraints
		nb0 = nb[jj];
		if(nb0>0)
			{

			pnb  = (nb0+bs-1)/bs*bs; // simd aligned number of box constraints

			for(ii=0; ii<nb0-3; ii+=4)
				{

				v_lam0  = _mm256_load_pd( &ptr_lam[ii+0] );
				v_lam1  = _mm256_load_pd( &ptr_lam[ii+pnb] );
				v_resm0 = _mm256_load_pd( &ptr_res_m[ii+0] );
				v_resm1 = _mm256_load_pd( &ptr_res_m[ii+pnb] );
				v_resd0 = _mm256_load_pd( &ptr_res_d[ii+0] );
				v_resd1 = _mm256_load_pd( &ptr_res_d[ii+pnb] );
				v_tinv0 = _mm256_load_pd( &ptr_t_inv[ii+0] );
				v_tinv1 = _mm256_load_pd( &ptr_t_inv[ii+pnb] );
				v_tmp0  = _mm256_mul_pd( v_lam0, v_resd0 );
				v_tmp1  = _mm256_mul_pd( v_lam1, v_resd1 );
				v_tmp0  = _mm256_sub_pd( v_resm0, v_tmp0 );
				v_tmp1  = _mm256_add_pd( v_resm1, v_tmp1 );
				v_tmp0  = _mm256_mul_pd( v_tmp0, v_tinv0 );
				v_tmp1  = _mm256_mul_pd( v_tmp1, v_tinv1 );
				v_tmp0  = _mm256_sub_pd( v_tmp0, v_tmp1 );
				_mm256_store_pd( &ptr_qx[ii+0], v_tmp0 );

				}
			if(ii<nb0)
				{

				ii_left = nb0-ii;
				i_mask  = _mm256_castpd_si256( _mm256_sub_pd( _mm256_loadu_pd( d_mask ), _mm256_broadcast_sd( &ii_left ) ) );

				v_lam0  = _mm256_load_pd( &ptr_lam[ii+0] );
				v_lam1  = _mm256_load_pd( &ptr_lam[ii+pnb] );
				v_resm0 = _mm256_load_pd( &ptr_res_m[ii+0] );
				v_resm1 = _mm256_load_pd( &ptr_res_m[ii+pnb] );
				v_resd0 = _mm256_load_pd( &ptr_res_d[ii+0] );
				v_resd1 = _mm256_load_pd( &ptr_res_d[ii+pnb] );
				v_tinv0 = _mm256_load_pd( &ptr_t_inv[ii+0] );
				v_tinv1 = _mm256_load_pd( &ptr_t_inv[ii+pnb] );
				v_tmp0  = _mm256_mul_pd( v_lam0, v_resd0 );
				v_tmp1  = _mm256_mul_pd( v_lam1, v_resd1 );
				v_tmp0  = _mm256_sub_pd( v_resm0, v_tmp0 );
				v_tmp1  = _mm256_add_pd( v_resm1, v_tmp1 );
				v_tmp0  = _mm256_mul_pd( v_tmp0, v_tinv0 );
				v_tmp1  = _mm256_mul_pd( v_tmp1, v_tinv1 );
				v_tmp0  = _mm256_sub_pd( v_tmp0, v_tmp1 );
				_mm256_maskstore_pd( &ptr_qx[ii+0], i_mask, v_tmp0 );

				}

			ptr_lam   += 2*pnb;
			ptr_t_inv += 2*pnb;
			ptr_res_d += 2*pnb;
			ptr_res_m += 2*pnb;
			ptr_qx    += pnb;

			}

		// general constraints
		ng0 = ng[jj];
		if(ng0>0)
			{

			png = (ng0+bs-1)/bs*bs; // simd aligned number of general constraints

			for(ii=0; ii<ng0-3; ii+=4)
				{

				v_lam0  = _mm256_load_pd( &ptr_lam[ii+0] );
				v_lam1  = _mm256_load_pd( &ptr_lam[ii+png] );
				v_resm0 = _mm256_load_pd( &ptr_res_m[ii+0] );
				v_resm1 = _mm256_load_pd( &ptr_res_m[ii+png] );
				v_resd0 = _mm256_load_pd( &ptr_res_d[ii+0] );
				v_resd1 = _mm256_load_pd( &ptr_res_d[ii+png] );
				v_tinv0 = _mm256_load_pd( &ptr_t_inv[ii+0] );
				v_tinv1 = _mm256_load_pd( &ptr_t_inv[ii+png] );
				v_tmp0  = _mm256_mul_pd( v_lam0, v_resd0 );
				v_tmp1  = _mm256_mul_pd( v_lam1, v_resd1 );
				v_tmp0  = _mm256_sub_pd( v_resm0, v_tmp0 );
				v_tmp1  = _mm256_add_pd( v_resm1, v_tmp1 );
				v_tmp0  = _mm256_mul_pd( v_tmp0, v_tinv0 );
				v_tmp1  = _mm256_mul_pd( v_tmp1, v_tinv1 );
				v_tmp0  = _mm256_sub_pd( v_tmp0, v_tmp1 );
				_mm256_store_pd( &ptr_qx[ii+0], v_tmp0 );

				}
			if(ii<ng0)
				{

				ii_left = ng0-ii;
				i_mask  = _mm256_castpd_si256( _mm256_sub_pd( _mm256_loadu_pd( d_mask ), _mm256_broadcast_sd( &ii_left ) ) );

				v_lam0  = _mm256_load_pd( &ptr_lam[ii+0] );
				v_lam1  = _mm256_load_pd( &ptr_lam[ii+png] );
				v_resm0 = _mm256_load_pd( &ptr_res_m[ii+0] );
				v_resm1 = _mm256_load_pd( &ptr_res_m[ii+png] );
				v_resd0 = _mm256_load_pd( &ptr_res_d[ii+0] );
				v_resd1 = _mm256_load_pd( &ptr_res_d[ii+png] );
				v_tinv0 = _mm256_load_pd( &ptr_t_inv[ii+0] );
				v_tinv1 = _mm256_load_pd( &ptr_t_inv[ii+png] );
				v_tmp0  = _mm256_mul_pd( v_lam0, v_resd0 );
				v_tmp1  = _mm256_mul_pd( v_lam1, v_resd1 );
				v_tmp0  = _mm256_sub_pd( v_resm0, v_tmp0 );
				v_tmp1  = _mm256_add_pd( v_resm1, v_tmp1 );
				v_tmp0  = _mm256_mul_pd( v_tmp0, v_tinv0 );
				v_tmp1  = _mm256_mul_pd( v_tmp1, v_tinv1 );
				v_tmp0  = _mm256_sub_pd( v_tmp0, v_tmp1 );
				_mm256_maskstore_pd( &ptr_qx[ii+0], i_mask, v_tmp0 );

				}

			}

		}

	}




