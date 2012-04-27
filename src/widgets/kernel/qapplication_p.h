/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QAPPLICATION_P_H
#define QAPPLICATION_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qapplication_*.cpp, qwidget*.cpp, qcolor_x11.cpp, qfiledialog.cpp
// and many other.  This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.
//

#include "QtWidgets/qapplication.h"
#include "QtGui/qevent.h"
#include "QtGui/qfont.h"
#include "QtGui/qcursor.h"
#include "QtGui/qregion.h"
#include "QtGui/qwindow.h"
#include "qwidget.h"
#include "QtGui/qplatformnativeinterface_qpa.h"
#include "QtCore/qmutex.h"
#include "QtCore/qtranslator.h"
#include "QtCore/qbasictimer.h"
#include "QtCore/qhash.h"
#include "QtCore/qpointer.h"
#include "private/qcoreapplication_p.h"
#include "QtCore/qpoint.h"
#include <QTime>
#include <QWindowSystemInterface>
#include "private/qwindowsysteminterface_qpa_p.h"
#include "QtGui/qplatformintegration_qpa.h"
#include "private/qguiapplication_p.h"

QT_BEGIN_NAMESPACE

class QClipboard;
class QGraphicsScene;
class QObject;
class QWidget;
class QSocketNotifier;
class QTouchDevice;
#ifndef QT_NO_GESTURES
class QGestureManager;
#endif

extern Q_GUI_EXPORT bool qt_is_gui_used;
#ifndef QT_NO_CLIPBOARD
extern QClipboard *qt_clipboard;
#endif

#if defined (Q_OS_WIN32) || defined (Q_OS_CYGWIN) || defined(Q_OS_WINCE)
extern QSysInfo::WinVersion qt_winver;
enum { QT_TABLET_NPACKETQSIZE = 128 };
# ifdef Q_OS_WINCE
  extern DWORD qt_cever;
# endif
#elif defined (Q_OS_MAC)
extern QSysInfo::MacVersion qt_macver;
#endif
#if defined(Q_WS_QWS)
class QWSManager;
class QDirectPainter;
struct QWSServerCleaner { ~QWSServerCleaner(); };
#endif

#ifndef QT_NO_TABLET
struct QTabletDeviceData
{
#ifndef Q_WS_MAC
    int minPressure;
    int maxPressure;
    int minTanPressure;
    int maxTanPressure;
    int minX, maxX, minY, maxY, minZ, maxZ;
    inline QPointF scaleCoord(int coordX, int coordY, int outOriginX, int outExtentX,
                              int outOriginY, int outExtentY) const;
#endif
};

static inline int sign(int x)
{
    return x >= 0 ? 1 : -1;
}

#ifndef Q_WS_MAC
inline QPointF QTabletDeviceData::scaleCoord(int coordX, int coordY,
                                            int outOriginX, int outExtentX,
                                            int outOriginY, int outExtentY) const
{
    QPointF ret;

    if (sign(outExtentX) == sign(maxX))
        ret.setX(((coordX - minX) * qAbs(outExtentX) / qAbs(qreal(maxX - minX))) + outOriginX);
    else
        ret.setX(((qAbs(maxX) - (coordX - minX)) * qAbs(outExtentX) / qAbs(qreal(maxX - minX)))
                 + outOriginX);

    if (sign(outExtentY) == sign(maxY))
        ret.setY(((coordY - minY) * qAbs(outExtentY) / qAbs(qreal(maxY - minY))) + outOriginY);
    else
        ret.setY(((qAbs(maxY) - (coordY - minY)) * qAbs(outExtentY) / qAbs(qreal(maxY - minY)))
                 + outOriginY);

    return ret;
}
#endif

typedef QList<QTabletDeviceData> QTabletDeviceDataList;
QTabletDeviceDataList *qt_tablet_devices();
# if defined(Q_WS_MAC)
typedef QHash<int, QTabletDeviceData> QMacTabletHash;
QMacTabletHash *qt_mac_tablet_hash();
# endif
#endif

typedef QHash<QByteArray, QFont> FontHash;
FontHash *qt_app_fonts_hash();

typedef QHash<QByteArray, QPalette> PaletteHash;
PaletteHash *qt_app_palettes_hash();

#define QApplicationPrivateBase QGuiApplicationPrivate

class Q_WIDGETS_EXPORT QApplicationPrivate : public QApplicationPrivateBase
{
    Q_DECLARE_PUBLIC(QApplication)
public:
    QApplicationPrivate(int &argc, char **argv, QApplication::Type type, int flags);
    ~QApplicationPrivate();

    virtual void notifyLayoutDirectionChange();
    virtual void notifyActiveWindowChange(QWindow *);

    virtual bool shouldQuit();

#if defined(Q_WS_X11)
#ifndef QT_NO_SETTINGS
    static bool x11_apply_settings();
#endif
    static void reset_instance_pointer();
#elif defined(Q_WS_QWS)
    static bool qws_apply_settings();
    static QWidget *findWidget(const QObjectList&, const QPoint &, bool rec);
#endif
#ifdef Q_OS_WINCE
    static int autoMaximizeThreshold;
#endif
    static bool autoSipEnabled;
    static QString desktopStyleKey();


    void createEventDispatcher();
    QString appName() const;
    static void dispatchEnterLeave(QWidget *enter, QWidget *leave);

    //modality
    bool isWindowBlocked(QWindow *window, QWindow **blockingWindow = 0) const Q_DECL_OVERRIDE;
    static bool isBlockedByModal(QWidget *widget);
    static bool modalState();
    static bool tryModalHelper(QWidget *widget, QWidget **rettop = 0);
#ifdef Q_WS_MAC
    static QWidget *tryModalHelper_sys(QWidget *top);
	bool canQuit();
#endif

    bool notify_helper(QObject *receiver, QEvent * e);

    void construct(
#ifdef Q_WS_X11
                   Display *dpy = 0, Qt::HANDLE visual = 0, Qt::HANDLE cmap = 0
#endif
                   );
    void initialize();
    void process_cmdline();

#if defined(Q_WS_X11)
    static void x11_initialize_style();
#endif

    bool inPopupMode() const;
    void closePopup(QWidget *popup);
    void openPopup(QWidget *popup);
    static void setFocusWidget(QWidget *focus, Qt::FocusReason reason);
    static QWidget *focusNextPrevChild_helper(QWidget *toplevel, bool next);

#ifndef QT_NO_SESSIONMANAGER
    QSessionManager *session_manager;
    QString session_id;
    QString session_key;
    bool is_session_restored;
#endif

#ifndef QT_NO_GRAPHICSVIEW
    // Maintain a list of all scenes to ensure font and palette propagation to
    // all scenes.
    QList<QGraphicsScene *> scene_list;
#endif

    QBasicTimer toolTipWakeUp, toolTipFallAsleep;
    QPoint toolTipPos, toolTipGlobalPos, hoverGlobalPos;
    QPointer<QWidget> toolTipWidget;

    static QSize app_strut;
    static QWidgetList *popupWidgets;
    static QStyle *app_style;
    static int app_cspec;
    static QPalette *sys_pal;
    static QPalette *set_pal;

protected:
    void notifyThemeChanged();

public:
    static QFont *sys_font;
    static QFont *set_font;
    static QWidget *main_widget;
    static QWidget *focus_widget;
    static QWidget *hidden_focus_widget;
    static QWidget *active_window;
    static QIcon *app_icon;
#ifndef QT_NO_WHEELEVENT
    static int  wheel_scroll_lines;
#endif

    static bool animate_ui;
    static bool animate_menu;
    static bool animate_tooltip;
    static bool animate_combo;
    static bool fade_menu;
    static bool fade_tooltip;
    static bool animate_toolbox;
    static bool widgetCount; // Coupled with -widgetcount switch
    static bool load_testability; // Coupled with -testability switch

    static void setSystemPalette(const QPalette &pal);
    static void setPalette_helper(const QPalette &palette, const char* className, bool clearWidgetPaletteHash);
    static void initializeWidgetPaletteHash();
    static void initializeWidgetFontHash();
    static void setSystemFont(const QFont &font);

#if defined(Q_WS_X11)
    static void applyX11SpecificCommandLineArguments(QWidget *main_widget);
#elif defined(Q_WS_QWS)
    static void applyQWSSpecificCommandLineArguments(QWidget *main_widget);
#endif

#ifdef Q_WS_MAC
    static OSStatus globalEventProcessor(EventHandlerCallRef, EventRef, void *);
    static OSStatus globalAppleEventProcessor(const AppleEvent *, AppleEvent *, long);
    static OSStatus tabletProximityCallback(EventHandlerCallRef, EventRef, void *);
    static void qt_initAfterNSAppStarted();
    static void setupAppleEvents();
    static bool qt_mac_apply_settings();
#endif

#ifdef Q_WS_QWS
    QPointer<QWSManager> last_manager;
    QWSServerCleaner qwsServerCleaner;
# ifndef QT_NO_DIRECTPAINTER
    QHash<WId, QDirectPainter *> *directPainters;
# endif
    QRect maxWindowRect(const QScreen *screen) const { return maxWindowRects[screen]; }
    void setMaxWindowRect(const QScreen *screen, int screenNo, const QRect &rect);
    void setScreenTransformation(QScreen *screen, int screenNo, int transformation);
#endif

    static QApplicationPrivate *instance() { return self; }

    static QString styleOverride;

#ifdef QT_KEYPAD_NAVIGATION
    static QWidget *oldEditFocus;
    static Qt::NavigationMode navigationMode;
#endif

#if defined(Q_WS_MAC) || defined(Q_WS_X11)
    void _q_alertTimeOut();
    QHash<QWidget *, QTimer *> alertTimerHash;
#endif
#ifndef QT_NO_STYLE_STYLESHEET
    static QString styleSheet;
#endif
    static QPointer<QWidget> leaveAfterRelease;
    static QWidget *pickMouseReceiver(QWidget *candidate, const QPoint &windowPos, QPoint *pos,
                                      QEvent::Type type, Qt::MouseButtons buttons,
                                      QWidget *buttonDown, QWidget *alienWidget);
    static bool sendMouseEvent(QWidget *receiver, QMouseEvent *event, QWidget *alienWidget,
                               QWidget *native, QWidget **buttonDown, QPointer<QWidget> &lastMouseReceiver,
                               bool spontaneous = true);
    void sendSyntheticEnterLeave(QWidget *widget);

    static QWindow *windowForWidget(const QWidget *widget)
    {
        if (QWindow *window = widget->windowHandle())
            return window;
        if (const QWidget *nativeParent = widget->nativeParentWidget())
            return nativeParent->windowHandle();
        return 0;
    }

#ifdef Q_OS_WIN
    static HWND getHWNDForWidget(const QWidget *widget)
    {
        if (QWindow *window = windowForWidget(widget))
            if (window->handle())
                return static_cast<HWND> (QGuiApplication::platformNativeInterface()->
                                          nativeResourceForWindow(QByteArrayLiteral("handle"), window));
        return 0;
    }
#endif

#ifndef QT_NO_GESTURES
    QGestureManager *gestureManager;
    QWidget *gestureWidget;
#endif
#if defined(Q_WS_X11) || defined(Q_WS_WIN)
    QPixmap *move_cursor;
    QPixmap *copy_cursor;
    QPixmap *link_cursor;
#endif
#if defined(Q_WS_WIN)
    QPixmap *ignore_cursor;
#endif

    static void updateTouchPointsForWidget(QWidget *widget, QTouchEvent *touchEvent);
    void initializeMultitouch();
    void initializeMultitouch_sys();
    void cleanupMultitouch();
    void cleanupMultitouch_sys();
    QWidget *findClosestTouchPointTarget(QTouchDevice *device, const QPointF &screenPos);
    void appendTouchPoint(const QTouchEvent::TouchPoint &touchPoint);
    void removeTouchPoint(int touchPointId);
    static void translateRawTouchEvent(QWidget *widget,
                                       QTouchDevice *device,
                                       const QList<QTouchEvent::TouchPoint> &touchPoints,
                                       ulong timestamp);
    static void translateTouchCancel(QTouchDevice *device, ulong timestamp);

private:
#ifdef Q_WS_QWS
    QHash<const QScreen*, QRect> maxWindowRects;
#endif

    static QApplicationPrivate *self;

    static void giveFocusAccordingToFocusPolicy(QWidget *w,
                                                Qt::FocusPolicy focusPolicy,
                                                Qt::FocusReason focusReason);
    static bool shouldSetFocus(QWidget *w, Qt::FocusPolicy policy);


    static bool isAlien(QWidget *);
};

#if defined(Q_WS_WIN)
  extern void qt_win_set_cursor(QWidget *, bool);
#elif defined(Q_WS_X11)
  extern void qt_x11_enforce_cursor(QWidget *, bool);
  extern void qt_x11_enforce_cursor(QWidget *);
#else
  extern void qt_qpa_set_cursor(QWidget * w, bool force);
#endif

QT_END_NAMESPACE

#endif // QAPPLICATION_P_H
