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

// Network files
#include "Utils/Networking/NetworkObject.hpp"
#include "Utils/Networking/ObjectType.hpp"
#include "Utils/Networking/NetworkObjectManager.hpp"

#include "Player.hpp"
#include "Utils/Physics/CollisionManager.hpp"

using namespace std;
using namespace sf;

void addThemeCommands();

// TODO setup a view manager that handles windows size changes
int main()
{
    initObjectTypeConstructor(typeid(Player).hash_code(), {[](){ return new Player(7, 7, false); }});
    initObjectTypeConstructor(typeid(Ball).hash_code(), {[](){ return new Ball({0,0},{0,0},0); }});

    // setup for sfml and tgui
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Example Game");
    window.setFramerateLimit(144);
    WindowHandler::setRenderWindow(&window);

    tgui::Gui gui{window};
    gui.setRelativeView({0, 0, 1920/(float)window.getSize().x, 1080/(float)window.getSize().y});
    tgui::Theme::setDefault("themes/Dark.txt");
    Command::color::setDefaultColor({255,255,255,255});
    // -----------------------

    WorldHandler::getWorld().SetGravity({0.f,0.f});
    WorldHandler::getWorld().SetContactListener(new CollisionManager);

    udp::SocketUI sDisplay(50001, {}, {});
    sDisplay.initConnectionDisplay(gui);
    // sDisplay.closeConnectionDisplay();
    // sDisplay.setConnectionVisible();
    NetworkObjectManager::init(sDisplay.getServer(), sDisplay.getClient(), {[](){ sf::Packet temp;
        temp << "data maybe";
        return temp;
    }});
    NetworkObjectManager::onDataReceived([](sf::Packet* data){
        std::string temp;
        *data >> temp;
        Command::Prompt::print(temp);
    });

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
        NetworkObjectManager::lock();
        EventHelper::Event::ThreadSafe::update();
        NetworkObjectManager::unlock();
        window.clear();
        // updating the delta time var
        sf::Time deltaTime = deltaClock.restart();
        fixedUpdate += deltaTime.asSeconds();
        WorldHandler::updateTime(deltaTime.asSeconds());
        sf::Event event;
        while (window.pollEvent(event))
        {
            gui.handleEvent(event);

            //! Required for LiveVar and CommandPrompt to work as intended
            NetworkObjectManager::lock();
            LiveVar::UpdateLiveVars(event);
            NetworkObjectManager::unlock();
            NetworkObjectManager::lock();
            Command::Prompt::UpdateEvent(event);
            NetworkObjectManager::unlock();
            //! ----------------------------------------------------------

            if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape)
                window.close();

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
        NetworkObjectManager::lock();
        UpdateManager::Update(deltaTime.asSeconds());
        NetworkObjectManager::unlock();
        if (fixedUpdate >= 0.2)
        {
            fixedUpdate = 0;
            NetworkObjectManager::lock();
            UpdateManager::FixedUpdate();
            NetworkObjectManager::unlock();
        }
        NetworkObjectManager::lock();
        UpdateManager::LateUpdate(deltaTime.asSeconds());
        NetworkObjectManager::unlock();
        //! Updates all the vars being displayed
        NetworkObjectManager::lock();
        VarDisplay::Update();
        NetworkObjectManager::unlock();
        //! ------------------------------=-----
        //! Updates all Terminating Functions
        NetworkObjectManager::lock();
        TerminatingFunction::UpdateFunctions(deltaTime.asSeconds());
        NetworkObjectManager::unlock();
        //* Updates for the terminating functions display
        TFuncDisplay::update();
        //! ------------------------------

        auto temp = sf::RectangleShape({10000,10000});
        temp.setFillColor(sf::Color(50,0,50));
        window.draw(temp);

        sDisplay.updateInfoDisplay();

        //! Do physics before this
        NetworkObjectManager::lock();
        WorldHandler::updateWorld();
        NetworkObjectManager::unlock();
        //! Draw after this

        //* Write code here

        // ---------------

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