/* bbox.h
 *
 * An axis-aligned bounding box
 */

#ifndef BBOX_H
#define BBOX_H


#include "linalg.h"


class BBox {

 public:

  vec3 min;			// min corner
  vec3 max;			// max corner

  BBox() {}

  BBox( vec3 c0, vec3 c1 ) {
    min = c0;
    max = c1;
  }

  void renderGL( mat4 &WCS_to_VCS, mat4 &WCS_to_CCS, vec3 lightDir );
};


#endif
