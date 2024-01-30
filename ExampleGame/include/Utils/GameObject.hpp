#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#pragma once

#include "Utils/UpdateInterface.hpp"
#include "Utils/Graphics/DrawableObject.hpp"

class GameObject : virtual public UpdateInterface, virtual public DrawableObject {};

#endif
