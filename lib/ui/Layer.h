//
// Created by ajaxian on 11/14/23.
//

#ifndef ORANGE_LAYER_H
#define ORANGE_LAYER_H

namespace Orange {

    class Layer {
    public:
        virtual ~Layer() = default;

        virtual void OnAttach() {}

        virtual void OnDetach() {}

        virtual void OnUpdate(float ts) {}

        virtual void OnUIRender() {}
    };

}

#endif //ORANGE_LAYER_H
