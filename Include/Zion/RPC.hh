#pragma once
#include "RakNet/NetworkTypes.h"

namespace Zion {
    class RPC {
        friend class Server;

        private:
            static void PlayerJoin(RPCParameters *params);
            static void RequestClass(RPCParameters *params);
            static void RequestSpawn(RPCParameters *params);
            static void Spawn(RPCParameters *params);
            static void Death(RPCParameters *params);
            static void ClientMessage(RPCParameters *params);
            static void CommandText(RPCParameters *params);
            static void ClickMap(RPCParameters *params);
            static void DialogResponse(RPCParameters *params);
            static void SetInterior(RPCParameters *params);
            static void UpdateScoreAndPings(RPCParameters *params);
            static void EnterVehicle(RPCParameters *params);
            static void ExitVehicle(RPCParameters *params);
            static void DamageVehicle(RPCParameters *params);
            static void ClickTextDraw(RPCParameters *params);
    };
};