/* eye.C
 */


#include "headers.h"
#include "eye.h"
#include "main.h"


// Output a eye

ostream& operator << ( ostream& stream, Eye const& obj )

{
  stream << "eye" << endl
	 << "  " << obj.position << endl
	 << "  " << obj.lookAt << endl
	 << "  " << obj.upDir << endl
	 << "  " << obj.fovy << endl;

  return stream;
}


// Input a eye

istream& operator >> ( istream& stream, Eye & obj )

{
  skipComments( stream );  stream >> obj.position;
  skipComments( stream );  stream >> obj.lookAt;
  skipComments( stream );  stream >> obj.upDir;
  skipComments( stream );  stream >> obj.fovy;

  return stream;
}

