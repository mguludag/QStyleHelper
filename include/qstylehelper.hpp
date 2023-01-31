#ifndef QSTYLEHELPER_HPP
#define QSTYLEHELPER_HPP
#include <QtGlobal>

#include <QProcess>
#include <QSettings>
#include <QBasicTimer>

#ifdef QT_WIDGETS_LIB
#include <QApplication>
#include <QStyle>
#include <QStyleFactory>
#include <QPalette>
#include <QToolTip>
#endif


#if defined(Q_OS_WIN) && QT_VERSION_MAJOR == 5 && QT_VERSION_MINOR <= 15
#include <dwmapi.h>

#pragma comment(lib, "Dwmapi.lib")
#endif


class QStyleHelper : public QObject
{
    Q_OBJECT
public:
    static QStyleHelper &instance();
    ~QStyleHelper();

    static void setTitleBarDarkColor();
    static void setTitleBarDarkColor(QWidget& window);

#ifdef QT_WIDGETS_LIB
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
        static constexpr QColor lightGray{190, 190, 190};
        static constexpr QColor gray{164, 166, 168};
        static constexpr QColor midDarkGray{68, 68, 68};
        static constexpr QColor darkGray{53, 53, 53};
        static constexpr QColor black{25, 25, 25};
        static constexpr QColor blue{42, 130, 218};
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
#ifdef Q_OS_WINDOWS
    qputenv("QT_QPA_PLATFORM", "windows:darkmode=1");
#endif // Q_OS_WINDOWS
}
#include <QDebug>
inline void QStyleHelper::setTitleBarDarkColor(QWidget &window)
{
#if defined(Q_OS_WIN) && QT_VERSION_MAJOR == 5 && QT_VERSION_MINOR <= 15
    auto hwnd = window.winId();
    const BOOL darkBorder = TRUE;
    DwmSetWindowAttribute((HWND)hwnd, 19, &darkBorder, sizeof(darkBorder));
#endif
}

#ifdef QT_WIDGETS_LIB
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
#ifdef QT_WIDGETS_LIB
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


#ifdef QT_WIDGETS_LIB
inline void QStyleHelper::QPaletteHelper::setPalette(bool dark)
{
    QPalette palette{dark ? mCustomDarkPalette : mCustomLightPalette};

    mIsDarkPalette = dark;

    QApplication::setPalette(palette);
    QToolTip::setPalette(palette);

    auto borderBrush = (palette == QApplication::style()->standardPalette() ?
                            palette.midlight() : palette.mid());
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

inline QStyleHelper::QPaletteHelper::QPaletteHelper() : mCustomDarkPalette(QApplication::style()->standardPalette()),
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

#endif


#endif // QSTYLEHELPER_HPP
