#include "Utils/Graphics/DrawableManager.hpp"

bool _drawableComp::operator() (const DrawableObj* lhs, const DrawableObj* rhs) const
{
    if (lhs->getLayer() < rhs->getLayer())
        return true;
    else if (lhs->getLayer() == rhs->getLayer() && lhs->getID() < rhs->getID())
        return true;
    else    
        return false;
}

std::set<DrawableObj*, _drawableComp> DrawableManager::_drawables;

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
