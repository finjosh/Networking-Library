#include "include/Networking/SocketDisplay.hpp"

SocketDisplay::~SocketDisplay()
{
    _parent->destroy();
}

void SocketDisplay::init(tgui::Gui& gui)
{
    _parent = tgui::ChildWindow::create("Socket Info", tgui::ChildWindow::Close);
    gui.add(_parent);

    _parent->setResizable(true);
    _parent->setSize({"20%","35%"});
    _parent->onClosing(_close, this);

    _parent->add(tgui::ScrollablePanel::create());
    _list = tgui::ListView::create();
    _parent->add(_list);
    _list->setResizableColumns(true);
    _list->addColumn("Name");
    _list->setColumnAutoResize(0,true);
    _list->addColumn("Data");
    _list->setColumnExpanded(1,true);
    _list->setMultiSelect(false);

    _clientData = tgui::TreeView::create();
    _parent->add(_clientData);

    _parent->onSizeChange(updateUISize, this);
}

void SocketDisplay::close()
{
    _parent = nullptr;
    _list = nullptr;
}

void SocketDisplay::setSocket(Server& server)
{
    _socket = &server;
    _server = &server;
    _isServer = true;
    initData();
}

void SocketDisplay::setSocket(Client& client)
{
    _socket = &client;
    _client = &client;
    _isServer = false;
    initData();
}

void SocketDisplay::removeSocket()
{
    _socket = nullptr;
    _server = nullptr;
    _client = nullptr;
}

void SocketDisplay::update()
{
    if (_socket == nullptr) return;
    _list->changeItem(5, {"Connection Open Time", std::to_string(_socket->getConnectionTime())});
    _list->changeItem(6,{"Packets Stored", std::to_string(_socket->DataPackets.size())});

    if (_isServer)
    {
        _clientData->setVisible(true);
        _clientData->setEnabled(true);
        auto scroll = _clientData->getVerticalScrollbarValue();
        _clientData->removeAllItems();
        _clientData->addItem({"Client Data"});
        _clientData->addItem({"New Client IDs"});
        _clientData->addItem({"Deleted Client IDs"});
        for (auto data: _server->getClients())
        {
            tgui::String id(data.first);
            _clientData->addItem({"Client Data", id, {"IP: " + sf::IpAddress(data.second.id).toString()}});
            _clientData->addItem({"Client Data", id, {"Port: " + std::to_string(data.second.port)}});
            _clientData->addItem({"Client Data", id, {"Packets/s: " + std::to_string(data.second.PacketsPerSecond)}});
            _clientData->addItem({"Client Data", id, {"Last packet (s): " + std::to_string(data.second.TimeSinceLastPacket)}});
            _clientData->addItem({"Client Data", id, {"Connection Time (s): " + std::to_string(data.second.ConnectionTime)}});
        }
        for (auto data: _server->newClientIDs)
        {
            _clientData->addItem({"New Client IDs", {std::to_string(data)}});
        }
        for (auto data: _server->deletedClientIDs)
        {
            _clientData->addItem({"Deleted Client IDs", {std::to_string(data)}});
        }
        _clientData->setVerticalScrollbarValue(scroll);
    }
    else
    {
        _clientData->setVisible(false);
        _clientData->setEnabled(false);
    }
}

void SocketDisplay::initData()
{
    if (_socket == nullptr) return;
    _list->removeAllItems();
    _list->addItem({"ID", std::to_string(_socket->getID())});
    _list->addItem({"Public IP", _socket->getIP().toString()});
    _list->addItem({"Local IP", _socket->getLocalIP().toString()});
    _list->addItem({"Port", std::to_string(_socket->getPort())});
    _list->addItem({"Connection Open", (_socket->isConnectionOpen() ? "True" : "False")});
    _list->addItem({"Connection Open Time", std::to_string(_socket->getConnectionTime())});
    _list->addItem({"Packets Stored", std::to_string(_socket->DataPackets.size())});
    updateUISize();
}

void SocketDisplay::_close(bool* tguiAbortClose)
{
    (*tguiAbortClose) = true;
    _parent->setVisible(false);
    _parent->setEnabled(false);
}

void SocketDisplay::updateUISize()
{
    _list->setSize({"100%",{_list->getCurrentHeaderHeight() + 10 + _list->getItemHeight()*_list->getItemCount()}});
    _clientData->setSize({"100%",tgui::String(_parent->getInnerSize().y - _list->getFullSize().y - 10)});
    _clientData->setPosition({"0%",tgui::String(_list->getSize().y)});
}
