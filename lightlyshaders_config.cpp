#include "lightlyshaders_config.h"
#include "ui_lightlyshaders_config.h"

#include <QDialog>
#include <QVBoxLayout>

#include <kwineffects.h>
#include <kwineffects_interface.h>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusArgument>

#include <KConfigGroup>
#include <KPluginFactory>
#include <KAboutData>

K_PLUGIN_FACTORY_WITH_JSON(LightlyShadersConfigFactory,
                           "lightlyshaders_config.json",
                           registerPlugin<LightlyShadersConfig>();)


class ConfigDialog : public QWidget , public Ui::Form
{
public:
    explicit ConfigDialog(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
}
};

class LightlyShadersConfig::Private
{
public:
    Private(LightlyShadersConfig *config)
        : q(config)
        , roundness("roundness")
        , outline("outline")
        , alpha("alpha")
        , darkBorder("dark_border")
        , inverseOutline("inverse_outline")
        , defaultRoundness(5)
        , defaultOutline(false)
        , defaultAlpha(15)
        , defaultDarkBorder(false)
        , defaultInverseOutline(false)
    {}
    LightlyShadersConfig *q;
    QString roundness, outline, alpha, darkBorder, inverseOutline;
    QVariant defaultRoundness, defaultOutline, defaultAlpha, defaultDarkBorder, defaultInverseOutline;
    ConfigDialog *ui;
};

LightlyShadersConfig::LightlyShadersConfig(QWidget* parent, const QVariantList& args)
    : KCModule(KAboutData::pluginData(QStringLiteral("kwin4_effect_lightlyshaders")), parent, args)
    , d(new Private(this))
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(d->ui = new ConfigDialog(this));
    setLayout(layout);
}

LightlyShadersConfig::~LightlyShadersConfig()
{
    delete d;
}

void
LightlyShadersConfig::load()
{
    KCModule::load();
    KConfigGroup conf = KSharedConfig::openConfig("lightlyshaders.conf")->group("General");
    d->ui->roundness->setValue(conf.readEntry(d->roundness, d->defaultRoundness).toInt());
    d->ui->outline->setChecked(conf.readEntry(d->outline, d->defaultOutline).toBool());
    d->ui->alpha->setValue(conf.readEntry(d->alpha, d->defaultAlpha).toInt());
    d->ui->darkBorder->setChecked(conf.readEntry(d->darkBorder, d->defaultDarkBorder).toBool());
    d->ui->inverseOutline->setChecked(conf.readEntry(d->inverseOutline, d->defaultInverseOutline).toBool());
    emit changed(false);
}

void
LightlyShadersConfig::save()
{
    KCModule::save();
    KConfigGroup conf = KSharedConfig::openConfig("lightlyshaders.conf")->group("General");
    conf.writeEntry(d->roundness, d->ui->roundness->value());
    conf.writeEntry(d->outline, d->ui->outline->isChecked());
    conf.writeEntry(d->alpha, d->ui->alpha->value());
    conf.writeEntry(d->darkBorder, d->ui->darkBorder->isChecked());
    conf.writeEntry(d->inverseOutline, d->ui->inverseOutline->isChecked());
    conf.sync();
    emit changed(false);
    OrgKdeKwinEffectsInterface interface(QStringLiteral("org.kde.KWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.reconfigureEffect(QStringLiteral("kwin4_effect_lightlyshaders"));
}

void
LightlyShadersConfig::defaults()
{
    KCModule::defaults();
    d->ui->roundness->setValue(d->defaultRoundness.toInt());
    d->ui->outline->setChecked(d->defaultOutline.toBool());
    d->ui->alpha->setValue(d->defaultAlpha.toInt());
    d->ui->darkBorder->setChecked(d->defaultDarkBorder.toBool());
    d->ui->inverseOutline->setChecked(d->defaultInverseOutline.toBool());
    emit changed(true);
}

#include "lightlyshaders_config.moc"
