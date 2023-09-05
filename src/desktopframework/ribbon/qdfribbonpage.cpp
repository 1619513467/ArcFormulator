#include "private/qdfribbonbar_p.h"
#include "private/qdfribbongroup_p.h"
#include "private/qdfribbonpage_p.h"
#include "qdfribbon_def.h"
#include <QApplication>
#include <QEvent>
#include <QPainter>
#include <QStyleOption>
#include <ribbon/qdfribbonbar.h>
#include <ribbon/qdfribbongroup.h>
#include <ribbon/qdfribbonpage.h>
#include <ribbon/qdfribbonstyle.h>
#include <ribbon/qdfstylehelpers.h>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    #include <QtMath>
#else
    #include <QtCore/qmath.h>
#endif

#ifdef Q_OS_WIN
    #include <qt_windows.h>
#endif// Q_OS_WIN

QDF_USE_NAMESPACE

static const int pageMarginPopup = 2;
static const int minRibbonWidth = 100;

QdfRibbonPagePrivate::QdfRibbonPagePrivate()
{
    m_animationStep = 0.0;
    m_timerElapse = 0;

    m_buttonScrollGroupLeft = nullptr; // Button to draw left scroll
    m_buttonScrollGroupRight = nullptr;// Button to draw right scroll

    m_title = QdfRibbonBar::tr_compatible(RibbonUntitledString);
    m_minimazeRibbon = false;
    m_doVisiblePage = false;
    m_allowPress = false;
    m_doPopupPage = false;
    m_animation = false;
    m_contextColor = QdfRibbonPage::ContextColorNone;
    m_layoutDirection = Qt::RightToLeft;
    m_associativeTab = nullptr;
    m_groupsHeight = -1;
    m_groupScrollPos = 0;
    m_pageScrollPos = 0;
    m_scrollPosTarget = 0;
}

QdfRibbonPagePrivate::~QdfRibbonPagePrivate()
{
}

void QdfRibbonPagePrivate::init()
{
    QDF_Q(QdfRibbonPage);

    m_buttonScrollGroupLeft = new QdfRibbonGroupScroll(q, true);
    m_buttonScrollGroupLeft->setVisible(false);

    m_buttonScrollGroupRight = new QdfRibbonGroupScroll(q, false);
    m_buttonScrollGroupRight->setVisible(false);

    QObject::connect(m_buttonScrollGroupLeft, SIGNAL(pressed()), this, SLOT(pressLeftScrollButton()));
    QObject::connect(m_buttonScrollGroupRight, SIGNAL(pressed()), this, SLOT(pressRightScrollButton()));

    QObject::connect(m_buttonScrollGroupLeft, SIGNAL(stopScrollTimer()), this,
                     SLOT(forcedStopScrollTimer()));
    QObject::connect(m_buttonScrollGroupRight, SIGNAL(stopScrollTimer()), this,
                     SLOT(forcedStopScrollTimer()));
}

bool QdfRibbonPagePrivate::collapseGroups(int &leftOffset, int actualWidth,
                                          QdfRibbonControlSizeDefinition::GroupSize size, bool adjust)
{
    Q_ASSERT(m_layoutDirection == Qt::RightToLeft || m_layoutDirection == Qt::LeftToRight);

    bool ret = false;
    if (leftOffset > actualWidth)
    {
        QListIterator<QdfRibbonGroup *> iterator(m_listGroups);
        bool normDir = m_layoutDirection == Qt::RightToLeft;
        if (normDir)
        {
            iterator.toBack();
        }

        while (normDir ? iterator.hasPrevious() : iterator.hasNext())
        {
            QdfRibbonGroup *group = normDir ? iterator.previous() : iterator.next();
            ;
            if (!group->isVisible())
            {
                continue;
            }
            if (group->currentSize() <= size)
            {
                int width_0 = group->sizeHint().width();
                if (adjust)
                {
                    group->qdf_d()->adjustCurrentSize(false);
                }
                else
                {
                    group->qdf_d()->reduce();
                }
                int width_1 = group->sizeHint().width();
                int delta = width_0 - width_1;
                leftOffset -= delta;
                ret = ret || (delta != 0);
            }
            if (leftOffset <= actualWidth)
            {
                break;
            }
        }
    }
    return ret;
}

bool QdfRibbonPagePrivate::expandGroups(int &leftOffset, int actualWidth,
                                        QdfRibbonControlSizeDefinition::GroupSize size, bool adjust)
{
    Q_ASSERT(m_layoutDirection == Qt::RightToLeft || m_layoutDirection == Qt::LeftToRight);

    bool ret = false;
    bool normDir = m_layoutDirection == Qt::RightToLeft;
    QListIterator<QdfRibbonGroup *> iterator(m_listGroups);
    if (!normDir)
    {
        iterator.toBack();
    }

    while (normDir ? iterator.hasNext() : iterator.hasPrevious())
    {
        QdfRibbonGroup *group = normDir ? iterator.next() : iterator.previous();
        if (!group->isVisible())
        {
            continue;
        }
        if (group->currentSize() >= size)
        {
            int width_0 = group->sizeHint().width();
            if (adjust)
            {
                group->qdf_d()->adjustCurrentSize(true);
            }
            else
            {
                group->qdf_d()->expand();
            }
            int width_1 = group->sizeHint().width();
            int delta = width_0 - width_1;
            leftOffset -= delta;
            ret = ret || (delta != 0);
        }
        if (leftOffset > actualWidth)
        {
            break;
        }
    }
    return ret;
}

QdfRibbonControlSizeDefinition::GroupSize QdfRibbonPagePrivate::getMinGroupSize() const
{
    QdfRibbonControlSizeDefinition::GroupSize size = QdfRibbonControlSizeDefinition::GroupPopup;
    for (QList<QdfRibbonGroup *>::const_iterator it = m_listGroups.constBegin();
         it != m_listGroups.constEnd(); ++it)
    {
        QdfRibbonGroup *group = *it;
        size = qMin(size, group->currentSize());
        if (size == QdfRibbonControlSizeDefinition::GroupLarge)
        {
            break;
        }
    }
    return size;
}

QdfRibbonControlSizeDefinition::GroupSize QdfRibbonPagePrivate::getMaxGroupSize() const
{
    QdfRibbonControlSizeDefinition::GroupSize size = QdfRibbonControlSizeDefinition::GroupLarge;
    for (QList<QdfRibbonGroup *>::const_iterator it = m_listGroups.constBegin();
         it != m_listGroups.constEnd(); ++it)
    {
        QdfRibbonGroup *group = *it;
        size = qMax(size, group->currentSize());
    }
    return size;
}

bool QdfRibbonPagePrivate::canReduce() const
{
    for (QList<QdfRibbonGroup *>::const_iterator it = m_listGroups.constBegin();
         it != m_listGroups.constEnd(); ++it)
    {
        QdfRibbonGroup *group = *it;

        if (group->qdf_d()->canReduce())
        {
            return true;
        }
    }
    return false;
}

static QdfRibbonControlSizeDefinition::GroupSize
qtc_size_inc(QdfRibbonControlSizeDefinition::GroupSize &size)
{
    QdfRibbonControlSizeDefinition::GroupSize ret = size;
    size = (QdfRibbonControlSizeDefinition::GroupSize)(size + 1);
    return ret;
}

static QdfRibbonControlSizeDefinition::GroupSize
qtc_size_dec(QdfRibbonControlSizeDefinition::GroupSize &size)
{
    QdfRibbonControlSizeDefinition::GroupSize ret = size;
    size = (QdfRibbonControlSizeDefinition::GroupSize)(size - 1);
    return ret;
}

int QdfRibbonPagePrivate::adjustGroups(int leftOffset, int actualWidth)
{
    QdfRibbonControlSizeDefinition::GroupSize size = getMaxGroupSize();
    while (size > QdfRibbonControlSizeDefinition::GroupLarge && leftOffset < actualWidth)
    {
        if (expandGroups(leftOffset, actualWidth, size, true))
        {
            continue;
        }
        if (!expandGroups(leftOffset, actualWidth, qtc_size_dec(size), false))
        {
            break;
        }
    }

    if (leftOffset > actualWidth)
    {
        size = getMinGroupSize();
        while (canReduce() && leftOffset > actualWidth)
        {
            if (collapseGroups(leftOffset, actualWidth, size, true))
            {
                continue;
            }
            if (!collapseGroups(leftOffset, actualWidth, qtc_size_inc(size), false))
            {
                break;
            }
        }
    }
    return leftOffset;
}

int QdfRibbonPagePrivate::calculateGroupsWidth()
{
    int width = 0;
    for (int i = 0, count = m_listGroups.size(); i < count; ++i)
    {
        QdfRibbonGroup *group = m_listGroups.at(i);
        if (!group->isVisible())
        {
            continue;
        }
        width += group->sizeHint().width();
    }
    return width;
}

void QdfRibbonPagePrivate::updateLayout(bool updateScroll)
{
    QDF_Q(QdfRibbonPage);

    int leftOffset = 0;
    int actualWidth = q->width();
    if (QdfRibbonBar *ribbon = qobject_cast<QdfRibbonBar *>(q->parentWidget()))
    {
        if (!(q->windowFlags() & Qt::Popup) && ribbon->qdf_d()->m_logotypeLabel->isVisible())
        {
            QRect rectLogotype = ribbon->qdf_d()->m_logotypeLabel->geometry();
            if (!rectLogotype.isNull())
            {
                actualWidth -= rectLogotype.width();

                if (ribbon->qdf_d()->m_logotypeLabel->alignmentLogotype() == Qt::AlignLeft)
                {
                    leftOffset += rectLogotype.width();
                }
            }
        }
    }

    bool IsMinimized = actualWidth < minRibbonWidth;
    if (!IsMinimized)
    {
        int gropsWidth = calculateGroupsWidth() + leftOffset;
        adjustGroups(gropsWidth, actualWidth);
    }

    static int margin = 4;
    for (QList<QdfRibbonGroup *>::iterator it = m_listGroups.begin(); it != m_listGroups.end(); ++it)
    {
        QdfRibbonGroup *group = *(it);
        if (!group->isVisible())
        {
            continue;
        }

        if (it == m_listGroups.begin())
        {
            leftOffset += margin;
        }

        QSize groupSizeHint = group->sizeHint();

        //        groupSizeHint.rwidth() -= 52;

        int controlsLeftOffset = 0;
        QWidget *parentWidget = group;
        if (group->isReduced())
        {
            // Is the group in a popup stage then create a sub-window RibbonGroupPopup
            group->qdf_d()->reposition(QRect(QPoint(-m_groupScrollPos + leftOffset, margin - 1),
                                             QSize(groupSizeHint.width(), q->size().height())));
            Q_ASSERT(group->qdf_d()->m_groupPopup != 0);
            parentWidget = group->qdf_d()->m_groupPopup;
        }

        if (!(group->controlsAlignment() & Qt::AlignLeft))
        {
            QSize contentSize = group->qdf_d()->updateControlsLayout(nullptr);
            QSize parentSize = parentWidget->sizeHint();
            controlsLeftOffset = QStyle::alignedRect(Qt::LeftToRight, group->controlsAlignment(),
                                                     contentSize, QRect(QPoint(0, 0), parentSize))
                                         .left();
        }

        group->qdf_d()->updateControlsLayout(parentWidget, controlsLeftOffset);

        if (!group->isReduced())
        {
            group->qdf_d()->reposition(QRect(QPoint(-m_groupScrollPos + leftOffset, margin - 1),
                                             QSize(groupSizeHint.width(), q->size().height())));
        }

        group->qdf_d()->updateOptionButtonLayout();
        leftOffset += groupSizeHint.width();
    }

    if (updateScroll)
    {
        showGroupScroll(false);
    }
}

void QdfRibbonPagePrivate::removeGroup(int index, bool deleteGroup)
{
    if (validateGroupIndex(index))
    {
        QdfRibbonGroup *group = m_listGroups.at(index);

        group->removeEventFilter(this);
        QList<QAction *> actList = group->actions();

        for (int i = actList.size() - 1; i >= 0; --i)
        {
            m_listShortcuts.removeOne(actList.at(i));
        }

        m_listGroups.removeAt(index);

        if (deleteGroup)
        {
            delete group;
        }
        else
        {
            group->setParent(nullptr);
        }

        updateLayout();
    }
    else
    {
        Q_ASSERT(false);
    }
}

int QdfRibbonPagePrivate::groupIndex(QdfRibbonGroup *group) const
{
    return m_listGroups.indexOf(group);
}

int QdfRibbonPagePrivate::calcReducedGroupsWidth() const
{
    QDF_Q(const QdfRibbonPage);
    static int margin = 4;
    int totalWidth = 0;
    for (QList<QdfRibbonGroup *>::const_iterator it = m_listGroups.constBegin();
         it != m_listGroups.constEnd(); ++it)
    {
        QdfRibbonGroup *group = *(it);
        if (!group->isVisible())
        {
            continue;
        }

        if (group->qdf_d()->canReduce())
        {
            totalWidth = 0;
            break;
        }

        if (it == m_listGroups.begin())
        {
            totalWidth = margin * 2;
        }

        QRect rect = group->geometry();
        totalWidth += rect.width();
    }

    int widthLogotype = 0;
    if (QdfRibbonBar *ribbon = qobject_cast<QdfRibbonBar *>(q->parentWidget()))
    {
        if (!(q->windowFlags() & Qt::Popup) && ribbon->qdf_d()->m_logotypeLabel->isVisible())
        {
            QRect rectLogotype = ribbon->qdf_d()->m_logotypeLabel->geometry();
            if (!rectLogotype.isNull())
            {
                widthLogotype = rectLogotype.width();
            }
        }
    }

    return totalWidth + widthLogotype;
}

void QdfRibbonPagePrivate::enableGroupScroll(bool scrollLeft, bool scrollRight)
{
    QDF_Q(QdfRibbonPage);

    Q_ASSERT(m_buttonScrollGroupLeft);
    Q_ASSERT(m_buttonScrollGroupRight);

    if (!m_buttonScrollGroupRight || !m_buttonScrollGroupLeft)
    {
        return;
    }

    QRect rcPage(q->geometry());

    if (scrollLeft)
    {
        m_buttonScrollGroupLeft->raise();
        m_buttonScrollGroupLeft->setVisible(true);

        int leftOffset = 0;
        if (QdfRibbonBar *ribbon = qobject_cast<QdfRibbonBar *>(q->parentWidget()))
        {
            if (!(q->windowFlags() & Qt::Popup) && ribbon->qdf_d()->m_logotypeLabel->isVisible() &&
                ribbon->qdf_d()->m_logotypeLabel->alignmentLogotype() == Qt::AlignLeft)
            {
                QRect rectLogotype = ribbon->qdf_d()->m_logotypeLabel->geometry();
                if (!rectLogotype.isNull())
                {
                    leftOffset = rectLogotype.width() - 2;
                }
            }
        }

        QRect rc(QPoint(0 + leftOffset - 1, 1), QSize(12, rcPage.height() - 1));

        if ((q->windowFlags() & Qt::Popup))
        {
            rc.setHeight(rc.height() + pageMarginPopup);
        }

        m_buttonScrollGroupLeft->setGeometry(rc);
    }
    else
    {
        m_buttonScrollGroupLeft->setDown(false);
        m_buttonScrollGroupLeft->setVisible(false);
    }

    if (scrollRight)
    {
        m_buttonScrollGroupRight->raise();
        m_buttonScrollGroupRight->setVisible(true);

        int leftOffset = 0;
        if (QdfRibbonBar *ribbon = qobject_cast<QdfRibbonBar *>(q->parentWidget()))
        {
            if (!(q->windowFlags() & Qt::Popup) && ribbon->qdf_d()->m_logotypeLabel->isVisible() &&
                ribbon->qdf_d()->m_logotypeLabel->alignmentLogotype() == Qt::AlignRight)
            {
                QRect rectLogotype = ribbon->qdf_d()->m_logotypeLabel->geometry();
                if (!rectLogotype.isNull())
                {
                    leftOffset = rectLogotype.width() - 3;
                }
            }
        }

        QRect rc(QPoint(rcPage.width() - leftOffset - 12 + 1, 1), QSize(12, rcPage.height() - 1));

        if ((q->windowFlags() & Qt::Popup))
        {
            rc.setHeight(rc.height() + pageMarginPopup);
        }

        m_buttonScrollGroupRight->setGeometry(rc);
    }
    else
    {
        m_buttonScrollGroupRight->setDown(false);
        m_buttonScrollGroupRight->setVisible(false);
    }
    if (!m_buttonScrollGroupLeft->isVisible() && !m_buttonScrollGroupRight->isVisible())
    {
        m_pageScrollPos = 0;
    }
}

void QdfRibbonPagePrivate::showGroupScroll(bool onlyCalc)
{
    QDF_Q(QdfRibbonPage);
    int totalWidth = calcReducedGroupsWidth();
    if (totalWidth == 0 && !m_buttonScrollGroupLeft->isVisible() &&
        !m_buttonScrollGroupRight->isVisible())
    {
        return;
    }

    int groupsLength = q->width();
    int scrollPos = m_groupScrollPos;

    if (totalWidth > groupsLength)
    {
        if (scrollPos > totalWidth - groupsLength)
        {
            scrollPos = totalWidth - groupsLength;
        }
    }
    else
    {
        scrollPos = 0;
    }

    if (scrollPos < 0)
    {
        scrollPos = 0;
    }

    m_groupScrollPos = scrollPos;
    if (!onlyCalc)
    {
        enableGroupScroll(scrollPos > 0, totalWidth - groupsLength - scrollPos > 0);
    }
}

void QdfRibbonPagePrivate::scrollGroupAnimate()
{
    if (qAbs(m_groupScrollPos - m_scrollPosTarget) > qFabs(m_animationStep))
    {
        m_groupScrollPos = int((double) m_groupScrollPos + m_animationStep);
        updateLayout(false);
    }
    else
    {
        m_animation = false;
        m_groupScrollPos = m_scrollPosTarget;
        m_timerElapse = 0;
        m_scrollTimer.stop();
        showGroupScroll(false);
    }
}

void QdfRibbonPagePrivate::startScrollGropsAnimate(int groupScrollPos, int scrollPosTarget)
{
    QDF_Q(QdfRibbonPage)
    m_animation = true;
    m_scrollPosTarget = scrollPosTarget;
    m_groupScrollPos = groupScrollPos;
    m_timerElapse = QApplication::doubleClickInterval() * 4 / 5;
    m_animationStep = double(m_scrollPosTarget - m_groupScrollPos) / (m_timerElapse > 200 ? 8.0 : 3.0);

    if (m_animationStep > 0 && m_animationStep < 1)
    {
        m_animationStep = 1;
    }
    if (m_animationStep < 0 && m_animationStep > -1)
    {
        m_animationStep = -1;
    }

    int nms = 40;
    m_scrollTimer.start(nms, q);
    scrollGroupAnimate();
}

void QdfRibbonPagePrivate::listPageWidth(int totalWidth, int realWidth, QList<int> &pagesWidth)
{
    if (totalWidth > realWidth)
    {
        pagesWidth << realWidth;
        listPageWidth(totalWidth - realWidth, realWidth, pagesWidth);
    }
    else
    {
        pagesWidth << totalWidth;
    }
    return;
}

void QdfRibbonPagePrivate::pressLeftScrollButton()
{
    QDF_Q(QdfRibbonPage)
    QList<int> pagesWidth;
    listPageWidth(calcReducedGroupsWidth(), q->width(), pagesWidth);

    if (0 < m_pageScrollPos)
    {
        m_pageScrollPos--;
    }

    int scrollPos = m_groupScrollPos;
    m_groupScrollPos -= pagesWidth[m_pageScrollPos];
    showGroupScroll(true);
    startScrollGropsAnimate(scrollPos, m_groupScrollPos);
}

void QdfRibbonPagePrivate::pressRightScrollButton()
{
    QDF_Q(QdfRibbonPage)
    QList<int> pagesWidth;
    listPageWidth(calcReducedGroupsWidth(), q->width(), pagesWidth);

    m_groupScrollPos += pagesWidth[m_pageScrollPos];
    showGroupScroll(true);

    if (pagesWidth.size() - 1 > m_pageScrollPos)
    {
        m_pageScrollPos++;
    }

    startScrollGropsAnimate(0, m_groupScrollPos);
}

void QdfRibbonPagePrivate::forcedStopScrollTimer()
{
    m_scrollTimer.stop();
    showGroupScroll(false);
}

bool QdfRibbonPagePrivate::eventFilter(QObject *obj, QEvent *event)
{
    bool res = QObject::eventFilter(obj, event);

    if (!qobject_cast<QdfRibbonGroup *>(obj))
    {
        return res;
    }

    switch (event->type())
    {
        case QEvent::ActionAdded:
            if (QActionEvent *actEvent = static_cast<QActionEvent *>(event))
            {
                m_listShortcuts.append(actEvent->action());
                if (m_associativeTab)
                {
                    m_associativeTab->addAction(actEvent->action());
                }
            }
            break;
        case QEvent::ActionRemoved:
            if (QActionEvent *actEvent = static_cast<QActionEvent *>(event))
            {
                if (m_associativeTab)
                {
                    m_associativeTab->removeAction(actEvent->action());
                }

                int index = m_listShortcuts.indexOf(actEvent->action());
                if (index != -1)
                {
                    m_listShortcuts.removeAt(index);
                }
            }
            break;
        default:
            break;
    }

    return res;
}

QdfRibbonPage::QdfRibbonPage(QWidget *parent) : QWidget(parent)
{
    QDF_INIT_PRIVATE(QdfRibbonPage);
    QDF_D(QdfRibbonPage);
    d->init();
}

QdfRibbonPage::QdfRibbonPage(QdfRibbonBar *ribbonBar, const QString &title) : QWidget(ribbonBar)
{
    QDF_INIT_PRIVATE(QdfRibbonPage);
    QDF_D(QdfRibbonPage);
    d->init();
    setTitle(title);
}

QdfRibbonPage::~QdfRibbonPage()
{
    QDF_D(QdfRibbonPage);
    if (d->m_associativeTab)
    {
        d->m_associativeTab->setPage(nullptr);
    }

    if (QdfRibbonBar *ribbon = qobject_cast<QdfRibbonBar *>(parentWidget()))
    {
        ribbon->detachPage(this);
    }
    QDF_FINI_PRIVATE();
}

bool QdfRibbonPage::isVisible() const
{
    QDF_D(const QdfRibbonPage)
    return QWidget::isVisible() || (d->m_associativeTab && d->m_associativeTab->isVisible());
}

void QdfRibbonPage::addGroup(QdfRibbonGroup *group)
{
    insertGroup(-1, group);
}

QdfRibbonGroup *QdfRibbonPage::addGroup(const QString &title)
{
    return insertGroup(-1, title);
}

QdfRibbonGroup *QdfRibbonPage::addGroup(const QIcon &icon, const QString &title)
{
    return insertGroup(-1, icon, title);
}

void QdfRibbonPage::insertGroup(int index, QdfRibbonGroup *group)
{
    QDF_D(QdfRibbonPage);

    group->setParent(this);

    if (QdfRibbonBar *ribbonBar = qobject_cast<QdfRibbonBar *>(parentWidget()))
    {
        group->setProperty(_qdf_TitleGroupsVisible, ribbonBar->isTitleGroupsVisible());
    }

    if (!d->validateGroupIndex(index))
    {
        index = d->m_listGroups.count();
        d->m_listGroups.append(group);
    }
    else
    {
        d->m_listGroups.insert(index, group);
    }

    connect(group, SIGNAL(actionTriggered(QAction *)), this, SLOT(actionTriggered(QAction *)),
            Qt::QueuedConnection);
    connect(group, SIGNAL(released()), this, SLOT(released()));

    group->show();
    d->updateLayout();
}

QdfRibbonGroup *QdfRibbonPage::insertGroup(int index, const QString &title)
{
    QdfRibbonGroup *group = new QdfRibbonGroup(nullptr, title);
    insertGroup(index, group);
    return group;
}

QdfRibbonGroup *QdfRibbonPage::insertGroup(int index, const QIcon &icon, const QString &title)
{
    if (QdfRibbonGroup *group = insertGroup(index, title))
    {
        group->setIcon(icon);
        return group;
    }
    return nullptr;
}

void QdfRibbonPage::removeGroup(QdfRibbonGroup *group)
{
    QDF_D(QdfRibbonPage);
    removeGroup(d->groupIndex(group));
}

void QdfRibbonPage::removeGroup(int index)
{
    QDF_D(QdfRibbonPage);
    d->removeGroup(index, true);
}

void QdfRibbonPage::detachGroup(QdfRibbonGroup *group)
{
    QDF_D(QdfRibbonPage);
    detachGroup(d->groupIndex(group));
}

void QdfRibbonPage::detachGroup(int index)
{
    QDF_D(QdfRibbonPage);
    d->removeGroup(index, false);
}

void QdfRibbonPage::clearGroups()
{
    QDF_D(QdfRibbonPage);
    for (int i = (int) d->m_listGroups.count() - 1; i >= 0; i--)
    {
        removeGroup(i);
    }
}

QAction *QdfRibbonPage::defaultAction() const
{
    QDF_D(const QdfRibbonPage);
    if (d->m_associativeTab)
    {
        return d->m_associativeTab->defaultAction();
    }
    return nullptr;
}

int QdfRibbonPage::groupCount() const
{
    QDF_D(const QdfRibbonPage);
    return d->m_listGroups.count();
}

QdfRibbonGroup *QdfRibbonPage::getGroup(int index) const
{
    QDF_D(const QdfRibbonPage);
    if (index < 0 || index >= d->m_listGroups.size())
    {
        return nullptr;
    }
    return d->m_listGroups[index];
}

int QdfRibbonPage::groupIndex(QdfRibbonGroup *group) const
{
    QDF_D(const QdfRibbonPage);
    Q_ASSERT(group != nullptr);
    if (group && d->m_listGroups.contains(group))
    {
        return d->m_listGroups.indexOf(group);
    }
    return -1;
}

QList<QdfRibbonGroup *> QdfRibbonPage::groups() const
{
    QDF_D(const QdfRibbonPage);
    return d->m_listGroups;
}

void QdfRibbonPage::setContextColor(ContextColor color)
{
    QDF_D(QdfRibbonPage);
    d->m_contextColor = color;

    if (d->m_associativeTab)
    {
        d->m_associativeTab->setContextTab(color);
        update();
        if (parentWidget())
        {
            parentWidget()->update();
        }
#ifdef Q_OS_WIN
        if (QdfRibbonBar *ribbonBar = qobject_cast<QdfRibbonBar *>(parentWidget()))
        {
            ribbonBar->updateWindowTitle();
        }
#endif// Q_OS_WIN
    }
}

QdfRibbonPage::ContextColor QdfRibbonPage::contextColor() const
{
    QDF_D(const QdfRibbonPage);
    return d->m_contextColor;
}

void QdfRibbonPage::setVisible(bool visible)
{
    QDF_D(QdfRibbonPage);
    if (d->m_associativeTab)
    {
        d->m_associativeTab->setContextTab(d->m_contextColor);
        d->m_associativeTab->setContextTextTab(d->m_contextTitle);
    }

    bool saveVisible = isVisible();

    if (d->m_minimazeRibbon && d->m_associativeTab)
    {
        if (d->m_doPopupPage)
        {
            QWidget::setVisible(visible);
        }

        if (!d->m_doPopupPage)
        {
            d->m_associativeTab->setVisible(visible);
            if (saveVisible != isVisible())
            {
#ifdef Q_OS_WIN
                if (QdfRibbonBar *ribbonBar = qobject_cast<QdfRibbonBar *>(parentWidget()))
                {
                    ribbonBar->updateWindowTitle();
                }
#endif// Q_OS_WIN
            }
        }

        if (!visible && d->m_doPopupPage)
        {
            d->m_doPopupPage = false;
        }
    }
    else
    {
        if (d->m_doVisiblePage)
        {
            QWidget::setVisible(visible);
        }
        else if (!visible)
        {
            QWidget::setVisible(false);
        }

        if (!d->m_doVisiblePage && d->m_associativeTab)
        {
            d->m_associativeTab->setVisible(visible);
        }

        if (QdfRibbonBar *ribbonBar = qobject_cast<QdfRibbonBar *>(parentWidget()))
        {
            if (!d->m_doVisiblePage && !visible)
            {
                bool setCurrentPage = false;
                int index = ribbonBar->currentPageIndex();
                if (index >= 0 && index < ribbonBar->getPageCount())
                {
                    QdfRibbonPage *page = ribbonBar->getPage(index);
                    if (page->isVisible())
                    {
                        ribbonBar->setCurrentPageIndex(index);
                        setCurrentPage = true;
                    }
                }

                for (int i = 0, count = ribbonBar->getPageCount(); count > i && !setCurrentPage; ++i)
                {
                    QdfRibbonPage *page = ribbonBar->getPage(i);
                    if (page->qdf_d()->m_associativeTab && page->qdf_d()->m_associativeTab->isVisible())
                    {
                        ribbonBar->setCurrentPageIndex(i);
                        break;
                    }
                }
            }
            else if (visible && saveVisible != isVisible())
            {
                for (int i = 0, count = ribbonBar->getPageCount(); count > i; ++i)
                {
                    if (i == ribbonBar->currentPageIndex())
                    {
                        QdfRibbonPage *page = ribbonBar->getPage(i);
                        if (page == this && page->qdf_d()->m_associativeTab->isVisible())
                        {
                            QWidget::setVisible(visible);
                            break;
                        }
                    }
                }
                ribbonBar->updateLayout();
            }

            if (d->m_associativeTab && d->m_contextColor != QdfRibbonPage::ContextColorNone &&
                saveVisible != visible)
            {
#ifdef Q_OS_WIN
                if (QdfRibbonBar *ribbonBar = qobject_cast<QdfRibbonBar *>(parentWidget()))
                {
                    ribbonBar->updateWindowTitle();
                }
#endif// Q_OS_WIN
            }
        }
    }
}

void QdfRibbonPage::setTitle(const QString &title)
{
    QDF_D(QdfRibbonPage);

    if (d->m_title == title)
    {
        return;
    }

    d->m_title = title;

    if (d->m_associativeTab)
    {
        d->m_associativeTab->setTextTab(d->m_title);
    }

    emit titleChanged(d->m_title);
}

const QString &QdfRibbonPage::title() const
{
    QDF_D(const QdfRibbonPage);
    return d->m_associativeTab ? d->m_associativeTab->textTab() : d->m_title;
}

void QdfRibbonPage::setContextTitle(const QString &title)
{
    QDF_D(QdfRibbonPage);
    d->m_contextTitle = title;

    if (d->m_associativeTab)
    {
        d->m_associativeTab->setContextTextTab(d->m_contextTitle);
#ifdef Q_OS_WIN
        if (QdfRibbonBar *ribbonBar = qobject_cast<QdfRibbonBar *>(parentWidget()))
        {
            ribbonBar->updateWindowTitle();
        }
#endif// Q_OS_WIN
    }
}

void QdfRibbonPage::released()
{
    QDF_D(QdfRibbonPage);
    if (d->m_minimazeRibbon && isVisible() && QApplication::activePopupWidget() == this)
    {
        hide();
    }
}

void QdfRibbonPage::actionTriggered(QAction *action)
{
    QDF_D(QdfRibbonPage);

    if (d->m_listShortcuts.indexOf(action) != -1)
    {
        return;
    }

    if (d->m_minimazeRibbon && isVisible())
    {
        if (action->menu())
        {
            return;
        }
        close();
    }
}

const QString &QdfRibbonPage::contextTitle() const
{
    QDF_D(const QdfRibbonPage);
    return d->m_associativeTab ? d->m_associativeTab->contextTextTab() : d->m_title;
}

void QdfRibbonPage::setContextGroupName(const QString &groupName)
{
    QDF_D(QdfRibbonPage);
    d->m_contextGroupName = groupName;

    if (d->m_associativeTab)
    {
        d->m_associativeTab->setContextGroupName(d->m_contextGroupName);
#ifdef Q_OS_WIN
        if (QdfRibbonBar *ribbonBar = qobject_cast<QdfRibbonBar *>(parentWidget()))
        {
            ribbonBar->updateWindowTitle();
        }
#endif// Q_OS_WIN
    }
}

void QdfRibbonPage::setTabWidth(int width)
{
    QDF_D(QdfRibbonPage);
    if (d->m_associativeTab == nullptr)
    {
        return;
    }
    d->m_associativeTab->setTabWidth(width);
}

int QdfRibbonPage::tabWidth() const
{
    QDF_D(const QdfRibbonPage);
    if (d->m_associativeTab == nullptr)
    {
        return -1;
    }
    return d->m_associativeTab->tabWidth();
}

void QdfRibbonPage::updateLayout()
{
    QDF_D(QdfRibbonPage);
    d->updateLayout();
}

void QdfRibbonPage::setAssociativeTab(QWidget *widget)
{
    QDF_D(QdfRibbonPage);
    d->m_associativeTab = qobject_cast<QdfRibbonTab *>(widget);

    if (!d->m_associativeTab)
    {
        return;
    }

    d->m_associativeTab->setPage(this);
    d->m_associativeTab->setContextTab(d->m_contextColor);
    d->m_associativeTab->setContextTextTab(d->m_contextTitle);

    for (int j = 0, count = groupCount(); count > j; ++j)
    {
        if (QdfRibbonGroup *group = getGroup(j))
        {
            QList<QAction *> actList = group->actions();

            for (int i = 0, actCount = actList.size(); actCount > i; ++i)
            {
                QAction *a = actList.at(i);
                d->m_listShortcuts.append(a);
                d->m_associativeTab->addAction(a);
            }
        }
    }
}

QWidget *QdfRibbonPage::associativeTab() const
{
    QDF_D(const QdfRibbonPage);
    return d->m_associativeTab;
}

void QdfRibbonPage::setPageVisible(bool visible)
{
    QDF_D(QdfRibbonPage);
    d->m_doVisiblePage = true;
    if (visible && d->m_associativeTab && !d->m_associativeTab->isHidden())
    {
        setVisible(true);
        emit activated();
    }
    else if (d->m_associativeTab && !d->m_associativeTab->isHidden())
    {
        setVisible(false);
    }
    d->m_doVisiblePage = false;
}

void QdfRibbonPage::popup()
{
    QDF_D(QdfRibbonPage);
    d->m_doPopupPage = true;
    setVisible(true);
}

void QdfRibbonPage::setRibbonMinimized(bool minimized)
{
    QDF_D(QdfRibbonPage);
    if (d->m_minimazeRibbon != minimized)
    {
        d->m_minimazeRibbon = minimized;
    }
}

void QdfRibbonPage::activatingPage(bool &allow)
{
    emit activating(allow);
}

void QdfRibbonPage::setGroupsHeight(int height)
{
    QDF_D(QdfRibbonPage);
    d->m_groupsHeight = height;
}

bool QdfRibbonPage::event(QEvent *event)
{
    QDF_D(QdfRibbonPage);
    switch (event->type())
    {
        case QEvent::LayoutRequest:
            d->updateLayout();
            break;
        case QEvent::Show:
            if (d->m_minimazeRibbon && d->m_associativeTab)
            {
                d->m_associativeTab->setTrackingMode(true);
                d->m_timer.start(QApplication::doubleClickInterval(), this);
                d->m_allowPress = true;
            }
            break;
        case QEvent::Hide:
            if (d->m_minimazeRibbon && d->m_associativeTab)
            {
                d->m_associativeTab->setTrackingMode(false);
                d->m_allowPress = true;
            }
            break;
        case QEvent::Timer:
            {
                QTimerEvent *timerEvent = static_cast<QTimerEvent *>(event);
                if (d->m_minimazeRibbon && d->m_associativeTab)
                {
                    if (d->m_timer.timerId() == timerEvent->timerId())
                    {
                        d->m_allowPress = false;
                        return true;
                    }
                }
                if (d->m_scrollTimer.timerId() == timerEvent->timerId())
                {
                    d->scrollGroupAnimate();
                    event->accept();
                    return true;
                }
            }
            break;
        case QEvent::StyleChange:
        case QEvent::FontChange:
            {
                qdf_set_font_to_ribbon_children(this, font());
            }
            break;
        case QEvent::ChildAdded:
            {
                QChildEvent *childEvent = static_cast<QChildEvent *>(event);
                if (QdfRibbonGroup *group = qobject_cast<QdfRibbonGroup *>(childEvent->child()))
                {
                    group->installEventFilter(d);
                }

                if (childEvent->added())
                {
                    QWidget *widget = qobject_cast<QWidget *>(childEvent->child());
                    if (widget != nullptr)
                    {
                        if (qobject_cast<QdfRibbonGroup *>(widget) != nullptr)
                        {
                            qobject_cast<QdfRibbonGroup *>(widget)->setFont(font());
                        }
                        else
                        {
                            widget->setFont(font());
                            qdf_set_font_to_ribbon_children(widget, font());
                        }
                    }
                }
            }
            break;
        case QEvent::ChildRemoved:
            {
                QChildEvent *childEvent = static_cast<QChildEvent *>(event);
                if (QdfRibbonGroup *group = qobject_cast<QdfRibbonGroup *>(childEvent->child()))
                {
                    group->removeEventFilter(d);
                }
            }
            break;
        case QEvent::MouseButtonPress:
            if (d->m_minimazeRibbon && d->m_associativeTab)
            {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
                if (!rect().contains(mouseEvent->pos()))
                {
                    if (d->m_allowPress)
                    {
                        QWidget *clickedWidget = QApplication::widgetAt(mouseEvent->globalPos());

                        if (clickedWidget == d->m_associativeTab)
                        {
                            const QPoint targetPoint =
                                    clickedWidget->mapFromGlobal(mouseEvent->globalPos());

                            QMouseEvent evPress(mouseEvent->type(), targetPoint, mouseEvent->globalPos(),
                                                mouseEvent->button(), mouseEvent->buttons(),
                                                mouseEvent->modifiers());
                            QApplication::sendEvent(clickedWidget, &evPress);

                            QMouseEvent eDblClick(QEvent::MouseButtonDblClick, targetPoint,
                                                  mouseEvent->globalPos(), mouseEvent->button(),
                                                  mouseEvent->buttons(), mouseEvent->modifiers());
                            QApplication::sendEvent(d->m_associativeTab, &eDblClick);
                            return true;
                        }
                    }
                    else if (d->m_associativeTab && QRect(d->m_associativeTab->mapToGlobal(QPoint()),
                                                          d->m_associativeTab->size())
                                                            .contains(mouseEvent->globalPos()))
                    {
                        setAttribute(Qt::WA_NoMouseReplay);
                    }

                    if (QApplication::activePopupWidget() == this)
                    {
                        if (QWidget *clickedTab = qobject_cast<QdfRibbonTab *>(
                                    QApplication::widgetAt(mouseEvent->globalPos())))
                        {
                            if (d->m_associativeTab && clickedTab != d->m_associativeTab)
                            {
                                const QPoint targetPoint =
                                        clickedTab->mapFromGlobal(mouseEvent->globalPos());

                                QMouseEvent evPress(mouseEvent->type(), targetPoint,
                                                    mouseEvent->globalPos(), mouseEvent->button(),
                                                    mouseEvent->buttons(), mouseEvent->modifiers());
                                QApplication::sendEvent(clickedTab, &evPress);

                                return false;
                            }
                        }
                    }
                }
            }
            break;
        default:
            break;
    }

    return QWidget::event(event);
}

QSize QdfRibbonPage::sizeHint() const
{
    QSize resultSize = QWidget::sizeHint();

    // Calculate a max height of the group. Group may contain a different number of rows(3 - rows, 5 - rows)
    int maxHeight = 0;
    for (int i = 0, count = groupCount(); count > i; i++)
    {
        if (QdfRibbonGroup *group = getGroup(i))
        {
            maxHeight = qMax(maxHeight, group->sizeHint().height());
        }
    }
    resultSize.setHeight(maxHeight);

    if ((windowFlags() & Qt::Popup))
    {
        resultSize.setHeight(resultSize.height() - pageMarginPopup);
    }

    return resultSize;
}

void QdfRibbonPage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if ((windowFlags() & Qt::Popup))
    {
        QPainter p(this);
        QdfStyleOptionRibbon opt;
        opt.init(this);
        opt.rect.adjust(0, 0, 0, pageMarginPopup);
        style()->drawControl((QStyle::ControlElement) QdfRibbonStyle::CE_RibbonGroups, &opt, &p,
                             this->parentWidget());
    }
}

void QdfRibbonPage::changeEvent(QEvent *event)
{
    QDF_D(QdfRibbonPage);
    switch (event->type())
    {
        case QEvent::StyleChange:
            d->updateLayout();
            break;
        case QEvent::FontChange:
            {
                for (int i = 0; i < d->m_listGroups.size(); i++)
                {
                    QdfRibbonGroup *group = d->m_listGroups.at(i);
                    group->setFont(font());
                }
                d->updateLayout();
            }
            break;
        default:
            break;
    };
    return QWidget::changeEvent(event);
}

void QdfRibbonPage::resizeEvent(QResizeEvent *event)
{
    QDF_D(QdfRibbonPage);
    QWidget::resizeEvent(event);

    if ((windowFlags() & Qt::Popup))
    {
        QSize delta = event->size() - event->oldSize();
        if (delta.isNull())
        {
            return;
        }
        d->updateLayout();
    }
}

#ifdef Q_OS_WIN
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
bool QdfRibbonPage::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    bool res = QWidget::nativeEvent(eventType, message, result);
    MSG *msg = static_cast<MSG *>(message);
    #else
bool QdfRibbonPage::winEvent(MSG *message, long *result)
{
    bool res = QWidget::winEvent(message, result);
    MSG *msg = static_cast<MSG *>(message);
    #endif
    if (!res)
    {
        if (msg->message == WM_LBUTTONDOWN)
        {
            if (QWidget *activePopupWidget = QApplication::activePopupWidget())
            {
                if (activePopupWidget == this)
                {
                    POINT curPos = msg->pt;
                    QPoint globalPos(curPos.x, curPos.y);

                    QPoint pos = mapFromGlobal(globalPos);

                    QToolButton *toolButton =
                            qobject_cast<QToolButton *>(activePopupWidget->childAt(pos));

                    if (toolButton && toolButton->isEnabled())
                    {
                        if (QAction *action = toolButton->defaultAction())
                        {
                            if (action->menu())
                            {
                                int button = Qt::LeftButton;
                                int state = 0;

                                pos = toolButton->mapFromGlobal(globalPos);

                                QMouseEvent e(QEvent::MouseButtonPress, pos, globalPos,
                                              Qt::MouseButton(button),
                                              Qt::MouseButtons(state & Qt::MouseButtonMask),
                                              Qt::KeyboardModifiers(state & Qt::KeyboardModifierMask));

                                res = QApplication::sendEvent(toolButton, &e);
                                res = res && e.isAccepted();
                            }
                        }
                    }
                }
            }
        }
    }
    return res;
}
#endif