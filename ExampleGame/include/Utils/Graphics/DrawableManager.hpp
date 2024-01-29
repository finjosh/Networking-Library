#ifndef DRAWABLEMANAGER_H
#define DRAWABLEMANAGER_H

#pragma once

#include <set>

#include "SFML/Graphics/RenderWindow.hpp"

#include "Drawable.hpp"

class DrawableObj;

struct _drawableComp
{
    bool operator() (const DrawableObj* lhs, const DrawableObj* rhs) const;
};

class DrawableManager
{
public:
    /// @brief draws all drawable objs
    static void draw(sf::RenderWindow& window);

protected:
    static void addDrawable(DrawableObj* DrawableObj);
    static void removeDrawable(DrawableObj* DrawableObj);

    friend DrawableObj;

private:
    inline DrawableManager() = default;

    /// @note first is the layer
    /// @note second is the obj
    static std::set<DrawableObj*, _drawableComp> _drawables;
};

#endif
