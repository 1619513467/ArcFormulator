#ifndef DESKTOPFRAMEWORK_QDFRIBBONSTYLE_H
#define DESKTOPFRAMEWORK_QDFRIBBONSTYLE_H

#include <QStylePlugin>
#include <qdf_global.h>
#include <ribbon/qdfofficestyle.h>
#include <ribbon/qdfribbonbar.h>

QDF_BEGIN_NAMESPACE

class QdfRibbonStylePrivate;
class QDF_EXPORT QdfRibbonStyle : public QdfOfficeStyle
{
    Q_OBJECT
    Q_PROPERTY(bool activeTabAccented READ isActiveTabAccented WRITE setActiveTabAccented)
public:
    enum QPixelMetric
    {
        PM_RibbonReducedGroupWidth = PM_CustomBase + 1,
        PM_RibbonHeightCaptionGroup,
    };

    enum QPrimitiveElement
    {
        PE_RibbonPopupBarButton = PE_RibbonCustomBase + 1,
        PE_RibbonFileButton,
        PE_RibbonOptionButton,
        PE_RibbonGroupScrollButton,
        PE_RibbonSliderButton,
        PE_RibbonTab,
        PE_RibbonContextHeaders,
        PE_RibbonQuickAccessButton,
        PE_RibbonFillRect,
        PE_RibbonRect,
        PE_RibbonKeyTip,
        PE_Backstage,
        PE_RibbonBackstageCloseButton,
    };

    enum QContentsType
    {
        CT_RibbonSliderButton = CT_CustomBase + 1,
        CT_RibbonGroupButton,
    };

    enum QControlElement
    {
        CE_RibbonTabShapeLabel = CE_CustomBase + 1,
        CE_RibbonFileButtonLabel,
        CE_RibbonBar,
        CE_RibbonTabBar,
        CE_RibbonGroups,
        CE_Group,
        CE_ReducedGroup,
        CE_PopupSizeGrip,
    };
    enum QStyleHint
    {
        SH_FlatFrame = SH_CustomBase + 1,
        SH_RibbonBackstageHideTabs,
        SH_RibbonItemUpperCase,
    };

public:
    QdfRibbonStyle();
    QdfRibbonStyle(QMainWindow *mainWindow);
    virtual ~QdfRibbonStyle();

public:
    bool isActiveTabAccented() const;
    void setActiveTabAccented(bool accented);
    QFont font(const QWidget *widget) const;

public:
    virtual void polish(QApplication *);
    virtual void unpolish(QApplication *);
    virtual void polish(QWidget *widget);
    virtual void unpolish(QWidget *widget);

    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter,
                               const QWidget *widget = 0) const;
    virtual void drawControl(ControlElement element, const QStyleOption *opt, QPainter *p,
                             const QWidget *w = 0) const;
    virtual void drawComplexControl(ComplexControl cc, const QStyleOptionComplex *option, QPainter *p,
                                    const QWidget *widget = 0) const;

    virtual int pixelMetric(PixelMetric pm, const QStyleOption *option = 0,
                            const QWidget *widget = 0) const;
    virtual int styleHint(StyleHint hint, const QStyleOption *opt = nullptr,
                          const QWidget *widget = nullptr, QStyleHintReturn *returnData = nullptr) const;
    virtual QPixmap standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt = 0,
                                   const QWidget *widget = 0) const;
    virtual QSize sizeFromContents(ContentsType ct, const QStyleOption *opt, const QSize &csz,
                                   const QWidget *widget) const;
    virtual QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc,
                                 const QWidget *widget) const;
    virtual QRect subElementRect(SubElement sr, const QStyleOption *opt, const QWidget *widget) const;

protected:
    // for QForm
    virtual bool drawFrame(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawShapedFrame(const QStyleOption *, QPainter *, const QWidget *) const;
    // for stausBar
    virtual bool drawPanelStatusBar(const QStyleOption *, QPainter *, const QWidget *) const;
    // for QdfRibbonBar
    virtual void drawRibbonBar(const QStyleOption *option, QPainter *p, const QWidget *widget) const;
    virtual void drawRibbonTabBar(const QStyleOption *opt, QPainter *p, const QWidget *w) const;
    virtual void drawRibbonGroups(const QStyleOption *option, QPainter *p, const QWidget *widget) const;
    virtual void drawGroup(const QStyleOption *option, QPainter *p, const QWidget *widget) const;
    virtual void drawReducedGroup(const QStyleOption *option, QPainter *p, const QWidget *widget) const;
    // for SizeGrip
    virtual bool drawSizeGrip(const QStyleOption *, QPainter *, const QWidget *) const;
    // for TitleBar
    virtual bool drawIndicatorToolBarSeparator(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawTitleBar(const QStyleOptionComplex *, QPainter *, const QWidget *) const;
    virtual void drawContextHeaders(const QStyleOption *opt, QPainter *p) const;

    // for Menu
    virtual bool drawFrameMenu(const QStyleOption *opt, QPainter *p, const QWidget *w) const;
    // for Slider
    virtual bool drawSlider(const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const;
    // for file menu
    virtual void drawSystemButton(const QStyleOption *option, QPainter *p, const QWidget *widget) const;
    virtual void drawOptionButton(const QStyleOption *option, QPainter *p, const QWidget *widget) const;
    virtual void drawGroupScrollButton(const QStyleOption *option, QPainter *p,
                                       const QWidget *widget) const;
    virtual void drawFileButtonLabel(const QStyleOption *option, QPainter *p,
                                     const QWidget *widget) const;
    virtual void drawPopupSystemButton(const QStyleOption *option, QPainter *p,
                                       const QWidget *widget) const;
    virtual void drawQuickAccessButton(const QStyleOption *option, QPainter *p,
                                       const QWidget *widget) const;
    virtual void drawPopupResizeGripper(const QStyleOption *option, QPainter *p,
                                        const QWidget *widget) const;
    virtual bool drawMenuItem(const QStyleOption *, QPainter *, const QWidget *) const;
    // for ribbon tab
    virtual void drawTabShape(const QStyleOption *opt, QPainter *p, const QWidget *widget) const;
    virtual void drawTabShapeLabel(const QStyleOption *opt, QPainter *p, const QWidget *widget) const;
    // for TabBar
    virtual bool drawTabBarTabShape(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawTabBarTabLabel(const QStyleOption *, QPainter *, const QWidget *) const;
    // for ToolBar
    virtual bool drawToolBar(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawPanelButtonTool(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawToolButtonLabel(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawToolButton(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawGroupControlEntry(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual void drawItemText(QPainter *painter, const QRect &rect, int flags, const QPalette &pal,
                              bool enabled, const QString &text,
                              QPalette::ColorRole textRole = QPalette::NoRole) const;
    // for BackstageButton
    // virtual void drawPanelBackstageButton(const QStyleOption*, QPainter*, const QWidget*) const;

    virtual bool drawIndicatorArrow(PrimitiveElement, const QStyleOption *, QPainter *,
                                    const QWidget *) const;
    virtual void drawRectangle(QPainter *p, const QRect &rect, bool selected, bool pressed, bool enabled,
                               bool checked, bool popuped, BarType barType, BarPosition barPos) const;
    virtual void drawSplitButtonPopup(QPainter *p, const QRect &rect, bool selected, bool enabled,
                                      bool popuped) const;

    void drawFillRect(const QStyleOption *opt, QPainter *p, const QWidget *widget) const;
    void drawRect(const QStyleOption *opt, QPainter *p, const QWidget *widget) const;
    void drawKeyTip(const QStyleOption *opt, QPainter *p, const QWidget *widget) const;
    void drawBackstage(const QStyleOption *opt, QPainter *p, const QWidget *widget) const;
    void drawRibbonBackstageCloseButton(const QStyleOption *opt, QPainter *p,
                                        const QWidget *widget) const;
    void drawRibbonSliderButton(const QStyleOption *opt, QPainter *p, const QWidget *widget) const;

protected:
    void calcRects(const QRect &rc, const QSize &pmSize, const QFontMetrics &fMetric,
                   const QString &text, bool hasMenu, QRect *pmRect, QRect *strFirsRect,
                   QRect *strSecondRect) const;
    virtual bool showToolTip(const QPoint &pos, QWidget *w);

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event);

private:
    friend class QdfRibbonPaintManager;
    QDF_DECLARE_EX_PRIVATE(QdfRibbonStyle)
    Q_DISABLE_COPY(QdfRibbonStyle)
};

class QDF_EXPORT QdfRibbonStylePlugin : public QStylePlugin
{
    Q_OBJECT
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#endif
public:
    QStringList keys() const;
    QStyle *create(const QString &key);
};


QDF_END_NAMESPACE

#endif//DESKTOPFRAMEWORK_QDFRIBBONSTYLE_H