#ifndef DRAWABLE_H
#define DRAWABLE_H

#pragma once

#include "Utils/Object.hpp"

#include "DrawableManager.hpp"

class DrawableManager;

class DrawableObj : public virtual Object
{
public:
    DrawableObj(const int& layer = 0);
    ~DrawableObj();

    /// @brief changes the order in which this obj will be drawn
    /// @param layer smaller is earlier and larger is later
    void setLayer(const int& layer);
    int getLayer() const;

protected:

    /// @brief should be used to draw the obj
    virtual void Draw(sf::RenderWindow& window) = 0;

    friend DrawableManager;

private:
    int _layer = 0;
};

#endif
