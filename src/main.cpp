#include "Vivid.h"
#include "common/maths/Vec.h"

const int number_of_sphere = 36;
float radius = 2.5f;

class ExampleInterface : public RenderingInterface
{
private:
    Vivid::Maths::Vec3 lightColor = Vivid::Maths::Vec3(1.0f, 0.5f, 1.0f);
    glm::vec3 suzannePosition = glm::vec3(0, 50, -200);
    glm::vec3 lightPosition = glm::vec3(0, 0, 0);

    Vivid::Mesh lightMesh;
    Ref<Vivid::Shader> lightShader;
    Vivid::Entity* suzanne = new Vivid::Entity(1, "Spheres");
    Vivid::Entity* light = new Vivid::Entity(2, "DirectionalLight");
    Vivid::ModelComponent* modelComponent1;
    Vivid::TransformComponent* sphereTransformComponent = new Vivid::TransformComponent();
    Vivid::TransformComponent* lightTransformComponent = new Vivid::TransformComponent();
    Vivid::Mesh* sphere;

    Vivid::PointLightComponent* pointLightComponent;
    Vivid::DirectionalLightComponent* directionalLightComponent;
    Ref<Vivid::Shader> shader;

    std::vector<Vivid::Maths::Vec3> translations;
    float factor = 300.0f;
    float scale = 45.0f;

public:
    void Setup() override
    {
        // Can write custom opengl confs here
        OPENGL_CONFS

        shader = MakeRef<Vivid::Shader>("./../assets/shaders/instancing.vertexShader.glsl",
                                        "./../assets/shaders/instancing.pixelShader.glsl");

        sphere = new Vivid::Mesh("./../assets/obj/newIcoSphere.obj", number_of_sphere);
        sphere->BindShader(shader);

        modelComponent1 = new Vivid::ModelComponent();
        modelComponent1->AddMesh(sphere);
        sphereTransformComponent->SetScale(Vivid::Maths::Vec3(scale));

        directionalLightComponent = new Vivid::DirectionalLightComponent();
        directionalLightComponent->SetDirection(Vivid::Maths::Vec3(-0.67f, 0.625f, 0.480f));
        directionalLightComponent->SetDiffuseColor(Vivid::Maths::Vec3(1.0f, 33.0f/255.0f, 33.0f/255.0f));
        directionalLightComponent->SetSpecularColor(Vivid::Maths::Vec3(1.0f, 220.0f/255.0f, 42.0f/255.0f));

        //
        Vivid::ECS::AddComponent(modelComponent1, suzanne);
        Vivid::ECS::AddComponent(sphereTransformComponent, suzanne);

        Vivid::ECS::AddComponent(directionalLightComponent, light);

        for (unsigned int i = 0; i < number_of_sphere; i++)
        {
            Vivid::Maths::Vec3 translation;

            // Make a grid of sphere
            translation.x = (float)(i % 6) * factor + 200.0f;
            translation.y = (float)(i / 6) * factor + 200.0f;
            translation.z = 0.0f;

            translations.push_back(translation);
        }
    }

    void Draw() override
    {
        Vector<Vivid::DirectionalLightComponent*> directionalLights = Vivid::ECS::GetAllComponents<Vivid::DirectionalLightComponent>();
        Vivid::Maths::Vec3 lightDiffuseColor = directionalLights[0]->GetDiffuseColor();
        Vivid::Maths::Vec3 lightSpecularColor = directionalLights[0]->GetSpecularColor();
        Vivid::Maths::Vec3 lightDir = directionalLights[0]->GetDirection();

        sphere->BindShader(shader);
        shader->SetUniform3f("LightDiffuseColor", lightDiffuseColor);
        shader->SetUniform3f("LightDir", lightDir);
        shader->SetUniform3f("LightSpecularColor", lightSpecularColor);
        shader->SetUniform3f("LightColor", lightColor);
        for (unsigned int i = 0; i < number_of_sphere; i++)
        {
            shader->SetUniform3f(("offsets[" + std::to_string(i) + "]"), translations[i]);
        }

        // Ray Cast to a sphere
        // A Better approach would be to use a object based buffer
        // An object based buffer essentially renders objects with their color to a hash of it's objectID. Using that we can get the objectID of the selected pixel and get the object.
        // But since it's just sphere, we can use ray cast to get the object pretty easily without modifying the renderer by a lot.
        if(InputHandler::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
        {
            Vivid::Maths::Vec2 mousePos = InputHandler::GetMousePosition();
            OrthoCamera* camera = static_cast<OrthoCamera*>(Application::GetInstance()->GetCamera());
            if (camera != nullptr)
            {
                Vivid::Maths::Vec3 rayPos = camera->RayCast(mousePos);

                // ImGUI paddings
                // These change according to zoom levels, hence there is no easy work around to get the exact position using imgui
                rayPos.x += -16.0859;
                rayPos.y += -38.375;
                for (unsigned int i = 0; i < number_of_sphere; i++)
                {
                    Vivid::Maths::Vec3 translation = translations[i];
                    float z2 = -(rayPos.x - translation.x) * (rayPos.x - translation.x) - (rayPos.y - translation.y) * (rayPos.y - translation.y) + radius * radius * scale * scale;
                    if (z2 >= 0.0f)
                    {
                        shader->SetUniform1f("isSelected[" + std::to_string(i) + "]", 1.0f);

                        // Get the exact 3D point in sphere
                        Vivid::Maths::Vec3 hitPoint = rayPos;

                        // Calculate the normal of the hit point
                        Vivid::Maths::Vec3 normal = hitPoint - translation;

                        // x^2 + y^2 + z^2 = r^2 => z = sqrt(r^2 - x^2 - y^2)
                        hitPoint.z = sqrt(std::max(0.0f, z2));

                        float mag = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
                        normal = normal * (1.0f / mag);
                        shader->SetUniform3f("hitNormal", normal);
                    }
                    else {
                        shader->SetUniform1f("isSelected[" + std::to_string(i) + "]", 0.0f);
                    }
                }
            }
        }
    }

    void ImGuiRender() override
    {
        ImGui::Begin("Debug");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }
};

Application* Vivid::CreateApplication()
{
    Vivid::Maths::Vec3 pos = Vivid::Maths::Vec3(0, 0, 0);
    float rot = 0.0f;
    float zoom = 1.0f;

    OrthoCamera* camera = new OrthoCamera(pos, rot, zoom, -1000.0f, 1000.0f);
    Application* app = Application::GetInstance(1920, 1080, "Vivid: ECS TEST Example", camera);

    app->SetRenderingInterface(new ExampleInterface);

    return app;
}

int main()
{
    return Vivid::main(0, nullptr);
}
