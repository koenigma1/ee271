#include "rastBBox_fix_sv.h"
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <stdarg.h>
void abort_(const char * s, ...)
{
  va_list args;
  va_start(args, s);
  vfprintf(stderr, s, args);
  fprintf(stderr, "\n");
  va_end(args);
  abort();
}
//Given a uPoly and Bbox check that Bbox is good bound 
// on uPoly
int rastBBox_bbox_check( int   v0_x,     //uPoly
			  int   v0_y,     //uPoly
			  int   v1_x,     //uPoly
			  int   v1_y,     //uPoly
			  int   v2_x,     //uPoly
			  int   v2_y,     //uPoly
			  int   v3_x,     //uPoly
			  int   v3_y,     //uPoly
			  int  q,        //uPoly
			  int  valid_Poly,
			  int   ll_x,     //BBOX
			  int   ll_y,     //BBOX
			  int   ur_x,     //BBOX
			  int   ur_y,     //BBOX
			  int   ss_w_lg2, //Subsample
			  int   screen_w, //Screen
			  int   screen_h, //Screen
			  int  valid ,   //BBoX
			  int   r_shift,  //Config
			  int   r_val     //Congig 
			  )
{

  //SETUP Values to Be Checked
  int   check_ll_x  = ll_x;     //BBOX
  int   check_ll_y  = ll_y;     //BBOX
  int   check_ur_x  = ur_x;     //BBOX
  int   check_ur_y  = ur_y;     //BBOX
  int  check_valid = valid ;   //BBOX  

  int  correct = 1 ;  //Assume true

  u_Poly poly;
  poly.v[0].x[0] = v0_x;
  poly.v[0].x[1] = v0_y;
  poly.v[1].x[0] = v1_x; 
  poly.v[1].x[1] = v1_y;
  poly.v[2].x[0] = v2_x;
  poly.v[2].x[1] = v2_y;
  poly.v[3].x[0] = v3_x;
  poly.v[3].x[1] = v3_y;
  poly.q = q;

  //
  //Copy Past C++ Bounding Box Function ****BEGIN****
  //
  // note that bool,true, and false are not in c

        int i;
  			int vertices = (poly.q) ? 4:3; // used to be poly.vertices
        #define _FLOOR_MASK  ~((1 << (r_shift - ss_w_lg2)) - 1)
        #define _FLOOR_SS(SS) (SS & _FLOOR_MASK)
        #define _CEIL_SS(SS)  _FLOOR_SS(SS - 1 + (1 << (r_shift - ss_w_lg2)))
        #define _MIN_VAL(X1,X2) ((X1 < X2) ? X1 : X2)
        #define _MIN_PER_AXIS(X1,X2,X3,X4) (( _MIN_VAL(X1,X2) < _MIN_VAL(X3,X4)) ? _MIN_VAL(X1,X2) : _MIN_VAL(X3,X4))
        #define _MAX_VAL(X1,X2) ((X1 > X2) ? X1 : X2)
        #define _MAX_PER_AXIS(X1,X2,X3,X4) (( _MAX_VAL(X1,X2) > _MAX_VAL(X3,X4)) ? _MAX_VAL(X1,X2) : _MAX_VAL(X3,X4))
        #define _VALID_COORD(VERTICE, AXIS, PITCH) (poly.v[VERTICE].x[AXIS] > 0 && poly.v[VERTICE].x[AXIS] < PITCH)

        // Validate that the polygon lays within the screen
        valid = 0;
        // If at least 1 point is valid we are good
        for(i = 0; i < vertices; i++) {
                if( _VALID_COORD(i, 0, screen_w) && _VALID_COORD(i, 1, screen_h))
                        valid = 1;
        }
        if (!valid)
                                        return;

        // Calculate a clamped BBox and floor the fixed point result
        if (vertices == 3) {
          ll_x = _FLOOR_SS( _MIN_PER_AXIS(poly.v[0].x[0], poly.v[1].x[0], poly.v[2].x[0], poly.v[2].x[0]) );
          ur_x = _FLOOR_SS( _MAX_PER_AXIS(poly.v[0].x[0], poly.v[1].x[0], poly.v[2].x[0], poly.v[2].x[0]) );
          ll_y = _FLOOR_SS( _MIN_PER_AXIS(poly.v[0].x[1], poly.v[1].x[1], poly.v[2].x[1], poly.v[2].x[1]) );
          ur_y = _FLOOR_SS( _MAX_PER_AXIS(poly.v[0].x[1], poly.v[1].x[1], poly.v[2].x[1], poly.v[2].x[1]) );
        } else {
          ll_x = _FLOOR_SS( _MIN_PER_AXIS(poly.v[0].x[0], poly.v[1].x[0], poly.v[2].x[0], poly.v[3].x[0]) );
          ur_x = _FLOOR_SS( _MAX_PER_AXIS(poly.v[0].x[0], poly.v[1].x[0], poly.v[2].x[0], poly.v[3].x[0]) );
          ll_y = _FLOOR_SS( _MIN_PER_AXIS(poly.v[0].x[1], poly.v[1].x[1], poly.v[2].x[1], poly.v[3].x[1]) );
          ur_y = _FLOOR_SS( _MAX_PER_AXIS(poly.v[0].x[1], poly.v[1].x[1], poly.v[2].x[1], poly.v[3].x[1]) );
        }

      // Clip BBox to visible screen space
        ur_x = _MIN_VAL(ur_x, screen_w);
        ur_y = _MIN_VAL(ur_y, screen_h);
        ll_x = _MAX_VAL(ll_x, 0);
        ll_y = _MAX_VAL(ll_y, 0);

        #undef _FLOOR_SS
        #undef _FLOOR_MASK
        #undef _MIN_VAL
        #undef _MIN_PER_AXIS
        #undef _MAX_VAL
        #undef _MAX_PER_AXIS

  //
  //Copy Past C++ Bounding Box Function ****END****
  //

  //Check if BBox matches
  correct = 1 ;

  correct = check_valid == valid ? correct : 0 ; 
  if( check_valid != valid ){
    printf( "\nerror: valid signal incorrect hw vs gold %i vs %i (valid_input=%i)\n" , check_valid , valid, valid_Poly);
    // printf( "\debug: ll_x  vs gold   %d vs %d\n" , ll_x ,check_ll_x);
    // printf( "\debug: ll_y  vs gold   %d vs %d\n" , ll_y ,check_ll_y);
    // printf( "\debug: ur_x  vs gold   %d vs %d\n" , ur_x ,check_ur_x);
    // printf( "\debug: ur_y  vs gold   %d vs %d\n" , ur_y ,check_ur_y);
  }

  if( check_valid == valid && valid == 1 ){

    correct = check_ll_x  == ll_x   ? correct : 0 ;  
    if( check_ll_x  != ll_x ){
      printf( "\nerror: ll_x  != gold   %d != %d\n" , ll_x ,check_ll_x);
    }
    
    correct = check_ll_y  == ll_y   ? correct : 0 ;  
    if( check_ll_y  != ll_y ){
      printf( "\nerror: ll_y  != gold   %d != %d\n" , ll_y ,check_ll_y);
    }
    
    correct = check_ur_x  == ur_x   ? correct : 0 ;  
    if( check_ur_x  != ur_x ){
      printf( "\nerror: ur_x != gold   %d != %d\n" , ur_x ,check_ur_x);
    }
    
    correct = check_ur_y  == ur_y   ? correct : 0 ;  
    if( check_ur_y  != ur_y ){
      printf( "\nerror: ur_y  != gold   %d != %d\n" , ur_y ,check_ur_y);
    }
  }
  return  correct ; 
  //Check if BBox matches
  
}




//Given a uPoly and a sample location check that hit agrees with
// sample lieing in/out of uPoly.
// Return 0 if disagree
// Return 1 if agree
int rastBBox_stest_check( int   v0_x,      //uPoly
			  int   v0_y,      //uPoly
			  int   v1_x,      //uPoly
			  int   v1_y,      //uPoly
			  int   v2_x,      //uPoly
			  int   v2_y,      //uPoly
			  int   v3_x,      //uPoly
			  int   v3_y,      //uPoly
			  int   q,         //uPoly
			  int   s_x,       //SAMPLE 
			  int   s_y,       //SAMPLE
			  int   hit        //HIT
			  )   
{

  //SETUP Values to Be Checked
  int  check_hit = hit ; // Check if Hit is correct
  int  correct = 1 ;  //Assume true

  u_Poly poly;
  poly.v[0].x[0] = v0_x;
  poly.v[0].x[1] = v0_y;
  poly.v[1].x[0] = v1_x; 
  poly.v[1].x[1] = v1_y;
  poly.v[2].x[0] = v2_x;
  poly.v[2].x[1] = v2_y;
  poly.v[3].x[0] = v3_x;
  poly.v[3].x[1] = v3_y;
  poly.q = q;

  //
  //Copy Past C++ Sample Test Function ****BEGIN****
  //
  // note that bool,true, and false are not in c

  

  int result = 0 ; // Default to miss state
  int vertices = (poly.q) ? 4:3; // used to be poly.vertices

  /////
  ///// Sample Test Function Goes Here
  /////

  // shift vertices such that sample is origin
  v0_x = poly.v[0].x[0] - s_x;
  v0_y = poly.v[0].x[1] - s_y;
  v1_x = poly.v[1].x[0] - s_x;
  v1_y = poly.v[1].x[1] - s_y;
  v2_x = poly.v[2].x[0] - s_x;
  v2_y = poly.v[2].x[1] - s_y;
  v3_x = poly.v[3].x[0] - s_x;
  v3_y = poly.v[3].x[1] - s_y;

  // Distance of origin shifted edge
  if (poly.q) {
    long dist0 = v0_x * v1_y - v1_x * v0_y; // 0-1 edge
    long dist1 = v1_x * v2_y - v2_x * v1_y; // 1-2 edge
    long dist2 = v2_x * v3_y - v3_x * v2_y; // 1-2 edge
    long dist3 = v3_x * v0_y - v0_x * v3_y; // 1-2 edge
    long dist4 = v1_x * v3_y - v3_x * v1_y; // 1-2 edge

    int b0 = dist0 <= 0;
    int b1 = dist1 < 0;
    int b2 = dist2 < 0;
    int b3 = dist3 <= 0;
    int b4 = dist4 < 0;
   
    result = ( b1 && b2 && ! b4 )
      || ( ! b1 && ! b2 && b4 )
      || ( b0 && b3 && b4 )
      || ( ! b0 && ! b3 && ! b4 ) ;
  } else {
    long dist0 = v0_x * v1_y - v1_x * v0_y; // 0-1 edge
    long dist1 = v1_x * v2_y - v2_x * v1_y; // 1-2 edge
    long dist2 = v2_x * v0_y - v0_x * v2_y; // 2-0 edge
  
    // Test if origin is on right side of shifted edge
    int b0 = dist0 <= 0;
    int b1 = dist1 < 0;
    int b2 = dist2 <= 0;

    // Triangle min terms with culling
    result = ( b0 && b1 && b2 );
  }
  /////
  ///// Sample Test Function Goes Here
  /////
  //result = result-1; //Return 0 if hit, otherwise return -1

  //
  //Copy Past C++ Sample Test Function ****END****
  //

  //Check if Correct 
  correct = (result == check_hit) ;

  return correct ;
}




//Given a uPoly determine if the hits argument 
// is equivelant to the number of actual hits
int rastBBox_check( int   v0_x,      //uPoly
			   int   v0_y,      //uPoly
			   int   v1_x,      //uPoly
			   int   v1_y,      //uPoly
			   int   v2_x,      //uPoly
			   int   v2_y,      //uPoly
			   int   v3_x,      //uPoly
			   int   v3_y,      //uPoly
			   int   q,        //uPoly
			   int   hits,      //Number of Samples in uPoly
			   int   ss_w_lg2,  //Subsample
			   int   screen_w,  //Screen
			   int   screen_h,  //Screen
			   int   r_shift,   //Config
			   int   r_val      //Congig 
			   )   
{
  int ss_w = r_val >> ss_w_lg2 ;// SubSample Width
  int valid , ur_x, ur_y, ll_x, ll_y;

  u_Poly poly;
  poly.v[0].x[0] = v0_x;
  poly.v[0].x[1] = v0_y;
  poly.v[1].x[0] = v1_x; 
  poly.v[1].x[1] = v1_y;
  poly.v[2].x[0] = v2_x;
  poly.v[2].x[1] = v2_y;
  poly.v[3].x[0] = v3_x;
  poly.v[3].x[1] = v3_y;
  poly.q = q;

  //
  //Copy Past C++ Bounding Box Function ****BEGIN****
  //
  // note that bool,true, and false are not in c

        int i;
	int vertices = (poly.q) ? 4:3; // used to be poly.vertices

        #define _FLOOR_MASK  ~((1 << (r_shift - ss_w_lg2)) - 1)
        #define _FLOOR_SS(SS) (SS & _FLOOR_MASK)
        #define _CEIL_SS(SS)  _FLOOR_SS(SS - 1 + (1 << (r_shift - ss_w_lg2)))
        #define _MIN_VAL(X1,X2) ((X1 < X2) ? X1 : X2)
        #define _MIN_PER_AXIS(X1,X2,X3,X4) (( _MIN_VAL(X1,X2) < _MIN_VAL(X3,X4)) ? _MIN_VAL(X1,X2) : _MIN_VAL(X3,X4))
        #define _MAX_VAL(X1,X2) ((X1 > X2) ? X1 : X2)
        #define _MAX_PER_AXIS(X1,X2,X3,X4) (( _MAX_VAL(X1,X2) > _MAX_VAL(X3,X4)) ? _MAX_VAL(X1,X2) : _MAX_VAL(X3,X4))
        #define _VALID_COORD(VERTICE, AXIS, PITCH) (poly.v[VERTICE].x[AXIS] > 0 && poly.v[VERTICE].x[AXIS] < PITCH)

        // Validate that the polygon lays within the screen
        valid = 0;
        // If at least 1 point is valid we are good
        for(i = 0; i < vertices; i++) {
                if( _VALID_COORD(i, 0, screen_w) && _VALID_COORD(i, 1, screen_h))
                        valid = 1;
        }
        if (!valid)
                                        return;

        // Calculate a clamped BBox and floor the fixed point result
        if (vertices == 3) {
          ll_x = _FLOOR_SS( _MIN_PER_AXIS(poly.v[0].x[0], poly.v[1].x[0], poly.v[2].x[0], poly.v[2].x[0]) );
          ur_x = _FLOOR_SS( _MAX_PER_AXIS(poly.v[0].x[0], poly.v[1].x[0], poly.v[2].x[0], poly.v[2].x[0]) );
          ll_y = _FLOOR_SS( _MIN_PER_AXIS(poly.v[0].x[1], poly.v[1].x[1], poly.v[2].x[1], poly.v[2].x[1]) );
          ur_y = _FLOOR_SS( _MAX_PER_AXIS(poly.v[0].x[1], poly.v[1].x[1], poly.v[2].x[1], poly.v[2].x[1]) );
        } else {
          ll_x = _FLOOR_SS( _MIN_PER_AXIS(poly.v[0].x[0], poly.v[1].x[0], poly.v[2].x[0], poly.v[3].x[0]) );
          ur_x = _FLOOR_SS( _MAX_PER_AXIS(poly.v[0].x[0], poly.v[1].x[0], poly.v[2].x[0], poly.v[3].x[0]) );
          ll_y = _FLOOR_SS( _MIN_PER_AXIS(poly.v[0].x[1], poly.v[1].x[1], poly.v[2].x[1], poly.v[3].x[1]) );
          ur_y = _FLOOR_SS( _MAX_PER_AXIS(poly.v[0].x[1], poly.v[1].x[1], poly.v[2].x[1], poly.v[3].x[1]) );
        }

      // Clip BBox to visible screen space
        ur_x = _MIN_VAL(ur_x, screen_w);
        ur_y = _MIN_VAL(ur_y, screen_h);
        ll_x = _MAX_VAL(ll_x, 0);
        ll_y = _MAX_VAL(ll_y, 0);

        #undef _FLOOR_SS
        #undef _FLOOR_MASK
        #undef _MIN_VAL
        #undef _MIN_PER_AXIS
        #undef _MAX_VAL
        #undef _MAX_PER_AXIS

  //
  //Copy Past C++ Bounding Box Function ****END****
  //


  int s_x , sl_x ;
  int s_y , sl_y ;
  ushort j_x , j_y ;
  int count = 0 ;

  for( sl_x = ll_x ; sl_x <= ur_x ; sl_x += ss_w )
  {
    for( sl_y = ll_y ; sl_y <= ur_y ; sl_y += ss_w ){

      rastBBox_jhash_jit_fix( sl_x, sl_y, ss_w_lg2, &j_x, &j_y);
      s_x = sl_x + (j_x << 2) ;
      s_y = sl_y + (j_y << 2) ;

      //
      //Copy Past C++ Sample Test Function ****BEGIN****
      //
      // note that bool,true, and false are not in c
       

  int result = 0 ; // Default to miss state
  int vertices = (poly.q) ? 4:3; // used to be poly.vertices

  /////
  ///// Sample Test Function Goes Here
  /////

  // shift vertices such that sample is origin
  v0_x = poly.v[0].x[0] - s_x;
  v0_y = poly.v[0].x[1] - s_y;
  v1_x = poly.v[1].x[0] - s_x;
  v1_y = poly.v[1].x[1] - s_y;
  v2_x = poly.v[2].x[0] - s_x;
  v2_y = poly.v[2].x[1] - s_y;
  v3_x = poly.v[3].x[0] - s_x;
  v3_y = poly.v[3].x[1] - s_y;

  // Distance of origin shifted edge
  if (poly.q) {
    long dist0 = v0_x * v1_y - v1_x * v0_y; // 0-1 edge
    long dist1 = v1_x * v2_y - v2_x * v1_y; // 1-2 edge
    long dist2 = v2_x * v3_y - v3_x * v2_y; // 1-2 edge
    long dist3 = v3_x * v0_y - v0_x * v3_y; // 1-2 edge
    long dist4 = v1_x * v3_y - v3_x * v1_y; // 1-2 edge

    int b0 = dist0 <= 0;
    int b1 = dist1 < 0;
    int b2 = dist2 < 0;
    int b3 = dist3 <= 0;
    int b4 = dist4 < 0;
   
    result = ( b1 && b2 && ! b4 )
      || ( ! b1 && ! b2 && b4 )
      || ( b0 && b3 && b4 )
      || ( ! b0 && ! b3 && ! b4 ) ;
  } else {
    long dist0 = v0_x * v1_y - v1_x * v0_y; // 0-1 edge
    long dist1 = v1_x * v2_y - v2_x * v1_y; // 1-2 edge
    long dist2 = v2_x * v0_y - v0_x * v2_y; // 2-0 edge
  
    // Test if origin is on right side of shifted edge
    int b0 = dist0 <= 0;
    int b1 = dist1 < 0;
    int b2 = dist2 <= 0;

    // Triangle min terms with culling
    result = ( b0 && b1 && b2 );
  }

  /////
  ///// Sample Test Function Goes Here
  /////
  //result = result-1; //Return 0 if hit, otherwise return -1



	  
	  
	  
	  
	  
	  
	  
	  
	  
  
      //
      //Copy Past C++ Sample Test Function ****END****
      //
      
      count = result != 0 ? count+1 : count  ;
    }
  }

  if( hits != count ){
    printf( "Check hw to gold  %d to %d \n" , hits , count );
  	 printf ("BBox: (%lx,%lx) - (%lx,%lx)\n\n", ll_x, ll_y, ur_x, ur_y);
  }

  return hits == count;
}



int zbuff_init( int w,
		int h,
		int ss_w 
		)
{
  int i ;

  wB = w ; 
  hB = h ;
  sswB = ss_w;
  ssB = ss_w * ss_w ;

  frameBuffer = (ushort*)malloc( sizeof( ushort ) * wB * hB * ssB * 4 ) ;
  depthBuffer = (uint*)malloc( sizeof( uint ) *  wB * hB * ssB ) ; 

  for( i = 0 ; i < ( wB * hB * ssB * 4 ) ; i++ ){
    frameBuffer[i] = 0 ;
  }

  for( i = 0 ; i < ( wB * hB * ssB ) ; i++ ){
    depthBuffer[i] = UINT_MAX ;
  }

  return 1;
} 

#define  IDX_F( x , y , sx , sy , c ) \
  ((((( y*wB ) + x)*sswB+sy)*sswB+sx)*4+c)

#define  IDX_D( x , y , sx , sy ) \
  (((( y*wB ) + x)*sswB+sy)*sswB+sx)


int zbuff_rop(  int x , 
		int y , 
		int ss_x , 
		int ss_y ,
		int d ,    //actually a uint
		int R ,    //actually a ushort
		int G ,    //actually a ushort
		int B      //actually a ushort
		)
{
 
  if( d <= depthBuffer[ IDX_D( x, y, ss_x, ss_y ) ] ){
    depthBuffer[ IDX_D( x, y, ss_x, ss_y ) ] = d ;
    uint id = IDX_F( x, y, ss_x, ss_y, 0 ) ;
    frameBuffer[ id ] = R ;
    frameBuffer[ id + 1 ] = G ;
    frameBuffer[ id + 2 ] = B ;
    frameBuffer[ id + 3 ] = USHRT_MAX ;
  }

  return 1;
}

void eval_ss1( uchar* rgb , ushort* fb_pix)
{
  int i , j , k;

  uint rgb_l[4];
  rgb_l[0] = 0 ;
  rgb_l[1] = 0 ;
  rgb_l[2] = 0 ;

  for( i = 0 ; i < sswB ; i++ )
    for( j = 0 ; j < sswB ; j++ )
      for( k = 0 ; k < 4 ; k++ ){
	rgb_l[k] += fb_pix[ (i*sswB+j)*4 + k ];
      }

  for( k = 0 ; k < 3 ; k++ )
    rgb[k] = (uchar) (( rgb_l[k] / ssB ) >> ( 8 )) ;
  
}

uchar* eval_ss(){

  int x ;
  int y ;

  uchar* rgb ;
  ushort* fb_pix ;

  uchar* img = blank(wB,hB);
  
  for( y=0 ;  y<hB ; y++ ) {
    for( x=0 ; x<wB ; x++ ) {
      rgb = &(img[ (y*wB + x)*3]);
      fb_pix = &( frameBuffer[ IDX_F( x , y , 0 , 0 , 0 ) ] ) ;
      eval_ss1( rgb , fb_pix ) ;
    }
  }

  return img ;
}

int write_ppm( )
{

  uchar* img = eval_ss();
  write_ppm_file( "sv_out.ppm" , img  , wB , hB );

  return 1;
}

uchar *blank( int w , int h )
{
  int x ;
  int y ;

  uchar* img ;
  uchar* rgba ;

  img = (uchar*) malloc( sizeof(uchar) * w * h * 3 );
  
  for( y=0 ;  y<h ; y++ ) {
    for( x=0 ; x<w ; x++ ) {
      rgba = &(img[(y*w+x)*3]);
      rgba[0] =  255 ; // Set R
      rgba[1] =  255 ; // Set G
      rgba[2] =  255 ; // Set B
    }
  }
  
  return img;
}

void write_ppm_file( 
		    char* file_name , 
		    uchar* imgBuffer , 
		    int w , 
		    int h 
		     )
{
  /*Taken From: http://www.cse.ohio-state.edu/~shareef/cse681/labs/writePPM.html */

  FILE *stream;
  int iy, ix;

  stream = fopen(file_name, "wb" );            // Open the file for write
  fprintf( stream, "P6\n%d %d\n255\n", w, h ); // Write the file header information
  
  for( iy = 0; iy < h; iy++){             // Write the contents of the buffer to file
    for( ix = 0; ix < w; ix++ ){

      // Access pixel (ix, iy) by indexing the appropriate row and then column in the
      // image buffer and taking into account that each pixel has
      // three unsigned char values. This command will write a single pixel value (rgb)
      // to the file in binary format.
      fwrite( imgBuffer + (iy * w + ix) * 3, 1, 3, stream );
    }
  }

  fclose( stream );

  return ;
}


void rastBBox_40t8_hash( uchar* arr40 , ushort* val , int shift )
{
  uchar arr32[4];
  uchar arr16[2];
  uchar arr8;

  ushort mask = 0x00ff ;
  mask = mask >> shift ;

  arr32[0] = arr40[0] ^ arr40[1] ; 
  arr32[1] = arr40[1] ^ arr40[2] ; 
  arr32[2] = arr40[2] ^ arr40[3] ; 
  arr32[3] = arr40[3] ^ arr40[4] ; 

  arr16[0] = arr32[0] ^ arr32[2] ;
  arr16[1] = arr32[1] ^ arr32[3] ;

  arr8 = arr16[0] ^ arr16[1] ;

  mask = arr8 & mask ;
  val[0] = mask ;

}

//int count = 0 ;

void rastBBox_jhash_jit_fix( 
			      long s_x,
			      long s_y,
			      long ss_w_lg2,
			      ushort* jitter_x,
			      ushort* jitter_y)
{

  long  x = s_x >> 4 ;
  long  y = s_y >> 4 ; 
  uchar arr40_1[5] ;
  uchar arr40_2[5] ;

  long* arr40_1_ptr = (long*)arr40_1;
  long* arr40_2_ptr = (long*)arr40_2;

  ushort val_x[1] ; 
  ushort val_y[1] ; 

  *arr40_1_ptr = ( y << 20 ) | x ; 
  *arr40_2_ptr = ( x << 20 ) | y ; 

  //assert( (( y << 20 ) & x ) == 0 );
  //assert( (( x << 20 ) & y ) == 0 );

  /*printf( "C: %.6x %.6x -> %.5x %.5x -> %.10x %.10x -> %.2x %.2x %.2x %.2x %.2x   %.2x %.2x %.2x %.2x %.2x \n",
	  s_x , s_y ,
	  x , y ,
	  *arr40_1_ptr, *arr40_2_ptr,
	  arr40_1[4] , arr40_1[3], arr40_1[2], arr40_1[1], arr40_1[0],
	  arr40_2[4] , arr40_2[3], arr40_2[2], arr40_2[1], arr40_2[0]);*/

  /*if( s_x != 0 && s_y != 0 )
    count++;

  if( count > 10 ) 
  exit(0);*/

  rastBBox_40t8_hash( arr40_1 , val_x , ss_w_lg2 );
  rastBBox_40t8_hash( arr40_2 , val_y , ss_w_lg2 );

  *jitter_x = (long)( val_x[0] );
  *jitter_y = (long)( val_y[0] );

}

int rastBBox_jhash_jit_fix_check( 
			      int s_x,
			      int s_y,
			      int ss_w_lg2,
			      int jitter_x,
			      int jitter_y,
			      int s_j_x,
			      int s_j_y )
{

  ushort gold_jitter_x ;
  ushort gold_jitter_y ;
  long gold_s_j_x;
  long gold_s_j_y;
  int valid;
  

  rastBBox_jhash_jit_fix( s_x , s_y ,
			  ss_w_lg2,
			  &gold_jitter_x , 
			  &gold_jitter_y );

  gold_s_j_x = s_x + (gold_jitter_x << 2); //brittle only works for 10
  gold_s_j_y = s_y + (gold_jitter_y << 2); //brittle only works for 10

  valid = 1 ;

  if( (s_x & jitter_x) != 0 ){
    valid = 0 ;
    printf( "Jitter Inappropriately Large X %d %d\n" , s_x , jitter_x );
  }

  if( (s_y & jitter_y) != 0 ){
    valid = 0 ;
    printf( "Jitter Inappropriately Large Y %d %d\n" , s_y , jitter_y );
  }

  if( gold_jitter_x != jitter_x ){
    valid = 0 ;
    printf( " Mismatch on j_x Gold: %d Bench: %d\n" , gold_jitter_x , jitter_x );
  }

  if( gold_jitter_y != jitter_y ){
    valid = 0 ;
    printf( " Mismatch on j_y Gold: %d Bench: %d\n" , gold_jitter_y , jitter_y );
  }

  if( gold_s_j_x != s_j_x ){
    valid = 0 ;
    printf( " Mismatch on j_x Gold: %ld Bench: %d\n" , gold_s_j_x , s_j_x );
  }

  if( gold_s_j_y != s_j_y ){
    valid = 0 ;
    printf( " Mismatch on j_y Gold: %ld Bench: %d\n" , gold_s_j_y , s_j_y );
  }

  return valid ;

}
