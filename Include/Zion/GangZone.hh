#pragma once
#include "Base.hh"
#include "Player.hh"
#include "List.hh"
#include <stdint.h>

namespace Zion {
    class GangZone {
        public:
            GangZone(Vector2 min, Vector2 max);
            ~GangZone();

            void FlashForAll(uint32_t color);
            void FlashForPlayer(Player *player, uint32_t color);
            void HideForAll();
            void HideForPlayer(Player *player);
            void ShowForAll(uint32_t color);
            void ShowForPlayer(Player *player, uint32_t color);
            void StopFlashForAll();
            void StopFlashForPlayer(Player *player);

            inline uint16_t GetIndex() {
                return this->index;
            }

        private:
            uint16_t index;
            Vector2 min;
            Vector2 max;

            static uint16_t nextId;
            static List<uint16_t> freeIds;
    };
};