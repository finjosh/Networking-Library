#include "Utils/Graphics/DrawableManager.hpp"

bool _drawableComp::operator() (const DrawableObject* lhs, const DrawableObject* rhs) const
{
    if (lhs->getLayer() < rhs->getLayer() || lhs->getLayer() == rhs->getLayer() && lhs->getID() < rhs->getID())
        return true;
    else    
        return false;
}

std::set<DrawableObject*, _drawableComp> DrawableManager::_drawables;

void DrawableManager::draw(sf::RenderWindow& window)
{
    for (auto drawable: _drawables)    
    {
        if (drawable->isEnabled())
            drawable->Draw(window);
    }
}

void DrawableManager::addDrawable(DrawableObject* DrawableObject)
{
    _drawables.insert({DrawableObject});
}

void DrawableManager::removeDrawable(DrawableObject* DrawableObject)
{
    _drawables.erase(DrawableObject);
}
