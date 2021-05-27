/* wavefrontobj.cpp
 */


#include "headers.h"
#include "wavefrontobj.h"
#include "material.h"
#include "bvh.h"


// Convert Wavefront object to a list of materials and triangles for the BVH.

void WavefrontObj::copyWavefrontToBVH( BVH &bvh )

{
  bvh.obj       = obj;
  bvh.vertices  = &obj->vertices;
  bvh.texcoords = &obj->texcoords;
  bvh.normals   = &obj->normals;
  bvh.facetnorms= &obj->facetnorms;

  // Each group in the wavefront object
  
  for (int groupID=0; groupID<obj->groups.size(); groupID++) {

    // Get this group's material
    
    wfMaterial *fromMat = obj->groups[groupID]->material;
    Material *toMat = new Material();

    toMat->name = fromMat->name;
    toMat->ka = fromMat->ambient;
    toMat->kd = fromMat->diffuse;
    toMat->ks = fromMat->specular;
    toMat->n  = fromMat->shininess;
    toMat->Ie = fromMat->emissive;
    toMat->alpha = fromMat->alpha;

    if (fromMat->texmap != NULL) {
      Texture *tex = new Texture(); // not used for OpenGL ... just for raytracing lookups
      tex->texmap    = fromMat->texmap;
      tex->width     = fromMat->width;
      tex->height    = fromMat->height;
      tex->hasAlpha  = fromMat->hasAlpha;
      toMat->texture = tex;
    }

    // Not provided in wfMaterial:

    toMat->texName     = "";
    toMat->bumpMapName = "";
    toMat->g           = 1;
    toMat->alpha       = 1;

    // Add to Materials

    bvh.materials.add( toMat );

    // Add the triangles of this group

    for (int j=0; j<obj->groups[groupID]->triangles.size(); j++) {
      wfTriangle *tri = obj->groups[groupID]->triangles[j];
      bvh.triangles.add( BVH_triangle( tri->vindices[0], tri->vindices[1], tri->vindices[2], // indices into vertices[] 
				       tri->tindices[0], tri->tindices[1], tri->tindices[2], // indices into texcoords[]
				       tri->nindices[0], tri->nindices[1], tri->nindices[2], // indices into normals[]
				       bvh.materials.size()-1,                               // index into mats[]
				       tri->findex ) );                                      // index into facetnorms[]
    }    
  }
}
