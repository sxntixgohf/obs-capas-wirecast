#pragma once

#include <QString>
#include <QVector>

// Una fila del dock. Nada fijo: el usuario elige la fuente,
// la etiqueta que ve en el boton, y puede agregar/quitar filas.
// "sourceName" es el nombre exacto de la fuente en OBS.
// Si la fuente no existe, el dock la marca como no encontrada,
// no se rompe y no se borra sola.
struct LayerRow {
    QString label;      // texto del boton, editable por el usuario
    QString sourceName; // nombre de la fuente en OBS, elegido en el desplegable
    bool isCamera;      // true = Camaras (exclusiva), false = Overlays (independiente)
    bool operator==(const LayerRow &o) const
    {
        return label == o.label && sourceName == o.sourceName && isCamera == o.isCamera;
    }
};

struct LayersConfig {
    QString sceneName; // escena Principal elegida por el usuario
    bool autoLive = false;
    QVector<LayerRow> cameras;
    QVector<LayerRow> overlays;

    // Valores iniciales solo para que el dock no nazca vacio la
    // primera vez. El usuario los cambia todos desde el dock.
    // No son nombres obligatorios: son ejemplos editables.
    static LayersConfig defaults();
};

// Guarda/carga en JSON (capas.json) en la carpeta de config del modulo.
// Asi la configuracion sobrevive al cerrar OBS y no depende de
// ninguna escena ni coleccion en concreto.
class LayersConfigStore {
public:
    static QString configPath();
    static LayersConfig load();
    static void save(const LayersConfig &config);
};
