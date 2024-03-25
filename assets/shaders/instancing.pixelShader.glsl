#version 330 core

in vec3 v_VertPos;
in vec3 v_CrntPos;
in vec3 v_Normal;
in vec3 v_Color;
in float v_IsSelected;
in float v_InstanceID;

uniform vec3 pointLightColor;
uniform vec3 pointLightPos;
uniform float pointLightIntensity;

uniform vec3 LightDiffuseColor;
uniform vec3 LightDir;
uniform vec3 LightSpecularColor;
uniform vec3 LightColor;

uniform vec3 hitNormal;


out vec4 FragColor;



// Linear to sRGB conversion
vec3 linearToRgb(vec3 color)
{
    return pow(color, vec3(1.0f / 2.2f));
}

// sRGB to Linear conversion
vec3 rgbToLinear(vec3 color)
{
    return pow(color, vec3(2.2f));
}

// Phone BRDF
vec3 phongBRDF(vec3 lightDir, vec3 viewDir, vec3 normal, vec3 diffuseColor, vec3 specularColor, float shininess)
{
    vec3 color = diffuseColor;
    vec3 reflectDir = reflect(-lightDir, normal);
    float specDot = max(dot(reflectDir, viewDir), 0.0);
    color += pow(specDot, shininess) * specularColor;

    return color;
}

vec4 directionalLight(float instanceID)
{
    vec3 lightDir = normalize(LightDir);
    // It's an ortho camera so the view direction is the same for all vertices
    vec3 viewDir = vec3(0.0f, 0.0f, 1.0f);
//    vec3 viewDir = normalize(-v_VertPos);
    vec3 normal = normalize(v_Normal);
    vec3 radiance = rgbToLinear(LightDiffuseColor.rgb) * 0.1f;
    float lightIntensity = (int(instanceID) % 6 + 1) * 0.5f;
    float irradiance = max(dot(lightDir, normal), 0.0) * lightIntensity;
    float shininess = (instanceID/6) + 1.0f;

    vec3 brdf = phongBRDF(lightDir, viewDir, normal, rgbToLinear(LightDiffuseColor.rbg), rgbToLinear(LightSpecularColor.rgb), shininess);
    radiance += irradiance * brdf * rgbToLinear(LightColor.rgb);

    return vec4(linearToRgb(radiance), 1.0f);
}

void main()
{
    // outputs final color
    if (v_IsSelected == 1.0f)
    {
        vec3 color = (vec3(1.0f, 1.0f, 1.0f) + hitNormal) / 2.0f;
        FragColor = vec4(color, 1.0f);
        return;
    }

    FragColor = directionalLight(v_InstanceID);
}

////////////
//
//#version 330 core
//
//in vec3 v_VertPos;
//in vec3 v_CrntPos;
//in vec3 v_Normal;
//in vec3 v_Color;
//in float v_IsSelected;
//in float v_InstanceID;
//
//uniform vec3 pointLightColor;
//uniform vec3 pointLightPos;
//uniform float pointLightIntensity;
//
//uniform vec3 LightDiffuseColor;
//uniform vec3 LightDir;
//uniform float LightIntensity;
//uniform vec3 LightSpecularColor;
//uniform vec3 LightColor;
//uniform float shininess;
//
//uniform vec3 hitNormal;
//
//out vec4 FragColor;
//
//vec4 pointLight()
//{
//    // used in two variables so I calculate it here to not have to do it twice
//    vec3 lightVec = pointLightPos - v_CrntPos;
//
//    // intensity of light with respect to distance
//    float dist = length(lightVec);
//    float a = 3.0;
//    float b = 0.7;
//    float inten = pointLightIntensity * 100.0f / dist;
//
//    // diffuse lighting
//    vec3 normal = normalize(v_Normal);
//    vec3 lightDirection = normalize(lightVec);
//    float diffuse = min(max(dot(normal, lightDirection) * inten, 0.0f), 1.0f);
//
//    return vec4(diffuse, diffuse, diffuse, 1.0f) * vec4(pointLightColor, 1.0f);
//    //	return vec4(v_Offset, 1.0f);
//}
//
//// Linear to sRGB conversion
//vec3 linearToRgb(vec3 color)
//{
//    return pow(color, vec3(1.0f / 2.2f));
//}
//
//// sRGB to Linear conversion
//vec3 rgbToLinear(vec3 color)
//{
//    return pow(color, vec3(2.2f));
//}
//
//// Phone BRDF
//vec3 phongBRDF(vec3 lightDir, vec3 viewDir, vec3 normal, vec3 diffuseColor, vec3 specularColor, float shininess)
//{
//    vec3 color = diffuseColor;
//    vec3 reflectDir = reflect(-lightDir, normal);
//    float specDot = max(dot(reflectDir, viewDir), 0.0);
//    color += pow(specDot, shininess) * specularColor;
//
//    return color;
//}
//
//vec4 directionalLight(float v_InstanceID)
//{
//    vec3 lightDir = normalize(LightDir);
//    vec3 viewDir = normalize(-v_VertPos);
//    vec3 normal = normalize(v_Normal);
//    vec3 radiance = rgbToLinear(LightDiffuseColor.rgb) * 0.1f;
//    float irradiance = max(dot(lightDir, normal), 0.0) * LightIntensity;
//
//    vec3 brdf = phongBRDF(lightDir, viewDir, normal, rgbToLinear(LightDiffuseColor.rbg), rgbToLinear(LightSpecularColor.rgb), shininess);
//    radiance += irradiance * brdf * rgbToLinear(LightColor.rgb);
//
//    return vec4(linearToRgb(radiance), 1.0f);
//}
//
//void main()
//{
//    // outputs final color
////    if (v_IsSelected == 1.0f)
////    {
////        vec3 color = (vec3(1.0f, 1.0f, 1.0f) + hitNormal) / 2.0f;
////        FragColor = vec4(color, 1.0f);
////        return;
////    }
//
//    FragColor = directionalLight(v_InstanceID);
////    FragColor = pointLight() + directionalLight(v_InstanceID);
//}

//////////
