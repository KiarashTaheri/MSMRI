/*
head	1.1;
access;
symbols
	mifbet_v1_1:1.1
	start:1.1.1.1
	msmri:1.1.1
	mifbet_v1_0:1.1;
locks; strict;
comment	@ * @;


1.1
date	2010.12.06.22.31.49;	author kevin;	state Exp;
branches
	1.1.1.1;
next	;
commitid	d6usVf0bRaBMWgZu;

1.1.1.1
date	2011.03.11.19.36.48;	author kevin;	state Exp;
branches;
next	;
commitid	OyGdGxDa2z7iWsbv;


desc
@@


1.1
log
@Added bet source code and makefile.  This version of bet is possibly part
of FSL v1.3.  This source code compiles, links and runs on Debian 5.0.
Preliminary tests show that it behaves similarly to the research version,
located in /res1/code/packages/fsl/bin.  Requires validation on a number of
MIFs for this code to be certified for MIFBet v1.0 formal release.
@
text
@
/*
 * (c) Copyright, 1986-1995
 * Biomedical Imaging Resource
 * Mayo Foundation
 *
 
*/

struct header_key          		/*      header_key       */
    {                                  		/* off + size*/
        int sizeof_hdr;                		/* 0 + 4     */
        char data_type[10];            		/* 4 + 10    */
        char db_name[18];              		/* 14 + 18   */
        int extents;                   		/* 32 + 4    */
        short int session_error;       		/* 36 + 2    */
        char regular;                  		/* 38 + 1    */
        char hkey_un0;                 		/* 39 + 1    */
    };           				/* total=40  */

struct image_dimension      		/*      image_dimension  */
    {                               		/* off + size*/
        short int dim[8];           		/* 0 + 16    */
        char vox_units[4];			/* 16 + 4    */
        char cal_units[8];			/* 20 + 4    */
        short int unused1;			/* 24 + 2    */
        short int datatype;			/* 30 + 2    */
        short int bitpix;                   	/* 32 + 2    */
        short int dim_un0;                      /* 34 + 2    */
        float pixdim[8];                        /* 36 + 32   */
        float vox_offset;                    	/* 68 + 4    */
        float funused1;                       	/* 72 + 4    */
        float funused2;                      	/* 76 + 4    */
        float funused3;                      	/* 80 + 4    */
        float cal_max;                    	/* 84 + 4    */
        float cal_min;                     	/* 88 + 4    */
        int compressed;                     	/* 92 + 4    */
        int verified;                     	/* 96 + 4    */
        int glmax, glmin;                 	/* 100 + 8   */
    };          				/* total=108 */
         
struct data_history          		/*      data_history     */
    {                                    	/* off + size*/
        char descrip[80];                	/* 0 + 80    */
        char aux_file[24];               	/* 80 + 24   */
        char orient;                     	/* 104 + 1   */
        char originator[10];             	/* 105 + 10  */
        char generated[10];              	/* 115 + 10  */
        char scannum[10];                	/* 125 + 10  */
        char patient_id[10];             	/* 135 + 10  */
        char exp_date[10];               	/* 145 + 10  */
        char exp_time[10];               	/* 155 + 10  */
        char hist_un0[3];                	/* 165 + 3   */
        int views;                       	/* 168 + 4   */
        int vols_added;                  	/* 172 + 4   */
        int start_field;                 	/* 176 + 4   */
        int field_skip;                  	/* 180 + 4   */
        int omax,omin;                   	/* 184 + 8   */
        int smax,smin;                   	/* 192 + 8   */
    };                     			/* total=200 */

struct dsr                 		/*      dsr              */
    {                                  		/* off + size*/
        struct header_key hk;          		/* 0 + 40    */
        struct image_dimension dime;   		/* 40 + 108  */
        struct data_history hist;      		/* 148 + 200 */
    };                     			/* total=348 */
	
#define DT_NONE				0
#define DT_UNKNOWN			0
#define DT_BINARY			1
#define DT_UNSIGNED_CHAR	        2
#define DT_SIGNED_SHORT		        4
#define DT_SIGNED_INT		        8
#define DT_FLOAT			16
#define DT_COMPLEX			32
#define DT_DOUBLE			64
#define DT_RGB				128
#define DT_ALL				255
		
void swap_4(unsigned char*);
void swap_2(unsigned char*);
void swap_hdr(struct dsr*);
void avw_print(char*, struct dsr*);
void avw_read(char*, image_struct*);
void avw_write(char*, image_struct);


/*
1.1.1.1
log
@Starting new project "MIFBet".  Initial version 1.0 is released for Debian
5.0, compiled with CMIF v5.3.
@
text
@@

*/