#include "Zion/TextDraw.hh"
#include "Zion/Server.hh"
#include "Zion/Util.hh"
#include "RakNet/SAMP/SAMP.h"
using namespace Zion;

TextDraw::TextDraw(Vector2 position, const char *text) {
    if(freeIds.GetLength() > 0)
        this->index = freeIds.Pop();
    else
        this->index = nextId++;

    if(this->index < Server::textDraws.GetLength())
        Server::textDraws[this->index] = this;
    else {
        while(Server::textDraws.Push(nullptr) < this->index);
        Server::textDraws[this->index] = this;
    }

    this->tdt.box = 0;
    this->tdt.left = 0;
    this->tdt.right = 0;
    this->tdt.center = 0;
    this->tdt.proportional = 1;
    this->tdt.letterWidth = 0.48F;
    this->tdt.letterHeight = 1.12F;
    this->tdt.letterColor = 0xFFE1E1E1;
    this->tdt.lineWidth = 1280.0F;
    this->tdt.lineHeight = 1280.0F;
    this->tdt.boxColor = 0x80808080;
    this->tdt.shadow = 2;
    this->tdt.outline = 0;
    this->tdt.backgroundColor = 0xFF000000;
    this->tdt.style = 1;
    this->tdt.x = position.x;
    this->tdt.y = position.y;

    int length = strlen(text);

    this->text = (char *)calloc(length + 1, sizeof(char));
    strcpy(this->text, text);
    this->textLength = length;
}

TextDraw::~TextDraw() {
    this->HideForAll();
    delete[] this->text;

    if(this->index == Server::textDraws.GetLength() - 1) {
        while(Server::textDraws.GetLength() > 0 && Server::textDraws.Back() == nullptr)
            Server::textDraws.Pop();
    }
    else
        Server::textDraws[this->index] = nullptr;

    freeIds.Push(this->index);
}

void TextDraw::SetPosition(Vector2 position) {
    this->tdt.x = position.x;
    this->tdt.y = position.y;
}

void TextDraw::SetLetterSize(float width, float height) {
    this->tdt.letterWidth = width;
    this->tdt.letterHeight = height;
}

void TextDraw::SetTextSize(float width, float height) {
    this->tdt.lineWidth = width;
    this->tdt.lineHeight = height;
}

void TextDraw::SetAlignment(int alignment) {
    this->tdt.left = 0;
    this->tdt.center = 0;
    this->tdt.right = 0;

    switch(alignment) {
        case 1:
            this->tdt.left = 1;
            break;

        case 2:
            this->tdt.center = 1;
            break;

        case 3:
            this->tdt.right = 1;
            break;
    }
}

void TextDraw::SetLetterColor(uint32_t color) {
    this->tdt.letterColor = Util::RGBAToARGB(color);
}

void TextDraw::UseBox(bool use) {
    this->tdt.box = use;
}

void TextDraw::SetBoxColor(uint32_t color) {
    this->tdt.boxColor = Util::RGBAToARGB(color);
}

void TextDraw::SetShadow(uint8_t size) {
    this->tdt.shadow = size;
}

void TextDraw::SetOutline(uint8_t size) {
    this->tdt.outline = size;
}

void TextDraw::SetBackgroundColor(uint32_t color) {
    this->tdt.backgroundColor = Util::RGBAToARGB(color);
}

void TextDraw::SetStyle(uint8_t style) {
    this->tdt.style = style;
}

void TextDraw::SetProportional(bool proportional) {
    this->tdt.proportional = proportional;
}

void TextDraw::SetSelectable(uint8_t set) {
    this->tdt.selectable = set;
}

void TextDraw::SetText(const char *text) {
    delete[] this->text;

    int length = strlen(text);

    this->text = (char *)calloc(length + 1, sizeof(char));
    strcpy(this->text, text);
    this->textLength = length;

    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<uint16_t>(this->textLength);
    bitStream.Write(this->text, this->textLength);

    Server::RPC(
        SAMP::RPC::TEXT_DRAW_SET_STRING, &bitStream, UNASSIGNED_PLAYER_ID, true);
}

void TextDraw::SetPreviewModel(int model) {
    this->tdt.model = model;
}

void TextDraw::SetPreviewRotation(Vector3 rotation, float zoom)  {
    this->tdt.rotation = rotation;
    this->tdt.zoom = zoom;
}

void TextDraw::SetPreviewVehicleColor(uint8_t color1, uint8_t color2) {
    this->tdt.color1 = color1;
    this->tdt.color2 = color2;
}

void TextDraw::UpdateForPlayer(Player *player) {
    this->HideForPlayer(player);
    this->ShowForPlayer(player);
}

void TextDraw::UpdateForAll() {
    this->HideForAll();
    this->ShowForAll();
}

void TextDraw::ShowForAll() {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<TextDrawTransmit>(this->tdt);
    bitStream.Write<uint16_t>(this->textLength);
    bitStream.Write(this->text, this->textLength);

    Server::RPC(
        SAMP::RPC::SHOW_TEXT_DRAW, &bitStream, UNASSIGNED_PLAYER_ID, true);
}

void TextDraw::HideForAll() {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);

    Server::RPC(
        SAMP::RPC::TEXT_DRAW_HIDE, &bitStream, UNASSIGNED_PLAYER_ID, true);
}

void TextDraw::ShowForPlayer(Player *player) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);
    bitStream.Write<TextDrawTransmit>(this->tdt);
    bitStream.Write<uint16_t>(this->textLength);
    bitStream.Write(this->text, this->textLength);

    Server::RPC(
        SAMP::RPC::SHOW_TEXT_DRAW, &bitStream, player->rakId, false);
}

void TextDraw::HideForPlayer(Player *player) {
    RakNet::BitStream bitStream;
    bitStream.Write<uint16_t>(this->index);

    Server::RPC(
        SAMP::RPC::TEXT_DRAW_HIDE, &bitStream, player->rakId, false);
}

List<uint16_t> TextDraw::freeIds;
uint16_t TextDraw::nextId = 0;