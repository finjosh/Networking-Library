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

bool runExample(tgui::BackendGui& gui)
{
    try
    {
        // tgui::Theme theme{"themes/CustomBlack.txt"};

        // gui.add(tgui::Picture::create("RedBackground.jpg"));

        auto tabs = tgui::Tabs::create();
        // tabs->setRenderer(theme.getRenderer("Tabs"));
        tabs->setTabHeight(30);
        tabs->setPosition(70, 40);
        tabs->add("Tab - 1");
        tabs->add("Tab - 2");
        tabs->add("Tab - 3");
        gui.add(tabs);

        auto menu = tgui::MenuBar::create();
        // menu->setRenderer(theme.getRenderer("MenuBar"));
        menu->setHeight(22.f);
        menu->addMenu("File");
        menu->addMenuItem("Load");
        menu->addMenuItem("Save");
        menu->addMenuItem("Exit");
        menu->addMenu("Edit");
        menu->addMenuItem("Copy");
        menu->addMenuItem("Paste");
        menu->addMenu("Help");
        menu->addMenuItem("About");
        gui.add(menu);

        auto label = tgui::Label::create();
        // label->setRenderer(theme.getRenderer("Label"));
        label->setText("This is a label.\nAnd these are radio buttons:");
        label->setPosition(10, 90);
        label->setTextSize(18);
        gui.add(label);

        auto radioButton = tgui::RadioButton::create();
        // radioButton->setRenderer(theme.getRenderer("RadioButton"));
        radioButton->setPosition(20, 140);
        radioButton->setText("Yep!");
        radioButton->setSize(25, 25);
        gui.add(radioButton);

        radioButton = tgui::RadioButton::create();
        // radioButton->setRenderer(theme.getRenderer("RadioButton"));
        radioButton->setPosition(20, 170);
        radioButton->setText("Nope!");
        radioButton->setSize(25, 25);
        gui.add(radioButton);

        radioButton = tgui::RadioButton::create();
        // radioButton->setRenderer(theme.getRenderer("RadioButton"));
        radioButton->setPosition(20, 200);
        radioButton->setText("Don't know!");
        radioButton->setSize(25, 25);
        gui.add(radioButton);

        label = tgui::Label::create();
        // label->setRenderer(theme.getRenderer("Label"));
        label->setText("We've got some edit boxes:");
        label->setPosition(10, 240);
        label->setTextSize(18);
        gui.add(label);

        auto editBox = tgui::EditBox::create();
        // editBox->setRenderer(theme.getRenderer("EditBox"));
        editBox->setSize(200, 25);
        editBox->setTextSize(18);
        editBox->setPosition(10, 270);
        editBox->setDefaultText("Click to edit text...");
        gui.add(editBox);

        label = tgui::Label::create();
        // label->setRenderer(theme.getRenderer("Label"));
        label->setText("And some list boxes too...");
        label->setPosition(10, 310);
        label->setTextSize(18);
        gui.add(label);

        auto listBox = tgui::ListBox::create();
        // listBox->setRenderer(theme.getRenderer("ListBox"));
        listBox->setSize(250, 120);
        listBox->setItemHeight(24);
        listBox->setPosition(10, 340);
        listBox->addItem("Item 1");
        listBox->addItem("Item 2");
        listBox->addItem("Item 3");
        gui.add(listBox);

        label = tgui::Label::create();
        // label->setRenderer(theme.getRenderer("Label"));
        label->setText("It's the progress bar below");
        label->setPosition(10, 470);
        label->setTextSize(18);
        gui.add(label);

        auto progressBar = tgui::ProgressBar::create();
        // progressBar->setRenderer(theme.getRenderer("ProgressBar"));
        progressBar->setPosition(10, 500);
        progressBar->setSize(200, 20);
        progressBar->setValue(50);
        gui.add(progressBar);

        label = tgui::Label::create();
        // label->setRenderer(theme.getRenderer("Label"));
        label->setText(tgui::String::fromNumber(progressBar->getValue()) + "%");
        label->setPosition(220, 500);
        label->setTextSize(18);
        gui.add(label);

        label = tgui::Label::create();
        // label->setRenderer(theme.getRenderer("Label"));
        label->setText("That's the slider");
        label->setPosition(10, 530);
        label->setTextSize(18);
        gui.add(label);

        auto slider = tgui::Slider::create();
        // slider->setRenderer(theme.getRenderer("Slider"));
        slider->setPosition(10, 560);
        slider->setSize(200, 18);
        slider->setValue(4);
        gui.add(slider);

        auto scrollbar = tgui::Scrollbar::create();
        // scrollbar->setRenderer(theme.getRenderer("Scrollbar"));
        scrollbar->setPosition(380, 40);
        scrollbar->setSize(18, 540);
        scrollbar->setMaximum(100);
        scrollbar->setViewportSize(70);
        gui.add(scrollbar);

        auto comboBox = tgui::ComboBox::create();
        // comboBox->setRenderer(theme.getRenderer("ComboBox"));
        comboBox->setSize(120, 21);
        comboBox->setPosition(420, 40);
        comboBox->addItem("Item 1");
        comboBox->addItem("Item 2");
        comboBox->addItem("Item 3");
        comboBox->setSelectedItem("Item 2");
        gui.add(comboBox);

        auto child = tgui::ChildWindow::create();
        // child->setRenderer(theme.getRenderer("ChildWindow"));
        child->setClientSize({250, 120});
        child->setPosition(420, 80);
        child->setTitle("Child window");
        gui.add(child);

        label = tgui::Label::create();
        // label->setRenderer(theme.getRenderer("Label"));
        label->setText("Hi! I'm a child window.");
        label->setPosition(30, 30);
        label->setTextSize(15);
        child->add(label);

        auto button = tgui::Button::create();
        // button->setRenderer(theme.getRenderer("Button"));
        button->setPosition(75, 70);
        button->setText("OK");
        button->setSize(100, 30);
        button->onPress([=]{ child->setVisible(false); });
        child->add(button);

        auto checkbox = tgui::CheckBox::create();
        // checkbox->setRenderer(theme.getRenderer("CheckBox"));
        checkbox->setPosition(420, 240);
        checkbox->setText("Ok, I got it");
        checkbox->setSize(25, 25);
        gui.add(checkbox);

        checkbox = tgui::CheckBox::create();
        // checkbox->setRenderer(theme.getRenderer("CheckBox"));
        checkbox->setPosition(570, 240);
        checkbox->setText("No, I didn't");
        checkbox->setSize(25, 25);
        gui.add(checkbox);

        label = tgui::Label::create();
        // label->setRenderer(theme.getRenderer("Label"));
        label->setText("Chatbox");
        label->setPosition(420, 280);
        label->setTextSize(18);
        gui.add(label);

        auto chatbox = tgui::ChatBox::create();
        // chatbox->setRenderer(theme.getRenderer("ChatBox"));
        chatbox->setSize(300, 100);
        chatbox->setTextSize(18);
        chatbox->setPosition(420, 310);
        chatbox->setLinesStartFromTop();
        chatbox->addLine("texus: Hey, this is TGUI!", tgui::Color::Green);
        chatbox->addLine("Me: Looks awesome! ;)", tgui::Color::Yellow);
        chatbox->addLine("texus: Thanks! :)", tgui::Color::Green);
        chatbox->addLine("Me: The widgets rock ^^", tgui::Color::Yellow);
        gui.add(chatbox);

        button = tgui::Button::create();
        // button->setRenderer(theme.getRenderer("Button"));
        button->setPosition(gui.getView().getSize().x - 115.f, gui.getView().getSize().y - 50.f);
        button->setText("Exit");
        button->setSize(100, 40);
        gui.add(button);
    }
    catch (const tgui::Exception& e)
    {
        std::cerr << "TGUI Exception: " << e.what() << std::endl;
        return false;
    }

    return true;
}

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
    tgui::Theme::setDefault("themes/CustomBlack.txt");
    runExample(gui);
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
    
    TFuncDisplay::setVisible();
    CommandPrompt::setVisible();
    VarDisplay::setVisible();

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

        auto temp = sf::RectangleShape({10000,10000});
        temp.setFillColor(sf::Color(50,0,50));
        window.draw(temp);

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
