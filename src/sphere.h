// sphere.h


#ifndef SPHERE_H
#define SPHERE_H


#include "linalg.h"
#include "seq.h"
#include "gpuProgram.h"
#include "object.h"


// icosahedron vertices (taken from Jon Leech http://www.cs.unc.edu/~jon)

#define tau 0.8506508084      /* t=(1+sqrt(5))/2, tau=t/sqrt(1+t^2)  */
#define one 0.5257311121      /* one=1/sqrt(1+t^2) , unit sphere     */

#define NUM_VERTS 12
#define NUM_FACES 20


class SphereFace {
 public:
  unsigned int v[3];
  SphereFace() {}
  SphereFace( int v0, int v1, int v2 ) {
    v[0] = v0; v[1] = v1; v[2] = v2;
  }
};


class Sphere: public Object {

  public:

  Sphere() {
    centre = vec3(0,0,0);
    radius = 1;
    mat = new Material();
    mat->setDefault();
    mat->kd = vec3( 215/255.0, 181/255.0, 71/255.0 );
    buildSphere( 2 );
  }

  Sphere( vec3 c, float r ) {
    centre = c;
    radius = r;
    mat = new Material();
    mat->setDefault();
    mat->kd = vec3( 215/255.0, 181/255.0, 71/255.0 );
    buildSphere( 2 );
  }

  // A sphere with 0 levels is a truncated icosahedron.  Each
  // additional level refines the previous level by converting each
  // triangular face into four triangular faces, placing all face
  // vertices at distance 1 from the origin.
  
  void buildSphere( int numLevels ) {

    for (int i=0; i<NUM_VERTS; i++)
      verts.add( icosahedronVerts[i] );

    for (int i=0; i<verts.size(); i++)
      verts[i] = verts[i].normalize();

    for (int i=0; i<NUM_FACES; i++)
      faces.add( SphereFace( icosahedronFaces[i][0],
                             icosahedronFaces[i][1],
                             icosahedronFaces[i][2] ) );

    for (int i=0; i<numLevels; i++)
      refine();

    //gpu.init( vertShader, fragShader, "in sphere.h" );

    setupVAO();
  };

  ~Sphere() {}

  bool rayInt( vec3 rayStart, vec3 rayDir, int objPartIndex, float maxParam,
	       vec3 &intPoint, vec3 &intNorm, vec3 &intTexCoords, float &intParam, Material * & mat, int &intPartIndex );

  void input( istream &stream );
  void output( ostream &stream ) const;

  vec3 textureColour( vec3 &p, int objPartIndex, float &alpha, vec3 &texCoords );

  void renderGL( GPUProgram *prog, mat4 &WCS_to_VCS, mat4 &VCS_to_CCS, float scale );

  void renderGL( GPUProgram *prog, mat4 &WCS_to_VCS, mat4 &VCS_to_CCS ) {
    renderGL( prog, WCS_to_VCS, VCS_to_CCS, 1 );
  }

 private:

  vec3   centre;
  float  radius;

  seq<vec3>       verts;
  seq<SphereFace> faces;
  GLuint          VAO; 
  
  GPUProgram      gpu;

  void refine();
  void setupVAO();

  static vec3 icosahedronVerts[NUM_VERTS];
  static int icosahedronFaces[NUM_FACES][3];
};

#endif
