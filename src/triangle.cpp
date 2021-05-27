// triangle.cpp


#include "headers.h"
#include "triangle.h"
#include "main.h"
#include "texture.h"


// Compute plane/ray intersection, and then the local coordinates to
// see whether the intersection point is inside.

bool Triangle::rayInt( vec3 rayStart, vec3 rayDir, int objPartIndex, float maxParam,
		       vec3 &intPoint, vec3 &intNorm, vec3 &intTexCoords, float &intParam, Material * &mat, int &intPartIndex )

{
  float t;

  // Compute ray/plane intersection

  float dn = rayDir * faceNormal;

  if (fabs(dn) < 0.0001) 	// *** CHANGED TO ALLOW INTERSECTION FROM BEHIND TRIANGLE ***
    return false;		// ray is parallel to plane

  t = (dist - rayStart*faceNormal) / dn;
  if (t < 0)
    return false; // plane is behind starting point

  if (t > maxParam)
    return false; // too far away

  vec3 point = rayStart + t * rayDir;

  // Compute barycentric coords
  //
  // See notes at http://watkins.cs.queensu.ca/~jstewart/454/notes/32/32-barycentric.html
  //
  // There's some redundant code below, but it will likely be
  // optimized out by the compiler.  It's left this way to show the
  // correspondence to the notes above.

  vec3 a = verts[1].position - verts[0].position;
  vec3 b = verts[2].position - verts[0].position;
  vec3 x = point - verts[0].position;

  float p = a*a;
  float q = a*b;
  float r = a*b;
  float s = b*b;

  float factor = 1/(p*s - q*r);

  float alpha = factor * ( s*(a*x) - q*(b*x));
  float beta  = factor * (-r*(a*x) + p*(b*x));
  float gamma = 1 - alpha - beta;

  // Check that point is inside triangle
  
  if (alpha < 0 || beta < 0 || gamma < 0)
    return false;

  // Gather information to return
  
  intParam = t;
  intPoint = point;
  mat      = this->mat;

  // Find the normal with bump mapping

  if (mat->bumpMap != NULL) {
    intNorm = faceNormal; // NOT YET IMPLEMENTED!
    return true;
  }

  // No bump mapping: Find the normal as interpolated

  if (verts[0].normal.x == 0 && verts[0].normal.y == 0 && verts[0].normal.z == 0)
    intNorm = faceNormal;
  else {
    intNorm = gamma * verts[0].normal + alpha * verts[1].normal + beta * verts[2].normal;
    intNorm = intNorm.normalize();
  }

  // Texture coordinates at int point.  This assumes that the texture
  // coordinates at v0,v1,v2 are (0,0), (1,0), (0,1).

  intTexCoords = gamma * verts[0].texCoords + alpha * verts[1].texCoords + beta * verts[2].texCoords;

  return true;
}


// Determine the texture colour at a point


vec3 Triangle::textureColour( vec3 &p, int objPartIndex, float &alpha, vec3 &texCoords )

{
  // No texture map?

  if (mat->texture == NULL) {
    alpha = 1;
    return vec3(1,1,1);
  }

  return mat->texture->texel( texCoords.x, texCoords.y, alpha );
}


// Output a triangle

void Triangle::output( ostream &stream ) const

{
  stream << "triangle" << endl
	 << "  " << verts[0] << endl
	 << "  " << verts[1] << endl
	 << "  " << verts[2] << endl;
}


// Input a triangle

void Triangle::input( istream &stream )

{
  skipComments( stream );  stream >> verts[0];
  skipComments( stream );  stream >> verts[1];
  skipComments( stream );  stream >> verts[2];

  // Compute the triangle normal and the barycentric factor which is
  // used in computing the barycentric coordinates of a point.

  faceNormal = (verts[1].position - verts[0].position) ^ (verts[2].position - verts[0].position);
  barycentricFactor = 1.0/faceNormal.length();
  faceNormal = barycentricFactor * faceNormal;
  dist   = verts[0].position * faceNormal;
}


// Render with openGL

void Triangle::renderGL( GPUProgram *gpuProg, mat4 &WCS_to_VCS, mat4 &VCS_to_CCS )

{
  if (VAO == 0) { // create VBO
    
    vec3 attribs[3*3];

    for (int i=0; i<3; i++) {
      attribs[i]   = verts[i].position;  // position
      attribs[i+3] = faceNormal;	 // normal
      attribs[i+6] = verts[i].texCoords; // texcoords (3D !!!)
    }

    glGenVertexArrays( 1, &VAO );
    glBindVertexArray( VAO );

    GLuint VBO;
    glGenBuffers( 1, &VBO );
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof(attribs), attribs, GL_STATIC_DRAW );

    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray( 0 );
    
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (void*) (sizeof(vec3)*3) );
    glEnableVertexAttribArray( 1 );

    glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, 0, (void*) (sizeof(vec3)*6) );
    glEnableVertexAttribArray( 2 );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray( 0 );
  }

  // Draw

  mat->setMaterialForOpenGL( gpuProg );

  gpuProg->setMat4( "MV",  WCS_to_VCS );

  mat4 MVP = VCS_to_CCS * WCS_to_VCS;
  gpuProg->setMat4( "MVP", MVP );

  glBindVertexArray( VAO );
  glDrawArrays( GL_TRIANGLES, 0, 3 );
  glBindVertexArray( 0 );
}
