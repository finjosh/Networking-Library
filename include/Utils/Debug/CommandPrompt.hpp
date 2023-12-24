#ifndef COMMANDPROMPT_H
#define COMMANDPROMPT_H

#pragma once

#include <list>

#include "TGUI/Backend/SFML-Graphics.hpp"
#include "TGUI/Widgets/EditBox.hpp"
#include "TGUI/Widgets/ChatBox.hpp"
#include "TGUI/Widgets/ListBox.hpp"
#include "TGUI/Widgets/ChildWindow.hpp"

#include "include/Utils/Debug/CommandHandler.hpp"
#include "include/Utils/StringHelper.hpp"

class CommandPrompt
{
public:
    /// @brief initializes the command prompt UI with the given tgui::Gui
    static void init(tgui::Gui& sfmlGui);
    /// @brief used to erase all pointers to UI in tgui so the program can close without a crash
    static void close();

    /// @brief used to update the command prompts keyboard events
    static void UpdateEvent(const sf::Event& event);

    /// @brief opens the command prompts window
    static void openPrompt();
    /// @brief closes the command prompts window without removing it from tgui
    /// @param abortTguiClose set to nullptr if not used with tgui
    static void closePrompt(bool* abortTguiClose);

    /// @brief only prints to the command prompt IF the command prompt is set to true
    /// @note setting command prompt to allow prints is only done through the command prompt its self during runtime
    static void print(const std::string& str, const Command::color& color = Command::color(255,255,255,255));

    static bool isPrintAllowed();

    /// @param print true if printing is allowed
    static void allowPrint(const bool& print = true);

protected:
    static void MaximizePrompt();
    /// @brief handles the resizing of the prompt
    static void ResizePrompt();

    static void UpdateAutoFill();

    /// @brief commits the currently selected auto fill
    static void AutoFill();

    static void addHistory(const std::string& command);
    static void setMaxHistory(const size_t& size);
    static size_t getMaxHistory();
    static void clearHistory();

private:
    inline CommandPrompt() = default;

    static tgui::ChildWindow::Ptr _parent;
    static tgui::EditBox::Ptr _textBox;
    static tgui::ListBox::Ptr _autoFillList;
    static tgui::ChatBox::Ptr _chatBox;

    static std::list<std::string> _commandHistory;
    static size_t _maxHistory;

    static bool _allowPrint;

    static tgui::Layout2d _parentSize;
    static tgui::Layout2d _parentPos;
};

#endif
