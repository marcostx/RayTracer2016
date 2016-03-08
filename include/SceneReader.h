#ifndef __SceneReader_h
#define __SceneReader_h

#include "pugixml.hpp"
#include "Scene.h"
#include "Camera.h"
#include "MeshSweeper.h"
#include "TriangleMeshShape.h"
#include "MeshReader.h"

using namespace pugi;
using namespace Graphics;

class SceneReader {
public:
	SceneReader(const char * filename);

	bool readImage(int & height, int & width);
	Camera * readCamera();
	Scene * readScene();

private:
	Actor * readBox(xml_node_iterator xml_box);
	Actor * readCone(xml_node_iterator xml_cone);
	Actor * readCylinder(xml_node_iterator xml_cylinder);
	Actor * readSphere(xml_node_iterator xml_sphere);
	Actor * readMesh(xml_node_iterator xml_mesh);

	vec3 readTranslation(xml_node_iterator xml_translation);
	vec3 readScale(xml_node_iterator xml_scale);
	quat readRotation(xml_node_iterator xml_rotation);

	Material * readMaterial(xml_node xml_material);
	Light * readLight(xml_node_iterator xml_light);

	xml_document document;
	xml_node root;
};

#endif