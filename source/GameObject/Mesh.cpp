#include "Mesh.h"
#define DEBUG

void Mesh::loadShapes(const std::string &objFile) {
  std::string err = tinyobj::LoadObj(this->shapes, this->materials, objFile.c_str());
  if(!err.empty()) {
    std::cerr << err << std::endl;
  }
  //printf("texCoords size: %d\n", this->shapes[0].mesh.texcoords.size());
  this->resize_obj();
  this->sendBufs();
  //this->computeBound();
}

void Mesh::loadTexture(const std::string &filename) {
  glGenTextures(1, &texId);
  glBindTexture(GL_TEXTURE_2D, texId);
  bmp = imageLoad(filename.c_str());
  glTexImage2D(GL_TEXTURE_2D, 0, 3, TEX_SIZE, TEX_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, bmp);
 
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

/*void Mesh::loadMipmapTexture(const std::string &filename) {
  bmp = imageLoad(filename.c_str());
  glGenTextures(1, &texId);
  glBindTexture(GL_TEXTURE_2D, texId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, MIPMAP_SIZE, MIPMAP_SIZE, 0, GL_BGRA, GL_UNSIGNED_BYTE, bmp);
  glGenerateMipmap(GL_TEXTURE_2D);
  printf("after glgeneratemipmap call \n");

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  printf("after glTexParameter calls\n");
}*/

void Mesh::loadClueMipmapTexture(const std::string &filename, int size) {
    bmp = imageLoad(filename.c_str());
    //printf("loaded file in mip map texture function\n");
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    //printf("before gltexImage2D call\n");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, bmp);
    // printf("after gltexImage2D call\n");
      //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, TEX_SIZE, TEX_SIZE, 0, GL_BGRA, GL_UNSIGNED_BYTE, bmp);
    glGenerateMipmap(GL_TEXTURE_2D);
    // printf("after glgeneratemipmap call\n");
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

}

void Mesh::loadMipmapTexture(const std::string &filename, int size) {
    bmp = imageLoad(filename.c_str());
    //printf("loaded file in mip map texture function\n");
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    //printf("before gltexImage2D call\n");
    glTexImage2D(GL_TEXTURE_2D, 0, 3, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, bmp);
    // printf("after gltexImage2D call\n");
      //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, TEX_SIZE, TEX_SIZE, 0, GL_BGRA, GL_UNSIGNED_BYTE, bmp);
    glGenerateMipmap(GL_TEXTURE_2D);
    // printf("after glgeneratemipmap call\n");
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //printf("after glTexParameter calls\n");
    
}

void Mesh::computeBound() {
  glm::vec3 result = glm::vec3(0, 0, 0);
  float minX, minY, minZ;
  float maxX, maxY, maxZ;

  minX = minY = minZ = 1.1754E+38F;
  maxX = maxY = maxZ = -1.1754E+38F;

  for (size_t s = 0; s < this->shapes.size(); ++s) {
    for (size_t v = 0; v < this->shapes[s].mesh.positions.size() / 3; v++) {
      if(this->shapes[s].mesh.positions[3*v+0] < minX) minX = this->shapes[s].mesh.positions[3*v+0];
      if(this->shapes[s].mesh.positions[3*v+0] > maxX) maxX = this->shapes[s].mesh.positions[3*v+0];

      if(this->shapes[s].mesh.positions[3*v+1] < minY) minY = this->shapes[s].mesh.positions[3*v+1];
      if(this->shapes[s].mesh.positions[3*v+1] > maxY) maxY = this->shapes[s].mesh.positions[3*v+1];

      if(this->shapes[s].mesh.positions[3*v+2] < minZ) minZ = this->shapes[s].mesh.positions[3*v+2];
      if(this->shapes[s].mesh.positions[3*v+2] > maxZ) maxZ = this->shapes[s].mesh.positions[3*v+2];
    }
  }

  this->center.x = (minX + maxX) / 2.0;
  this->center.y = (minY + maxY) / 2.0;
  this->center.z = (minZ + maxZ) / 2.0;
  this->radius = glm::distance(glm::vec3(minX, minY, minZ), glm::vec3(maxX, maxY, maxZ));

#ifdef DEBUG
  //printf("Center: %f %f %f; Radius: %f\n", this->center.x, this->center.y, this->center.z, this->radius);
#endif
}

void Mesh::resize_obj() {
  float minX, minY, minZ;
  float maxX, maxY, maxZ;
  float scaleX, scaleY, scaleZ;
  float shiftX, shiftY, shiftZ;
  float epsilon = 0.001;

  minX = minY = minZ = 1.1754E+38F;
  maxX = maxY = maxZ = -1.1754E+38F;

  //Go through all vertices to determine min and max of each dimension
  for (size_t i = 0; i < this->shapes.size(); i++) {
    for (size_t v = 0; v < this->shapes[i].mesh.positions.size() / 3; v++) {
      if(this->shapes[i].mesh.positions[3*v+0] < minX) minX = this->shapes[i].mesh.positions[3*v+0];
      if(this->shapes[i].mesh.positions[3*v+0] > maxX) maxX = this->shapes[i].mesh.positions[3*v+0];

      if(this->shapes[i].mesh.positions[3*v+1] < minY) minY = this->shapes[i].mesh.positions[3*v+1];
      if(this->shapes[i].mesh.positions[3*v+1] > maxY) maxY = this->shapes[i].mesh.positions[3*v+1];

      if(this->shapes[i].mesh.positions[3*v+2] < minZ) minZ = this->shapes[i].mesh.positions[3*v+2];
      if(this->shapes[i].mesh.positions[3*v+2] > maxZ) maxZ = this->shapes[i].mesh.positions[3*v+2];
    }
  }
  //From min and max compute necessary scale and shift for each dimension
  float maxExtent, xExtent, yExtent, zExtent;
  xExtent = maxX-minX;
  yExtent = maxY-minY;
  zExtent = maxZ-minZ;
  if (xExtent >= yExtent && xExtent >= zExtent) {
    maxExtent = xExtent;
  }
  if (yExtent >= xExtent && yExtent >= zExtent) {
    maxExtent = yExtent;
  }
  if (zExtent >= xExtent && zExtent >= yExtent) {
    maxExtent = zExtent;
  }
  scaleX = 2.0 / maxExtent;
  shiftX = minX + (xExtent/ 2.0);
  scaleY = 2.0 / maxExtent;
  shiftY = minY + (yExtent / 2.0);
  scaleZ = 2.0/ maxExtent;
  shiftZ = minZ + (zExtent)/2.0;

  minX -= shiftX;
  minX *= scaleX;
  minY -= shiftY;
  minY *= scaleY;
  minZ -= shiftZ;
  minZ *= scaleZ;
  maxX -= shiftX;
  maxX *= scaleX;
  maxY -= shiftY;
  maxY *= scaleY;
  maxZ -= shiftZ;
  maxZ *= scaleZ;

  // set box dimensions/center according to max x, y, and z
  this->center = glm::vec3((maxX + minX) * 0.5, (maxY + minY) * 0.5, (maxZ + minZ) * 0.5);
  this->dimensions = glm::vec3(maxX - minX, maxY - minY, maxZ - minZ);

#ifdef DEBUG
  /*std::cout << "min x: " << minX << std::endl;
  std::cout << "max x: " << maxX << std::endl;
  std::cout << "min y: " << minY << std::endl;
  std::cout << "max y: " << maxY << std::endl;
  std::cout << "min z: " << minZ << std::endl;
  std::cout << "max z: " << maxZ << std::endl;
  std::cout << "dimensions: <" << dimensions.x << ", " << dimensions.y << ", " << dimensions.z << ">" << std::endl;
  std::cout << "center: <" << center.x << ", " << center.y << ", " << center.z << ">" << std::endl;*/
#endif

  //Go through all verticies shift and scale them
  for (size_t i = 0; i < this->shapes.size(); i++) {
    for (size_t v = 0; v < this->shapes[i].mesh.positions.size() / 3; v++) {
      this->shapes[i].mesh.positions[3*v+0] = (this->shapes[i].mesh.positions[3*v+0] - shiftX) * scaleX;
      assert(this->shapes[i].mesh.positions[3*v+0] >= -1.0 - epsilon);
      assert(this->shapes[i].mesh.positions[3*v+0] <= 1.0 + epsilon);
      this->shapes[i].mesh.positions[3*v+1] = (this->shapes[i].mesh.positions[3*v+1] - shiftY) * scaleY;
      assert(this->shapes[i].mesh.positions[3*v+1] >= -1.0 - epsilon);
      assert(this->shapes[i].mesh.positions[3*v+1] <= 1.0 + epsilon);
      this->shapes[i].mesh.positions[3*v+2] = (this->shapes[i].mesh.positions[3*v+2] - shiftZ) * scaleZ;
      assert(this->shapes[i].mesh.positions[3*v+2] >= -1.0 - epsilon);
      assert(this->shapes[i].mesh.positions[3*v+2] <= 1.0 + epsilon);
    }
  }
}

void Mesh::sendWallTexBuf() {
  for (int s = 0; s < this->shapes.size(); ++s) {
    for (int i = 0; i < this->shapes[s].mesh.texcoords.size(); i++) {
      this->shapes[s].mesh.texcoords[i] *= 10;
    }
    const std::vector<float> &texBuf = this->shapes[s].mesh.texcoords;
    glGenBuffers(1, &this->texBufObj);
    glBindBuffer(GL_ARRAY_BUFFER, this->texBufObj);
    glBufferData(GL_ARRAY_BUFFER, texBuf.size() * sizeof(float), &texBuf[0], GL_STATIC_DRAW);
  }
}

void Mesh::sendBufs() {
  for (int s = 0; s < this->shapes.size(); ++s) {
    // Send the position array to the GPU
    const std::vector<float> &posBuf = this->shapes[s].mesh.positions;
    glGenBuffers(1, &this->posBufObj);
    glBindBuffer(GL_ARRAY_BUFFER, this->posBufObj);
    glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW);

	// Send the normal array to the GPU
	const std::vector<float> &norBuf = this->shapes[s].mesh.normals;
	glGenBuffers(1, &this->norBufObj);
	glBindBuffer(GL_ARRAY_BUFFER, this->norBufObj);
	glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);

	//send the texture array to the GPU
	const std::vector<float> &texBuf = this->shapes[s].mesh.texcoords;
	glGenBuffers(1, &this->texBufObj);
	glBindBuffer(GL_ARRAY_BUFFER, this->texBufObj);
	glBufferData(GL_ARRAY_BUFFER, texBuf.size() * sizeof(float), &texBuf[0], GL_STATIC_DRAW);
	/**
    // compute the normals per vertex
    int idx1, idx2, idx3;
    glm::vec3 v1, v2, v3;
    //for every vertex initialize a normal to 0
    for (int j = 0; j < this->shapes[s].mesh.positions.size()/3; j++) {
      this->norBuf.push_back(0);
      this->norBuf.push_back(0);
      this->norBuf.push_back(0);
    }
    // compute the normals for every face
    //then add its normal to its associated vertex
    for (int i = 0; i < this->shapes[s].mesh.indices.size()/3; i++) {
      idx1 = this->shapes[s].mesh.indices[3*i+0];
      idx2 = this->shapes[s].mesh.indices[3*i+1];
      idx3 = this->shapes[s].mesh.indices[3*i+2];
      v1 = glm::vec3(this->shapes[s].mesh.positions[3*idx1 +0],this->shapes[s].mesh.positions[3*idx1 +1], this->shapes[s].mesh.positions[3*idx1 +2]); 
      v2 = glm::vec3(this->shapes[s].mesh.positions[3*idx2 +0],this->shapes[s].mesh.positions[3*idx2 +1], this->shapes[s].mesh.positions[3*idx2 +2]); 
      v3 = glm::vec3(this->shapes[s].mesh.positions[3*idx3 +0],this->shapes[s].mesh.positions[3*idx3 +1], this->shapes[s].mesh.positions[3*idx3 +2]); 

      glm::vec3 sideA = v2 - v1;
      glm::vec3 sideB = v3 - v1;
      glm::vec3 norm = glm::cross(sideA, sideB);
      this->norBuf[3*idx1+0] += norm.x;
      this->norBuf[3*idx1+1] += norm.y;
      this->norBuf[3*idx1+2] += norm.z;
      this->norBuf[3*idx2+0] += norm.x;
      this->norBuf[3*idx2+1] += norm.y;
      this->norBuf[3*idx2+2] += norm.z;
      this->norBuf[3*idx3+0] += norm.x;
      this->norBuf[3*idx3+1] += norm.y;
      this->norBuf[3*idx3+2] += norm.z;
    }
    glGenBuffers(1, &this->norBufObj);
    glBindBuffer(GL_ARRAY_BUFFER, this->norBufObj);
    glBufferData(GL_ARRAY_BUFFER, this->norBuf.size()*sizeof(float), &this->norBuf[0], GL_STATIC_DRAW);
	*/
    // Send the index array to the GPU
    const std::vector<unsigned int> &indBuf = this->shapes[s].mesh.indices;
    glGenBuffers(1, &this->indBufObj);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indBufObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indBuf.size()*sizeof(unsigned int), &indBuf[0], GL_STATIC_DRAW);

    // Unbind the arrays
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    GLSL::checkVersion();
    assert(glGetError() == GL_NO_ERROR);
  }
}

void Mesh::drawObject(Handles *handles)
{
  // Enable and bind position array for drawing
  GLSL::enableVertexAttribArray(handles->aPosition);
  glBindBuffer(GL_ARRAY_BUFFER, this->posBufObj);
  glVertexAttribPointer(handles->aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // Enable and bind normal array for drawing
  GLSL::enableVertexAttribArray(handles->aNormal);
  glBindBuffer(GL_ARRAY_BUFFER, this->norBufObj);
  glVertexAttribPointer(handles->aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

  for (int s = 0; s < this->shapes.size(); ++s) {
    // Bind index array for drawing
    int nIndices = (int)this->shapes[s].mesh.indices.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indBufObj);

    glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
  }

  GLSL::disableVertexAttribArray(handles->aPosition);
  GLSL::disableVertexAttribArray(handles->aNormal);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Disable and unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

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

char* imageLoad(const char* filename) {
  FILE *file;
  unsigned long size;                 /*  size of the image in bytes. */
  unsigned long i;                    /*  standard counter. */
  unsigned short int planes;          /*  number of planes in image (must be 1)  */
  unsigned short int bpp;             /*  number of bits per pixel (must be 24) */
  char temp;                          /*  used to convert bgr to rgb color. */
  char* data;

  /*  make sure the file is there. */
  if ((file = fopen(filename, "rb")) == NULL) {
    printf("File Not Found : %s\n", filename);
    return 0;
  }

  /*  seek through the bmp header, up to the width height: */
  fseek(file, 18, SEEK_CUR);

  /*  No 100% errorchecking anymore!!! */

  /*  read the width */
  int sizeX = getint(file);

  /*  read the height */
  int sizeY = getint(file);

  /*  calculate the size (assuming 24 bits or 3 bytes per pixel). */
  size = sizeX * sizeY * 3;

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
  data = (char *)malloc(size);
  if (data == NULL) {
    printf("Error allocating memory for color-corrected image data");
    return 0;
  }

  if ((i = fread(data, size, 1, file)) != 1) {
    printf("Error reading image data from %s.\n", filename);
    return 0;
  }

  for (i = 0; i<size; i += 3) { /*  reverse all of the colors. (bgr -> rgb) */
    temp = data[i];
    data[i] = data[i + 2];
    data[i + 2] = temp;
  }

  fclose(file); /* Close the file and release the filedes */

  /*  we're done. */
  return data;
}
