#include "SceneReader.h"

SceneReader::SceneReader(const char* filename) 
{

	document.load_file(filename);
	root = document.first_child();

}

bool SceneReader::readImage(int & height, int & width) 
{
	xml_node xml_image = root.child("image");

	if (xml_image != NULL) 
	{
		height = xml_image.child("height").text().as_int();
		width = xml_image.child("width").text().as_int();
		return true;
	}
	return false;
}

Camera* SceneReader::readCamera() 
{
	xml_node xml_camera = root.child("camera");

	if (xml_camera != NULL) 
	{
		Camera* camera = new Camera();

		REAL x, y, z;

		const char* position = xml_camera.child("position").text().as_string();
		sscanf(position, "%f %f %f", &x, &y, &z);
		camera->setPosition(vec3(x, y, z));

		const char* pointingTo = xml_camera.child("to").text().as_string();
		sscanf(pointingTo, "%f %f %f", &x, &y, &z);
		vec3 pointingVector = vec3(x, y, z) - camera->getPosition();
		camera->setDistance(pointingVector.length());
		camera->setDirectionOfProjection(pointingVector.versor());

		const char* upVector = xml_camera.child("up").text().as_string();
		sscanf(upVector, "%f %f %f", &x, &y, &z);
		camera->setViewUp(vec3(x, y, z));

		xml_node optional;

		if ((optional = xml_camera.child("angle")) != NULL) 
		{
			float angle = optional.text().as_float();
			camera->setViewAngle(angle);
		}

		if ((optional = xml_camera.child("aspect")) != NULL) 
		{
			const char* stringAspect = optional.text().as_string();
			int height;
			int width;
			sscanf(stringAspect, "%d:%d", &width, &height);
			camera->setAspectRatio((float)width / (float)height);
		}

		if ((optional = xml_camera.child("projection")) != NULL) 
		{
			const char* projectionType = optional.text().as_string();
			if (strcmp(projectionType, "perspective") == 0) 
			{
				camera->setProjectionType(Camera::Perspective);
			}
			else 
			{
				camera->setProjectionType(Camera::Parallel);
			}
		}

		return camera;
	}

	return NULL;
}

Scene* SceneReader::readScene() 
{
	xml_node xml_scene = root.child("scene");
	Scene* scene = Scene::New();

	xml_attribute sceneName;

	if ((sceneName = xml_scene.attribute("name")) != NULL) 
	{
		scene->setName(sceneName.value());
	}

	xml_node optional;

	if ((optional = xml_scene.child("background")) != NULL) 
	{
		const char* background = optional.text().as_string();
		REAL x, y, z;
		sscanf(background, "%f %f %f", &x, &y, &z);
		scene->backgroundColor = Color(x, y, z);
		xml_scene.remove_child("background");
	}

	if ((optional = xml_scene.child("ambient")) != NULL) 
	{
		const char* ambient = optional.text().as_string();
		REAL x, y, z;
		sscanf(ambient, "%f %f %f", &x, &y, &z);
		scene->ambientLight = Color(x, y, z);
		xml_scene.remove_child("ambient");
	}

	xml_object_range<xml_node_iterator> children = xml_scene.children();

	for (xml_node_iterator sceneElement = children.begin(); sceneElement != children.end(); ++sceneElement) 
	{
		if (strcmp(sceneElement->name(), "mesh") == 0) 
		{
			scene->addActor(readMesh(sceneElement));
		}
		else if (strcmp(sceneElement->name(), "sphere") == 0) 
		{
			scene->addActor(readSphere(sceneElement));
		}
		else if (strcmp(sceneElement->name(), "cylinder") == 0) 
		{
			scene->addActor(readCylinder(sceneElement));
		}
		else if (strcmp(sceneElement->name(), "box") == 0) 
		{
			scene->addActor(readBox(sceneElement));
		}
		else if (strcmp(sceneElement->name(), "cone") == 0) 
		{
			scene->addActor(readCone(sceneElement));
		}
		else if (strcmp(sceneElement->name(), "light") == 0) 
		{
			scene->addLight(readLight(sceneElement));
		}
	}

	return scene;
}

Actor* SceneReader::readBox(xml_node_iterator xml_box) 
{
	vec3 center(0, 0, 0);
	quat orientation(vec3(0, 0, 0));
	vec3 scale(1, 1, 1);

	xml_node optional;

	if ((optional = xml_box->child("center")) != NULL) 
	{
		const char* centerString = optional.text().as_string();
		REAL x, y, z;
		sscanf(centerString, "%f %f %f", &x, &y, &z);
		center.set(x, y, z);
	}
	
	if ((optional = xml_box->child("orientation")) != NULL) 
	{
		const char* orientationString = optional.text().as_string();
		REAL x, y, z;
		sscanf(orientationString, "%f %f %f", &y, &x, &z);
		orientation.eulerAngles(vec3(x,y,z));
	}
	
	if ((optional = xml_box->child("scale")) != NULL) 
	{
		const char* scaleString = optional.text().as_string();
		REAL x, y, z;
		sscanf(scaleString, "%f %f %f", &x, &y, &z);
		scale.set(x, y, z);
	}

	TriangleMesh* mesh = MeshSweeper::makeBox(center, orientation, scale);

	Primitive* primitive = new TriangleMeshShape(mesh);

	if ((optional = xml_box->child("transform")) != NULL)
	{
		vec3 position(0, 0, 0);
		quat q(vec3(0, 0, 0));
		vec3 scale(1, 1, 1);

		xml_object_range<xml_node_iterator> transformations = optional.children();

		for (xml_node_iterator transformation = transformations.begin(); transformation != transformations.end(); ++transformation)
		{
			if (strcmp(transformation->name(), "position") == 0)
			{
				position = readTranslation(transformation);
			}
			else if (strcmp(transformation->name(), "scale") == 0)
			{
				scale = readScale(transformation);
			}
			else if (strcmp(transformation->name(), "rotation") == 0)
			{
				q = readRotation(transformation);
			}
		}

		mesh->transform(mat4::TRS(position, q, scale));
	}

	if ((optional = xml_box->child("material")) != NULL)
	{
		Material* material = readMaterial(optional);
		primitive->setMaterial(material);
	}
	else
		primitive->setMaterial(MaterialFactory::New());

	return new Actor(*primitive);
}

Actor* SceneReader::readCone(xml_node_iterator xml_cone) 
{
	vec3 center(0, 0, 0);
	REAL radius = 1.0f;
	vec3 height(0, 1, 0);
	int segments = 16;

	xml_node optional;
	
	if ((optional = xml_cone->child("center")) != NULL) 
	{
		const char* centerString = optional.text().as_string();
		REAL x, y, z;
		sscanf(centerString, "%f %f %f", &x, &y, &z);
		center.set(x, y, z);
	}

	if ((optional = xml_cone->child("radius")) != NULL) 
	{
		radius = optional.text().as_float();
	}

	if ((optional = xml_cone->child("height")) != NULL) 
	{
		const char* heightString = optional.text().as_string();
		REAL x, y, z;
		sscanf(heightString, "%f %f %f", &x, &y, &z);
		height.set(x, y, z);
	}

	if ((optional = xml_cone->child("segments")) != NULL) 
	{
		segments = optional.text().as_int();
	}

	TriangleMesh* mesh = MeshSweeper::makeCone(center, radius, height, segments);

	Primitive* primitive = new TriangleMeshShape(mesh);

	if ((optional = xml_cone->child("transform")) != NULL)
	{
		vec3 position(0, 0, 0);
		quat q(vec3(0, 0, 0));
		vec3 scale(1, 1, 1);

		xml_object_range<xml_node_iterator> transformations = optional.children();

		for (xml_node_iterator transformation = transformations.begin(); transformation != transformations.end(); ++transformation)
		{
			if (strcmp(transformation->name(), "position") == 0)
			{
				position = readTranslation(transformation);
			}
			else if (strcmp(transformation->name(), "scale") == 0)
			{
				scale = readScale(transformation);
			}
			else if (strcmp(transformation->name(), "rotation") == 0)
			{
				q = readRotation(transformation);
			}
		}

		mesh->transform(mat4::TRS(position, q, scale));
	}

	if ((optional = xml_cone->child("material")) != NULL)
	{
		Material* material = readMaterial(optional);
		primitive->setMaterial(material);
	}
	else
		primitive->setMaterial(MaterialFactory::New());

	return new Actor(*primitive);
}

Actor* SceneReader::readCylinder(xml_node_iterator xml_cylinder) 
{
	vec3 center(0, 0, 0);
	REAL radius = 1.0f;
	vec3 height(0, 1, 0);
	int segments = 16;

	xml_node optional;
	
	if ((optional = xml_cylinder->child("center")) != NULL) 
	{
		const char* centerString = optional.text().as_string();
		REAL x, y, z;
		sscanf(centerString, "%f %f %f", &x, &y, &z);
		center.set(x, y, z);
	}

	if ((optional = xml_cylinder->child("radius")) != NULL) 
	{
		radius = optional.text().as_float();
	}

	if ((optional = xml_cylinder->child("height")) != NULL) 
	{
		const char* heightString = optional.text().as_string();
		REAL x, y, z;
		sscanf(heightString, "%f %f %f", &x, &y, &z);
		height.set(x, y, z);
	}

	if ((optional = xml_cylinder->child("segments")) != NULL) 
	{
		segments = optional.text().as_int();
	}

	TriangleMesh* mesh = MeshSweeper::makeCylinder(center, radius, height, segments);

	Primitive* primitive = new TriangleMeshShape(mesh);

	if ((optional = xml_cylinder->child("transform")) != NULL)
	{
		vec3 position(0, 0, 0);
		quat q(vec3(0, 0, 0));
		vec3 scale(1, 1, 1);

		xml_object_range<xml_node_iterator> transformations = optional.children();

		for (xml_node_iterator transformation = transformations.begin(); transformation != transformations.end(); ++transformation)
		{
			if (strcmp(transformation->name(), "position") == 0)
			{
				position = readTranslation(transformation);
			}
			else if (strcmp(transformation->name(), "scale") == 0)
			{
				scale = readScale(transformation);
			}
			else if (strcmp(transformation->name(), "rotation") == 0)
			{
				q = readRotation(transformation);
			}
		}

		mesh->transform(mat4::TRS(position, q, scale));
	}

	if ((optional = xml_cylinder->child("material")) != NULL)
	{
		Material* material = readMaterial(optional);
		primitive->setMaterial(material);
	}
	else
		primitive->setMaterial(MaterialFactory::New());

	return new Actor(*primitive);
}

Actor* SceneReader::readSphere(xml_node_iterator xml_sphere) 
{
	vec3 center(0, 0, 0);
	REAL radius = 1.0f;
	int meridians = 16;

	xml_node optional;
	
	if ((optional = xml_sphere->child("center")) != NULL) 
	{
		const char* centerString = optional.text().as_string();
		REAL x, y, z;
		sscanf(centerString, "%f %f %f", &x, &y, &z);
		center.set(x, y, z);
	}

	if ((optional = xml_sphere->child("radius")) != NULL) 
	{
		radius = optional.text().as_float();
	}

	if ((optional = xml_sphere->child("meridians")) != NULL)
	{
		meridians = optional.text().as_float();
	}

	TriangleMesh* mesh = MeshSweeper::makeSphere(center, radius, meridians);
	
	Primitive* primitive = new TriangleMeshShape(mesh);

	if ((optional = xml_sphere->child("transform")) != NULL) 
	{
		vec3 position(0, 0, 0);
		quat q(vec3(0, 0, 0));
		vec3 scale(1, 1, 1);

		xml_object_range<xml_node_iterator> transformations = optional.children();

		for (xml_node_iterator transformation = transformations.begin(); transformation != transformations.end(); ++transformation)
		{
			if (strcmp(transformation->name(), "position") == 0)
			{
				position = readTranslation(transformation);
			}
			else if (strcmp(transformation->name(), "scale") == 0)
			{
				scale = readScale(transformation);
			}
			else if (strcmp(transformation->name(), "rotation") == 0)
			{
				q = readRotation(transformation);
			}
		}

		mesh->transform(mat4::TRS(position, q, scale));
	}

	if ((optional = xml_sphere->child("material")) != NULL)
	{
		Material* material = readMaterial(optional);
		primitive->setMaterial(material);
	}
	else
		primitive->setMaterial(MaterialFactory::New());

	return new Actor(*primitive);
}

Actor* SceneReader::readMesh(xml_node_iterator xml_mesh) 
{
	const char* objFileName = xml_mesh->attribute("file").value();

	TriangleMesh* mesh = MeshReader().execute(objFileName);

	xml_node optional;
	
	Primitive* primitive = new TriangleMeshShape(mesh);

	if ((optional = xml_mesh->child("transform")) != NULL)
	{
		vec3 position(0, 0, 0);
		quat q(vec3(0, 0, 0));
		vec3 scale(1, 1, 1);

		xml_object_range<xml_node_iterator> transformations = optional.children();

		for (xml_node_iterator transformation = transformations.begin(); transformation != transformations.end(); ++transformation)
		{
			if (strcmp(transformation->name(), "position") == 0)
			{
				position = readTranslation(transformation);
			}
			else if (strcmp(transformation->name(), "scale") == 0)
			{
				scale = readScale(transformation);
			}
			else if (strcmp(transformation->name(), "rotation") == 0)
			{
				q = readRotation(transformation);
			}
		}

		mesh->transform(mat4::TRS(position, q, scale));
	}

	if ((optional = xml_mesh->child("material")) != NULL)
	{
		Material* material = readMaterial(optional);
		primitive->setMaterial(material);
	}
	else
		primitive->setMaterial(MaterialFactory::New());

	return new Actor(*primitive);
}

vec3 SceneReader::readTranslation(xml_node_iterator xml_translation) 
{
	const char* stringTranslation = xml_translation->text().as_string();
	REAL x, y, z;
	sscanf(stringTranslation, "%f %f %f", &x, &y, &z);
	vec3 translation(x, y, z);

	return translation;
}

vec3 SceneReader::readScale(xml_node_iterator xml_scale) 
{
	float scale;
	scale = xml_scale->text().as_float();

	vec3 scaleVec(scale, scale, scale);
	
	return scaleVec;
}

quat SceneReader::readRotation(xml_node_iterator xml_rotation) 
{
	const char* stringAxis = xml_rotation->child("axis").text().as_string();
	REAL x, y, z;
	sscanf(stringAxis, "%f %f %f", &x, &y, &z);
	vec3 axis(x, y, z);

	float angle = xml_rotation->child("angle").text().as_float();

	quat rotation(angle, axis);

	return rotation;
}

Light* SceneReader::readLight(xml_node_iterator xml_light) 
{
	REAL x, y, z;
	Color lightColor = Color::white;
	int falloff = 0;
	int type;

	const char* stringPosition = xml_light->child("position").text().as_string();
	sscanf(stringPosition, "%f %f %f %d", &x, &y, &z, &type);
	vec3 position(x, y, z);

	xml_node optional;

	if ((optional = xml_light->child("color")) != NULL) 
	{
		const char* stringColor = optional.text().as_string();
		sscanf(stringColor, "%f %f %f", &x, &y, &z);
		lightColor.setRGB(x, y, z);
	}

	Light* light = new Light(position, lightColor);
	
	if (type == 0)
		light->setDirectional(true);
	else
		light->setDirectional(false);

	if ((optional = xml_light->child("falloff")) != NULL) 
	{
		falloff = optional.text().as_int();
		
		switch (falloff) 
		{
			case 1:
				light->flags.enable(Light::Linear, true);
				break;
			case 2:
				light->flags.enable(Light::Squared, true);
				break;
		}
	}

	return light;
}

Material* SceneReader::readMaterial(xml_node xml_material) 
{
	Material* material = MaterialFactory::New();

	REAL x, y, z;
	xml_node optional;
	
	if ((optional = xml_material.child("ambient")) != NULL) 
	{
		const char* ambientString = optional.text().as_string();
		sscanf(ambientString, "%f %f %f", &x, &y, &z);
		material->surface.ambient = Color(x, y, z);
	}

	if ((optional = xml_material.child("diffuse")) != NULL) 
	{
		const char* diffuseString = optional.text().as_string();
		sscanf(diffuseString, "%f %f %f", &x, &y, &z);
		material->surface.diffuse = Color(x, y, z);
	}

	if ((optional = xml_material.child("spot")) != NULL) 
	{
		const char* spotString = optional.text().as_string();
		sscanf(spotString, "%f %f %f", &x, &y, &z);
		material->surface.spot = Color(x, y, z);
	}
	
	if ((optional = xml_material.child("shine")) != NULL) 
	{
		float shine = optional.text().as_float();
		material->surface.shine = shine;
	}

	if ((optional = xml_material.child("specular")) != NULL) 
	{
		const char* specularString = optional.text().as_string();
		sscanf(specularString, "%f %f %f", &x, &y, &z);
		material->surface.specular = Color(x, y, z);
	}

	return material;
}