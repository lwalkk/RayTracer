/* vertex.h
 */


#ifndef VERTEX_H
#define VERTEX_H


#include <iostream>
#include "linalg.h"


class Vertex {
 public:
  vec3 position;		// vertex position
  vec3 normal;			// vertex normal
  vec3 texCoords;		// vertex texture coordinates

  friend ostream& operator << ( ostream& stream, Vertex const& v );
  friend istream& operator >> ( istream& stream, Vertex & v );
};


#endif
