#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 col;
layout(location = 3) in vec3 normal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;

uniform vec3 offsets[36];
uniform float isSelected[36];

out vec2 v_TexCoord;
out vec3 v_Color;
out vec3 v_CrntPos;
out vec3 v_Normal;
out vec3 v_VertPos;
out float v_IsSelected;
out float v_InstanceID;

void main()
{
    vec4 temp = u_Model * vec4(position, 1.0f) + vec4(offsets[gl_InstanceID].rgb, 1.0f);
    v_CrntPos = vec3(temp);
    temp = u_View * temp;
    v_VertPos = vec3(temp);
    gl_Position = u_Proj * temp;
    v_TexCoord = texCoord;
    v_Color = col;
    v_Normal =  normal;
    v_IsSelected = isSelected[gl_InstanceID];
    v_InstanceID = float(gl_InstanceID);
}

////////////

//#version 330 core
//layout(location = 0) in vec3 position;
//layout(location = 1) in vec2 texCoord;
//layout(location = 2) in vec3 col;
//layout(location = 3) in vec3 normal;
//
//uniform mat4 u_Model;
//uniform mat4 u_View;
//uniform mat4 u_Proj;
//
//uniform vec3 offsets[36];
//uniform float isSelected[36];
//
//out vec2 v_TexCoord;
//out vec3 v_Color;
//out vec3 v_CrntPos;
//out vec3 v_Normal;
//out vec3 v_VertPos;
//out float v_InstanceID;
//out float v_IsSelected;
//
//void main()
//{
//    vec4 temp = u_Model * vec4(position + offsets[gl_InstanceID], 1.0f);
//    v_CrntPos = vec3(temp);
//    temp = u_View * temp;
//    v_VertPos = vec3(temp);
//    gl_Position = u_Proj * temp;
//    v_TexCoord = texCoord;
//    v_Color = col;
//    v_Normal = normal;
//    v_InstanceID = float(gl_InstanceID);
//    v_IsSelected = isSelected[gl_InstanceID];
//}

//////////
