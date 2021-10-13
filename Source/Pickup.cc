#include "Zion/Pickup.hh"
#include "Zion/Server.hh"
#include "RakNet/SAMP/SAMP.h"
using namespace Zion;

Pickup::Pickup(int32_t model, int32_t type, Vector3 position, Player *player) {
    if(freeIds.GetLength() > 0)
        this->index = freeIds.Pop();
    else
        this->index = nextId++;

    if(this->index < Server::pickups.GetLength())
        Server::pickups[this->index] = this;
    else {
        while(Server::pickups.Push(nullptr) < this->index);
        Server::pickups[this->index] = this;
    }

    this->player = player;
    this->showing = false;
    this->pt.model = model;
    this->pt.type = type;
    this->pt.position = position;
}

Pickup::~Pickup() {
    this->Hide();

    if(this->index == Server::pickups.GetLength() - 1) {
        while(Server::pickups.GetLength() > 0 && Server::pickups.Back() == nullptr)
            Server::pickups.Pop();
    }
    else
        Server::pickups[this->index] = nullptr;

    freeIds.Push(this->index);
}

void Pickup::Show() {
    if(this->showing == true)
        return;

    if(this->player != nullptr) {
        RakNet::BitStream bitStream;
        bitStream.Write<int32_t>(this->index);
        bitStream.Write<PickupTransmit>(this->pt);
        Server::RPC(SAMP::RPC::CREATE_PICKUP, &bitStream, this->player->rakId, false);
    }
    else {
        RakNet::BitStream bitStream;
        bitStream.Write<int32_t>(this->index);
        bitStream.Write<PickupTransmit>(this->pt);
        Server::RPC(SAMP::RPC::CREATE_PICKUP, &bitStream, UNASSIGNED_PLAYER_ID, true);
    }

    this->showing = true;
}

void Pickup::Hide() {
    if(this->showing == false)
        return;

    if(this->player != nullptr) {
        RakNet::BitStream bitStream;
        bitStream.Write<int32_t>(this->index);
        Server::RPC(SAMP::RPC::DESTROY_PICKUP, &bitStream, this->player->rakId, false);
    }
    else {
        RakNet::BitStream bitStream;
        bitStream.Write<int32_t>(this->index);
        Server::RPC(SAMP::RPC::DESTROY_PICKUP, &bitStream, UNASSIGNED_PLAYER_ID, true);
    }

    this->showing = false;
}

void Pickup::ShowForPlayer(Player *player) {
    RakNet::BitStream bitStream;
    bitStream.Write<int32_t>(this->index);
    bitStream.Write<PickupTransmit>(this->pt);
    Server::RPC(SAMP::RPC::CREATE_PICKUP, &bitStream, player->rakId, false);
}

List<uint16_t> Pickup::freeIds;
uint16_t Pickup::nextId = 0;