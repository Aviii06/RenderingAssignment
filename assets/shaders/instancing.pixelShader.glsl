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

vec3 phongColor(vec3 lightDir, vec3 viewDir, vec3 normal, vec3 diffuseColor, vec3 specularColor, float shininess)
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
    // vec3 viewDir = normalize(-v_VertPos);
    vec3 normal = normalize(v_Normal);
    vec3 radiance = LightDiffuseColor.rgb * 0.1f;
    float lightIntensity = (int(instanceID) % 6 + 1) * 0.5f;
    float irradiance = max(dot(lightDir, normal), 0.0) * lightIntensity;
    float shininess = (instanceID/6) + 1.0f;

    vec3 brdf = phongColor(lightDir, viewDir, normal, LightDiffuseColor.rbg, LightSpecularColor.rgb, shininess);
    radiance += irradiance * brdf * LightColor.rgb;

    return vec4(radiance, 1.0f);
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
