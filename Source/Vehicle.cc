#include "Zion/Vehicle.hh"
#include "Zion/Server.hh"
#include "Zion/Util.hh"
#include "RakNet/SAMP/SAMP.h"
#include <string.h>
using namespace Zion;

PACK(struct NewVehicle {
    NewVehicle();

    uint16_t    index;
    int32_t     model;
    Vector3     position;
    float       zAngle;
    uint8_t     aColor1;
    uint8_t     aColor2;
    float       health;
    uint8_t     interior;
    uint32_t    doorDamageStatus;
    uint32_t    panelDamageStatus;
    uint8_t     lightDamageStatus;
    uint8_t     tireDamageStatus;
    uint8_t     addSiren;
    uint8_t     modSlots[14];
    uint8_t     paintjob;
    uint32_t    cColor1;
    uint32_t    cColor2;
    uint8_t     unknown;
});

NewVehicle::NewVehicle() {
    this->index = 0;
    this->model = 0;
    this->position = {0.0f, 0.0f, 0.0f};
    this->zAngle = 0.0F;
    this->aColor1 = 0;
    this->aColor2 = 0;
    this->health = 0.0F;
    this->interior = 0;
    this->doorDamageStatus = 0;
    this->panelDamageStatus = 0;
    this->lightDamageStatus = 0;
    this->tireDamageStatus = 0;
    this->addSiren = 0;
    for(size_t index = 0; index < 14; index++)
        this->modSlots[index] = 0;
    this->paintjob = 0;
    this->cColor1 = 0;
    this->cColor2 = 0;
    this->unknown = 0;
}

Vehicle::Vehicle(uint16_t model, float health, Vector3 position, float zAngle, uint8_t color1, uint8_t color2) {
    this->model = model;
    this->position = position;
    this->quaternion = Util::ToQuaternion({0.0f, 0.0f, zAngle});
    this->color1 = color1;
    this->color2 = color2;
    this->health = health;
    this->engine = 0;
    this->lights = 0;
    this->alarm = 0;
    this->bonnet = 0;
    this->boot = 0;
    this->objective = 0;
    this->doorsLocked = 0;
    this->sirenOn = 0;
    this->panelDamageStatus = 0;
    this->doorDamageStatus = 0;
    this->lightDamageStatus = 0;
    this->tireDamageStatus = 0;
    this->paintjob = 0;

    if(freeIds.GetLength() > 0)
        this->index = freeIds.Pop();
    else
        this->index = nextId++;

    if(this->index < Server::vehicles.GetLength())
        Server::vehicles[this->index] = this;
    else {
        while(Server::vehicles.Push(nullptr) < this->index);
        Server::vehicles[this->index] = this;
    }

    this->AddForWorld();
}

Vehicle::~Vehicle() {
    this->RemoveForWorld();

    if(this->index == Server::vehicles.GetLength() - 1) {
        while(Server::vehicles.GetLength() > 0 && Server::vehicles.Back() == nullptr)
            Server::vehicles.Pop();
    }
    else
        Server::vehicles[this->index] = nullptr;

    freeIds.Push(this->index);
}

void Vehicle::AddForWorld() {
    NewVehicle newVehicle;

    newVehicle.index = this->index;
    newVehicle.model = this->model;
    newVehicle.position = this->position;
    newVehicle.zAngle = Util::RadiansToDegrees(Util::GetAngles(this->quaternion).z);
    newVehicle.cColor1 = this->color1;
    newVehicle.cColor2 = this->color2;
    newVehicle.health = this->health;
    newVehicle.panelDamageStatus = this->panelDamageStatus;
    newVehicle.doorDamageStatus = this->doorDamageStatus;
    newVehicle.lightDamageStatus = this->lightDamageStatus;
    newVehicle.tireDamageStatus = this->tireDamageStatus;
    newVehicle.paintjob = this->paintjob;

    RakNet::BitStream bitStream;
    bitStream.Write<NewVehicle>(newVehicle);
    Server::RPC(SAMP::RPC::WORLD_VEHICLE_ADD, &bitStream, UNASSIGNED_PLAYER_ID, true);
}

void Vehicle::AddForPlayer(Player *player) {
    NewVehicle newVehicle;

    newVehicle.index = this->index;
    newVehicle.model = this->model;
    newVehicle.position = this->position;
    newVehicle.zAngle = Util::RadiansToDegrees(Util::GetAngles(this->quaternion).z);
    newVehicle.cColor1 = this->color1;
    newVehicle.cColor2 = this->color2;
    newVehicle.health = this->health;
    newVehicle.panelDamageStatus = this->panelDamageStatus;
    newVehicle.doorDamageStatus = this->doorDamageStatus;
    newVehicle.lightDamageStatus = this->lightDamageStatus;
    newVehicle.tireDamageStatus = this->tireDamageStatus;
    newVehicle.paintjob = this->paintjob;

    RakNet::BitStream bitStream;
    bitStream.Write<NewVehicle>(newVehicle);
    Server::RPC(SAMP::RPC::WORLD_VEHICLE_ADD, &bitStream, player->rakId, false);
}

void Vehicle::RemoveForWorld() {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    Server::RPC(SAMP::RPC::WORLD_VEHICLE_REMOVE, &bitStream, UNASSIGNED_PLAYER_ID, true);
}

void Vehicle::SendDamangeForPlayer(Player *player) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<uint32_t>(this->panelDamageStatus);
    bitStream.Write<uint32_t>(this->doorDamageStatus);
    bitStream.Write<uint8_t>(this->lightDamageStatus);
    bitStream.Write<uint8_t>(this->tireDamageStatus);

    Server::RPC(SAMP::RPC::DAMAGE_VEHICLE, &bitStream, player->rakId, false);
}

void Vehicle::SetParams(uint8_t objective, uint8_t doorsLocked) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<uint8_t>(objective);
    bitStream.Write<uint8_t>(doorsLocked);

    Server::RPC(SAMP::RPC::DAMAGE_VEHICLE, &bitStream, UNASSIGNED_PLAYER_ID, true);

    this->objective = objective;
    this->doorsLocked = doorsLocked;
}

void Vehicle::SetParamsForPlayer(Player *player, uint8_t objective, uint8_t doorsLocked) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<uint8_t>(objective);
    bitStream.Write<uint8_t>(doorsLocked);

    Server::RPC(SAMP::RPC::DAMAGE_VEHICLE, &bitStream, player->rakId, false);
}

void Vehicle::LinkVehicleToInterior(uint8_t interior) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<uint8_t>(interior);

    Server::RPC(SAMP::RPC::LINK_VEHICLE_TO_INTERIOR, &bitStream, UNASSIGNED_PLAYER_ID, true);
}

void Vehicle::SetPosition(Vector3 position) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<Vector3>(position);

    Server::RPC(SAMP::RPC::SET_VEHICLE_POS, &bitStream, UNASSIGNED_PLAYER_ID, true);

    this->position = position;
}

void Vehicle::SetVelocity(Vector3 velocity) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<uint8_t>(0); // Turn
    bitStream.Write<Vector3>(velocity);

    Server::RPC(SAMP::RPC::SET_VEHICLE_VELOCITY, &bitStream, UNASSIGNED_PLAYER_ID, true);

    this->position = position;
}

void Vehicle::SetNumberplate(const char *numberplate) {
    int8_t length = strlen(numberplate);

    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<uint8_t>(length);
    bitStream.Write(numberplate, length);

    Server::RPC(SAMP::RPC::SET_NUMBER_PLATE, &bitStream, UNASSIGNED_PLAYER_ID, true);
}

void Vehicle::SetHealth(float health) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<float>(health);

    Server::RPC(SAMP::RPC::SET_VEHICLE_HEALTH, &bitStream, UNASSIGNED_PLAYER_ID, true);

    this->health = health;
}

void Vehicle::SetZAngle(float angle) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<float>(angle);

    Server::RPC(SAMP::RPC::SET_VEHICLE_Z_ANGLE, &bitStream, UNASSIGNED_PLAYER_ID, true);

    this->quaternion = Util::ToQuaternion(
        {this->quaternion.x, this->quaternion.y, angle});
}

uint16_t Vehicle::nextId = 1;
List<uint16_t> Vehicle::freeIds;