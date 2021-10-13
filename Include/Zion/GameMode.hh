#pragma once
#include "Base.hh"
#include "Player.hh"
#include "Vehicle.hh"
#include "Util.hh"
#include "TextDraw.hh"

namespace Zion {
    class GameMode {
        public:
            virtual void OnPlayerInteriorChange(Player *player, int newInterior, int oldInterior) {}
            virtual void OnDialogResponse(Player *player, int dialogId, int buttonId, int listItem, const char *inputText) {}
            virtual void OnPlayerClickMap(Player *player, Vector3 position) {}
            virtual void OnPlayerCommandText(Player *player, const char *command) {}
            virtual void OnClientMessage(Player *player, const char *message) {}
            virtual void OnPlayerDeath(Player *player, Player *killer, int reason) {}
            virtual void OnPlayerSpawn(Player *player) {}
            virtual bool OnPlayerRequestSpawn(Player *player) { return true; }
            virtual bool OnPlayerRequestClass(Player *player, int classId) { return true; }
            virtual void OnPlayerConnect(Player *player) {}
            virtual void OnPlayerDisconnect(Player *player, int reason) {}
            virtual void OnPlayerUpdate(Player *player) {}
            virtual void OnPlayerEnterCheckpoint(Player *player) {}
            virtual void OnPlayerLeaveCheckpoint(Player *player) {}
            virtual void OnPlayerEnterRaceCheckpoint(Player *player) {}
            virtual void OnPlayerLeaveRaceCheckpoint(Player *player) {}
            virtual void OnPlayerStateChange(Player *player, int newState, int oldState) {}
            virtual void OnPlayerEnterVehicle(Player *player, Vehicle *vehicle, bool ispassenger) {}
            virtual void OnPlayerExitVehicle(Player *player, Vehicle *vehicle) {}
            virtual void OnVehicleDamageStatusUpdate(Vehicle *vehicle, Player *player) {}
            virtual void OnGameModeInit() {}
            virtual void OnGameModeExit() {}
            virtual void OnPlayerClickTextDraw(Player *player, TextDraw *textDraw) {}
    };
};