/* light.C
 */


#include "headers.h"

#include "light.h"
#include "main.h"


#define LIGHT_RADIUS 0.1


// Draw a light

void Light::draw( GPUProgram *prog, mat4 &WCS_to_VCS, mat4 &VCS_to_CCS, vec3 lightDir )

{
  if (sphere == NULL) {
    sphere = new Sphere();
    sphere->mat->kd = colour;
  }

  mat4 MV = WCS_to_VCS * translate( position );

  sphere->renderGL( prog, MV, VCS_to_CCS, LIGHT_RADIUS * scene->sceneScale );
}



// Output a light

ostream& operator << ( ostream& stream, Light const& obj )

{
  stream << "light" << endl
	 << "  " << obj.position << endl
	 << "  " << obj.colour << endl;
  return stream;
}


// Input a light

istream& operator >> ( istream& stream, Light & obj )

{
  skipComments( stream );  stream >> obj.position;
  skipComments( stream );  stream >> obj.colour;
  return stream;
}

