// Copyright (c) 2011-2018 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef GOTHAM_QT_MACNOTIFICATIONHANDLER_H
#define GOTHAM_QT_MACNOTIFICATIONHANDLER_H

#include <QObject>

/** Macintosh-specific notification handler (supports UserNotificationCenter).
 */
class MacNotificationHandler : public QObject
{
    Q_OBJECT

public:
    /** shows a macOS 10.8+ UserNotification in the UserNotificationCenter
     */
    void showNotification(const QString &title, const QString &text);

    /** check if OS can handle UserNotifications */
    bool hasUserNotificationCenterSupport();
    static MacNotificationHandler *instance();
};


#endif // GOTHAM_QT_MACNOTIFICATIONHANDLER_H
