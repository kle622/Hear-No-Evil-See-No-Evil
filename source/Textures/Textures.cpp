#include "Textures.h"

Image *TextureImage;
RGB myimage[64][64];
RGB* g_pixel;
int g_CiboLen = 24;
int g_GiboLen = 6;

Textures::Textures(Mesh* mesh, Handles *handles){
  this->mesh = mesh;
  this->handles = handles;
};
// Gets passed handles object and then draws?
void Textures::drawTexture(int texId) {


  float CubePos[] = {
    -1.5, -1.5, -1.5, //back face 5 verts :0 - clockwise 
    -1.5, 1.5, -1.5,
    1.5, 1.5, -1.5,
    1.5, -1.5, -1.5,
    1.5, -1.5, 1.5, //right face 5 verts :4
    1.5, 1.5, 1.5,
    1.5, 1.5, -1.5,
    1.5, -1.5, -1.5,
    -1.5, -1.5, 1.5, //front face 4 verts :8
    -1.5, 1.5, 1.5,
    1.5, 1.5, 1.5,
    1.5, -1.5, 1.5,
    -1.5, -1.5, -1.5, //left face 4 verts :12
    -1.5, 1.5, -1.5,
    -1.5, 1.5, 1.5,
    -1.5, -1.5, 1.5
  };

  static GLfloat CubeTex[] = { 
    0, 0, // back 
    0, 1,
    1, 1,
    1, 0,
    0, 0, //right 
    0, 1,
    1, 1,
    1, 0,
    0, 0, //front 
    0, 1,
    1, 1,
    1, 0,
    0, 0, // left 
    0, 1,
    1, 1,
    1, 0
  };

  unsigned short idx[] = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11, 8, 12, 13, 14, 14, 15, 12 };

  g_CiboLen = 24;
  glGenBuffers(1, &this->mesh->posBufObj);
  glBindBuffer(GL_ARRAY_BUFFER, this->mesh->posBufObj);
  glBufferData(GL_ARRAY_BUFFER, sizeof(CubePos), CubePos, GL_STATIC_DRAW);

  glGenBuffers(1, &this->mesh->indBufObj);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->mesh->indBufObj);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);

  glGenBuffers(1, &this->mesh->texBufObj);
  glBindBuffer(GL_ARRAY_BUFFER, this->mesh->texBufObj);
  glBufferData(GL_ARRAY_BUFFER, sizeof(CubeTex), CubeTex, GL_STATIC_DRAW);

  //set up the texture unit
  glUniform1i(this->handles->texFlag, 1);

  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);

  glUniform1i(this->handles->h_uTexUnit, texId);
  glUniform1i(this->handles->texFlag, 0);

  glBindTexture(GL_TEXTURE_2D, 0);
  GLSL::enableVertexAttribArray(this->handles->aPosition);
  glBindBuffer(GL_ARRAY_BUFFER, this->mesh->posBufObj);
  glVertexAttribPointer(this->handles->aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

  GLSL::enableVertexAttribArray(this->handles->h_aTexCoord);
  glBindBuffer(GL_ARRAY_BUFFER, this->mesh->texBufObj);
  glVertexAttribPointer(this->handles->h_aTexCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

  // bind ibo
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->mesh->indBufObj);
  //glDrawElements(GL_TRIANGLES, g_CiboLen, GL_UNSIGNED_SHORT, 0);
  glDrawElements(GL_TRIANGLES, g_GiboLen, GL_UNSIGNED_SHORT, 0);
}

//routines to load in a bmp files - must be 2^nx2^m and a 24bit bmp
GLvoid Textures::LoadTexture(char* image_file, int texID) {

  TextureImage = (Image *)malloc(sizeof(Image));
  if (TextureImage == NULL) {
    printf("Error allocating space for image");
    exit(1);
  }
  cout << "trying to load " << image_file << endl;
  if (!ImageLoad(image_file, TextureImage)) {
    exit(1);
  }
  /*  2d texture, level of detail 0 (normal), 3 components (red, green, blue),            */
  /*  x size from image, y size from image,                                              */
  /*  border 0 (normal), rgb color data, unsigned byte data, data  */
  glBindTexture(GL_TEXTURE_2D, texID);
  glTexImage2D(GL_TEXTURE_2D, 0, 3,
    TextureImage->sizeX, TextureImage->sizeY,
    0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage->data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); /*  cheap scaling when image bigger than texture */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); /*  cheap scaling when image smalled than texture*/

}


/* BMP file loader loads a 24-bit bmp file only */

/*
* getint and getshort are help functions to load the bitmap byte by byte
*/
static unsigned int getint(FILE *fp) {
  int c, c1, c2, c3;

  /*  get 4 bytes */
  c = getc(fp);
  c1 = getc(fp);
  c2 = getc(fp);
  c3 = getc(fp);

  return ((unsigned int)c) +
    (((unsigned int)c1) << 8) +
    (((unsigned int)c2) << 16) +
    (((unsigned int)c3) << 24);
}

static unsigned int getshort(FILE *fp){
  int c, c1;

  /* get 2 bytes*/
  c = getc(fp);
  c1 = getc(fp);

  return ((unsigned int)c) + (((unsigned int)c1) << 8);
}

/*  quick and dirty bitmap loader...for 24 bit bitmaps with 1 plane only.  */

int Textures::ImageLoad(char *filename, Image *image) {
  FILE *file;
  unsigned long size;                 /*  size of the image in bytes. */
  unsigned long i;                    /*  standard counter. */
  unsigned short int planes;          /*  number of planes in image (must be 1)  */
  unsigned short int bpp;             /*  number of bits per pixel (must be 24) */
  char temp;                          /*  used to convert bgr to rgb color. */

  /*  make sure the file is there. */
  if ((file = fopen(filename, "rb")) == NULL) {
    printf("File Not Found : %s\n", filename);
    return 0;
  }

  /*  seek through the bmp header, up to the width height: */
  fseek(file, 18, SEEK_CUR);

  /*  No 100% errorchecking anymore!!! */

  /*  read the width */    image->sizeX = getint(file);

  /*  read the height */
  image->sizeY = getint(file);

  /*  calculate the size (assuming 24 bits or 3 bytes per pixel). */
  size = image->sizeX * image->sizeY * 3;

  /*  read the planes */
  planes = getshort(file);
  if (planes != 1) {
    printf("Planes from %s is not 1: %u\n", filename, planes);
    return 0;
  }

  /*  read the bpp */
  bpp = getshort(file);
  if (bpp != 24) {
    printf("Bpp from %s is not 24: %u\n", filename, bpp);
    return 0;
  }

  /*  seek past the rest of the bitmap header. */
  fseek(file, 24, SEEK_CUR);

  /*  read the data.  */
  image->data = (char *)malloc(size);
  if (image->data == NULL) {
    printf("Error allocating memory for color-corrected image data");
    return 0;
  }

  if ((i = fread(image->data, size, 1, file)) != 1) {
    printf("Error reading image data from %s.\n", filename);
    return 0;
  }

  for (i = 0; i<size; i += 3) { /*  reverse all of the colors. (bgr -> rgb) */
    temp = image->data[i];
    image->data[i] = image->data[i + 2];
    image->data[i + 2] = temp;
  }

  fclose(file); /* Close the file and release the filedes */

  /*  we're done. */
  return 1;
}