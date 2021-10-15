#pragma once
#include "Base.hh"
#include "Vehicle.hh"
#include "RakNet/NetworkTypes.h"
#include <stdint.h>
#include <exception>

namespace Zion {
    class PlayerException : public std::exception {
        public:
            inline PlayerException(const char *what) : what_(what) {}

            inline const char *what() const noexcept {
                return this->what_;
            }

            const char *what_;
    };

    class Player  {
        friend class Server;
        friend class RPC;
        friend class Vehicle;
        friend class TextDraw;
        friend class Pickup;
        friend class GangZone;

        public:
            struct State {
                enum {
                    None,
                    OnFoot,
                    Driver,
                    Passenger,
                    Wasted,
                    Spectating,
                };
            };

            void Spawn();
            void SetPosition(Vector3 position);
            void SetPositionFindZ(Vector3 position);
            Vector3 GetPosition();
            void SetFacingAngle(float angle);
            float GetFacingAngle();
            float GetDistanceFromPoint(Vector3 point);
            void SetInterior(uint8_t interior);
            uint8_t GetInterior();
            void SetHealth(float health);
            float GetHealth();
            void SetArmour(float armour);
            float GetArmour();
            void SetAmmo(uint8_t weaponSlot, uint16_t ammo);
            void SetTeam(uint8_t team);
            uint8_t GetTeam();
            void SetDrunkLevel(int32_t level);
            int32_t GetDrunkLevel();
            void SetColor(uint32_t color);
            uint32_t GetColor();
            void SetSkin(uint32_t skin);
            uint32_t GetSkin();
            void GiveWeapon(int32_t weaponId, int32_t weaponAmmo);
            void ResetWeapons();
            void SetArmedWeapon(int32_t weaponSlot);
            void GiveMoney(int32_t money);
            void ResetMoney();
            void SetName(const char *name);
            int32_t GetMoney();
            uint8_t GetState();
            int32_t GetPing();
            bool GetKeys(int *lr, int *ud, int *keys);
            const char *GetName();
            void SetTime(uint8_t hour, uint8_t minute);
            void ToggleClock(bool toggle);
            void SetWeather(uint8_t weather);
            void ForceClassSelection();
            void SetWantedLevel(uint8_t level);
            uint8_t GetWantedLevel();
            void SetFightingStyle(uint8_t style);
            uint8_t GetFightingStyle();
            void SetVelocity(Vector3 velocity);
            Vector3 GetVelocity();
            void PlayAudioStream(const char *url, Vector3 position, float radius, bool usePosition);
            void StopAudioStream();
            void SetSkillLevel(uint32_t skill, uint16_t level);
            void RemoveBuilding(uint32_t model, Vector3 position, float radius);
            void SetAttachedObject(uint32_t index, uint32_t model, uint32_t bone, Vector3 offset, Vector3 rotation, Vector3 scale);
            void RemoveAttachedObject(uint32_t index);
            bool IsAttachedObjectSlotUsed(uint32_t index);
            void EditAttachedObject(uint32_t index);
            void PutInVehicle(Vehicle *vehicle, uint8_t seat);
            Vehicle *GetVehicle();
            int8_t GetVehicleSeat();
            void RemoveFromVehicle();
            void ToggleControllable(bool toggle);
            void PlaySound_(uint32_t sound, Vector3 position);
            void ApplyAnimation(const char *lib, const char *name, float delta, bool loop, bool lockx, bool locky, bool freeze, int32_t time);
            void ClearAnimations();
            uint8_t GetSpecialAction();
            void SetSpecialAction(uint8_t specialAction);
            void SetCheckpoint(Vector3 position, float radius);
            void DisableCheckpoint();
            void SetRaceCheckpoint(uint8_t type, Vector3 position, Vector3 nextPosition, float radius);
            void DisableRaceCheckpoint();
            void SetWorldBounds(Vector2 max, Vector2 min);
            void SetColorForPlayer(Player *player, uint32_t color);
            void ShowNameTagForPlayer(Player *player, bool show);
            void SetMapIcon(uint8_t index, Vector3 position, uint8_t icon, uint32_t color, uint8_t style);
            void RemoveMapIcon(uint8_t index);
            void SetCameraPosition(Vector3 position);
            void SetCameraLookAt(Vector3 position);
            void SetCameraBehind();
            void InterpolateCameraPosition(Vector3 from, Vector3 to, int32_t time, uint8_t mode);
            void InterpolateCameraLookAt(Vector3 from, Vector3 to, int32_t time, uint8_t mode);
            bool IsInCheckpoint();
            bool IsInRaceCheckpoint();
            void EnableStuntBonus(bool enable);
            void ToggleSpectating(bool toggle);
            void SpectatePlayer(Player *player, uint8_t mode);
            void SpectateVehicle(Vehicle *vehicle, uint8_t mode);
            void CreateExplosion(Vector3 position, int32_t type, float radius);
            void SendClientMessage(uint32_t color, const char *format, ...);
            void GameText(int32_t style, int32_t time, const char *format, ...);
            void EnableCursor(bool enable, uint32_t color);
            void ShowDialog(uint16_t id, uint8_t style, const char *title, const char *button1, const char *button2, const char *info);
            bool IsConnected();
            bool IsNPC();
            void Kick();
            uint8_t GetWeapon();
            void Ban();
            void Ban(const char *reason);
            void ChatBubble(const char *text, uint32_t color, float drawDistance, uint32_t expireTime);

            inline uint16_t GetIndex() {
                return this->index;
            }

            int32_t score;
            void *data;
            
        private:
            PACK(struct OnFootInfo {
                uint16_t    lrAnalog;
                uint16_t    udAnalog;
                uint16_t    keys;
                Vector3     position;
                Vector4     quaternion;
                uint8_t     health;
                uint8_t     armour;
                uint8_t     currentWeapon;
                uint8_t     specialAction;
                Vector3     moveSpeed;
                Vector3     surfOffsets;
                uint16_t    surfInfo;
                int32_t     animation;
            });

            PACK(struct InCarInfo {
                uint16_t    vehicleId;
                uint16_t    lrAnalog;
                uint16_t    udAnalog;
                uint16_t    keys;
                Vector4     quaternion;
                Vector3     position;
                Vector3     moveSpeed;
                float       carHealth;
                uint8_t     playerHealth;
                uint8_t     playerArmour;
                uint8_t     currentWeapon;
                uint8_t     sirenOn;
                uint8_t     landingGearState;
                uint16_t    trailerIdOrThrustAngle;
                float       trainSpeed;
            });

            PACK(struct SpawnInfo {
                uint8_t     team;
                int32_t     skin;
                uint8_t     unknown;
                Vector3     position;
                float       zAngle;
                int32_t     spawnWeapons[3];
                int32_t     spawnWeaponsAmmo[3];
            });

            PACK(struct AimInfo {
                uint8_t     camMode;
                Vector3     aimf1;
                Vector3     aimPosition;
                float       aimZ;
                uint8_t     camExtZoom : 6;
                uint8_t     weaponState : 2;
                uint8_t     aspectRatio;
            });

            PACK(struct BulletInfo {
                uint8_t     hitType;
                uint16_t    hitId;
                Vector3     hitOrigin;
                Vector3     hitTarget;
                Vector3     centerOfHit;
                uint8_t     weaponId;
            });

            PACK(struct PassengerInfo {
                uint16_t    vehicleId;
                uint8_t     seatFlags : 7;
                uint8_t     driveBy : 1;
                uint8_t     currentWeapon;
                uint8_t     playerHealth;
                uint8_t     playerArmour;
                uint16_t    lrAnalog;
                uint16_t    udAnalog;
                uint16_t    keys;
                Vector3     position;
            });

            struct AttachedObject {
                uint32_t    model;
                uint32_t    bone;
                Vector3     offset;
                Vector3     rotation;
                Vector3     scale;
                bool        used;
            };

            struct WeaponInfo {
                uint8_t     weapon;
                uint16_t    ammo;
            };

            struct CheckpointInfo {
                bool        active;
                bool        in;
                float       radius;
                Vector3     position;
            };

            struct RaceCheckpointInfo {
                bool        active;
                bool        in;
                float       radius;
                Vector3     position;
                Vector3     nextPosition;
            };

            Player(PlayerID rakId, uint16_t index, const char *name);
            ~Player();

            void SendOnFootInfo();
            void SendInCarInfo();
            void SendAimInfo();
            void SendBulletInfo();
            void SendPassengerInfo();
            void Update();
            void SetState(uint8_t state);
            void AddForPlayer(Player *player);
            void AddForWorld();
            void Spawn(uint8_t spawn);

            OnFootInfo onFootInfo;
            InCarInfo inCarInfo;
            SpawnInfo spawnInfo;
            AimInfo aimInfo;
            BulletInfo bulletInfo;
            PassengerInfo passengerInfo;
            AttachedObject attachedObject;
            WeaponInfo weaponInfo[13];
            CheckpointInfo checkpointInfo;
            RaceCheckpointInfo raceCheckpointInfo;

            PlayerID rakId;
            uint16_t index;
            uint8_t state;
            Vector3 position;
            Vector4 quaternion;
            char *name;
            uint8_t nameLength;
            bool spawned;
            uint8_t team;
            int32_t skin;
            uint32_t color;
            uint8_t fightingStyle;
            uint8_t interior;
            int32_t money;
            uint8_t wantedLevel;
            int32_t drunkLevel;
            AttachedObject attachedObjects[19];

        public:
            AimInfo *GetAimInfo() { 
                return &this->aimInfo; 
            }
    };
};
