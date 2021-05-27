/* object.C
 */


#include "headers.h"

#include <cstring>
#include <cstdlib>
#include "object.h"
#include "main.h"


ostream& operator << ( ostream& stream, Object const& obj )

{
  obj.output( stream );
  stream << "  " << obj.mat->name << endl;
  return stream;
}

istream& operator >> ( istream& stream, Object & obj )

{
  char matName[1000];
  int i;

  obj.input( stream );

  // Get the material name

  skipComments( stream );
  stream >> matName;

  // Find the material

  for (i=0; i<scene->materials.size(); i++)
    if (strcmp( scene->materials[i]->name, matName ) == 0)
      break;

  if (i == scene->materials.size()) {
    cerr << "line " << lineNum << ": Material " << matName << " not found" << endl;
    abort();
  }

  // Store it with the object

  obj.mat = scene->materials[i];

  return stream;
}
