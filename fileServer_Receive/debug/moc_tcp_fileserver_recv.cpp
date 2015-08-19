/****************************************************************************
** Meta object code from reading C++ file 'tcp_fileserver_recv.h'
**
** Created: Thu Aug 20 00:49:21 2015
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
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x05,

 // slots: signature, parameters, type, tag, flags
      47,   20,   20,   20, 0x0a,
      55,   20,   20,   20, 0x0a,
      62,   20,   20,   20, 0x0a,
      81,   20,   20,   20, 0x0a,
     116,  104,   20,   20, 0x0a,
     167,   20,  159,   20, 0x0a,
     180,   20,  159,   20, 0x0a,
     194,   20,   20,   20, 0x0a,
     211,  207,   20,   20, 0x0a,
     238,  233,   20,   20, 0x0a,
     267,   20,   20,   20, 0x0a,
     283,  233,   20,   20, 0x0a,
     311,   20,   20,   20, 0x0a,
     338,  330,   20,   20, 0x0a,
     359,  330,   20,   20, 0x0a,
     380,   20,   20,   20, 0x0a,
     399,   20,   20,   20, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Tcp_FileServer_Recv[] = {
    "Tcp_FileServer_Recv\0\0emitGetNumsSignal(qint64)\0"
    "start()\0stop()\0acceptConnection()\0"
    "updateServerProgress()\0socketError\0"
    "displayError(QAbstractSocket::SocketError)\0"
    "QString\0bindIpAddr()\0Getifconfig()\0"
    "LogInitLog()\0str\0LogWriteFile(QString)\0"
    "data\0LogWriteDataFile(QByteArray)\0"
    "LogDeleteFile()\0LogWriteMpgData(QByteArray)\0"
    "startPlayProcess()\0dirname\0"
    "DelteMpgDir(QString)\0MkdirMpgDir(QString)\0"
    "MainPlayerThread()\0PktDeal()\0"
};

void Tcp_FileServer_Recv::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Tcp_FileServer_Recv *_t = static_cast<Tcp_FileServer_Recv *>(_o);
        switch (_id) {
        case 0: _t->emitGetNumsSignal((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 1: _t->start(); break;
        case 2: _t->stop(); break;
        case 3: _t->acceptConnection(); break;
        case 4: _t->updateServerProgress(); break;
        case 5: _t->displayError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 6: { QString _r = _t->bindIpAddr();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 7: { QString _r = _t->Getifconfig();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 8: _t->LogInitLog(); break;
        case 9: _t->LogWriteFile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 10: _t->LogWriteDataFile((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 11: _t->LogDeleteFile(); break;
        case 12: _t->LogWriteMpgData((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 13: _t->startPlayProcess(); break;
        case 14: _t->DelteMpgDir((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 15: _t->MkdirMpgDir((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 16: _t->MainPlayerThread(); break;
        case 17: _t->PktDeal(); break;
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
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void Tcp_FileServer_Recv::emitGetNumsSignal(qint64 _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
