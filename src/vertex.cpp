/* vertex.C
 */


#include "headers.h"

#include "vertex.h"
#include "main.h"


ostream& operator << ( ostream& stream, Vertex const& v )

{
  stream << v.position << "/" << v.texCoords.x << " " << v.texCoords.y << "/" << v.normal;
  return stream;
}

istream& operator >> ( istream& stream, Vertex & v )

{
  char c;
  
  // Get position

  skipComments( stream );
  stream >> v.position;

  // Anything else, starting with a '/' ?

  skipComments( stream );
  if (stream.peek() != '/') {
    v.texCoords = vec3(0,0,0);
    v.normal = vec3(0,0,0);
    return stream;
  }

  // Eat the '/'

  stream.get(c);
  skipComments( stream );

  if (stream.peek() == '/')
    v.texCoords = vec3(0,0,0); // no texCoords provided
  else {
    stream >> v.texCoords.x >> v.texCoords.y;

    skipComments( stream );
    if (stream.peek() != '/') {
      v.normal = vec3(0,0,0);
      return stream;
    }
  }

  stream.get(c);
  skipComments( stream );
  stream >> v.normal;
  v.normal = v.normal.normalize();

  return stream;
}

