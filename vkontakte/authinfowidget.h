/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-02-19
 * Description : A KIPI plugin to export images to VKontakte web service.
 *
 * Copyright (C) 2011, 2012, 2015  Alexander Potashev <aspotashev@gmail.com>
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

#ifndef AUTHINFOWIDGET_H
#define AUTHINFOWIDGET_H

// Qt includes

#include <QGroupBox>

class QLabel;
class QPushButton;

class KJob;

namespace Vkontakte
{
    class VkApi;
}

namespace KIPIVkontaktePlugin
{

class AuthInfoWidget : public QGroupBox
{
    Q_OBJECT

public:

    AuthInfoWidget(QWidget* const parent, Vkontakte::VkApi* const vkapi);
    ~AuthInfoWidget();

    QString albumsURL() const;

Q_SIGNALS:

    void authCleared();
    void signalUpdateAuthInfo();

public Q_SLOTS:

    void startAuthentication(bool forceLogout);

protected Q_SLOTS:

    void slotChangeUserClicked();

    void updateAuthInfo();

    void startGetUserInfo();
    void slotGetUserInfoDone(KJob* kjob);

protected:

    void handleVkError(KJob* kjob);

private:

    // VK.com interface
    Vkontakte::VkApi* m_vkapi;

    // Data
    int          m_userId;
    QString      m_userFullName;

    // GUI
    QLabel*      m_loginLabel;
    QPushButton* m_changeUserButton;
};

} // namespace KIPIVkontaktePlugin

#endif // AUTHINFOWIDGET_H
