/* material.C
 */


#include "headers.h"

#include "material.h"
#include "main.h"


void Material::setMaterialForOpenGL( GPUProgram *gpuProg )

{
  gpuProg->setVec3( "kd", kd );
  gpuProg->setVec3( "ks", ks );
  gpuProg->setVec3( "ka", ka );
  gpuProg->setVec3( "Ie", Ie );
  gpuProg->setFloat( "shininess", n );

  if (texture != NULL) {

    // Always use texture unit 0 for the object texture
      
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, texture->textureID );
    gpuProg->setInt( "objTexture", 0 );

    if (texture->hasAlpha) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else
      glDisable(GL_BLEND);
  }

  gpuProg->setInt( "texturing", (texture != NULL ? 1 : 0) );
}



ostream& operator << ( ostream& stream, Material const& mat )

{
  stream << "material" << endl
	 << "  " << mat.name << endl
	 << "  " << mat.ka << endl
	 << "  " << mat.kd << endl
	 << "  " << mat.ks << endl
	 << "  " << mat.n << endl
	 << "  " << mat.g << endl
	 << "  " << mat.Ie << endl
	 << "  " << mat.alpha << endl
	 << "  " << mat.texName << endl;
  
  return stream;
}

istream& operator >> ( istream& stream, Material & mat )

{
  char matName[1000], texName[1000], bumpName[1000];

  skipComments( stream );  stream >> matName;
  skipComments( stream );  stream >> mat.ka;
  skipComments( stream );  stream >> mat.kd;
  skipComments( stream );  stream >> mat.ks;
  skipComments( stream );  stream >> mat.n;
  skipComments( stream );  stream >> mat.g;
  skipComments( stream );  stream >> mat.Ie;
  skipComments( stream );  stream >> mat.alpha;
  skipComments( stream );  stream >> texName;
  skipComments( stream );  stream >> bumpName;

  mat.name = strdup( matName );

  // Check that ks + kd <= 1

  vec3 sum = mat.ks + mat.kd;
  if (sum.x > 1 || sum.y > 1 || sum.z > 1) {
    cerr << "ERROR: Material " << matName << " has kd (" << mat.kd << "), ks (" << mat.ks << "), and 1-alpha (" << 1-mat.alpha << ") that sum to more than one.  Fix this."  << endl;
    exit(1);
  }

  // Store the TEXTURE with the material

  if (texName[0] == '-' && texName[1] == '\0') {

    mat.texture = NULL;		// no texture
    mat.texName = strdup( "-" );

  } else {

    // Search for this texture

    int i;
    for (i=0; i<scene->textures.size(); i++)
      if (strcmp( texName, scene->textures[i]->name ) == 0)
	break;

    // Create the texture if it's not already loaded
    
    if (i == scene->textures.size())
      scene->textures.add( new Texture( texName ) );

    mat.texture = scene->textures[ i ];
    mat.texName = texName;
  }

  // Store the BUMP MAP with the material

  if (bumpName[0] == '-' && bumpName[1] == '\0') {

    mat.bumpMap = NULL;		// no bump map
    mat.bumpMapName = strdup( "-" );

  } else {

    // Search for this texture (bump maps and textures are
    // stored in the same way ... it's only their use that
    // differs).

    int i;
    for (i=0; i<scene->textures.size(); i++)
      if (strcmp( texName, scene->textures[i]->name ) == 0)
	break;

    // Create the texture if it's not already loaded
    
    if (i == scene->textures.size())
      scene->textures.add( new Texture( bumpName ) );

    mat.bumpMap = scene->textures[ i ];
    mat.bumpMapName = bumpName;
  }

  return stream;
}
