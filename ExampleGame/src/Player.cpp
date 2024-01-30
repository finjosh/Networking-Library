#include "Player.hpp"

Player::Player(const float& x, const float& y, const int& layer) : DrawableObject(layer)
{
    b2BodyDef bodyDef;
    bodyDef.position.Set(x/PIXELS_PER_METER, y/PIXELS_PER_METER);
    bodyDef.type = b2_dynamicBody;
    bodyDef.linearDamping = 1;
    b2PolygonShape b2shape;
    b2shape.SetAsBox(10/PIXELS_PER_METER/2.0, 10/PIXELS_PER_METER/2.0);
    b2FixtureDef fixtureDef;
    fixtureDef.density = 1.0;
    fixtureDef.friction = 0.1;
    fixtureDef.restitution = 0.1;
    fixtureDef.shape = &b2shape;

    _body = WorldHandler::getWorld().CreateBody(&bodyDef);
    _body->CreateFixture(&fixtureDef);

    //* not optimal as the body could be destroyed 
    this->onEnabled(b2Body::SetEnabled, this->_body, true);
    this->onDisabled(b2Body::SetEnabled, this->_body, false);

    RectangleShape::setSize({10,10});
    RectangleShape::setOrigin(5,5);
    RectangleShape::setPosition(x,y);
}

Player::~Player()
{
    WorldHandler::getWorld().DestroyBody(_body);
}

void Player::Draw(sf::RenderWindow& window)
{
    RectangleShape::setPosition({_body->GetPosition().x*PIXELS_PER_METER, _body->GetPosition().y*PIXELS_PER_METER});
    window.draw(*this);
}

void Player::throwBall() const
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(*WindowHandler::getRenderWindow());
    new Ball(_body->GetPosition(), _body->GetLocalPoint({mousePos.x/PIXELS_PER_METER, mousePos.y/PIXELS_PER_METER}), 100);
}

void Player::Update(const float& deltaTime)
{
    _burstCooldown += deltaTime;
    _shootCooldown += deltaTime;

    if (WindowHandler::getRenderWindow()->hasFocus())
    {
        b2Vec2 vel(0,0);

        if (sf::Keyboard::isKeyPressed((sf::Keyboard::W))){
            vel.y -= 10;
        }

        if (sf::Keyboard::isKeyPressed((sf::Keyboard::S))){
            vel.y += 10;
        }

        if (sf::Keyboard::isKeyPressed((sf::Keyboard::D))){
            vel.x += 10;
        }

        if (sf::Keyboard::isKeyPressed((sf::Keyboard::A))){
            vel.x -= 10;
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && _shootCooldown >= 0.25)
        {
            _shootCooldown = 0.f;
            this->throwBall();
        }

        float currentLength = _body->GetLinearVelocity().Length();
        vel.Normalize();
        if (currentLength >= 10)
        {
            vel.x *= currentLength;
            vel.y *= currentLength;
        }
        else
        {
            vel.x *= 10;
            vel.y *= 10;
        }
        if (_burstCooldown >= 1.5 && sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            _burstCooldown = 0;
            _body->ApplyForceToCenter({vel.x*450, vel.y*450}, true);
        }
        _body->SetLinearVelocity(vel);
    }
}

void Player::destroy()
{
    delete(this);
}
