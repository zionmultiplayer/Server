#include "Zion/RPC.hh"
#include "Zion/Server.hh"
#include "RakNet/SAMP/SAMP.h"
using namespace Zion;

void RPC::PlayerJoin(RPCParameters *params) {
    RakNet::BitStream data(params->input, BITS_TO_BYTES(params->numberOfBitsOfData), false);

    int32_t version;
    uint8_t mod;
    uint8_t nameLength;
    char *name;
    uint32_t token;

    data.Read<int32_t>(version);
    data.Read<uint8_t>(mod);
    data.Read<uint8_t>(nameLength);
    name = new char[nameLength + 1];
    data.Read(name, nameLength);
    name[nameLength] = '\0';
    data.Read<uint32_t>(token);

    if(version != 4057 || SAMP::token != (token ^ 4057)) {
        RakNet::BitStream bitStream;
        bitStream.Write<uint8_t>(4); // Bad version
        Server::RPC(SAMP::RPC::CONNECTION_REJECTED, &bitStream, params->sender, false);
        Server::rakServer->Kick(params->sender);
        return;
    }

    Player *player = Server::AddPlayer(params->sender, params->senderIndex, name);
    delete[] name;
    
    Server::InitGameForPlayer(player);
    Server::gameMode->OnPlayerConnect(player);
}

void RPC::RequestClass(RPCParameters *params) {
    int32_t classId = *(int32_t *)params->input;
    if(classId < 0 || classId >= Server::classes.GetLength())
        return;

    Player *player = Server::players[params->senderIndex];

    bool outcome = Server::gameMode->OnPlayerRequestClass(player, classId);
    if(outcome)
        player->spawnInfo = Server::classes[classId];

    RakNet::BitStream bitStream;
    bitStream.Write<uint8_t>(outcome);
    bitStream.Write<Player::SpawnInfo>(player->spawnInfo);
    Server::RPC(SAMP::RPC::REQUEST_CLASS, &bitStream, params->sender, false);
}

void RPC::RequestSpawn(RPCParameters *params) {
    Server::players[params->senderIndex]->Spawn(
        Server::gameMode->OnPlayerRequestSpawn(Server::players[params->senderIndex]) ? 2 : 0);
}

void RPC::Spawn(RPCParameters *params) {
    Player *player = Server::players[params->senderIndex];
    
    Server::SendPlayerListToPlayer(player);
    Server::SendVehicleListToPlayer(player);
    Server::SendPickupListToPlayer(player);
    
    player->AddForWorld();
    player->spawned = true;
    Server::gameMode->OnPlayerSpawn(player);
}

void RPC::Death(RPCParameters *params) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(params->senderIndex);
    Server::RPC(SAMP::RPC::WORLD_PLAYER_DEATH, &bitStream, params->sender, true);

    RakNet::BitStream data(params->input, BITS_TO_BYTES(params->numberOfBitsOfData), false);

    uint8_t reason;
    uint16_t killerId;

    data.Read<uint8_t>(reason);
    data.Read<uint16_t>(killerId);

    Server::gameMode->OnPlayerDeath(
        Server::GetPlayerByIndex(params->senderIndex), Server::GetPlayerByIndex(killerId), reason);
}

void RPC::ClientMessage(RPCParameters *params) {
    RakNet::BitStream data(params->input, BITS_TO_BYTES(params->numberOfBitsOfData), false);

    uint8_t messageLength;
    char *message;

    data.Read<uint8_t>(messageLength);
    message = new char[messageLength + 1];
    data.Read(message, messageLength);
    message[messageLength] = '\0';

    Server::gameMode->OnClientMessage(Server::players[params->senderIndex], message);
    delete[] message;
}

void RPC::CommandText(RPCParameters *params) {
    RakNet::BitStream data(params->input, BITS_TO_BYTES(params->numberOfBitsOfData), false);

    int32_t commandLength;
    char *command;

    data.Read<int32_t>(commandLength);
    command = new char[commandLength + 1];
    data.Read(command, commandLength);
    command[commandLength] = '\0';

    Server::gameMode->OnPlayerCommandText(Server::players[params->senderIndex], command);
    delete[] command;
}

void RPC::ClickMap(RPCParameters *params) {
    Server::gameMode->OnPlayerClickMap(
        Server::players[params->senderIndex], *(Vector3 *)params->input); 
}

void RPC::DialogResponse(RPCParameters *params) {
    RakNet::BitStream data(params->input, BITS_TO_BYTES(params->numberOfBitsOfData), false);

    uint16_t dialogId;
    uint8_t buttonId;
    uint16_t listItem;
    uint8_t inputLength;
    char *inputText = nullptr;

    data.Read<uint16_t>(dialogId);
    data.Read<uint8_t>(buttonId);
    data.Read<uint16_t>(listItem);
    data.Read<uint8_t>(inputLength);
    inputText = new char[inputLength + 1];
    inputText[inputLength] = '\0';
    data.Read(inputText, inputLength);

    Server::gameMode->OnDialogResponse(
        Server::players[params->senderIndex], dialogId, buttonId, listItem, inputText);

    delete[] inputText;
}

void RPC::SetInterior(RPCParameters *params) {
    Player *player = Server::players[params->senderIndex];
    
    if(BITS_TO_BYTES(params->numberOfBitsOfData) != 1)
        player->Kick();

    uint8_t oldInterior = player->interior;
    player->interior = *params->input;

    Server::gameMode->OnPlayerInteriorChange(player, oldInterior, player->interior);
}

void RPC::UpdateScoreAndPings(RPCParameters *params) {
    RakNet::BitStream bitStream;

    for(uint16_t index = 0; index < Server::players.GetLength(); index++) {
        Player *player = Server::players[params->senderIndex];
        if(player == nullptr)
            continue;

        bitStream.Write<uint16_t>(index);
        bitStream.Write<int32_t>(player->score);
        bitStream.Write<int32_t>(Server::rakServer->GetLastPing(player->rakId));
    }

    Server::RPC(SAMP::RPC::UPDATE_SCORES_PINGS_IPS, &bitStream, params->sender, false);
}

void RPC::EnterVehicle(RPCParameters *params) {
    RakNet::BitStream data(params->input, BITS_TO_BYTES(params->numberOfBitsOfData), false);

    uint16_t vehicleId;
    uint8_t passenger;

    data.Read<uint16_t>(vehicleId);
    data.Read<uint8_t>(passenger);

    if(vehicleId < Server::vehicles.GetLength()) {
        if(Server::vehicles[vehicleId] != nullptr) {
            RakNet::BitStream bitStream;
            bitStream.Write<uint16_t>(params->senderIndex);
            bitStream.Write<uint16_t>(vehicleId);
            bitStream.Write<uint8_t>(passenger);

            Server::RPC(SAMP::RPC::ENTER_VEHICLE, &bitStream, params->sender, true);
            Server::gameMode->OnPlayerEnterVehicle(
                Server::players[params->senderIndex], Server::vehicles[vehicleId], passenger);
        }
    }
}

void RPC::ExitVehicle(RPCParameters *params) {
    uint16_t vehicleId = *(uint16_t *)params->input;

    if(vehicleId < Server::vehicles.GetLength()) {
        if(Server::vehicles[vehicleId] != nullptr) {
            RakNet::BitStream bitStream;
            bitStream.Write<uint16_t>(params->senderIndex);
            bitStream.Write<uint16_t>(vehicleId);

            Server::RPC(SAMP::RPC::EXIT_VEHICLE, &bitStream, params->sender, true);
            Server::gameMode->OnPlayerExitVehicle(
                Server::players[params->senderIndex], Server::vehicles[vehicleId]);
        }
    }
}

void RPC::DamageVehicle(RPCParameters *params) {
    RakNet::BitStream data(params->input, BITS_TO_BYTES(params->numberOfBitsOfData), false);

    uint16_t vehicleId;
    uint32_t panelDamageStatus;
    uint32_t doorDamageStatus;
    uint8_t lightDamageStatus;
    uint8_t tireDamageStatus;

    data.Read<uint16_t>(vehicleId);

    if(vehicleId < Server::vehicles.GetLength()) {
        if(Server::vehicles[vehicleId] != nullptr) {
            data.Read<uint32_t>(panelDamageStatus);
            data.Read<uint32_t>(doorDamageStatus);
            data.Read<uint8_t>(lightDamageStatus);
            data.Read<uint8_t>(tireDamageStatus);

            Vehicle *vehicle = Server::vehicles[vehicleId];
            vehicle->panelDamageStatus = panelDamageStatus;
            vehicle->doorDamageStatus = doorDamageStatus;
            vehicle->lightDamageStatus = lightDamageStatus;
            vehicle->tireDamageStatus = tireDamageStatus;

            data.SetReadOffset(0);

            Server::RPC(SAMP::RPC::DAMAGE_VEHICLE, &data, params->sender, true);
            Server::gameMode->OnVehicleDamageStatusUpdate(vehicle, Server::players[params->senderIndex]);
        }
    }
}

void RPC::ClickTextDraw(RPCParameters *params) {
    Server::gameMode->OnPlayerCli
}