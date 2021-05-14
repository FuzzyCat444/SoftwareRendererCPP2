#ifndef MESH_HPP
#define MESH_HPP

#include "Math.hpp"

#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include <utility>

struct Vertex
{
	Vertex();
	Vertex(Vector3 xyz, Vector3 rgb, Vector2 uv);
	Vertex(Vector3 xyz, Vector3 rgb, Vector2 uv, Vector3 normal);

	Vector3 xyz;
	Vector3 rgb;
	Vector2 uv;
	Vector3 normal;
};

struct Triangle
{
    Triangle();
	Triangle(int v0, int v1, int v2);

	int v0, v1, v2;
};

struct WavefrontIndices
{
    WavefrontIndices();
	WavefrontIndices(int v, int vt, int vn);
    bool operator==(const WavefrontIndices& wi) const;
    struct Hash
    {
        std::size_t operator()(const WavefrontIndices& wi) const;
    };

	int v, vt, vn;
};

class Mesh
{
public:
	enum class Shading
	{
		KEEP_NORMALS, MAKE_FLAT
	};

	Mesh();
	Mesh(std::vector<Vertex> vertices, std::vector<Triangle> triangles, Shading shading);

	void invertNormals();

	const std::vector<Vertex>& getVertices() const;
	const std::vector<Triangle>& getTriangles() const;
	const std::vector<Vector3>& getFaceNormals() const;

	static Mesh loadFromFile(std::string objFile, Shading shading);
	static Mesh generateUVSphere(int rings, int segments, Shading shading);
private:
	void computeNormals(Shading shading);

	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;
	std::vector<Vector3> faceNormals;
};

#endif

