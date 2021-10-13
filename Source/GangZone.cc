#include "Zion/GangZone.hh"
#include "Zion/Server.hh"
#include "Zion/Util.hh"
#include "RakNet/SAMP/SAMP.h"
using namespace Zion;

GangZone::GangZone(Vector2 min, Vector2 max) {
    if(freeIds.GetLength() > 0)
        this->index = freeIds.Pop();
    else
        this->index = nextId++;

    if(this->index < Server::gangzones.GetLength())
        Server::gangzones[this->index] = this;
    else {
        while(Server::gangzones.Push(nullptr) < this->index);
        Server::gangzones[this->index] = this;
    }

    this->min = min;
    this->max = max;
}

GangZone::~GangZone() {
    this->HideForAll();

    if(this->index == Server::gangzones.GetLength() - 1) {
        while(Server::gangzones.GetLength() > 0 && Server::gangzones.Back() == nullptr)
            Server::gangzones.Pop();
    }
    else
        Server::gangzones[this->index] = nullptr;

    freeIds.Push(this->index);
}

void GangZone::FlashForAll(uint32_t color) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<uint32_t>(Util::RGBAToARGB(color));

    Server::RPC(SAMP::RPC::GANG_ZONE_FLASH, &bitStream, UNASSIGNED_PLAYER_ID, true);
}

void GangZone::FlashForPlayer(Player *player, uint32_t color) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<uint32_t>(Util::RGBAToARGB(color));

    Server::RPC(SAMP::RPC::GANG_ZONE_FLASH, &bitStream, player->rakId, false);
}

void GangZone::HideForAll() {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);

    Server::RPC(SAMP::RPC::GANG_ZONE_DESTROY, &bitStream, UNASSIGNED_PLAYER_ID, true);
}

void GangZone::HideForPlayer(Player *player) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);

    Server::RPC(SAMP::RPC::GANG_ZONE_DESTROY, &bitStream, player->rakId, false);
}

void GangZone::ShowForAll(uint32_t color) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<Vector2>(this->min);
    bitStream.Write<Vector2>(this->max);
    bitStream.Write<uint32_t>(Util::RGBAToARGB(color));

    Server::RPC(SAMP::RPC::GANG_ZONE_CREATE, &bitStream, UNASSIGNED_PLAYER_ID, true);
}

void GangZone::ShowForPlayer(Player *player, uint32_t color) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<Vector2>(this->min);
    bitStream.Write<Vector2>(this->max);
    bitStream.Write<uint32_t>(Util::RGBAToARGB(color));

    Server::RPC(SAMP::RPC::GANG_ZONE_CREATE, &bitStream, player->rakId, false);
}

void GangZone::StopFlashForAll() {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);

    Server::RPC(SAMP::RPC::GANG_ZONE_STOP_FLASH, &bitStream, UNASSIGNED_PLAYER_ID, true);
}

void GangZone::StopFlashForPlayer(Player *player) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);

    Server::RPC(SAMP::RPC::GANG_ZONE_STOP_FLASH, &bitStream, player->rakId, false);
}

uint16_t GangZone::nextId = 0;
List<uint16_t> GangZone::freeIds;