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

int main()
{
    //! If you want Terminating functions to be attached to the CommandHandler you need to call the initializer
    TFunc::initCommands();
    //! --------------------------

    // setup for sfml and tgui
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Networking Library");
    window.setFramerateLimit(144);
    window.setPosition(Vector2i(-8, -8));

    tgui::Gui gui{window};
    gui.setRelativeView({0, 0, 1920/(float)window.getSize().x, 1080/(float)window.getSize().y});
    tgui::Theme::setDefault("themes/Black.txt");
    // -----------------------

    Server server(50001);
    server.openServer({});

    SocketUI sDisplay(50001);
    // sDisplay.init(gui);
    // sDisplay.setSocket(server);
    sDisplay.initConnectionDisplay(gui);
    sDisplay.setInfoVisible();
    sDisplay.initInfoDisplay(gui);
    sDisplay.setConnectionVisible();

    //! Required to initialize VarDisplay and CommandPrompt
    // creates the UI for the VarDisplay
    VarDisplay::init(gui); 
    // creates the UI for the CommandPrompt
    CommandPrompt::init(gui);
    // create the UI for the TFuncDisplay
    TFuncDisplay::init(gui);
    //! ---------------------------------------------------
    
    sf::Clock deltaClock;
    while (window.isOpen())
    {
        window.clear();
        // updating the delta time var
        sf::Time deltaTime = deltaClock.restart();
        sf::Event event;
        while (window.pollEvent(event))
        {
            gui.handleEvent(event);

            //! Required for LiveVar and CommandPrompt to work as intended
            LiveVar::UpdateLiveVars(event);
            CommandPrompt::UpdateEvent(event);
            //! ----------------------------------------------------------

            if (event.type == sf::Event::Closed)
                window.close();
        }
        //! Updates all the vars being displayed
        VarDisplay::Update();
        //! ------------------------------=-----
        //! Updates all Terminating Functions
        TerminatingFunction::UpdateFunctions(deltaTime.asSeconds());
        //* Updates for the terminating functions display
        TFuncDisplay::update();
        //! ------------------------------

        sDisplay.update();

        // draw for tgui
        gui.draw();
        // display for sfml window
        window.display();
    }

    sDisplay.closeConnectionDisplay();

    //! Required so that VarDisplay and CommandPrompt release all data
    VarDisplay::close();
    CommandPrompt::close();
    TFuncDisplay::close();
    //! --------------------------------------------------------------

    window.close();

    return 0;
}
