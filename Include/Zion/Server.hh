#pragma once
#include "Player.hh"
#include "Vehicle.hh"
#include "List.hh"
#include "GameMode.hh"
#include "TextDraw.hh"
#include "Pickup.hh"
#include "GangZone.hh"
#include "RakNet/RakServer.h"
#include <stdint.h>

namespace Zion {
    class ServerException : public std::exception {
        public:
            inline ServerException(const char *what) : what_(what) {}

            inline const char *what() const noexcept {
                return this->what_;
            }

            const char *what_;
    };

    class Server {
        friend class Player;
        friend class RPC;
        friend class Vehicle;
        friend class TextDraw;
        friend class Pickup;
        friend class GangZone;

        public:
            static void Start(uint16_t maxPlayers, uint16_t port = 7777, const char *hostname = nullptr);
            static void Stop();
            static void Update();
            static void SetServerName(const char *name);
            
            inline static Player *GetPlayerByIndex(unsigned short index) {
                if(index >= players.GetLength())
                    return nullptr;
                return players[index];
            }

            inline static Vehicle *GetVehicleByIndex(unsigned short index) {
                if(index >= vehicles.GetLength())
                    return nullptr;
                return vehicles[index];
            }

            inline static TextDraw *GetTextDrawByIndex(unsigned short index) {
                if(index >= textDraws.GetLength())
                    return nullptr;
                return textDraws[index];
            }

            inline static Pickup *GetPickupByIndex(unsigned int index) {
                if(index >= pickups.GetLength())
                    return nullptr;
                return pickups[index];
            }

            inline static GangZone *GetGangZoneByIndex(unsigned short index) {
                if(index >= gangzones.GetLength())
                    return nullptr;
                return gangzones[index];
            }

            inline static size_t AddPlayerClass(uint8_t team, int32_t model, Vector3 position,
                float angle, uint8_t weapon1, uint16_t weapon1Ammo, uint8_t weapon2,
                uint16_t weapon2Ammo, uint8_t weapon3, uint16_t weapon3Ammo) {

                return classes.Push({
                    team,
                    model,
                    0,
                    position,
                    angle,
                    {weapon1, weapon2, weapon3},
                    {weapon1Ammo, weapon2Ammo, weapon3Ammo}});
            }

            static bool                        zoneNames;
            static bool                        useCJAnims;
            static bool                        allowInteriorWeapons;
            static bool                        limitGlobalChatRadius;
            static float                       globalChatRadius;
            static bool                        stuntBonus;
            static float                       nameTagDrawDistance;
            static bool                        disableEnterExits;
            static bool                        nameTagLOS;
            static bool                        manualVehicleEngineAndLight;
            static bool                        showPlayerTags;
            static int32_t                     showPlayerMarkers;
            static uint8_t                     worldTime;
            static uint8_t                     weather;
            static float                       gravity;
            static int32_t                     deathDropMoney;
            static bool                        instagib;
            static int32_t                     lagCompensation;
            static int32_t                     vehicleFriendlyFire;
            static List<Player::SpawnInfo>     classes;
            static GameMode *                  gameMode;

        private:
            inline static bool RPC(unsigned char uniqueId, RakNet::BitStream *bitStream,
                PlayerID rakPlayerId, bool broadcast, PacketPriority priority = HIGH_PRIORITY,
                PacketReliability reliability = RELIABLE_ORDERED) {
                    
                return rakServer->RPC(uniqueId, bitStream, priority,
                    reliability, 0, rakPlayerId, broadcast, false,
                    UNASSIGNED_NETWORK_ID, nullptr);
            }

            inline static bool Send(RakNet::BitStream *bitStream, PlayerID rakPlayerId,
                bool broadcast, PacketPriority priority = HIGH_PRIORITY,
                PacketReliability reliability = RELIABLE_ORDERED) {

                return rakServer->Send(bitStream, priority,
                    reliability, 0, rakPlayerId, broadcast);
            }

            static void HandleQuery(SOCKET sock, sockaddr_in addr, uint8_t *buffer, size_t size);
            static Player *AddPlayer(PlayerID rakId, uint16_t index, const char *name);
            static void SendPlayerListToPlayer(Player *player);
            static void SendVehicleListToPlayer(Player *player);
            static void SendPickupListToPlayer(Player *player);
            static void RemovePlayer(Player *player, int reason);
            static void InitGameForPlayer(Player *player);
            static void Packet_AimSync(Player *player, uint8_t *data, size_t size);
            static void Packet_PlayerSync(Player *player, uint8_t *data, size_t size);
            static void Packet_BulletSync(Player *player, uint8_t *data, size_t size);
            static void Packet_VehicleSync(Player *player, uint8_t *data, size_t size);
            static void Packet_PassengerSync(Player *player, uint8_t *data, size_t size);
            static void Packet_WeaponsUpdate(Player *player, uint8_t *data, size_t size);
            static void Packet_StatsUpdate(Player *player, uint8_t *data, size_t size);

            static RakServer *                 rakServer;
            static List<Player *>              players;
            static List<Vehicle *>             vehicles;
            static List<TextDraw *>            textDraws;
            static List<Pickup *>              pickups;
            static List<GangZone *>            gangzones;
            static char *                      name;
            static uint8_t                     nameLength;
            static uint16_t                    maxPlayers;
            static uint16_t                    playersOnline;
    };
};
