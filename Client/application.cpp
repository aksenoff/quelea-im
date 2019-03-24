#include "application.h"

Application::Application(int argc, char** argv): QApplication (argc, argv)
{
    _singleton = new QSharedMemory("foobar", this);
}

Application::~Application()
{
    if(_singleton->isAttached())
        _singleton->detach();
}

bool Application::lock()
{
    if(_singleton->attach(QSharedMemory::ReadOnly))
    {
        _singleton->detach();
        return false;
    }

    if(_singleton->create(1))
        return true;

    return false;
}
