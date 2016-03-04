//[]------------------------------------------------------------------------[]
//|                                                                          |
//|                          GVSG Graphics Classes                           |
//|                               Version 1.0                                |
//|                                                                          |
//|              Copyright® 2010-2016, Paulo Aristarco Pagliosa              |
//|              All Rights Reserved.                                        |
///|                                                                          |
//[]------------------------------------------------------------------------[]
//
//  OVERVIEW: MeshReader.cpp
//  ========
//  Source file for mesh reader.

#include <stdio.h>
#include "MeshReader.h"

using namespace Graphics;

void
readMaterialFile(const char* fileName)
{
  FILE* file = fopen(fileName, "r");

  if (file == 0)
    return;
  printf("Reading Wavefront MTL file %s... ", fileName);

  Material* material = 0;

  for (char strm[128]; fscanf(file, "%s", strm) != EOF;)
  {
    switch(strm[0])
    {
      case 'n':
        fscanf(file, "%s", strm); // newmtl
        material = MaterialFactory::New(strm);
        break;

      case 'N':
      {
        REAL shininess;

        fscanf(file, "%f", &shininess);
        // wavefront shininess is from [0, 1000], so scale for OpenGL
        shininess *= REAL(128.0 / 1000.0);
        material->surface.shine = shininess;
        break;
      }

      case 'K':
      {
        REAL r;
        REAL g;
        REAL b;

        switch(strm[1])
        {
          case 'd':
            fscanf(file, "%f %f %f", &r, &g, &b);
            material->surface.diffuse.setRGB(r, g, b);
            break;

          case 's':
            fscanf(file, "%f %f %f", &r, &g, &b);
            material->surface.spot.setRGB(r, g, b);
            break;

          case 'a':
            fscanf(file, "%f %f %f", &r, &g, &b);
            material->surface.ambient.setRGB(r, g, b);
            break;

          default:
            fgets(strm, sizeof(strm), file);
            break;
        }
        break;
      }

      default:
        fgets(strm, sizeof(strm), file);
        break;
    }
  }
  fclose(file);
  puts("done");
}

void
readMeshSize(FILE* file, TriangleMesh::Arrays& data)
{
  int nv = 0;
  int nn = 0;
  int nt = 0;

  for (char strm[128]; fscanf(file, "%s", strm) != EOF;)
    switch (strm[0])
    {
      case 'v':
        switch (strm[1])
        {
          case '\0':
            nv++;
            break;
        
          case 'n':
            nn++;
            break;
        }
        fgets(strm, sizeof(strm), file);
        break;

      case 'f':
      {
        int v;
        int n;
        int t;

        fscanf(file, "%s", strm);
        /* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
        if (strstr(strm, "//"))
        {
          /* v//n */
          sscanf(strm, "%d//%d", &v, &n);
          fscanf(file, "%d//%d", &v, &n);
          fscanf(file, "%d//%d", &v, &n);
          nt++;
          while (fscanf(file, "%d//%d", &v, &n) > 0)
            nt++;
        }
        else if (sscanf(strm, "%d/%d/%d", &v, &t, &n) == 3)
        {
          /* v/t/n */
          fscanf(file, "%d/%d/%d", &v, &t, &n);
          fscanf(file, "%d/%d/%d", &v, &t, &n);
          nt++;
          while (fscanf(file, "%d/%d/%d", &v, &t, &n) > 0)
            nt++;
        }
        else if (sscanf(strm, "%d/%d", &v, &t) == 2)
        {
          /* v/t */
          fscanf(file, "%d/%d", &v, &t);
          fscanf(file, "%d/%d", &v, &t);
          nt++;
          while (fscanf(file, "%d/%d", &v, &t) > 0)
            nt++;
        }
        else
        {
          /* v */
          fscanf(file, "%d", &v);
          fscanf(file, "%d", &v);
          nt++;
          while (fscanf(file, "%d", &v) > 0)
            nt++;
        }
        break;
      }

      case 'm':
        fscanf(file, "%s", strm);
        readMaterialFile(strm);
        break;

      default:
        fgets(strm, sizeof(strm), file);
    }
  data.numberOfVertices = nv;
  data.numberOfNormals = 0/* nn */;
  data.numberOfTriangles = nt;
}

void
readMeshData(FILE* file, TriangleMesh::Arrays& data)
{
  vec3* vertex = data.vertices;
  // vec3* normal = data.normals;
  TriangleMesh::Triangle* triangle = data.triangles;
  Material* material = MaterialFactory::getDefaultMaterial();

  for (char strm[128]; fscanf(file, "%s", strm) != EOF;)
    switch (strm[0])
    {
      case 'v':
      {
        REAL x;
        REAL y;
        REAL z;

        switch (strm[1])
        {
          case '\0':
            fscanf(file, "%f %f %f", &x, &y, &z);
            vertex->set(x, y, z);
            vertex++;
            break;

          case 'n':
            fscanf(file, "%f %f %f", &x, &y, &z);
            // normal->set(x, y, z);
            // normal++;
            break;

          default:
            fgets(strm, sizeof(strm), file);
        }
        break;
      }

      case 'f':
      {
        int v;
        int n;
        int t;

        triangle->v[3] = material->getIndex();
        fscanf(file, "%s", strm);
        /* Can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
        if (strstr(strm, "//"))
        {
          /* v//n */
          sscanf(strm, "%d//%d", &v, &n);
          triangle->v[0] = v - 1;
          // triangle->n[0] = n - 1;
          fscanf(file, "%d//%d", &v, &n);
          triangle->v[1] = v - 1;
          // triangle->n[1] = n - 1;
          fscanf(file, "%d//%d", &v, &n);
          triangle->v[2] = v - 1;
          // triangle->n[2] = triangle->n[3] = n - 1;
          triangle++;
          while (fscanf(file, "%d//%d", &v, &n) > 0)
          {
            triangle->v[0] = triangle[-1].v[0];
            // triangle->n[0] = triangle[-1].n[0];
            triangle->v[1] = triangle[-1].v[2];
            // triangle->n[1] = triangle[-1].n[2];
            triangle->v[2] = v - 1;
            // triangle->n[2] = triangle->n[3] = n - 1;
            triangle++;
          }
        }
        else if (sscanf(strm, "%d/%d/%d", &v, &t, &n) == 3)
        {
          /* v/t/n */
          triangle->v[0] = v - 1;
          // triangle->t[0] = t - 1;
          // triangle->n[0] = n - 1;
          fscanf(file, "%d/%d/%d", &v, &t, &n);
          triangle->v[1] = v - 1;
          // triangle->t[1] = t - 1;
          // triangle->n[1] = n - 1;
          fscanf(file, "%d/%d/%d", &v, &t, &n);
          triangle->v[2] = v - 1;
          // triangle->t[2] = t - 1;
          // triangle->n[2] = triangle->n[3] = n - 1;
          triangle++;
          while (fscanf(file, "%d/%d/%d", &v, &t, &n) > 0)
          {
            triangle->v[0] = triangle[-1].v[0];
            // triangle->t[0] = triangle[-1].t[0];
            // triangle->n[0] = triangle[-1].n[0];
            triangle->v[1] = triangle[-1].v[2];
            // triangle->t[1] = triangle[-1].t[2];
            // triangle->n[1] = triangle[-1].n[2];
            triangle->v[2] = v - 1;
            // triangle->tindices[2] = t - 1;
            // triangle->n[2] = triangle->n[3] = n - 1;
            triangle++;
          }
        }
        else if (sscanf(strm, "%d/%d", &v, &t) == 2)
        {
          /* v/t */
          triangle->v[0] = v - 1;
          // triangle->t[0] = t - 1;
          fscanf(file, "%d/%d", &v, &t);
          triangle->v[1] = v - 1;
          // triangle->t[1] = t - 1;
          fscanf(file, "%d/%d", &v, &t);
          triangle->v[2] = v - 1;
          // triangle->t[2] = t - 1;
          // triangle->setNormal(-1);
          triangle++;
          while (fscanf(file, "%d/%d", &v, &t) > 0)
          {
            triangle->v[0] = triangle[-1].v[0];
            // triangle->t[0] = triangle[-1].t[0];
            triangle->v[1] = triangle[-1].v[2];
            // triangle->t[1] = triangle[-1].t[2];
            triangle->v[2] = v - 1;
            // triangle->t[2] = t - 1;
            // triangle->setNormal(-1);
            triangle++;
          }
        }
        else
        {
          /* v */
          sscanf(strm, "%d", &v);
          triangle->v[0] = v - 1;
          fscanf(file, "%d", &v);
          triangle->v[1] = v - 1;
          fscanf(file, "%d", &v);
          triangle->v[2] = v - 1;
          // triangle->setNormal(-1);
          triangle++;
          while (fscanf(file, "%d", &v) > 0)
          {
            triangle->v[0] = triangle[-1].v[0];
            triangle->v[1] = triangle[-1].v[2];
            triangle->v[2] = v - 1;
            // triangle->setNormal(-1);
            triangle++;
          }
        }
        break;
      }

      case 'u':
      {
        fscanf(file, "%s", strm);

        Material* m = MaterialFactory::get(strm);

        if (m != 0)
          material = m;
        break;
      }

      default:
        fgets(strm, sizeof(strm), file);
    }
}


//////////////////////////////////////////////////////////
//
// MeshReader implementation
// ==========
TriangleMesh*
MeshReader::execute(const char* fileName)
//[]----------------------------------------------------[]
//|  Execute (read Wavefront OBJ file)                   |
//[]----------------------------------------------------[]
{
  FILE* file = fopen(fileName, "r");

  if (file == 0)
    return 0;

  TriangleMesh::Arrays data;

  readMeshSize(file, data);
  data.vertices = new vec3[data.numberOfVertices];
  if (data.numberOfNormals != 0)
    data.normals = new vec3[data.numberOfNormals];
  data.triangles = new TriangleMesh::Triangle[data.numberOfTriangles];
  rewind(file);
  printf("Reading Wavefront OBJ file %s... ", fileName);
  readMeshData(file, data);
  fclose(file);
  puts("done");
  /*
  file = fopen("a.msh", "w");
  data.print(file);
  fclose(file);
  */

  TriangleMesh* mesh = new TriangleMesh(data);

  mesh->computeNormals();
  return mesh;
}
