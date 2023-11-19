//
// Created by ajaxian on 11/14/23.
//

#ifndef ORANGE_APPLICATION_H
#define ORANGE_APPLICATION_H

#include "Layer.h"

#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <memory>
#include <functional>
#include <filesystem>
#include "Image.h"

#include "imgui.h"
#include "orange/lib/core/Resources.h"

struct GLFWwindow;
struct GLFWmonitor;

namespace Orange {

    struct ApplicationSpecification {
        std::string Name = "Walnut App";
        uint32_t Width = 1600;
        uint32_t Height = 900;

        std::filesystem::path IconPath;

        bool WindowResizeable = true;

        // Uses custom Walnut titlebar instead
        // of Windows default
        bool CustomTitlebar = false;

        // Window will be created in the center
        // of primary monitor
        bool CenterWindow = false;
    };

    class Application {
    public:
        Application(const ApplicationSpecification &applicationSpecification = ApplicationSpecification());

        ~Application();

        static Application &Get();

        void Run();

        void SetMenubarCallback(const std::function<void()> &menubarCallback) { m_MenubarCallback = menubarCallback; }

        template<typename T>
        void PushLayer() {
            static_assert(std::is_base_of<Layer, T>::value, "Pushed type is not subclass of Layer!");
            m_LayerStack.emplace_back(std::make_shared<T>())->OnAttach();
        }

        void PushLayer(const std::shared_ptr<Layer> &layer) {
            m_LayerStack.emplace_back(layer);
            layer->OnAttach();
        }

        void Close();

        float GetTime();

        GLFWwindow *GetWindowHandle() const { return m_WindowHandle; }

        bool IsTitleBarHovered() const { return m_TitleBarHovered; }

        bool IsMaximized() const;
        std::shared_ptr<Image> GetApplicationIcon() const { return Resources::getIconOrange(); }

        static void SubmitResourceFree(std::function<void()> &&func);

        const char* getConfigFileName();

        void setConfigFileName(const char* name);

        const char* getLogFileName();

        void setLogFileName(const char* name);

        template<typename Func>
        void QueueEvent(Func&& func)
        {
            m_EventQueue.push(func);
        }
    private:
        void Init();

        void Shutdown();

        // For custom titlebars
        void UI_DrawTitlebar(float& outTitlebarHeight);
        void UI_DrawMenubar();
        GLFWmonitor* getMonitor(GLFWwindow* window);
    private:
        ApplicationSpecification m_Specification;
        GLFWwindow *m_WindowHandle = nullptr;
        bool m_Running = false;

        float m_TimeStep = 0.0f;
        float m_FrameTime = 0.0f;
        float m_LastFrameTime = 0.0f;

        bool m_TitleBarHovered = false;

        std::vector<std::shared_ptr<Layer>> m_LayerStack;
        std::function<void()> m_MenubarCallback;

        std::mutex m_EventQueueMutex;
        std::queue<std::function<void()>> m_EventQueue;

    };

    Application *CreateApplication(int argc, char **argv);
}

#endif //ORANGE_APPLICATION_H
