#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "Raster.hpp"
#include "Mesh.hpp"
#include "Camera.hpp"
#include "Math.hpp"
#include "LightSource.hpp"

#include <functional>
#include <vector>
#include <utility>
#include <cmath>

class Renderer
{
public:
	Renderer(Raster* image);

	void clearColor(Color color);
	void clearDepth();
	void clearColorDepth(Color color);

	enum class Lighting
	{
	    NONE, DIFFUSE
	};

	void fogPostProcess(double fogStart, double fogEnd, Color fogColor);

	void enableDepthTest(bool enable);

	void renderMesh(Mesh& mesh, const Raster& texture, const Transform& transform, const Camera& camera, const std::vector<LightSource>& lights, Lighting lighting);
private:
	Raster* image;
	std::vector<double> depth;
	bool depthTestEnabled;

	std::vector<Vertex> verticesCopy;
	std::vector<bool> renderFace;

    bool testDepth(int index, double d)
    {
        if (index < 0 || index >= depth.size())
            return false;
        if (d < depth.at(index) || !depthTestEnabled)
        {
            depth.at(index) = d;
            return true;
        }
        return false;
    }

	enum class ClipPlane
	{
		NEAR,
		LEFT,
		RIGHT,
		BOTTOM,
		TOP,
		NONE
	};
	static ClipPlane nextClipPlane(ClipPlane plane);

	struct EdgeClip
	{
		enum class ClipResult
		{
			REMOVED, CLIPPED, KEPT
		} result;
		Vertex v;
	};
	EdgeClip clipEdge(Vertex v0, Vertex v1, ClipPlane plane, const Camera& camera) const;

	struct TriangleClip
	{
		enum class ClipResult
		{
			REMOVED, CLIPPED_ONE, CLIPPED_TWO, KEPT
		} result;
		struct
		{
			Vertex v0, v1, v2;
		} t0, t1;
	};
	TriangleClip clipTriangle(Vertex v0, Vertex v1, Vertex v2, ClipPlane plane, const Camera& camera) const;

	void doTriangle(Vertex v0, Vertex v1, Vertex v2, ClipPlane plane, const Raster& texture, const Camera& camera);

	Vertex applyPerspective(Vertex v, const Raster& texture, const Camera& camera);

	struct LinearInterpolate
	{
		LinearInterpolate() {}
        LinearInterpolate(Vertex v0, Vertex v1, double startT, double incT)
        {
            Vertex difference;

            difference.xyz.add(v1.xyz);
            difference.xyz.sub(v0.xyz);

            difference.rgb.add(v1.rgb);
            difference.rgb.sub(v0.rgb);

            difference.uv.add(v1.uv);
            difference.uv.sub(v0.uv);

            incValue = difference;
            incValue.xyz.scl(incT);
            incValue.rgb.scl(incT);
            incValue.uv.scl(incT);

            Vertex start{ difference };
            start.xyz.scl(startT);
            start.rgb.scl(startT);
            start.uv.scl(startT);
            start.xyz.add(v0.xyz);
            start.rgb.add(v0.rgb);
            start.uv.add(v0.uv);

            value = start;
        }

        void step()
        {
            value.xyz.add(incValue.xyz);
            value.rgb.add(incValue.rgb);
            value.uv.add(incValue.uv);
        }

		Vertex value;
	private:
		Vertex incValue;
	};

	void rasterizeTriangle(Vertex v0, Vertex v1, Vertex v2, const Raster& texture, const Camera& camera);
};

#endif
