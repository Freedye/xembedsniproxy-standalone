/*
    Minimal, self-contained replacement for KSelectionOwner (KF6::WindowSystem),
    implementing just what xembedsniproxy needs: claiming an X11 "manager
    selection" (the ICCCM convention also used for WM_Sn, the systray spec, etc.)
    and finding out when that ownership is lost again.

    Note: for simplicity this claims the selection using XCB_CURRENT_TIME rather
    than a real server timestamp, which is a minor deviation from the ICCCM
    recommendation. It's harmless for our use case since claim() here is only
    ever called once at startup with force=false (i.e. we never contend for the
    selection against another owner).

    SPDX-FileCopyrightText: 2026 Federico Bolcato <federico.bolc@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#pragma once

#include <QAbstractNativeEventFilter>
#include <QObject>

#include <xcb/xcb.h>

class X11SelectionOwner : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    explicit X11SelectionOwner(xcb_atom_t selectionAtom, QObject *parent = nullptr);
    ~X11SelectionOwner() override;

    /**
     * Tries to become the owner of the selection.
     * If @p force is false and the selection already has an owner, this fails.
     */
    void claim(bool force = false);

    /**
     * Gives up ownership, if we currently have it.
     */
    void release();

    xcb_window_t ownerWindow() const
    {
        return m_window;
    }

Q_SIGNALS:
    void claimedOwnership();
    void failedToClaimOwnership();
    void lostOwnership();

protected:
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;

private:
    xcb_connection_t *connection() const;

    xcb_atom_t m_selectionAtom;
    xcb_window_t m_window = XCB_WINDOW_NONE;
    bool m_owning = false;
};
