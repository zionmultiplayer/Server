#pragma once
#include "TextDraw.hh"
#include <stdint.h>

namespace Zion {
    class ProgressBar {
        public:
            ProgressBar(
                Vector2 position, float width = 55.5F, float height = 3.2F,
                float max = 100.0F, float value = 0.0F, uint32_t color = 0xFE4066FF);

            void ShowForAll();
            void ShowForPlayer(Player *player);
            void HideForAll();
            void HideForPlayer(Player *player);
            void Update();

            Vector2 position;
            float width;
            float height;
            float max;
            float value;
            uint32_t color;

        private:
            float Percent(float x, float width, float max, float value);

            TextDraw *back;
            TextDraw *fill;
            TextDraw *main;
    };
};