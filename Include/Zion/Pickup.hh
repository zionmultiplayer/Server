#pragma once
#include "Base.hh"
#include "Player.hh"
#include "List.hh"
#include <stdint.h>

namespace Zion {
    PACK(struct PickupTransmit {
        int32_t model;
        int32_t type;
        Vector3 position;
    });

    class Pickup {
        friend class Server;

        public:
            Pickup(int32_t model, int32_t type, Vector3 position, Player *player = nullptr);
            ~Pickup();

            void Show();
            void Hide();

            inline int32_t GetIndex() {
                return this->index;
            }

        private:
            void ShowForPlayer(Player *player);

            int32_t index;
            PickupTransmit pt;
            Player *player;
            bool showing;

            static List<uint16_t> freeIds;
            static uint16_t nextId;
    };
};