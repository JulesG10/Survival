#pragma once

#include "stdafx.h"

typedef struct Triangle {
    Vector3 a, b, c;
}Triangle;

typedef struct CollisionModel {
    Model model;

    Vector3 position;

    Vector3 axis;
    float angle;

    Vector3 scale;

    std::vector<std::vector<Triangle>> triangles;
}CollisionModel;

std::vector<std::vector<Triangle>> GetTriangles(Model m, Vector3 position, Vector3 axis, float angle, Vector3 scale);

CollisionModel CreateCollisionModel(Model m, Vector3 position, Vector3 axis, float angle, Vector3 scale);

void UpdateCollisionModel(CollisionModel* col);
void DrawTriangles(std::vector<std::vector<Triangle>> tri, Color c);

float LineLengthSquared(Vector3* line);
float Determinant3x3(
    float a1, float b1, float c1,
    float a2, float b2, float c2,
    float a3, float b3, float c3);

Quaternion TriangleToPlaneObjNormalized(Triangle t);

Vector3 MatrixMultiplyVector3(Vector3 a, Matrix transform);
Vector3 CrossProduct(Vector3 a, Vector3 b);
Vector3 IntersectionPointsOfPlanes(Quaternion p0, Quaternion p1, Quaternion p2);
Vector3 IntersectSpans(float a0, float a1, float b0, float b1);

Vector3* TrianglePointsCrossingNormalizedPlane(Triangle t, Quaternion p);
Vector3* PlanePlaneIntersectionLine(Quaternion p1, Quaternion p2);
Vector3* LineCrossingsIntervalLine(Vector3* line, Vector3* triIntPts0, Vector3* triIntPts1);

bool ModelCollision(CollisionModel a, CollisionModel b);
bool IsVecNull(Vector3* vec, size_t s);
bool CheckTrianglesCollision(Triangle t1, Triangle t2);


