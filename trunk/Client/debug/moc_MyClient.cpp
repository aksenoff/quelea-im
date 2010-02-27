/****************************************************************************
** Meta object code from reading C++ file 'MyClient.h'
**
** Created: Sat 20. Feb 00:12:02 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../MyClient.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MyClient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MyClient[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x08,
      26,    9,    9,    9, 0x08,
      66,    9,    9,    9, 0x08,
      85,    9,    9,    9, 0x08,
     101,    9,    9,    9, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MyClient[] = {
    "MyClient\0\0slotReadyRead()\0"
    "slotError(QAbstractSocket::SocketError)\0"
    "slotSendToServer()\0slotConnected()\0"
    "conn()\0"
};

const QMetaObject MyClient::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MyClient,
      qt_meta_data_MyClient, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MyClient::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MyClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MyClient::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MyClient))
        return static_cast<void*>(const_cast< MyClient*>(this));
    return QWidget::qt_metacast(_clname);
}

int MyClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotReadyRead(); break;
        case 1: slotError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 2: slotSendToServer(); break;
        case 3: slotConnected(); break;
        case 4: conn(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
