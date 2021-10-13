#pragma once
#include "Base.hh"
#include "Player.hh"
#include <stdint.h>

namespace Zion {
    PACK(struct TextDrawTransmit {
            struct {
                uint8_t box : 1;
                uint8_t left : 1;
                uint8_t right : 1;
                uint8_t center : 1;
                uint8_t proportional : 1;
                uint8_t padding : 3;
            };

            float letterWidth;
            float letterHeight;
            uint32_t letterColor;
            float lineWidth;
            float lineHeight;
            uint32_t boxColor;
            uint8_t shadow;
            uint8_t outline;
            uint32_t backgroundColor;
            uint8_t style;
            uint8_t selectable;
            float x;
            float y;
            uint16_t model;
            Vector3 rotation;
            float zoom;
            uint16_t color1;
            uint16_t color2;
    });

    class TextDraw {
        public:
            TextDraw(Vector2 position, const char *text);
            ~TextDraw();

            void SetPosition(Vector2 position);
            void SetLetterSize(float width, float height);
            void SetLineSize(float width, float height);
            void SetAlignment(int alignment);
            void SetLetterColor(uint32_t color);
            void UseBox(bool use);
            void SetBoxColor(uint32_t color);
            void SetShadow(uint8_t size);
            void SetOutline(uint8_t size);
            void SetBackgroundColor(uint32_t color);
            void SetStyle(uint8_t style);
            void SetProportional(bool proportional);
            void SetSelectable(uint8_t set);
            void SetText(const char *text);
            void SetPreviewModel(int modelId);
            void SetPreviewRotation(Vector3 rotation, float zoom) ;
            void SetPreviewVehicleColor(uint8_t color1, uint8_t color2);
            void UpdateForPlayer(Player *player);
            void UpdateForAll();
            void ShowForAll();
            void HideForAll();
            void ShowForPlayer(Player *player);
            void HideForPlayer(Player *player);

            inline uint16_t GetIndex() {
                return this->index;
            }

            TextDrawTransmit tdt;

        private:
            uint16_t index;
            char *text;
            uint8_t textLength;

            static List<uint16_t> freeIds;
            static uint16_t nextId;
    };
};