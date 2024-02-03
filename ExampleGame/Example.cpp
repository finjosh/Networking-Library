#include <iostream>

#include "SFML/Graphics.hpp"

#include "TGUI/TGUI.hpp"
#include "TGUI/Backend/SFML-Graphics.hpp"

#include "box2d/Box2D.h"

#include "Utils/Utils.hpp"
#include "Networking/SocketUI.hpp"
#include "Networking/Client.hpp"
#include "Networking/Server.hpp"
#include "Utils/Graphics/WindowHandler.hpp"
#include "Utils/Physics/WorldHandler.hpp"

#include "Utils/UpdateManager.hpp"
#include "Utils/Graphics/DrawableManager.hpp"
#include "Utils/ObjectManager.hpp"

#include "Player.hpp"
#include "Utils/Physics/CollisionManager.hpp"

using namespace std;
using namespace sf;

void addThemeCommands();

// TODO make a physics body class (will stop physics when the object is disabled)
// TODO setup a view manager that handles windows size changes
int main()
{
    // setup for sfml and tgui
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Example Game");
    window.setFramerateLimit(144);
    window.setPosition(Vector2i(-8, -8));
    WindowHandler::setRenderWindow(&window);

    tgui::Gui gui{window};
    gui.setRelativeView({0, 0, 1920/(float)window.getSize().x, 1080/(float)window.getSize().y});
    tgui::Theme::setDefault("themes/Dark.txt");
    Command::color::setDefaultColor({255,255,255,255});
    // -----------------------

    WorldHandler::getWorld().SetGravity({0.f,0.f});
    WorldHandler::getWorld().SetContactListener(new CollisionListener);

    udp::SocketUI sDisplay(50001, {[&sDisplay](){ sDisplay.getServer()->sendToAll(udp::Socket::DataPacketTemplate() << "Some Data"); }}, {[&sDisplay](){ sDisplay.getClient()->sendToServer(udp::Socket::DataPacketTemplate() << "Some Data"); }});
    sDisplay.initConnectionDisplay(gui);
    sDisplay.closeConnectionDisplay();
    // sDisplay.setConnectionVisible();

    //! Required to initialize VarDisplay and CommandPrompt
    // creates the UI for the VarDisplay
    VarDisplay::init(gui); 
    // creates the UI for the CommandPrompt
    Command::Prompt::init(gui);
    addThemeCommands();
    // create the UI for the TFuncDisplay
    TFuncDisplay::init(gui);

    //! ---------------------------------------------------

    std::list<unsigned long long> ids;

    new Player(100,100);

    UpdateManager::Start();
    sf::Clock deltaClock;
    float fixedUpdate = 0;
    while (window.isOpen())
    {
        EventHelper::Event::ThreadSafe::update();
        window.clear();
        // updating the delta time var
        sf::Time deltaTime = deltaClock.restart();
        fixedUpdate += deltaTime.asSeconds();
        sf::Event event;
        while (window.pollEvent(event))
        {
            gui.handleEvent(event);

            //! Required for LiveVar and CommandPrompt to work as intended
            LiveVar::UpdateLiveVars(event);
            Command::Prompt::UpdateEvent(event);
            //! ----------------------------------------------------------

            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Z)
            {
                if (ids.size() > 0)
                {
                    Object::Ptr temp = ObjectManager::getObject(ids.front());
                    temp->setEnabled(!temp->isEnabled());
                }
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::X)
            {
                if (ids.size() > 0)
                {
                    Object::Ptr temp = ObjectManager::getObject(ids.front());
                    Collider* collider = temp->cast<Collider>();
                    if (collider != nullptr)
                    {
                        collider->setPhysicsEnabled(!collider->isPhysicsEnabled());
                    }
                }
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up)
            {
                Player* temp = new Player(rand()%window.getSize().x, rand()%window.getSize().y, false);
                ids.push_back(temp->getID());
            }
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down)
            {
                if (ids.size() > 0)
                {
                    Object::Ptr obj(ObjectManager::getObject(ids.front()));
                    if (obj.isValid())
                    {
                        obj->destroy();
                    }
                    ids.erase(ids.begin());
                }
            }
        }
        UpdateManager::Update(deltaTime.asSeconds());
        if (fixedUpdate >= 0.2)
        {
            fixedUpdate = 0;
            UpdateManager::FixedUpdate();
        }
        UpdateManager::LateUpdate(deltaTime.asSeconds());
        //! Updates all the vars being displayed
        VarDisplay::Update();
        //! ------------------------------=-----
        //! Updates all Terminating Functions
        TerminatingFunction::UpdateFunctions(deltaTime.asSeconds());
        //* Updates for the terminating functions display
        TFuncDisplay::update();
        //! ------------------------------

        auto temp = sf::RectangleShape({10000,10000});
        temp.setFillColor(sf::Color(50,0,50));
        window.draw(temp);

        sDisplay.updateInfoDisplay();

        //! Do physics before this
        WorldHandler::getWorld().Step(deltaTime.asSeconds(), int32(8), int32(3));
        //! Draw after this

        DrawableManager::draw(window);

        // draw for tgui
        gui.draw();
        // display for sfml window
        window.display();
    }

    sDisplay.closeConnectionDisplay();

    //! Required so that VarDisplay and CommandPrompt release all data
    VarDisplay::close();
    Command::Prompt::close();
    TFuncDisplay::close();
    //! --------------------------------------------------------------

    window.close();

    return 0;
}

void addThemeCommands()
{
    Command::Handler::addCommand(Command::command{"setTheme", "Function used to set the theme of the UI (The previous outputs in the command prompt will not get updated color)", 
        {Command::print, "Trying calling one of the sub commands"},
        std::list<Command::command>{
            Command::command{"default", "(Currently does not work, coming soon) Sets the theme back to default", {[](){ 
                tgui::Theme::setDefault(""); //! This does not work due to a tgui issue
                // Note that command color does not update with theme so you have to set the default color
                Command::color::setDefaultColor({0,0,0,255}); // black
            }}},
            // Dark theme is a custom theme made by me 
            // It can be found here: https://github.com/finjosh/TGUI-DarkTheme
            Command::command{"dark", "Sets the them to the dark theme", {[](){ 
                tgui::Theme::getDefault()->load("themes/Dark.txt"); 
                // Note that command color does not update with theme so you have to set the default color
                Command::color::setDefaultColor({255,255,255,255}); // white
            }}}, 
            Command::command{"black", "Sets the them to the black theme", {[](){ 
                tgui::Theme::getDefault()->load("themes/Black.txt"); 
                // Note that command color does not update with theme so you have to set the default color
                Command::color::setDefaultColor({255,255,255,255}); // white
            }}},
            Command::command{"grey", "Sets the them to the transparent grey theme", {[](){ 
                tgui::Theme::getDefault()->load("themes/TransparentGrey.txt"); 
                // Note that command color does not update with theme so you have to set the default color
                Command::color::setDefaultColor({0,0,0,255}); // black
            }}}
        }
    });
}