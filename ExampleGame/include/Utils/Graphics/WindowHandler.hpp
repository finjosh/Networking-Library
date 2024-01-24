#ifndef WINDOWHANDLER_H
#define WINDOWHANDLER_H

#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

class WindowHandler
{
public:

    static sf::RenderWindow* getRenderWindow();
    /// @warning does NOT clear memory from last window if already set
    static void setRenderWindow(sf::RenderWindow* renderWindow);

private:
    inline WindowHandler() = default;
    
    static sf::RenderWindow* _renderWindow;
};

#endif
