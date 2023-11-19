#include "../lib/ui/Application.h"
#include "../lib/EntryPoint.h"
#include "../lib/ui/Image.h"
#include "../lib/ui/UI.h"

class ExampleLayer : public Orange::Layer {
public:
    virtual void OnUIRender() override
    {
        ImGui::Begin("Hello");
        ImGui::Button("Button");
        ImGui::End();

        ImGui::ShowDemoWindow();

        UI_DrawAboutModal();
    }

    void UI_DrawAboutModal()
    {
        if (!m_AboutModalOpen)
            return;

        ImGui::OpenPopup("About");
        m_AboutModalOpen = ImGui::BeginPopupModal("About", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        if (m_AboutModalOpen)
        {
            auto image = Orange::Application::Get().GetApplicationIcon();
            ImGui::Image(image->GetDescriptorSet(), { 48, 48 });

            ImGui::SameLine();
            Orange::UI::ShiftCursorX(20.0f);

            ImGui::BeginGroup();
            ImGui::Text("Walnut application framework");
            ImGui::Text("by Studio Cherno.");
            ImGui::EndGroup();

            if (Orange::UI::ButtonCentered("Close"))
            {
                m_AboutModalOpen = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    void ShowAboutModal()
    {
        m_AboutModalOpen = true;
    }
private:
    bool m_AboutModalOpen = false;

};

Orange::Application *Orange::CreateApplication(int argc, char **argv) {
    Orange::ApplicationSpecification spec;
    spec.Name = "Orange Sample App";
    spec.CustomTitlebar = true;

    Orange::Application *app = new Orange::Application(spec);

    app->setConfigFileName("sampleapp.ini");
    app->setLogFileName("sampleapp.log");

    std::shared_ptr<ExampleLayer> exampleLayer = std::make_shared<ExampleLayer>();
    app->PushLayer(exampleLayer);
    app->SetMenubarCallback([app, exampleLayer]() {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit"))
            {
                app->Close();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About"))
            {
                exampleLayer->ShowAboutModal();
            }
            ImGui::EndMenu();
        }
    });

    return app;
}