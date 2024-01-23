#ifndef TFUNCDISPLAY_H
#define TFUNCDISPLAY_H

#pragma once

#include "Utils/TerminatingFunction.hpp"

#include "TGUI/Backend/SFML-Graphics.hpp"
#include "TGUI/Widgets/ChildWindow.hpp"
#include "TGUI/Widgets/ListView.hpp"

#include "SFML/Graphics/RenderWindow.hpp"

#if __has_include("CommandHandler.hpp")
#include "CommandHandler.hpp" 
#endif

class TFuncDisplay
{
public:
    /// @brief creates all of the UI needed for the display to work
    /// @param gui used to initalized the UI for the display
    static void init(tgui::Gui& gui);
    /// @brief removes all of the pointers from storage so the program can close without a crash
    static void close();

    /// @brief Call this every frame to update the terminating functions being shown
    static void update();

    /// @brief closes and opens the window
    static void setVisible(bool visible = true);
    static bool isVisible();

    /// @brief only shows the top bar when minimized
    static void minimize();
    static void maximize();
    static bool isMinimized();

protected:
    /// @brief used for tgui backend
    static void _close(bool* abortTguiClose);

private:
    inline TFuncDisplay() = default;

    static float _parentHeight;
    static tgui::ChildWindow::Ptr _parent;
    static tgui::ListView::Ptr _list;
};

#endif
