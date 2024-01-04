#ifndef VARDISPLAY_H
#define VARDISPLAY_H

#pragma once

#include <map>
#include <algorithm>

#include "LiveVar.hpp" 

#if __has_include("CommandHandler.hpp")
#include "CommandHandler.hpp" 
#endif

#include "TGUI/Backend/SFML-Graphics.hpp"
#include "TGUI/Widgets/RichTextLabel.hpp"
#include "TGUI/Widgets/ScrollablePanel.hpp"
#include "TGUI/Widgets/ChildWindow.hpp"

#include "SFML/Graphics/RenderWindow.hpp"

//! Edit this to change the alternating color of the vars
const std::string ALT_COLOR = "#999999";

/**
 * @brief auto adds any var that is made in live var and auto updates when that var is changed
*/
class VarDisplay
{
public:
    /// @brief creates all of the UI needed for the display to work
    /// @param gui used to initalized the UI for the display
    static void init(tgui::Gui& gui);
    /// @brief removes all of the pointers from storage so the program can close without a crash
    static void close();

    /// @brief updates the live vars
    /// @note call this every frame
    static void Update();

    static void setVisible(bool visible = true);
    /// @brief only shows the displays title bar
    static void minimizeWindow();
    /// @brief shows the entire display (including the variables)
    static void maximizeWindow();

protected:
    /// @brief adds a var to the list
    /// @note used with the events from live vars
    static void addVar(const std::string& name, const float& value);
    /// @brief sets the value of a var 
    /// @note used with the events from live vars
    static void setVar(const std::string& name, const float& value);
    /// @brief removes a var from the list
    /// @note used with the events from live vars
    static void removeVar(const std::string& name);
    
    /// @brief used for tgui backend
    static void _closeWindow(bool* abortTguiClose);

private:
    VarDisplay() = default;

    static std::map<std::string, float> _vars;
    static bool _varChanged;

    static tgui::ChildWindow::Ptr _parent;
    /// @brief the height of the parent before being minimized
    static float _parentHeight;
    static tgui::RichTextLabel::Ptr _varLabel;
    static tgui::ScrollablePanel::Ptr _scrollPanel;
};

#endif
