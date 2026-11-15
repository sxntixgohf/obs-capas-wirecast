#pragma once

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "layers-config.h"

class QLabel;
class QCheckBox;

// Dock "Capas Wirecast" para OBS Studio (probado contra SDK 31.1.1,
// usa solo APIs estables que ya existian en OBS 30.2.3).
//
// Idea central, pedida por Santiago:
// - 1 sola escena Principal. No se duplica la escena por camara.
// - Capa Camaras: exclusiva. Solo una capturadora visible a la vez.
//   Tocar una camara la prepara en Preview; GO la manda a Program.
//   Con AutoLive, sale directo.
// - Capa Overlays: independiente. Cada lower third / logo tiene su
//   propio ON/OFF y su estado persiste al cambiar de camara.
//   Eso evita el reenvio accidental al volver a una escena.
//
// Todo configurable: cada fila es un desplegable con las fuentes
// reales de OBS. El usuario agrega, quita y renombra filas cuando
// quiera. Nada esta fijo en el codigo.
class LayersDock : public QWidget {
    Q_OBJECT

public:
    explicit LayersDock(QWidget *parent = nullptr);
    ~LayersDock() override = default;

private slots:
    void onSceneChanged(int index);
    void onAddCamera();
    void onAddOverlay();
    void onGoClicked();
    void onAutoLiveToggled(bool checked);
    void onRefreshClicked();

private:
    void rebuildRows();
    void saveAndRefresh();
    void updateLiveIndicators();

    // --- Logica OBS (aislada para facilitar ajustes de API) ---
    // Enciende/apaga una fuente dentro de una escena dada.
    // Devuelve false si la fuente no esta en esa escena.
    bool setSceneItemVisible(void *sceneSource, const QString &sourceName, bool visible);
    // Aplica la camara elegida: en Preview si AutoLive esta OFF,
    // directo en Program si esta ON. Apaga las demas camaras.
    void applyCameraToScene(const QString &sourceName);
    // En Modo Estudio, manda lo preparado en Preview a Program
    // con la transicion actual de OBS.
    void doTransition();

    LayersConfig config;

    QComboBox *sceneCombo = nullptr;
    QVBoxLayout *camerasLayout = nullptr;
    QVBoxLayout *overlaysLayout = nullptr;
    QLabel *statusLabel = nullptr;
    QCheckBox *autoLiveCheck = nullptr;
    QPushButton *goButton = nullptr;

    // Camara actualmente en Program y en Preview, segun el dock.
    QString programCamera;
    QString previewCamera;
};
