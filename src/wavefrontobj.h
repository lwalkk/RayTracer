/* wavefrontobj.h
 */


#ifndef WAVEFRONTOBJ_H
#define WAVEFRONTOBJ_H


#include "object.h"
#include "wavefront.h"
#include "bvh.h"


class WavefrontObj : public Object {

  void copyWavefrontToBVH( BVH &bvh );

 public:

  wfModel *obj;

  BVH bvh;			/* bounding volume hierarchy of triangle primitives */

  WavefrontObj() {}

  WavefrontObj( const char *filename ) {
    obj = new wfModel( filename, MIPMAP_LINEAR ); // Read the object
    copyWavefrontToBVH( bvh ); // Copy to the BVH
    bvh.buildTree(); // Build the BVH
  }

  void renderGL( GPUProgram * gpuProg, mat4 &WCS_to_VCS, mat4 &VCS_to_CCS ) {
    obj->draw( gpuProg, WCS_to_VCS, VCS_to_CCS );
  }
  
  bool rayInt( vec3 rayStart, vec3 rayDir, int objPartIndex, float maxParam, vec3 &intPoint, vec3 &intNorm, vec3 &intTexCoords, float &intParam, Material * &mat, int &intPartIndex ) {
    return bvh.rayInt( rayStart, rayDir, objPartIndex, maxParam, intPoint, intNorm, intTexCoords, intParam, mat, intPartIndex );
  }

  vec3 textureColour( vec3 &p, int objPartIndex, float &alpha, vec3 &texCoords ) {
    return bvh.textureColour( p, objPartIndex, alpha, texCoords );
  }

  void renderGL() {
    cerr << "No renderGL method without gpuProgram" << endl;
    exit(1);
  }

  void input( istream &stream ) {
    cerr << "No streaming input method for Wavefront objects" << endl;
    exit(1);
  }

  void output( ostream &stream ) {
    stream << "wavefront" << endl
	   << "  " << obj->pathname << endl;
  }
};

#endif
