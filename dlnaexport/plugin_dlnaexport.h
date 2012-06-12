/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-05-28
 * Description : a KIPI plugin to export pics through DLNA technology.
 *
 * Copyright (C) 2012 by Smit Mehta <smit dot meh at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef PLUGIN_DLNAEXPORT_H
#define PLUGIN_DLNAEXPORT_H

// Qt includes

#include <QVariant>

// LibKIPI includes

#include <libkipi/plugin.h>

using namespace KIPI;

namespace KIPIDLNAExportPlugin
{

class Plugin_DLNAExport : public Plugin
{
    Q_OBJECT

public:

    Plugin_DLNAExport(QObject* const parent, const QVariantList& args);
    virtual ~Plugin_DLNAExport();

    Category category(KAction* const action) const;
    void setup(QWidget* const);

private Q_SLOTS:

    void slotExport();

private:

    class Private;
    Private* const d;
};

}  // namespace KIPIDLNAExportPlugin

#endif // PLUGIN_DLNAEXPORT_H
