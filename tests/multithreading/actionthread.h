/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-12-28
 * Description : re-implementation of action thread using threadweaver
 *
 * Copyright (C) 2011-2012
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

#ifndef ACTIONTHREAD_H
#define ACTIONTHREAD_H

// Qt includes

#include <QThread>

// KDE includes

#include <kurl.h>

#include "actions.h"
#include "weaverobservertest.h"

#include <threadweaver/Job.h>

class ActionThread : public QThread
{
    Q_OBJECT

public:

    ActionThread(QObject *parent=0);
    ~ActionThread();

    void rotate(const KUrl::List& urlList, KIPIJPEGLossLessPlugin::RotateAction val = KIPIJPEGLossLessPlugin::Rot180);
    void convert2grayscale(const KUrl::List& urlList);
    void cancel();

protected:

    void run();

private Q_SLOTS:

    void slotFinished();
    void slotJobDone(ThreadWeaver::Job*);
    void slotJobStarted(ThreadWeaver::Job*);

private:

    WeaverObserverTest* m_log;

    class ActionThreadPriv;
    ActionThreadPriv* const d;
};

#endif // ACTIONTHREAD_H
