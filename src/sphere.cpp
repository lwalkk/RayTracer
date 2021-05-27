// sphere.cpp


#include "sphere.h"
#include "main.h"


// icosahedron vertices (taken from Jon Leech http://www.cs.unc.edu/~jon)

vec3 Sphere::icosahedronVerts[NUM_VERTS] = {
  vec3(  tau,  one,    0 ),
  vec3( -tau,  one,    0 ),
  vec3( -tau, -one,    0 ),
  vec3(  tau, -one,    0 ),
  vec3(  one,   0 ,  tau ),
  vec3(  one,   0 , -tau ),
  vec3( -one,   0 , -tau ),
  vec3( -one,   0 ,  tau ),
  vec3(   0 ,  tau,  one ),
  vec3(   0 , -tau,  one ),
  vec3(   0 , -tau, -one ),
  vec3(   0 ,  tau, -one )
};


// icosahedron faces (taken from Jon Leech http://www.cs.unc.edu/~jon)

int Sphere::icosahedronFaces[NUM_FACES][3] = {
  { 4, 8, 7 },
  { 4, 7, 9 },
  { 5, 6, 11 },
  { 5, 10, 6 },
  { 0, 4, 3 },
  { 0, 3, 5 },
  { 2, 7, 1 },
  { 2, 1, 6 },
  { 8, 0, 11 },
  { 8, 11, 1 },
  { 9, 10, 3 },
  { 9, 2, 10 },
  { 8, 4, 0 },
  { 11, 0, 5 },
  { 4, 9, 3 },
  { 5, 3, 10 },
  { 7, 8, 1 },
  { 6, 1, 11 },
  { 7, 2, 9 },
  { 6, 10, 2 },
};


// Add a level to the sphere

void Sphere::refine()

{
  int n = faces.size();

  for (int i=0; i<n; i++) {

    SphereFace f = faces[i];

    verts.add( (verts[ f.v[0] ] + verts[ f.v[1] ]).normalize() );
    verts.add( (verts[ f.v[1] ] + verts[ f.v[2] ]).normalize() );
    verts.add( (verts[ f.v[2] ] + verts[ f.v[0] ]).normalize() );

    int v01 = verts.size() - 3;
    int v12 = verts.size() - 2;
    int v20 = verts.size() - 1;

    faces.add( SphereFace( f.v[0], v01, v20 ) );
    faces.add( SphereFace( f.v[1], v12, v01 ) );
    faces.add( SphereFace( f.v[2], v20, v12 ) );

    faces[i].v[0] = v01;
    faces[i].v[1] = v12;
    faces[i].v[2] = v20;
  }
}


void Sphere::setupVAO()

{
  // Set up buffers of vertices, normals, and face indices.  These are
  // collected from the sphere's 'seq<vec3> verts' and
  // 'seq<SphereFace> faces' structures.

  // copy vertices

  int nVerts = verts.size();

  GLfloat *vertexBuffer = new GLfloat[ nVerts * 3 ];

  for (int i=0; i<nVerts; i++)
    ((vec3 *) vertexBuffer)[i] = verts[i];

  // copy normals
  //
  // Since the vertices are on a sphere centred at the origin, and are
  // distance 1 from the origin, the normals are the same as the
  // vertices.

  GLfloat *normalBuffer = new GLfloat[ nVerts * 3 ];

  for (int i=0; i<nVerts; i++)
    ((vec3 *) normalBuffer)[i] = verts[i];

  // Build texture coordinates

  GLfloat *texCoordBuffer = new GLfloat[ nVerts * 3 ];

  for (int i=0; i<nVerts; i++) {
    float theta = asin(verts[i].z) / 3.14159 + 0.5;
    float phi = atan2( verts[i].y, verts[i].x ) / (2*3.14159);
    if (phi < 0) phi++;
    ((vec3 *) texCoordBuffer)[i] = vec3( phi, theta, 0 );
  }

  // copy faces

  int nFaces = faces.size();

  GLuint *indexBuffer = new GLuint[ nFaces * 3 ];

  for (int i=0; i<faces.size(); i++)
    for (int j=0; j<3; j++) 
      indexBuffer[3*i+j] = faces[i].v[j]; 

  // Create a VAO

  glGenVertexArrays( 1, &VAO );
  glBindVertexArray( VAO );

  // store vertices (i.e. one triple of floats per vertex)

  GLuint vertexBufferID;
  glGenBuffers( 1, &vertexBufferID );
  glBindBuffer( GL_ARRAY_BUFFER, vertexBufferID );

  glBufferData( GL_ARRAY_BUFFER, nVerts * 3 * sizeof(GLfloat), vertexBuffer, GL_STATIC_DRAW );

  // attribute 0 = position

  glEnableVertexAttribArray( 0 );
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );

  // store vertex normals (i.e. one triple of floats per vertex)

  GLuint normalBufferID;
  glGenBuffers( 1, &normalBufferID );
  glBindBuffer( GL_ARRAY_BUFFER, normalBufferID );

  glBufferData( GL_ARRAY_BUFFER, nVerts * 3 * sizeof(GLfloat), normalBuffer, GL_STATIC_DRAW );

  // attribute 1 = normal

  glEnableVertexAttribArray( 1 );
  glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, 0 );

  // store vertex texture coordiantes

  GLuint texCoordBufferID;
  glGenBuffers( 1, &texCoordBufferID );
  glBindBuffer( GL_ARRAY_BUFFER, texCoordBufferID );

  glBufferData( GL_ARRAY_BUFFER, nVerts * 3 * sizeof(GLfloat), texCoordBuffer, GL_STATIC_DRAW );

  // attribute 2 = texture coordinates

  glEnableVertexAttribArray( 2 );
  glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, 0, 0 );

  // store faces (i.e. one triple of vertex indices per face)

  GLuint indexBufferID;
  glGenBuffers( 1, &indexBufferID );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBufferID );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, nFaces * 3 * sizeof(GLuint), indexBuffer, GL_STATIC_DRAW );

  // Clean up

  delete[] vertexBuffer;
  delete[] normalBuffer;
  delete[] indexBuffer;
}


// Ray / sphere intersection

bool Sphere::rayInt( vec3 rayStart, vec3 rayDir, int objPartIndex, float maxParam,
		     vec3 &intPoint, vec3 &intNorm, vec3 &intTexCoords, float &intParam, Material * &mat, int &intPartIndex )

{
  float a,b,c,d,t0,t1;

  // Does it intersect? ... Solve a quadratic for
  // the parameter at the point of intersection

  a = rayDir * rayDir;
  b = 2 * (rayDir * (rayStart - centre));
  c = (rayStart - centre) * (rayStart - centre) - radius * radius;

  d = b*b - 4*a*c;

  if (d < 0)
    return false;

  // Find the parameter at the point of intersection

  d = sqrt(d);
  t0 = (-b + d) / (2*a);
  t1 = (-b - d) / (2*a);

  intParam = (t0 < t1 ? t0 : t1);

  if (intParam > maxParam)
    return false; // too far away

  // Compute the point of intersection

  intPoint = rayStart + intParam * rayDir;

  // Compute the normal at the intersection point

  intNorm = (intPoint - centre).normalize();

  mat = this->mat;

  return true;
}


// Output a sphere

void Sphere::output( ostream &stream ) const

{
  stream << "sphere" << endl
	 << "  " << radius << endl
	 << "  " << centre << endl;
}


// Input a sphere

void Sphere::input( istream &stream )

{
  skipComments( stream );  stream >> radius;
  skipComments( stream );  stream >> centre;
}


// Render with openGL

#define PI          3.1415926
#define PHI_STEP    PI/8.0
#define THETA_STEP  PI/8.0

void Sphere::renderGL( GPUProgram *prog, mat4 &WCS_to_VCS, mat4 &VCS_to_CCS, float s )

{
  mat->setMaterialForOpenGL( prog );

  mat4 MV  = WCS_to_VCS * translate( centre ) * scale( s, s, s );
  mat4 MVP = VCS_to_CCS * MV;

  prog->setMat4( "MV",  MV );
  prog->setMat4( "MVP", MVP );

  glBindVertexArray( VAO );
  glDrawElements( GL_TRIANGLES, faces.size()*3, GL_UNSIGNED_INT, 0 );
  glBindVertexArray( 0 );
}


vec3 Sphere::textureColour( vec3 &p, int objPartIndex, float &alpha, vec3 &texCoords )

{
  // No texture map?

  if (mat->texture == NULL) {
    alpha = 1;
    return vec3(1,1,1);
  }

  // Texture map:

  vec3 dir = (p - centre).normalize();

  float theta = asin(dir.z) / PI + 0.5;
  float phi = atan2( dir.y, dir.x ) / (2*PI);
  if (phi < 0) phi++;

  return mat->texture->texel( phi, theta, alpha );
}
