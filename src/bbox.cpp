/* bbox.C
 */


#include "headers.h"
#include "bbox.h"
#include "drawSegs.h"
#include "main.h"


void BBox::renderGL( mat4 &WCS_to_VCS, mat4 &WCS_to_CCS, vec3 lightDir ) 

{
  vec3 pts[24] = {
    vec3(min.x, min.y, min.z),
    vec3(min.x, min.y, max.z),
    vec3(min.x, max.y, min.z),
    vec3(min.x, max.y, max.z),
    vec3(max.x, min.y, min.z),
    vec3(max.x, min.y, max.z),
    vec3(max.x, max.y, min.z),
    vec3(max.x, max.y, max.z),
    vec3(min.x, min.y, min.z),
    vec3(max.x, min.y, min.z),
    vec3(min.x, min.y, max.z),
    vec3(max.x, min.y, max.z),
    vec3(min.x, max.y, min.z),
    vec3(max.x, max.y, min.z),
    vec3(min.x, max.y, max.z),
    vec3(max.x, max.y, max.z),
    vec3(min.x, min.y, min.z),
    vec3(min.x, max.y, min.z),
    vec3(min.x, min.y, max.z),
    vec3(min.x, max.y, max.z),
    vec3(max.x, min.y, min.z),
    vec3(max.x, max.y, min.z),
    vec3(max.x, min.y, max.z),
    vec3(max.x, max.y, max.z)
  };

  vec3 colours[24];
  for (int i=0; i<24; i++)
    colours[i] = vec3(0.5,0.5,0.5);

  scene->segs->drawSegs( GL_LINES, pts, colours, 24, WCS_to_VCS, WCS_to_CCS, lightDir );
}
