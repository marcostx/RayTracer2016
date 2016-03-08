#include "Parser.h"
#include <iostream>

using namespace std;
int
main(int argc, char const *argv[])
{

	//SceneReader* s = new SceneReader("simple-scene.xml");
	//s->readCamera();

	Parser* p = new Parser("simple-scene.xml");
	Scene *s = new Scene("lol");
	s = p->parseScene();

	system("pause");
	return 0;
}