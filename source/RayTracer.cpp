//[]------------------------------------------------------------------------[]
//|                                                                          |
//|                          GVSG Graphics Library                           |
//|                               Version 1.0                                |
//|                                                                          |
//|              Copyright® 2007-2016, Paulo Aristarco Pagliosa              |
//|              All Rights Reserved.                                        |
//|                                                                          |
//[]------------------------------------------------------------------------[]
//
//  OVERVIEW: RayTracer.cpp
//  ========
//  Source file for simple ray tracer.

#include <map>
#include <time.h>
#include "BVH.h"
#include "RayTracer.h"
#include "algorithm"
#include <stdlib.h>
#include <algorithm>
#include <math.h>
#include <iostream>

using namespace std;
using namespace Graphics;

void
printElapsedTime(const char* s, clock_t time)
{
	printf("%sElapsed time: %.4f s\n", s, (REAL)time / CLOCKS_PER_SEC);
}


//////////////////////////////////////////////////////////
//
// RayTracer implementation
// =========
RayTracer::RayTracer(Scene& scene, Camera* camera) :
Renderer(scene, camera),
maxRecursionLevel(6),
minWeight(MIN_WEIGHT)
//[]---------------------------------------------------[]
//|  Constructor                                        |
//[]---------------------------------------------------[]
{
	// TODO: UNCOMMENT THE CODE BELOW

	int n = scene.getNumberOfActors();

	printf("Building aggregates for %d actors...\n", n);

	clock_t t = clock();
	Array<ModelPtr> models(n);
	map<uint, ModelPtr> aggregates;
	string actorNames;
	int totalNodes = 0;
	int i = 1;

	for (ActorIterator ait(scene.getActorIterator()); ait; i++)
	{
		const Actor* a = ait++;

		printf("Processing actor %d/%d...\n", i, n);
		if (!a->isVisible())
			continue;

		Primitive* p = dynamic_cast<Primitive*>(a->getModel());
		const TriangleMesh* mesh = p->triangleMesh();
		// checking if the id already exists in idList

		if (mesh != 0)	
		{
			ModelPtr& a = aggregates[mesh->id];

			BVH* bvh = new BVH(std::move(p->refine()));

			totalNodes += bvh->size();
			a = bvh;

			models.add(new ModelInstance(*a, *p));
			
		}
	}
	printf("Building scene aggregate...\n");
	{
		BVH* bvh = new BVH(std::move(models));

		totalNodes += bvh->size();
		aggregate = bvh;
	}
	printf("BVH(s) built: %d (%d nodes)\n", aggregates.size() + 1, totalNodes);
	printElapsedTime("", clock() - t);

}

//
// Auxiliary VRC
//
static vec3 VRC_u;
static vec3 VRC_v;
static vec3 VRC_n;

//
// Auxiliary mapping variables
//
static REAL V_h;
static REAL V_w;
static REAL I_h;
static REAL I_w;

void
RayTracer::render()
//[]---------------------------------------------------[]
//|  Render                                             |
//[]---------------------------------------------------[]
{
	System::warning("Invoke renderImage(image) to run the ray tracer\n");
}

static int64 numberOfRays;
static int64 numberOfHits;

void
RayTracer::renderImage(Image& image, bool isAdaptative)
//[]---------------------------------------------------[]
//|  Run the ray tracer                                 |
//[]---------------------------------------------------[]
{
	clock_t t = clock();

	image.getSize(W, H);
	// init auxiliary VRC
	VRC_n = camera->getViewPlaneNormal();
	VRC_v = camera->getViewUp();
	VRC_u = VRC_v.cross(VRC_n);
	// init auxiliary mapping variables
	I_w = Math::inverse<REAL>(REAL(W));
	I_h = Math::inverse<REAL>(REAL(H));

	REAL height = camera->windowHeight();

	W >= H ? V_w = (V_h = height) * W * I_h : V_h = (V_w = height) * H * I_w;
	if (isAdaptative)
		adaptativeScan(image);
	else
		scan(image);
	printf("\nNumber of rays: %lu", numberOfRays);
	printf("\nNumber of hits: %lu", numberOfHits);
	printElapsedTime("\nDONE! ", clock() - t);
}

static Ray pixelRay;

inline vec3
VRC_point(REAL x, REAL y)
{
	return V_w * (x * I_w - 0.5f) * VRC_u + V_h * (y * I_h - 0.5f) * VRC_v;
}

void
RayTracer::setPixelRay(REAL x, REAL y)
//[]---------------------------------------------------[]
//|  Set pixel ray                                      |
//|  @param x coordinate of the pixel                   |
//|  @param y cordinates of the pixel                   |
//[]---------------------------------------------------[]
{
	vec3 p = VRC_point(x, y);

	switch (camera->getProjectionType())
	{
	case Camera::Perspective:
		pixelRay.direction = (p - camera->getDistance() * VRC_n).versor();
		break;

	case Camera::Parallel:
		pixelRay.origin = camera->getPosition() + p;
		break;
	}
}

void
RayTracer::clearVisitedMatrix(int rows, int cols)
{
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			visited[i][j] = Coordinate(-99999, -99999);
}

void
RayTracer::printMatrix(int columns, int lines)
{
	for (int i = 0; i < lines; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			printf("%d %d", this->visited[j][i].x, this->visited[j][i].y);
		}
		printf("\n");
	}
	printf("\n");
}

void
RayTracer::adaptativeScan(Image& image)
//[]---------------------------------------------------[]
//|  Adaptative scan for aliasing problem               |
//[]---------------------------------------------------[]
{
	// init pixel ray
	pixelRay = Ray(camera->getPosition(), -VRC_n);
	numberOfRays = numberOfHits = 0;

	Pixel* pixels = new Pixel[W];

	for (int j = 0; j < H; j++)
	{

		printf("Scanning line %d of %d\r", j, H);
		// matrix of visited points ... initializing
		visited = new Coordinate*[W * 5];
		for (int i = 0; i < W * 5; i++)
			visited[i] = new Coordinate[5];

		clearVisitedMatrix(W * 5, 5);

		// agregating points of the line border above
		if (j>0)
		{
			for (int p = 0; p < W * 5; p++)
				visited[p][0] = border[p];
			delete[] border;
		}

		for (int i = 0; i < W; i++){
			pixels[i] = subDivision(i, j, 1.0, 0);
		}

		// storing the border
		if (j < H-1)
		{
			border = new Coordinate[W * 5];

			// saving the border (below)
			for (int p = 0; p < W * 5; p++)
				border[p] = visited[p][4];
		}

		delete[]visited;
		image.write(j, pixels);
	}
	delete[]pixels;

}

Color
RayTracer::checkVisitedPoints(Color& color, double i, double j)
{
	double fracpart_i, fracpart_j, intpart_i, intpart_j;
	int matrixCoordinate_i, matrixCoordinate_j;
	matrixCoordinate_i = matrixCoordinate_j = 0;

	fracpart_i = modf(i, &intpart_i);
	fracpart_j = modf(j, &intpart_j);

	// mapping coordinates of VRC to coordinates of matrix
	if (intpart_i == 0.0)
		matrixCoordinate_i = abs(0 + (5 * (i - 1)));
	else if (fracpart_i == 0.25)
		matrixCoordinate_i = abs(1 + (5 * (i - 1)));
	else if (fracpart_i == 0.50)
		matrixCoordinate_i = abs(2 + (5 * (i - 1)));
	else if (fracpart_i == 0.75)
		matrixCoordinate_i = abs(3 + (5 * (i - 1)));
	else if (intpart_i >= 1.0){
		matrixCoordinate_i = abs(4 + (5 * (i - 1)));
	}

	if (intpart_j == 0.0)
		matrixCoordinate_j = 0;
	else if (fracpart_j == 0.25)
		matrixCoordinate_j = 1;
	else if (fracpart_j == 0.50)
		matrixCoordinate_j = 2;
	else if (fracpart_j == 0.75)
		matrixCoordinate_j = 3;
	else if (intpart_j >= 1.0)
		matrixCoordinate_j = 4;


	// already visited (exclamation point)
	if (visited[matrixCoordinate_i][matrixCoordinate_j].x != -99999){
		color = visited[matrixCoordinate_i][matrixCoordinate_j].color;
	}
	else{
		visited[matrixCoordinate_i][matrixCoordinate_j].x = i;
		visited[matrixCoordinate_i][matrixCoordinate_j].y = j;
		color = shoot(i, j);
		visited[matrixCoordinate_i][matrixCoordinate_j].color = color;
	}

	return color;
}

Color
RayTracer::subDivision(int i, int j, REAL sub, int level)
{
	if (level <= 3){
		Color topLeft;
		Color topRight;
		Color bottomLeft;
		Color bottomRight;

		topLeft = checkVisitedPoints(topLeft, i, j);
		topRight = checkVisitedPoints(topRight, i + sub, j);
		bottomLeft = checkVisitedPoints(bottomLeft, i, j + sub);
		bottomRight = checkVisitedPoints(bottomRight, i + sub, j + sub);

		// computig mean
		Color meanColor = (topLeft + topRight + bottomLeft + bottomRight);
		meanColor = Color(meanColor.r / 4, meanColor.g / 4, meanColor.b / 4);

		// checking if the value of colors get distance of the mean
		Color topLeftDiff = meanColor - topLeft;
		Color topRightDiff = meanColor - topRight;
		Color bottomLeftDiff = meanColor - bottomLeft;
		Color bottomRightDiff = meanColor - bottomRight;

		// cheking threshold
		if (std::max(std::max(fabs(topLeftDiff.r), fabs(topLeftDiff.g)), fabs(topLeftDiff.b)) < ADAPT_DISTANCE &&
			std::max(std::max(fabs(topRightDiff.r), fabs(topRightDiff.g)), fabs(topRightDiff.b)) < ADAPT_DISTANCE &&
			std::max(std::max(fabs(bottomLeftDiff.r), fabs(bottomLeftDiff.g)), fabs(bottomLeftDiff.b)) < ADAPT_DISTANCE &&
			std::max(std::max(fabs(bottomRightDiff.r), fabs(bottomRightDiff.g)), fabs(bottomRightDiff.b)) < ADAPT_DISTANCE){
			return meanColor;
		}

		else{
			// sum (Ci) / 4

			Color res = (subDivision(i, j, sub / 2, level + 1) + subDivision(i + (sub / 2), j, sub / 2, level + 1)
				+ subDivision(i, j + (sub / 2), sub / 2, level + 1) + subDivision(i + (sub / 2), j + (sub / 2), sub / 2, level + 1));

			res.r = res.r / 4;
			res.g = res.g / 4;
			res.b = res.b / 4;

			return res;
		}
	}
	else{
		return shoot(i, j);
	}
}

void
RayTracer::scan(Image& image)
//[]---------------------------------------------------[]
//|  Basic scan with optional jitter                    |
//[]---------------------------------------------------[]
{
	// init pixel ray
	pixelRay = Ray(camera->getPosition(), -VRC_n);
	numberOfRays = numberOfHits = 0;

	Pixel* pixels = new Pixel[W];

	for (int j = 0; j < H; j++)
	{
		REAL y = j + 0.5f;

		printf("Scanning line %d of %d\r", j + 1, H);
		for (int i = 0; i < W; i++)
			pixels[i] = shoot(i + 0.5f, y);
		image.write(j, pixels);
	}
	delete[]pixels;
}

Color
RayTracer::shoot(REAL x, REAL y)
//[]---------------------------------------------------[]
//|  Shoot a pixel ray                                  |
//|  @param x coordinate of the pixel                   |
//|  @param y cordinates of the pixel                   |
//|  @return RGB color of the pixel                     |
//[]---------------------------------------------------[]
{
	// set pixel ray
	setPixelRay(x, y);

	// trace pixel ray
	Color color = trace(pixelRay, 0, 1.0f);

	// adjust RGB color
	if (color.r > 1.0f)
		color.r = 1.0f;
	if (color.g > 1.0f)
		color.g = 1.0f;
	if (color.b > 1.0f)
		color.b = 1.0f;
	// return pixel color
	return color;
}

Color
RayTracer::trace(const Ray& ray, uint level, REAL weight)
//[]---------------------------------------------------[]
//|  Trace a ray                                        |
//|  @param the ray                                     |
//|  @param recursion level                             |
//|  @param ray weight                                  |
//|  @return color of the ray                           |
//[]---------------------------------------------------[]
{
	// limiar
	if (weight <= getMinWeight() || level > getMaxRecursionLevel())
		return Color::black;

	else
		return shade(ray, level, weight);

	return Color::black;
}

Color
RayTracer::shade(const Ray& ray, uint level, REAL weight)
{
	Intersection inter_;
	numberOfRays++;
	// if the pixel ray intersect some actor in scene
	if (aggregate->intersect(ray, inter_))
	{
		numberOfHits++;
		// default color
		Color r_(0, 0, 0);

		// treating the precision problem
		inter_.p = inter_.p + 0.01 * inter_.triangle->normal(inter_);

		// getting the array iterator of lights in scene
		LightIterator lit = scene->getLightIterator();

		while (lit.current() != 0)
		{
			vec3 L;

			// current light
			Light* light = lit.current();

			// obtaining the light attr 
			if (light->isDirectional())
				L = light->position.versor();
			// not directional light ..
			else
				// ray direction from light position to pixel ray intersection point
				L = (inter_.p - light->position).versor();

			Ray shadowR(inter_.p, -L);
			Intersection shadowRayInter;

			// Now, lets see if the shadow ray intersect another actor in scene
			// cos, in this case, the color of the material at point inter.p will
			// be black
			if (!aggregate->intersect(shadowR, shadowRayInter))
			{
				Color difuseColor = inter_.object->getMaterial()->surface.diffuse;
				vec3 normal = inter_.triangle->normal(inter_);

				if ((normal.negate()).dot(L) > 0.0)
					r_ += difuseColor * (normal.negate()).dot(L); // updating the color 

			}
			lit++;
		}

		// reflection color
		Color Or;

		Or = inter_.object->getMaterial()->surface.specular;

		// verifying if is necessary trace reflection ray
		if (Or.r != 0.0 && Or.g != 0.0 && Or.b != 0.0)
		{
			// N
			vec3 normalAtP = inter_.triangle->normal(inter_);
			// Rr = (V - (2 * (N*V))N
			vec3 directionOfReflection = (ray.direction - (2 * normalAtP.dot(ray.direction)) * normalAtP).versor();

			Ray reflectionRay(inter_.p, directionOfReflection, 0.0001f);

			// getting the highest component value
			float highestComponent = std::max(std::max(Or.r, Or.g), Or.b);

			// recursively find the color
			r_ += Or * trace(reflectionRay, level + 1, weight * highestComponent);
		}

		return inter_.object->getMaterial()->surface.ambient * scene->ambientLight + r_;
	}
	else
		return scene->backgroundColor;
}