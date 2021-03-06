#include "pugixml.hpp"
#include "Scene.h"
#include "Camera.h"
#include "Material.h"
#include <string.h>
#include "TriangleMeshShape.h"
#include "MeshReader.h"
#include "MeshSweeper.h"

using namespace pugi;
using namespace Graphics;

class Parser {
public:
	Parser(const char * filename);
	void parseImage(int & height, int & width);
	Camera* parseCamera();
	Scene* parseScene();

private:
	Actor* parseMesh(xml_node_iterator);
	Light* parseLight(xml_node_iterator);

	Material* parseMaterial(xml_node);
	Actor* parseSphere(xml_node_iterator);
	Actor* parseCone(xml_node_iterator);
	Actor* parseBox(xml_node_iterator);
	Actor* parseCylinder(xml_node_iterator);

	xml_document xml;
	xml_node root;
};