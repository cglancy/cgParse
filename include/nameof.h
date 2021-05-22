#pragma once

#include <typeinfo>
#include <QString>

#ifdef Q_OS_MAC
#include <cxxabi.h>
#endif

template<typename Object>
QString classNameOf()
{
    QString name = typeid(Object).name();

#ifdef Q_OS_MAC
    char *pStr = abi::__cxa_demangle(name.toLocal8Bit().data(), nullptr, nullptr, nullptr);
    name = pStr;
    if (pStr)
        delete pStr;
#else
    name = name.mid(6);
#endif

    // remove namespace
    name = name.section("::", -1);

    return name;
}
