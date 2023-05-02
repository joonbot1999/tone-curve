#define _USE_MATH_DEFINES
#include <iostream>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <src/spline.h>
//#include "tiffio.h"
//#pragma comment(lib, "libtiff")

using namespace std;
//Preset
// DO NOT TOUCH THESE INITIAL VERTICES
// (0 - 1023) -> from 2 ^ 16 / 64
// (0 - 255) -> from 2 ^ 16 / 256 or 2 ^ 10 / 4

vector < sf::Vector2f > vertVec{ {0, 255}, {255, 0} };
vector < sf::Sprite > spriteVec;
vector<sf::Sprite> savedSprite;
vector < double > xCoord{ 0, 255 };
vector < double > yCoord{ 255, 0 };
vector<sf::RectangleShape> rectVec;
vector<sf::CircleShape> buttVec;
map < int, double > xyMap;
map < int, double > tbitMap;
map < int, double > sbitMap;
tk::spline spline;
int spriteCoord;
int buttonIndex;
vector<int> tiffVec8;

/*std::pair<int, int> getImageSize(const std::string strPath) {
    TIFF* read = TIFFOpen(strPath.c_str(), "r");
    int nWidth, nHeight;
    if (read)
    {
        unsigned short* buf = NULL;
        buf = (unsigned short*)_TIFFmalloc(TIFFScanlineSize(read));
        int nImageLength = 0;
        TIFFGetField(read, TIFFTAG_IMAGELENGTH, &nImageLength);
        TIFFGetField(read, TIFFTAG_IMAGEWIDTH, &nWidth); // set the width of the image
        TIFFGetField(read, TIFFTAG_IMAGELENGTH, &nHeight);    // set the height of theimage
        _TIFFfree(buf);
        TIFFClose(read);
    }
    return std::make_pair(nWidth, nHeight);
}

bool LoadTiffFile(const std::string &strPath, uint16_t* pImage, int nImageSize)
{
    bool bRet = false;
    if (pImage)
    {
        TIFF* read = TIFFOpen(strPath.c_str(), "r");
        if (read)
        {
            int nWidth, nHeight;
            unsigned short* buf = NULL;
            buf = (unsigned short*)_TIFFmalloc(TIFFScanlineSize(read));
            int nImageLength = 0;
            TIFFGetField(read, TIFFTAG_IMAGELENGTH, &nImageLength);
            TIFFGetField(read, TIFFTAG_IMAGEWIDTH, &nWidth); // set the width of the image
            TIFFGetField(read, TIFFTAG_IMAGELENGTH, &nHeight);    // set the height of theimage
            if (nImageSize <= nWidth * nHeight)
            {
                for (int row = 0; row < nImageSize; row++)
                {
                    int nOffset = row * nWidth;
                    if (TIFFReadScanline(read, buf, row, 0) < 0)
                        break;
                    memcpy(&pImage[nOffset], buf, sizeof(uint16_t) * nWidth);
                }
                bRet = true;
            }
            _TIFFfree(buf);
            TIFFClose(read);
        }
    }
    return bRet;
}

void tiffTest() {
    uint16_t* pImage16 = NULL;
    std::string fn = ".\\test.tif";
    std::pair<int, int> img_size  = getImageSize(fn);
    auto len = img_size.first * img_size.second;
    pImage16 = new uint16_t[len];
    bool bRet = LoadTiffFile(fn, pImage16, len);
    for (int i = 0; i < len; i++) {
        tiffVec8.push_back(pImage16[i] / 256);
    }
}*/

// Returns a sprite used for the control
sf::Sprite returnSprite() {
    sf::Texture texture;
    if (!texture.loadFromFile("Sprite/button.png")) {
        cout << "No img in the directory!!!";
        exit(1);
    };
    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setScale(float(0.01), float(0.01));
    return sprite;
}

// Returns an image to be casted/copied
sf::Image returnImage() {
    sf::Image mappingImg;
    sf::Texture texture;
    if (!mappingImg.loadFromFile("Sprite/test3.jpg")) {
        cout << "No img in the directory!!!";
        exit(1);
    }
    texture.loadFromImage(mappingImg);
    /*tiffTest();
    sf::Image image;
    sf::Texture texture;
    image.create(1000, 1000);
    for (int i = 0; i < 1000 * 1000; i++) {
        sf::Color c;
        c = sf::Color(sf::Uint8(tiffVec8[i]), sf::Uint8(tiffVec8[i]), sf::Uint8(tiffVec8[i]), 255);
        image.setPixel(i % 1000, i / 1000, c);
    }
    
    texture.loadFromImage(image);*/
    return mappingImg;
}

// Makes a map using cubic spline interpolation
void splineMake() {
    spline.set_points(xCoord, yCoord);
    for (int i = 0; i < 256; i++) {
        if (spline(i) >= 255) { // If its splining point is above 255
            xyMap[i] = 255;
            sbitMap[i] = 65535;
        }
        else if (spline(i) <= 0) { // If its splining point is below 0
            xyMap[i] = 0;
            sbitMap[i] = 0;
        }
        else {
            xyMap[i] = spline(i);
            sbitMap[i] = (spline(i) * 256) - 1;
        }
    }
}

void vertexSort(sf::Vector2f vertex) {
    size_t sz = 0;
    if (std::find(vertVec.begin(), vertVec.end(), vertex) != vertVec.end()) { // If there's a duplicate value
        return;
    }
    else {
        while (sz < spriteVec.size()) {
            if (vertex.x <= 0 || vertex.x >= 255) { // Left and right boundary check
                break;
            }
            if (vertex.y <= 0) { // Lower boundary check
                vertex.y = 0;
            }
            else if (vertex.y >= 255) { // Upper boundary check
                vertex.y = 255;
            }
            if (vertex.x == spriteVec[sz].getPosition().x) { // Duplicate x value
                break;
            }
            if (vertex.x <= spriteVec[sz].getPosition().x) {
                sf::Sprite sprite = returnSprite();
                vertVec.insert(vertVec.begin() + sz, vertex);
                if (vertex.y == 0) {
                    sprite.setPosition(vertex.x, vertex.y);
                }
                else {
                    sprite.setPosition(vertex.x - 4, vertex.y - 4);
                }
                spriteVec.insert(spriteVec.begin() + sz, sprite);
                xCoord.insert(xCoord.begin() + sz, vertex.x);
                yCoord.insert(yCoord.begin() + sz, vertex.y);
                break;
            }
            sz++;
        }
    }
}

// Check for a sprite's boundary
void spriteBoundary(sf::Vector2i relativePos) {
    vector<int> obj;
    size_t spriteTrack = 0;
    while (spriteTrack < spriteVec.size()) {
        if (spriteVec[spriteTrack].getGlobalBounds().contains(relativePos.x, relativePos.y)) { // If the sprite is with in the boundary
            spriteCoord = spriteTrack;
            break;
        }
        spriteTrack++;
    }
}

// Erase a sprite
void erase() {
    spriteVec.erase(spriteVec.begin() + spriteCoord);
    vertVec.erase(vertVec.begin() + spriteCoord);
    xCoord.erase(xCoord.begin() + spriteCoord);
    yCoord.erase(yCoord.begin() + spriteCoord);
}

int main() {
    sf::Text rs;
    rs.setString("Reset");
    sf::Text togOn;
    togOn.setString("Manual On");
    sf::Text togOff;
    togOff.setString("Manual Off");
    
    // Reset
    sf::RectangleShape toggleOff;
    toggleOff.setSize(sf::Vector2f(100, 50));
    toggleOff.setOutlineColor(sf::Color::Red);
    toggleOff.setOutlineThickness(5);
    toggleOff.setPosition(64, 300);
    rectVec.push_back(toggleOff);

    // Turn on
    sf::RectangleShape toggleOn;
    toggleOn.setSize(sf::Vector2f(100, 50));
    toggleOn.setOutlineColor(sf::Color::Green);
    toggleOn.setOutlineThickness(5);
    toggleOn.setPosition(64, 400);
    rectVec.push_back(toggleOn);

    // Turn off
    sf::RectangleShape toggleBackOn;
    toggleBackOn.setSize(sf::Vector2f(100, 50));
    toggleBackOn.setOutlineColor(sf::Color::Blue);
    toggleBackOn.setOutlineThickness(5);
    toggleBackOn.setPosition(64, 500);
    rectVec.push_back(toggleBackOn);

    // Save
    sf::RectangleShape toggleSave;
    toggleSave.setSize(sf::Vector2f(100, 50));
    toggleSave.setOutlineColor(sf::Color::Cyan);
    toggleSave.setOutlineThickness(5);
    toggleSave.setPosition(400, 78);
    rectVec.push_back(toggleSave);

    // Load
    sf::RectangleShape toggleLoad;
    toggleLoad.setSize(sf::Vector2f(100, 50));
    toggleLoad.setOutlineColor(sf::Color::Magenta);
    toggleLoad.setOutlineThickness(5);
    toggleLoad.setPosition(550, 78);
    rectVec.push_back(toggleLoad);

    sf::CircleShape buttonUp(20, 3);
    buttonUp.setPosition(103, 700);
    buttVec.push_back(buttonUp);

    sf::CircleShape buttonDown(20, 3);
    buttonDown.rotate(180.0f);
    buttonDown.setPosition(143, 800);
    buttVec.push_back(buttonDown);

    sf::CircleShape buttonLeft(20, 3);
    buttonLeft.rotate(270.0f);
    buttonLeft.setPosition(73, 770);
    buttVec.push_back(buttonLeft);

    sf::CircleShape buttonRight(20, 3);
    buttonRight.rotate(90.0f);
    buttonRight.setPosition(173, 730);
    buttVec.push_back(buttonRight);

    sf::Sprite init1 = returnSprite();
    init1.setScale(0, 0);
    init1.setPosition(0, 255);

    sf::Sprite init2 = returnSprite();
    init2.setScale(0, 0);
    init2.setPosition(255, 0);

    spriteVec.push_back(init1);
    spriteVec.push_back(init2);

    sf::Image mappingImg;
    sf::Image stationary;
    mappingImg = returnImage();
    stationary = returnImage();

    const sf::Uint8* sth = mappingImg.getPixelsPtr();
    const sf::Uint8* toChange;

    int xPixel = mappingImg.getSize().x;
    int yPixel = mappingImg.getSize().y;
    sf::RenderWindow window(sf::VideoMode(258 + xPixel, 258 + yPixel), "SFML window", sf::Style::Close);
    window.setPosition(sf::Vector2i(0, 0));
    sf::Texture textureImage;
    textureImage.loadFromImage(mappingImg);
    sf::Sprite pic(textureImage);
    bool dragging = false;
    bool outOfBounds = false;
    bool reset = false;
    bool manualControl = false;
    bool manualOff = false;
    bool buttonClick = false;
    bool save = false;
    bool load = false;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (manualOff) {
                manualControl = false;
                manualOff = false;
            }
            // Manual control mode
            else if (manualControl && buttonClick && event.type == sf::Event::MouseButtonReleased) {
                // Up
                if (buttonIndex == 0) {
                    spriteVec[spriteCoord].setPosition(spriteVec[spriteCoord].getPosition().x, spriteVec[spriteCoord].getPosition().y - 1);
                    xCoord.erase(xCoord.begin() + spriteCoord);
                    xCoord.insert(xCoord.begin() + spriteCoord, spriteVec[spriteCoord].getPosition().x);
                    yCoord.erase(yCoord.begin() + spriteCoord);
                    yCoord.insert(yCoord.begin() + spriteCoord, spriteVec[spriteCoord].getPosition().y - 1);
                }
                // Down
                else if (buttonIndex == 1) {
                    spriteVec[spriteCoord].setPosition(spriteVec[spriteCoord].getPosition().x, spriteVec[spriteCoord].getPosition().y + 1);
                    xCoord.erase(xCoord.begin() + spriteCoord);
                    xCoord.insert(xCoord.begin() + spriteCoord, spriteVec[spriteCoord].getPosition().x);
                    yCoord.erase(yCoord.begin() + spriteCoord);
                    yCoord.insert(yCoord.begin() + spriteCoord, spriteVec[spriteCoord].getPosition().y + 1);
                }
                // Left
                else if (buttonIndex == 2) {
                    spriteVec[spriteCoord].setPosition(spriteVec[spriteCoord].getPosition().x - 1, spriteVec[spriteCoord].getPosition().y);
                    xCoord.erase(xCoord.begin() + spriteCoord);
                    xCoord.insert(xCoord.begin() + spriteCoord, spriteVec[spriteCoord].getPosition().x - 1);
                    yCoord.erase(yCoord.begin() + spriteCoord);
                    yCoord.insert(yCoord.begin() + spriteCoord, spriteVec[spriteCoord].getPosition().y);
                }
                // Right
                else if (buttonIndex == 3) {
                    cout << endl << spriteVec[spriteCoord].getPosition().x << ", " << spriteVec[spriteCoord].getPosition().y;
                    spriteVec[spriteCoord].setPosition(spriteVec[spriteCoord].getPosition().x + 1, spriteVec[spriteCoord].getPosition().y);
                    cout << endl << spriteVec[spriteCoord].getPosition().x << ", " << spriteVec[spriteCoord].getPosition().y;
                    xCoord.erase(xCoord.begin() + spriteCoord);
                    xCoord.insert(xCoord.begin() + spriteCoord, spriteVec[spriteCoord].getPosition().x + 1);
                    yCoord.erase(yCoord.begin() + spriteCoord);
                    yCoord.insert(yCoord.begin() + spriteCoord, spriteVec[spriteCoord].getPosition().y);
                }
                buttonClick = false;
            }
            // toggle save
            else if (save && event.type == sf::Event::MouseButtonReleased) {
                savedSprite = spriteVec;
                save = false;
            }
            // toggle load
            else if (load && event.type == sf::Event::MouseButtonReleased) {
                spriteVec = savedSprite;
                xCoord.clear();
                yCoord.clear();
                for (int i = 0; i < spriteVec.size(); i++) {
                    xCoord.push_back(spriteVec[i].getPosition().x);
                    yCoord.push_back(spriteVec[i].getPosition().y);
                }
                load = false;
            }
            // Completely resets the curve
            else if (reset && event.type == sf::Event::MouseButtonReleased) {
                if (spriteVec.size() > 2) {
                    vertVec.erase(vertVec.begin() + 1, vertVec.begin() + vertVec.size() - 1);
                    spriteVec.erase(spriteVec.begin() + 1, spriteVec.begin() + spriteVec.size() - 1);
                    xCoord.erase(xCoord.begin() + 1, xCoord.begin() + xCoord.size() - 1);
                    yCoord.erase(yCoord.begin() + 1, yCoord.begin() + yCoord.size() - 1);
                }
                reset = false;
            }
            // If my sprite goes out of bounds
            else if (outOfBounds && event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                dragging = false;
                sf::Vector2f relativePos = spriteVec[spriteCoord].getPosition();
                erase();
                outOfBounds = false;
                vertexSort(relativePos);
            }
            // If my sprite is new or is within the bounds
            else if ((event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)) {
                if (dragging) {
                    erase();
                    dragging = false;
                }
                if (sf::Mouse::getPosition(window).x < 260 && sf::Mouse::getPosition(window).y < 260) {
                    // for new sprite
                    sf::Vector2f relativePos = { float(sf::Mouse::getPosition(window).x), float(sf::Mouse::getPosition(window).y) };
                    outOfBounds = false;
                    vertexSort(relativePos);
                }
            }
            // Sprite remove condition
            else if ((event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right)) {
                sf::Vector2i mpos = sf::Mouse::getPosition(window);
                if (spriteVec[spriteCoord].getGlobalBounds().contains(mpos.x, mpos.y)) {
                    spriteBoundary(mpos);
                    erase();
                    cout << endl << spriteVec.size();
                }
                dragging = false;
            }
            else if (event.type == sf::Event::MouseButtonPressed) { 
                sf::Vector2i mpos = sf::Mouse::getPosition(window);
                for (auto& it : spriteVec) {
                    if (it.getGlobalBounds().contains(mpos.x, mpos.y)) {
                        spriteBoundary(mpos);
                        dragging = true;
                    }
                }
                if (rectVec[2].getGlobalBounds().contains(mpos.x, mpos.y)) {
                    manualOff = true;
                }
                if (rectVec[0].getGlobalBounds().contains(mpos.x, mpos.y)) {
                    reset = true;
                }
                else if (rectVec[1].getGlobalBounds().contains(mpos.x, mpos.y)) {
                    manualControl = true;
                }
                if (rectVec[3].getGlobalBounds().contains(mpos.x, mpos.y)) {
                    save = true;
                }
                else if (rectVec[4].getGlobalBounds().contains(mpos.x, mpos.y)) {
                    load = true;
                }
                if (manualControl) {
                    for (int i = 0; i < buttVec.size(); i++) {
                        if (buttVec[i].getGlobalBounds().contains(mpos.x, mpos.y)) {
                            buttonIndex = i;
                            buttonClick = true;
                            break;
                        }
                    }
                }
            }
            else if (event.type == sf::Event::MouseMoved) {
                // dragging condition
                if (dragging) {
                    // Bound check
                    if ((spriteVec[spriteCoord - 1].getPosition().x + 4 >= sf::Mouse::getPosition(window).x) || (float(spriteVec[spriteCoord + 1].getPosition().x + 4) <= sf::Mouse::getPosition(window).x) || (spriteVec[spriteCoord].getPosition().y + 4 > 256) || (spriteVec[spriteCoord].getPosition().y < 0)) {
                        outOfBounds = true;
                    }
                    else {
                        spriteVec[spriteCoord].setPosition(float(event.mouseMove.x), float(event.mouseMove.y));
                        
                        //spriteVec.erase(spriteVec.begin() + spriteCoord);
                        //spriteVec.insert(spriteVec.begin() + spriteCoord, );
                        xCoord.erase(xCoord.begin() + spriteCoord);
                        xCoord.insert(xCoord.begin() + spriteCoord, double(event.mouseMove.x));
                        yCoord.erase(yCoord.begin() + spriteCoord);
                        yCoord.insert(yCoord.begin() + spriteCoord, double(event.mouseMove.y));
                    }
                }
            }
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear();
        window.draw(toggleOff);
        window.draw(toggleOn);
        window.draw(toggleBackOn);
        window.draw(toggleSave);
        window.draw(toggleLoad);
        sf::CircleShape buttonUp(20, 3);
        buttonUp.setPosition(103, 700);

        sf::CircleShape buttonDown(20, 3);
        buttonDown.rotate(180.0f);
        buttonDown.setPosition(143, 800);

        sf::CircleShape buttonLeft(20, 3);
        buttonLeft.rotate(270.0f);
        buttonLeft.setPosition(73, 770);

        sf::CircleShape buttonRight(20, 3);
        buttonRight.rotate(90.0f);
        buttonRight.setPosition(173, 730);
        window.draw(buttonUp);
        window.draw(buttonDown);
        window.draw(buttonLeft);
        window.draw(buttonRight);

        // Initializes the graph
        sf::Vertex y[] =
        {
            sf::Vertex(sf::Vector2f(1, 0)),
            sf::Vertex(sf::Vector2f(1, 255))
        };
        window.draw(y, 2, sf::Lines);
        sf::Vertex x[] =
        {
            sf::Vertex(sf::Vector2f(-5, 256)),
            sf::Vertex(sf::Vector2f(255, 256))
        };
        window.draw(x, 2, sf::Lines);
        if (spriteVec.size() == 2) {
            sf::Vertex line[] =
            {
                sf::Vertex(sf::Vector2f(spriteVec[0].getPosition().x, spriteVec[0].getPosition().y)),
                sf::Vertex(sf::Vector2f(spriteVec[1].getPosition().x, spriteVec[1].getPosition().y))
            };
            window.draw(line, 2, sf::Lines);
            window.draw(spriteVec[spriteVec.size() - 1]);
        }
        // Cubic interpolation graph
        else if (spriteVec.size() > 2) {
            splineMake();
            for (size_t i = 0; i < xyMap.size() - 1; i++) {
                sf::Vertex line[] =
                {
                    sf::Vertex(sf::Vector2f(i, xyMap[i])),
                    sf::Vertex(sf::Vector2f(i + 1, xyMap[i + 1]))
                };
                sf::Vertex vertex(sf::Vector2f(i, xyMap[i]));
                window.draw(&vertex, 1, sf::Points);
                window.draw(line, 2, sf::Lines);
            }
            window.draw(spriteVec[spriteVec.size() - 1]);
        }
        // Sprite creation
        for (size_t i = 0; i < spriteVec.size() - 1; i++) {
            window.draw(spriteVec[i]);
        }
        // Canvas
        /*for (int i = 0; i < xPixel * yPixel * 4; i += 4) {
            &sth[i];
        }*/
        for (int i = 0; i < xPixel; i++) {
            for (int j = 0; j < yPixel; j++) {
                sf::Color c = stationary.getPixel(i, j);
                sf::Color color;
                // Initializes to this
                if (vertVec.size() == 2) {
                    color = sf::Color(sf::Uint8(c.r), sf::Uint8(c.g), sf::Uint8(c.b), 255);
                }
                // Colors go through 255 - x because the entire window is flipped upside down
                else {
                    color = sf::Color(255 - sf::Uint8(xyMap[c.r]), 255 - sf::Uint8(xyMap[c.g]), 255 - sf::Uint8(xyMap[c.b]), 255);
                }
                mappingImg.setPixel(i, j, color);
            }
        }
        textureImage.loadFromImage(mappingImg);
        pic.setTexture(textureImage);
        pic.setPosition(float(255), float(255));
        window.draw(pic);
        window.display();
    }
    return 0;
}