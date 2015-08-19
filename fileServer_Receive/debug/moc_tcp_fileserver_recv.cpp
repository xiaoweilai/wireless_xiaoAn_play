/****************************************************************************
** Meta object code from reading C++ file 'tcp_fileserver_recv.h'
**
** Created: Wed Aug 19 22:33:38 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../tcp_fileserver_recv.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tcp_fileserver_recv.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Tcp_FileServer_Recv[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x0a,
      29,   20,   20,   20, 0x0a,
      36,   20,   20,   20, 0x0a,
      55,   20,   20,   20, 0x0a,
      90,   78,   20,   20, 0x0a,
     141,   20,  133,   20, 0x0a,
     154,   20,  133,   20, 0x0a,
     168,   20,   20,   20, 0x0a,
     185,  181,   20,   20, 0x0a,
     212,  207,   20,   20, 0x0a,
     241,   20,   20,   20, 0x0a,
     257,   20,   20,   20, 0x0a,
     276,   20,   20,   20, 0x0a,
     291,   20,   20,   20, 0x0a,
     310,   20,   20,   20, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Tcp_FileServer_Recv[] = {
    "Tcp_FileServer_Recv\0\0start()\0stop()\0"
    "acceptConnection()\0updateServerProgress()\0"
    "socketError\0displayError(QAbstractSocket::SocketError)\0"
    "QString\0bindIpAddr()\0Getifconfig()\0"
    "LogInitLog()\0str\0LogWriteFile(QString)\0"
    "data\0LogWriteDataFile(QByteArray)\0"
    "LogDeleteFile()\0startPlayProcess()\0"
    "DelteMpgFile()\0MainPlayerThread()\0"
    "PktDeal()\0"
};

void Tcp_FileServer_Recv::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Tcp_FileServer_Recv *_t = static_cast<Tcp_FileServer_Recv *>(_o);
        switch (_id) {
        case 0: _t->start(); break;
        case 1: _t->stop(); break;
        case 2: _t->acceptConnection(); break;
        case 3: _t->updateServerProgress(); break;
        case 4: _t->displayError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 5: { QString _r = _t->bindIpAddr();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 6: { QString _r = _t->Getifconfig();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 7: _t->LogInitLog(); break;
        case 8: _t->LogWriteFile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 9: _t->LogWriteDataFile((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 10: _t->LogDeleteFile(); break;
        case 11: _t->startPlayProcess(); break;
        case 12: _t->DelteMpgFile(); break;
        case 13: _t->MainPlayerThread(); break;
        case 14: _t->PktDeal(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Tcp_FileServer_Recv::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Tcp_FileServer_Recv::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_Tcp_FileServer_Recv,
      qt_meta_data_Tcp_FileServer_Recv, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Tcp_FileServer_Recv::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Tcp_FileServer_Recv::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Tcp_FileServer_Recv::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Tcp_FileServer_Recv))
        return static_cast<void*>(const_cast< Tcp_FileServer_Recv*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int Tcp_FileServer_Recv::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
