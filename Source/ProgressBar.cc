#include "Zion/ProgressBar.hh"
using namespace Zion;

ProgressBar::ProgressBar(Vector2 position, float width, float height, float max, float value, uint32_t color) {
    this->back = new TextDraw(position, "_");
    this->back->UseBox(true);
    this->back->SetTextSize(position.x + width - 4.0F, 0.0F);
    this->back->SetLetterSize(1.0F, height / 10.0F);
    this->back->SetBoxColor(0x00000000 | (color & 0x000000FF));

    this->fill = new TextDraw({position.x + 1.2F, position.y + 2.15F}, "_");
    this->fill->UseBox(true);
    this->fill->SetTextSize(position.x + width - 5.5F, 0.0F);
    this->fill->SetLetterSize(1.0F, height / 10.0F - 0.35F);
    this->fill->SetBoxColor((color & 0xFFFFFF00) | (0x66 & ((color & 0x000000FF) / 2)));

    this->main = new TextDraw({position.x + 1.2F, position.y + 2.15F}, "_");
    this->main->UseBox(true);

    if(value < 0)
        this->main->SetTextSize(Percent(position.x, width, max, 0.0F), 0.0F);
    else if(value > max)
        this->main->SetTextSize(Percent(position.x, width, max, max), 0.0F);
    else
        this->main->SetTextSize(Percent(position.x, width, max, value), 0.0F);

    this->main->SetLetterSize(1.0F, height / 10.0F - 0.35F);
    this->main->SetBoxColor(color);

    this->position = position;
    this->height = height;
    this->width = width;
    this->max = max;
    this->value = value;
    this->color = color;
}

void ProgressBar::ShowForAll() {
    this->back->ShowForAll();
    this->fill->ShowForAll();
    this->main->ShowForAll();
}

void ProgressBar::ShowForPlayer(Player *player) {
    this->back->ShowForPlayer(player);
    this->fill->ShowForPlayer(player);
    this->main->ShowForPlayer(player);
}

void ProgressBar::HideForAll() {
    this->back->HideForAll();
    this->fill->HideForAll();
    this->main->HideForAll();
}

void ProgressBar::HideForPlayer(Player *player) {
    this->back->HideForPlayer(player);
    this->fill->HideForPlayer(player);
    this->main->HideForPlayer(player);
}

void ProgressBar::Update() {
    this->back->SetTextSize(this->position.x + this->width - 4.0F, 0.0F);
    this->back->SetLetterSize(1.0F, this->height / 10.0F);
    this->back->SetBoxColor(0x00000000 | (this->color & 0x000000FF));
    this->back->UpdateForAll();

    this->fill->SetTextSize(this->position.x + this->width - 5.5F, 0.0F);
    this->fill->SetLetterSize(1.0F, this->height / 10.0F - 0.35F);
    this->fill->SetBoxColor((this->color & 0xFFFFFF00) | (0x66 & ((this->color & 0x000000FF) / 2)));
    this->fill->UpdateForAll();

    if(this->value < 0)
        this->main->SetTextSize(this->Percent(this->position.x, this->width, this->max, 0.0F), 0.0F);
    else if(this->value > this->max)
        this->main->SetTextSize(this->Percent(this->position.x, this->width, this->max, this->max), 0.0F);
    else
        this->main->SetTextSize(this->Percent(this->position.x, this->width, this->max, this->value), 0.0F);

    this->main->SetLetterSize(1.0F, this->height / 10.0F - 0.35F);
    this->main->SetBoxColor(this->color);
    this->main->UpdateForAll();
}

float ProgressBar::Percent(float x, float width, float max, float value) {
    return ((x - 3.0) + (((((x - 2.0) + width) - x) / max) * value));
}