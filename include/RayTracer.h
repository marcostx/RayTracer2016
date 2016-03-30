#ifndef __RayTracer_h
#define __RayTracer_h

//[]------------------------------------------------------------------------[]
//|                                                                          |
//|                          GVSG Graphics Library                           |
//|                               Version 1.0                                |
//|                                                                          |
//|              Copyright  2007-2016, Paulo Aristarco Pagliosa              |
//|              All Rights Reserved.                                        |
//|                                                                          |
//[]------------------------------------------------------------------------[]
//
//  OVERVIEW: RayTracer.h
//  ========
//  Class definition for simple ray tracer.

#include "Image.h"
#include "Intersection.h"
#include "Renderer.h"

namespace Graphics
{ // begin namespace Graphics

#define MIN_WEIGHT REAL(0.01)
#define MAX_RECURSION_LEVEL 6
#define ADAPT_DISTANCE 0.06


	// struct to design Coordinates
	struct Coordinate
	{
	public:
		double x, y;
		Color color;

		Coordinate(){}
		Coordinate(double paramx, double paramy) : x(paramx), y(paramy) {}
		Coordinate(double paramx, double paramy, Color cC) : x(paramx), y(paramy), color(cC){}
	};

	//////////////////////////////////////////////////////////
	//
	// RayTracer: simple ray tracer class
	// =========
	class RayTracer : public Renderer
	{
	public:
		struct DebugInfo
		{
			Ray ray;
			Intersection hit;

		};
		Coordinate**  visited;
		Coordinate* border;
		// Constructor
		RayTracer(Scene&, Camera* = 0);

		uint getMaxRecursionLevel() const
		{
			return maxRecursionLevel;
		}

		REAL getMinWeight() const
		{
			return minWeight;
		}

		void setMaxRecursionLevel(uint rl)
		{
			maxRecursionLevel = dMin<uint>(rl, MAX_RECURSION_LEVEL);
		}

		void setMinWeight(REAL w)
		{
			minWeight = dMax<REAL>(w, MIN_WEIGHT);
		}

		void render();
		virtual void renderImage(Image&, bool);

		void debug(int, int, DebugInfo&);

	protected:
		ObjectPtr<Model> aggregate;
		uint maxRecursionLevel;
		REAL minWeight;

		virtual void scan(Image&);
		virtual void setPixelRay(REAL, REAL);
		virtual Color shoot(REAL, REAL);
		virtual void adaptativeScan(Image&);
		virtual Color trace(const Ray&, uint, REAL);
		virtual Color shade(const Ray&, uint, REAL);
		virtual Color subDivision(int, int, REAL, int);
		virtual Color checkVisitedPoints(Color&, double, double);
		virtual void clearVisitedMatrix(int, int);
		virtual void printMatrix(int, int);

	}; // RayTracer

} // end namespace Graphics

#endif // __RayTracer_h