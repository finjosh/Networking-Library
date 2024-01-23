#include "Utils/Graphics/WindowHandler.hpp"

sf::RenderWindow* WindowHandler::_renderWindow = nullptr;

sf::RenderWindow* WindowHandler::getRenderWindow()
{
    return _renderWindow;
}

void WindowHandler::setRenderWindow(sf::RenderWindow* renderWindow)
{
    _renderWindow = renderWindow;
}
