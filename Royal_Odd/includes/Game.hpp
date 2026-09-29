#ifndef GAME_HPP
#define GAME_HPP

#include "Ankaria.h"

class Instance : public Ankaria
{
public:
    Instance (Settings settings) : Ankaria(settings) {};
    void Game() override;
};

#endif