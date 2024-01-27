#include "Utils/Graphics/Drawable.hpp"

DrawableObj::DrawableObj(const int& layer) : _layer(layer)
{
    DrawableManager::addDrawable(this);
}

DrawableObj::~DrawableObj()
{
    DrawableManager::removeDrawable(this);
}

void DrawableObj::setLayer(const int& layer)
{
    DrawableManager::removeDrawable(this);
    _layer = layer;
    DrawableManager::addDrawable(this);
}

int DrawableObj::getLayer() const
{
    return _layer;
}
