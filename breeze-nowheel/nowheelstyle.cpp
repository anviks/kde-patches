// Breeze-NoWheel: a Qt widget style that looks exactly like Breeze, but stops the
// mouse wheel from changing dropdowns and spinboxes (and sliders inside scrolling
// pages). The wheel scrolls the page instead.
//
// Environment variables (optional):
//   NOWHEEL_BASE_STYLE=Breeze    style to wrap (default Breeze, falls back to Fusion)
//   NOWHEEL_ALLOW_FOCUSED=1      still allow wheel changes on a control you clicked first

#include <QAbstractScrollArea>
#include <QAbstractSlider>
#include <QAbstractSpinBox>
#include <QApplication>
#include <QComboBox>
#include <QEvent>
#include <QProxyStyle>
#include <QScrollBar>
#include <QStyleFactory>
#include <QStylePlugin>
#include <QWheelEvent>

namespace {

const char *kSavedPolicy = "_nowheel_saved_focus_policy";

QStyle *createBaseStyle()
{
    QString name = qEnvironmentVariable("NOWHEEL_BASE_STYLE", QStringLiteral("Breeze"));
    if (name.compare(QLatin1String("BreezeNoWheel"), Qt::CaseInsensitive) == 0)
        name = QStringLiteral("Breeze"); // avoid wrapping ourselves
    QStyle *base = QStyleFactory::create(name);
    if (!base)
        base = QStyleFactory::create(QStringLiteral("Fusion"));
    return base;
}

bool insideScrollArea(const QWidget *w)
{
    for (QWidget *p = w->parentWidget(); p; p = p->parentWidget()) {
        if (qobject_cast<QAbstractScrollArea *>(p))
            return true;
        if (p->isWindow())
            break;
    }
    return false;
}

bool isTarget(const QWidget *w)
{
    if (qobject_cast<const QComboBox *>(w) || qobject_cast<const QAbstractSpinBox *>(w))
        return true;
    // Sliders only matter inside scrolling pages; never touch real scrollbars.
    if (qobject_cast<const QAbstractSlider *>(w) && !qobject_cast<const QScrollBar *>(w))
        return true;
    return false;
}

} // namespace

class NoWheelStyle : public QProxyStyle
{
    Q_OBJECT
public:
    NoWheelStyle()
        : QProxyStyle(createBaseStyle())
        , m_allowFocused(qEnvironmentVariableIntValue("NOWHEEL_ALLOW_FOCUSED") != 0)
    {
    }

    void polish(QWidget *w) override
    {
        QProxyStyle::polish(w);
        if (!isTarget(w))
            return;
        // Don't let the wheel give these controls keyboard focus.
        if (w->focusPolicy() == Qt::WheelFocus) {
            w->setProperty(kSavedPolicy, int(Qt::WheelFocus));
            w->setFocusPolicy(Qt::StrongFocus);
        }
        w->removeEventFilter(this);
        w->installEventFilter(this);
    }

    void unpolish(QWidget *w) override
    {
        if (isTarget(w)) {
            w->removeEventFilter(this);
            const QVariant saved = w->property(kSavedPolicy);
            if (saved.isValid()) {
                w->setFocusPolicy(Qt::FocusPolicy(saved.toInt()));
                w->setProperty(kSavedPolicy, QVariant());
            }
        }
        QProxyStyle::unpolish(w);
    }

    // Keep the polish/unpolish overloads for QApplication/QPalette visible.
    using QProxyStyle::polish;
    using QProxyStyle::unpolish;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override
    {
        if (event->type() != QEvent::Wheel)
            return QProxyStyle::eventFilter(obj, event);

        auto *w = qobject_cast<QWidget *>(obj);
        if (!w || !isTarget(w))
            return false;

        // Standalone sliders (e.g. a volume slider in a toolbar) keep wheel control.
        if (qobject_cast<QAbstractSlider *>(w) && !insideScrollArea(w))
            return false;

        if (m_allowFocused && w->hasFocus())
            return false;

        forwardToScrollArea(w, static_cast<QWheelEvent *>(event));
        return true; // the control itself never sees the wheel
    }

private:
    // Hand the wheel event to the nearest scroll area that can actually scroll.
    // (Qt doesn't propagate synthesized wheel events to parents, so we walk up ourselves.)
    static void forwardToScrollArea(QWidget *from, QWheelEvent *e)
    {
        for (QWidget *p = from->parentWidget(); p; p = p->parentWidget()) {
            auto *area = qobject_cast<QAbstractScrollArea *>(p);
            if (area && area->viewport()) {
                QWidget *vp = area->viewport();
                QWheelEvent copy(vp->mapFromGlobal(e->globalPosition()), e->globalPosition(),
                                 e->pixelDelta(), e->angleDelta(), e->buttons(), e->modifiers(),
                                 e->phase(), e->inverted(), e->source(), e->pointingDevice());
                copy.ignore();
                QCoreApplication::sendEvent(vp, &copy);
                if (copy.isAccepted())
                    break;
            }
            if (p->isWindow())
                break;
        }
        e->accept();
    }

    const bool m_allowFocused;
};

class NoWheelStylePlugin : public QStylePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QStyleFactoryInterface_iid FILE "nowheelstyle.json")
public:
    QStyle *create(const QString &key) override
    {
        if (key.compare(QLatin1String("BreezeNoWheel"), Qt::CaseInsensitive) == 0)
            return new NoWheelStyle;
        return nullptr;
    }
};

#include "nowheelstyle.moc"
