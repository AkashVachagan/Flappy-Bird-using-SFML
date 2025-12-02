#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <string>

const float gravity = 0.025;
const float birdScaleX = 0.27;
const float birdScaleY = 0.32;
const float flapVel = -3.5;
const float bgScaleX = 2.2;
const float bgScaleY = 2.1;
const int birdInitX = 100;
const int birdInitY = 250;
const int birdInitVel = -5;
const int groundX = 0;
const int groundY = 900;
const float groundScaleX = 3.2;
const float groundScaleY = 1;

class Bird{
    public:
    float x;
    float y;
    float vel;
    sf::Sprite birdSprite;

    Bird(float x, float y, float vel, const sf::Texture& texture){
        this->x = x;
        this->y = y;
        this->vel = vel;
        birdSprite.setTexture(texture);
        birdSprite.setScale(birdScaleX, birdScaleY);
    }

    void flap(){
        vel = flapVel;
    }

    void calc(){
        vel += gravity;
        y += 0.5*vel;
        birdSprite.setPosition(x, y);
    }
};

class Pipe{
    public:
        float lowerPipeHeight;
        float upperPipeHeight;
        sf::Sprite lowerPipeSprite;
        sf::Sprite upperPipeSprite;
        int x;
        bool entered;

        Pipe(float lowerPipeHeight, sf::Texture& lowerPipeTex, sf::Texture& upperPipeTex){
            this->lowerPipeHeight = lowerPipeHeight;
            this->upperPipeHeight = lowerPipeHeight - 300;
            this->lowerPipeSprite.setTexture(lowerPipeTex);
            this->upperPipeSprite.setTexture(upperPipeTex);
            this->entered = false;
            lowerPipeSprite.setScale(0.5, 1);
            upperPipeSprite.setScale(0.5, 1);
            upperPipeSprite.setOrigin(0, upperPipeSprite.getGlobalBounds().height);
            this->x = sf::VideoMode::getDesktopMode().width;
        }

        void update(){
            lowerPipeSprite.setPosition(x, lowerPipeHeight);
            upperPipeSprite.setPosition(x, upperPipeHeight);
        }

        void calc(){
            x -= 2;
        }
};

float getRandomHeight(){
    float h = 400 + (rand() % 300);
    return h;
}


bool checkCollision(sf::Sprite& birdSprite, sf::Sprite& groundSprite, const std::vector<Pipe>& pipes){
    
    sf::FloatRect birdBox = birdSprite.getGlobalBounds();
    birdBox.left += 30;
    birdBox.top += 40;
    birdBox.width -= 55;
    birdBox.height -= 75;
    sf::FloatRect groundBox = groundSprite.getGlobalBounds();

    if (birdBox.intersects(groundBox)){
        return true;
    }

    for (auto& p: pipes){
        sf::FloatRect lPipeBox = p.lowerPipeSprite.getGlobalBounds();
        sf::FloatRect hPipeBox = p.upperPipeSprite.getGlobalBounds();

        if (birdBox.intersects(lPipeBox) || birdBox.intersects(hPipeBox)){
            return true;
        }
    }

    return false;
}

bool getCount(sf::Sprite& birdSprite, std::vector<Pipe>& pipes){
    sf::FloatRect birdBox = birdSprite.getGlobalBounds();
    birdBox.left += 30;
    birdBox.top += 40;

    for (auto& p: pipes){
        if (!p.entered && birdBox.left > p.lowerPipeSprite.getGlobalBounds().left && birdBox.top > p.upperPipeHeight){
            p.entered = true;
            return true;
        }
    }

    return false;
}

int main(){
    srand(time(0));
    int count = 0;
    sf::VideoMode mode = sf::VideoMode::getFullscreenModes()[0];
    sf::RenderWindow window(mode, "Flappy Bird");
    window.setVerticalSyncEnabled(true);
    sf::Font font;
    font.loadFromFile("./fonts/Orbitron/Orbitron-VariableFont_wght.ttf");
    sf::Text text;
    text.setFont(font);
    text.setFillColor(sf::Color(255, 0, 0, 150));
    text.setCharacterSize(150);
    text.setPosition(900, 20);
    text.setStyle(sf::Text::Bold);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(3.f);
    sf::Texture birdTex;
    sf::Texture bgTex;
    sf::Texture groundTex;
    sf::Texture lowerPipeTex;
    sf::Texture upperPipeTex;
    bgTex.loadFromFile("./imgs/background.png");
    birdTex.loadFromFile("./imgs/bird.png");
    groundTex.loadFromFile("./imgs/ground.png");
    lowerPipeTex.loadFromFile("./imgs/downPipe.png");
    upperPipeTex.loadFromFile("./imgs/upPipe.png");
    sf::Sprite bgSprite(bgTex);
    bgSprite.setScale(bgScaleX, bgScaleY);
    sf::Sprite groundSprite(groundTex);
    groundSprite.setPosition(groundX, groundY);
    groundSprite.setScale(groundScaleX, groundScaleY);
    Bird bird(birdInitX, birdInitY, birdInitVel, birdTex);

    sf::Time pipeInterval = sf::seconds(0.75f);
    sf::Clock clock;
    std::vector<Pipe> pipes;
    while (window.isOpen() && window.hasFocus()){ // remove hasFocus() later
        sf::Event event;

        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed){
                window.close();
            }

            if (event.type == sf::Event::KeyPressed){
                if (event.key.scancode == sf::Keyboard::Scan::Space){
                    bird.flap();
                }
            }
        }

        if (clock.getElapsedTime() > pipeInterval){
            pipes.emplace_back(getRandomHeight(), lowerPipeTex, upperPipeTex);
            clock.restart();
        }

        bird.calc();
        window.clear();
        window.draw(bgSprite);

        for (auto& p: pipes){
            p.update();
            window.draw(p.lowerPipeSprite);
            window.draw(p.upperPipeSprite);
            
            p.calc();
        }
        
        pipes.erase(std::remove_if(pipes.begin(), pipes.end(), [](const Pipe& pipe){return pipe.x < -pipe.upperPipeSprite.getGlobalBounds().width-20;}), pipes.end());

        window.draw(groundSprite);
        window.draw(bird.birdSprite);

        if (getCount(bird.birdSprite, pipes)){
            count++;
        }
        
        text.setString(std::to_string(count));
        window.draw(text);

        if (checkCollision(bird.birdSprite, groundSprite, pipes)){
            break;
        }
        
        window.display();
    }
}
