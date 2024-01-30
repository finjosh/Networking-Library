#ifndef DRAWABLEMANAGER_H
#define DRAWABLEMANAGER_H

#pragma once

#include <set>

#include "SFML/Graphics/RenderWindow.hpp"

#include "DrawableObject.hpp"

class DrawableObject;

struct _drawableComp
{
    bool operator() (const DrawableObject* lhs, const DrawableObject* rhs) const;
};

class DrawableManager
{
public:
    /// @brief draws all drawable objs
    static void draw(sf::RenderWindow& window);

protected:
    static void addDrawable(DrawableObject* DrawableObject);
    static void removeDrawable(DrawableObject* DrawableObject);

    friend DrawableObject;

private:
    inline DrawableManager() = default;

    /// @note first is the layer
    /// @note second is the obj
    static std::set<DrawableObject*, _drawableComp> _drawables;
};

#endif
