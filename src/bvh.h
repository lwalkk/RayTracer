// bvh.h
//
// Bounding volume hierarchy for triangles


#ifndef BVH_H
#define BVH_H

#include "linalg.h"
#include "seq.h"
#include "material.h"
#include "bbox.h"
#include "main.h"
#include "wavefront.h"


class BVH_triangle {

public:
  unsigned int v0, v1, v2;      // indices into vertices list
  unsigned int t0, t1, t2;      // indices into texcoords list
  unsigned int n0, n1, n2;      // indices into normal list
  unsigned int materialID;      // index into material list
  unsigned int faceID;          // index into facetnorms[]

 BVH_triangle() {}

 BVH_triangle( unsigned int _v0, unsigned int _v1, unsigned int _v2, 
	       unsigned int _t0, unsigned int _t1, unsigned int _t2, 
	       unsigned int _n0, unsigned int _n1, unsigned int _n2, 
	       unsigned int _materialID, unsigned int _faceID )
  {
    v0 = _v0; v1 = _v1; v2 = _v2;
    t0 = _t0; t1 = _t1; t2 = _t2;
    n0 = _n0; n1 = _n1; n2 = _n2;
    materialID = _materialID;
    faceID = _faceID;
  }
};



class BVH_node {

public:

  BBox bbox;		           // node's bounding box
  bool isLeaf;                     // true iff this is a leaf in the BVH
  union {
    seq<BVH_node*> *children;	   // present only for non-leaves
    seq<int>       *triangles;     // present only for leaves and contains INDICES of leaf triangles
  };
};



class BVH {

  bool rayBoxInt( vec3 &rayStart, vec3 &rayDir, float tmin, float tmax, BBox &bbox );

  void freeTree( BVH_node *n ) {
    if (!n->isLeaf)
      for (int i=0; i<n->children->size(); i++)
	freeTree( (*n->children)[i] );
    delete n;
  }

  BVH_node *buildSubtree( seq<int> &triangleIndices, int depth );
  BVH_node *makeLeafNode( seq<int> &triangleIndices );

  BBox triangleBBox( int triIndex );
  BBox trianglesBBox( seq<int> &triangleIndices );

  float boxBoxDistance( BBox &b1, BBox &b2 );

public:

  wfModel   *obj;
  seq<vec3> *vertices;
  seq<vec3> *texcoords;
  seq<vec3> *normals;
  seq<vec3> *facetnorms;
  seq<Material*> materials;
  seq<BVH_triangle> triangles;

  BVH_node *root;

  BVH() {
    root = NULL;
  }

  ~BVH() {
    if (root != NULL)
      freeTree( root );
    // Note that vertices, texcoords, and materials are stored
    // elsewhere and should not be deleted here.
  }

  void buildTree() {
    // cout << "Building with " << vertices->size() << " vertices, " << texcoords->size() << " texcoords, " << materials.size() << " materials, " << triangles.size() << " triangles." << endl;
    if (triangles.size() == 0)
      root = NULL;
    else {
      // Create a sequence of all triangle indices
      seq<int> triangleIndices( triangles.size() );
      for (int i=0; i<triangles.size(); i++)
	triangleIndices.add( i );
      // Build the tree
      root = buildSubtree( triangleIndices, 0 );
    }
  };
  
  bool rayInt( vec3 rayStart, vec3 rayDir, int sourceTriangleIndex, float maxParam, vec3 &intPoint, vec3 &intNormal, vec3 &intTexCoords, float &intParam, Material * &mat, int &intTriangleIndex ) {
    if (root == NULL)
      return false;
    return rayIntBVH( root, rayStart, rayDir, sourceTriangleIndex, maxParam, intPoint, intNormal, intTexCoords, intParam, mat, intTriangleIndex );
  }

  void renderGL( mat4 &WCS_to_VCS, mat4 &WCS_to_CCS, vec3 lightDir ) {
    if (root != NULL)
      renderSubtreeGL( root, WCS_to_VCS, WCS_to_CCS, lightDir, scene->bvhDisplayDepth );
  }

  // Determine the texture colour at a point

  vec3 textureColour( vec3 &p, int triangleIndex, float &alpha, vec3 &texCoords ) {
    if (!obj->hasVertexTexCoords) { // no texture coordinates
      alpha = 1;
      return vec3(1,1,1);
    } else
      return materials[ triangles[triangleIndex].materialID ]->texture->texel( texCoords.x, texCoords.y, alpha );
  }

  bool rayIntBVH( BVH_node *n, vec3 rayStart, vec3 rayDir, int sourceTriangleIndex, float maxParam, vec3 & intPoint, vec3 & intNormal, vec3 &intTexCoords, float & intParam, Material * &mat, int &intTriangleIndex );

  void renderSubtreeGL( BVH_node *root, mat4 &WCS_to_VCS, mat4 &WCS_to_CCS, vec3 lightDir, int levelsRemaining );

  bool triangleInt( vec3 &rayStart, vec3 &rayDir, int triangleIndex, float maxParam, float &param, vec3 &point, vec3 &normal, vec3 &texcoords, float &alpha, float &beta, float &gamma );

};


#endif
