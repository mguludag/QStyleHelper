/*
 * MIT License
 *
 * Copyright (c) 2023 Muhammed Galib Uludag
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef QSTYLEHELPER_HPP
#define QSTYLEHELPER_HPP
#include <QtGlobal>

#include <QBasicTimer>
#include <QProcess>
#include <QSettings>
#include <QWindow>

#ifdef QT_WIDGETS_LIB
#include <QApplication>
#include <QPalette>
#include <QStyle>
#include <QStyleFactory>
#include <QToolTip>
#endif

#if defined(Q_CC_GNU) || defined(Q_CC_CLANG)
#define DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(Q_CC_MSVC)
#define DEPRECATED(func) __declspec(deprecated) func
#endif

#if defined(Q_OS_WIN) && defined(Q_CC_MSVC)
#include <QOperatingSystemVersion>
#include <dwmapi.h>

#pragma comment(lib, "Dwmapi.lib")


enum DwmWindowAttribute : uint
{
    UseImmersiveDarkMode = 20,
    WindowCornerPreference = 33,
    BorderColor,
    CaptionColor,
    TextColor,
    VisibleFrameBorderThickness,
    SystemBackdropType,
    UndocumentedSystemBackdropType = 1029,
    Last
};

#endif

#if defined(Q_OS_WIN) && defined(Q_CC_MSVC) && defined(QT_WIDGETS_LIB)

typedef enum _WINDOWCOMPOSITIONATTRIB
{
    WCA_THEME_ATTRIBUTES = 10,
    WCA_ACCENT_POLICY = 19,
    WCA_FREEZE_REPRESENTATION = 20,
    WCA_MICA = 26
} WINDOWCOMPOSITIONATTRIB;

typedef struct _WINDOWCOMPOSITIONATTRIBDATA
{
    WINDOWCOMPOSITIONATTRIB Attrib;
    PVOID pvData;
    SIZE_T cbData;
} WINDOWCOMPOSITIONATTRIBDATA;

typedef enum _ACCENT_STATE
{
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4, // RS4 1803
    ACCENT_ENABLE_HOSTBACKDROP = 5, // RS5 1809
    ACCENT_INVALID_STATE = 6
} ACCENT_STATE;

typedef struct _ACCENT_POLICY
{
    ACCENT_STATE AccentState;
    DWORD AccentFlags;
    DWORD GradientColor;
    DWORD AnimationId;
} ACCENT_POLICY;

typedef BOOL (WINAPI *pfnGetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

typedef BOOL (WINAPI *pfnSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

#endif


class QStyleHelper : public QObject
{
    Q_OBJECT
public:
    static QStyleHelper &instance();
    ~QStyleHelper();

    static void setTitleBarDarkColor();
    static void setTitleBarDarkColor(QList<QWindow*> &&windows, bool dark = true);
    static void setMica(QList<QWindow*> &&windows, bool acrylic = false);

    DEPRECATED(static void setAcrylicBlurWindow(QList<QWindow*> &&windows, bool acrylic = false));

#if defined(QT_WIDGETS_LIB) && defined(Q_CC_MSVC)
    static void setTitleBarDarkColor(std::initializer_list<std::reference_wrapper<QWidget>> &&windows, bool dark = true);
    static void setMica(std::initializer_list<std::reference_wrapper<QWidget>> &&windows, bool acrylic = false);
    DEPRECATED(static void setAcrylicBlurWindow(std::initializer_list<std::reference_wrapper<QWidget>> &&windows, bool acrylic = false));
#endif

#if defined(QT_WIDGETS_LIB)
    static QStringList QStyleNames();
    QStyleHelper &setWidgetStyle(const QString& styleName);
    QStyleHelper &setCustomDarkPalette(const QPalette &newCustomDarkPalette);
    QStyleHelper &setCustomLightPalette(const QPalette &newCustomLightPalette);
    QStyleHelper &setAutoChangePalette(bool autochange);
    QStyleHelper &setDarkPalette();
    QStyleHelper &setLightPalette();
#endif

signals:
    void colorSchemeChanged(bool isDark);

protected:
    void timerEvent(QTimerEvent *) override;
    bool isDark();

private:
    QStyleHelper();

#ifdef QT_WIDGETS_LIB
    struct QPaletteHelper
    {
        QPaletteHelper();

        void setCustomDarkPalette(const QPalette &newCustomDarkPalette);
        void setCustomLightPalette(const QPalette &newCustomLightPalette);
        void setPalette(bool dark);
        const bool& style();


    private:
        bool mIsDarkPalette;
        QPalette mCustomDarkPalette;
        QPalette mCustomLightPalette;
        const QColor lightGray{190, 190, 190};
        const QColor gray{164, 166, 168};
        const QColor midDarkGray{68, 68, 68};
        const QColor darkGray{53, 53, 53};
        const QColor black{25, 25, 25};
        const QColor blue{42, 130, 218};
    };
#endif

private:
    bool mIsDark;
    bool mAutoChangePalette;
    QBasicTimer mTimer;
    QSettings mSettings;
#ifdef QT_WIDGETS_LIB
    QPaletteHelper mPaletteHelper;
#endif
};



inline QStyleHelper::QStyleHelper() :
    mIsDark(false), mSettings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                              QSettings::NativeFormat)
{
    mTimer.start(500, Qt::VeryCoarseTimer, this);
}

inline QStyleHelper &QStyleHelper::instance()
{
    static QStyleHelper d;
    return d;
}

inline QStyleHelper::~QStyleHelper()
{
    mTimer.stop();
}

inline void QStyleHelper::setTitleBarDarkColor()
{
#if defined(Q_OS_WIN)
    qputenv("QT_QPA_PLATFORM", "windows:darkmode=1");
#endif // Q_OS_WINDOWS
}

#if defined(QT_WIDGETS_LIB) && defined(Q_CC_MSVC)
inline void QStyleHelper::setTitleBarDarkColor(std::initializer_list<std::reference_wrapper<QWidget>> &&windows, bool dark)
{
#if defined(Q_OS_WIN)
    for (auto &w : windows) {
        auto hwnd = w.get().winId();
        const BOOL darkBorder = static_cast<BOOL>(dark);
        DwmSetWindowAttribute((HWND)hwnd, DwmWindowAttribute::UseImmersiveDarkMode, &darkBorder, sizeof(darkBorder));
    }
#endif
}

inline void QStyleHelper::setAcrylicBlurWindow(std::initializer_list<std::reference_wrapper<QWidget>> &&windows, bool acrylic)
{
#if defined(Q_OS_WIN) && defined(Q_CC_MSVC) && defined(QT_WIDGETS_LIB)
    for (auto &w : windows) {
        auto hwnd = w.get().winId();

        HMODULE hUser = GetModuleHandle(L"user32.dll");
        if (hUser){
            auto setWCA = (pfnSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
            if (setWCA){
                ACCENT_POLICY accent = { acrylic ? ACCENT_ENABLE_ACRYLICBLURBEHIND : ACCENT_ENABLE_BLURBEHIND, 3, 0x0cccccc, 0 };
                WINDOWCOMPOSITIONATTRIBDATA data{WCA_MICA, &accent, sizeof(accent)};
                setWCA((HWND)hwnd, &data);
            }
        }
    }
#endif
}

inline void QStyleHelper::setMica(std::initializer_list<std::reference_wrapper<QWidget>> &&windows, bool acrylic)
{
#if defined(Q_OS_WIN) && defined(Q_CC_MSVC) && defined(QT_WIDGETS_LIB)
    for (auto &w : windows) {
        auto hwnd = w.get().winId();

        HMODULE hUser = GetModuleHandle(L"user32.dll");
        if (hUser){
            auto setWCA = (pfnSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
            if (setWCA){
                ACCENT_POLICY accent = {ACCENT_ENABLE_HOSTBACKDROP,
                                         static_cast<DWORD>((acrylic ? 3 : 2)), 0x0cccccc, 0 };
                WINDOWCOMPOSITIONATTRIBDATA data{WCA_ACCENT_POLICY, &accent, sizeof(accent)};
                setWCA((HWND)hwnd, &data);
                const MARGINS margins{-1, -1, -1, -1};
                DwmExtendFrameIntoClientArea((HWND)hwnd, &margins);
                setWCA((HWND)hwnd, &data);
                const auto build = QOperatingSystemVersion::current().microVersion();
                const BOOL backdrop = acrylic ? (build < 22523 ? 2 : 3) : (build < 22523 ? 1 : 2);
                DwmSetWindowAttribute((HWND)hwnd, build < 22523 ?
                                          DwmWindowAttribute::UndocumentedSystemBackdropType : DwmWindowAttribute::SystemBackdropType,
                                      &backdrop, sizeof(backdrop));
            }
        }
    }
#endif
}
#endif

inline void QStyleHelper::setTitleBarDarkColor(QList<QWindow *> &&windows, bool dark)
{
#if defined(Q_OS_WIN)
    for (auto &w : windows) {
        auto hwnd = w->winId();
        const BOOL darkBorder = static_cast<BOOL>(dark);
        DwmSetWindowAttribute((HWND)hwnd, DwmWindowAttribute::UseImmersiveDarkMode, &darkBorder, sizeof(darkBorder));
    }
#endif
}

inline void QStyleHelper::setAcrylicBlurWindow(QList<QWindow *> &&windows, bool acrylic)
{
#if defined(Q_OS_WIN) && defined(Q_CC_MSVC) && defined(QT_WIDGETS_LIB)
    for (auto &w : windows) {
        auto hwnd = w->winId();

        HMODULE hUser = GetModuleHandle(L"user32.dll");
        if (hUser){
            auto setWCA = (pfnSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
            if (setWCA){
                ACCENT_POLICY accent = { acrylic ? ACCENT_ENABLE_ACRYLICBLURBEHIND : ACCENT_ENABLE_BLURBEHIND, 0, 0, 0 };
                WINDOWCOMPOSITIONATTRIBDATA data{WCA_ACCENT_POLICY, &accent, sizeof(accent)};
                setWCA((HWND)hwnd, &data);
            }
        }
    }
#endif
}

inline void QStyleHelper::setMica(QList<QWindow *> &&windows, bool acrylic)
{
#if defined(Q_OS_WIN) && defined(Q_CC_MSVC) && defined(QT_QML_LIB)
    for (auto &w : windows) {
        auto hwnd = w->winId();
        const auto build = QOperatingSystemVersion::current().microVersion();
        const BOOL backdrop = acrylic ? (build < 22523 ? 2 : 3) : (build < 22523 ? 1 : 2);
        DwmSetWindowAttribute((HWND)hwnd, build < 22523 ?
                                  DwmWindowAttribute::UndocumentedSystemBackdropType : DwmWindowAttribute::SystemBackdropType,
                              &backdrop, sizeof(backdrop));
    }
#endif
}

#if defined(QT_WIDGETS_LIB)
inline QStringList QStyleHelper::QStyleNames() { return QStyleFactory::keys(); }

inline QStyleHelper &QStyleHelper::setWidgetStyle(const QString &styleName)
{
    QApplication::setStyle(QStyleFactory::create(styleName));
    return *this;
}

inline QStyleHelper &QStyleHelper::setCustomDarkPalette(const QPalette &newCustomDarkPalette)
{
    mPaletteHelper.setCustomDarkPalette(newCustomDarkPalette);
    return *this;
}

inline QStyleHelper &QStyleHelper::setCustomLightPalette(const QPalette &newCustomLightPalette)
{
    mPaletteHelper.setCustomLightPalette(newCustomLightPalette);
    return *this;
}

inline QStyleHelper &QStyleHelper::setAutoChangePalette(bool autochange)
{
    mAutoChangePalette = autochange;
    return *this;
}

inline QStyleHelper &QStyleHelper::setDarkPalette()
{
    mPaletteHelper.setPalette(true);
    return *this;
}

inline QStyleHelper &QStyleHelper::setLightPalette()
{
    mPaletteHelper.setPalette(false);
    return *this;
}
#endif

inline void QStyleHelper::timerEvent(QTimerEvent *)
{
    auto result = isDark();
    if(result == mIsDark){
        mIsDark = !result;
        emit colorSchemeChanged(mIsDark);
#if defined(QT_WIDGETS_LIB)
        mAutoChangePalette ? mPaletteHelper.setPalette(mIsDark) : void();
#endif
    }
}

inline bool QStyleHelper::isDark()
{
#ifdef Q_OS_WINDOWS
    return mSettings.value("AppsUseLightTheme", true).toBool();
#elif Q_OS_UNIX
//TODO: add linux equivalent for detecting scheme changes
//      org.freedesktop.portal.Desktop /org/freedesktop/portal/desktop org.freedesktop.portal.Settings.Read "org.freedesktop.appearance" "color-scheme"
//      https://code.qt.io/cgit/qt/qtbase.git/tree/src/plugins/platformthemes/xdgdesktopportal/qxdgdesktopportaltheme.cpp?h=dev#n116
#endif
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#if defined(QT_WIDGETS_LIB)
inline void QStyleHelper::QPaletteHelper::setPalette(bool dark)
{
    QPalette palette{dark ? mCustomDarkPalette : mCustomLightPalette};

    mIsDarkPalette = dark;

    QApplication::setPalette(palette);
    QToolTip::setPalette(palette);

    auto borderBrush = (palette == QApplication::style()->standardPalette() ? palette.midlight() : palette.mid());
    static auto tbStyleSheet = QString("QToolBar { border: 1px solid %1; }");
    static auto app = dynamic_cast<QApplication*>(QApplication::instance());
    app->setStyleSheet(tbStyleSheet.arg(borderBrush.color().name()));
}

inline const bool& QStyleHelper::QPaletteHelper::style()
{
    return mIsDarkPalette;
}

inline void QStyleHelper::QPaletteHelper::setCustomLightPalette(const QPalette &newCustomLightPalette)
{
    mCustomLightPalette = newCustomLightPalette;
}

inline QStyleHelper::QPaletteHelper::QPaletteHelper()
    : mCustomDarkPalette(QApplication::style()->standardPalette()),
      mCustomLightPalette(QApplication::style()->standardPalette())
{
    mCustomDarkPalette.setColor(QPalette::Window, darkGray);
    mCustomDarkPalette.setColor(QPalette::WindowText, Qt::white);
    mCustomDarkPalette.setColor(QPalette::Light, lightGray);
    mCustomDarkPalette.setColor(QPalette::Midlight, midDarkGray);
    mCustomDarkPalette.setColor(QPalette::Dark, darkGray);
    mCustomDarkPalette.setColor(QPalette::Mid, midDarkGray);
    mCustomDarkPalette.setColor(QPalette::Shadow, black);
    mCustomDarkPalette.setColor(QPalette::Base, black);
    mCustomDarkPalette.setColor(QPalette::AlternateBase, darkGray);
    mCustomDarkPalette.setColor(QPalette::ToolTipBase, darkGray);
    mCustomDarkPalette.setColor(QPalette::ToolTipText, Qt::white);
    mCustomDarkPalette.setColor(QPalette::Text, Qt::white);
    mCustomDarkPalette.setColor(QPalette::Button, darkGray);
    mCustomDarkPalette.setColor(QPalette::ButtonText, Qt::white);
    mCustomDarkPalette.setColor(QPalette::BrightText, Qt::red);
    mCustomDarkPalette.setColor(QPalette::Link, blue);
    mCustomDarkPalette.setColor(QPalette::Highlight, blue);
    mCustomDarkPalette.setColor(QPalette::HighlightedText, Qt::black);
    mCustomDarkPalette.setColor(QPalette::Disabled, QPalette::Text, midDarkGray);
    mCustomDarkPalette.setColor(QPalette::Disabled, QPalette::WindowText, midDarkGray);
    mCustomDarkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, midDarkGray);
    mCustomDarkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, midDarkGray);
    mCustomDarkPalette.setColor(QPalette::Disabled, QPalette::Base, midDarkGray);
    mCustomDarkPalette.setColor(QPalette::Disabled, QPalette::Window, midDarkGray);
    mCustomDarkPalette.setColor(QPalette::Disabled, QPalette::Highlight, midDarkGray);
}

inline void QStyleHelper::QPaletteHelper::setCustomDarkPalette(const QPalette &newCustomDarkPalette)
{
    mCustomDarkPalette = newCustomDarkPalette;
}

#endif //QT_WIDGETS_LIB


#endif // QSTYLEHELPER_HPP
