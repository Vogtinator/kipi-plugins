/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-10-18
 * Description : a widget to edit a tag with multiple alternative
 *               language string entries.
 *
 * Copyright (C) 2007-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "altlangstringedit.moc"

// Qt includes

#include <QGridLayout>

// KDE includes

#include <kdialog.h>
#include <kglobal.h>
#include <klocale.h>

// LibKDcraw includes

#include <libkdcraw/squeezedcombobox.h>

// LibKExiv2 includes

#include <libkexiv2/altlangstredit.h>

// Local includes

#include "metadatacheckbox.h"

using namespace KDcrawIface;

namespace KIPIMetadataEditPlugin
{

class AltLangStringsEdit::AltLangStringsEditPriv
{
public:

    AltLangStringsEditPriv()
    {
        valueCheck = 0;
        editor     = 0;
    }

    KExiv2::AltLangMap oldValues;
    MetadataCheckBox*  valueCheck;
    AltLangStrEdit*    editor;
};

AltLangStringsEdit::AltLangStringsEdit(QWidget* parent, const QString& title, const QString& desc)
                  : QWidget(parent), d(new AltLangStringsEditPriv)
{
    d->valueCheck = new MetadataCheckBox(title, this);
    d->editor     = new AltLangStrEdit(this);
    d->editor->setClickMessage(desc);

    // --------------------------------------------------------

    QGridLayout* grid = new QGridLayout(this);
    grid->setAlignment( Qt::AlignTop );
    grid->addWidget(d->valueCheck, 0, 0, 1, 1);
    grid->addWidget(d->editor,     1, 0, 1, 1);
    grid->setMargin(0);
    grid->setSpacing(KDialog::spacingHint());

    // --------------------------------------------------------

    connect(d->valueCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalToggled(bool)));

    connect(d->valueCheck, SIGNAL(toggled(bool)),
            d->editor, SLOT(setEnabled(bool)));

    connect(d->valueCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->editor, SIGNAL(signalModified(const QString&, const QString&)),
            this, SIGNAL(signalModified()));

    connect(d->editor, SIGNAL(signalValueAdded(const QString&, const QString&)),
            this, SIGNAL(signalModified()));

    connect(d->editor, SIGNAL(signalValueDeleted(const QString&)),
            this, SIGNAL(signalModified()));

    connect(d->editor, SIGNAL(signalSelectionChanged(const QString&)),
            this, SLOT(slotSelectionChanged(const QString&)));
}

AltLangStringsEdit::~AltLangStringsEdit()
{
    delete d;
}

void AltLangStringsEdit::setValid(bool v)
{
    d->valueCheck->setValid(v);
}

bool AltLangStringsEdit::isValid() const
{
    return d->valueCheck->isValid();
}

void AltLangStringsEdit::setValues(const KExiv2::AltLangMap& values)
{
    d->oldValues = values;
    d->editor->setValues(values);
}

bool AltLangStringsEdit::getValues(KExiv2::AltLangMap& oldValues, KExiv2::AltLangMap& newValues)
{
    oldValues = d->oldValues;
    newValues = d->editor->values();
    return d->valueCheck->isChecked();
}

QString AltLangStringsEdit::defaultAltLang() const
{
    return d->editor->defaultAltLang();
}

bool AltLangStringsEdit::asDefaultAltLang() const
{
    return d->editor->asDefaultAltLang();
}

void AltLangStringsEdit::slotSelectionChanged(const QString& lang)
{
    emit signalDefaultLanguageEnabled(lang == defaultAltLang());
}

}  // namespace KIPIMetadataEditPlugin
