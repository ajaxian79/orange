//
// Created by Blake Tullysmith on 11/19/23.
//

#ifndef ORANGE_RESOURCES_H
#define ORANGE_RESOURCES_H

#include <memory>
#include "../ui/Image.h"

namespace Orange {
    class Resources {
    public:
        static void load();
        static void release();

        static const std::shared_ptr<Orange::Image> &getIconOrange();

        static const std::shared_ptr<Orange::Image> &getIconClose();

        static const std::shared_ptr<Orange::Image> &getIconMinimize();

        static const std::shared_ptr<Orange::Image> &getIconMaximize();

        static const std::shared_ptr<Orange::Image> &getIconRestore();

    private:
        static void loadFonts();
        static void loadImages();
        static std::shared_ptr<Orange::Image> iconOrange;
        static std::shared_ptr<Orange::Image> iconClose;
        static std::shared_ptr<Orange::Image> iconMinimize;
        static std::shared_ptr<Orange::Image> iconMaximize;
        static std::shared_ptr<Orange::Image> iconRestore;
    };

} // Orange

#endif //ORANGE_RESOURCES_H
