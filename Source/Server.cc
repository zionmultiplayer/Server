#include "Zion/Server.hh"
#include "Zion/RPC.hh"
#include "RakNet/PacketEnumerations.h"
#include "RakNet/SAMP/SAMP.h"
#include <string.h>
using namespace Zion;

void Server::HandleQuery(SOCKET sock, sockaddr_in addr, uint8_t *buffer, size_t size) {
    if(buffer[10] == 0x70)
        sendto(sock, (const char *)buffer, 15, 0, (sockaddr *)&addr, sizeof(addr));
    else if(buffer[10] == 0x69) {
        RakNet::BitStream bitStream(buffer, 10, true);
        bitStream.Write<uint16_t>(0x69);
        bitStream.Write<uint16_t>(playersOnline);
        bitStream.Write<uint16_t>(maxPlayers);
        bitStream.Write<int32_t>(nameLength);
        bitStream.Write(name, nameLength);
        bitStream.Write<int32_t>(4); // game mode name length
        bitStream.Write("Zion", 4); // game mode name
        bitStream.Write<int32_t>(0); // map name length
        
        sendto(sock, (const char *)bitStream.GetData(),
            bitStream.GetNumberOfBytesUsed(), 0, (sockaddr *)&addr, sizeof(addr));
    }
}

Player *Server::AddPlayer(PlayerID rakId, uint16_t index, const char *name) {
    Player *player = new Player(rakId, index, name);

    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(index);
    bitStream.Write<int32_t>(1); // Padding
    bitStream.Write<uint8_t>(0); // Is NPC
    bitStream.Write<uint8_t>(player->nameLength);
    bitStream.Write(player->name, player->nameLength);
    RPC(SAMP::RPC::SERVER_JOIN, &bitStream, rakId, true);

    if(index < players.GetLength())
        players[index] = player;
    else {
        while(players.Push(nullptr) < index);
        players[index] = player;
    }

    playersOnline++;
    return player;
}

void Server::RemovePlayer(Player *player, int reason) {
    if(player->index == players.GetLength() - 1) {
        while(players.GetLength() > 0 && players.Back() == nullptr)
            players.Pop();
    }
    else
        players[player->index] = nullptr;

    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(player->index);
    bitStream.Write<uint8_t>(reason);
    RPC(SAMP::RPC::SERVER_QUIT, &bitStream, player->rakId, true);

    playersOnline--;
    delete player;
}

void Server::InitGameForPlayer(Player *player) {
    RakNet::BitStream bitStream;
    bitStream.WriteCompressed<bool>(zoneNames);
    bitStream.WriteCompressed<bool>(useCJAnims);
    bitStream.WriteCompressed<bool>(allowInteriorWeapons);
    bitStream.WriteCompressed<bool>(limitGlobalChatRadius);
    bitStream.Write<float>(globalChatRadius);
    bitStream.WriteCompressed<bool>(stuntBonus);
    bitStream.Write<float>(nameTagDrawDistance);
    bitStream.WriteCompressed<bool>(disableEnterExits);
    bitStream.WriteCompressed<bool>(nameTagLOS);
    bitStream.WriteCompressed<bool>(manualVehicleEngineAndLight);
    bitStream.Write<int32_t>(classes.GetLength());
    bitStream.Write<uint16_t>(player->index);
    bitStream.WriteCompressed<bool>(showPlayerTags);
    bitStream.Write<int32_t>(showPlayerMarkers);
    bitStream.Write<uint8_t>(worldTime);
    bitStream.Write<uint8_t>(weather);
    bitStream.Write<float>(gravity);
    bitStream.WriteCompressed<bool>(false); // Lan mode
    bitStream.Write<int32_t>(deathDropMoney);
    bitStream.WriteCompressed<bool>(instagib);
    bitStream.Write<int32_t>(35); // On foot send rate
    bitStream.Write<int32_t>(35); // In car send rate
    bitStream.Write<int32_t>(35); // Firing send rate
    bitStream.Write<int32_t>(5); // Send multiplier
    bitStream.Write<int32_t>(lagCompensation);
    bitStream.Write<uint8_t>(nameLength);
    bitStream.Write(name, nameLength);

    uint8_t vehicles[212];
    memset(vehicles, 1, sizeof(uint8_t) * 212);

    bitStream.Write((char *)vehicles, sizeof(uint8_t) * 212);
    bitStream.Write<int32_t>(vehicleFriendlyFire);

    RPC(SAMP::RPC::INIT_GAME, &bitStream, player->rakId, false);
}

void Server::SendPlayerListToPlayer(Player *player) {
    RakNet::BitStream bitStream;
    Player *p = nullptr;

    for(uint16_t index = 0; index < players.GetLength(); index++) {
        if(index == player->index || players[index] == nullptr)
            continue;

        p = players[index];

        bitStream.Reset();
        bitStream.Write<uint16_t>(index);
        bitStream.Write<int32_t>(1); // Padding
        bitStream.Write<uint8_t>(0); // Is NPC
        bitStream.Write<uint8_t>(p->nameLength);
        bitStream.Write(p->name, p->nameLength);
        RPC(SAMP::RPC::SERVER_JOIN, &bitStream, player->rakId, false);

        if(p->spawned)
            p->AddForPlayer(player);
    }
}

void Server::SendVehicleListToPlayer(Player *player) {
    for(uint16_t index = 1; index < vehicles.GetLength(); index++) {
        if(vehicles[index] == nullptr)
            continue;

        vehicles[index]->AddForPlayer(player);
        vehicles[index]->SendDamangeForPlayer(player);
    }
}

void Server::SendPickupListToPlayer(Player *player) {
    for(int32_t index = 0; index < pickups.GetLength(); index++) {
        Pickup *pickup = pickups[index];
        if(pickup == nullptr || pickup->player != nullptr || pickup->showing == false)
            continue;

        pickups[index]->ShowForPlayer(player);
    }
}

void Server::Packet_AimSync(Player *player, uint8_t *data, size_t size) {
    if(size != sizeof(Player::AimInfo)) {
        player->Kick();
        return;
    }

    player->aimInfo = *(Player::AimInfo *)data;
    player->SendAimInfo();
}

void Server::Packet_PlayerSync(Player *player, uint8_t *data, size_t size) {
    if(size != sizeof(Player::OnFootInfo)) {
        player->Kick();
        return;
    }

    if(player->state != Player::State::OnFoot)
        player->SetState(Player::State::OnFoot);

    player->onFootInfo = *(Player::OnFootInfo *)data;
    player->SendOnFootInfo();
    player->Update();
}

void Server::Packet_BulletSync(Player *player, uint8_t *data, size_t size) {
    if(size != sizeof(Player::BulletInfo)) {
        player->Kick();
        return;
    }

    player->bulletInfo = *(Player::BulletInfo *)data;
    player->SendBulletInfo();
}

void Server::Packet_VehicleSync(Player *player, uint8_t *data, size_t size) {
    if(size != sizeof(Player::InCarInfo)) {
        player->Kick();
        return;
    }

    Player::InCarInfo inCarInfo = *(Player::InCarInfo *)data;

    if(inCarInfo.vehicleId < vehicles.GetLength()) {
        Vehicle *vehicle = vehicles[inCarInfo.vehicleId];
        if(vehicle != nullptr) {
            vehicle->position = inCarInfo.position;
            vehicle->quaternion = inCarInfo.quaternion;
            vehicle->sirenOn = inCarInfo.sirenOn;
            vehicle->health = inCarInfo.carHealth;
            vehicle->velocity = inCarInfo.moveSpeed;
        }
    }

    if(player->state != Player::State::Driver)
        player->SetState(Player::State::Driver);

    player->inCarInfo = inCarInfo;
    player->SendInCarInfo();
    player->Update();
}

void Server::Packet_PassengerSync(Player *player, uint8_t *data, size_t size) {
    if(size != sizeof(Player::PassengerInfo)) {
        player->Kick();
        return;
    }

    if(player->state != Player::State::Passenger)
        player->SetState(Player::State::Passenger);

    player->passengerInfo = *(Player::PassengerInfo *)data;
    player->SendPassengerInfo();
    player->Update();
}

void Server::Packet_WeaponsUpdate(Player *player, uint8_t *data, size_t size) {
    if(size % 4 != 0)
        player->Kick();

    RakNet::BitStream bitStream(data, size, false);

    for(int _ = size / 4; _ > 0; _--) {
        uint8_t slot = 0xFF;
        bitStream.Read<uint8_t>(slot);

        if(slot > 13) {
            bitStream.IgnoreBits(24);
            continue;
        }

        bitStream.Read<uint8_t>(player->weaponInfo[slot].weapon);
        bitStream.Read<uint16_t>(player->weaponInfo[slot].ammo);
    }
}

void Server::Packet_StatsUpdate(Player *player, uint8_t *data, size_t size) {
    if(size != 8)
        player->Kick();

    RakNet::BitStream bitStream(data, size, false);
    bitStream.Read<int32_t>(player->money);
    bitStream.Read<int32_t>(player->drunkLevel);
}

void Server::Start(uint16_t maxPlayers, uint16_t port, const char *hostname) {
    Stop();

    rakServer = new RakServer();
    SAMP::port = port;
    SAMP::HandleQuery = HandleQuery;
    Server::maxPlayers = maxPlayers;

    rakServer->RegisterAsRemoteProcedureCall(SAMP::RPC::CLIENT_JOIN, RPC::PlayerJoin);
    rakServer->RegisterAsRemoteProcedureCall(SAMP::RPC::REQUEST_CLASS, RPC::RequestClass);
    rakServer->RegisterAsRemoteProcedureCall(SAMP::RPC::REQUEST_SPAWN, RPC::RequestSpawn);
    rakServer->RegisterAsRemoteProcedureCall(SAMP::RPC::SPAWN, RPC::Spawn);
    rakServer->RegisterAsRemoteProcedureCall(SAMP::RPC::DEATH, RPC::Death);
    rakServer->RegisterAsRemoteProcedureCall(SAMP::RPC::CHAT, RPC::ClientMessage);
    rakServer->RegisterAsRemoteProcedureCall(SAMP::RPC::SERVER_COMMAND, RPC::CommandText);
    rakServer->RegisterAsRemoteProcedureCall(SAMP::RPC::MAP_MARKER, RPC::ClickMap);
    rakServer->RegisterAsRemoteProcedureCall(SAMP::RPC::DIALOG_RESPONSE, RPC::DialogResponse);
    rakServer->RegisterAsRemoteProcedureCall(SAMP::RPC::ENTER_VEHICLE, RPC::EnterVehicle);
    rakServer->RegisterAsRemoteProcedureCall(SAMP::RPC::EXIT_VEHICLE, RPC::ExitVehicle);
    rakServer->RegisterAsRemoteProcedureCall(SAMP::RPC::DAMAGE_VEHICLE, RPC::DamageVehicle);
    rakServer->RegisterAsRemoteProcedureCall(SAMP::RPC::UPDATE_SCORES_PINGS_IPS, RPC::UpdateScoreAndPings);
    rakServer->RegisterAsRemoteProcedureCall(SAMP::RPC::SET_INTERIOR_ID, RPC::SetInterior);
    rakServer->Start(maxPlayers, 0, 5, port, hostname);

    gameMode->OnGameModeInit();
}

void Server::Stop() {
    gameMode->OnGameModeExit();

    if(rakServer != nullptr) {
        SAMP::HandleQuery = nullptr;
        rakServer->Disconnect(15);
        delete rakServer;
    }
}

void Server::Update() {
    Packet *packet = rakServer->Receive();
    
    while(packet != nullptr) {
        Player *player = nullptr;

        if(packet->playerIndex < players.GetLength())
            player = players[packet->playerIndex];

        switch(packet->data[0]) {
            case SAMP::Packet::AIM_SYNC:
                Packet_AimSync(player, packet->data + 1, BITS_TO_BYTES(packet->bitSize) - 1);
                break;

            case SAMP::Packet::PLAYER_SYNC:
                Packet_PlayerSync(player, packet->data + 1, BITS_TO_BYTES(packet->bitSize) - 1);
                break;

            case SAMP::Packet::BULLET_SYNC:
                Packet_BulletSync(player, packet->data + 1, BITS_TO_BYTES(packet->bitSize) - 1);
                break;

            case SAMP::Packet::VEHICLE_SYNC:
                Packet_VehicleSync(player, packet->data + 1, BITS_TO_BYTES(packet->bitSize) - 1);
                break;

            case SAMP::Packet::PASSENGER_SYNC:
                Packet_PassengerSync(player, packet->data + 1, BITS_TO_BYTES(packet->bitSize) - 1);
                break;

            case SAMP::Packet::WEAPONS_UPDATE:
                Packet_WeaponsUpdate(player, packet->data + 1, BITS_TO_BYTES(packet->bitSize) - 1);
                break;

            case SAMP::Packet::STATS_UPDATE:
                Packet_StatsUpdate(player, packet->data + 1, BITS_TO_BYTES(packet->bitSize) - 1);
                break;

            case ID_DISCONNECTION_NOTIFICATION:
                gameMode->OnPlayerDisconnect(player, 1);
                RemovePlayer(player, 1);
                player = nullptr;
                break;

            case ID_CONNECTION_LOST:
                gameMode->OnPlayerDisconnect(player, 0);
                RemovePlayer(player, 0);
                player = nullptr;
                break;
        }

        if(player)
            gameMode->OnPlayerUpdate(player);

        rakServer->DeallocatePacket(packet);
        packet = rakServer->Receive();
    }
}

void Server::SetServerName(const char *name) {
    size_t nameLength = strlen(name);
    if(nameLength >= 254)
        throw ServerException("nameLength >= 254");

    if(Server::name != nullptr)
        delete[] Server::name;

    Server::name = new char[nameLength + 1];
    strcpy(Server::name, name);
}

bool                        Server::zoneNames = true;
bool                        Server::useCJAnims = true;
bool                        Server::allowInteriorWeapons = true;
bool                        Server::limitGlobalChatRadius = false;
float                       Server::globalChatRadius = 200.0F;
bool                        Server::stuntBonus = false;
float                       Server::nameTagDrawDistance = 70.0F;
bool                        Server::disableEnterExits = false;
bool                        Server::nameTagLOS = true;
bool                        Server::manualVehicleEngineAndLight = false;
bool                        Server::showPlayerTags = true;
int32_t                     Server::showPlayerMarkers = true;
uint8_t                     Server::worldTime = 12;
uint8_t                     Server::weather = 10;
float                       Server::gravity = 0.008F;
int32_t                     Server::deathDropMoney = 0;
bool                        Server::instagib = false;
int32_t                     Server::lagCompensation = 1;
int32_t                     Server::vehicleFriendlyFire = 0;
List<Player::SpawnInfo>     Server::classes;
GameMode *                  Server::gameMode = nullptr;
RakServer *                 Server::rakServer =  nullptr;
List<Player *>              Server::players;
List<Vehicle *>             Server::vehicles;
List<TextDraw *>            Server::textDraws;
List<Pickup *>              Server::pickups;
List<GangZone *>            Server::gangzones;
char *                      Server::name = strdup("Zion");
uint8_t                     Server::nameLength = 4;
uint16_t                    Server::maxPlayers = 0;
uint16_t                    Server::playersOnline = 0;
