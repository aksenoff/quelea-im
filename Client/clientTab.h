#ifndef CLIENTTAB_H
#define CLIENTTAB_H
#include <QTabWidget>

class ClientTab : public QTabWidget
{
public:
    QTabBar* getTabBar() const;
};

#endif
