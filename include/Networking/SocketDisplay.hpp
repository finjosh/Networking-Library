#ifndef SOCKETDISPLAY_H
#define SOCKETDISPLAY_h

#pragma once

#include "TGUI/Backend/SFML-Graphics.hpp"
#include "TGUI/Widgets/ChildWindow.hpp"
#include "TGUI/Widgets/ScrollablePanel.hpp"
#include "TGUI/Widgets/TreeView.hpp"
#include "TGUI/Widgets/ListView.hpp"

#include "include/Networking/Client.hpp"
#include "include/Networking/Server.hpp"

class SocketDisplay
{
public:

    ~SocketDisplay();

    void init(tgui::Gui& gui);
    void close();

    void setSocket(Server& server);
    void setSocket(Client& client);

    /// @brief DO this when destroying the socket that was set
    void removeSocket();

    /// @brief call this every frame to update time based values
    void update();

protected:
    void initData();
    void _close(bool* tguiAbortClose);
    void updateUISize();

private:

    tgui::ChildWindow::Ptr _parent;
    tgui::ListView::Ptr _list;
    tgui::TreeView::Ptr _clientData;

    SocketPlus* _socket;
    Server* _server;
    Client* _client;
    bool _isServer = false;
};

#endif
