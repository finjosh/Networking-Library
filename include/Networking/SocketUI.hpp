#ifndef SOCKETUI_H
#define SOCKETUI_h

#pragma once

#include <numeric>
#include <thread>

#include "TGUI/Backend/SFML-Graphics.hpp"
#include "TGUI/Widgets/ChildWindow.hpp"
#include "TGUI/Widgets/TreeView.hpp"
#include "TGUI/Widgets/ListView.hpp"
#include "TGUI/Widgets/EditBox.hpp"
#include "TGUI/Widgets/Button.hpp"
#include "TGUI/Widgets/CheckBox.hpp"
#include "TGUI/Widgets/ScrollablePanel.hpp"
#include "TGUI/Widgets/Label.hpp"

#include "include/Networking/Client.hpp"
#include "include/Networking/Server.hpp"
#include "include/Utils/TerminatingFunction.hpp"
// TODO add a request to stop 
//      ^ so that we can delete this socket UI without leaking memory
class SocketUI
{
public:

    SocketUI(Port serverPort, funcHelper::func<void> serverCustomPacketSendFunction, funcHelper::func<void> clientCustomPacketSendFunction);
    ~SocketUI();

    /// @brief creates the UI for opening a server or client connection
    void initConnectionDisplay(tgui::Gui& gui);
    /// @brief completely closes the connection display removing it from UI
    void closeConnectionDisplay();
    void setConnectionVisible(bool visible = true);

    /// @brief creates the UI for the connection info display
    void initInfoDisplay(tgui::Gui& gui);
    /// @brief completely closes the info display removing it from UI
    void closeInfoDisplay();
    void setInfoVisible(bool visible = true);

    /// @returns true if either the client or server has an open connection
    bool isConnectionOpen();
    /// @returns if the current socket is a server
    /// @note if connection is closed this is invalid
    bool isServer();
    /// @returns true if neither a server or client is selected
    bool isEmpty();
    /// @brief might not be in use if isServer is false
    /// @return pointer to the server being used
    Server* getServer();
    /// @brief might not be in use if isServer is true
    /// @return pointer to the client being used
    Client* getClient();
    /// @note nullptr if no socket is in use
    /// @returns the pointer to the socket that is currently in use
    SocketPlus* getSocket();
    // /// @param server true if server false if client
    // void setServer(bool server);

    /// @brief call this every frame
    void updateInfoDisplay();

protected:
    void initData();
    void _close(bool isInfoDisplay, bool* tguiAbortClose);
    void updateUISize();
    void setServer();
    void setClient();
    /// @brief this is no longer a client or a server
    void setEmpty();

    //* Functions for updating the UI in the connection parent
    void updateConnectionDisplay();
    /// @brief adds the given widget to the connection display with proper spacing
    /// @param indent the number of indents to move the widget by
    /// @param spacing the space from the last widget
    /// @note for a space put a nullptr in
    void addWidgetToConnection(tgui::Widget::Ptr widgetPtr, float indent = 0, float spacing = 10);
    /// @brief resets the state of the checkboxes, edit boxes, and buttons
    void resetUIConnectionStates();

    /// @brief tries to open connection with the current data
    void tryOpenConnection();
    void closeConnection();

private:

    enum validIP
    {
        valid,
        invalid,
        checking
    };

    validIP _validIPState = validIP::valid;

    tgui::ChildWindow::Ptr _infoParent;
    tgui::ListView::Ptr _list;
    tgui::TreeView::Ptr _clientData;

    tgui::ChildWindow::Ptr _connectionParent;
    tgui::CheckBox::Ptr _serverCheck;
    tgui::CheckBox::Ptr _clientCheck;
    tgui::CheckBox::Ptr _passCheck;
    tgui::EditBox::Ptr _passEdit;
    tgui::EditBox::Ptr _IPEdit;
    tgui::Label::Ptr _IPState;
    // Open / close connection
    tgui::Button::Ptr _tryOpenConnection;
    tgui::Button::Ptr _sendPassword;
    tgui::ScrollablePanel::Ptr _panel;

    SocketPlus* _socket = nullptr;
    Server _server;
    funcHelper::func<void> _sSendFunc;
    Client _client;
    funcHelper::func<void> _cSendFunc;
    bool _isServer = false;

};

#endif
