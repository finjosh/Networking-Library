#include "include/Networking/SocketUI.hpp"

SocketUI::SocketUI(Port serverPort) : _client(sf::IpAddress::LocalHost, serverPort), _server(serverPort) {}

SocketUI::~SocketUI()
{
    if (_infoParent != nullptr) _infoParent->destroy();
    if (_connectionParent != nullptr) _connectionParent->destroy();
}

void SocketUI::initConnectionDisplay(tgui::Gui& gui)
{
    if (_connectionParent != nullptr) return;

    _connectionParent = tgui::ChildWindow::create("Connection Manager", tgui::ChildWindow::Close);
    gui.add(_connectionParent);
    _connectionParent->setPosition({"45%","45%"});
    _connectionParent->onClosing(_close, this, false);
    _connectionParent->setResizable(true);

    _serverCheck = tgui::CheckBox::create("Server");
    _clientCheck = tgui::CheckBox::create("Client");

    _passCheck = tgui::CheckBox::create("Check 1");
    _passEdit = tgui::EditBox::create();
    _passEdit->setDefaultText("Password");
    _IPEdit = tgui::EditBox::create();
    _IPEdit->setDefaultText("Server IP");
    _tryOpenConnection = tgui::Button::create("Try Open Connection");
    _tryOpenConnection->onClick([this](){
        TFunc::Add([this](TData* data){
            if (this->getClient()->NeedsPassword())
                this->UpdateConnectionDisplay();
            else if (!this->isServer() && !this->getClient()->isConnectionOpen())
                data->setRunning();
        });
    });
    _sendPassword = tgui::Button::create("Send Password");
    _IPState = tgui::Label::create("Checking IP...");

    _panel = tgui::ScrollablePanel::create();
    
    _connectionParent->add(_panel);
    _panel->setSize({"100%", "100%"});

    //* Updating which widgets to show
    _serverCheck->onCheck([this](){ _clientCheck->setChecked(false); UpdateConnectionDisplay(); });
    _serverCheck->onUncheck([this](){ this->resetUIConnectionStates(); UpdateConnectionDisplay(); });
    _clientCheck->onCheck([this](){ _serverCheck->setChecked(false); UpdateConnectionDisplay(); });
    _clientCheck->onUncheck([this](){ this->resetUIConnectionStates(); UpdateConnectionDisplay(); });
    _passCheck->onChange(UpdateConnectionDisplay, this);
    _passEdit->onTextChange([this](){ 
        if (this->_socket != nullptr)
        {
            this->_socket->setPassword(_passEdit->getText().toStdString());
        }
    });
    _IPEdit->onReturnOrUnfocus([this](){
        if (this->_socket != nullptr && !this->isServer() && _IPEdit->getText() != "")
        {
            //! not optimal but works good enough
            this->_validIPState = validIP::checking;

            std::thread* tempThread = new std::thread([this](){
                std::string pass = _IPEdit->getText().toStdString();
                if (SocketPlus::isValidIpAddress(pass))
                {
                    this->getClient()->setServerData(sf::IpAddress(pass)); 
                    this->_validIPState = validIP::valid;
                    return;
                }
                this->_validIPState = validIP::invalid;
            });

            TFunc::Add([this, tempThread](TData* data){
                if (this->_validIPState == validIP::checking)
                {
                    data->setRunning();
                    std::string temp = "Checking IP";
                    for (int i = 0; i < int(data->totalTime*2)%3 + 1; i++)
                        temp += '.';

                    this->_IPState->setText(temp);
                    return;
                }
                else if (this->_validIPState == validIP::invalid)
                {
                    this->_IPEdit->setDefaultText("Invalid IP Entered");
                    this->_IPEdit->setText("");
                }
                else
                {
                    this->_IPEdit->setDefaultText("Server IP");
                    this->_IPEdit->setText(this->getClient()->getServerIP().toString());
                }
                tempThread->detach();
                delete(tempThread); //! bad as all functions could be cleared and thread will never be removed
                this->UpdateConnectionDisplay();
            });
            this->UpdateConnectionDisplay();
        } 
    });

    UpdateConnectionDisplay();
}

void SocketUI::closeConnectionDisplay()
{
    if (_connectionParent == nullptr) return;
    _connectionParent->getParent()->remove(_connectionParent);
    _connectionParent = nullptr;
}

void SocketUI::setConnectionVisible(bool visible)
{
    if (_connectionParent == nullptr) return;
    _connectionParent->setVisible(visible);
    _connectionParent->setEnabled(visible);
    _connectionParent->moveToFront();
}

void SocketUI::initInfoDisplay(tgui::Gui& gui)
{
    if (_infoParent != nullptr) return;

    _infoParent = tgui::ChildWindow::create("Connection Info", tgui::ChildWindow::Close);
    gui.add(_infoParent);

    _infoParent->setResizable(true);
    _infoParent->setSize({"20%","35%"});
    _infoParent->onClosing(_close, this, true);

    _list = tgui::ListView::create();
    _infoParent->add(_list);
    _list->setResizableColumns(true);
    _list->addColumn("Name");
    _list->setColumnAutoResize(0,true);
    _list->addColumn("Data");
    _list->setColumnExpanded(1,true);
    _list->setMultiSelect(false);

    _clientData = tgui::TreeView::create();
    _infoParent->add(_clientData);

    _infoParent->onSizeChange(updateUISize, this);
    updateUISize();
}

void SocketUI::closeInfoDisplay()
{
    if (_infoParent == nullptr) return;
    _infoParent->getParent()->remove(_infoParent);
    _infoParent = nullptr;
    _list = nullptr;
    _clientData = nullptr;
}

void SocketUI::setInfoVisible(bool visible)
{
    if (_infoParent == nullptr) return;
    _infoParent->setVisible(visible);
    _infoParent->setEnabled(visible);
    _infoParent->moveToFront();
}

bool SocketUI::isConnectionOpen()
{
    return (_client.isConnectionOpen() || _server.isConnectionOpen());
}

bool SocketUI::isServer()
{
    return _isServer;
}

bool SocketUI::isEmpty()
{
    return (_socket == nullptr);
}

Server* SocketUI::getServer()
{
    return &_server;
}

Client* SocketUI::getClient()
{
    return &_client;
}

SocketPlus* SocketUI::getSocket()
{
    if (!isConnectionOpen())
        return nullptr;

    if (isServer())
        return &_server;
    return &_client;    
}

void SocketUI::update()
{
    if (_socket == nullptr) return;
    _list->changeItem(5, {"Connection Open Time", std::to_string(_socket->getConnectionTime())});
    _list->changeItem(6,{"Packets Stored", std::to_string(_socket->DataPackets.size())});

    // TODO check if this works
    if (_isServer)
    {
        _clientData->setVisible(true);
        _clientData->setEnabled(true);
        // auto scroll = _clientData->getVerticalScrollbarValue();
        // _clientData->removeAllItems();
        // _clientData->addItem({"Client Data"});
        // _clientData->addItem({"New Client IDs"});
        // _clientData->addItem({"Deleted Client IDs"});
        for (auto data: _server.getClients())
        {
            tgui::String id(data.first);
            if (_clientData->addItem({"Client Data", id}, false))
            {
                _clientData->addItem({"Client Data", id, {"IP: " + sf::IpAddress(data.second.id).toString()}});
                _clientData->addItem({"Client Data", id, {"Port: " + std::to_string(data.second.port)}});
                _clientData->addItem({"Client Data", id, {"Packets/s: " + std::to_string(data.second.PacketsPerSecond)}});
                _clientData->addItem({"Client Data", id, {"Last packet (s): " + std::to_string(data.second.TimeSinceLastPacket)}});
                _clientData->addItem({"Client Data", id, {"Connection Time (s): " + std::to_string(data.second.ConnectionTime)}});
            }
            else
            {
                _clientData->changeItem({"Client Data", id}, {"IP: " + sf::IpAddress(data.second.id).toString()});
                _clientData->changeItem({"Client Data", id}, {"Port: " + std::to_string(data.second.port)});
                _clientData->changeItem({"Client Data", id}, {"Packets/s: " + std::to_string(data.second.PacketsPerSecond)});
                _clientData->changeItem({"Client Data", id}, {"Last packet (s): " + std::to_string(data.second.TimeSinceLastPacket)});
                _clientData->changeItem({"Client Data", id}, {"Connection Time (s): " + std::to_string(data.second.ConnectionTime)});
            }
        }
        for (auto data: _server.newClientIDs)
        {
            _clientData->addItem({"New Client IDs", {std::to_string(data)}});
        }
        for (auto data: _server.deletedClientIDs)
        {
            _clientData->addItem({"Deleted Client IDs", {std::to_string(data)}});
        }
        // _clientData->setVerticalScrollbarValue(scroll);
    }
    else
    {
        _clientData->setVisible(false);
        _clientData->setEnabled(false);
    }
}

void SocketUI::initData()
{
    if (_socket == nullptr || _infoParent == nullptr) return;
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

void SocketUI::_close(bool isInfoDisplay, bool* tguiAbortClose)
{
    (*tguiAbortClose) = true;
    if (isInfoDisplay)
    {
        _infoParent->setVisible(false);
        _infoParent->setEnabled(false);
    }
    else
    {
        _connectionParent->setVisible(false);
        _connectionParent->setEnabled(false);
    }
}

void SocketUI::updateUISize()
{
    _list->setSize({"100%",{_list->getCurrentHeaderHeight() + 10 + _list->getItemHeight()*_list->getItemCount()}});
    _clientData->setSize({"100%",tgui::String(_infoParent->getInnerSize().y - _list->getFullSize().y - 10)});
    _clientData->setPosition({"0%",tgui::String(_list->getSize().y)});
}

void SocketUI::setServer()
{
    _isServer = true;
    _socket = &_server;
}

void SocketUI::setClient()
{
    _isServer = false;
    _socket = &_client;
}

void SocketUI::setEmpty()
{
    if (this->getSocket() == nullptr) return;
    if (_isServer)
    {
        _server.CloseServer();
    }
    else
    {
        _client.Disconnect();
    }   
    _socket = nullptr;
}

void SocketUI::UpdateConnectionDisplay()
{
    _panel->removeAllWidgets();

    addWidgetToConnection(_serverCheck);

    //* Is a Server
    if (_serverCheck->isChecked())
    {
        this->setServer();

        _passCheck->setText("Password");
        addWidgetToConnection(_passCheck, 1);

        if (_passCheck->isChecked())
        {
            addWidgetToConnection(_passEdit, 2);
        }

        if (this->getServer()->isConnectionOpen())
            _tryOpenConnection->setText("Close Server");
        else    
            _tryOpenConnection->setText("Open Server");

        addWidgetToConnection(_tryOpenConnection, 1);

        // adding the client check to the UI
        addWidgetToConnection(_clientCheck);
    }
    //* Is a Client
    else if (_clientCheck->isChecked())
    {
        addWidgetToConnection(_clientCheck);
        this->setClient();

        addWidgetToConnection(_IPEdit, 1);
        if (_validIPState == validIP::checking) 
            addWidgetToConnection(_IPState, 1, 5);

        if (_client.NeedsPassword())
        {
            _passEdit->setText("");
            addWidgetToConnection(_passEdit);
            addWidgetToConnection(_sendPassword);
        }

        _tryOpenConnection->setText("Try Connect");
        addWidgetToConnection(_tryOpenConnection, 1);
    }
    else
    {
        addWidgetToConnection(_clientCheck);
        this->setEmpty();
    }
}

void SocketUI::addWidgetToConnection(tgui::Widget::Ptr widgetPtr, float indent, float spacing)
{
    float currentHeight = 0;
    auto& widgets = _panel->getWidgets();
    if (widgets.size() != 0) 
    {
        currentHeight = widgets.back()->getPosition().y + widgets.back()->getSize().y;
    }
     
    if (widgetPtr != nullptr)
    {
        _panel->add(widgetPtr);
        widgetPtr->setPosition((1 + indent) * 10, currentHeight + spacing);
    }
}

void SocketUI::resetUIConnectionStates()
{
    _passCheck->setChecked(false);
    _passEdit->setText("");
    _IPEdit->setText("");
}
