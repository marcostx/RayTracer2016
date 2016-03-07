#include "Parser.h"

Parser::Parser(const char * filename) {
	xml.load_file(filename);
	root = xml.first_child();
}

bool Parser::parseImage(int & height, int & width) {
	xml_node xml_image = root.child("image");
	if (xml_image != NULL) {
		height = xml_image.child("height").text().as_int();
		width = xml_image.child("width").text().as_int();
		return true;
	}
	return false;
}
	
Camera* Parser::parseCamera(){
	///
	xml_node camera = root.child("camera");
	// if empty, return NULL or	 a default camera setting
	if (camera != NULL)
	{
		// filling the params
		float x,y,z;
		Camera* c_ = new Camera();

		const char* pos = camera.child("position").text().as_string();
		sscanf(pos,"%f %f %f",&x,&y,&z);

		vec3 vec_pos = vec3(x,y,z);
		// setting the camera position
		c_->setPosition(vec_pos);

		const char* focal = camera.child("to").text().as_string();
		sscanf(focal,"%f %f %f",&x,&y,&z);
		vec3 direction = vec3(x, y, z) - c_->getPosition();

		// setting the D
		c_->setDistance(direction.length());
		// setting the DOP
		c_->setDirectionOfProjection(direction.versor());

		const char* vup = camera.child("up").text().as_string();
		sscanf(vup,"%f %f %f",&x,&y,&z);
		vec3 vec_vup = vec3(x,y,z);
		// setting the VUP
		c_->setViewUp(vec_vup);

		// optional settings
		xml_node op;
		op = camera.child("angle");
		if ( op != NULL )
		{
			float angle = op.text().as_float();
			c_->setViewAngle(angle);
		}
		op = camera.child("aspect");
		if ( op != NULL )
		{
			const char* stringAspect = op.text().as_string();
			int height;
			int width;
			sscanf(stringAspect, "%d:%d", &width, &height);
			c_->setAspectRatio((float)width / (float)height);
		}
		op = camera.child("projection");
		if ( op != NULL )
		{
			const char* proj = op.text().as_string();
			
			if (strcmp(proj,"parallel") == 0)
				c_->setProjectionType(Camera::Parallel);
			else
				c_->setProjectionType(Camera::Perspective);	
		}
		return c_;
	}
	return NULL;
}


Scene* Parser::parseScene(){
	///
	xml_node scene = root.child("scene");
	
	// optional name
	Scene* _s;
	if (scene.attribute("name") != NULL)
		 _s = new Scene(scene.attribute("name").value());
	else
		 _s = Scene::New();

	// optional settings
	xml_node op;
	op = scene.child("background");
	float x,y,z;
	if ( op != NULL )
	{
		const char* back = op.text().as_string();
		sscanf(back,"%f %f %f",&x,&y,&z);

		_s->backgroundColor = Color(x,y,z);
		scene.remove_child("background");
	}
	op = scene.child("ambient");
	if ( op != NULL )
	{
		const char* amb = op.text().as_string();
		sscanf(amb,"%f %f %f",&x,&y,&z);

		_s->ambientLight = Color(x,y,z);
		scene.remove_child("ambient");
	}
	
	// processing the objets in the scene
	xml_object_range<xml_node_iterator> objets = scene.children();
	Light* light;
	for (xml_node_iterator sceneElement = objets.begin(); sceneElement != objets.end(); ++sceneElement) {
		
		if (strcmp(sceneElement->name(), "mesh") == 0)
			_s->addActor(parseMesh(sceneElement));

		if (strcmp(sceneElement->name(), "sphere") == 0)
			_s->addActor(parseSphere(sceneElement));

		if (strcmp(sceneElement->name(), "box") == 0)
			_s->addActor(parseBox(sceneElement));

		if (strcmp(sceneElement->name(), "cone") == 0)
			_s->addActor(parseCone(sceneElement));

		if (strcmp(sceneElement->name(), "cylinder") == 0)
			_s->addActor(parseCylinderh(sceneElement));

		else if (strcmp(sceneElement->name(), "light") == 0){
			light = parseLight(sceneElement);
			_s->addLight(light);
		}
	}

	return _s;
}

Actor* Parser::parseSphere(xml_node_iterator sceneElement)
{
	// default values
	vec3 center(0,0,0);
	REAL radius=1.0;
	int meridians=16;

	// opt values
	xml_node op;
	op = sceneElement.child("center");
	REAL x,y,z;
	if ( op != NULL )
	{
		const char* center_vector = op.text().as_string();

		sscanf(center_vector,"%f %f %f",&x,&y,&z);
		center.set(x,y,z);
	}
	op = sceneElement.child("radius");
	if (op != NULL)
		radius = op.text.as_float();
	op = sceneElement.child("meridians");
	if (op != NULL)
		meridians = op.text.as_int();

	// now, lets make the mesh of Sphere
	TriangleMesh* sphereMesh = MeshSweeper::makeSphere(center,radius,meridians);

	Primitive* mesh = new TriangleMeshShape(sphereMesh);

	if ((op = sceneElement->child("transform")) != NULL) {
		vec3 position(0, 0, 0);
		quat q(vec3(0, 0, 0));
		vec3 scale(1,1,1);
		float x, y, z;

		xml_object_range<xml_node_iterator> transformations = op.children();

		for (xml_node_iterator transformation = transformations.begin(); transformation != transformations.end(); ++transformation)
		{
			if (strcmp(transformation->name(), "position") == 0)
			{
				const char * stringTranslation = transformation->text().as_string();

				sscanf(stringTranslation, "%f %f %f", &x, &y, &z);
				vec3 translationVec(x, y, z);
				position = translationVec;
			}
			else if (strcmp(transformation->name(), "scale") == 0)
			{
				float s_ = transformation->text().as_float();
				vec3 newS(s_, s_, s_);
				scale = newS;
			}
			else if (strcmp(transformation->name(), "rotation") == 0)
			{
				float angle = transformation->child("angle").text().as_float();

				const char * _Axis = transformation->child("axis").text().as_string();
				sscanf(_Axis, "%f %f %f", &x, &y, &z);
				vec3 axis(x, y, z);
				q = quat(axis, angle);
			}

			mesh->transform(mat4::TRS(position, q, scale));
		}
	}
	if ((op = sceneElement->child("material")) != NULL) {
		Material * material = parseMaterial(op);
		primitive->setMaterial(material);
	}

	return new Actor(*primitive);
	
}
	
Light* Parser::parseLight(xml_node_iterator sceneElement)
{
	// setting the light
	REAL x,y,z;

	const char* type = sceneElement->attribute("type").value();
	Light * l;

	// checking the type of light
	if (strcmp(type,"point") == 0)
	{
		
		// positon
		const char* pos = sceneElement->child("position").text().as_string();
		sscanf(pos,"%f %f %f",&x,&y,&z);

		vec3 position(x,y,z);

		// optional settings
		// color
		xml_node op = sceneElement->child("color");
		Color color = Color::white;
		if (op != NULL) 
		{
			const char* c = op.text().as_string();
			sscanf(c,"%f %f %f",&x,&y,&z);

			color.setRGB(x, y, z);
		}
		l = new Light(position,color);
		int falloff=0;

		// setting falloff
		op = sceneElement->child("falloff");
		if (op != NULL) {
			falloff = op.text().as_int();
			switch (falloff) {
				case 1:
					l->flags.enable(Light::Linear, true);
					break;
				case 2:
					l->flags.enable(Light::Squared, true);
					break;
			}
		}
	}
	else if (strcmp(type,"directional") == 0)
	{
		
		// direction
		const char* dir = sceneElement->child("direction").text().as_string();
		sscanf(dir,"%f %f %f",&x,&y,&z);

		vec3 direction(x,y,z);

		// optional settings
		// color
		xml_node op = sceneElement->child("color");
		Color color = Color::white;
		if (op != NULL) 
		{
			const char* c = op.text().as_string();
			sscanf(c,"%f %f %f",&x,&y,&z);

			color.setRGB(x,y,z);
		}
		l = new Light(direction,color);
		l->setDirectional(true);
	}
	else if(strcmp(type,"spot") == 0)
	{
		///// TODO: OPCIONAL !
		//// .
		/// ..
		// ...
	}

	return l;

}

Actor* Parser::parseMesh(xml_node_iterator sceneElement)
{
	// setting the mesh
	const char* filename = sceneElement->attribute("file").value();

	TriangleMesh* mesh = MeshReader().execute(filename);

	Primitive* primitive = new TriangleMeshShape(mesh);

	xml_node op;
	if ((op = sceneElement->child("transform")) != NULL) {
		vec3 position(0, 0, 0);
		quat q(vec3(0, 0, 0));
		vec3 scale(1,1,1);
		float x, y, z;

		xml_object_range<xml_node_iterator> transformations = op.children();

		for (xml_node_iterator transformation = transformations.begin(); transformation != transformations.end(); ++transformation)
		{
			if (strcmp(transformation->name(), "position") == 0)
			{
				const char * stringTranslation = transformation->text().as_string();

				sscanf(stringTranslation, "%f %f %f", &x, &y, &z);
				vec3 translationVec(x, y, z);
				position = translationVec;
			}
			else if (strcmp(transformation->name(), "scale") == 0)
			{
				float s_ = transformation->text().as_float();
				vec3 newS(s_, s_, s_);
				scale = newS;
			}
			else if (strcmp(transformation->name(), "rotation") == 0)
			{
				float angle = transformation->child("angle").text().as_float();

				const char * _Axis = transformation->child("axis").text().as_string();
				sscanf(_Axis, "%f %f %f", &x, &y, &z);
				vec3 axis(x, y, z);
				q = quat(axis, angle);
			}

			primitive->transform(mat4::TRS(position, q, scale));
		}
	}
	if ((op = sceneElement->child("material")) != NULL) {
		Material * material = parseMaterial(op);
		primitive->setMaterial(material);
	}

	return new Actor(*primitive);
}

Material* Parser::parseMaterial(xml_node xml_mat)
{
	Material * material = MaterialFactory::New();

	float x, y, z;
	xml_node op;

	if ((op = xml_mat.child("ambient")) != NULL) {
		const char* ambientString = op.text().as_string();
		sscanf(ambientString, "%f %f %f", &x, &y, &z);
		material->surface.ambient = Color(x, y, z);
	}
	if ((op = xml_mat.child("diffuse")) != NULL) {
		const char* diffuseString = op.text().as_string();
		sscanf(diffuseString, "%f %f %f", &x, &y, &z);
		material->surface.diffuse = Color(x, y, z);
	}
	if ((op = xml_mat.child("spot")) != NULL) {
		const char* spotString = op.text().as_string();
		sscanf(spotString, "%f %f %f", &x, &y, &z);
		material->surface.spot = Color(x, y, z);
	}
	if ((op = xml_mat.child("shine")) != NULL) {
		float shine = op.text().as_float();
		material->surface.shine = shine;
	}
	if ((op = xml_mat.child("specular")) != NULL) {
		const char* specularString = op.text().as_string();
		sscanf(specularString, "%f %f %f", &x, &y, &z);
		material->surface.specular = Color(x, y, z);
	}

	return material;
}

