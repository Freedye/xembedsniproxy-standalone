/*
    Main
    SPDX-FileCopyrightText: 2015 David Edmundson <davidedmundson@kde.org>
    SPDX-FileCopyrightText: 2026 Federico Bolcato <federico.bolc@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <QGuiApplication>

#include "fdoselectionmanager.h"

//#include "config-workspace.h"
#include "debug.h"
#include "snidbus.h"
#include "xcbutils.h"

#ifdef None
#ifndef FIXX11H_None
#define FIXX11H_None
inline constexpr XID XNone = None;
#undef None
inline constexpr XID None = XNone;
#endif
#undef None
#endif

#include <QDBusConnection>
#include <QDBusMetaType>

namespace Xcb
{
Xcb::Atoms *atoms;
Xcb::TrayVisual *trayVisual;
}

int main(int argc, char **argv)
{
    // the whole point of this is to interact with X, if we are in any other session, force trying to connect to X
    // if the QPA can't load xcb, this app is useless anyway.
    qputenv("QT_QPA_PLATFORM", "xcb");

    QGuiApplication::setDesktopSettingsAware(false);
    QCoreApplication::setAttribute(Qt::AA_DisableSessionManager);

    QGuiApplication app(argc, argv);

    if (QGuiApplication::platformName() != QLatin1String("xcb")) {
        qFatal("xembed-sni-proxy is only useful XCB. Aborting");
    }

    QCoreApplication::setApplicationName(QStringLiteral("xembedsniproxy"));
    QCoreApplication::setApplicationVersion(QStringLiteral("1.0"));

    app.setQuitOnLastWindowClosed(false);

    // make sure only one instance runs at a time; a plain well-known service
    // name claim is all KDBusService::Unique was really buying us here
    if (!QDBusConnection::sessionBus().registerService(QStringLiteral("org.xembedsniproxy.XEmbedSniProxy"))) {
        qCDebug(SNIPROXY) << "another instance is already running, exiting";
        return 0;
    }

    qDBusRegisterMetaType<KDbusImageStruct>();
    qDBusRegisterMetaType<KDbusImageVector>();
    qDBusRegisterMetaType<KDbusToolTipStruct>();

    Xcb::atoms = new Xcb::Atoms();
    Xcb::trayVisual = new Xcb::TrayVisual();

    FdoSelectionManager manager;

    auto rc = app.exec();

    delete Xcb::atoms;
    delete Xcb::trayVisual;
    return rc;
}
