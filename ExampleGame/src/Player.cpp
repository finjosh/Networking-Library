#include "Player.hpp"

Player::Player(const float& x, const float& y, const bool& handleInput, const int& layer) : DrawableObject(layer), _handleInput(handleInput)
{
    b2BodyDef bodyDef;
    bodyDef.position.Set(x/PIXELS_PER_METER, y/PIXELS_PER_METER);
    bodyDef.type = b2_dynamicBody;
    bodyDef.linearDamping = 1;
    bodyDef.fixedRotation = true;
    b2PolygonShape b2shape;
    b2shape.SetAsBox(10/PIXELS_PER_METER/2.0, 10/PIXELS_PER_METER/2.0);
    b2FixtureDef fixtureDef;
    fixtureDef.density = 1.0;
    fixtureDef.friction = 0.1;
    fixtureDef.restitution = 0.1;
    fixtureDef.shape = &b2shape;

    _body = WorldHandler::getWorld().CreateBody(&bodyDef);
    _body->CreateFixture(&fixtureDef);
    //! TESTING
    CollisionCallbacks::setBody(_body);
    //! -------

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

    handleInput();
    
    b2Vec2 vel(0,0);

    if (_state.up){
        vel.y -= 10;
    }

    if (_state.down){
        vel.y += 10;
    }

    if (_state.right){
        vel.x += 10;
    }

    if (_state.left){
        vel.x -= 10;
    }

    if (_state.shoot && _shootCooldown >= 0.25)
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
    if (_burstCooldown >= 1.5 && _state.burst)
    {
        _burstCooldown = 0;
        _body->ApplyForceToCenter({vel.x*450, vel.y*450}, true);
    }
    _body->SetLinearVelocity(vel);
}

void Player::destroy()
{
    delete(this);
}

void Player::handleInput()
{
    if (_handleInput && WindowHandler::getRenderWindow()->hasFocus())
    {
        _state.reset();

        if (sf::Keyboard::isKeyPressed((sf::Keyboard::W))){
            _state.up = true;
        }

        if (sf::Keyboard::isKeyPressed((sf::Keyboard::S))){
            _state.down = true;
        }

        if (sf::Keyboard::isKeyPressed((sf::Keyboard::D))){
            _state.right = true;
        }

        if (sf::Keyboard::isKeyPressed((sf::Keyboard::A))){
            _state.left = true;
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            _state.shoot = true;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            _state.burst = true;
        }
    }
}

#include "Utils/Debug/CommandPrompt.hpp"

void Player::BeginContact(b2Contact* contact) 
{
    if (static_cast<CollisionCallbacks*>((void*)contact->GetFixtureB()->GetBody()->GetUserData().pointer)->cast<Ball>() == nullptr)
        return;
    
    Command::Prompt::print("Player contact with ball begin");
}

void Player::EndContact(b2Contact* contact) 
{
    if (static_cast<CollisionCallbacks*>((void*)contact->GetFixtureB()->GetBody()->GetUserData().pointer)->cast<Ball>() == nullptr)
        return;
    
    Command::Prompt::print("Player contact with ball end");
}
