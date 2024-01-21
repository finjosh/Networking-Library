#include <iostream>

#include "SFML/Graphics.hpp"

#include "TGUI/TGUI.hpp"
#include "TGUI/Backend/SFML-Graphics.hpp"

#include "include/Utils/Utils.hpp"
#include "include/Networking/SocketUI.hpp"
#include "include/Networking/Client.hpp"
#include "include/Networking/Server.hpp"

using namespace std;
using namespace sf;

void addThemeCommands();

int main()
{
    // setup for sfml and tgui
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Networking Library");
    window.setFramerateLimit(144);
    window.setPosition(Vector2i(-8, -8));

    tgui::Gui gui{window};
    gui.setRelativeView({0, 0, 1920/(float)window.getSize().x, 1080/(float)window.getSize().y});
    tgui::Theme::setDefault("themes/Dark.txt");
    Command::color::setDefaultColor({255,255,255,255});
    // -----------------------

    SocketUI sDisplay(50001, {[&sDisplay](){ sDisplay.getServer()->sendToAll(SocketPlus::DataPacketTemplate() << "Some Data"); }}, {[&sDisplay](){ sDisplay.getClient()->sendToServer(SocketPlus::DataPacketTemplate() << "Some Data"); }});
    // sDisplay.init(gui);
    // sDisplay.setSocket(server);
    sDisplay.initConnectionDisplay(gui);
    sDisplay.setInfoVisible();
    sDisplay.initInfoDisplay(gui);
    sDisplay.setConnectionVisible();
    sDisplay.getServer()->onDataReceived([](sf::Packet packet){ 
        std::string temp;
        packet >> temp;
        Command::Prompt::print(temp); 
    });
    sDisplay.getClient()->onDataReceived([](sf::Packet packet){ 
        std::string temp;
        packet >> temp;
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
    
    TFuncDisplay::setVisible();
    Command::Prompt::setVisible();

    //! ---------------------------------------------------
    
    sf::Clock deltaClock;
    while (window.isOpen())
    {
        EventHelper::Event::ThreadSafe::update();
        window.clear();
        // updating the delta time var
        sf::Time deltaTime = deltaClock.restart();
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

            //! TEST for sending packets
            // if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P)
            // {
            //     if (!sDisplay.isEmpty() && sDisplay.isServer())
            //     {
            //         auto temp = SocketPlus::DataPacketTemplate();
            //         temp << "Some Other Data";
            //         sDisplay.getServer()->SendToAll(temp);
            //     }
            //     else if (!sDisplay.isEmpty())
            //     {
            //         auto temp = SocketPlus::DataPacketTemplate();
            //         temp << "Some Other Data";
            //         sDisplay.getClient()->SendToServer(temp);
            //     }
            // } 
        }
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