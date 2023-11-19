//
// Created by ajaxian on 11/14/23.
//

#include "Application.h"
#include "ImGuiTheme.h"
#include "UI.h"
#include "../core/Log.h"

//
// Adapted from Dear ImGui OpenGL3 example and Walnut which was adapted from Dear ImGui Vulkan example
//


#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

#undef GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#ifdef INCLUDE_GL
#include <GL/gl.h>
#endif

#ifdef INCLUDE_OPENGL
#include <OpenGL/gl.h>
#endif

#include <glm/glm.hpp>

#include <iostream>

// Embedded font
#include "../fonts/Roboto-Regular.embed"
#include "../fonts/Roboto-Bold.embed"
#include "../fonts/Roboto-Italic.embed"
#include "imgui_internal.h"

// Embedded images
#include "../images/resources.embed"

extern bool g_ApplicationRunning;

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

//#define IMGUI_UNLIMITED_FRAME_RATE
#ifdef _DEBUG
#define IMGUI_VULKAN_DEBUG_REPORT
#endif

static void glfw_error_callback(int error, const char *description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static std::unordered_map<std::string, ImFont*> s_Fonts;

static Orange::Application *s_Instance = nullptr;

namespace Orange {

    Application::Application(const ApplicationSpecification &specification)
            : m_Specification(specification) {
        s_Instance = this;

        Init();
    }

    Application::~Application() {
        Shutdown();

        s_Instance = nullptr;
    }

    Application &Application::Get() {
        return *s_Instance;
    }

    void Application::Init() {
        // Setup GLFW window
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit()) {
            std::cerr << "Could not initalize GLFW!\n";
            return;
        }


        // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
        // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
        // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
        // GL 3.0 + GLSL 130
        const char *glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

        // Create window with graphics context
        // glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        if (m_Specification.CustomTitlebar)
        {
            glfwWindowHint(GLFW_TITLEBAR, false);

            // NOTE(Yan): Undecorated windows are probably
            //            also desired, so make this an option
            glfwWindowHint(GLFW_DECORATED, false);
//            glfwWindowHint(GLFW_FLOATING, true);
//            glfwWindowHint(GLFW_RESIZABLE, true);
        }

        m_WindowHandle = glfwCreateWindow(m_Specification.Width, m_Specification.Height, m_Specification.Name.c_str(),
                                          nullptr, nullptr);
        if (m_WindowHandle == nullptr) {
            std::cerr << "Could not initalize GLFW!\n";
            return;
        }

        glfwMakeContextCurrent(m_WindowHandle);
        glfwSwapInterval(1); // Enable vsync

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;



        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

        // Setup Dear ImGui style
//        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // Theme colors
        UI::SetHazelTheme();

        // Style
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowPadding = ImVec2(10.0f, 10.0f);
        style.FramePadding = ImVec2(8.0f, 6.0f);
        style.ItemSpacing = ImVec2(6.0f, 6.0f);
        style.ChildRounding = 6.0f;
        style.PopupRounding = 6.0f;
        style.FrameRounding = 6.0f;
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(m_WindowHandle, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        // Load default font
        ImFontConfig fontConfig;
        fontConfig.FontDataOwnedByAtlas = false;
        ImFont *robotoFont = io.Fonts->AddFontFromMemoryTTF((void *) g_RobotoRegular, sizeof(g_RobotoRegular), 20.0f,
                                                            &fontConfig);
        s_Fonts["Default"] = robotoFont;
        s_Fonts["Bold"] = io.Fonts->AddFontFromMemoryTTF((void*)g_RobotoBold, sizeof(g_RobotoBold), 20.0f, &fontConfig);
        s_Fonts["Italic"] = io.Fonts->AddFontFromMemoryTTF((void*)g_RobotoItalic, sizeof(g_RobotoItalic), 20.0f, &fontConfig);
        io.FontDefault = robotoFont;


        // Load images

        int w, h;
        void* data = Image::Decode(g_OrangeIcon, sizeof(g_OrangeIcon), &w, &h);
        m_AppHeaderIcon = std::make_shared<Orange::Image>(w, h, ImageFormat::RGBA, data, sizeof(g_OrangeIcon));
        free(data);

        data = Image::Decode(g_WindowMinimizeIcon, sizeof(g_WindowMinimizeIcon), &w, &h);
        m_IconMinimize = std::make_shared<Orange::Image>(w, h, ImageFormat::RGBA, data, sizeof(g_WindowMinimizeIcon));
        free(data);

        data = Image::Decode(g_WindowMaximizeIcon, sizeof(g_WindowMaximizeIcon), &w, &h);
        m_IconMaximize = std::make_shared<Orange::Image>(w, h, ImageFormat::RGBA, data, sizeof(g_WindowMaximizeIcon));
        free(data);

        data = Image::Decode(g_WindowRestoreIcon, sizeof(g_WindowRestoreIcon), &w, &h);
        m_IconRestore = std::make_shared<Orange::Image>(w, h, ImageFormat::RGBA, data, sizeof(g_WindowRestoreIcon));
        free(data);

        data = Image::Decode(g_WindowCloseIcon, sizeof(g_WindowCloseIcon), &w, &h);
        m_IconClose = std::make_shared<Orange::Image>(w, h, ImageFormat::RGBA, data, sizeof(g_WindowCloseIcon));
        free(data);
    }

    void Application::Shutdown() {
        for (auto &layer: m_LayerStack)
            layer->OnDetach();

        m_LayerStack.clear();


        // Release resources
        // NOTE(Yan): to avoid doing this manually, we shouldn't
        //            store resources in this Application class
        m_AppHeaderIcon.reset();
        m_IconClose.reset();
        m_IconMinimize.reset();
        m_IconMaximize.reset();
        m_IconRestore.reset();

        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(m_WindowHandle);
        glfwTerminate();

        g_ApplicationRunning = false;

        Log::Shutdown();
    }

    GLFWmonitor* Application::getMonitor(GLFWwindow* window) {
        GLFWmonitor* MonitorHandle = glfwGetWindowMonitor(window);
        int x,y,width,height;
        int wx,wy,wwidth,wheight;
        glfwGetWindowPos(window, &wx, &wy);
        glfwGetWindowSize(window, &wwidth, &wheight);

        if (MonitorHandle == NULL) {

            int count = 0;

            GLFWmonitor** monitors = glfwGetMonitors(&count);
            ImRect windowRect = ImRect((float) wx,(float) wy,(float) wwidth,(float) wheight);

            for (int index = 0; index < count; index++) {
                glfwGetMonitorWorkarea(monitors[index],&x,&y,&width, &height);

                ImRect monitorRect = ImRect((float) x,(float) y,(float) width,(float) height);
                if (monitorRect.Contains(windowRect)) {
                    MonitorHandle = monitors[index];
                }

//                        printf("x,y: %d, %d\nwidth,height: %d,%d\n", x,y,width,height);
            }

//                    printf("\nwx,wy: %d, %d\nwwidth,wheight: %d,%d\n", wx,wy,wwidth,wheight);
        }

        return MonitorHandle;
    }

    void Application::UI_DrawTitlebar(float& outTitlebarHeight)
    {
        const float titlebarHeight = 58.0f;
        const bool isMaximized = IsMaximized();
        float titlebarVerticalOffset = isMaximized ? -6.0f : 0.0f;
        const ImVec2 windowPadding = ImGui::GetCurrentWindow()->WindowPadding;

        ImGui::SetCursorPos(ImVec2(windowPadding.x, windowPadding.y + titlebarVerticalOffset));
        const ImVec2 titlebarMin = ImGui::GetCursorScreenPos();
        const ImVec2 titlebarMax = { ImGui::GetCursorScreenPos().x + ImGui::GetWindowWidth() - windowPadding.y * 2.0f,
                                     ImGui::GetCursorScreenPos().y + titlebarHeight };
        auto* bgDrawList = ImGui::GetBackgroundDrawList();
        auto* fgDrawList = ImGui::GetForegroundDrawList();
        bgDrawList->AddRectFilled(titlebarMin, titlebarMax, UI::Colors::Theme::titlebar);
        // DEBUG TITLEBAR BOUNDS
        // fgDrawList->AddRect(titlebarMin, titlebarMax, UI::Colors::Theme::invalidPrefab);

        // Logo
        {
            const int logoWidth = 48;// m_LogoTex->GetWidth();
            const int logoHeight = 48;// m_LogoTex->GetHeight();
            const ImVec2 logoOffset(16.0f + windowPadding.x, 5.0f + windowPadding.y + titlebarVerticalOffset);
            const ImVec2 logoRectStart = { ImGui::GetItemRectMin().x + logoOffset.x, ImGui::GetItemRectMin().y + logoOffset.y };
            const ImVec2 logoRectMax = { logoRectStart.x + logoWidth, logoRectStart.y + logoHeight };
            fgDrawList->AddImage(m_AppHeaderIcon->GetDescriptorSet(), logoRectStart, logoRectMax);
        }

        ImGui::BeginHorizontal("Titlebar", { ImGui::GetWindowWidth() - windowPadding.y * 2.0f, ImGui::GetFrameHeightWithSpacing() });

        static float moveOffsetX;
        static float moveOffsetY;
        const float w = ImGui::GetContentRegionAvail().x;
        const float buttonsAreaWidth = 94;

        // Title bar drag area
        // On Windows we hook into the GLFW win32 window internals
        ImGui::SetCursorPos(ImVec2(windowPadding.x, windowPadding.y + titlebarVerticalOffset)); // Reset cursor pos
        // DEBUG DRAG BOUNDS
        // fgDrawList->AddRect(ImGui::GetCursorScreenPos(), ImVec2(ImGui::GetCursorScreenPos().x + w - buttonsAreaWidth, ImGui::GetCursorScreenPos().y + titlebarHeight), UI::Colors::Theme::invalidPrefab);
        ImGui::InvisibleButton("##titleBarDragZone", ImVec2(w - buttonsAreaWidth, titlebarHeight));

        m_TitleBarHovered = ImGui::IsItemHovered();

        if (isMaximized)
        {
            float windowMousePosY = ImGui::GetMousePos().y - ImGui::GetCursorScreenPos().y;
            if (windowMousePosY >= 0.0f && windowMousePosY <= 5.0f)
                m_TitleBarHovered = true; // Account for the top-most pixels which don't register
        }

        // Draw Menubar
        if (m_MenubarCallback)
        {
            ImGui::SuspendLayout();
            {
                ImGui::SetItemAllowOverlap();
                const float logoHorizontalOffset = 16.0f * 2.0f + 48.0f + windowPadding.x;
                ImGui::SetCursorPos(ImVec2(logoHorizontalOffset, 6.0f + titlebarVerticalOffset));
                UI_DrawMenubar();

                if (ImGui::IsItemHovered())
                    m_TitleBarHovered = false;
            }

            ImGui::ResumeLayout();
        }

        {
            // Centered Window title
            ImVec2 currentCursorPos = ImGui::GetCursorPos();
            ImVec2 textSize = ImGui::CalcTextSize(m_Specification.Name.c_str());
            ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() * 0.5f - textSize.x * 0.5f, 2.0f + windowPadding.y + 6.0f));
            ImGui::Text("%s", m_Specification.Name.c_str()); // Draw title
            ImGui::SetCursorPos(currentCursorPos);
        }

        // Window buttons
        const ImU32 buttonColN = UI::Colors::ColorWithMultipliedValue(UI::Colors::Theme::text, 0.9f);
        const ImU32 buttonColH = UI::Colors::ColorWithMultipliedValue(UI::Colors::Theme::text, 1.2f);
        const ImU32 buttonColP = UI::Colors::Theme::textDarker;
        const float buttonWidth = 14.0f;
        const float buttonHeight = 14.0f;

        // Minimize Button

        ImGui::Spring();
        UI::ShiftCursorX(-8.0f);
        UI::ShiftCursorY(8.0f);
        {
            const int iconWidth = m_IconMinimize->GetWidth();
            const int iconHeight = m_IconMinimize->GetHeight();
            const float padY = (buttonHeight - (float)iconHeight) / 2.0f;
            ImGui::InvisibleButton("Minimize", ImVec2(buttonWidth, buttonHeight));

            if (UI::DrawButtonImage(m_IconMinimize, buttonColN, buttonColH, buttonColP, UI::RectExpanded(UI::GetItemRect(), 0.0f, -padY))) {
                // TODO: move this stuff to a better place, like Window class
                if (m_WindowHandle)
                {
                    glfwIconifyWindow(m_WindowHandle);
                }
            }
        }

        // Maximize Button
        ImGui::Spring(-1.0f, 17.0f);
        UI::ShiftCursorX(-8.0f);
        UI::ShiftCursorY(8.0f);
        {
            const int iconWidth = m_IconMaximize->GetWidth();
            const int iconHeight = m_IconMaximize->GetHeight();

            ImGui::InvisibleButton("Restore", ImVec2(buttonWidth, buttonHeight));

            if (UI::DrawButtonImage(m_IconRestore, buttonColN, buttonColH, buttonColP))
            {
                GLFWmonitor* MonitorHandle = getMonitor(m_WindowHandle);
                int x, y, width, height;

                if (MonitorHandle != NULL) {
                    glfwGetMonitorWorkarea(MonitorHandle, &x, &y, &width, &height);

                    x = width / 30;
                    y = height / 30;

                    width -= x * 2;
                    height -= y * 2;

                    glfwSetWindowPos(m_WindowHandle, x, y);
                    glfwSetWindowSize(m_WindowHandle, width, height);
                }
            }
        }

        // Maximize Button
        ImGui::Spring(-1.0f, 17.0f);
        UI::ShiftCursorX(-8.0f);
        UI::ShiftCursorY(8.0f);
        {
            const int iconWidth = m_IconMaximize->GetWidth();
            const int iconHeight = m_IconMaximize->GetHeight();

            ImGui::InvisibleButton("Maximize", ImVec2(buttonWidth, buttonHeight));

            if (UI::DrawButtonImage(m_IconMaximize, buttonColN, buttonColH, buttonColP))
            {
                GLFWmonitor* MonitorHandle = getMonitor(m_WindowHandle);
                int x,y,width,height;

                if (MonitorHandle != NULL) {
                    glfwGetMonitorWorkarea(MonitorHandle, &x, &y, &width, &height);
                    glfwSetWindowPos(m_WindowHandle, x, y);
                    glfwSetWindowSize(m_WindowHandle, width, height);
                }
            }
        }

        // Close Button
        ImGui::Spring(-1.0f, 15.0f);
        UI::ShiftCursorX(-8.0f);
        UI::ShiftCursorY(8.0f);
        {
            const int iconWidth = m_IconClose->GetWidth();
            const int iconHeight = m_IconClose->GetHeight();

            ImGui::InvisibleButton("Close", ImVec2(buttonWidth, buttonHeight));

            if (UI::DrawButtonImage(m_IconClose, UI::Colors::Theme::text, UI::Colors::ColorWithMultipliedValue(UI::Colors::Theme::text, 1.4f), buttonColP))
                Application::Get().Close();

        }

        ImGui::Spring(-1.0f, 18.0f);
        ImGui::EndHorizontal();

        outTitlebarHeight = titlebarHeight;
    }

    void Application::UI_DrawMenubar()
    {
        if (!m_MenubarCallback)
            return;

        if (m_Specification.CustomTitlebar)
        {
            const ImRect menuBarRect = { ImGui::GetCursorPos(), { ImGui::GetContentRegionAvail().x + ImGui::GetCursorScreenPos().x, ImGui::GetFrameHeightWithSpacing() } };

            ImGui::BeginGroup();
            if (UI::BeginMenubar(menuBarRect))
            {
                m_MenubarCallback();
            }

            UI::EndMenubar();
            ImGui::EndGroup();

        }
        else
        {
            if (ImGui::BeginMenuBar())
            {
                m_MenubarCallback();
                ImGui::EndMenuBar();
            }
        }
    }

    void Application::Run() {
        m_Running = true;

        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        ImGuiIO &io = ImGui::GetIO();

        // Main loop
        while (!glfwWindowShouldClose(m_WindowHandle) && m_Running) {
            // Poll and handle events (inputs, window resize, etc.)
            // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
            // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
            // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
            // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
            glfwPollEvents();

            for (auto &layer: m_LayerStack)
                layer->OnUpdate(m_TimeStep);

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            {
                static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

                // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
                // because it would be confusing to have two docking targets within each others.
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
                if (m_MenubarCallback)
                    window_flags |= ImGuiWindowFlags_MenuBar;

                const ImGuiViewport *viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->WorkPos);
                ImGui::SetNextWindowSize(viewport->WorkSize);
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                ImGuiWindowFlags_NoMove;
                window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
                if (!m_Specification.CustomTitlebar && m_MenubarCallback)
                    window_flags |= ImGuiWindowFlags_MenuBar;

                const bool isMaximized = IsMaximized();

                // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
                // and handle the pass-thru hole, so we ask Begin() to not render a background.
                if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                    window_flags |= ImGuiWindowFlags_NoBackground;

                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, isMaximized ? ImVec2(6.0f, 6.0f) : ImVec2(1.0f, 1.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);

                ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
                ImGui::Begin("DockSpaceWindow", nullptr, window_flags);
                ImGui::PopStyleColor(); // MenuBarBg
                ImGui::PopStyleVar(2);

                ImGui::PopStyleVar(2);

                {
                    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(50, 50, 50, 255));
                    // Draw window border if the window is not maximized
                    if (!isMaximized)
                        UI::RenderWindowOuterBorders(ImGui::GetCurrentWindow());

                    ImGui::PopStyleColor(); // ImGuiCol_Border
                }

                if (m_Specification.CustomTitlebar)
                {
                    float titleBarHeight;
                    UI_DrawTitlebar(titleBarHeight);
                    ImGui::SetCursorPosY(titleBarHeight);

                }

                // Dockspace
                ImGuiIO& io = ImGui::GetIO();
                ImGuiStyle& style = ImGui::GetStyle();
                float minWinSizeX = style.WindowMinSize.x;
                style.WindowMinSize.x = 370.0f;
                ImGui::DockSpace(ImGui::GetID("MyDockspace"));
                style.WindowMinSize.x = minWinSizeX;

                if (!m_Specification.CustomTitlebar)
                    UI_DrawMenubar();

                for (auto& layer : m_LayerStack)
                    layer->OnUIRender();

                ImGui::End();
            }

            // Rendering
            ImGui::Render();
            ImDrawData* main_draw_data = ImGui::GetDrawData();
            const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);
            int display_w, display_h;
            glfwGetFramebufferSize(m_WindowHandle, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w,
                         clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            if (!main_is_minimized)
                ImGui_ImplOpenGL3_RenderDrawData(main_draw_data);


            // Update and Render additional Platform Windows
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                GLFWwindow *backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }

            if (!main_is_minimized)
                glfwSwapBuffers(m_WindowHandle);
            else
                std::this_thread::sleep_for(std::chrono::milliseconds(5));

            float time = GetTime();
            m_FrameTime = time - m_LastFrameTime;
            m_TimeStep = glm::min<float>(m_FrameTime, 0.0333f);
            m_LastFrameTime = time;

            while(!m_EventQueue.empty()) {
                std::function<void()> func = m_EventQueue.front();
                m_EventQueue.pop();

                func();
            }
        }

    }

    void Application::Close() {
        m_Running = false;
    }

    bool Application::IsMaximized() const
    {
        return (bool)glfwGetWindowAttrib(m_WindowHandle, GLFW_MAXIMIZED);
    }

    float Application::GetTime() {
        return (float) glfwGetTime();
    }

    const char* Application::getConfigFileName() {
        ImGuiIO &io = ImGui::GetIO();
        (void) io;

        return io.IniFilename;
    }

    void Application::setConfigFileName(const char* name){
        ImGuiIO &io = ImGui::GetIO();
        (void) io;

        io.IniFilename = name;
    }

    const char* Application::getLogFileName(){
        ImGuiIO &io = ImGui::GetIO();
        (void) io;

        return io.LogFilename;
    }

    void Application::setLogFileName(const char* name) {
        ImGuiIO &io = ImGui::GetIO();
        (void) io;

        io.LogFilename = name;
    }
}