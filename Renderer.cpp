#include "Renderer.hpp"

Renderer::Renderer(Raster* image)
	: image{ image }
{
	clearDepth();
}

void Renderer::clearColor(Color color) //clears to black
{
	image->clear();
}

void Renderer::clearDepth()
{
	depth.assign(image->getWidth() * image->getHeight(), 1.0e100);
}

void Renderer::clearColorDepth(Color color)
{
	clearColor(color);
	clearDepth();
}

void Renderer::renderMesh(Mesh& mesh, const Raster& texture, const Transform& transform, const Camera& camera, const std::vector<LightSource>& lights, Lighting lighting)
{
	const std::vector<Vertex>& vertices = mesh.getVertices();
	const std::vector<Triangle>& triangles = mesh.getTriangles();
	const std::vector<Vector3>& faceNormals = mesh.getFaceNormals();

	if (verticesCopy.size() < vertices.size())
        verticesCopy.resize(vertices.size());
    if (renderFace.size() < triangles.size())
        renderFace.resize(triangles.size());

	// Model transform (and lighting calculations)
	for (int i = 0; i < vertices.size(); i++)
	{
		Vertex v = vertices.at(i);
		v.xyz = transform.apply(v.xyz);
		v.normal = transform.applyNormal(v.normal);
		v.normal.norm();

        if (lighting == Lighting::DIFFUSE)
        {
            Vector3 lightAccum;
            for (int j = 0; j < lights.size(); j++)
            {
                LightSource light = lights.at(j);
                switch (light.type)
                {
                case LightType::POINT:
                {
                    Vector3 vertexToLight{ light.point.position };
                    vertexToLight.sub(v.xyz);
                    double dot = vertexToLight.dot(v.normal);
                    double len = vertexToLight.len();
                    double brightness = fmax(0.0, dot / len);
                    double dim = fmax(0.0, 1.0 - len / light.point.attenuation);
                    Vector3 lightColor = light.point.color;
                    lightColor.scl(brightness * dim);
                    lightAccum.add(lightColor);
                    break;
                }
                case LightType::DIRECTIONAL:
                {
                    Vector3 dir = light.directional.direction;
                    dir.scl(-1.0);
                    double dot = dir.dot(v.normal);
                    double brightness = fmax(0.0, dot / dir.len());
                    Vector3 lightColor = light.directional.color;
                    lightColor.scl(brightness);
                    lightAccum.add(lightColor);
                    break;
                }
                case LightType::AMBIENT:
                {
                    Vector3 lightColor = light.ambient.color;
                    lightAccum.add(lightColor);
                    break;
                }
                }
            }
            v.rgb.mul(lightAccum);
        }

		verticesCopy.at(i) = v;
	}

	// Backface culling
	for (int i = 0; i < triangles.size(); i++)
	{
		Vector3 p0 = verticesCopy.at(triangles.at(i).v0).xyz;
		Vector3 view = camera.getPosition();
		Vector3 diff = view;
		diff.sub(p0);
		Vector3 normal = transform.applyNormal(faceNormals.at(i));
		normal.norm();
		renderFace.at(i) = diff.dot(normal) > 0.0;
	}

	// View transform
	for (int i = 0; i < vertices.size(); i++)
	{
		Vertex v = verticesCopy.at(i);
		v.xyz = camera.getTransform().apply(v.xyz);
		verticesCopy.at(i) = v;
	}

    // Triangle recursive clipping and rasterization
	for (int i = 0; i < triangles.size(); i++)
	{
		Triangle tri = triangles.at(i);
		Vertex v0 = verticesCopy.at(tri.v0);
		Vertex v1 = verticesCopy.at(tri.v1);
		Vertex v2 = verticesCopy.at(tri.v2);

		if (renderFace.at(i))
			doTriangle(v0, v1, v2, ClipPlane::NEAR, texture, camera);
	}
}

Renderer::TriangleClip Renderer::clipTriangle(Vertex v0, Vertex v1, Vertex v2, ClipPlane plane, const Camera& camera) const
{
	EdgeClip v0v1 = clipEdge(v0, v1, plane, camera);
	EdgeClip v1v2 = clipEdge(v1, v2, plane, camera);
	EdgeClip v2v0 = clipEdge(v2, v0, plane, camera);

	if (v0v1.result == EdgeClip::ClipResult::REMOVED
		&& v1v2.result == EdgeClip::ClipResult::REMOVED
		&& v2v0.result == EdgeClip::ClipResult::REMOVED)
		return TriangleClip{ TriangleClip::ClipResult::REMOVED, {}, {} };

	if (v0v1.result == EdgeClip::ClipResult::KEPT
		&& v1v2.result == EdgeClip::ClipResult::KEPT
		&& v2v0.result == EdgeClip::ClipResult::KEPT)
		return TriangleClip{ TriangleClip::ClipResult::KEPT, { v0, v1, v2 }, {} };

	if (v0v1.result == EdgeClip::ClipResult::CLIPPED)
	{
		std::swap(v0v1, v1v2);
		std::swap(v0, v2);
	}
	if (v0v1.result == EdgeClip::ClipResult::CLIPPED)
	{
		std::swap(v0v1, v2v0);
		std::swap(v1, v2);
	}

	if (v0v1.result == EdgeClip::ClipResult::REMOVED)
		return TriangleClip{ TriangleClip::ClipResult::CLIPPED_ONE, { v1v2.v, v2v0.v, v2 }, {} };
	else
		return TriangleClip{ TriangleClip::ClipResult::CLIPPED_TWO, { v1v2.v, v2v0.v, v0 }, { v1v2.v, v0, v1 } };

	return TriangleClip{ TriangleClip::ClipResult::REMOVED, {}, {} };
}

void Renderer::doTriangle(Vertex v0, Vertex v1, Vertex v2, ClipPlane plane, const Raster& texture, const Camera& camera)
{
	ClipPlane nextPlane = nextClipPlane(plane);
	if (plane == ClipPlane::NONE)
		rasterizeTriangle(v0, v1, v2, texture);
	else
	{
		if (plane == ClipPlane::LEFT)
		{
			v0 = applyPerspective(v0, texture, camera);
			v1 = applyPerspective(v1, texture, camera);
			v2 = applyPerspective(v2, texture, camera);
		}
		TriangleClip clip = clipTriangle(v0, v1, v2, plane, camera);
		switch (clip.result)
		{
		case TriangleClip::ClipResult::CLIPPED_ONE:
			doTriangle(clip.t0.v0, clip.t0.v1, clip.t0.v2, nextPlane, texture, camera);
			break;
		case TriangleClip::ClipResult::CLIPPED_TWO:
			doTriangle(clip.t0.v0, clip.t0.v1, clip.t0.v2, nextPlane, texture, camera);
			doTriangle(clip.t1.v0, clip.t1.v1, clip.t1.v2, nextPlane, texture, camera);
			break;
		case TriangleClip::ClipResult::KEPT:
			doTriangle(v0, v1, v2, nextPlane, texture, camera);
			break;
		default:
			break;
		}
	}
}

Vertex Renderer::applyPerspective(Vertex v, const Raster& texture, const Camera& camera)
{
	double oneOverZ = 1.0 / (camera.getPerspective() * -v.xyz.z);
	v.xyz.x *= oneOverZ;
	v.xyz.y *= oneOverZ * camera.getAspect();
	v.xyz.z = oneOverZ;
	v.rgb.scl(oneOverZ);
	v.uv.y = 1.0 - v.uv.y;
	v.uv.mul(Vector2{ (double) texture.getWidth(), (double) texture.getHeight() });
	v.uv.scl(oneOverZ);
	return v;
}

void Renderer::rasterizeTriangle(Vertex v0, Vertex v1, Vertex v2, const Raster& texture)
{
	auto toScreenSpace = [this, &texture](Vertex& vertex)
	{
		vertex.xyz.x = image->getWidth() * 0.5 * (1.0 + vertex.xyz.x);
		vertex.xyz.y = image->getHeight() * 0.5 * (1.0 - vertex.xyz.y);
	};
	toScreenSpace(v0);
	toScreenSpace(v1);
	toScreenSpace(v2);

	if (v1.xyz.y < v0.xyz.y)
		std::swap(v0, v1);
	if (v2.xyz.y < v1.xyz.y)
		std::swap(v1, v2);
	if (v1.xyz.y < v0.xyz.y)
		std::swap(v0, v1);

	double t = getT(v0.xyz.y, v2.xyz.y, v1.xyz.y);
	LinearInterpolate lin{ v0, v2, t, 0.0 };
	Vertex v1l = lin.value;
	Vertex v1r = v1;
	if (v1r.xyz.x < v1l.xyz.x)
		std::swap(v1l, v1r);

	int yPixelStart;
	int yPixelEnd;
	double yDifference;
	double yTInc;
	double yStartT;
	LinearInterpolate leftEdge;
	LinearInterpolate rightEdge;

	auto scanline = [this, &texture](LinearInterpolate& leftEdge, LinearInterpolate& rightEdge, int y)
	{
		Vertex& lv = leftEdge.value;
		Vertex& rv = rightEdge.value;
		int xPixelStart = (int) floor(lv.xyz.x + 0.5);
		int xPixelEnd = (int) floor(rv.xyz.x - 0.5);
		double xDifference = rv.xyz.x - lv.xyz.x;
		double xTInc = 1.0 / xDifference;
		double xStartT = (xPixelStart + 0.5 - lv.xyz.x) * xTInc;
		LinearInterpolate scanline{ lv, rv, xStartT, xTInc };
		Vertex& v = scanline.value;

		int pixelIndex = image->getIndex(xPixelStart, y);
		int depthIndex = pixelIndex >> 2;
		for (int x = xPixelStart; x <= xPixelEnd; x++)
		{
			double z = 1.0 / v.xyz.z;
			Vector3 rgb = v.rgb;
			rgb.scl(z);
			Vector2 uv = v.uv;
			uv.scl(z);

			Color pixel = texture.getPixel((int) uv.x, (int) uv.y);
			pixel.r *= rgb.x;
			pixel.g *= rgb.y;
			pixel.b *= rgb.z;
			pixel.limit();

			if (pixel.a > 0 && testDepth(depthIndex, z))
				image->setPixel(pixelIndex, pixel);

			scanline.step();
			pixelIndex += 4;
			depthIndex++;
		}
		leftEdge.step();
		rightEdge.step();
	};

	// Top half
	yPixelStart = (int) floor(v0.xyz.y + 0.5);
	yPixelEnd = (int) floor(v1.xyz.y - 0.5);
	yDifference = v1.xyz.y - v0.xyz.y;
	yTInc = 1.0 / yDifference;
	yStartT = (yPixelStart + 0.5 - v0.xyz.y) * yTInc;
	leftEdge = LinearInterpolate{ v0, v1l, yStartT, yTInc };
	rightEdge = LinearInterpolate{ v0, v1r, yStartT, yTInc };
	for (int y = yPixelStart; y <= yPixelEnd; y++)
		scanline(leftEdge, rightEdge, y);

	// Bottom half
	yPixelStart = (int) floor(v2.xyz.y - 0.5);
	yPixelEnd = (int) floor(v1.xyz.y + 0.5);
	yDifference = v2.xyz.y - v1.xyz.y;
	yTInc = 1.0 / yDifference;
	yStartT = (v2.xyz.y - (yPixelStart + 0.5)) * yTInc;
	leftEdge = LinearInterpolate{ v2, v1l, yStartT, yTInc };
	rightEdge = LinearInterpolate{ v2, v1r, yStartT, yTInc };
	for (int y = yPixelStart; y >= yPixelEnd; y--)
		scanline(leftEdge, rightEdge, y);
}

Renderer::EdgeClip Renderer::clipEdge(Vertex v0, Vertex v1, ClipPlane plane, const Camera& camera) const
{
	auto sortClip = [&v0, &v1](double a, double b, double c, bool flipped)
	{
		if (b < a)
		{
			std::swap(a, b);
			std::swap(v0, v1);
		}
		if (flipped)
		{
			if (b < c)
				return EdgeClip{ EdgeClip::ClipResult::KEPT, {} };
			if (a > c)
				return EdgeClip{ EdgeClip::ClipResult::REMOVED, {} };
		}
		else
		{
			if (a > c)
				return EdgeClip{ EdgeClip::ClipResult::KEPT, {} };
			if (b < c)
				return EdgeClip{ EdgeClip::ClipResult::REMOVED, {} };
		}

		LinearInterpolate lin;
		double t = getT(a, b, c);
		lin = LinearInterpolate{ v0, v1, t, 0.0 };

		return EdgeClip{ EdgeClip::ClipResult::CLIPPED, lin.value };
};

	switch (plane)
	{
	case ClipPlane::NEAR:
		return sortClip(v0.xyz.z, v1.xyz.z, -camera.getNearClip(), true);
	case ClipPlane::LEFT:
		return sortClip(v0.xyz.x, v1.xyz.x, -1.0, false);
	case ClipPlane::RIGHT:
		return sortClip(v0.xyz.x, v1.xyz.x, 1.0, true);
	case ClipPlane::BOTTOM:
		return sortClip(v0.xyz.y, v1.xyz.y, -1.0, false);
	case ClipPlane::TOP:
		return sortClip(v0.xyz.y, v1.xyz.y, 1.0, true);
	case ClipPlane::NONE:
		return { EdgeClip::ClipResult::KEPT, {} };
	}
}

Renderer::ClipPlane Renderer::nextClipPlane(ClipPlane plane)
{
	switch (plane)
	{
	case ClipPlane::NEAR:
		return ClipPlane::LEFT;
	case ClipPlane::LEFT:
		return ClipPlane::RIGHT;
	case ClipPlane::RIGHT:
		return ClipPlane::BOTTOM;
	case ClipPlane::BOTTOM:
		return ClipPlane::TOP;
	case ClipPlane::TOP:
		return ClipPlane::NONE;
	default:
		return ClipPlane::NONE;
	}
}
