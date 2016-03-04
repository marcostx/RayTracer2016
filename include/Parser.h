#include "pugixml.hpp"
#include "Scene.h"
#include "Camera.h"
#include "Material.h"
#include <string.h>
#include "TriangleMeshShape.h"
#include "MeshReader.h"

using namespace pugi;
using namespace Graphics;

class Parser {
	public:
		Parser(const char * filename);
		bool parseImage(int & height, int & width);
		Camera* parseCamera();
		Scene* parseScene();
		
	private:
		Actor* parseMesh(xml_node_iterator);
		Light* parseLight(xml_node_iterator);

		Material* parseMaterial(xml_node); 

		xml_document xml;
		xml_node root;
};
