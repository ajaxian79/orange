#include "../lib/ui/Application.h"
#include "../lib/EntryPoint.h"

class ExampleLayer : public Orange::Layer {
public:
    virtual void OnUIRender() override {
        ImGui::Begin("Hello");
        ImGui::Button("Button");
        ImGui::End();

        ImGui::ShowDemoWindow();
    }
};

Orange::Application *Orange::CreateApplication(int argc, char **argv) {
    Orange::ApplicationSpecification spec;
    spec.Name = "Orange Sample App";

    Orange::Application *app = new Orange::Application(spec);
    app->PushLayer<ExampleLayer>();
    app->SetMenubarCallback([app]() {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit")) {
                app->Close();
            }
            ImGui::EndMenu();
        }
    });
    return app;
}