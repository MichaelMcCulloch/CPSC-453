#version 430


in vec2 vPos;

out vec4 FragmentColour;


struct Sphere
{
    vec4 center;
    vec4 diffuseColor;
    vec4 specularColor;
    float phongExp;
    float radius;
    float reflectance;
};

struct Plane
{
    vec4 norm;
    vec4 point;
    vec4 diffuseColor;
    vec4 specularColor;
    float phongExp;
    float reflectance;
};

struct Triangle
{
    vec4 A;
    vec4 B;
    vec4 C;
    vec4 diffuseColor;
    vec4 specularColor;
    float phongExp;
    float reflectance;
};

struct Light
{
    vec4 center;
    vec4 color;
    float radius;
    float intensity;
};

layout(std140, binding = 1) uniform SphereData
{
    Sphere sphere[50];
};
layout(std140, binding = 2) uniform TriangleData
{
    Triangle triangle[50];
};
layout(std140, binding = 3) uniform PlaneData
{
    Plane plane[50];
};
layout(std140, binding = 4) uniform LightData
{
    Light light[50];
};

//for iterating over
uniform int numSpheres;
uniform int numTriangles;
uniform int numPlanes;
uniform int numLights;

const int numBounce = 10; //the number of bounces which will be made
const vec3 ambientLight = vec3(0.1, 0.1, 0.1);
uniform vec3 cameraOrigin;
uniform float fov;

const float pi = 3.1415926535897931;

//the tracing operation; returns pixel color
vec3 trace(vec3 origin, vec3 ray, int depth);
//calculate a direction vector from which px on screen we are rendering
vec3 directionFromPixel(vec2 pixel);
float vecToMagnitude(vec3 v);

//determine intersection point for ray from origin, and selected shape in selected shape index
float intersectSphere(vec3 origin, vec3 ray, int sphereIndex);
float intersectPlane(vec3 origin, vec3 ray, int planeIndex);
float intersectTriangle(vec3 origin, vec3 ray, int triangleIndex);



void main(void)
{
    vec3 dir = directionFromPixel(vPos);
    FragmentColour = vec4(trace(cameraOrigin, dir, 10), 1.0);
}


vec3 trace(vec3 origin, vec3 ray, int depth)
{
    vec3 phong;

    //for each reflection
    for (int hop = 0; hop < 1; hop++)
    {

        //find intersection point and type of object
        int objectType = -1; //0 = sphere; 1 = plane; 2 = triangle
        Sphere s;
        Plane p;
        Triangle t;

        float minT = -1;
        for (int i = 0; i < numSpheres; i++)
        {
            float test = intersectSphere(origin, ray, i);
            if (test > 0 && (minT < 0 || test < minT))
            {
                minT = test;
                objectType = 0;
                s = sphere[i];
            }
        }
        for (int i = 0; i < numPlanes; i++)
        {
            float test = intersectPlane(origin, ray, i);
            if (test > 0 && (minT < 0 || test < minT))
            {
                minT = test;
                objectType = 1;
                p = plane[i];
            }
        }
        for (int i = 0; i < numTriangles; i++)
        {
            float test = intersectTriangle(origin, ray, i);
            if (test > 0 && (minT < 0 || test < minT))
            {
                minT = test;
                objectType = 2;
                t = triangle[i];
            }
        }

        //no intersection
        if (minT < 0)
            return vec3(0, 0, 0);

        float phongExp;
        vec3 diffuse;
        vec3 specular;
        vec3 normal;

        vec3 intersect = origin + minT * ray; //point of intersection with object

        //collect values from object
        switch (objectType)
        {
        case 0:
            diffuse = s.diffuseColor.xyz;
            specular = s.specularColor.xyz;
            phongExp = s.phongExp;

            normal = normalize(intersect - s.center.xyz);
            break;
        case 1:
            diffuse = p.diffuseColor.xyz;
            specular = p.specularColor.xyz;
            phongExp = p.phongExp;

            normal = normalize(p.norm.xyz);
            break;
        case 2:
            diffuse = t.diffuseColor.xyz;
            specular = t.specularColor.xyz;
            phongExp = t.phongExp;

            vec3 A = t.A.xyz;
            vec3 B = t.B.xyz;
            vec3 C = t.C.xyz;

            vec3 N = B - A;
            vec3 M = C - A;
            normal = normalize(cross(N, M));
            break;
        default:
            //never gonna happen
            break;
        }
        int lightSamples = 1;
        //calculate phong lighting for each light source
        for (int l = 0; l < numLights; l++)
        {
            //check if in shadow from light, sum shadow
            vec3 shadow = vec3(0, 0, 0);
            vec3 center, rLight;
            float dist;
            for (int n = 0; n < lightSamples; n++)
            {
                center = light[l].center.xyz;
                dist = vecToMagnitude(center - intersect);
                rLight = normalize(center - intersect);

                bool found = false;

                //check if an object is between light and object
                for (int i = 0; i < numSpheres && !found; i++)
                {
                    //move slightly in direction of normal
                    float test = intersectSphere(intersect + 0.0001 * normal, rLight, i);
                    if (test >= 0 && test <= dist)
                        found = true;
                }

                for (int i = 0; i < numPlanes && !found; i++)
                {
                    float test = intersectPlane(intersect + 0.00001 * normal, rLight, i);
                    if (test >= 0 && test <= dist)
                        found = true;
                }

                for (int i = 0; i < numTriangles && !found; i++)
                {
                    float test = intersectTriangle(intersect + 0.00001 * normal, rLight, i);
                    if (test >= 0 && test <= dist)
                        found = true;
                }

                if (found)
                {
                    shadow += vec3(0, 0, 0);
                }
                else
                {
                    shadow += light[l].color.xyz * light[l].intensity / lightSamples;
                }
            }
            vec3 R = rLight - 2 * (dot(rLight, normal)) * normal ;

            phong += diffuse * ambientLight;
            phong += (specular * pow(max(0, dot(R, ray)), phongExp) * shadow);
            phong += (diffuse * (max(0, dot(rLight, normal))) * shadow);
        }
    }
    return phong;
}

float vecToMagnitude(vec3 v)
{
    return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
}

vec3 directionFromPixel(vec2 pixel)
{
    float rads = fov * pi / 180;
    float z = 1 / (2 * tan(rads / 2.0));
    vec3 dir = vec3(pixel.x, pixel.y, -z);
    return normalize(dir);
}

float intersectSphere(vec3 origin, vec3 ray, int i)
{

    vec3 d = ray;
    vec3 center = sphere[i].center.xyz;
    vec3 oc = origin - center;
    float r = sphere[i].radius;

    float b = dot(d, oc);
    float c = dot(oc, oc) - r * r;

    float disc = b * b - c;
    if (disc < 0)
        return -1;

    disc = sqrt(disc);
    float t0 = -(b - disc);
    float t1 = -(b + disc);

    return (t0 < t1) ? t0 : t1;
}

float intersectPlane(vec3 origin, vec3 ray, int i)
{
    vec3 o = origin;
    vec3 d = ray;
    vec3 q = plane[i].point.xyz;
    vec3 n = normalize(plane[i].norm.xyz);

    float qn = dot(q, n);
    float on = dot(o, n);
    float dn = dot(d, n);
    float t = (qn - on) / dn;
    return t;
}

float intersectTriangle(vec3 origin, vec3 ray, int i)
{

    vec3 A = triangle[i].A.xyz;
    vec3 B = triangle[i].B.xyz;
    vec3 C = triangle[i].C.xyz;

    vec3 N = B - A;
    vec3 M = C - A;

    vec3 pvec = cross(ray, M);
    float det = dot(N, pvec);

    float invDet = 1 / det;

    vec3 tvec = origin - A;

    float u = dot(tvec, pvec) * invDet;
    if (u < 0 || u > 1)
        return -1;

    vec3 qvec = cross(tvec, N);
    float v = dot(ray, qvec) * invDet;
    if (v < 0 || u + v > 1)
        return -1;

    float t = dot(M, qvec) * invDet;

    vec3 norm = cross(N, M);

    //same plane calc
    vec3 o = origin;
    vec3 d = ray;
    vec3 q = A;
    vec3 n = normalize(norm);

    float qn = dot(q, n);
    float on = dot(o, n);
    float dn = dot(d, n);
    float p = (qn - on) / dn;
    return p;
}

float rand(vec2 co)
{
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}