#include "Zion/GameMode.hh"
#include "Zion/Server.hh"

class Bare : public Zion::GameMode {
    public:
        void OnGameModeInit();
        void OnPlayerConnect(Zion::Player *player);
        void OnPlayerSpawn(Zion::Player *player);
        bool OnPlayerRequestClass(Zion::Player *player, int classId);
};

void Bare::OnGameModeInit() {
    Zion::Server::SetServerName("Bare");
    Zion::Server::showPlayerMarkers = 1;
    Zion::Server::showPlayerTags = true;

    Zion::Server::AddPlayerClass(
        255, 265, {1958.3783F, 1343.1572F, 15.3746F}, 270.1425F, 0, 0, 0, 0, -1, -1);
}

void Bare::OnPlayerConnect(Zion::Player *player) {
    player->GameText(5, 5000, "~w~Zion: ~r~Bare Gamemode");
}

void Bare::OnPlayerSpawn(Zion::Player *player) {
    player->SetInterior(0);
    player->ToggleClock(false);
}

bool Bare::OnPlayerRequestClass(Zion::Player *player, int classId) {
    player->SetInterior(14);
    player->SetPosition({258.4893F, -41.4008F, 1002.0234F});
    player->SetFacingAngle({270.0F});
    player->SetCameraPosition({256.0815F, -43.0475F, 1004.0234F});
    player->SetCameraLookAt({258.4893F, -41.4008F, 1002.0234F});

    return true;
}

int main(int argc, char **argv) {

    Zion::Server::gameMode = new Bare();
    Zion::Server::Start(32);
    atexit(Zion::Server::Stop);

    while(true)
        Zion::Server::Update();

    return 0;
}