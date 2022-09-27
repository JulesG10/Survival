#include "modelcol.h"

void DrawTriangles(std::vector<std::vector<Triangle>> tri, Color c)
{
    for (std::vector<Triangle> tl : tri)
    {
        for (Triangle t : tl)
        {
            //DrawTriangle3D(t.a, t.b, t.c, GREEN);

            DrawLine3D(t.a, t.b, c);
            DrawLine3D(t.b, t.c, c);
            DrawLine3D(t.c, t.a, c);
        }
    }
}

Vector3 MatrixMultiplyVector3(Vector3 a, Matrix transform)
{
    Vector3 out = { 0, 0, 0 };
    out.x = (transform.m0 * a.x + transform.m1 * a.y + transform.m2 * a.z) + transform.m3;
    out.y = (transform.m4 * a.x + transform.m5 * a.y + transform.m6 * a.z) + transform.m7;
    out.z = (transform.m8 * a.x + transform.m9 * a.y + transform.m10 * a.z) + transform.m11;

    /*
    * https://gist.github.com/justinmeiners/3d73741bd5e09e0b398a0342e5e22eac
    float num = 1.f / ((transform.m12 * a.x + transform.m13 * a.y + transform.m14 * a.z) + transform.m15);

    out.x *= num;
    out.y *= num;
    out.z *= num;
    */
    /*
    a.x = transform.m0 * a.x + transform.m1 * a.y + transform.m2 * a.z + transform.m3 * 1;
    a.y = transform.m4 * a.x + transform.m5 * a.y + transform.m6 * a.z + transform.m7 * 1;
    a.z = transform.m8 * a.x + transform.m9 * a.y + transform.m10 * a.z + transform.m11 * 1;
    Vector3Add(a, { transform.m12, transform.m13, transform.m14 });
    */

    return Vector3Add(out, { transform.m12, transform.m13, transform.m14 });;
}

std::vector<std::vector<Triangle>> GetTriangles(Model m, Vector3 position, Vector3 axis, float angle, Vector3 scale)
{
    angle -= angle * 2;

    Matrix matScale = MatrixScale(scale.x, scale.y, scale.z);
    Matrix matRotation = MatrixRotate(axis, angle * DEG2RAD);
    Matrix matTranslation = MatrixTranslate(position.x, position.y, position.z);

    Matrix transform = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);

    std::vector<std::vector<Triangle>> tri = {};

    for (size_t k = 0; k < m.meshCount; k++)
    {
        Mesh mesh = m.meshes[k];
        std::vector<Triangle> triangles = {};

        for (size_t i = 0; i < mesh.triangleCount; i++)
        {
            Vector3 a, b, c;
            Vector2 u1, u2, u3;
            Vector3* vertdata = (Vector3*)mesh.vertices;
            Vector2* uvdata = (Vector2*)mesh.texcoords;

            if (mesh.indices)
            {
                a = vertdata[mesh.indices[i * 3 + 0]];
                b = vertdata[mesh.indices[i * 3 + 1]];
                c = vertdata[mesh.indices[i * 3 + 2]];

                u1 = uvdata[mesh.indices[i * 3 + 0]];
                u2 = uvdata[mesh.indices[i * 3 + 1]];
                u3 = uvdata[mesh.indices[i * 3 + 2]];
            }
            else
            {
                a = vertdata[i * 3 + 0];
                b = vertdata[i * 3 + 1];
                c = vertdata[i * 3 + 2];

                u1 = uvdata[i * 3 + 0];
                u2 = uvdata[i * 3 + 1];
                u3 = uvdata[i * 3 + 2];
            }

            a = MatrixMultiplyVector3(a, transform);
            b = MatrixMultiplyVector3(b, transform);
            c = MatrixMultiplyVector3(c, transform);

            triangles.push_back({ a,b,c });
        }
        tri.push_back(triangles);
    }

    return tri;
}

void UpdateCollisionModel(CollisionModel* col)
{
    col->triangles = GetTriangles(col->model, col->position, col->axis, col->angle, col->scale);
}

CollisionModel CreateCollisionModel(Model m, Vector3 position, Vector3 axis, float angle, Vector3 scale)
{
    CollisionModel col = {0};
    col.model = m;

    col.angle = angle;
    col.axis = axis;

    col.position = position;

    col.scale = scale;

    UpdateCollisionModel(&col);

    return col;
}

bool ModelCollision(CollisionModel a, CollisionModel b)
{
    for (std::vector<Triangle> lista : a.triangles)
    {
        for (const Triangle& ta : lista)
        {
            for (std::vector<Triangle> listb : b.triangles)
            {
                for (const Triangle& tb : listb)
                {
                    if (CheckTrianglesCollision(ta, tb))
                    {
                        return true;
                    }
                }
            }
        }
    }
}


bool IsVecNull(Vector3* vec, size_t s)
{
    int v = 0;
    for (size_t i = 0; i < s; i++)
    {
        if ((vec[0].x + vec[0].y + vec[0].z) == 0)
        {
            return true;
        }
    }
    return false;
}

float LineLengthSquared(Vector3* line)
{
    float a = line[1].x - line[0].x;
    float b = line[1].y - line[0].y;
    float c = line[1].z - line[0].z;
    return a * a + b * b + c * c;
};

float Determinant3x3(
    float a1, float b1, float c1,
    float a2, float b2, float c2,
    float a3, float b3, float c3)
{
    return a1 * b2 * c3 - a1 * b3 * c2 + a3 * b1 * c2 - a2 * b1 * c3 + a2 * b3 * c1 - a3 * b2 * c1;
}

Vector3 CrossProduct(Vector3 a, Vector3 b)
{
    return  {
         a.y * b.z - a.z * b.y,
         a.z * b.x - a.x * b.z,
         a.x * b.y - a.y * b.x
    };
}


Quaternion TriangleToPlaneObjNormalized(Triangle t)
{
    Quaternion res;
    Vector3 a = {
        t.c.x - t.a.x,
        t.c.y - t.a.y,
        t.c.z - t.a.z
    };
    Vector3 b = {
        t.b.x - t.a.x,
        t.b.y - t.a.y,
        t.b.z - t.a.z
    };
    res.x = a.y * b.z - a.z * b.y;
    res.y = a.z * b.x - a.x * b.z;
    res.z = a.x * b.y - a.y * b.x;
    res.w = -res.x * t.a.x - res.y * t.a.y - res.z * t.a.z;
    return res;
}

Vector3* TrianglePointsCrossingNormalizedPlane(Triangle t, Quaternion p)
{
    float df0 = t.a.x * p.x + t.a.y * p.y + t.a.z * p.z + p.w;
    float df1 = t.b.x * p.x + t.b.y * p.y + t.b.z * p.z + p.w;
    float df2 = t.c.x * p.x + t.c.y * p.y + t.b.z * p.z + p.w;

    Vector3 res[2] = { {0}, {0} };
    if (df0 * df1 < 0.0) {
        float d0 = abs(df0);
        float d1 = abs(df1);
        float d2 = abs(df2);
        float t01 = d0 / (d1 + d0);

        if (df1 * df2 < 0.0) {
            float t12 = d1 / (d2 + d1);

            res[0] = {
                    t.a.x + (t.b.x - t.a.x) * t01,
                    t.a.y + (t.b.y - t.a.y) * t01,
                    t.a.z + (t.b.z - t.a.z) * t01,
            };

            res[1] = {
                 t.b.x + (t.c.x - t.b.x) * t12,
                    t.b.y + (t.c.y - t.b.y) * t12,
                    t.b.z + (t.c.z - t.b.z) * t12,
            };
        }
        else {
            float t20 = d2 / (d0 + d2);

            res[0] = {
                  t.a.x + (t.b.x - t.a.x) * t01,
                    t.a.y + (t.b.y - t.a.y) * t01,
                    t.a.z + (t.b.z - t.a.z) * t01,
            };

            res[1] = {
                 t.c.x + (t.a.x - t.c.x) * t20,
                    t.c.y + (t.a.y - t.c.y) * t20,
                    t.c.z + (t.a.z - t.c.z) * t20,
            };
        }
    }
    else {
        if (df1 * df2 < 0.0)
        {


            float d0 = abs(df0);
            float d1 = abs(df1);
            float d2 = abs(df2);
            float t12 = d1 / (d2 + d1);
            float t20 = d2 / (d0 + d2);

            res[0] = {
                 t.b.x + (t.c.x - t.b.x) * t12,
                    t.b.y + (t.c.y - t.b.y) * t12,
                    t.b.z + (t.c.z - t.b.z) * t12,
            };

            res[1] = {
                 t.c.x + (t.a.x - t.c.x) * t20,
                    t.c.y + (t.a.y - t.c.y) * t20,
                    t.c.z + (t.a.z - t.c.z) * t20,
            };
        }
    }

    return res;
}

Vector3 IntersectionPointsOfPlanes(Quaternion p0, Quaternion p1, Quaternion p2)
{

    float d0 = Determinant3x3(
        p0.x, p0.y, p0.z,
        p1.x, p1.y, p1.z,
        p2.x, p2.y, p2.z);

    if (d0 == 0) {
        return { 0 };
    }

    float dx = Determinant3x3(
        -p0.w, p0.y, p0.z,
        -p1.w, p1.y, p1.z,
        -p2.w, p2.y, p2.z
    );

    float dy = Determinant3x3(
        p0.x, -p0.w, p0.z,
        p1.x, -p1.w, p1.z,
        p2.x, -p2.w, p2.z
    );

    float dz = Determinant3x3(
        p0.x, p0.y, -p0.w,
        p1.x, p1.y, -p1.w,
        p2.x, p2.y, -p2.w
    );

    return { dx / d0, dy / d0, dz / d0 };
}

Vector3* PlanePlaneIntersectionLine(Quaternion p1, Quaternion p2)
{
    Vector3 orthoNormal = CrossProduct({ p1.x, p1.y, p1.z }, { p2.x, p2.y, p2.z });
    Quaternion orthoPlane = { orthoNormal.x, orthoNormal.y, orthoNormal.z, 0 };

    Vector3 pointOnLine = IntersectionPointsOfPlanes(p1, p2, orthoPlane);
    Vector3 theLine[2] = { pointOnLine, {pointOnLine.x + orthoNormal.x, pointOnLine.y + orthoNormal.y, pointOnLine.z + orthoNormal.z} };
    return theLine;
}

Vector3 IntersectSpans(float a0, float a1, float b0, float b1)
{
    float maxStart = max(a0, b0);
    float minEnd = min(a1, b1);

    if (maxStart <= minEnd) {
        return { maxStart, minEnd };
    }

    return { 0 };
}

Vector3* LineCrossingsIntervalLine(Vector3* line, Vector3* triIntPts0, Vector3* triIntPts1)
{

    float lineLength2 = LineLengthSquared(line);
    float triIntTs0_0 = -1.0 * ((line[0].x - triIntPts0[0].x) * (line[1].x - line[0].x) + (line[0].y - triIntPts0[0].y) * (line[1].y - line[0].y) + (line[0].z - triIntPts0[0].z) * (line[1].z - line[0].z)) / lineLength2;
    float triIntTs1_0 = -1.0 * ((line[0].x - triIntPts1[0].x) * (line[1].x - line[0].x) + (line[0].y - triIntPts1[0].y) * (line[1].y - line[0].y) + (line[0].z - triIntPts1[0].z) * (line[1].z - line[0].z)) / lineLength2;
    float triIntTs0_1 = -1.0 * ((line[0].x - triIntPts0[1].x) * (line[1].x - line[0].x) + (line[0].y - triIntPts0[1].y) * (line[1].y - line[0].y) + (line[0].z - triIntPts0[1].z) * (line[1].z - line[0].z)) / lineLength2;
    float triIntTs1_1 = -1.0 * ((line[0].x - triIntPts1[1].x) * (line[1].x - line[0].x) + (line[0].y - triIntPts1[1].y) * (line[1].y - line[0].y) + (line[0].z - triIntPts1[1].z) * (line[1].z - line[0].z)) / lineLength2;

    Vector3 intersectedSpan = { 0 };
    Vector3 intersection[2] = { {0}, {0} };

    if (triIntTs0_0 < triIntTs0_1) {
        if (triIntTs1_0 < triIntTs1_1) {
            intersectedSpan = IntersectSpans(triIntTs0_0, triIntTs0_1, triIntTs1_0, triIntTs1_1);
        }
        else {
            intersectedSpan = IntersectSpans(triIntTs0_0, triIntTs0_1, triIntTs1_1, triIntTs1_0);
        }
    }
    else {
        if (triIntTs1_0 < triIntTs1_1) {
            intersectedSpan = IntersectSpans(triIntTs0_1, triIntTs0_0, triIntTs1_0, triIntTs1_1);
        }
        else {
            intersectedSpan = IntersectSpans(triIntTs0_1, triIntTs0_0, triIntTs1_1, triIntTs1_0);
        }
    }

    if (intersectedSpan.x == 0 && intersectedSpan.y == 0 && intersectedSpan.z == 0)
    {
        return intersection;
    }

    intersection[0] = {
            line[0].x + (line[1].x - line[0].x) * intersectedSpan.x,
            line[0].y + (line[1].y - line[0].y) * intersectedSpan.x,
            line[0].z + (line[1].z - line[0].z) * intersectedSpan.x
    };
    intersection[1] =
    {
        line[0].x + (line[1].x - line[0].x) * intersectedSpan.y,
        line[0].y + (line[1].y - line[0].y) * intersectedSpan.y,
        line[0].z + (line[1].z - line[0].z) * intersectedSpan.y
    };

    return intersection;
}


bool CheckTrianglesCollision(Triangle t1, Triangle t2)
{
    Vector3* intersection = new Vector3[2];

    Quaternion p1 = TriangleToPlaneObjNormalized(t1);
    Vector3* tri1CrossPts = TrianglePointsCrossingNormalizedPlane(t1, p1);

    if (!IsVecNull(tri1CrossPts, 2))
    {
        Quaternion p2 = TriangleToPlaneObjNormalized(t2);
        Vector3* tri2CrossPts = TrianglePointsCrossingNormalizedPlane(t2, p2);
        if (!IsVecNull(tri2CrossPts, 2))
        {
            Vector3* intLine = PlanePlaneIntersectionLine(p1, p2);
            intersection = LineCrossingsIntervalLine(intLine, tri1CrossPts, tri2CrossPts);
        }
    }

    return IsVecNull(intersection, 2);
}
