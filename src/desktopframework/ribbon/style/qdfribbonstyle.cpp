#include <QApplication>
#include <QBitmap>
#include <QLabel>
#include <QMdiArea>
#include <QPaintEngine>
#include <QPainter>
#include <QTabBar>
#include <qevent.h>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    #include <qdrawutil.h>
#endif /* Q_OS_WIN*/

#ifdef Q_OS_WIN
    #include <qt_windows.h>
    #include <wingdi.h>
#endif /* Q_OS_WIN*/

#include "../private/qdfribbon_p.h"
#include "../private/qdfribbonbar_p.h"
#include "../private/qdfribbonbutton_p.h"
#include "../private/qdfribbongroup_p.h"
#include "../qdfribbon_def.h"
#include "../qdfribbontabbar.h"
#include "qdfcommonstyle_p.h"
#include "qdfofficeframehelper.h"
#include "qdfribbonstyle_p.h"
#include <qdfmainwindow.h>
#include <ribbon/qdfofficepopupmenu.h>
#include <ribbon/qdfribbonbackstageview.h>
#include <ribbon/qdfribbongallery.h>
#include <ribbon/qdfribbongroup.h>
#include <ribbon/qdfribbonquickaccessbar.h>
#include <ribbon/qdfribbonstatusbar.h>
#include <ribbon/qdfribbonstyle.h>
#include <ribbon/qdfribbonsystempopupbar.h>
#include <ribbon/qdfribbontooltip.h>
#include <ribbon/qdfstylehelpers.h>

#if defined(__GNUC__)
    #pragma GCC diagnostic ignored "-Wswitch"
#endif

QDF_USE_NAMESPACE

static const int windowsSepHeight = 9;      // height of the separator
static const int windowsItemFrame = 2;      // menu item frame width
static const int windowsItemHMargin = 3;    // menu item hor text margin
static const int windowsCheckMarkWidth = 16;// checkmarks width on windows
static const int windowsItemVMargin = 2;    // menu item ver text margin
static const int windowsRightBorder = 15;   // right border on windows
static const int windowsTabSpacing = 12;    // space between text and tab
static const int windowsArrowHMargin = 6;   // arrow horizontal margin
static const int splitActionPopupWidth = 20;// Split button drop down width in popups

#ifdef Q_OS_WIN
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
QT_BEGIN_NAMESPACE
Q_GUI_EXPORT QPixmap qt_pixmapFromWinHICON(HICON icon);
QT_END_NAMESPACE
    #endif// QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#endif    // Q_OS_WIN

static bool use2000style = true;

/* QdfRibbonStylePrivate */
QdfRibbonStylePrivate::QdfRibbonStylePrivate()
{
    m_fileButtonImageCount = 3;
    m_flatFrame = false;
    m_destroyKeyTips = false;
    m_completeKey = false;
    m_blockKeyTips = false;
    m_isActiveTabAccented = false;
}

#ifdef Q_OS_WIN
const double g_logPixel = 96.0;
static QFont qtc_LOGFONTtoQFont(LOGFONT &lf, bool dpiaware)
{
    QString family = QString(lf.lfFaceName);
    QFont qf(family);
    qf.setItalic(lf.lfItalic);
    if (lf.lfWeight != FW_DONTCARE)
    {
        int weight;
        if (lf.lfWeight < 400)
        {
            weight = QFont::Light;
        }
        else if (lf.lfWeight < 600)
        {
            weight = QFont::Normal;
        }
        else if (lf.lfWeight < 700)
        {
            weight = QFont::DemiBold;
        }
        else if (lf.lfWeight < 800)
        {
            weight = QFont::Bold;
        }
        else
        {
            weight = QFont::Black;
        }
        qf.setWeight(weight);
    }
    int lfh = qAbs(lf.lfHeight);

    double factor = 64.0;
    if (QSysInfo::windowsVersion() <= QSysInfo::WV_XP)
    {
        factor = 72.0;
    }

    HDC displayDC = GetDC(0);
    double currentlogPixel = (double) GetDeviceCaps(displayDC, LOGPIXELSY);
    double delta = g_logPixel / currentlogPixel;
    double scale = factor * (dpiaware ? 1 : delta);

    qf.setPointSizeF((double) lfh * scale / currentlogPixel);

    ReleaseDC(0, displayDC);

    qf.setUnderline(false);
    qf.setOverline(false);
    qf.setStrikeOut(false);

    return qf;
}
#endif

void QdfRibbonStylePrivate::initialization()
{
    QDF_Q(QdfRibbonStyle);
    updateColors();
    QFont fontRibbon = q->font(0);
    QApplication::setFont(fontRibbon, "QRibbon::QdfRibbonBar");
}

void QdfRibbonStylePrivate::updateColors()
{
    QDF_Q(QdfRibbonStyle);

    m_clrFileButtonText = QColor(255, 255, 255);
    m_clrRibbonGrayText = m_clrMenuBarGrayText;

    QdfStyleHelper &helper = q->helper();

    // [Ribbon]
    m_lineWidthTop = helper.getInteger("Ribbon", "LineWidthTop", 1);
    m_clrRibbonGroupCaptionText = helper.getColor("Ribbon", "GroupCaptionText");
    m_clrRibbonFace = helper.getColor("Ribbon", "RibbonFace");
    m_clrRibbonTabBarBackground = helper.getColor("Ribbon", "RibbonTabBarBackground");
    m_clrRibbonTabBarText = helper.getColor("Ribbon", "RibbonTabBarText");
    m_clrRibbonSeparator = helper.getColor("Ribbon", "RibbonSeparator");
    m_clrRibbonLight = helper.getColor("Ribbon", "RibbonLight");

    m_clrRibbonText = helper.getColor("Ribbon", "RibbonText");
    m_clrGroupClientText = helper.getColor("Ribbon", "GroupClientText");
    m_clrGroupClientGrayText = QColor(141, 141, 141);
    m_clrRibbonInactiveFace = helper.getColor("Ribbon", "RibbonInactiveFace", m_clrRibbonFace);

    m_clrMinimizedFrameEdgeHighLight = helper.getColor("Ribbon", "MinimizedFrameEdgeHighLight");
    m_clrMinimizedFrameEdgeShadow = helper.getColor("Ribbon", "MinimizedFrameEdgeShadow");
    m_flatFrame = (bool) helper.getInteger("Window", "FlatFrame", 0);
    m_fileButtonImageCount = helper.getInteger("Ribbon", "FileButtonImageCount", 3);

    m_paintManager->modifyColors();
}

#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
void QdfRibbonStylePrivate::tabLayout(const QStyleOptionTabV3 *opt, const QWidget *widget,
                                      QRect *textRect, QRect *iconRect) const
#else
void QdfRibbonStylePrivate::tabLayout(const QStyleOptionTab *opt, const QWidget *widget, QRect *textRect,
                                      QRect *iconRect) const
#endif
{
    QDF_Q(const QdfRibbonStyle);
    const QStyle *proxyStyle = q->proxy();

    Q_ASSERT(textRect);
    Q_ASSERT(iconRect);
    QRect tr = opt->rect;
    bool verticalTabs = opt->shape == QTabBar::RoundedEast || opt->shape == QTabBar::RoundedWest ||
                        opt->shape == QTabBar::TriangularEast || opt->shape == QTabBar::TriangularWest;

    bool botton = opt->shape == QTabBar::RoundedSouth;

    if (verticalTabs)
    {
        tr.setRect(0, 0, tr.height(), tr.width());// 0, 0 as we will have a translate transform
    }

    int verticalShift = proxyStyle->pixelMetric(QStyle::PM_TabBarTabShiftVertical, opt, widget);
    int horizontalShift = proxyStyle->pixelMetric(QStyle::PM_TabBarTabShiftHorizontal, opt, widget);

    int xShift = botton ? 0 : 5;
    int hpadding = proxyStyle->pixelMetric(QStyle::PM_TabBarTabHSpace, opt, widget) / 2 - xShift;
    int vpadding = proxyStyle->pixelMetric(QStyle::PM_TabBarTabVSpace, opt, widget) / 2;

    if (opt->shape == QTabBar::RoundedSouth || opt->shape == QTabBar::TriangularSouth)
    {
        verticalShift = -verticalShift;
    }

    tr.adjust(hpadding, verticalShift - vpadding, horizontalShift - hpadding, vpadding);

    bool selected = opt->state & QStyle::State_Selected;
    if (selected)
    {
        tr.setTop(tr.top() - verticalShift);
        tr.setRight(tr.right() - horizontalShift);
    }

    // left widget
    if (!opt->leftButtonSize.isEmpty())
    {
        tr.setLeft(tr.left() + 4 +
                   (verticalTabs ? opt->leftButtonSize.height() : opt->leftButtonSize.width()));
    }
    // right widget
    if (!opt->rightButtonSize.isEmpty())
    {
        tr.setRight(tr.right() - 4 -
                    (verticalTabs ? opt->rightButtonSize.height() : opt->rightButtonSize.width()));
    }

    // icon
    if (!opt->icon.isNull())
    {
        QSize iconSize = opt->iconSize;
        if (!iconSize.isValid())
        {
            int iconExtent = proxyStyle->pixelMetric(QStyle::PM_SmallIconSize);
            iconSize = QSize(iconExtent, iconExtent);
        }
        QSize tabIconSize = opt->icon.actualSize(
                iconSize, (opt->state & QStyle::State_Enabled) ? QIcon::Normal : QIcon::Disabled,
                (opt->state & QStyle::State_Selected) ? QIcon::On : QIcon::Off);

        *iconRect = QRect(tr.left(), tr.center().y() - tabIconSize.height() / 2, tabIconSize.width(),
                          tabIconSize.height());

        xShift = botton ? 6 : 0;

        if (!verticalTabs && botton &&
            !((opt->position == QStyleOptionTab::OnlyOneTab ||
               opt->position == QStyleOptionTab::Beginning)))
        {
            xShift = 0;
        }

        if (opt->shape == QTabBar::RoundedEast && opt->position != QStyleOptionTab::OnlyOneTab &&
            opt->position != QStyleOptionTab::Beginning)
        {
            //            iconRect->translate(xShift-11, 0);
        }
        else
        {
            iconRect->translate(xShift, 0);
        }

        if (!verticalTabs)
        {
            *iconRect = proxyStyle->visualRect(opt->direction, opt->rect, *iconRect);
        }

        xShift = botton ? /*2 +*/ tabIconSize.width() : 4;

        if (!verticalTabs && botton &&
            !((opt->position == QStyleOptionTab::OnlyOneTab ||
               opt->position == QStyleOptionTab::Beginning)))
        {
            xShift -= 7;
        }

        tr.translate(xShift, 0);
    }

    if (!verticalTabs)
    {
        tr = proxyStyle->visualRect(opt->direction, opt->rect, tr);
    }

    *textRect = tr;
}

void QdfRibbonStylePrivate::tabHoverUpdate(QTabBar *tabBar, QEvent *event)
{
    if (event->type() == QEvent::HoverMove || event->type() == QEvent::HoverEnter)
    {
        QHoverEvent *he = static_cast<QHoverEvent *>(event);
        int index = tabBar->tabAt(he->pos());
        if (index != -1)
        {
            QRect rect = tabBar->tabRect(index);
            switch (tabBar->shape())
            {
                case QTabBar::RoundedNorth:
                case QTabBar::TriangularNorth:
                    rect.adjust(0, 0, 11, 0);
                    break;
                case QTabBar::RoundedSouth:
                case QTabBar::TriangularSouth:
                    //                    if (tab->position != QStyleOptionTab::Beginning)
                    rect.adjust(-11, 0, 0, 0);
                    break;
                case QTabBar::RoundedWest:
                case QTabBar::TriangularWest:
                    rect.adjust(0, -11, 0, 0);
                    break;
                case QTabBar::RoundedEast:
                case QTabBar::TriangularEast:
                    rect.adjust(0, -11, 0, 0);
                    break;
                default:
                    break;
            }

            m_oldHoverRectTab = rect;
            tabBar->update(rect);
        }
    }
    else if (event->type() == QEvent::HoverLeave)
    {
        tabBar->update(m_oldHoverRectTab);
        m_oldHoverRectTab = QRect();
    }
}

void QdfRibbonStylePrivate::setMacSmallSize(QWidget *widget)
{
    m_macSmallSizeWidgets.insert(widget, widget->testAttribute(Qt::WA_MacSmallSize));
    widget->setAttribute(Qt::WA_MacSmallSize, true);
}

void QdfRibbonStylePrivate::unsetMacSmallSize(QWidget *widget)
{
    if (m_macSmallSizeWidgets.contains(widget))
    {
        bool testMacSmallSize = m_macSmallSizeWidgets.value(widget);
        widget->setAttribute(Qt::WA_MacSmallSize, testMacSmallSize);
        m_macSmallSizeWidgets.remove(widget);
    }
}

QdfRibbonPaintManagerInterface *QdfRibbonStylePrivate::ribbonPaintManager() const
{
    QDF_Q(const QdfRibbonStyle);
    QdfRibbonPaintManagerInterface *ribbonPaintManager =
            qobject_cast<QdfRibbonPaintManagerInterface *>(q->paintManager());
    return ribbonPaintManager;
}

void QdfRibbonStylePrivate::makePaintManager()
{
    QDF_Q(QdfRibbonStyle)

    setPaintManager(*new QdfRibbonPaintManager(q));
}

QdfRibbonStyle::QdfRibbonStyle() : QdfOfficeStyle(new QdfRibbonStylePrivate)
{
    QDF_D(QdfRibbonStyle);
    d->initialization();
}

QdfRibbonStyle::QdfRibbonStyle(QMainWindow *mainWindow)
    : QdfOfficeStyle(mainWindow, new QdfRibbonStylePrivate)
{
    QDF_D(QdfRibbonStyle);
    d->initialization();
}

QdfRibbonStyle::~QdfRibbonStyle()
{
}

bool QdfRibbonStyle::isActiveTabAccented() const
{
    QDF_D(const QdfRibbonStyle);
    return d->m_isActiveTabAccented;
}

void QdfRibbonStyle::setActiveTabAccented(bool accented)
{
    QDF_D(QdfRibbonStyle);
    if (d->m_isActiveTabAccented == accented)
    {
        return;
    }
    d->m_isActiveTabAccented = accented;
    d->refreshMetrics();
}

QFont QdfRibbonStyle::font(const QWidget *widget) const
{
    if (qobject_cast<const QdfRibbonBar *>(widget) || widget == nullptr)
    {
#ifdef Q_OS_WIN
        NONCLIENTMETRICS ncm;
        ncm.cbSize = FIELD_OFFSET(NONCLIENTMETRICS, lfMessageFont) + sizeof(LOGFONT);
        ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
        return qtc_LOGFONTtoQFont(ncm.lfMenuFont, isDPIAware());
#else
        QMenuBar menu;
        menu.setAttribute(Qt::WA_MacSmallSize);
        QFont fontRegular = QApplication::font(&menu);

        double scale = isDPIAware() ? 1 / QdfDrawHelpers::dpiScaled(1.0) : 1;
        fontRegular.setPointSizeF(fontRegular.pointSizeF() * scale);
        return fontRegular;
#endif
    }
    else
    {
        return QApplication::font(widget);
    }
}

void QdfRibbonStyle::polish(QApplication *application)
{
    QdfOfficeStyle::polish(application);

    if (application)
    {
        application->installEventFilter(this);
    }

    QDF_D(QdfRibbonStyle);
    d->updateColors();
}

void QdfRibbonStyle::unpolish(QApplication *application)
{
    QdfOfficeStyle::unpolish(application);

    if (application)
    {
        application->removeEventFilter(this);
    }
}

void QdfRibbonStyle::polish(QWidget *widget)
{
    QdfOfficeStyle::polish(widget);

    if (isNativeDialog(widget))
    {
        return;
    }

    QDF_D(QdfRibbonStyle);
    if (qobject_cast<QdfRibbonBar *>(widget))
    {
        d->setMacSmallSize(widget);
        QPalette palette = widget->palette();
        palette.setColor(QPalette::Light, d->m_clrMenuBarGrayText);
        palette.setColor(QPalette::ButtonText, d->m_clrRibbonText);
        widget->setPalette(palette);
    }

    if (qobject_cast<QStatusBar *>(widget))
    {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::Light, d->m_clrMenuBarGrayText);
        palette.setColor(QPalette::ButtonText, d->m_clrStatusBarText);
        widget->setPalette(palette);
    }

    if (qobject_cast<QdfRibbonGroup *>(widget))
    {
        QPalette palette = widget->palette();

        palette.setColor(QPalette::Light, d->m_clrMenuBarGrayText);
        palette.setColor(QPalette::WindowText, d->m_clrRibbonGroupCaptionText);
        palette.setColor(QPalette::ButtonText, d->m_clrGroupClientText);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, d->m_clrGroupClientGrayText);

        widget->setPalette(palette);
    }
    else if (qobject_cast<QdfRibbonGroupPopupButton *>(widget))
    {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::Light, d->m_clrToolBarGrayText);
        palette.setColor(QPalette::WindowText, d->m_clrToolBarText);
        widget->setPalette(palette);
    }
    else if ((qobject_cast<QdfRibbonGroup *>(widget->parentWidget()) &&
              qobject_cast<QToolButton *>(widget)))
    {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::Light, d->m_clrToolBarGrayText);
        palette.setColor(QPalette::WindowText, d->m_clrToolBarText);
        widget->setPalette(palette);
    }
    else if (qobject_cast<QdfRibbonSystemPopupBar *>(widget->parentWidget()) &&
             qobject_cast<QToolButton *>(widget))
    {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::Light, d->m_clrMenuBarGrayText);
        palette.setColor(QPalette::ButtonText, d->m_clrMenuPopupText);
        widget->setPalette(palette);
    }
    else if (qobject_cast<QdfRibbonSystemButton *>(widget))
    {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::ButtonText, d->m_clrFileButtonText);
        widget->setPalette(palette);
    }
    else if (qobject_cast<QdfOfficePopupMenu *>(widget->parentWidget()) &&
             qobject_cast<QLabel *>(widget))
    {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::WindowText,
                         getTextColor(false, false, true, false, false, TypePopup, BarNone));
        widget->setPalette(palette);
    }
    else if (qobject_cast<QTabBar *>(widget) && qobject_cast<QMdiArea *>(widget->parentWidget()))
    {
        widget->setAutoFillBackground(true);

        QPalette palette = widget->palette();
        palette.setColor(QPalette::Background, helper().getColor("TabManager", "AccessHeader"));
        widget->setPalette(palette);

        ((QTabBar *) widget)->setExpanding(false);
        widget->installEventFilter(this);
    }
    d->ribbonPaintManager()->setupPalette(widget);
}

void QdfRibbonStyle::unpolish(QWidget *widget)
{
    QdfOfficeStyle::unpolish(widget);
    QDF_D(QdfRibbonStyle);

    if (getParentWidget<QdfRibbonBar>(widget) || qobject_cast<QdfRibbonBar *>(widget))
    {
        d->unsetMacSmallSize(widget);
    }

    if (widget == nullptr)
    {
        return;
    }

    if (static_cast<QdfRibbonBar *>((QdfRibbonBar *) widget))
    {
        widget->removeEventFilter(this);
    }

    if (qobject_cast<QTabBar *>(widget) && qobject_cast<QMdiArea *>(widget->parentWidget()))
    {
        widget->removeEventFilter(this);
        ((QTabBar *) widget)->setExpanding(true);
    }
}

void QdfRibbonStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                                   QPainter *painter, const QWidget *widget) const
{
    switch ((QPrimitiveElement) element)
    {
        case PE_RibbonPopupBarButton:
            drawPopupSystemButton(option, painter, widget);
            break;
        case PE_RibbonFileButton:
            drawSystemButton(option, painter, widget);
            break;
        case PE_RibbonOptionButton:
            drawOptionButton(option, painter, widget);
            break;
        case PE_RibbonGroupScrollButton:
            drawGroupScrollButton(option, painter, widget);
            break;
        case PE_RibbonQuickAccessButton:
            drawQuickAccessButton(option, painter, widget);
            break;
        case PE_RibbonTab:
            drawTabShape(option, painter, widget);
            break;
        case PE_RibbonContextHeaders:
            drawContextHeaders(option, painter);
            break;
        case PE_RibbonFillRect:
            drawFillRect(option, painter, widget);
            break;
        case PE_RibbonRect:
            drawRect(option, painter, widget);
            break;
        case PE_RibbonKeyTip:
            drawKeyTip(option, painter, widget);
            break;
        case PE_Backstage:
            drawBackstage(option, painter, widget);
            break;
        case PE_RibbonBackstageCloseButton:
            drawRibbonBackstageCloseButton(option, painter, widget);
            break;
        case PE_RibbonSliderButton:
            drawRibbonSliderButton(option, painter, widget);
            break;
        default:
            QdfOfficeStyle::drawPrimitive(element, option, painter, widget);
    }
}

void QdfRibbonStyle::drawControl(ControlElement element, const QStyleOption *opt, QPainter *p,
                                 const QWidget *widget) const
{
    switch (element)
    {
        case CE_RibbonTabShapeLabel:
            drawTabShapeLabel(opt, p, widget);
            break;
        case CE_RibbonBar:
            drawRibbonBar(opt, p, widget);
            break;
        case CE_RibbonTabBar:
            drawRibbonTabBar(opt, p, widget);
            break;
        case CE_RibbonFileButtonLabel:
            drawFileButtonLabel(opt, p, widget);
            break;
        case CE_RadioButtonLabel:
        case CE_CheckBoxLabel:
            {
                if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt))
                {
                    uint alignment = visualAlignment(btn->direction, Qt::AlignLeft | Qt::AlignVCenter);
                    if (!proxy()->styleHint(SH_UnderlineShortcut, btn, widget))
                    {
                        alignment |= Qt::TextHideMnemonic;
                    }
                    QPixmap pix;
                    QRect textRect = btn->rect;
                    if (!btn->icon.isNull())
                    {
                        pix = btn->icon.pixmap(btn->iconSize, btn->state & State_Enabled
                                                                      ? QIcon::Normal
                                                                      : QIcon::Disabled);
                        proxy()->drawItemPixmap(p, btn->rect, alignment, pix);
                        if (btn->direction == Qt::RightToLeft)
                        {
                            textRect.setRight(textRect.right() - btn->iconSize.width() - 4);
                        }
                        else
                        {
                            textRect.setLeft(textRect.left() + btn->iconSize.width() + 4);
                        }
                    }
                    if (!btn->text.isEmpty())
                    {
                        proxy()->drawItemText(p, textRect, alignment | Qt::TextShowMnemonic,
                                              btn->palette, btn->state & State_Enabled, btn->text,
                                              QPalette::ButtonText);
                    }
                }
                break;
            }
        case CE_RibbonGroups:
            drawRibbonGroups(opt, p, widget);
            break;
        case CE_Group:
            drawGroup(opt, p, widget);
            break;
        case CE_ReducedGroup:
            drawReducedGroup(opt, p, widget);
            break;
        case CE_PopupSizeGrip:
            drawPopupResizeGripper(opt, p, widget);
            break;
        default:
            QdfOfficeStyle::drawControl(element, opt, p, widget);
            break;
    }
}

void QdfRibbonStyle::drawComplexControl(ComplexControl cc, const QStyleOptionComplex *option,
                                        QPainter *painter, const QWidget *widget) const
{
    QdfOfficeStyle::drawComplexControl(cc, option, painter, widget);
}

int QdfRibbonStyle::pixelMetric(PixelMetric pm, const QStyleOption *option, const QWidget *widget) const
{
    int ret = 0;
    switch (pm)
    {
        case PM_ButtonIconSize:
        case PM_SmallIconSize:
            {
                if (widget &&
                    (qobject_cast<const QdfRibbonStatusBar *>(widget->parentWidget()) ||
                     qobject_cast<const QdfRibbonStatusBarSwitchGroup *>(widget->parentWidget())))
                {
                    ret = int(QdfDrawHelpers::dpiScaled(14.));
                }
                else
                {
                    ret = QdfOfficeStyle::pixelMetric(pm, option, widget);
                }
            }
            break;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        case PM_MenuBarPanelWidth:
            ret = 0;
            break;
#endif
        case PM_MenuBarVMargin:
            {
                ret = QdfOfficeStyle::pixelMetric(pm, option, widget);
                if (!option)
                {
                    if (const QdfRibbonBar *ribbonBar = qobject_cast<const QdfRibbonBar *>(widget))
                    {
                        ret += ribbonBar->qdf_d()->topBorder() + ribbonBar->titleBarHeight() + 2;
                    }
                }
            }
            break;

        case PM_MenuBarHMargin:
            ret = 2;
            break;

        case PM_TabBarTabHSpace:
            {
                ret = QdfOfficeStyle::pixelMetric(pm, option, widget);

                if (widget && qobject_cast<QMdiArea *>(widget->parentWidget()))
                {
                    if (const QStyleOptionTab *taOption =
                                qstyleoption_cast<const QStyleOptionTab *>(option))
                    {
                        bool verticalTabs = taOption->shape == QTabBar::RoundedEast ||
                                            taOption->shape == QTabBar::RoundedWest ||
                                            taOption->shape == QTabBar::TriangularEast ||
                                            taOption->shape == QTabBar::TriangularWest;

                        bool botton = taOption->shape == QTabBar::RoundedSouth;
                        if (!verticalTabs && botton)
                        {
                            if (taOption->icon.isNull() &&
                                (taOption->position == QStyleOptionTab::OnlyOneTab ||
                                 taOption->position == QStyleOptionTab::Beginning))
                            {
                                ret += 11;
                            }
                            else if (!taOption->icon.isNull() &&
                                     !(taOption->position == QStyleOptionTab::OnlyOneTab ||
                                       taOption->position == QStyleOptionTab::Beginning))
                            {
                                ret -= 11;
                            }
                        }
                    }
                }
            }
            break;
        case PM_RibbonReducedGroupWidth:
            if (const QStyleOptionToolButton *optBut =
                        qstyleoption_cast<const QStyleOptionToolButton *>(option))
            {
                QString strFirstRow, strSecondRow;
                QdfCommonStyle::splitString(optBut->text, strFirstRow, strSecondRow);
                bool towRow = !strFirstRow.isEmpty() && !strSecondRow.isEmpty();
                if (strFirstRow.count() < strSecondRow.count())
                {
                    strFirstRow = strSecondRow;
                }

#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
                ret = optBut->fontMetrics.boundingRect(strFirstRow).width() +
                      optBut->fontMetrics.width(QLatin1Char(' ')) * 4;
#else
                ret = optBut->fontMetrics.boundingRect(strFirstRow).width() +
                      optBut->fontMetrics.horizontalAdvance(QLatin1Char(' ')) * 4;
#endif
                if (towRow)
                {
                    ret = ret + 16;
                }

                QPixmap soCenter = cached("RibbonGroupButton.png");
                QRect rcSrc = sourceRectImage(soCenter.rect(), 0, 3);
                ret = qMax(ret, rcSrc.width());
            }
            break;
        case PM_RibbonHeightCaptionGroup:
            if (const QStyleOptionGroupBox *optGroup =
                        qstyleoption_cast<const QStyleOptionGroupBox *>(option))
            {
                bool titleGroupsVisible =
                        widget ? widget->property(_qdf_TitleGroupsVisible).toBool() : false;
                ret = titleGroupsVisible ? option->fontMetrics.height() + 3 : 0;
                if (titleGroupsVisible)
                {
                    QSize textSize = optGroup->fontMetrics.size(Qt::TextShowMnemonic, optGroup->text);
                    if (ret < textSize.height())
                    {
                        ret = textSize.height();
                    }
                }
                break;
            }
            // fallthrough
        case PM_TitleBarHeight:
            {
                ret = QdfOfficeStyle::pixelMetric(pm, option, widget);
                break;
            }
        case PM_MenuHMargin:
            {
                if (qobject_cast<const QdfRibbonSystemPopupBar *>(widget))
                {
                    ret = 6;
                }
                else if (qobject_cast<const QdfOfficePopupMenu *>(widget))
                {
                    ret = -1;
                }
                else
                {
                    ret = QdfOfficeStyle::pixelMetric(pm, option, widget);
                }
                break;
            }
        case PM_MenuVMargin:
            {
                if (qobject_cast<const QdfRibbonSystemPopupBar *>(widget))
                {
                    ret = 19;
                }
                else if (qobject_cast<const QdfRibbonBackstageView *>(widget))
                {
                    ret = 7;
                }
                else if (qobject_cast<const QdfOfficePopupMenu *>(widget))
                {
                    ret = 0;
                }
                else
                {
                    ret = QdfOfficeStyle::pixelMetric(pm, option, widget);
                }
                break;
            }
        case PM_ToolBarIconSize:
            {
                if (isDPIAware() && getParentWidget<QdfRibbonBar>(widget))
                {
                    ret = QdfOfficeStyle::pixelMetric(pm, option, widget);
                    ret = (double) ret / ((double) (QdfDrawHelpers::getDPIToPercent() / 100.0));
                }
                else if (qobject_cast<const QdfRibbonSystemPopupBar *>(widget) ||
                         qobject_cast<const QdfRibbonPageSystemPopup *>(widget))
                {
                    ret = 32;
                }
                else
                {
                    ret = QdfOfficeStyle::pixelMetric(pm, option, widget);
                }
                break;
            }
        case PM_ToolBarFrameWidth:
            {
                if (qobject_cast<const QdfRibbonQuickAccessBar *>(widget))
                {
                    ret = 0;
                }
                else
                {
                    ret = QdfOfficeStyle::pixelMetric(pm, option, widget);
                }
                break;
            }
        case PM_SliderControlThickness:
            {
                if (const QStyleOptionSlider *sl = qstyleoption_cast<const QStyleOptionSlider *>(option))
                {
                    int space =
                            (sl->orientation == Qt::Horizontal) ? sl->rect.height() : sl->rect.width();
                    int ticks = sl->tickPosition;
                    int n = 0;
                    if (ticks & QSlider::TicksAbove)
                    {
                        ++n;
                    }
                    if (ticks & QSlider::TicksBelow)
                    {
                        ++n;
                    }
                    if (!n)
                    {
                        ret = space;
                        break;
                    }

                    int thick = 6;// Magic constant to get 5 + 16 + 5
                    if (ticks != QSlider::TicksBothSides && ticks != QSlider::NoTicks)
                    {
                        thick += proxy()->pixelMetric(PM_SliderLength, sl, widget) / 4;
                    }

                    space -= thick;
                    if (space > 0)
                    {
                        thick += (space * 2) / (n + 2);
                    }
                    ret = thick;
                }
                else
                {
                    ret = 0;
                }
            }
            break;
        case PM_ToolBarSeparatorExtent:
            {
                if (qobject_cast<const QdfRibbonToolBarControl *>(widget))
                {
                    ret = int(QdfDrawHelpers::dpiScaled(3.));
                }
                else if (qobject_cast<const QdfRibbonStatusBar *>(widget))
                {
                    ret = int(QdfDrawHelpers::dpiScaled(3.));
                }
                else
                {
                    ret = QdfOfficeStyle::pixelMetric(pm, option, widget);
                }
            }
            break;

        default:
            ret = QdfOfficeStyle::pixelMetric(pm, option, widget);
            break;
    }
    return ret;
}

int QdfRibbonStyle::styleHint(StyleHint hint, const QStyleOption *opt, const QWidget *widget,
                              QStyleHintReturn *returnData) const
{
    QDF_D(const QdfRibbonStyle);
    int ret = 0;
    if (hint == QStyle::SH_ToolButtonStyle)
    {
        if (const QdfRibbonSystemButton *sysButton = qobject_cast<const QdfRibbonSystemButton *>(widget))
        {
            ret = sysButton->toolButtonStyle();
        }
    }
    else if (hint == (StyleHint) SH_FlatFrame)
    {
        ret = d->m_flatFrame;
    }
    else if (hint == SH_UnderlineShortcut)
    {
        const QdfRibbonBar *ribbonBar = ::getParentWidget<const QdfRibbonBar>(widget);
        if (ribbonBar && !qobject_cast<const QMenu *>(widget))
        {
            ret = 0;
        }
        else if (ribbonBar && ribbonBar->qdf_d()->m_keyTips.size() > 0)
        {
            ret = 1;
        }
        else
        {
            ret = QdfOfficeStyle::styleHint(hint, opt, widget, returnData);
        }
    }
    else if (hint == (StyleHint) SH_RibbonBackstageHideTabs)
    {
        return d->ribbonPaintManager()->isRibbonBackstageHideTabs();
    }
    else if (hint == (StyleHint) SH_RibbonItemUpperCase)
    {
        return d->ribbonPaintManager()->isTopLevelMenuItemUpperCase(widget);
    }
    else
    {
        ret = QdfOfficeStyle::styleHint(hint, opt, widget, returnData);
    }
    return ret;
}

QPixmap QdfRibbonStyle::standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt,
                                       const QWidget *widget) const
{
    return QdfOfficeStyle::standardPixmap(standardPixmap, opt, widget);
}

QSize QdfRibbonStyle::sizeFromContents(ContentsType ct, const QStyleOption *opt, const QSize &csz,
                                       const QWidget *widget) const
{
    QSize sz(csz);
    switch (ct)
    {
        case CT_ToolButton:
            {
                if (qobject_cast<const QdfRibbonSystemButton *>(widget))
                {
                    sz = QdfOfficeStyle::sizeFromContents(ct, opt, csz, widget);
                }
                else if (widget &&
                         (qobject_cast<const QdfRibbonStatusBar *>(widget->parentWidget()) ||
                          qobject_cast<const QdfRibbonStatusBarSwitchGroup *>(widget->parentWidget())))
                {
                    if (const QStyleOptionToolButton *optToolButton =
                                qstyleoption_cast<const QStyleOptionToolButton *>(opt))
                    {
                        if (!optToolButton->icon.isNull())
                        {
                            sz += QSize(5, 4);
                        }
                        else
                        {
                            sz.setHeight(sz.height() + 5);
                        }
                    }
                }
                else
                {
                    sz = QdfOfficeStyle::sizeFromContents(ct, opt, csz, widget);
                }
            }
            break;
        case CT_MenuItem:
            {
                const QMenu *menu = qobject_cast<const QdfRibbonSystemPopupBar *>(widget);
                if (!menu)
                {
                    menu = qobject_cast<const QdfRibbonPageSystemPopup *>(widget);
                    if (menu)
                    {
                        if (const QStyleOptionMenuItem *mi =
                                    qstyleoption_cast<const QStyleOptionMenuItem *>(opt))
                        {
                            if (mi->text.count(QString(_qdf_PopupLable)) > 0)
                            {
                                sz = QdfOfficeStyle::sizeFromContents(ct, opt, csz, widget);
                                sz.setWidth(1);
                                break;
                            }
                        }
                    }
                }

                if (menu)
                {
                    if (const QStyleOptionMenuItem *mi =
                                qstyleoption_cast<const QStyleOptionMenuItem *>(opt))
                    {
                        int w = sz.width();
                        sz = QdfOfficeStyle::sizeFromContents(ct, opt, csz, widget);

                        if (mi->menuItemType == QStyleOptionMenuItem::Separator)
                        {
                            sz = QSize(10, windowsSepHeight);
                        }
                        else if (mi->icon.isNull())
                        {
                            sz.setHeight(sz.height() - 2);

                            int maxheight = 0;
                            QList<QAction *> list = menu->actions();
                            for (int j = 0; j < list.count(); j++)
                            {
                                QAction *action = list.at(j);
                                if (!action->icon().isNull())
                                {
                                    int iconExtent = pixelMetric(PM_LargeIconSize, opt, widget);
                                    maxheight =
                                            qMax(sz.height(),
                                                 action->icon().actualSize(QSize(iconExtent, iconExtent))
                                                                 .height() +
                                                         2 * windowsItemFrame);
                                }
                            }

                            if (maxheight > 0)
                            {
                                sz.setHeight(maxheight);
                            }

                            w -= 6;
                        }

                        if (mi->menuItemType != QStyleOptionMenuItem::Separator && !mi->icon.isNull())
                        {
                            int iconExtent = pixelMetric(PM_LargeIconSize, opt, widget);
                            sz.setHeight(
                                    qMax(sz.height(),
                                         mi->icon.actualSize(QSize(iconExtent, iconExtent)).height() +
                                                 2 * windowsItemFrame));
                        }
                        int maxpmw = mi->maxIconWidth;
                        int tabSpacing = use2000style ? 20 : windowsTabSpacing;
                        if (mi->text.contains(QLatin1Char('\t')))
                        {
                            w += tabSpacing;
                        }
                        else if (mi->menuItemType == QStyleOptionMenuItem::SubMenu)
                        {
                            w += 2 * windowsArrowHMargin;
                        }
                        else if (mi->menuItemType == QStyleOptionMenuItem::DefaultItem)
                        {
                            QFontMetrics fm(mi->font);
                            QFont fontBold = mi->font;
                            fontBold.setBold(true);
                            QFontMetrics fmBold(fontBold);
#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
                            w += fmBold.width(mi->text) - fm.width(mi->text);
#else
                            w += fmBold.horizontalAdvance(mi->text) - fm.horizontalAdvance(mi->text);
#endif
                        }

                        int checkcol = qMax(maxpmw, windowsCheckMarkWidth);
                        w += checkcol;
                        w += windowsRightBorder + 12;
                        sz.setWidth(w);

                        if (mi->menuItemType != QStyleOptionMenuItem::Separator &&
                            qobject_cast<const QdfRibbonPageSystemPopup *>(widget))
                        {
                            QFontMetrics fm(mi->font);
                            sz.setHeight(qMax(sz.height(), fm.height() * 3 + fm.height() / 2 +
                                                                   windowsItemVMargin * 2));
                        }
                    }
                }
                else if (qobject_cast<const QdfOfficePopupMenu *>(widget))
                {
                    if (const QStyleOptionMenuItem *mi =
                                qstyleoption_cast<const QStyleOptionMenuItem *>(opt))
                    {
                        sz = QdfOfficeStyle::sizeFromContents(ct, opt, csz, widget);
                        if (mi->menuItemType == QStyleOptionMenuItem::Separator)
                        {
                            sz.setHeight(csz.height());
                        }
                    }
                }
                else
                {
                    sz = QdfOfficeStyle::sizeFromContents(ct, opt, csz, widget);
                }
            }
            break;
        case CT_RibbonSliderButton:
            {
            }
            break;
        case CT_MenuBar:
            if (const QdfRibbonBar *ribbonBar = qobject_cast<const QdfRibbonBar *>(widget))
            {
                if (!ribbonBar->isVisible())
                {
                    sz = QdfOfficeStyle::sizeFromContents(ct, opt, csz, widget);
                    sz.setHeight(0);
                }
            }
            break;
        default:
            sz = QdfOfficeStyle::sizeFromContents(ct, opt, csz, widget);
            break;
    }

    return sz;
}

QRect QdfRibbonStyle::subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc,
                                     const QWidget *widget) const
{
    QRect ret;
    switch (cc)
    {
        case CC_ToolButton:
            if (const QStyleOptionToolButton *tb =
                        qstyleoption_cast<const QStyleOptionToolButton *>(opt))
            {
                int mbi = proxy()->pixelMetric(PM_MenuButtonIndicator, tb, widget);
                ret = tb->rect;
                switch (sc)
                {
                    case SC_ToolButton:
                        if ((tb->features & (QStyleOptionToolButton::MenuButtonPopup |
                                             QStyleOptionToolButton::PopupDelay)) ==
                            QStyleOptionToolButton::MenuButtonPopup)
                        {
                            if (tb->toolButtonStyle == Qt::ToolButtonTextUnderIcon)
                            {
                                QRect popupr =
                                        subControlRect(cc, opt, QStyle::SC_ToolButtonMenu, widget);
                                ret.adjust(0, 0, 0, -popupr.height());
                            }
                            else
                            {
                                ret.adjust(0, 0, -mbi, 0);
                            }
                            break;
                        }
                    case SC_ToolButtonMenu:
                        if ((tb->features & (QStyleOptionToolButton::MenuButtonPopup |
                                             QStyleOptionToolButton::PopupDelay)) ==
                            QStyleOptionToolButton::MenuButtonPopup)
                        {
                            if (!qobject_cast<const QdfRibbonGroupPopupButton *>(widget))
                            {
                                if (tb->toolButtonStyle == Qt::ToolButtonTextUnderIcon)
                                {
                                    QString str(tb->text);
                                    QString strFirstRow, strSecondRow;
                                    splitString(str, strFirstRow, strSecondRow);

                                    QSize textSize;
                                    if (!strFirstRow.isEmpty())
                                    {
                                        textSize =
                                                opt->fontMetrics.size(Qt::TextShowMnemonic, strFirstRow);
#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
                                        textSize.setWidth(textSize.width() +
                                                          opt->fontMetrics.width(QLatin1Char(' ')) * 2);
#else
                                        textSize.setWidth(
                                                textSize.width() +
                                                opt->fontMetrics.horizontalAdvance(QLatin1Char(' ')) *
                                                        2);
#endif
                                    }
                                    if (!strSecondRow.isEmpty())
                                    {
                                        QSize textSize1 = opt->fontMetrics.size(Qt::TextShowMnemonic,
                                                                                strSecondRow);
#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
                                        textSize1.setWidth(textSize1.width() +
                                                           opt->fontMetrics.width(QLatin1Char(' ')) * 2);
#else
                                        textSize1.setWidth(
                                                textSize1.width() +
                                                opt->fontMetrics.horizontalAdvance(QLatin1Char(' ')) *
                                                        2);
#endif
                                        textSize.setWidth(qMax(textSize.width(), textSize1.width()));
                                    }
                                    ret.adjust(0, tb->iconSize.height() + 8, 0, 0);
                                    ret.setWidth(qMax(textSize.width(), opt->rect.width()));
                                }
                                else
                                {
                                    ret.adjust(ret.width() - mbi, 0, 0, 0);
                                }
                            }
                            break;
                        }
                    default:
                        break;
                }
                ret = visualRect(tb->direction, tb->rect, ret);
            }
            break;
        default:
            ret = QdfOfficeStyle::subControlRect(cc, opt, sc, widget);
            break;
    }
    return ret;
}

QRect QdfRibbonStyle::subElementRect(SubElement sr, const QStyleOption *opt, const QWidget *widget) const
{
    QRect r;
    switch (sr)
    {
        case SE_TabBarTabText:
            if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt))
            {
                if (qobject_cast<QMdiArea *>(widget->parentWidget()))
                {
                    QDF_D(const QdfRibbonStyle);
#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
                    QStyleOptionTabV3 tabV3(*tab);
#else
                    QStyleOptionTab tabV3(*tab);
#endif
                    QRect dummyIconRect;
                    d->tabLayout(&tabV3, widget, &r, &dummyIconRect);

                    bool verticalTabs = tab->shape == QTabBar::RoundedEast ||
                                        tab->shape == QTabBar::RoundedWest ||
                                        tab->shape == QTabBar::TriangularEast ||
                                        tab->shape == QTabBar::TriangularWest;

                    bool botton = tab->shape == QTabBar::RoundedSouth;
                    if (!verticalTabs && botton && tab->icon.isNull() &&
                        (tab->position == QStyleOptionTab::OnlyOneTab ||
                         tab->position == QStyleOptionTab::Beginning))
                    {
                        r.translate(6, 0);
                    }
                }
                else
                {
                    r = QdfOfficeStyle::subElementRect(sr, opt, widget);
                }
            }
            break;
        case SE_TabBarTabLeftButton:
        case SE_TabBarTabRightButton:
            {
                r = QdfOfficeStyle::subElementRect(sr, opt, widget);

#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
                if (const QStyleOptionTabV3 *tab = qstyleoption_cast<const QStyleOptionTabV3 *>(opt))
#else
                if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt))
#endif
                {
                    QRect rc = r;
                    if (sr != SE_TabBarTabLeftButton)
                    {
                        if (tab->shape == QTabBar::RoundedSouth &&
                            (tab->position == QStyleOptionTab::OnlyOneTab ||
                             tab->position == QStyleOptionTab::Beginning))
                        {
                            rc.setLeft(rc.left() + rc.width() / 2);
                        }
                        else if (tab->shape == QTabBar::RoundedWest &&
                                 (tab->position == QStyleOptionTab::OnlyOneTab ||
                                  tab->position == QStyleOptionTab::Beginning))
                        {
                            rc.setTop(rc.top() + rc.width() / 2);
                        }
                        else if (tab->shape == QTabBar::RoundedEast)
                        {
                            rc.setTop(rc.top() /* - rc.width()/2*/);
                        }
                    }
                    r = rc;
                }
            }
            break;
        default:
            r = QdfOfficeStyle::subElementRect(sr, opt, widget);
            break;
    }
    return r;
}

bool QdfRibbonStyle::drawFrame(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QDF_D(const QdfRibbonStyle);
    return d->ribbonPaintManager()->drawFrame(opt, p, w);
}

bool QdfRibbonStyle::drawShapedFrame(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QDF_D(const QdfRibbonStyle)
    bool ret = false;
    if (qobject_cast<const QMdiArea *>(w))
    {
        ret = d->ribbonPaintManager()->drawShapedFrame(opt, p, w);
    }
    else if (qobject_cast<const QdfRibbonBackstageSeparator *>(w))
    {
        ret = d->ribbonPaintManager()->drawRibbonBackstageSeparator(opt, p, w);
    }
    return ret;
}

bool QdfRibbonStyle::drawPanelStatusBar(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QDF_D(const QdfRibbonStyle);
    return d->ribbonPaintManager()->drawPanelStatusBar(opt, p, w);
}

void QdfRibbonStyle::drawRibbonBar(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QDF_D(const QdfRibbonStyle);
    d->ribbonPaintManager()->drawRibbonBar(opt, p, w);
}

void QdfRibbonStyle::drawRibbonTabBar(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QDF_D(const QdfRibbonStyle);
    d->ribbonPaintManager()->drawRibbonTabBar(opt, p, w);
}


void QdfRibbonStyle::drawRibbonGroups(const QStyleOption *option, QPainter *p,
                                      const QWidget *widget) const
{
    QDF_D(const QdfRibbonStyle);
    d->ribbonPaintManager()->drawRibbonGroups(option, p, widget);
}


void QdfRibbonStyle::drawGroup(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    if (paintAnimation(tp_ControlElement, CE_Group, opt, p, widget, 300))
    {
        return;
    }

    QDF_D(const QdfRibbonStyle);
    d->ribbonPaintManager()->drawGroup(opt, p, widget);
}


void QdfRibbonStyle::drawReducedGroup(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    QDF_D(const QdfRibbonStyle);
    d->ribbonPaintManager()->drawReducedGroup(opt, p, widget);
}

bool QdfRibbonStyle::drawSizeGrip(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QDF_D(const QdfRibbonStyle);
    return d->ribbonPaintManager()->drawSizeGrip(opt, p, w);
}

void QdfRibbonStyle::drawContextHeaders(const QStyleOption *opt, QPainter *p) const
{
    QDF_D(const QdfRibbonStyle);
    return d->ribbonPaintManager()->drawContextHeaders(opt, p);
}

bool QdfRibbonStyle::drawIndicatorToolBarSeparator(const QStyleOption *opt, QPainter *p,
                                                   const QWidget *widget) const
{
    QDF_D(const QdfRibbonStyle);
    if (d->ribbonPaintManager()->drawIndicatorToolBarSeparator(opt, p, widget))
    {
        return true;
    }
    return QdfOfficeStyle::drawIndicatorToolBarSeparator(opt, p, widget);
}

bool QdfRibbonStyle::drawTitleBar(const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const
{
    QDF_D(const QdfRibbonStyle)
    if (!qobject_cast<const QdfRibbonBar *>(w))
    {
        return QdfOfficeStyle::drawTitleBar(opt, p, w);
    }

    if (const QdfStyleOptionTitleBar *optTitleBar =
                qstyleoption_cast<const QdfStyleOptionTitleBar *>(opt))
    {
        if (optTitleBar->frameHelper)
        {
            QRect rcCaption = optTitleBar->rect;
            bool dwmEnabled = optTitleBar->frameHelper->isDwmEnabled();
            bool active = optTitleBar->state & State_Active;

            QRect rcText = optTitleBar->rcTitleText;

            if (dwmEnabled)
            {
                optTitleBar->frameHelper->fillSolidRect(p, rcCaption, optTitleBar->airRegion,
                                                        QColor(0, 0, 0));
                QdfStyleOptionTitleBar optTitle = *optTitleBar;
                optTitle.rcTitleText = rcText;

                optTitleBar->frameHelper->drawTitleBar(p, optTitle);
            }

            if ((!optTitleBar->isBackstageVisible) && optTitleBar->listContextHeaders &&
                optTitleBar->listContextHeaders->count() > 0)
            {
                proxy()->drawPrimitive(
                        (QStyle::PrimitiveElement) QdfRibbonStyle::PE_RibbonContextHeaders, opt, p, w);
            }

            if (!dwmEnabled)
            {
                p->save();
                p->setPen(!active ? d->m_clrFrameCaptionTextInActive : d->m_clrFrameCaptionTextActive);
                p->setFont(QApplication::font("QApplication"));
                QString text =
                        optTitleBar->frameHelper ? optTitleBar->frameHelper->getWindowText() : QString();
                text = p->fontMetrics().elidedText(text.isEmpty() ? optTitleBar->text : text,
                                                   Qt::ElideRight, rcText.width());
                Qt::Alignment fl = Qt::AlignCenter;// optTitleBar->quickAccessVisible ? Qt::AlignCenter :
                                                   // Qt::AlignLeft | Qt::AlignVCenter;
                p->drawText(rcText, fl | Qt::TextSingleLine, text);
                p->restore();
            }

            if (optTitleBar->drawIcon && optTitleBar->frameHelper)
            {
                if (!dwmEnabled && optTitleBar->subControls & SC_TitleBarSysMenu &&
                    optTitleBar->titleBarFlags & Qt::WindowSystemMenuHint)
                {
                    QStyleOption tool(0);
                    tool.palette = optTitleBar->palette;

                    QRect ir = proxy()->subControlRect(CC_TitleBar, optTitleBar, SC_TitleBarSysMenu, w);

                    if (!optTitleBar->icon.isNull())
                    {
#ifdef Q_OS_WIN
                        if (HICON hIcon = optTitleBar->frameHelper->windowIcon(optTitleBar->icon,
                                                                               optTitleBar->rect))
                        {
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                            QPixmap px = qt_pixmapFromWinHICON(hIcon);
                            if (!px.isNull())
                            {
                                QSize iconSize = px.size();
                                int controlHeight = optTitleBar->rect.height();
                                int hPad = (controlHeight - iconSize.height()) / 2;
                                int vPad = (controlHeight - iconSize.width()) / 2;
                                int br = optTitleBar->frameHelper->frameBorder() / 2;
                                QRect r =
                                        QRect(hPad - br, vPad - br, iconSize.width(), iconSize.height());
                                r = visualRect(optTitleBar->direction, optTitleBar->rect, r);
                                p->drawPixmap(r.left(), r.top(), px);
                            }
    #else
                            ir.translate(optTitleBar->frameHelper->frameBorder(),
                                         optTitleBar->frameHelper->frameBorder());
                            ::DrawIconEx(p->paintEngine()->getDC(), ir.left(), ir.top(), hIcon, 0, 0, 0,
                                         nullptr, DI_NORMAL | DI_COMPAT);
    #endif
                        }
#else
                        ir.translate(4, 0);
                        optTitleBar->icon.paint(p, ir);
#endif// Q_OS_WIN
                    }
                    else
                    {
                        int iconSize = proxy()->pixelMetric(PM_SmallIconSize, optTitleBar, w);
                        QPixmap pm =
                                standardIcon(SP_TitleBarMenuButton, &tool, w).pixmap(iconSize, iconSize);
                        tool.rect = ir;
                        p->save();
                        proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
                        p->restore();
                    }
                }
            }
            return true;
        }
        else
        {
            if (optTitleBar->listContextHeaders && optTitleBar->listContextHeaders->count() > 0)
            {
                proxy()->drawPrimitive(
                        (QStyle::PrimitiveElement) QdfRibbonStyle::PE_RibbonContextHeaders, opt, p, w);
            }
            return true;
        }
    }
    return false;
}

bool QdfRibbonStyle::drawFrameMenu(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QDF_D(const QdfRibbonStyle);
    return d->ribbonPaintManager()->drawFrameMenu(opt, p, w);
}


bool QdfRibbonStyle::drawSlider(const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const
{
    QDF_D(const QdfRibbonStyle);
    return d->ribbonPaintManager()->drawSlider(opt, p, w);
}


void QdfRibbonStyle::drawSystemButton(const QStyleOption *option, QPainter *p,
                                      const QWidget *widget) const
{
    if (paintAnimation(tp_PrimitiveElement, PE_RibbonFileButton, option, p, widget))
    {
        return;
    }

    QDF_D(const QdfRibbonStyle);
    d->ribbonPaintManager()->drawSystemButton(option, p, widget);
}


void QdfRibbonStyle::drawOptionButton(const QStyleOption *option, QPainter *p,
                                      const QWidget *widget) const
{
    QDF_D(const QdfRibbonStyle);
    d->ribbonPaintManager()->drawOptionButton(option, p, widget);
}


void QdfRibbonStyle::drawGroupScrollButton(const QStyleOption *option, QPainter *p,
                                           const QWidget *widget) const
{
    QDF_D(const QdfRibbonStyle);
    d->ribbonPaintManager()->drawGroupScrollButton(option, p, widget);
}


void QdfRibbonStyle::drawFileButtonLabel(const QStyleOption *option, QPainter *p,
                                         const QWidget *widget) const
{
    QDF_D(const QdfRibbonStyle);
    if (const QStyleOptionToolButton *toolbutton =
                qstyleoption_cast<const QStyleOptionToolButton *>(option))
    {
        int alignment = Qt::TextShowMnemonic;
        if (!proxy()->styleHint(SH_UnderlineShortcut, toolbutton, widget))
        {
            alignment |= Qt::TextHideMnemonic;
        }

        alignment |= Qt::AlignCenter;
        alignment |= Qt::TextSingleLine;

        QString text = toolbutton->text;
        if (d->ribbonPaintManager()->isTopLevelMenuItemUpperCase(widget))
        {
            text = text.toUpper();
        }

        proxy()->drawItemText(p, toolbutton->rect, alignment, toolbutton->palette,
                              toolbutton->state & State_Enabled, text, QPalette::ButtonText);
    }
}


void QdfRibbonStyle::drawPopupSystemButton(const QStyleOption *option, QPainter *p,
                                           const QWidget *w) const
{
    Q_UNUSED(option);
    Q_UNUSED(p);
    Q_UNUSED(w);
}


void QdfRibbonStyle::drawQuickAccessButton(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QDF_D(const QdfRibbonStyle);
    d->ribbonPaintManager()->drawQuickAccessButton(opt, p, w);
}


void QdfRibbonStyle::drawPopupResizeGripper(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QDF_D(const QdfRibbonStyle);
    d->ribbonPaintManager()->drawPopupResizeGripper(opt, p, w);
}


bool QdfRibbonStyle::drawMenuItem(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    QDF_D(const QdfRibbonStyle)
    if (qobject_cast<const QdfRibbonPageSystemPopup *>(widget))
    {
        if (const QStyleOptionMenuItem *menuitem = qstyleoption_cast<const QStyleOptionMenuItem *>(opt))
        {
            if (menuitem->text.count(QString(_qdf_PopupLable)) > 0)
            {
                p->fillRect(menuitem->rect.adjusted(-1, 0, 2, 0), d->m_clrControlGalleryLabel);
                p->fillRect(menuitem->rect.left(), menuitem->rect.bottom() - 1, menuitem->rect.width(),
                            1, QColor(197, 197, 197));

                QFont fontSave = p->font();

                QFont font = fontSave;
                font.setBold(true);
                p->setFont(font);

                QRect rcText(menuitem->rect);
                rcText.adjust(7, 0, 0, 0);

                QPalette pal = menuitem->palette;
                pal.setColor(QPalette::WindowText, d->m_clrGroupClientText);

                int flags = 0;
                flags |= Qt::TextHideMnemonic | Qt::AlignVCenter | Qt::TextSingleLine;

                QString text = menuitem->text;
                text = p->fontMetrics().elidedText(text.remove(QString(_qdf_PopupLable)), Qt::ElideRight,
                                                   rcText.adjusted(2, 0, -2, 0).width());

                // draw text
                proxy()->drawItemText(p, rcText, flags, pal, opt->state & State_Enabled, text,
                                      QPalette::WindowText);
                p->setFont(fontSave);

                return true;
            }
            else
            {
                int x, y, w, h;
                menuitem->rect.getRect(&x, &y, &w, &h);
                int tab = menuitem->tabWidth;
                bool dis = !(menuitem->state & State_Enabled);
                bool checked = menuitem->checkType != QStyleOptionMenuItem::NotCheckable
                                       ? menuitem->checked
                                       : false;
                bool act = menuitem->state & State_Selected;

                // windows always has a check column, regardless whether we have an icon or not
                const int nIconSize = pixelMetric(PM_ToolBarIconSize, opt, widget);
                int checkcol = nIconSize;

                if (act)
                {
                    drawRectangle(p, menuitem->rect, true /*selected*/, false /*pressed*/,
                                  !dis /*enabled*/, false /*checked*/, false /*popuped*/, TypePopup,
                                  BarPopup);
                }

                if (menuitem->menuItemType == QStyleOptionMenuItem::Separator)
                {
                    int yoff = (y - 1 + h / 2);
                    int xIcon = nIconSize;
                    if (qobject_cast<const QdfOfficePopupMenu *>(widget))
                    {
                        xIcon = 0;
                    }
                    p->setPen(menuitem->palette.dark().color());
                    p->drawLine(x + 2 + /*menuitem->maxIconWidth*/ xIcon, yoff, x + w - 4, yoff);
                    p->setPen(menuitem->palette.light().color());
                    p->drawLine(x + 2 + /*menuitem->maxIconWidth*/ xIcon, yoff + 1, x + w - 4, yoff + 1);
                    return true;
                }

                QRect vCheckRect = visualRect(opt->direction, menuitem->rect,
                                              QRect(menuitem->rect.x(), menuitem->rect.y(), checkcol,
                                                    menuitem->rect.height()));

                // On Windows Style, if we have a checkable item and an icon we
                // draw the icon recessed to indicate an item is checked. If we
                // have no icon, we draw a checkmark instead.
                if (!menuitem->icon.isNull())
                {
                    QIcon::Mode mode = dis ? QIcon::Disabled : QIcon::Normal;

                    int iconSize = pixelMetric(PM_LargeIconSize, opt, widget);
                    if (iconSize > qMin(opt->rect.height(), opt->rect.width()))
                    {
                        iconSize = pixelMetric(PM_SmallIconSize, opt, widget);
                    }

                    if (act && !dis)
                    {
                        mode = QIcon::Active;
                    }
                    QPixmap pixmap;
                    if (checked)
                    {
                        // pixmap = menuitem->icon.pixmap(pixelMetric(PM_LargeIconSize, opt, widget), mode, QIcon::On);
                        pixmap = menuitem->icon.pixmap(iconSize, mode, QIcon::On);
                    }
                    else
                    {
                        // pixmap = menuitem->icon.pixmap(pixelMetric(PM_LargeIconSize, opt, widget), mode);
                        pixmap = menuitem->icon.pixmap(iconSize, mode);
                    }

                    int pixw = pixmap.width();
                    int pixh = pixmap.height();

                    QRect pmr(0, 0, pixw, pixh);
                    pmr.moveCenter(vCheckRect.center());
                    p->setPen(menuitem->palette.text().color());

                    if (checked)
                    {
                        QRect vIconRect = visualRect(opt->direction, opt->rect, pmr);
                        QRect rcChecked = menuitem->rect;
                        rcChecked.setLeft(vIconRect.left());
                        rcChecked.setWidth(vIconRect.width());
                        drawRectangle(p, rcChecked.adjusted(-2, 1, 2, -1), false /*selected*/,
                                      true /*pressed*/, !dis /*enabled*/, true /*checked*/,
                                      false /*popuped*/, TypePopup, BarPopup);
                    }

                    p->drawPixmap(pmr.topLeft() + QPoint(windowsItemHMargin, 0), pixmap);
                }

                int xm = windowsItemFrame + checkcol + (windowsItemHMargin * 2);
                int xpos = menuitem->rect.x() + xm;

                // draw text
                if (!menuitem->text.isEmpty())
                {
                    int height = menuitem->fontMetrics.height();
                    QRect textRect(xpos, y + windowsItemVMargin + 2,
                                   w - xm - windowsRightBorder - tab + 1, height);
                    QRect vTextRect = visualRect(opt->direction, menuitem->rect, textRect);

                    p->save();
                    QStringList split = menuitem->text.split(QString("\n"));
                    QString s = split.count() > 0 ? split.at(0) : menuitem->text;

                    QFont font = menuitem->font;
                    font.setBold(true);
                    p->setFont(font);

                    int t = s.indexOf(QLatin1Char('\t'));
                    int text_flags = Qt::AlignVCenter | Qt::TextShowMnemonic | Qt::TextDontClip |
                                     Qt::TextSingleLine;

                    if (!styleHint(SH_UnderlineShortcut, menuitem, widget))
                    {
                        text_flags |= Qt::TextHideMnemonic;
                    }
                    text_flags |= Qt::AlignLeft;

                    // draw hotkeys
                    if (t >= 0)
                    {
                        QRect vShortcutRect =
                                visualRect(opt->direction, menuitem->rect,
                                           QRect(textRect.topRight(),
                                                 QPoint(menuitem->rect.right(), textRect.bottom())));

                        p->setPen(opt->state & State_Enabled ? d->m_clrMenuPopupText
                                                             : d->m_clrMenuBarGrayText);
                        p->drawText(vShortcutRect, text_flags, s.mid(t + 1));
                        s = s.left(t);
                    }

                    p->setPen(opt->state & State_Enabled ? d->m_clrMenuPopupText
                                                         : d->m_clrMenuBarGrayText);

                    p->drawText(vTextRect, text_flags, s.left(t));
                    p->restore();
                    if (split.count() > 1)
                    {
                        text_flags |= Qt::TextWordWrap;
                        QRect textTipRect(xpos, y + vTextRect.height(), w - xm - 1, opt->rect.height());
                        QRect rc =
                                menuitem->fontMetrics.boundingRect(textTipRect, text_flags, split.at(1));
                        QRect vTextTipRect = visualRect(opt->direction, menuitem->rect, rc);
                        vTextTipRect.setTop(y + vTextRect.height());
                        vTextTipRect.setWidth(textTipRect.width());
                        p->setPen(opt->state & State_Enabled ? d->m_clrMenuPopupText
                                                             : d->m_clrMenuBarGrayText);
                        p->drawText(vTextTipRect, text_flags, split.at(1));
                    }
                }

                return true;
            }
        }
    }

    if (qobject_cast<const QdfRibbonBackstageView *>(widget))
    {
        d->ribbonPaintManager()->drawRibbonBackstageMenu(opt, p, widget);
        return true;
    }
    else
    {
        return QdfOfficeStyle::drawMenuItem(opt, p, widget);
    }
}


void QdfRibbonStyle::drawTabShape(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    if (paintAnimation(tp_PrimitiveElement, PE_RibbonTab, opt, p, widget))
    {
        return;
    }

    QDF_D(const QdfRibbonStyle);
    d->ribbonPaintManager()->drawTabShape(opt, p, widget);
}


void QdfRibbonStyle::drawTabShapeLabel(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QDF_D(const QdfRibbonStyle);
    d->ribbonPaintManager()->drawTabShapeLabel(opt, p, w);
}


bool QdfRibbonStyle::drawTabBarTabShape(const QStyleOption *opt, QPainter *p,
                                        const QWidget *widget) const
{
    QDF_D(const QdfRibbonStyle);
    return d->ribbonPaintManager()->drawTabBarTabShape(opt, p, widget);
}


bool QdfRibbonStyle::drawTabBarTabLabel(const QStyleOption *opt, QPainter *p,
                                        const QWidget *widget) const
{
    QDF_D(const QdfRibbonStyle);

    if (!qobject_cast<QMdiArea *>(widget->parentWidget()))
    {
        return QdfOfficeStyle::drawTabBarTabLabel(opt, p, widget);
    }

    if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt))
    {
#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
        QStyleOptionTabV3 tabV2(*tab);
#else
        QStyleOptionTab tabV2(*tab);
#endif
        QRect tr = tabV2.rect;
        bool verticalTabs = tabV2.shape == QTabBar::RoundedEast || tabV2.shape == QTabBar::RoundedWest ||
                            tabV2.shape == QTabBar::TriangularEast ||
                            tabV2.shape == QTabBar::TriangularWest;

        int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;
        if (!proxy()->styleHint(SH_UnderlineShortcut, opt, widget))
        {
            alignment |= Qt::TextHideMnemonic;
        }

        if (verticalTabs)
        {
            p->save();
            int newX, newY, newRot;
            if (tabV2.shape == QTabBar::RoundedEast || tabV2.shape == QTabBar::TriangularEast)
            {
                newX = tr.width() + tr.x();
                newY = tr.y();
                newRot = 90;
            }
            else
            {
                newX = tr.x();
                newY = tr.y() + tr.height();
                newRot = -90;
            }
            QTransform m = QTransform::fromTranslate(newX, newY);
            m.rotate(newRot);
            p->setTransform(m, true);
        }
        QRect iconRect;
        d->tabLayout(&tabV2, widget, &tr, &iconRect);
        // we compute tr twice because the style may override subElementRect
        tr = proxy()->subElementRect(SE_TabBarTabText, opt, widget);

        if (!tabV2.icon.isNull())
        {
            QPixmap tabIcon = tabV2.icon.pixmap(
                    tabV2.iconSize, (tabV2.state & State_Enabled) ? QIcon::Normal : QIcon::Disabled,
                    (tabV2.state & State_Selected) ? QIcon::On : QIcon::Off);
            p->drawPixmap(iconRect.x(), iconRect.y(), tabIcon);
        }
        proxy()->drawItemText(p, tr, alignment, tab->palette, tab->state & State_Enabled, tab->text,
                              QPalette::WindowText);

        if (verticalTabs)
        {
            p->restore();
        }

        if (tabV2.state & State_HasFocus)
        {
            const int OFFSET = 1 + pixelMetric(PM_DefaultFrameWidth);

            int x1, x2;
            x1 = tabV2.rect.left();
            x2 = tabV2.rect.right() - 1;

            QStyleOptionFocusRect fropt;
            fropt.QStyleOption::operator=(*tab);
            fropt.rect.setRect(x1 + 1 + OFFSET, tabV2.rect.y() + OFFSET, x2 - x1 - 2 * OFFSET,
                               tabV2.rect.height() - 2 * OFFSET);
            drawPrimitive(PE_FrameFocusRect, &fropt, p, widget);
        }
        return true;
    }
    return false;
}


bool QdfRibbonStyle::drawPanelButtonTool(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QDF_D(const QdfRibbonStyle);
    return d->ribbonPaintManager()->drawPanelButtonTool(opt, p, w);
}


void QdfRibbonStyle::calcRects(const QRect &rc, const QSize &pmSize, const QFontMetrics &fMetric,
                               const QString &text, bool hasMenu, QRect *pmRect, QRect *strFirsRect,
                               QRect *strSecondRect) const
{
    QRect rect(QPoint(0, 0), pmSize);
    *pmRect = rect;

    QString strFirstRow, strSecondRow;
    splitString(text, strFirstRow, strSecondRow);

    if (!strFirstRow.isEmpty())
    {
        QSize size = fMetric.size(Qt::TextSingleLine, strFirstRow);
        if (!size.isEmpty())
        {
            if (hasMenu && strSecondRow.isEmpty())
            {
                size.setWidth(size.width() + 4 + 5);
            }

            *strFirsRect = QRect(QPoint(0, rect.bottom() + 2), size);
            rect = rect.united(*strFirsRect);
        }
    }
    if (!strSecondRow.isEmpty())
    {
        QSize size = fMetric.size(Qt::TextSingleLine, strSecondRow);
        if (!size.isEmpty())
        {
            if (hasMenu)
            {
                size.setWidth(size.width() + 4 + 5);
            }
            *strSecondRect = QRect(QPoint(0, rect.bottom() + 2), size);
            rect = rect.united(*strSecondRect);
        }
    }

    int dx = (rc.width() - rect.width()) / 2;
    int dy = (rc.height() - rect.height()) / 2;

    if (!pmRect->isEmpty())
    {
        pmRect->translate(dx, dy);
        pmRect->translate((rect.width() - pmRect->width()) / 2, 0);
    }

    if (!strFirsRect->isEmpty())
    {
        strFirsRect->translate(dx, dy);
        strFirsRect->translate((rect.width() - strFirsRect->width()) / 2, 0);
    }

    if (!strSecondRect->isEmpty())
    {
        strSecondRect->translate(dx, dy);
        strSecondRect->translate((rect.width() - strSecondRect->width()) / 2, 0);
    }
}


bool QdfRibbonStyle::drawToolButtonLabel(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    if (qobject_cast<const QdfRibbonBackstageButton *>(w))
    {
        if (const QdfStyleOptionBackstageButton *toolbutton =
                    qstyleoption_cast<const QdfStyleOptionBackstageButton *>(opt))
        {
            QRect rect = toolbutton->rect;
            // QPalette pal = toolbutton->state & State_Enabled ? QPalette() : toolbutton->palette;
            QPalette pal = toolbutton->palette;

            // Arrow type always overrules and is always shown
            bool hasArrow = toolbutton->features & QStyleOptionToolButton::Arrow;
            if (((!hasArrow && toolbutton->icon.isNull()) && !toolbutton->text.isEmpty()) ||
                toolbutton->toolButtonStyle == Qt::ToolButtonTextOnly)
            {
                int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;
                if (!styleHint(SH_UnderlineShortcut, opt, w))
                {
                    alignment |= Qt::TextHideMnemonic;
                }

                proxy()->drawItemText(p, rect, alignment, /*toolbutton->palette*/ pal,
                                      /*opt->state & State_Enabled*/ true, toolbutton->text,
                                      QPalette::ButtonText);
            }
            else
            {
                QPixmap pm;
                QSize pmSize = toolbutton->iconSize;
                if (pmSize.width() > qMin(rect.width(), rect.height()))
                {
                    const int iconExtent = proxy()->pixelMetric(PM_SmallIconSize);
                    pmSize = QSize(iconExtent, iconExtent);
                }

                if (!toolbutton->icon.isNull())
                {
                    QIcon::State state = toolbutton->state & State_On ? QIcon::On : QIcon::Off;
                    QIcon::Mode mode;
                    if (!(toolbutton->state & State_Enabled))
                    {
                        mode = QIcon::Disabled;
                    }
                    else if ((opt->state & State_MouseOver) && (opt->state & State_AutoRaise))
                    {
                        mode = QIcon::Active;
                    }
                    else
                    {
                        mode = QIcon::Normal;
                    }
                    pm = toolbutton->icon.pixmap(toolbutton->rect.size().boundedTo(pmSize), mode, state);
                    pmSize = pm.size();
                }

                if (toolbutton->toolButtonStyle != Qt::ToolButtonIconOnly)
                {
                    int alignment = Qt::TextShowMnemonic;
                    if (!proxy()->styleHint(SH_UnderlineShortcut, opt, w))
                    {
                        alignment |= Qt::TextHideMnemonic;
                    }

                    p->setFont(toolbutton->font);

                    if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon)
                    {
                        QRect pmRect, strFirsRect, strSecondRect;
                        calcRects(rect, pmSize, toolbutton->fontMetrics, toolbutton->text,
                                  toolbutton->features & QStyleOptionToolButton::HasMenu, &pmRect,
                                  &strFirsRect, &strSecondRect);

                        if (!hasArrow)
                        {
                            drawItemPixmap(p, QStyle::visualRect(opt->direction, rect, pmRect),
                                           Qt::AlignHCenter, pm);
                        }
                        // else
                        //     drawArrow(this, toolbutton, pr, p, w);

                        alignment |= Qt::AlignCenter;

                        QString strFirstRow, strSecondRow;
                        splitString(toolbutton->text, strFirstRow, strSecondRow);

                        if (!strFirstRow.isEmpty())
                        {
                            if (toolbutton->features & QStyleOptionToolButton::HasMenu &&
                                strSecondRow.isEmpty())
                            {
                                alignment &= ~Qt::AlignCenter;
                                alignment |= Qt::AlignVCenter | Qt::AlignLeft;
                            }
                            proxy()->drawItemText(p, strFirsRect, alignment, /*toolbutton->palette*/ pal,
                                                  /*toolbutton->state & State_Enabled*/ true,
                                                  strFirstRow, QPalette::ButtonText);
                        }

                        if (!strSecondRow.isEmpty())
                        {
                            if (toolbutton->features & QStyleOptionToolButton::HasMenu)
                            {
                                alignment &= ~Qt::AlignCenter;
                                alignment |= Qt::AlignVCenter | Qt::AlignLeft;
                            }
                            proxy()->drawItemText(p, strSecondRect, alignment,
                                                  /*toolbutton->palette*/ pal,
                                                  /*toolbutton->state & State_Enabled*/ true,
                                                  strSecondRow, QPalette::ButtonText);
                        }
                    }
                    else
                    {
                        QRect pmRect(QPoint(4, (rect.height() - pmSize.height()) / 2), pmSize);
                        if (!hasArrow)
                        {
                            drawItemPixmap(p, QStyle::visualRect(opt->direction, rect, pmRect),
                                           Qt::AlignCenter, pm);
                        }
                        // else
                        //     drawArrow(this, toolbutton, pr, p, w);
                        alignment |= Qt::AlignLeft | Qt::AlignVCenter;

                        QSize sizeText =
                                toolbutton->fontMetrics.size(Qt::TextSingleLine, toolbutton->text);
                        QRect tcText(QPoint(pmRect.right() + 4, (rect.height() - sizeText.height()) / 2),
                                     sizeText);
                        proxy()->drawItemText(p, tcText, alignment, /*toolbutton->palette*/ pal,
                                              /*toolbutton->state & State_Enabled*/ true,
                                              toolbutton->text, QPalette::ButtonText);
                    }
                }
                else
                {
                    // if (hasArrow)
                    //     drawArrow(this, toolbutton, rect, p, w);
                    // else
                    {
                        QRect pr = rect;
                        if ((toolbutton->subControls & SC_ToolButtonMenu) ||
                            (toolbutton->features & QStyleOptionToolButton::HasMenu))
                        {
                            int mbi = proxy()->pixelMetric(PM_MenuButtonIndicator, opt, w);
                            pr.setWidth(pr.width() - mbi);
                        }
                        drawItemPixmap(p, pr, Qt::AlignCenter, pm);
                    }
                }
            }
        }
        return true;
    }
    else
    {
        return QdfOfficeStyle::drawToolButtonLabel(opt, p, w);
    }
}


bool QdfRibbonStyle::drawToolButton(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    if (qobject_cast<const QdfRibbonBackstageButton *>(w))
    {
        if (const QStyleOptionToolButton *toolbutton =
                    qstyleoption_cast<const QStyleOptionToolButton *>(opt))
        {
            QRect button, menuarea;
            button = proxy()->subControlRect(CC_ToolButton, toolbutton, SC_ToolButton, w);
            menuarea = proxy()->subControlRect(CC_ToolButton, toolbutton, SC_ToolButtonMenu, w);

            State flags = toolbutton->state & ~State_Sunken;

            if (flags & State_AutoRaise)
            {
                if (!(flags & State_MouseOver) || !(flags & State_Enabled))
                {
                    flags &= ~State_Raised;
                }
            }
            State mflags = flags;
            if (toolbutton->state & State_Sunken)
            {
                if (toolbutton->activeSubControls & SC_ToolButton)
                {
                    flags |= State_Sunken;
                }
                mflags |= State_Sunken;
            }

            QdfRibbonStyle *rs = (QdfRibbonStyle *) (this);
            bool animation = isAnimationEnabled();
            rs->setAnimationEnabled(false);
            proxy()->drawPrimitive(PE_PanelButtonTool, toolbutton, p, w);
            rs->setAnimationEnabled(animation);

            QStyleOptionToolButton label = *toolbutton;
            label.state = flags;
            proxy()->drawControl(CE_ToolButtonLabel, &label, p, w);

            QPixmap pm;
            QSize pmSize = toolbutton->iconSize;
            if (pmSize.width() > qMin(toolbutton->rect.width(), toolbutton->rect.height()))
            {
                const int iconExtent = proxy()->pixelMetric(PM_SmallIconSize);
                pmSize = QSize(iconExtent, iconExtent);
            }

            if (!toolbutton->icon.isNull())
            {
                QIcon::State state = toolbutton->state & State_On ? QIcon::On : QIcon::Off;
                QIcon::Mode mode;
                if (!(toolbutton->state & State_Enabled))
                {
                    mode = QIcon::Disabled;
                }
                else if ((opt->state & State_MouseOver) && (opt->state & State_AutoRaise))
                {
                    mode = QIcon::Active;
                }
                else
                {
                    mode = QIcon::Normal;
                }
                pm = toolbutton->icon.pixmap(toolbutton->rect.size().boundedTo(pmSize), mode, state);
                pmSize = pm.size();
            }

            if (toolbutton->subControls & SC_ToolButtonMenu)
            {
                QStyleOption tool(0);
                tool.palette = toolbutton->palette;

                QRect ir = menuarea, rcArrow;

                if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon)
                {
                    QString strFirstRow, strSecondRow;
                    splitString(toolbutton->text, strFirstRow, strSecondRow);
                    rcArrow = QRect(QPoint(strSecondRow.isEmpty() ? opt->rect.width() / 2 - 2
                                                                  : opt->rect.right() - 7,
                                           opt->rect.bottom() - 8),
                                    QSize(5, 4));
                }
                else
                {
                    rcArrow = QRect(
                            QPoint((ir.left() + ir.right() - 6) / 2, (ir.top() + ir.bottom() - 5) / 2),
                            QSize(5, 4));
                }

                tool.rect = rcArrow;
                tool.state = mflags;

                proxy()->drawPrimitive(PE_IndicatorArrowDown, &tool, p, w);
            }
            else if (toolbutton->features & QStyleOptionToolButton::HasMenu)
            {
                QStyleOptionToolButton newBtn = *toolbutton;
                if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon)
                {
                    QRect pmRect, strFirsRect, strSecondRect;
                    calcRects(toolbutton->rect, pmSize, toolbutton->fontMetrics, toolbutton->text, true,
                              &pmRect, &strFirsRect, &strSecondRect);

                    QRect r = strSecondRect.isEmpty() ? strFirsRect : strSecondRect;
                    newBtn.rect = QRect(QPoint(r.right() - 5, r.top() + (r.height()) / 2), QSize(5, 5));
                }
                else
                {
                    QRect ir = menuarea;
                    int mbi = proxy()->pixelMetric(PM_MenuButtonIndicator, toolbutton, w);
                    newBtn.rect = QRect(QPoint(ir.right() + 4 - mbi, ir.y() + ir.height() - mbi),
                                        QSize(mbi - 5, mbi - 5));
                }
                proxy()->drawPrimitive(PE_IndicatorArrowDown, &newBtn, p, w);
            }
        }
        return true;
    }
    else
    {
        return QdfOfficeStyle::drawToolButton(opt, p, w);
    }
}


bool QdfRibbonStyle::drawToolBar(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QDF_D(const QdfRibbonStyle);
    return d->ribbonPaintManager()->drawToolBar(opt, p, w);
}


bool QdfRibbonStyle::drawGroupControlEntry(const QStyleOption *opt, QPainter *p,
                                           const QWidget *widget) const
{
    Q_UNUSED(opt);
    Q_UNUSED(p);
    Q_UNUSED(widget);
    return false;
}


bool QdfRibbonStyle::drawIndicatorArrow(PrimitiveElement pe, const QStyleOption *opt, QPainter *p,
                                        const QWidget *w) const
{
    QDF_D(const QdfRibbonStyle);
    return d->ribbonPaintManager()->drawIndicatorArrow(pe, opt, p, w);
}


void QdfRibbonStyle::drawRectangle(QPainter *p, const QRect &rect, bool selected, bool pressed,
                                   bool enabled, bool checked, bool popuped, BarType barType,
                                   BarPosition barPos) const
{
    QDF_D(const QdfRibbonStyle);
    d->ribbonPaintManager()->drawRectangle(p, rect, selected, pressed, enabled, checked, popuped,
                                           barType, barPos);
}


void QdfRibbonStyle::drawSplitButtonPopup(QPainter *p, const QRect &rect, bool selected, bool enabled,
                                          bool popuped) const
{
    Q_UNUSED(p);
    Q_UNUSED(rect);
    Q_UNUSED(selected);
    Q_UNUSED(enabled);
    Q_UNUSED(popuped);
}


void QdfRibbonStyle::drawItemText(QPainter *painter, const QRect &rect, int alignment,
                                  const QPalette &pal, bool enabled, const QString &text,
                                  QPalette::ColorRole textRole) const
{
    if (text.isEmpty())
    {
        return;
    }

    QPen savedPen;
    if (textRole != QPalette::NoRole)
    {
        savedPen = painter->pen();
        painter->setPen(pal.color(textRole));
    }
    if (!enabled)
    {
        QPen pen = painter->pen();
        painter->setPen(pal.light().color());
        painter->setPen(pen);
    }

    painter->drawText(rect, alignment, text);

    if (textRole != QPalette::NoRole)
    {
        painter->setPen(savedPen);
    }
}


void QdfRibbonStyle::drawFillRect(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    QDF_D(const QdfRibbonStyle)
    if (qobject_cast<const QMenu *>(widget ? widget->parentWidget() : nullptr))
    {
        p->fillRect(opt->rect, d->m_clrControlGalleryMenuBk);
        return;
    }

    bool enabled = opt->state & State_Enabled;
    bool selected = opt->state & State_MouseOver;
    p->fillRect(opt->rect,
                selected && enabled ? d->m_clrControlGallerySelected : d->m_clrControlGalleryNormal);
}


void QdfRibbonStyle::drawRect(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    Q_UNUSED(widget)
    QDF_D(const QdfRibbonStyle)

    QPen savePen = p->pen();
    p->setPen(d->m_clrControlGalleryBorder);
    p->drawRect(opt->rect);
    p->setPen(savePen);
}


void QdfRibbonStyle::drawKeyTip(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    QDF_D(const QdfRibbonStyle);
    d->ribbonPaintManager()->drawKeyTip(opt, p, widget);
}


void QdfRibbonStyle::drawBackstage(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    QDF_D(const QdfRibbonStyle);
    d->ribbonPaintManager()->drawBackstage(opt, p, widget);
}


void QdfRibbonStyle::drawRibbonBackstageCloseButton(const QStyleOption *opt, QPainter *p,
                                                    const QWidget *w) const
{
    QDF_D(const QdfRibbonStyle);
    d->ribbonPaintManager()->drawRibbonBackstageCloseButton(opt, p, w);
}


void QdfRibbonStyle::drawRibbonSliderButton(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QDF_D(const QdfRibbonStyle);
    d->ribbonPaintManager()->drawRibbonSliderButton(opt, p, w);
}

template<class T>
static QWidget *getPrevParentWidget(QWidget *pWidget)
{
    if (qobject_cast<T *>(pWidget))
    {
        return nullptr;
    }

    QWidget *pPrevWidget = pWidget;
    while (pWidget)
    {
        pWidget = pWidget->parentWidget();
        if (qobject_cast<T *>(pWidget))
        {
            return pPrevWidget;
        }
        else
        {
            pPrevWidget = pWidget;
        }
    }
    return nullptr;
}


bool QdfRibbonStyle::showToolTip(const QPoint &pos, QWidget *w)
{
    Q_UNUSED(pos);

    if (getParentWidget<QdfRibbonSystemPopupBar>(w))
    {
        return true;
    }

    if (getParentWidget<QdfRibbonBar>(w))
    {
        QPoint p = pos;
        p += QPoint(2,
#ifdef Q_WS_WIN
                    21
#else
                    16
#endif
        );

        int posX = p.x();
        int posY = p.y();

        if (const QdfRibbonGroup *group = getParentWidget<QdfRibbonGroup>(w))
        {
            Q_UNUSED(group);
            posX = w->mapToGlobal(w->rect().topLeft()).x();

            if (QWidget *prevWidget = getPrevParentWidget<QdfRibbonGroup>(w))
            {
                if (QdfRibbonGallery *gallery = qobject_cast<QdfRibbonGallery *>(w))
                {
                    QRect rect = gallery->getDrawItemRect(gallery->selectedItem());
                    posX = prevWidget->mapToGlobal(rect.topLeft()).x();
                }
                else
                {
                    posX = prevWidget->mapToGlobal(prevWidget->rect().topLeft()).x();
                    if (qobject_cast<QdfRibbonToolBarControl *>(prevWidget))
                    {
                        posX = w->mapToGlobal(w->rect().topLeft()).x();
                    }
                }
            }
            if (group->windowFlags() & Qt::Popup)
            {
                posY = group->mapToGlobal(group->rect().bottomRight()).y() +
                       int(QdfDrawHelpers::dpiScaled(2.));
            }
            else if (const QdfRibbonPage *page = getParentWidget<QdfRibbonPage>(w))
            {
                posY = page->mapToGlobal(page->rect().bottomRight()).y() +
                       int(QdfDrawHelpers::dpiScaled(2.));
            }
        }

        QIcon icon;
        QString strTitleText;
        QString strTipText = w->toolTip();

        if (QdfRibbonGallery *gallery = qobject_cast<QdfRibbonGallery *>(w))
        {
            if (QdfRibbonGalleryItem *item = gallery->item(gallery->selectedItem()))
            {
                QString str = item->toolTip();
                if (strTipText != str)
                {
                    strTipText = str;
                }
            }
        }
        else if (strTipText.isEmpty())
        {
            QWidget *widget = w->parentWidget();

            if (!qobject_cast<QdfRibbonGroup *>(widget))
            {
                if (QdfRibbonPageSystemRecentFileList *recentFile =
                            qobject_cast<QdfRibbonPageSystemRecentFileList *>(w))
                {
                    if (QAction *currentAction = recentFile->getCurrentAction())
                    {
                        strTipText = currentAction->data().toString();
                    }
                }
                else
                {
                    while (widget)
                    {
                        strTipText = widget->toolTip();
                        if (!strTipText.isEmpty())
                        {
                            break;
                        }
                        widget = widget->parentWidget();
                        if (qobject_cast<QdfRibbonGroup *>(widget))
                        {
                            break;
                        }
                    }
                }
            }
        }
        else if (QToolButton *button = qobject_cast<QToolButton *>(w))
        {
            if (!strTipText.isEmpty())
            {
                strTipText.remove(QChar('&'));
            }

            icon = button->icon();
            QString str = button->text();
            str.remove(QChar('&'));
            if (strTipText != str)
            {
                strTitleText = str;
            }

            if (QAction *defAction = button->defaultAction())
            {
                QString strShortcuts;
                QKeySequence keySequence;
                QList<QKeySequence> lstShortcuts = defAction->shortcuts();

                foreach (keySequence, lstShortcuts)
                {
                    QString strShortcutString = keySequence.toString(QKeySequence::NativeText);
                    if (strShortcuts != "")
                    {
                        strShortcuts += ", ";
                    }
                    strShortcuts += strShortcutString;
                }
                if (strShortcuts != "")
                {
                    if (strTitleText != "")
                    {
                        strTitleText += " (" + strShortcuts + ")";
                    }
                    else
                    {
                        strTipText += " (" + strShortcuts + ")";
                    }
                }
            }
        }

        if (icon.isNull())
        {
        }

        if (!strTitleText.isEmpty() || !strTipText.isEmpty() || !icon.isNull())
        {
            QPoint globalPos(posX, posY);
            QdfRibbonToolTip::showToolTip(globalPos, strTitleText, strTipText, icon, w);
            return true;
        }
    }
    return false;
}

static QdfRibbonBar *findMainWindow()
{
    foreach (QWidget *widget, qApp->topLevelWidgets())
    {
        if (qobject_cast<QMainWindow *>(widget))
        {
            QList<QdfRibbonBar *> l = widget->findChildren<QdfRibbonBar *>();
            if (l.size() > 0)
            {
                return l.at(0);
            }
        }
    }
    return nullptr;
}


bool QdfRibbonStyle::eventFilter(QObject *watched, QEvent *event)
{
    QDF_D(QdfRibbonStyle);
    switch (event->type())
    {
        case QEvent::HoverMove:
        case QEvent::HoverEnter:
        case QEvent::HoverLeave:
            if (QTabBar *tabBar = qobject_cast<QTabBar *>(watched))
            {
                if (qobject_cast<QMdiArea *>(tabBar->parentWidget()))
                {
                    QDF_D(QdfRibbonStyle);
                    d->tabHoverUpdate(tabBar, event);
                }
            }
            break;
        case QEvent::KeyPress:
            if (QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event))
            {
                d->m_blockKeyTips = false;
                d->m_blockKeyTips = (!keyEvent->modifiers() ||
                                     (keyEvent->modifiers() & (Qt::MetaModifier | Qt::AltModifier))) &&
                                    keyEvent->text().length() == 1;

#ifdef Q_OS_WIN
                if (!d->m_blockKeyTips)
                {
                    quint32 virtualKey = keyEvent->nativeVirtualKey();
                    if (keyEvent->key() != Qt::Key_Alt && ::MapVirtualKey(virtualKey, 2) == 0)
                    {
                        d->m_blockKeyTips = true;
                    }
                }
#endif// Q_OS_WIN

                if (keyEvent->key() == Qt::Key_Alt)
                {
                    d->m_completeKey = static_cast<QKeyEvent *>(event)->text().isEmpty();
                    if (QMainWindow *widget = qobject_cast<QMainWindow *>(watched))
                    {
                        QList<QdfRibbonBar *> l = widget->findChildren<QdfRibbonBar *>();
                        for (int i = 0, count = l.count(); i < count; ++i)
                        {
                            const QdfRibbonBar *rb = l.at(i);
                            if (rb->keyTipsEnabled())
                            {
                                bool hasKeyTips = rb->qdf_d()->m_keyTips.count() > 0;
                                QdfHideKeyTipEvent ktEvent;
                                QApplication::sendEvent(l.at(i), &ktEvent);
                                d->m_destroyKeyTips = hasKeyTips;
                            }
                        }
                    }
                }
            }
            break;
        case QEvent::KeyRelease:
            if (QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event))
            {
                if (keyEvent->key() == Qt::Key_Alt)
                {
                    if (QMainWindow *widget = qobject_cast<QMainWindow *>(watched))
                    {
                        if (!d->m_destroyKeyTips)
                        {
                            QList<QdfRibbonBar *> l = widget->findChildren<QdfRibbonBar *>();
                            for (int i = 0; i < l.size(); ++i)
                            {
                                const QdfRibbonBar *rb = l.at(i);
                                if (!rb->isBackstageVisible() && rb->keyTipsEnabled() &&
                                    !d->m_blockKeyTips)
                                {
                                    QdfShowKeyTipEvent ktEvent(l.at(i));
                                    QApplication::sendEvent(l.at(i), &ktEvent);
                                    //                                    l.at(i)->setFocus();
                                }
                            }
                        }
                        else
                        {
                            d->m_destroyKeyTips = false;
                        }
                    }
                    d->m_completeKey = false;
                }
            }
            break;
        case QEvent::Show:
            if (QMenu *menu = qobject_cast<QMenu *>(watched))
            {
                if (QdfRibbonBar *ribbonBar = ::findMainWindow())
                {
                    if (ribbonBar->qdf_d()->m_levels.size() > 0 ||
                        ((qobject_cast<QdfRibbonPageSystemPopup *>(watched) ||
                          qobject_cast<QdfRibbonSystemPopupBar *>(watched)) &&
                         d->m_completeKey))
                    {
                        QApplication::postEvent(ribbonBar, new QdfShowKeyTipEvent(menu));
                    }
                }
                d->m_completeKey = false;
            }
            break;
            break;
        case QEvent::FontChange:
            if (qobject_cast<QdfRibbonBar *>(watched))
            {
                d->refreshMetrics();

                QWidgetList all = allWidgets();
                for (QWidgetList::ConstIterator it = all.constBegin(); it != all.constEnd(); ++it)
                {
                    QWidget *w = *it;
                    if (w->windowType() != Qt::Desktop && !w->testAttribute(Qt::WA_SetStyle))
                    {
                        QApplication::postEvent(w, new QEvent(QEvent::StyleChange));
                    }
                }
            }
            break;

        default:
            break;
    }
    return QdfOfficeStyle::eventFilter(watched, event);
}


QStringList QdfRibbonStylePlugin::keys() const
{
    return QStringList() << "QdfRibbonStyle";
}

QStyle *QdfRibbonStylePlugin::create(const QString &key)
{
    if (key.toLower() == QLatin1String("ribbonstyle"))
    {
        return new QdfRibbonStyle();
    }
    return nullptr;
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#else
QObject *qt_plugin_instance_ribbonstyle()
{
    static QObject *instance = nullptr;
    if (!instance)
    {
        instance = new QdfRibbonStylePlugin();
    }
    return instance;
}
#endif
