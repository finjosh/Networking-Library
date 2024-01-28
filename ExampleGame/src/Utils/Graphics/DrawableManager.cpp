#include "Utils/Graphics/DrawableManager.hpp"

bool DrawableComp::operator() (const DrawableObj* lhs, const DrawableObj* rhs) const
{
    return lhs->getLayer() < rhs->getLayer();
}

std::multiset<DrawableObj*, DrawableComp> DrawableManager::_drawables;

void DrawableManager::draw(sf::RenderWindow& window)
{
    for (auto drawable: _drawables)    
    {
        if (drawable->isEnabled())
            drawable->Draw(window);
    }
}

void DrawableManager::addDrawable(DrawableObj* DrawableObj)
{
    _drawables.insert({DrawableObj});
}

void DrawableManager::removeDrawable(DrawableObj* DrawableObj)
{
    _drawables.erase(DrawableObj);
}
