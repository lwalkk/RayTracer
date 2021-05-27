/* eye.h
 */


#ifndef EYE_H
#define EYE_H


#include "linalg.h"


class Eye {

 public:

  vec3 position;
  vec3 lookAt;
  vec3 upDir;
  float  fovy;

  Eye() {}

  friend ostream& operator << ( ostream& stream, Eye const& obj );
  friend istream& operator >> ( istream& stream, Eye & obj );
};

#endif
