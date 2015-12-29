/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-02-11
 * Description : A kipi plugin to export images to a MediaWiki wiki
 *
 * Copyright (C) 2011      by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
 * Copyright (C) 2011-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "plugin_wikimedia.h"

// To disable warnings under MSVC2008 about getpid().
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

// C ANSI includes

extern "C"
{
#include <unistd.h>
}

// Qt includes

#include <QDir>
#include <QAction>
#include <QApplication>

// KDE includes

#include <kconfig.h>
#include <kactioncollection.h>
#include <kpluginfactory.h>
#include <kwindowsystem.h>
#include <klocalizedstring.h>

// Libkipi includes

#include <KIPI/Interface>

// Local includes

#include "kipiplugins_debug.h"
#include "wmwindow.h"

namespace KIPIWikiMediaPlugin
{

K_PLUGIN_FACTORY( WikiMediaFactory, registerPlugin<Plugin_WikiMedia>(); )

class Plugin_WikiMedia::Private
{
public:

    Private()
    {
        actionExport = 0;
        dlgExport    = 0;
    }

    QAction *  actionExport;
    WMWindow* dlgExport;
};

Plugin_WikiMedia::Plugin_WikiMedia(QObject* const parent, const QVariantList& /*args*/)
    : Plugin(parent, "MediaWiki export"),
      d(new Private)
{
    qCDebug(KIPIPLUGINS_LOG) << "Plugin_MediaWiki plugin loaded";

    setUiBaseName("kipiplugin_wikimediaui.rc");
    setupXML();
}

Plugin_WikiMedia::~Plugin_WikiMedia()
{
    delete d;
}

void Plugin_WikiMedia::setup(QWidget* const widget)
{
    d->dlgExport = 0;

    Plugin::setup(widget);
    setupActions();

    if (!interface())
    {
        qCCritical(KIPIPLUGINS_LOG) << "Kipi interface is null!";
        return;
    }

    d->actionExport->setEnabled(true);
}

void Plugin_WikiMedia::setupActions()
{
    setDefaultCategory(ExportPlugin);

    d->actionExport = new QAction(this);
    d->actionExport->setText(i18n("Export to MediaWiki..."));
    d->actionExport->setIcon(QIcon::fromTheme(QLatin1String("kipi-wikimedia")));
    d->actionExport->setEnabled(false);

    connect(d->actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()) );

    addAction(QLatin1String("wikimediaexport"), d->actionExport);
}

void Plugin_WikiMedia::slotExport()
{
    QString tmp = QDir::tempPath() + QLatin1Char('/') + QLatin1String("kipi-mediawiki") + QString::number(getpid()) + QLatin1String("/");

    if (!d->dlgExport)
    {
        // We clean it up in the close button
        d->dlgExport = new WMWindow(tmp, QApplication::activeWindow());
    }
    else
    {
        if (d->dlgExport->isMinimized())
            KWindowSystem::unminimizeWindow(d->dlgExport->winId());

        d->dlgExport->reactivate();
    }
}

} // namespace KIPIWikiMediaPlugin

#include "plugin_wikimedia.moc"
