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

const int numBounce = 4; //the number of bounces which will be made
const vec3 ambientLight = vec3(0.1, 0.1, 0.1);
uniform vec3 cameraOrigin;
uniform float fov;

uniform mat4 transform;
uniform mat4 oTransform;

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
    dir = (transform * vec4(dir, 1.0)).xyz;
    vec3 newOrigin = (oTransform * vec4(cameraOrigin, 1.0)).xyz;
    FragmentColour = vec4(trace(newOrigin, dir, 10), 1.0);
}


vec3 trace(vec3 origin, vec3 ray, int depth)
{
    vec3 phong;

    //fwdPass collects the following information
    vec3 specularCalc[numBounce];  //max(0, dot(R, V)^P) * shadow
    vec3 diffuseCalc[numBounce];   //max(0, dot(L, N))   * shadow
    float reflectance[numBounce];
    vec3 diffuseColor[numBounce];
    vec3 specularColor[numBounce];
    for(int fwdPass = 0; fwdPass < numBounce; fwdPass++){

        specularCalc[fwdPass] = vec3(0,0,0);  //max(0, dot(R, V)^P) * shadow
        diffuseCalc[fwdPass] = vec3(0,0,0);   //max(0, dot(L, N))   * shadow
        reflectance[fwdPass] = 0;
        diffuseColor[fwdPass] = vec3(0,0,0);
        specularColor[fwdPass] = vec3(0,0,0);
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
        if (minT < 0){
            diffuseColor[fwdPass] = vec3(0,0,0);
            specularColor[fwdPass] = vec3(0,0,0);
        }
        //collect data from intersected object:
        vec3 normal;
        float phongExp;
        vec3 intersect = origin + minT * ray;
        switch (objectType)
        {
        case 0:
            diffuseColor[fwdPass] = s.diffuseColor.xyz;
            specularColor[fwdPass] = s.specularColor.xyz;
            reflectance[fwdPass] = s.reflectance;
            phongExp = s.phongExp;

            normal = normalize(intersect - s.center.xyz);
            break;
        case 1:
            diffuseColor[fwdPass] = p.diffuseColor.xyz;
            specularColor[fwdPass] = p.specularColor.xyz;
            reflectance[fwdPass] = p.reflectance;
            phongExp = p.phongExp;

            normal = normalize(p.norm.xyz);
            break;
        case 2:
            diffuseColor[fwdPass] = t.diffuseColor.xyz;
            specularColor[fwdPass] = t.specularColor.xyz;
            reflectance[fwdPass] = t.reflectance;
            phongExp = t.phongExp;

            vec3 A = t.A.xyz;
            vec3 B = t.B.xyz;
            vec3 C = t.C.xyz;

            vec3 N = B - A;
            vec3 M = C - A;
            normal = normalize(cross(N, M));
            break;
        default:
            // No intersection. Don't set anything
            break;
        }
        intersect = intersect + 0.00001 * normal;
        
        //collect phong lighting for each light source
        for (int l = 0; l < numLights; l++)
        {
            //check if in shadow from light, sum shadow
            vec3 center = light[l].center.xyz;
            vec3 rayToLight = center - intersect;
            vec3 rLight = normalize(rayToLight);
            vec3 shadow = vec3(0,0,0);
            float dist = vecToMagnitude(rayToLight);
           
         
            bool found = false;
            //check if an object is between light and object
            for (int i = 0; i < numSpheres && !found; i++)
            {
                //move slightly in direction of normal
                float test = intersectSphere(intersect, rLight, i);
                if (test >= 0 && test <= dist)
                    found = true;
            }

            for (int i = 0; i < numPlanes && !found; i++)
            {
                float test = intersectPlane(intersect, rLight, i);
                if (test >= 0 && test <= dist)
                    found = true;
            }

            for (int i = 0; i < numTriangles && !found; i++)
            {
                float test = intersectTriangle(intersect, rLight, i);
                if (test >= 0 && test <= dist)
                    found = true;
            }

            if (found)
                shadow += vec3(0, 0, 0);
            else
                shadow += light[l].color.xyz * light[l].intensity;
            
            vec3 R = rLight - 2 * (dot(rLight, normal)) * normal;
            //calculate phong data
            specularCalc[fwdPass] += pow(max(0, dot(R, ray)), phongExp) * shadow;
            diffuseCalc[fwdPass]  += (max(0, dot(rLight, normal))) * shadow;
        }

        //return temp;
        
        ray = ray - 2 * (dot(ray, normal)) * normal;
        origin = intersect;
    }

    vec3 reflectedColor[numBounce];
    for(int bwdPass = numBounce - 1; bwdPass >= 0; bwdPass--){
        vec3 temp = vec3(0,0,0);
        temp += ambientLight * diffuseColor[bwdPass];
        temp += diffuseCalc[bwdPass] * diffuseColor[bwdPass];
        temp += specularCalc[bwdPass] * specularColor[bwdPass];

        if (bwdPass < numBounce - 1){
            float ref = reflectance[bwdPass];
            temp = (1 - ref) * temp + ref * reflectedColor[bwdPass + 1];
        }
        reflectedColor[bwdPass] = temp;
    }

    return reflectedColor[0];

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