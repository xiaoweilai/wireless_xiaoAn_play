/****************************************************************************
** Meta object code from reading C++ file 'playerthread.h'
**
** Created: Thu Aug 20 22:46:30 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../playerthread.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'playerthread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_playerThread[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      40,   33,   13,   13, 0x0a,
      66,   57,   13,   13, 0x0a,
      94,   13,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_playerThread[] = {
    "playerThread\0\0emitMsgBoxSignal()\0"
    "ulnums\0dealNums(qint64)\0pkt,size\0"
    "recvPkt(QByteArray,quint32)\0DelteMpgFile()\0"
};

void playerThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        playerThread *_t = static_cast<playerThread *>(_o);
        switch (_id) {
        case 0: _t->emitMsgBoxSignal(); break;
        case 1: _t->dealNums((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 2: _t->recvPkt((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< quint32(*)>(_a[2]))); break;
        case 3: _t->DelteMpgFile(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData playerThread::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject playerThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_playerThread,
      qt_meta_data_playerThread, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &playerThread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *playerThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *playerThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_playerThread))
        return static_cast<void*>(const_cast< playerThread*>(this));
    return QThread::qt_metacast(_clname);
}

int playerThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void playerThread::emitMsgBoxSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
