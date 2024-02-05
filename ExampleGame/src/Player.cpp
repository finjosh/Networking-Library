#include "Player.hpp"

Player::Player(const float& x, const float& y, const bool& handleInput, const int& layer) : DrawableObject(layer), _handleInput(handleInput)
{
    b2PolygonShape b2shape;
    b2shape.SetAsBox(10/PIXELS_PER_METER/2.0, 10/PIXELS_PER_METER/2.0);
    b2FixtureDef fixtureDef;
    fixtureDef.density = 1.0;
    fixtureDef.friction = 0.25;
    fixtureDef.shape = &b2shape;

    Collider::initCollider(x/PIXELS_PER_METER,y/PIXELS_PER_METER);
    Collider::createFixture(fixtureDef);
    Collider::getBody()->SetLinearDamping(1);
    Collider::getBody()->SetFixedRotation(true);

    RectangleShape::setSize({10,10});
    RectangleShape::setOrigin(5,5);
    RectangleShape::setPosition(x,y);
}

Player::~Player()
{

}

void Player::Draw(sf::RenderWindow& window)
{
    RectangleShape::setPosition({this->getBody()->GetPosition().x*PIXELS_PER_METER, this->getBody()->GetPosition().y*PIXELS_PER_METER});
    window.draw(*this);
}

void Player::throwBall() const
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(*WindowHandler::getRenderWindow());
    new Ball(this->getBody()->GetPosition(), this->getBody()->GetLocalPoint({mousePos.x/PIXELS_PER_METER, mousePos.y/PIXELS_PER_METER}), 100);
}

bool Player::getHandleInput() const
{
    return _handleInput;
}

void Player::setHandleInput(const bool& handle)
{
    _handleInput = handle;
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

    float currentLength = this->getBody()->GetLinearVelocity().Length();
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
        this->getBody()->ApplyForceToCenter({vel.x*450, vel.y*450}, true);
    }
    this->getBody()->SetLinearVelocity(vel);
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

void Player::BeginContact(CollisionData collisionData) 
{
    if (collisionData.getCollider()->cast<Ball>() == nullptr)
        return;

    Command::Prompt::print("Player contact with ball begin");
}

void Player::EndContact(CollisionData collisionData) 
{
    if (collisionData.getCollider()->cast<Ball>() == nullptr)
        return;
    
    Command::Prompt::print("Player contact with ball end");
}
