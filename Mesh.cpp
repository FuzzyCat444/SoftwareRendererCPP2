#include "Mesh.hpp"

Vertex::Vertex()
	: Vertex{ {}, {}, {} }
{
}

Vertex::Vertex(Vector3 xyz, Vector3 rgb, Vector2 uv)
	: Vertex{ xyz, rgb, uv, Vector3{ 0.0, 0.0, 0.0 } }
{
}

Vertex::Vertex(Vector3 xyz, Vector3 rgb, Vector2 uv, Vector3 normal)
	:  xyz{ xyz }, rgb{ rgb }, uv{ uv }, normal{ normal }
{
}

Triangle::Triangle() {}

Triangle::Triangle(int v0, int v1, int v2)
	: v0{ v0 }, v1{ v1 }, v2{ v2 }
{
}

WavefrontIndices::WavefrontIndices() {}

WavefrontIndices::WavefrontIndices(int v, int vt, int vn)
    : v{ v }, vt{ vt }, vn{ vn }
{
}

bool WavefrontIndices::operator==(const WavefrontIndices& wi) const
{
    return v == wi.v && vt == wi.vt && vn == wi.vn;
}

std::size_t WavefrontIndices::Hash::operator()(const WavefrontIndices& wi) const
{
    int hash = wi.v;
    hash = 3 * hash + wi.vt;
    hash = 3 * hash + wi.vn;
    return hash;
}

Mesh::Mesh() {}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Triangle> triangles, Shading shading)
{
	this->vertices = vertices;
	this->triangles = triangles;
	computeNormals(shading);
}

void Mesh::invertNormals()
{
    for (int i = 0; i < vertices.size(); i++)
        vertices.at(i).normal.scl(-1.0);

    for (int i = 0; i < faceNormals.size(); i++)
        faceNormals.at(i).scl(-1.0);
}

const std::vector<Vertex>& Mesh::getVertices() const
{
	return vertices;
}

const std::vector<Triangle>& Mesh::getTriangles() const
{
	return triangles;
}

const std::vector<Vector3>& Mesh::getFaceNormals() const
{
	return faceNormals;
}

void Mesh::computeNormals(Shading shading)
{
    for (int i = 0; i < triangles.size(); i++)
    {
        Triangle tri = triangles.at(i);
        Vector3 p0 = vertices.at(tri.v0).xyz;
        Vector3 p1 = vertices.at(tri.v1).xyz;
        Vector3 p2 = vertices.at(tri.v2).xyz;

        p1.sub(p0);
        p2.sub(p0);
        Vector3 normal = p1.cross(p2);
        normal.norm();
        faceNormals.push_back(normal);
    }

	switch (shading)
	{
    case Shading::KEEP_NORMALS:
        for (int i = 0; i < vertices.size(); i++)
            vertices.at(i).normal.norm();
        break;
	case Shading::MAKE_FLAT:
	{
		std::vector<Vertex> newVertices;
		std::vector<Triangle> newTriangles;
		int vIndex = 0;
		for (int i = 0; i < triangles.size(); i++)
		{
			Triangle tri = triangles.at(i);
			Vertex v0 = vertices.at(tri.v0);
			Vertex v1 = vertices.at(tri.v1);
			Vertex v2 = vertices.at(tri.v2);
			Vector3 normal = faceNormals.at(i);

			v0.normal = normal;
			v1.normal = normal;
			v2.normal = normal;
			newVertices.push_back(v0);
			newVertices.push_back(v1);
			newVertices.push_back(v2);
			Triangle newTri{ vIndex + 0, vIndex + 1, vIndex + 2 };
			newTriangles.push_back(newTri);

			vIndex += 3;
		}
		vertices = newVertices;
		triangles = newTriangles;
		break;
	}
	}
}

Mesh Mesh::loadFromFile(std::string objFile, Shading shading)
{
    std::vector<Vector3> v;
    std::vector<Vector2> vt;
    std::vector<Vector3> vn;
    std::vector<WavefrontIndices> wavefrontIndices;
    std::ifstream input{ objFile };
    for (std::string line; getline(input, line);)
    {
        std::istringstream iss{ line };
        std::string tag;
        iss >> tag;
        if (tag == "v")
        {
            double x, y, z;
            iss >> x >> y >> z;
            v.push_back(Vector3{ x, y, z });
        }
        else if (tag == "vt")
        {
            double u, v;
            iss >> u >> v;
            vt.push_back(Vector2{ u, v });
        }
        else if (tag == "vn")
        {
            double x, y, z;
            iss >> x >> y >> z;
            vn.push_back(Vector3{ x, y, z });
        }
        else if (tag == "f")
        {
            auto setIndices = [](std::string indicesString, WavefrontIndices& wi)
            {
                int slash = indicesString.find("/");
                std::string num0 = indicesString.substr(0, slash);
                indicesString = indicesString.substr(slash + 1);
                slash = indicesString.find("/");
                std::string num1 = indicesString.substr(0, slash);
                std::string num2 = indicesString.substr(slash + 1);

                wi.v = std::stoi(num0) - 1;
                wi.vt = std::stoi(num1) - 1;
                wi.vn = std::stoi(num2) - 1;
            };
            std::string indices0, indices1, indices2;
            iss >> indices0 >> indices1 >> indices2;
            WavefrontIndices wi0, wi1, wi2;
            setIndices(indices0, wi0);
            setIndices(indices1, wi1);
            setIndices(indices2, wi2);
            wavefrontIndices.push_back(wi0);
            wavefrontIndices.push_back(wi1);
            wavefrontIndices.push_back(wi2);
        }
    }

    std::unordered_map<WavefrontIndices, int, WavefrontIndices::Hash> indexMap;

    int index = 0;
    for (int i = 0; i < wavefrontIndices.size(); i++)
    {
        WavefrontIndices wi = wavefrontIndices.at(i);
        auto found = indexMap.find(wi);
        if (found == indexMap.end())
            indexMap.insert(std::make_pair(wi, index++));
    }

    std::vector<Vertex> vertices;
    vertices.resize(indexMap.size());
    for (auto it = indexMap.begin(); it != indexMap.end(); it++)
    {
        WavefrontIndices wi = it->first;
        vertices.at(it->second) = Vertex{ v.at(wi.v), Vector3{ 1.0, 1.0, 1.0 }, vt.at(wi.vt), vn.at(wi.vn) };
    }

    std::vector<Triangle> triangles;
    for (int i = 0; i < wavefrontIndices.size(); i += 3)
    {
        WavefrontIndices wi0 = wavefrontIndices.at(i);
        WavefrontIndices wi1 = wavefrontIndices.at(i + 1);
        WavefrontIndices wi2 = wavefrontIndices.at(i + 2);
        triangles.push_back(Triangle{ indexMap.at(wi0), indexMap.at(wi1), indexMap.at(wi2) });
    }

    return Mesh{ vertices, triangles, shading };
}

Mesh Mesh::generateUVSphere(int rings, int segments, Shading shading)
{
    std::vector<Vertex> verts;
    std::vector<Triangle> tris;

    Vertex top{ Vector3{ 0.0, 1.0, 0.0 }, Vector3{ 1.0, 1.0, 1.0 }, Vector2{ 0.5, 1.0 }, Vector3{ 0.0, 1.0, 0.0 } };
    verts.push_back(top);
    Vertex bottom{ Vector3{ 0.0, -1.0, 0.0 }, Vector3{ 1.0, 1.0, 1.0 }, Vector2{ 0.5, 0.0 }, Vector3{ 0.0, -1.0, 0.0 } };
    verts.push_back(bottom);

    double ringAngleInc = radians(175.0) / (rings + 1);
    double segmentAngleInc = radians(360.0) / segments;
    double ringAngle = radians(2.5);
    double vInc = -1.0 / (rings + 1);
    double uInc = -1.0 / segments;
    double v = 1.0 + vInc;
    for (int r = 0; r < rings; r++)
    {
        double ringRadius = sin(ringAngle);
        double y = cos(ringAngle);

        double segmentAngle = 0.0;
        double u = 1.0;
        for (int s = 0; s < segments + 1; s++)
        {
            double x = cos(segmentAngle) * ringRadius;
            double z = sin(segmentAngle) * ringRadius;
            Vertex vt{ Vector3{ x, y, z }, Vector3{ 1.0, 1.0, 1.0 }, Vector2{ u, v }, Vector3{ x, y, z } };
            verts.push_back(vt);

            segmentAngle += segmentAngleInc;
            u += uInc;
        }
        ringAngle += ringAngleInc;
        v += vInc;
    }

    int ring0Index = 2;
    int ring1Index = ring0Index + segments + 1;
    for (int r = 0; r < rings - 1; r++)
    {
        for (int s = 0; s < segments; s++)
        {
            Triangle t0{ s + ring0Index, s + ring0Index + 1, s + ring1Index + 1 };
            Triangle t1{ s + ring1Index + 1, s + ring1Index, s + ring0Index };
            tris.push_back(t0);
            tris.push_back(t1);
        }
        ring0Index = ring1Index;
        ring1Index += segments + 1;
    }

    for (int s = 0; s < segments; s++)
    {
        Triangle top{ 0, s + 2 + 1, s + 2 };
        Triangle bottom{ 1, ring0Index + s, ring0Index + s + 1 };
        tris.push_back(top);
        tris.push_back(bottom);
    }

    return Mesh{ verts, tris, shading };
}
