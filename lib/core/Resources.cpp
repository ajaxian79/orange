//
// Created by Blake Tullysmith on 11/19/23.
//

#include "Resources.h"

typedef unsigned char uint8_t;

// Embedded font
#include "../fonts/Roboto-Regular.embed"
#include "../fonts/Roboto-Bold.embed"
#include "../fonts/Roboto-Italic.embed"

// Embedded images
#include "../images/resources.embed"

#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort

#include <iostream>

#include "imgui_internal.h"


static std::unordered_map<std::string, ImFont *> s_Fonts;

std::shared_ptr<Orange::Image> Orange::Resources::iconOrange;
std::shared_ptr<Orange::Image> Orange::Resources::iconClose;
std::shared_ptr<Orange::Image> Orange::Resources::iconMinimize;
std::shared_ptr<Orange::Image> Orange::Resources::iconMaximize;
std::shared_ptr<Orange::Image> Orange::Resources::iconRestore;

namespace Orange {

  void Resources::loadFonts() {

    ImGuiIO &io = ImGui::GetIO();
    (void) io;

    // Load default font
    ImFontConfig fontConfig;
    fontConfig.FontDataOwnedByAtlas = false;
    ImFont *robotoFont = io.Fonts->AddFontFromMemoryTTF((void *) g_RobotoRegular, sizeof(g_RobotoRegular), 20.0f,
                                                        &fontConfig);
    s_Fonts["Default"] = robotoFont;
    s_Fonts["Bold"] = io.Fonts->AddFontFromMemoryTTF((void *) g_RobotoBold, sizeof(g_RobotoBold), 20.0f, &fontConfig);
    s_Fonts["Italic"] = io.Fonts->AddFontFromMemoryTTF((void *) g_RobotoItalic, sizeof(g_RobotoItalic), 20.0f,
                                                       &fontConfig);
    io.FontDefault = robotoFont;

    io.Fonts->AddFontDefault(&fontConfig);
    io.Fonts->Build();

  }

  void Resources::loadImages() {
    int w, h;
    void *data = Image::Decode(g_OrangeIcon, sizeof(g_OrangeIcon), &w, &h);
    iconOrange = std::make_shared<Orange::Image>(w, h, ImageFormat::RGBA, data, sizeof(g_OrangeIcon));
    free(data);

    data = Image::Decode(g_WindowMinimizeIcon, sizeof(g_WindowMinimizeIcon), &w, &h);
    iconMinimize = std::make_shared<Orange::Image>(w, h, ImageFormat::RGBA, data, sizeof(g_WindowMinimizeIcon));
    free(data);

    data = Image::Decode(g_WindowMaximizeIcon, sizeof(g_WindowMaximizeIcon), &w, &h);
    iconMaximize = std::make_shared<Orange::Image>(w, h, ImageFormat::RGBA, data, sizeof(g_WindowMaximizeIcon));
    free(data);

    data = Image::Decode(g_WindowRestoreIcon, sizeof(g_WindowRestoreIcon), &w, &h);
    iconRestore = std::make_shared<Orange::Image>(w, h, ImageFormat::RGBA, data, sizeof(g_WindowRestoreIcon));
    free(data);

    data = Image::Decode(g_WindowCloseIcon, sizeof(g_WindowCloseIcon), &w, &h);
    iconClose = std::make_shared<Orange::Image>(w, h, ImageFormat::RGBA, data, sizeof(g_WindowCloseIcon));
    free(data);
  }

  void Resources::load() {
    loadFonts();
    loadImages();
  }

  void Resources::release() {
    iconOrange.reset();
    iconClose.reset();
    iconMinimize.reset();
    iconMaximize.reset();
    iconRestore.reset();
  }

  const std::shared_ptr<Orange::Image> &Resources::getIconOrange() {
    return iconOrange;
  }

  const std::shared_ptr<Orange::Image> &Resources::getIconClose() {
    return iconClose;
  }

  const std::shared_ptr<Orange::Image> &Resources::getIconMinimize() {
    return iconMinimize;
  }

  const std::shared_ptr<Orange::Image> &Resources::getIconMaximize() {
    return iconMaximize;
  }

  const std::shared_ptr<Orange::Image> &Resources::getIconRestore() {
    return iconRestore;
  }

} // Orange