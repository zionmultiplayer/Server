#pragma once
#include "Base.hh"
#include "List.hh"
#include "Util.hh"
#include <stdint.h>

namespace Zion {
    class Player;
    
    class Vehicle {
        friend class Server;
        friend class RPC;
        friend class Player;

        public:
            Vehicle(uint16_t model, float health, Vector3 position, float zAngle,
                uint8_t color1, uint8_t color2);
            ~Vehicle();

            void SetParams(uint8_t objective, uint8_t doorsLocked);
            void LinkVehicleToInterior(uint8_t interior);
            void SetParamsForPlayer(Player *player, uint8_t objective, uint8_t doorsLocked);
            void SetPosition(Vector3 position);
            void SetVelocity(Vector3 velocity);
            void SetNumberplate(const char *numberplate);
            void SetHealth(float health);

            void SetZAngle(float angle);

            inline void GetDamageStatus(int *panels, int *doors, int *lights, int *tires) {
                *panels = this->panelDamageStatus;
                *doors = this->doorDamageStatus;
                *lights = this->lightDamageStatus;
                *tires = this->tireDamageStatus;
            }

            inline float GetDistanceFromPoint(Vector3 point) {
                return Util::GetDistance(this->position, point);
            }

            inline Vector4 GetQuaternion() {
                return this->quaternion;
            }

            inline Vector3 GetVelocity() {
                return this->velocity;
            }

            inline float GetZAngle() {
                return Util::RadiansToDegrees(Util::GetAngles(this->quaternion).z);
            }

            inline uint16_t GetModel() {
                return this->model;
            }

            inline Vector3 GetPosition() {
                return this->position;
            }

            inline float GetHealth() {
                return this->health;
            }

            inline uint16_t GetIndex() {
                return this->index;
            }

        private:
            static uint16_t nextId;
            static List<uint16_t> freeIds;

            void AddForWorld();
            void AddForPlayer(Player *player);
            void RemoveForWorld();
            void SendDamangeForPlayer(Player *player);

            uint16_t    index;
            uint16_t    model;
            Vector3     position;
            Vector3     velocity;
            Vector4     quaternion;
            uint8_t     color1;
            uint8_t     color2;
            float       health;
            uint8_t     engine;
            uint8_t     lights;
            uint8_t     alarm;
            uint8_t     bonnet;
            uint8_t     boot;
            uint8_t     objective;
            uint8_t     doorsLocked;
            bool        sirenOn;
            uint32_t    panelDamageStatus;
            uint32_t    doorDamageStatus;
            uint8_t     lightDamageStatus;
            uint8_t     tireDamageStatus;
            uint8_t     paintjob;
    };
};