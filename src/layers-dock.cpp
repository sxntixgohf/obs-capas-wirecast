#include "layers-dock.h"

#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QStringList>

#include <obs.h>
#include <obs-frontend-api.h>

// ---------------------------------------------------------------------------
// Utilidades OBS
// ---------------------------------------------------------------------------

static bool enumSourcesCb(void *param, obs_source_t *source)
{
    auto *names = static_cast<QStringList *>(param);
    names->append(QString::fromUtf8(obs_source_get_name(source)));
    return true;
}

static QStringList allSourceNames()
{
    QStringList names;
    obs_enum_sources(enumSourcesCb, &names);
    names.sort(Qt::CaseInsensitive);
    return names;
}

static QStringList allSceneNames()
{
    QStringList names;
    auto cb = [](void *param, obs_source_t *source) {
        if (obs_source_get_type(source) == OBS_SOURCE_TYPE_SCENE) {
            auto *list = static_cast<QStringList *>(param);
            list->append(QString::fromUtf8(obs_source_get_name(source)));
        }
        return true;
    };
    obs_enum_sources(cb, &names);
    names.sort(Qt::CaseInsensitive);
    return names;
}

// Devuelve la fila por indice, sin guardar referencias que se puedan
// invalidar cuando el usuario agrega otra fila (QVector::append puede
// reubicar el vector). Por eso todos los botones capturan el indice,
// no la referencia a la fila.
static LayerRow *rowAt(LayersConfig &config, int index, bool isCamera)
{
    QVector<LayerRow> &list = isCamera ? config.cameras : config.overlays;
    if (index < 0 || index >= list.size())
        return nullptr;
    return &list[index];
}

// ---------------------------------------------------------------------------
// Dock
// ---------------------------------------------------------------------------

LayersDock::LayersDock(QWidget *parent) : QWidget(parent)
{
    config = LayersConfigStore::load();

    auto *main = new QVBoxLayout(this);
    main->setContentsMargins(8, 8, 8, 8);

    auto *title = new QLabel(QStringLiteral("Capas — estilo Wirecast"), this);
    title->setStyleSheet(QStringLiteral("font-weight: bold; font-size: 14px;"));
    main->addWidget(title);

    auto *hint = new QLabel(
        QStringLiteral("1 escena Principal. Cambia solo la camara, "
                       "los overlays se quedan como estaban."),
        this);
    hint->setWordWrap(true);
    main->addWidget(hint);

    auto *sceneRow = new QHBoxLayout();
    sceneRow->addWidget(new QLabel(QStringLiteral("Escena:"), this));
    sceneCombo = new QComboBox(this);
    sceneCombo->addItems(allSceneNames());
    if (!config.sceneName.isEmpty()) {
        int idx = sceneCombo->findText(config.sceneName);
        if (idx >= 0)
            sceneCombo->setCurrentIndex(idx);
    }
    connect(sceneCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LayersDock::onSceneChanged);
    sceneRow->addWidget(sceneCombo, 1);
    auto *refreshBtn = new QPushButton(QStringLiteral("Actualizar"), this);
    connect(refreshBtn, &QPushButton::clicked, this, &LayersDock::onRefreshClicked);
    sceneRow->addWidget(refreshBtn);
    main->addLayout(sceneRow);

    auto *goRow = new QHBoxLayout();
    goButton = new QPushButton(QStringLiteral("GO — Mandar al aire"), this);
    goButton->setMinimumHeight(36);
    connect(goButton, &QPushButton::clicked, this, &LayersDock::onGoClicked);
    goRow->addWidget(goButton, 2);

    autoLiveCheck = new QCheckBox(QStringLiteral("AutoLive"), this);
    autoLiveCheck->setChecked(config.autoLive);
    connect(autoLiveCheck, &QCheckBox::toggled, this, &LayersDock::onAutoLiveToggled);
    goRow->addWidget(autoLiveCheck, 1);
    main->addLayout(goRow);

    statusLabel = new QLabel(this);
    statusLabel->setWordWrap(true);
    main->addWidget(statusLabel);

    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    auto *scrollContent = new QWidget(scroll);
    auto *scrollLayout = new QVBoxLayout(scrollContent);

    scrollLayout->addWidget(
        new QLabel(QStringLiteral("CAMARAS — solo una al aire"), scrollContent));
    camerasLayout = new QVBoxLayout();
    scrollLayout->addLayout(camerasLayout);
    auto *addCam = new QPushButton(QStringLiteral("+ Agregar camara"), scrollContent);
    connect(addCam, &QPushButton::clicked, this, &LayersDock::onAddCamera);
    scrollLayout->addWidget(addCam);

    scrollLayout->addWidget(
        new QLabel(QStringLiteral("OVERLAYS — lower thirds, logos, textos"), scrollContent));
    overlaysLayout = new QVBoxLayout();
    scrollLayout->addLayout(overlaysLayout);
    auto *addOver = new QPushButton(QStringLiteral("+ Agregar overlay"), scrollContent);
    connect(addOver, &QPushButton::clicked, this, &LayersDock::onAddOverlay);
    scrollLayout->addWidget(addOver);

    scrollLayout->addStretch(1);
    scroll->setWidget(scrollContent);
    main->addWidget(scroll, 1);

    rebuildRows();
    updateLiveIndicators();
}

void LayersDock::rebuildRows()
{
    auto clearLayout = [](QLayout *layout) {
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != nullptr) {
            if (item->widget())
                item->widget()->deleteLater();
            delete item;
        }
    };
    clearLayout(camerasLayout);
    clearLayout(overlaysLayout);

    const QStringList sources = allSourceNames();

    auto makeRow = [&](int index, bool isCamera, QVBoxLayout *layout) {
        LayerRow *rowPtr = rowAt(config, index, isCamera);
        if (!rowPtr)
            return;
        const QString rowLabel = rowPtr->label;
        const QString rowSource = rowPtr->sourceName;

        auto *box = new QWidget(this);
        auto *h = new QHBoxLayout(box);
        h->setContentsMargins(0, 0, 0, 0);

        auto *labelEdit = new QLineEdit(rowLabel, box);
        labelEdit->setPlaceholderText(QStringLiteral("Nombre en el dock"));
        connect(labelEdit, &QLineEdit::editingFinished, this, [this, labelEdit, index, isCamera]() {
            LayerRow *r = rowAt(config, index, isCamera);
            if (!r)
                return;
            r->label = labelEdit->text();
            LayersConfigStore::save(config);
        });
        h->addWidget(labelEdit, 1);

        auto *combo = new QComboBox(box);
        combo->addItem(QStringLiteral("— elegir fuente —"));
        combo->addItems(sources);
        if (!rowSource.isEmpty()) {
            int idx = combo->findText(rowSource);
            if (idx >= 0) {
                combo->setCurrentIndex(idx);
            } else {
                combo->addItem(rowSource + QStringLiteral("  (no encontrada)"));
                combo->setCurrentIndex(combo->count() - 1);
            }
        }
        connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                [this, combo, index, isCamera]() {
                    LayerRow *r = rowAt(config, index, isCamera);
                    if (!r)
                        return;
                    QString v = combo->currentText();
                    if (v.endsWith(QStringLiteral("(no encontrada)")))
                        return;
                    if (v == QStringLiteral("— elegir fuente —"))
                        r->sourceName.clear();
                    else
                        r->sourceName = v;
                    LayersConfigStore::save(config);
                });
        h->addWidget(combo, 2);

        if (isCamera) {
            auto *btn = new QPushButton(rowLabel.isEmpty() ? QStringLiteral("Camara") : rowLabel, box);
            btn->setCheckable(true);
            btn->setChecked(previewCamera == rowSource && !rowSource.isEmpty());
            connect(btn, &QPushButton::clicked, this, [this, index]() {
                LayerRow *r = rowAt(config, index, true);
                if (!r || r->sourceName.isEmpty())
                    return;
                previewCamera = r->sourceName;
                applyCameraToScene(r->sourceName);
                updateLiveIndicators();
            });
            h->addWidget(btn);
        } else {
            auto *btn = new QPushButton(QStringLiteral("ON / OFF"), box);
            btn->setCheckable(true);
            connect(btn, &QPushButton::toggled, this, [this, btn, index](bool on) {
                LayerRow *r = rowAt(config, index, false);
                if (!r || r->sourceName.isEmpty())
                    return;
                const QString sourceName = r->sourceName;
                const QString label = r->label;
                void *program = obs_frontend_get_current_scene();
                if (program) {
                    setSceneItemVisible(program, sourceName, on);
                    obs_source_release((obs_source_t *)program);
                }
                if (obs_frontend_preview_program_mode_active()) {
                    void *preview = obs_frontend_get_current_preview_scene();
                    if (preview && preview != program) {
                        setSceneItemVisible(preview, sourceName, on);
                        obs_source_release((obs_source_t *)preview);
                    }
                }
                btn->setText(on ? QStringLiteral("AL AIRE") : QStringLiteral("OFF"));
                statusLabel->setText(on ? QStringLiteral("Overlay al aire: ") + label
                                        : QStringLiteral("Overlay apagado: ") + label);
            });
            h->addWidget(btn);
        }

        auto *del = new QPushButton(QStringLiteral("✕"), box);
        connect(del, &QPushButton::clicked, this, [this, index, isCamera]() {
            QVector<LayerRow> &list = isCamera ? config.cameras : config.overlays;
            if (index < 0 || index >= list.size())
                return;
            list.removeAt(index);
            saveAndRefresh();
        });
        h->addWidget(del);

        layout->addWidget(box);
    };

    for (int i = 0; i < config.cameras.size(); ++i)
        makeRow(i, true, camerasLayout);
    for (int i = 0; i < config.overlays.size(); ++i)
        makeRow(i, false, overlaysLayout);
}

void LayersDock::saveAndRefresh()
{
    LayersConfigStore::save(config);
    rebuildRows();
    updateLiveIndicators();
}

void LayersDock::updateLiveIndicators()
{
    QString prog = programCamera.isEmpty() ? QStringLiteral("—") : programCamera;
    QString prev = previewCamera.isEmpty() ? QStringLiteral("—") : previewCamera;
    statusLabel->setText(QStringLiteral("Program: %1  •  Preview: %2").arg(prog, prev));
}

void LayersDock::onSceneChanged(int)
{
    config.sceneName = sceneCombo->currentText();
    LayersConfigStore::save(config);
}

void LayersDock::onAddCamera()
{
    config.cameras.append({QStringLiteral("Camara nueva"), QString(), true});
    saveAndRefresh();
}

void LayersDock::onAddOverlay()
{
    config.overlays.append({QStringLiteral("Overlay nuevo"), QString(), false});
    saveAndRefresh();
}

void LayersDock::onAutoLiveToggled(bool checked)
{
    config.autoLive = checked;
    LayersConfigStore::save(config);
    goButton->setEnabled(!checked);
}

void LayersDock::onRefreshClicked()
{
    sceneCombo->clear();
    sceneCombo->addItems(allSceneNames());
    if (!config.sceneName.isEmpty()) {
        int idx = sceneCombo->findText(config.sceneName);
        if (idx >= 0)
            sceneCombo->setCurrentIndex(idx);
    }
    rebuildRows();
}

void LayersDock::onGoClicked()
{
    if (previewCamera.isEmpty() || previewCamera == programCamera)
        return;
    doTransition();
    programCamera = previewCamera;
    updateLiveIndicators();
}

// --- Logica OBS ---

bool LayersDock::setSceneItemVisible(void *sceneSource, const QString &sourceName, bool visible)
{
    if (!sceneSource || sourceName.isEmpty())
        return false;
    obs_scene_t *scene = obs_scene_from_source((obs_source_t *)sceneSource);
    if (!scene)
        return false;
    obs_sceneitem_t *item = obs_scene_find_source(scene, sourceName.toUtf8().constData());
    if (!item)
        return false;
    obs_sceneitem_set_visible(item, visible);
    return true;
}

void LayersDock::applyCameraToScene(const QString &sourceName)
{
    void *target = nullptr;
    if (config.autoLive || !obs_frontend_preview_program_mode_active()) {
        target = obs_frontend_get_current_scene();
    } else {
        target = obs_frontend_get_current_preview_scene();
    }
    if (!target)
        return;

    for (const LayerRow &row : config.cameras) {
        if (row.sourceName.isEmpty())
            continue;
        setSceneItemVisible(target, row.sourceName, row.sourceName == sourceName);
    }
    if (config.autoLive)
        programCamera = sourceName;

    obs_source_release((obs_source_t *)target);
    statusLabel->setText(config.autoLive
        ? QStringLiteral("AutoLive: al aire ") + sourceName
        : QStringLiteral("En Preview: ") + sourceName + QStringLiteral(" — pulsa GO"));
}

void LayersDock::doTransition()
{
    // Modo Estudio: usa la API oficial para disparar la transicion
    // de Preview a Program con la transicion actual de OBS.
    // obs_frontend_preview_program_trigger_transition() existe
    // desde OBS 0.16 y sigue vigente en la documentacion oficial.
    if (!obs_frontend_preview_program_mode_active())
        return;
    obs_frontend_preview_program_trigger_transition();
}
