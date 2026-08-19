/*
    SPDX-FileCopyrightText: 2026 Federico Bolcato <federico.bolc@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "selectionowner.h"

#include "c_ptr.h"
#include "debug.h"
#include "xcbutils.h"

#include <QGuiApplication>

#include <xcb/xcb_event.h>

X11SelectionOwner::X11SelectionOwner(xcb_atom_t selectionAtom, QObject *parent)
    : QObject(parent)
    , m_selectionAtom(selectionAtom)
{
    qApp->installNativeEventFilter(this);
}

X11SelectionOwner::~X11SelectionOwner()
{
    release();
}

xcb_connection_t *X11SelectionOwner::connection() const
{
    return qGuiApp->nativeInterface<QNativeInterface::QX11Application>()->connection();
}

void X11SelectionOwner::claim(bool force)
{
    xcb_connection_t *c = connection();
    xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(c)).data;

    if (!force) {
        const auto ownerCookie = xcb_get_selection_owner(c, m_selectionAtom);
        UniqueCPointer<xcb_get_selection_owner_reply_t> ownerReply(xcb_get_selection_owner_reply(c, ownerCookie, nullptr));
        if (ownerReply && ownerReply->owner != XCB_WINDOW_NONE) {
            qCWarning(SNIPROXY) << "selection already has an owner, refusing to steal it";
            Q_EMIT failedToClaimOwnership();
            return;
        }
    }

    if (m_window == XCB_WINDOW_NONE) {
        m_window = xcb_generate_id(c);
        xcb_create_window(c,
                           XCB_COPY_FROM_PARENT,
                           m_window,
                           screen->root,
                           -1,
                           -1,
                           1,
                           1,
                           0,
                           XCB_WINDOW_CLASS_INPUT_ONLY,
                           screen->root_visual,
                           0,
                           nullptr);
    }

    xcb_set_selection_owner(c, m_window, m_selectionAtom, XCB_CURRENT_TIME);

    // make sure we actually got it: someone else might have raced us
    const auto verifyCookie = xcb_get_selection_owner(c, m_selectionAtom);
    UniqueCPointer<xcb_get_selection_owner_reply_t> verifyReply(xcb_get_selection_owner_reply(c, verifyCookie, nullptr));
    if (!verifyReply || verifyReply->owner != m_window) {
        qCWarning(SNIPROXY) << "failed to become the selection owner";
        Q_EMIT failedToClaimOwnership();
        return;
    }

    // Announce ourselves as the new manager, per ICCCM's "Manager Selection"
    // convention (the same mechanism used for WM_Sn, the systray spec, etc.)
    xcb_client_message_event_t ev{};
    ev.response_type = XCB_CLIENT_MESSAGE;
    ev.format = 32;
    ev.window = screen->root;
    ev.type = Xcb::atoms->managerAtom;
    ev.data.data32[0] = XCB_CURRENT_TIME;
    ev.data.data32[1] = m_selectionAtom;
    ev.data.data32[2] = m_window;
    xcb_send_event(c, false, screen->root, XCB_EVENT_MASK_STRUCTURE_NOTIFY, reinterpret_cast<const char *>(&ev));
    xcb_flush(c);

    m_owning = true;
    Q_EMIT claimedOwnership();
}

void X11SelectionOwner::release()
{
    if (m_window == XCB_WINDOW_NONE) {
        return;
    }

    xcb_connection_t *c = connection();
    if (m_owning) {
        xcb_set_selection_owner(c, XCB_WINDOW_NONE, m_selectionAtom, XCB_CURRENT_TIME);
    }
    xcb_destroy_window(c, m_window);
    xcb_flush(c);

    m_window = XCB_WINDOW_NONE;
    m_owning = false;
}

bool X11SelectionOwner::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
{
    Q_UNUSED(result)

    if (eventType != "xcb_generic_event_t" || !m_owning) {
        return false;
    }

    const auto *ev = static_cast<xcb_generic_event_t *>(message);
    if (XCB_EVENT_RESPONSE_TYPE(ev) == XCB_SELECTION_CLEAR) {
        const auto *sce = reinterpret_cast<const xcb_selection_clear_event_t *>(ev);
        if (sce->owner == m_window && sce->selection == m_selectionAtom) {
            m_owning = false;
            Q_EMIT lostOwnership();
        }
    }

    return false;
}
