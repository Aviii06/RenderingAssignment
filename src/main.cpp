#include "Vivid.h"
#include "common/maths/Vec.h"

const int number_of_sphere = 36;
float sphere_radius = 50.0f;
float space_between_spheres = 300.0f;

class ExampleInterface : public RenderingInterface
{

public:
	void Setup() override
	{
		// Can write custom opengl confs here
		OPENGL_CONFS

		shader = MakeRef<Vivid::Shader>(
		    "./../assets/shaders/instancing.vertexShader.glsl",
		    "./../assets/shaders/instancing.pixelShader.glsl");

		sphere = new Vivid::Mesh("./../assets/obj/newIcoSphere.obj", number_of_sphere);
		sphere->BindShader(shader);

		modelComponent1 = new Vivid::ModelComponent();
		modelComponent1->AddMesh(sphere);
		sphereTransformComponent->SetScale(Vivid::Maths::Vec3(sphere_radius));

		directionalLightComponent = new Vivid::DirectionalLightComponent();
		directionalLightComponent->SetDirection(
		    Vivid::Maths::Vec3(-0.67f, 0.625f, 0.480f));
		directionalLightComponent->SetDiffuseColor(
		    Vivid::Maths::Vec3(1.0f, 33.0f / 255.0f, 33.0f / 255.0f));
		directionalLightComponent->SetSpecularColor(
		    Vivid::Maths::Vec3(1.0f, 220.0f / 255.0f, 42.0f / 255.0f));

		Vivid::ECS::AddComponent(modelComponent1, suzanne);
		Vivid::ECS::AddComponent(sphereTransformComponent, suzanne);

		Vivid::ECS::AddComponent(directionalLightComponent, light);

		// Initialize the translations
		for (unsigned int i = 0; i < number_of_sphere; i++)
		{
			Vivid::Maths::Vec3 translation;

			// Make a grid of sphere
			translation.x = (float)(i % 6) * space_between_spheres + 200.0f;
			translation.y = (float)(i / 6) * space_between_spheres + 200.0f;
			translation.z = 0.0f;

			translations.push_back(translation);
			translations[i] = translation;
		}
	}

	void Draw() override
	{
		sphereTransformComponent->SetScale(Vivid::Maths::Vec3(sphere_radius));
		for (unsigned int i = 0; i < number_of_sphere; i++)
		{
			Vivid::Maths::Vec3 translation;

			// Make a grid of sphere
			translation.x = (float)(i % 6) * space_between_spheres + 200.0f;
			translation.y = (float)(i / 6) * space_between_spheres + 200.0f;
			translation.z = 0.0f;

			translations[i] = translation;
		}

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
			shader->SetUniform3f(("offsets[" + std::to_string(i) + "]"),
			    translations[i]);
		}

		// Ray Cast to a sphere
		// A Better approach would be to use a object based buffer
		// An object based buffer essentially renders objects with their color to a
		// hash of it's objectID. Using that we can get the objectID of the selected
		// pixel and get the object. But since it's just sphere, we can use ray cast
		// to get the object pretty easily without modifying the renderer by a lot.
		if (InputHandler::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
		{
			Vivid::Maths::Vec2 mousePos = InputHandler::GetMousePosition();

			// finding the mouse position in the viewport
			mousePos.x -= Application::GetInstance()->GetWindow().GetViewportPosition().x;
			mousePos.y -= Application::GetInstance()->GetWindow().GetViewportPosition().y;
			mousePos.y = Application::GetInstance()->GetWindow().GetViewportHeight() - mousePos.y;

			Vivid::Maths::Vec3 hitPos = Vivid::Maths::Vec3(mousePos.x * 2, mousePos.y * 2, 0.0f);

			// Simply see if the ray intersects with the sphere by iterating through all the spheres
			// Better approach would be to use a bounding box to check if the ray intersects with the bounding box
			for (unsigned int i = 0; i < number_of_sphere; i++)
			{
				Vivid::Maths::Vec3 translation = translations[i];

				// x^2 + y^2 + z^2 = r^2 => z = sqrt(r^2 - x^2 - y^2)
				float z2 = sphere_radius * sphere_radius - (hitPos.x - translation.x) * (hitPos.x - translation.x) - (hitPos.y - translation.y) * (hitPos.y - translation.y);
				if (z2 >= 0.0f)
				{
					shader->SetUniform1f("isSelected[" + std::to_string(i) + "]", 1.0f);

					// x^2 + y^2 + z^2 = r^2 => z = sqrt(r^2 - x^2 - y^2)
					hitPos.z = sqrt(std::max(0.0f, z2));
					Vivid::Maths::Vec3 normal = hitPos - translation;

					float mag = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
					normal = normal * (1.0f / mag);
					shader->SetUniform3f("hitNormal", normal);
				}
				else
				{
					shader->SetUniform1f("isSelected[" + std::to_string(i) + "]", 0.0f);
				}
			}
		}
	}

	void ImGuiRender() override
	{
		ImGui::Begin("Debug");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
		    1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::SliderFloat("Sphere Radius", &sphere_radius, 0.0f, 200.0f);
		ImGui::SliderFloat("Space Between Spheres", &space_between_spheres, 0.0f, 500.0f);
		ImGui::End();
	}

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
};

Application* Vivid::CreateApplication()
{
	Vivid::Maths::Vec3 pos = Vivid::Maths::Vec3(0, 0, 0);
	float rot = 0.0f;
	float zoom = 1.0f;

	OrthoCamera* camera = new OrthoCamera(pos, rot, zoom, -1000.0f, 1000.0f);
	Application* app = Application::GetInstance(1920, 1080, "Rendering Assignment", camera);

	app->SetRenderingInterface(new ExampleInterface);

	return app;
}

int main() { return Vivid::main(0, nullptr); }
