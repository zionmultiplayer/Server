#include "Zion/Player.hh"
#include "Zion/Server.hh"
#include "Zion/Util.hh"
#include "RakNet/StringCompressor.h"
#include "RakNet/SAMP/SAMP.h"
#include <math.h>
#include <stdarg.h>
using namespace Zion;

Player::Player(PlayerID rakId, uint16_t index, const char *name) {
    size_t nameLength = strlen(name);
    if(nameLength >= ZION__MAX_NAME_LENGTH)
        throw PlayerException("nameLength >= ZION__MAX_NAME_LENGTH");

    this->name = new char[nameLength + 1];
    strcpy(this->name, name);

    this->nameLength = nameLength;
    this->rakId = rakId;
    this->index = index;
    this->spawned = false;
    this->state = State::None;
}

Player::~Player() {
    delete[] this->name;
}

void Player::SendOnFootInfo() {
    RakNet::BitStream bitStream;
    bitStream.Write<uint8_t>(SAMP::Packet::PLAYER_SYNC);
    bitStream.Write<uint16_t>(this->index);

    if(this->onFootInfo.lrAnalog) {
        bitStream.Write<bool>(true);
        bitStream.Write<uint16_t>(this->onFootInfo.lrAnalog);
    }
    else
        bitStream.Write<bool>(false);

    if(this->onFootInfo.udAnalog) {
        bitStream.Write<bool>(true);
        bitStream.Write<uint16_t>(this->onFootInfo.udAnalog);
    }
    else
        bitStream.Write<bool>(false);

    bitStream.Write<uint16_t>(this->onFootInfo.keys);
    bitStream.Write<Vector3>(this->onFootInfo.position);
    bitStream.WriteNormQuat<float>(
        this->onFootInfo.quaternion.w,
        this->onFootInfo.quaternion.x,
        this->onFootInfo.quaternion.y,
        this->onFootInfo.quaternion.z);

    uint8_t healthArmour = 0;
    uint8_t health = this->onFootInfo.health;
    uint8_t armour = this->onFootInfo.armour;

    if(health > 0 && health < 100)
        healthArmour = (uint8_t)(health / 7) << 4;
    else if(health >= 100)
        healthArmour = 0xF << 4;

    if(armour > 0 && armour < 100)
        healthArmour |=  (uint8_t)(armour / 7);
    else if(armour >= 100)
        healthArmour |= 0xF;

    bitStream.Write<uint8_t>(healthArmour);
    bitStream.Write<uint8_t>(this->onFootInfo.currentWeapon);
    bitStream.Write<uint8_t>(this->onFootInfo.specialAction);
    bitStream.WriteVector<float>(
        this->onFootInfo.moveSpeed.x,
        this->onFootInfo.moveSpeed.y,
        this->onFootInfo.moveSpeed.z);

    if(this->onFootInfo.surfInfo) {
        bitStream.Write<bool>(true);
        bitStream.Write<uint16_t>(this->onFootInfo.surfInfo);
        bitStream.Write<Vector3>(this->onFootInfo.surfOffsets);
    }
    else
        bitStream.Write<bool>(false);

    if(this->onFootInfo.animation) {
        bitStream.Write<bool>(true);
        bitStream.Write<int32_t>(this->onFootInfo.animation);
    }
    else
        bitStream.Write<bool>(false);

    Server::Send(&bitStream, this->rakId, true, HIGH_PRIORITY, UNRELIABLE_SEQUENCED);
}

void Player::SendInCarInfo() {
    RakNet::BitStream bitStream;
    bitStream.Write<uint8_t>(SAMP::Packet::VEHICLE_SYNC);
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<uint16_t>(this->inCarInfo.vehicleId);
    bitStream.Write<uint16_t>(this->inCarInfo.lrAnalog);
    bitStream.Write<uint16_t>(this->inCarInfo.udAnalog);
    bitStream.Write<uint16_t>(this->inCarInfo.keys);
    bitStream.WriteNormQuat<float>(
        this->inCarInfo.quaternion.w,
        this->inCarInfo.quaternion.x,
        this->inCarInfo.quaternion.y,
        this->inCarInfo.quaternion.z);
    bitStream.Write<Vector3>(this->inCarInfo.position);
    bitStream.WriteVector<float>(
        this->inCarInfo.moveSpeed.x,
        this->inCarInfo.moveSpeed.y,
        this->inCarInfo.moveSpeed.z);
    bitStream.Write<uint16_t>(this->inCarInfo.carHealth);

    uint8_t healthArmour = 0;
    uint8_t health = this->inCarInfo.playerHealth;
    uint8_t armour = this->inCarInfo.playerArmour;

    if(health > 0 && health < 100)
        healthArmour = (uint8_t)(health / 7) << 4;
    else if(health >= 100)
        healthArmour = 0xF << 4;

    if(armour > 0 && armour < 100)
        healthArmour |=  (uint8_t)(armour / 7);
    else if(armour >= 100)
        healthArmour |= 0xF;

    bitStream.Write<uint8_t>(healthArmour);
    bitStream.Write<uint8_t>(this->inCarInfo.currentWeapon);
    bitStream.Write<bool>(this->inCarInfo.sirenOn);
    bitStream.Write<bool>(this->inCarInfo.landingGearState);

    if(this->inCarInfo.trainSpeed) {
        bitStream.Write<bool>(true);
        bitStream.Write<float>(this->inCarInfo.trainSpeed);
    }
    else
        bitStream.Write<bool>(false);

    if(this->inCarInfo.trailerIdOrThrustAngle) {
        bitStream.Write<bool>(true);
        bitStream.Write<uint16_t>(this->inCarInfo.trailerIdOrThrustAngle);
    }
    else
        bitStream.Write<bool>(false);

    Server::Send(&bitStream, this->rakId, true, HIGH_PRIORITY, UNRELIABLE_SEQUENCED);
}

void Player::SendAimInfo() {
    RakNet::BitStream bitStream;
    bitStream.Write<uint8_t>(SAMP::Packet::AIM_SYNC);
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<AimInfo>(this->aimInfo);

    Server::Send(&bitStream, this->rakId, true, HIGH_PRIORITY, UNRELIABLE_SEQUENCED);
}

void Player::SendBulletInfo() {
    RakNet::BitStream bitStream;
    bitStream.Write<uint8_t>(SAMP::Packet::BULLET_SYNC);
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<BulletInfo>(this->bulletInfo);

    Server::Send(&bitStream, this->rakId, true, HIGH_PRIORITY, UNRELIABLE_SEQUENCED);
}

void Player::SendPassengerInfo() {
    RakNet::BitStream bitStream;
    bitStream.Write<uint8_t>(SAMP::Packet::PASSENGER_SYNC);
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<PassengerInfo>(this->passengerInfo);

    Server::Send(&bitStream, this->rakId, true, HIGH_PRIORITY, UNRELIABLE_SEQUENCED);
}


void Player::Update() {
    switch(this->state) {
        case State::OnFoot:
            this->position = this->onFootInfo.position;
            this->quaternion = this->onFootInfo.quaternion;
            break;

        case State::Driver:
            this->position = this->inCarInfo.position;
            this->quaternion = this->inCarInfo.quaternion;
            break;

        case State::Passenger:
            this->position = this->passengerInfo.position;
            this->quaternion = {0.0f, 0.0f, 0.0f, 0.0f};
            break;
    }

    if(this->checkpointInfo.active) {
        if(Util::GetDistance(this->position, this->checkpointInfo.position) < this->checkpointInfo.radius) {
            if(this->checkpointInfo.in == false) {
                this->checkpointInfo.in = true;
                Server::gameMode->OnPlayerEnterCheckpoint(this);
            }
        }
        else {
            if(this->checkpointInfo.in == true) {
                this->checkpointInfo.in = false;
                Server::gameMode->OnPlayerLeaveCheckpoint(this);
            }
        }
    }

    if(this->raceCheckpointInfo.active) {
        if(Util::GetDistance(this->position, this->raceCheckpointInfo.position) < this->raceCheckpointInfo.radius) {
            if(this->raceCheckpointInfo.in == false) {
                this->raceCheckpointInfo.in = true;
                Server::gameMode->OnPlayerEnterRaceCheckpoint(this);
            }
        }
        else {
            if(this->raceCheckpointInfo.in == true) {
                this->raceCheckpointInfo.in = false;
                Server::gameMode->OnPlayerLeaveRaceCheckpoint(this);
            }
        }
    }
}

void Player::SetState(uint8_t state) {
    uint8_t oldState = this->state;
    this->state = state;
    Server::gameMode->OnPlayerStateChange(this, this->state, oldState);
}

void Player::AddForWorld() {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<uint8_t>(this->team);
    bitStream.Write<int32_t>(this->skin);
    bitStream.Write<Vector3>(this->position);
    bitStream.Write<float>(Util::RadiansToDegrees(Util::GetAngles(this->quaternion).z));
    bitStream.Write<uint32_t>(this->color);
    bitStream.Write<uint8_t>(this->fightingStyle);
    Server::RPC(SAMP::RPC::WORLD_PLAYER_ADD, &bitStream, UNASSIGNED_PLAYER_ID, true);
}

void Player::AddForPlayer(Player *player) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<uint8_t>(this->team);
    bitStream.Write<int32_t>(this->skin);
    bitStream.Write<Vector3>(this->position);
    bitStream.Write<float>(Util::RadiansToDegrees(Util::GetAngles(this->quaternion).z));
    bitStream.Write<uint32_t>(this->color);
    bitStream.Write<uint8_t>(this->fightingStyle);
    Server::RPC(SAMP::RPC::WORLD_PLAYER_ADD, &bitStream, player->rakId, false);
}

void Player::Spawn(uint8_t spawn) {
    this->position = this->spawnInfo.position;
    this->skin = this->spawnInfo.skin;
    this->team = this->spawnInfo.team;

    RakNet::BitStream bitStream;
    bitStream.Write<uint8_t>(spawn);
    Server::RPC(SAMP::RPC::REQUEST_SPAWN, &bitStream, this->rakId, false);
}

// ________________________________ Public ________________________________

void Player::Spawn() {
    this->Spawn(2);
}

void Player::SetPosition(Vector3 position) {
    RakNet::BitStream bitStream;
    bitStream.Write<Vector3>(position);
    Server::RPC(SAMP::RPC::SET_PLAYER_POS, &bitStream, this->rakId, false);
}

void Player::SetPositionFindZ(Vector3 position) {
    RakNet::BitStream bitStream;
    bitStream.Write<Vector3>(position);
    Server::RPC(SAMP::RPC::SET_PLAYER_POS_FIND_Z, &bitStream, this->rakId, false);
}

Vector3 Player::GetPosition() {
    return this->position;
}

void Player::SetFacingAngle(float angle) {
    RakNet::BitStream bitStream;
    bitStream.Write<float>(angle);
    Server::RPC(SAMP::RPC::SET_PLAYER_FACING_ANGLE, &bitStream, this->rakId, false);
}

float Player::GetFacingAngle() {
    return Util::RadiansToDegrees(Util::GetAngles(this->quaternion).z);
}

float Player::GetDistanceFromPoint(Vector3 point) {
    return Util::GetDistance(this->position, point);
}

void Player::SetInterior(uint8_t interior) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint8_t>(interior);
    Server::RPC(SAMP::RPC::SET_PLAYER_INTERIOR, &bitStream, this->rakId, false);
    this->interior = interior;
}

uint8_t Player::GetInterior() {
    return this->interior;
}

void Player::SetHealth(float health) {
    RakNet::BitStream bitStream;
    bitStream.Write<float>(health);
    Server::RPC(SAMP::RPC::SET_PLAYER_HEALTH, &bitStream, this->rakId, false);
}

float Player::GetHealth() {
    switch(this->state) {
        case State::OnFoot:
            return this->onFootInfo.health;
            break;

        case State::Driver:
            return this->inCarInfo.playerHealth;
            break;

        case State::Passenger:
            return this->passengerInfo.playerHealth;
            break;
    }

    return 0.0F;
}

void Player::SetArmour(float armour) {
    RakNet::BitStream bitStream;
    bitStream.Write<float>(armour);
    Server::RPC(SAMP::RPC::SET_PLAYER_ARMOUR, &bitStream, this->rakId, false);
}

float Player::GetArmour() {
        switch(this->state) {
        case State::OnFoot:
            return this->onFootInfo.armour;
            break;

        case State::Driver:
            return this->inCarInfo.playerArmour;
            break;

        case State::Passenger:
            return this->passengerInfo.playerArmour;
            break;
    }

    return 0.0F;
}

void Player::SetAmmo(uint8_t weaponSlot, uint16_t ammo) {
    RakNet::BitStream bitStream;
    bitStream.Write<float>(ammo);
    Server::RPC(SAMP::RPC::SET_PLAYER_AMMO, &bitStream, this->rakId, false);
}

void Player::SetTeam(uint8_t team) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<uint8_t>(team);
    Server::RPC(SAMP::RPC::SET_PLAYER_TEAM, &bitStream, UNASSIGNED_PLAYER_ID, true);
    this->team = team;
}

uint8_t Player::GetTeam() {
    return this->team;
}

void Player::SetDrunkLevel(int32_t level) {
    RakNet::BitStream bitStream;
    bitStream.Write<int32_t>(level);
    Server::RPC(SAMP::RPC::SET_PLAYER_DRUNK_LEVEL, &bitStream, this->rakId, false);
}

int32_t Player::GetDrunkLevel() {
    return this->drunkLevel;
}

void Player::SetColor(uint32_t color) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<uint8_t>(color);
    Server::RPC(SAMP::RPC::SET_PLAYER_COLOR, &bitStream, UNASSIGNED_PLAYER_ID, true);
    this->color = color;
}

uint32_t Player::GetColor() {
    return this->color;
}

void Player::SetSkin(uint32_t skin) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<uint32_t>(skin);
    Server::RPC(SAMP::RPC::SET_PLAYER_SKIN, &bitStream, UNASSIGNED_PLAYER_ID, true);
    this->skin = skin;
}

uint32_t Player::GetSkin() {
    return this->skin;
}

void Player::GiveWeapon(int32_t weaponId, int32_t weaponAmmo) {
    RakNet::BitStream bitStream;
    bitStream.Write<int32_t>(weaponId);
    bitStream.Write<int32_t>(weaponAmmo);
    Server::RPC(SAMP::RPC::GIVE_PLAYER_WEAPON, &bitStream, this->rakId, false);
}

void Player::ResetWeapons() {
    Server::RPC(SAMP::RPC::RESET_PLAYER_WEAPONS, nullptr, this->rakId, false);
}

void Player::SetArmedWeapon(int32_t weaponSlot) {
    RakNet::BitStream bitStream;
    bitStream.Write<int32_t>(weaponSlot);
    Server::RPC(SAMP::RPC::SET_PLAYER_ARMED_WEAPON, &bitStream, this->rakId, false);
}

void Player::GiveMoney(int32_t money) {
    RakNet::BitStream bitStream;
    bitStream.Write<int32_t>(money);
    Server::RPC(SAMP::RPC::GIVE_PLAYER_MONEY, &bitStream, this->rakId, false);
    this->money += money;
}

void Player::ResetMoney() {
    Server::RPC(SAMP::RPC::RESET_PLAYER_MONEY, nullptr, this->rakId, false);
}

void Player::SetName(const char *name) {
    size_t nameLength = strlen(name);
    if(nameLength >= ZION__MAX_NAME_LENGTH)
        throw PlayerException("nameLength >= ZION__MAX_NAME_LENGTH");

    this->name = new char[nameLength + 1];
    strcpy(this->name, name);
    
    this->nameLength = nameLength;

    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<uint8_t>(nameLength);
    bitStream.Write(name, nameLength);
    bitStream.Write<uint8_t>(1); // Success
    Server::RPC(SAMP::RPC::SET_PLAYER_NAME, &bitStream, UNASSIGNED_PLAYER_ID, true);
}

int32_t Player::GetMoney() {
    return this->money;
}

uint8_t Player::GetState() {
    return this->state;
}

int32_t Player::GetPing() {
    return Server::rakServer->GetLastPing(this->rakId);
}

bool Player::GetKeys(int *lr, int *ud, int *keys) {
    switch(this->state) {
        case State::OnFoot:
            *lr = this->onFootInfo.lrAnalog;
            *ud = this->onFootInfo.udAnalog;
            *keys = this->onFootInfo.keys;
            break;

        case State::Driver:
            *lr = this->inCarInfo.lrAnalog;
            *ud = this->inCarInfo.udAnalog;
            *keys = this->inCarInfo.keys;
            break;

        case State::Passenger:
            *lr = this->passengerInfo.lrAnalog;
            *ud = this->passengerInfo.udAnalog;
            *keys = this->passengerInfo.keys;
            break;

        default:
            return false;
            break;
    }

    return true;
}

const char *Player::GetName() {
    return this->name;
}

void Player::SetTime(uint8_t hour, uint8_t minute) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint8_t>(hour);
    bitStream.Write<uint8_t>(minute);
    Server::RPC(SAMP::RPC::SET_PLAYER_TIME, &bitStream, this->rakId, false);
}

void Player::ToggleClock(bool toggle) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint8_t>(toggle);
    Server::RPC(SAMP::RPC::TOGGLE_CLOCK, &bitStream, this->rakId, false);
}

void Player::SetWeather(uint8_t weather) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint8_t>(weather);
    Server::RPC(SAMP::RPC::SET_WEATHER, &bitStream, this->rakId, false);
}

void Player::ForceClassSelection() {
    Server::RPC(SAMP::RPC::FORCE_CLASS_SELECTION, nullptr, this->rakId, false);
}

void Player::SetWantedLevel(uint8_t level) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint8_t>(level);
    Server::RPC(SAMP::RPC::SET_PLAYER_WANTED_LEVEL, &bitStream, this->rakId, false);
    this->wantedLevel = level;
}

uint8_t Player::GetWantedLevel() {
    return this->wantedLevel;
}

void Player::SetFightingStyle(uint8_t style) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<uint8_t>(style);
    Server::RPC(SAMP::RPC::SET_PLAYER_FIGHTING_STYLE, &bitStream,
        UNASSIGNED_PLAYER_ID, true);
    this->fightingStyle = style;
}

uint8_t Player::GetFightingStyle() {
    return this->fightingStyle;
}

void Player::SetVelocity(Vector3 velocity) {
    RakNet::BitStream bitStream;
    bitStream.Write<Vector3>(velocity);
    Server::RPC(SAMP::RPC::SET_PLAYER_VELOCITY, &bitStream, this->rakId, false);
}

Vector3 Player::GetVelocity() {
    switch(this->state) {
        case State::OnFoot:
            return this->onFootInfo.moveSpeed;
            break;

        case State::Driver:
            return this->inCarInfo.moveSpeed;
            break;
    }

    return {0.0f, 0.0f, 0.0f};
}

void Player::PlayAudioStream(const char *url, Vector3 position, float radius, bool usePosition) {
    uint8_t urlLength = strlen(url);

    RakNet::BitStream bitStream;
    bitStream.Write<uint8_t>(urlLength);
    bitStream.Write(url, urlLength);
    bitStream.Write<Vector3>(position);
    bitStream.Write<float>(radius);
    bitStream.Write<bool>(usePosition);
    Server::RPC(SAMP::RPC::PLAY_AUDIO_STREAM, &bitStream, this->rakId, false);
}

void Player::StopAudioStream() {
    Server::RPC(SAMP::RPC::STOP_AUDIO_STREAM, nullptr, this->rakId, false);
}

void Player::SetSkillLevel(uint32_t skill, uint16_t level) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<uint32_t>(skill);
    bitStream.Write<uint16_t>(level);
    Server::RPC(SAMP::RPC::SET_PLAYER_SKILL_LEVEL, &bitStream, UNASSIGNED_PLAYER_ID, true);
}

void Player::RemoveBuilding(uint32_t model, Vector3 position, float radius) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint32_t>(model);
    bitStream.Write<Vector3>(position);
    bitStream.Write<float>(radius);
    Server::RPC(SAMP::RPC::SET_PLAYER_SKILL_LEVEL, &bitStream, this->rakId, false);
}

void Player::SetAttachedObject(uint32_t index, uint32_t model, uint32_t bone, Vector3 offset, Vector3 rotation, Vector3 scale) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<uint32_t>(index);
    bitStream.Write<bool>(true);
    bitStream.Write<uint32_t>(model);
    bitStream.Write<uint32_t>(bone);
    bitStream.Write<Vector3>(offset);
    bitStream.Write<Vector3>(rotation);
    bitStream.Write<Vector3>(scale);
    Server::RPC(SAMP::RPC::SET_PLAYER_ATTACHED_OBJECT, &bitStream, UNASSIGNED_PLAYER_ID, true);

    this->attachedObjects[index].model = model;
    this->attachedObjects[index].bone = bone;
    this->attachedObjects[index].offset = offset;
    this->attachedObjects[index].rotation = rotation;
    this->attachedObjects[index].scale = scale;
    this->attachedObjects[index].used = true;
}

void Player::RemoveAttachedObject(uint32_t index) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<uint32_t>(index);
    bitStream.Write<bool>(false);
    Server::RPC(SAMP::RPC::SET_PLAYER_ATTACHED_OBJECT, &bitStream, UNASSIGNED_PLAYER_ID, true);

    this->attachedObjects[index].used = false;
}

bool Player::IsAttachedObjectSlotUsed(uint32_t index) {
    return this->attachedObjects[index].used;
}

void Player::EditAttachedObject(uint32_t index) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint32_t>(index);
    Server::RPC(SAMP::RPC::EDIT_ATTACHED_OBJECT, &bitStream, this->rakId, false);
}

void Player::PutInVehicle(Vehicle *vehicle, uint8_t seat) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(vehicle->index);
    bitStream.Write<uint8_t>(seat);
    Server::RPC(SAMP::RPC::PUT_PLAYER_IN_VEHICLE, &bitStream, this->rakId, false);
}

Vehicle *Player::GetVehicle() {
    switch(this->state) {
        case State::Driver:
            return Server::vehicles[this->inCarInfo.vehicleId];
            break;

        case State::Passenger:
            return Server::vehicles[this->passengerInfo.vehicleId];
            break;
    }

    return nullptr;
}

int8_t Player::GetVehicleSeat() {
    switch(this->state) {
        case State::Driver:
            return 0;
            break;

        case State::Passenger:
            return this->passengerInfo.seatFlags;
            break;

        default:
            return -1;
    }
}

void Player::RemoveFromVehicle() {
    Server::RPC(SAMP::RPC::REMOVE_PLAYER_FROM_VEHICLE, nullptr, this->rakId, false);
}

void Player::ToggleControllable(bool toggle) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(toggle);
    Server::RPC(SAMP::RPC::TOGGLE_PLAYER_CONTROLLABLE, &bitStream, this->rakId, false);
}

void Player::PlaySound_(uint32_t sound, Vector3 position) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint32_t>(sound);
    bitStream.Write<Vector3>(position);
    Server::RPC(SAMP::RPC::PLAY_SOUND, &bitStream, this->rakId, false);
}

void Player::ApplyAnimation(const char *lib, const char *name, float delta, bool loop, bool lockx, bool locky, bool freeze, int32_t time) {
    uint8_t libLength = strlen(lib);
    uint8_t nameLength = strlen(name);

    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<uint8_t>(libLength);
    bitStream.Write(lib, libLength);
    bitStream.Write<uint8_t>(nameLength);
    bitStream.Write(name, nameLength);
    bitStream.Write<float>(delta);
    bitStream.Write<bool>(loop);
    bitStream.Write<bool>(lockx);
    bitStream.Write<bool>(locky);
    bitStream.Write<bool>(freeze);
    bitStream.Write<int32_t>(time);
    Server::RPC(SAMP::RPC::APPLY_ANIMATION, &bitStream, UNASSIGNED_PLAYER_ID, true);
}

void Player::ClearAnimations() {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    Server::RPC(SAMP::RPC::CLEAR_ANIMATIONS, &bitStream, UNASSIGNED_PLAYER_ID, true);
}

uint8_t Player::GetSpecialAction() {
    return this->onFootInfo.specialAction;
}

void Player::SetSpecialAction(uint8_t specialAction) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    Server::RPC(SAMP::RPC::SET_PLAYER_SPECIAL_ACTION, &bitStream, this->rakId, false);
}

void Player::SetCheckpoint(Vector3 position, float radius) {
    RakNet::BitStream bitStream;
    bitStream.Write<Vector3>(position);
    bitStream.Write<float>(radius);
    Server::RPC(SAMP::RPC::SET_CHECKPOINT, &bitStream, this->rakId, false);

    this->checkpointInfo.active = true;
    this->checkpointInfo.position = position;
    this->checkpointInfo.radius = radius;
}

void Player::DisableCheckpoint() {
    Server::RPC(SAMP::RPC::DISABLE_CHECKPOINT, nullptr, this->rakId, false);
    this->checkpointInfo.active = false;
}

void Player::SetRaceCheckpoint(uint8_t type, Vector3 position, Vector3 nextPosition, float radius) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint8_t>(type);
    bitStream.Write<Vector3>(position);
    bitStream.Write<Vector3>(nextPosition);
    bitStream.Write<float>(radius);
    Server::RPC(SAMP::RPC::SET_RACE_CHECKPOINT, &bitStream, this->rakId, false);

    this->raceCheckpointInfo.active = true;
    this->raceCheckpointInfo.position = position;
    this->raceCheckpointInfo.nextPosition = nextPosition;
    this->raceCheckpointInfo.radius = radius;
}

void Player::DisableRaceCheckpoint() {
    Server::RPC(SAMP::RPC::DISABLE_RACE_CHECKPOINT, nullptr, this->rakId, false);
    this->checkpointInfo.active = false;
}

void Player::SetWorldBounds(Vector2 max, Vector2 min) {
    RakNet::BitStream bitStream;
    bitStream.Write<Vector2>(max);
    bitStream.Write<Vector2>(min);
    Server::RPC(SAMP::RPC::SET_PLAYER_WORLD_BOUNDS, &bitStream, this->rakId, false);
}

void Player::SetColorForPlayer(Player *player, uint32_t color) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<uint8_t>(color);
    Server::RPC(SAMP::RPC::SET_PLAYER_COLOR, &bitStream, player->rakId, false);
}

void Player::ShowNameTagForPlayer(Player *player, bool show) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<bool>(show);
    Server::RPC(SAMP::RPC::SHOW_PLAYER_NAME_TAG_FOR_PLAYER, &bitStream, player->rakId, false);
}

void Player::SetMapIcon(uint8_t index, Vector3 position, uint8_t icon, uint32_t color, uint8_t style) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint8_t>(index);
    bitStream.Write<Vector3>(position);
    bitStream.Write<uint8_t>(icon);
    bitStream.Write<uint32_t>(color);
    bitStream.Write<uint8_t>(style);
    Server::RPC(SAMP::RPC::SET_PLAYER_MAP_ICON, &bitStream, this->rakId, false);
}

void Player::RemoveMapIcon(uint8_t index) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint8_t>(index);
    Server::RPC(SAMP::RPC::REMOVE_PLAYER_MAP_ICON, &bitStream, this->rakId, false);
}

void Player::SetCameraPosition(Vector3 position) {
    RakNet::BitStream bitStream;
    bitStream.Write<Vector3>(position);
    Server::RPC(SAMP::RPC::SET_PLAYER_CAMERA_POS, &bitStream, this->rakId, false);
}

void Player::SetCameraLookAt(Vector3 position) {
    RakNet::BitStream bitStream;
    bitStream.Write<Vector3>(position);
    Server::RPC(SAMP::RPC::SET_PLAYER_CAMERA_LOOK_AT, &bitStream, this->rakId, false);
}

void Player::SetCameraBehind() {
    Server::RPC(SAMP::RPC::SET_CAMERA_BEHIND_PLAYER, nullptr, this->rakId, false);
}

void Player::InterpolateCameraPosition(Vector3 from, Vector3 to, int32_t time, uint8_t mode) {
    RakNet::BitStream bitStream;
    bitStream.Write<bool>(true);
    bitStream.Write<Vector3>(from);
    bitStream.Write<Vector3>(to);
    bitStream.Write<int32_t>(time);
    bitStream.Write<uint8_t>(mode);
    Server::RPC(SAMP::RPC::INTERPOLATE_CAMERA, &bitStream, this->rakId, false);
}

void Player::InterpolateCameraLookAt(Vector3 from, Vector3 to, int32_t time, uint8_t mode) {
    RakNet::BitStream bitStream;
    bitStream.Write<bool>(false);
    bitStream.Write<Vector3>(from);
    bitStream.Write<Vector3>(to);
    bitStream.Write<int32_t>(time);
    bitStream.Write<uint8_t>(mode);
    Server::RPC(SAMP::RPC::INTERPOLATE_CAMERA, &bitStream, this->rakId, false);
}

bool Player::IsInCheckpoint() {
    return 
        Util::GetDistance(this->position, this->checkpointInfo.position) < this->checkpointInfo.radius;
}

bool Player::IsInRaceCheckpoint() {
    return 
        Util::GetDistance(this->position, this->raceCheckpointInfo.position) < this->raceCheckpointInfo.radius;
}

void Player::EnableStuntBonus(bool enable) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint8_t>(enable);
    Server::RPC(SAMP::RPC::ENABLE_STUNT_BONUS_FOR_PLAYER, &bitStream, this->rakId, false);
}

void Player::ToggleSpectating(bool toggle) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint8_t>(toggle ? 1 : 0);
    Server::RPC(SAMP::RPC::TOGGLE_PLAYER_SPECTATING, &bitStream, this->rakId, false);

    if(toggle)
        this->SetState(State::Spectating);
    else
        this->SetState(State::None);
}

void Player::SpectatePlayer(Player *player, uint8_t mode) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(player->index);
    bitStream.Write<uint8_t>(mode);
    Server::RPC(SAMP::RPC::PLAYER_SPECTATE_PLAYER, &bitStream, this->rakId, false);

    this->SetState(State::Spectating);
}

void Player::SpectateVehicle(Vehicle *vehicle, uint8_t mode) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(vehicle->index);
    bitStream.Write<uint8_t>(mode);
    Server::RPC(SAMP::RPC::PLAYER_SPECTATE_VEHICLE, &bitStream, this->rakId, false);

    this->SetState(State::Spectating);
}

void Player::CreateExplosion(Vector3 position, int32_t type, float radius) {
    RakNet::BitStream bitStream;
    bitStream.Write<Vector3>(position);
    bitStream.Write<int32_t>(type);
    bitStream.Write<float>(radius);
    Server::RPC(SAMP::RPC::CREATE_EXPLOSION, &bitStream, this->rakId, false);
}

void Player::SendClientMessage(uint32_t color, const char *format, ...) {
    va_list args;
    va_start(args, format);

    char buffer[256];
    uint32_t bufferLength = vsnprintf(buffer, 255, format, args);

    va_end(args);

    RakNet::BitStream bitStream;
    bitStream.Write<uint32_t>(color);
    bitStream.Write<uint32_t>(bufferLength);
    bitStream.Write(buffer, bufferLength);
    Server::RPC(SAMP::RPC::CLIENT_MESSAGE, &bitStream, this->rakId, false);
}

void Player::GameText(int32_t style, int32_t time, const char *format, ...) {
    va_list args;
    va_start(args, format);

    char buffer[256];
    int32_t bufferLength = vsnprintf(buffer, 255, format, args);

    va_end(args);

    RakNet::BitStream bitStream;
    bitStream.Write<int32_t>(style);
    bitStream.Write<int32_t>(time);
    bitStream.Write<int32_t>(bufferLength);
    bitStream.Write(buffer, bufferLength);
    Server::RPC(SAMP::RPC::DISPLAY_GAME_TEXT, &bitStream, this->rakId, false);
}

void Player::EnableCursor(bool enable, uint32_t color) {
    RakNet::BitStream bitStream;
    bitStream.Write<bool>(enable);
    bitStream.Write<uint32_t>(color);

    Server::RPC(
        SAMP::RPC::CLICK_TEXT_DRAW, &bitStream, this->rakId, false);
}

void Player::ShowDialog(uint16_t id, uint8_t style, const char *title,
    const char *button1, const char *button2, const char *info) {

    uint8_t titleLength = strlen(title);
    uint8_t button1Length = strlen(button1);
    uint8_t button2Length = strlen(button2);
    
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(id);
    bitStream.Write<uint8_t>(style);
    bitStream.Write<uint8_t>(titleLength);
    bitStream.Write(title, titleLength);
    bitStream.Write<uint8_t>(button1Length);
    bitStream.Write(button1, button1Length);
    bitStream.Write<uint8_t>(button2Length);
    bitStream.Write(button2, button2Length);
    stringCompressor->EncodeString(info, 4096, &bitStream);

    Server::RPC(
        SAMP::RPC::SHOW_DIALOG, &bitStream, this->rakId, false);
}

bool Player::IsConnected() {
    return Server::rakServer->IsActivePlayerID(this->rakId);
}

bool Player::IsNPC() {
    return false;
}

void Player::Kick() {
    Server::rakServer->Kick(this->rakId);
}

uint8_t Player::GetWeapon() {
    switch(this->state) {
        case State::OnFoot:
            return this->onFootInfo.currentWeapon;
            break;

        case State::Driver:
            return this->inCarInfo.currentWeapon;
            break;

        case State::Passenger:
            return this->passengerInfo.currentWeapon;
            break;
    }

    return 0;
}

void Player::Ban() {
    char ip[22] = {0};
    unsigned short port = 0;
    Server::rakServer->GetPlayerIPFromID(this->rakId, ip, &port);
    Server::rakServer->AddToBanList(ip);
}

void Player::Ban(const char *reason) {
    this->SendClientMessage(0xFFFFFFFF, "[banned] reason: %s", reason);
    this->Ban();
}

void Player::ChatBubble(const char *text, uint32_t color, float drawDistance, uint32_t expireTime) {
    size_t length = strlen(text);
    if(length >= 255)
        throw PlayerException("length >= 255");

    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<uint32_t>(color);
    bitStream.Write<float>(drawDistance);
    bitStream.Write<uint32_t>(expireTime);
    bitStream.Write<uint8_t>((uint8_t)length);
    bitStream.Write(text, length);

    Server::RPC(SAMP::RPC::CHAT_BUBBLE, &bitStream, this->rakId, true);
}