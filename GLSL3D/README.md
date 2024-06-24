- [Étape 1 : Ray Origin \& Ray Direction](#étape-1--ray-origin--ray-direction)
- [Étape 2 : Champ de Distance (SDF) \& Ray Marching](#étape-2--champ-de-distance-sdf--ray-marching)
- [Étape 3 : Normales](#étape-3--normales)
- [Étape 4 : Lumières](#étape-4--lumières)
- [Étape 5 : Jouer avec les SDFs](#étape-5--jouer-avec-les-sdfs)
- [Ressources](#ressources)

## Étape 1 : Ray Origin & Ray Direction

<details>
  <summary>Code</summary>

```glsl
void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    // Centered UVs from -0.5 to 0.5
    vec2 uv = (fragCoord - iResolution.xy / 2.) / iResolution.y;
    
    // Compute ray_origin and ray_direction
    const float focal_length = 1.;
    vec3 ray_origin = vec3(0., -5., 0.);
    vec3 ray_direction = normalize(vec3(uv.x, ray_origin.y - focal_length, uv.y) - ray_origin);
    
    // Output ray_direction as a color to check that it is correct
    vec3 col = ray_direction;
    fragColor = vec4(col, 1.);
}
```

</details>
<br/>

<iframe width="640" height="360" frameborder="0" src="https://www.shadertoy.com/embed/4XtSz4?gui=true&t=10&paused=true&muted=false" allowfullscreen></iframe>

## Étape 2 : Champ de Distance (SDF) & Ray Marching

<details>
  <summary>Code</summary>

```glsl
// SDF (Signed Distance Field). Returns the distance to the object, and a negative distance if we are inside it.
float sphere_sdf(vec3 position, vec3 center, float radius)
{
    return distance(position, center) - radius;
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    // Centered UVs from -0.5 to 0.5
    vec2 uv = (fragCoord - iResolution.xy / 2.) / iResolution.y;
    
    // Compute ray_origin and ray_direction
    const float focal_length = 1.;
    vec3 ray_origin = vec3(0., -5., 0.);
    vec3 ray_direction = normalize(vec3(uv.x, ray_origin.y + focal_length, uv.y) - ray_origin);
    
    // Ray Marching
    float distance_along_ray = 0.;
    bool has_intersected_object = false;
    for(int i = 0; i < 100; ++i)
    {
        vec3 position_along_ray = ray_origin + ray_direction * distance_along_ray;
        float distance_to_closest_object = sphere_sdf(position_along_ray, vec3(0.), 1.);
        distance_along_ray += distance_to_closest_object;
        if(distance_to_closest_object < 0.0001)
        {
            has_intersected_object = true;
            break;
        }
    }
    vec3 col = has_intersected_object ? vec3(1.) : vec3(0.);
    fragColor = vec4(col, 1.);
}
```

</details>
<br/>

<iframe width="640" height="360" frameborder="0" src="https://www.shadertoy.com/embed/X3dSz4?gui=true&t=10&paused=true&muted=false" allowfullscreen></iframe>

## Étape 3 : Normales

<details>
  <summary>Code</summary>

```glsl
// SDF (Signed Distance Field). Returns the distance to the object, and a negative distance if we are inside it.
float sphere_sdf(vec3 position, vec3 center, float radius)
{
    return distance(position, center) - radius;
}

float scene_sdf(vec3 position)
{
    return sphere_sdf(position, vec3(0.), 1.);
}

vec3 compute_normal(vec3 position)
{
    // Compute the gradient of the sdf to get the normal
    float e = 0.001;
    return normalize(vec3(
        scene_sdf(position + vec3(e, 0., 0.)) - scene_sdf(position),
        scene_sdf(position + vec3(0., e, 0.)) - scene_sdf(position),
        scene_sdf(position + vec3(0., 0., e)) - scene_sdf(position)
    ));
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    // Centered UVs from -0.5 to 0.5
    vec2 uv = (fragCoord - iResolution.xy / 2.) / iResolution.y;
    
    // Compute ray_origin and ray_direction
    const float focal_length = 1.;
    vec3 ray_origin = vec3(0., -5., 0.);
    vec3 ray_direction = normalize(vec3(uv.x, ray_origin.y + focal_length, uv.y) - ray_origin);
    
    // Ray Marching
    float distance_along_ray = 0.;
    vec3 position_along_ray;
    bool has_intersected_object = false;
    for(int i = 0; i < 100; ++i)
    {
        position_along_ray = ray_origin + ray_direction * distance_along_ray;
        float distance_to_closest_object = scene_sdf(position_along_ray);
        distance_along_ray += distance_to_closest_object;
        if(distance_to_closest_object < 0.0001)
        {
            has_intersected_object = true;
            break;
        }
    }
    
    vec3 col;
    if(has_intersected_object)
    {
        vec3 normal = compute_normal(position_along_ray);
        col = normal;
    }
    else
    {
        col = vec3(0.);
    }
    fragColor = vec4(col, 1.);
}
```

</details>
<br/>

<iframe width="640" height="360" frameborder="0" src="https://www.shadertoy.com/embed/43tXz4?gui=true&t=10&paused=true&muted=false" allowfullscreen></iframe>

## Étape 4 : Lumières

<details>
  <summary>Code</summary>

```glsl
// SDF (Signed Distance Field). Returns the distance to the object, and a negative distance if we are inside it.
float sphere_sdf(vec3 position, vec3 center, float radius)
{
    return distance(position, center) - radius;
}

float scene_sdf(vec3 position)
{
    return sphere_sdf(position, vec3(0.), 1.);
}

vec3 compute_normal(vec3 position)
{
    // Compute the gradient of the sdf to get the normal
    float e = 0.001;
    return normalize(vec3(
        scene_sdf(position + vec3(e, 0., 0.)) - scene_sdf(position),
        scene_sdf(position + vec3(0., e, 0.)) - scene_sdf(position),
        scene_sdf(position + vec3(0., 0., e)) - scene_sdf(position)
    ));
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    // Centered UVs from -0.5 to 0.5
    vec2 uv = (fragCoord - iResolution.xy / 2.) / iResolution.y;
    
    // Compute ray_origin and ray_direction
    const float focal_length = 1.;
    vec3 ray_origin = vec3(0., -5., 0.);
    vec3 ray_direction = normalize(vec3(uv.x, ray_origin.y + focal_length, uv.y) - ray_origin);
    
    // Ray Marching
    float distance_along_ray = 0.;
    vec3 position_along_ray;
    bool has_intersected_object = false;
    for(int i = 0; i < 100; ++i)
    {
        position_along_ray = ray_origin + ray_direction * distance_along_ray;
        float distance_to_closest_object = scene_sdf(position_along_ray);
        distance_along_ray += distance_to_closest_object;
        if(distance_to_closest_object < 0.0001)
        {
            has_intersected_object = true;
            break;
        }
    }
    
    vec3 col;
    if(has_intersected_object)
    {
        vec3 normal = compute_normal(position_along_ray);
        vec3 light_direction = normalize(vec3(cos(iTime), sin(iTime), -1.));
        float light_attenuation = max(-dot(light_direction, normal), 0.);
        light_attenuation += 0.1;
        col = vec3(light_attenuation);
    }
    else
    {
        col = vec3(0.);
    }
    fragColor = vec4(col, 1.);
}
```

</details>
<br/>

<iframe width="640" height="360" frameborder="0" src="https://www.shadertoy.com/embed/X3dXz4?gui=true&t=10&paused=true&muted=false" allowfullscreen></iframe>

## Étape 5 : Jouer avec les SDFs

[Vous pouvez vous référer à cette page qui liste plein de formes et d'effets possibles avec les SDFs](https://iquilezles.org/articles/distfunctions/)

<details>
  <summary>Code</summary>

```glsl
// SDF (Signed Distance Field). Returns the distance to the object, and a negative distance if we are inside it.
float sphere_sdf(vec3 position, vec3 center, float radius)
{
    return distance(position, center) - radius;
}

float box_sdf(vec3 position, vec3 size)
{
  vec3 q = abs(position) - size;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}


float smooth_min(float d1, float d2, float k)
{
    float h = clamp( 0.5 + 0.5*(d2-d1)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) - k*h*(1.0-h);
}

vec3 limited_repetition(vec3 p, vec3 s, vec3 l)
{
    return p - s*round(p/s);
}

mat3x3 rotation(vec3 v, float a)
{
    float si = sin(a);
    float co = cos(a);
    float ic = 1. - co;

    return mat3x3( v.x*v.x*ic + co,       v.y*v.x*ic - si*v.z,    v.z*v.x*ic + si*v.y,
                   v.x*v.y*ic + si*v.z,   v.y*v.y*ic + co,        v.z*v.y*ic - si*v.x,
                   v.x*v.z*ic - si*v.y,   v.y*v.z*ic + si*v.x,    v.z*v.z*ic + co );
}

vec3 twist( vec3 p )
{
    const float k = .07; // or some other amount
    float c = cos(k*p.y);
    float s = sin(k*p.y);
    mat2  m = mat2(c,-s,s,c);
    vec3  q = vec3(m*p.xz,p.y);
    return q;
}

float scene_sdf(vec3 position)
{
    position.y -= 10.;
    position = inverse(rotation(vec3(1., 0.1, 0.), iTime)) * position;
    position = twist(position);
    position = limited_repetition(position, vec3(9.5), vec3(1.));
    //position = mod(position - 1.5, 3.) + 1.5;
    return smooth_min(
        sphere_sdf(position, vec3(0., 0., 0.), 1.),
        box_sdf(position, vec3(0.7)),
        .5
   );
}

vec3 compute_normal(vec3 position)
{
    // Compute the gradient of the sdf to get the normal
    float e = 0.001;
    return normalize(vec3(
        scene_sdf(position + vec3(e, 0., 0.)) - scene_sdf(position),
        scene_sdf(position + vec3(0., e, 0.)) - scene_sdf(position),
        scene_sdf(position + vec3(0., 0., e)) - scene_sdf(position)
    ));
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    // Centered UVs from -0.5 to 0.5
    vec2 uv = (fragCoord - iResolution.xy / 2.) / iResolution.y;
    
    // Compute ray_origin and ray_direction
    const float focal_length = 1.1;
    vec3 ray_origin = vec3(0., -5., 0.);
    vec3 ray_direction = normalize(vec3(uv.x, ray_origin.y + focal_length, uv.y) - ray_origin);
    
    // Ray Marching
    float distance_along_ray = 0.;
    vec3 position_along_ray;
    bool has_intersected_object = false;
    int i = 0;
    for(; i < 1000; ++i)
    {
        position_along_ray = ray_origin + ray_direction * distance_along_ray;
        float distance_to_closest_object = scene_sdf(position_along_ray);
        distance_along_ray += distance_to_closest_object*0.3;
        if(distance_to_closest_object < 0.0001)
        {
            has_intersected_object = true;
            break;
        }
        if(distance_along_ray > 100.)
            break;
    }
    
    vec3 col;
    if(has_intersected_object)
    {
        vec3 normal = compute_normal(position_along_ray);
        vec3 light_direction = normalize(vec3(cos(iTime), sin(iTime), -1.));
        float light_attenuation = 0.7 * max(-dot(light_direction, normal), 0.);
        light_attenuation += 0.02;
        col = vec3(light_attenuation);
    }
    else
    {
        col = 0.5*vec3(float(i)/1000.);
    }
    col = mix(vec3(37, 10, 97) / 255., vec3(242, 140, 255)/255.*3., col.r);
    fragColor = vec4(col, 1.);
}
```

</details>
<br/>

<iframe width="640" height="360" frameborder="0" src="https://www.shadertoy.com/embed/MXtSRN?gui=true&t=10&paused=true&muted=false" allowfullscreen></iframe>

## Ressources

[Série de vidéos : introduction au Ray Marching](https://youtu.be/PGtv-dBi2wE?list=PLGmrMu-IwbgtMxMiV3x4IrHPlPmg7FD-P)

[Une autre excellente vidéo d'introduction au Ray Marching](https://youtu.be/khblXafu7iA)

[Pour aller plus loin : une œuvre complète faite en ray marching, expliquée pas à pas](https://youtu.be/-pdSjBPH3zM)