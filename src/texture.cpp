/* texture.C
 */


#include "headers.h"
#ifndef _WIN32
  #include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <math.h>

#ifdef HAVEPNG
  #include <png.h>
#endif

#include "texture.h"

using namespace std;

bool Texture::useMipMaps = false;


/* Register the current texture with OpenGL, assigning
 * it a textureID.
 */

void Texture::registerWithOpenGL( )

{
  // Register it with OpenGL

  glGenTextures( 1, &textureID );
  glBindTexture( GL_TEXTURE_2D, textureID );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

  glTexImage2D( GL_TEXTURE_2D, 0, (hasAlpha ? GL_RGBA : GL_RGB), width, height, 0,
		(hasAlpha ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, texmap );

  glGenerateMipmap( GL_TEXTURE_2D );
}


/* Read a texture from a P6 PPM file
 */


unsigned char *Texture::readP6( char *filename )

{
  char buffer[1000];
  int i, xdim, ydim;
  unsigned char *a, *b, *pa, *pb;

  ifstream f( filename, ios::binary | ios::in );

  if (!f) {
    cerr << "Open of `" << filename << "' failed.\n";
    exit(1);
  }

  // first line

  do {
    i = 0;
    do 
      f.read(&buffer[i],1);
    while (buffer[i++] != '\n');
  } while (buffer[0] == '#');

  if (strncmp( buffer, "P6", 2 ) != 0) {
    cerr << filename << " is not a P6 file.\n";
    exit(1);
  }

  // second line

  do {
    i = 0;
    do 
      f.read(&buffer[i],1);
    while (buffer[i++] != '\n');
  } while (buffer[0] == '#');
  buffer[i] = '\0';
  sscanf( buffer, "%d %d", &xdim, &ydim );

  width = xdim;
  height = ydim;

  // third line

  do {
    i = 0;
    do 
      f.read(&buffer[i],1);
    while (buffer[i++] != '\n');
  } while (buffer[0] == '#');
  if (strncmp( buffer, "255", 3 ) != 0) {
    cerr << filename << " is not a 24-bit file.\n";
    exit(1);
  }

  // read the data (stored top-to-bottom, left-to-right)

  a = new unsigned char[ xdim * ydim * 3 ];
  f.read( (char *) a, xdim*ydim*3 );

  // flip the image vertically (stored bottom-to-top, left-to-right)

  b = new unsigned char[ xdim * ydim * 3 ];

  for (i=0; i<ydim; i++) {
    pa = a + (i)*xdim*3;
    pb = b + (ydim-1-i)*xdim*3;
    for (int j=0; j<xdim*3; j++)
      *(pb++) = *(pa++);
  }

  delete [] a;

  hasAlpha = false;

  return b;
}

#ifdef HAVEPNG
// Read a PNG file.  Most of this code is taken from example.c, which
// is provided with the libpng distribution.


#ifndef png_jmpbuf
#  define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#endif


#define PNG_BYTES_TO_CHECK 8

unsigned char *Texture::readPNG( char *filename )

{
  unsigned char *b, *pb;

  png_structp png_ptr;
  png_infop info_ptr;
  char header[PNG_BYTES_TO_CHECK];
  
  // Open file

  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    cerr << "Can't open PNG texture file '" << filename << "'." << endl;
    exit(-1);
  }

  // Check header

  int dummyStatus = fread( header, 1, PNG_BYTES_TO_CHECK, fp );
  bool is_png = !png_sig_cmp( (png_byte*) &header[0], 0, PNG_BYTES_TO_CHECK);
  if (!is_png) {
    cerr << "Texture file '" << filename << "' is not in PNG format." << endl;
    exit(-1);
  }

  /* Create and initialize the png_struct with the desired error handler
   * functions.  If you want to use the default stderr and longjump method,
   * you can supply NULL for the last three parameters.  We also supply the
   * the compiler header file version, so that we know if the application
   * was compiled with a compatible version of the library.  REQUIRED
   */

  png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );

  if (png_ptr == NULL) {
    cerr << "Can't initialize PNG file for reading: " << filename << endl;
    fclose(fp);
    exit(-1);
  }

  /* Allocate/initialize the memory for image information.  REQUIRED. */

  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL)
    {
      fclose(fp);
      png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
      cerr << "Can't allocate memory to read PNG file: " << filename << endl;
      exit(-1);
    }

  /* Set error handling if you are using the setjmp/longjmp method (this is
   * the normal method of doing things with libpng).  REQUIRED unless you
   * set up your own error handlers in the png_create_read_struct() earlier.
   */

  if (setjmp(png_jmpbuf(png_ptr))) {
    /* Free all of the memory associated with the png_ptr and info_ptr */
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    fclose(fp);
    /* If we get here, we had a problem reading the file */
    cerr << "Exception occurred while reading PNG file: " << filename << endl;
    exit(-1);
  }

  /* Set up the input control if you are using standard C streams */

  png_init_io(png_ptr, fp);

  /* If we have already read some of the signature */

  png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);

  // Warning: the following does NOT convert grey to RGB:

  png_read_png( png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL );

  
  // Store in texmap
  
  int numChannels = png_get_channels(png_ptr, info_ptr);

  if (png_get_bit_depth(png_ptr, info_ptr) != 8) {
    cerr << "Can't handle PNG files with bit depth other than 8.  '" << filename
	 << "' has " << png_get_bit_depth(png_ptr, info_ptr) << " bits per pixel." << endl;
    exit(-1);
  }


  width = png_get_image_width(png_ptr,info_ptr);
  height = png_get_image_height(png_ptr,info_ptr);

  int imageSize;

  if (numChannels == 4)
    imageSize = 4 * width * height;
  else
    imageSize = 3 * width * height;

  b = pb = new unsigned char[ imageSize ];

  for (int r=(int)info_ptr->height - 1; r >= 0; r--) {
    png_bytep row = info_ptr->row_pointers[r];
    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    for (int c=0; c < rowbytes; c++)
      switch (numChannels) {
      case 1:
	*(pb)++ = row[c];
	*(pb)++ = row[c];
	*(pb)++ = row[c];
	break;
      case 2:
	cerr << "Can't handle a two-channel PNG file: " << filename << endl;
	exit(-1);
	break;
      case 3:
      case 4:
	*(pb)++ = row[c];
	break;
      }
  }

  hasAlpha = (numChannels == 4);

  // Clean up PNG stuff

  png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
  fclose(fp);

  return b;
}
#endif

// Find the texel at [i][j] for i,j in [0,1]

vec3 Texture::texel( float i, float j, float &alpha )

{
  i = i - floor(i);
  j = j - floor(j);
  
  int x = (int) floor( i * (width-1) + 0.5 );
  int y = (int) floor( j * (height-1) + 0.5 );

  if (x<0) x = 0;
  if (x>width-1) x = width-1;
  if (y<0) y = 0;
  if (y>height-1) y = height-1;

  unsigned char *p;
  vec3 colour;

  p = texmap + (hasAlpha ? 4 : 3) * (y*width + x);

  colour.x = (*p++)/255.0f;
  colour.y = (*p++)/255.0f;
  colour.z = (*p++)/255.0f;

  if (hasAlpha)
    alpha = (*p++)/255.0f;
  else
    alpha = 1;

  return colour;
}

