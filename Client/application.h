#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QSharedMemory>

class Application: public QApplication
{
    Q_OBJECT
public:
    Application(int, char**);
    ~Application();

    bool lock();

private:
    QSharedMemory *_singleton;
};
#endif // APPLICATION_H
