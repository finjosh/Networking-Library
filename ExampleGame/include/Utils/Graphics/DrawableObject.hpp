#ifndef DRAWABLE_H
#define DRAWABLE_H

#pragma once

#include "SFML/Graphics/RenderWindow.hpp"

#include "Utils/Object.hpp"

class DrawableObject : public virtual Object
{
public:
    DrawableObject(const int& layer = 0);
    ~DrawableObject();

    /// @brief changes the order in which this obj will be drawn
    /// @param layer smaller is earlier and larger is later
    void setLayer(const int& layer);
    int getLayer() const;

    /// @brief should be used to draw the obj
    virtual void Draw(sf::RenderWindow& window) = 0;

protected:

private:
    int _layer = 0;
};

#endif
