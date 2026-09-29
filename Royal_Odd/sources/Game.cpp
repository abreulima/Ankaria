#include "Game.hpp"
#include <iostream>

void Instance::Game()
{
    Image cards = resources.LoadImage("cards", "cards.png");
    resources.CreateImageFromImage(cards, "Back", { 0, 0, 47, 63 });
    resources.CreateImageFromImage(cards, "2S", { 0, 64, 47, 63 });
    resources.CreateImageFromImage(cards, "3S", { 48, 64, 47, 63 });
    resources.CreateImageFromImage(cards, "4S", { 96, 64, 47, 63 });
    resources.CreateImageFromImage(cards, "5S", { 144, 64, 47, 63 });
    resources.CreateImageFromImage(cards, "6S", { 192, 64, 47, 63 });
    resources.CreateImageFromImage(cards, "7S", { 240, 64, 47, 63 });
    resources.CreateImageFromImage(cards, "8S", { 288, 64, 47, 63 });
    resources.CreateImageFromImage(cards, "9S", { 336, 64, 47, 63 });
    resources.CreateImageFromImage(cards, "AS", { 384, 64, 47, 63 });
    resources.CreateImageFromImage(cards, "JS", { 432, 64, 47, 63 });
    resources.CreateImageFromImage(cards, "QS", { 480, 64, 47, 63 });
    resources.CreateImageFromImage(cards, "KS", { 528, 64, 47, 63 });

    resources.LoadFont("monogram", "monogram-extended.ttf", 16);

    renderer.CreatePipeline("nebula", "nebula.wgsl");

    Scene game(*this);

    float scale = 2.0f;
    float margin = 50.0f;

    // Background
    {
        game.Add({
            Scale{1920, 1080},
            Sprite{"8S"},
            Shader{"nebula"},
        });
    }

    // Title
    {
        game.Add({
            Text{"Hello, welcome to my game!"},
            Position{settings.width / 2.0f, margin},
            Anchor{AnchorPosition::CENTER},
            Scale{scale}
        });
    }

    //

    scenes.Add("main", game);

    std::cout << "Hello World" << std::endl;
}

int main()
{
    Instance game(Settings
        {
            .title = "Royal Odd",
            .width = 1920,
            .height = 1080,
            .scale = 1.0f
        }
    );
    
    game.Run();
}