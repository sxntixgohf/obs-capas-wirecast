#include "layers-config.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <obs-module.h>

LayersConfig LayersConfig::defaults()
{
    LayersConfig c;
    c.sceneName = QString();
    c.autoLive = false;
    // Filas de ejemplo, todas editables y borrables.
    c.cameras = {
        {QStringLiteral("Camara 1"), QString(), true},
        {QStringLiteral("Camara 2"), QString(), true},
        {QStringLiteral("Camara 3"), QString(), true},
    };
    c.overlays = {
        {QStringLiteral("Lower third 1"), QString(), false},
        {QStringLiteral("Logo"), QString(), false},
    };
    return c;
}

QString LayersConfigStore::configPath()
{
    // obs_module_config_path() es la ruta oficial del plugin.
    char *path = obs_module_config_path("capas.json");
    QString result = QString::fromUtf8(path ? path : "");
    bfree(path);
    return result;
}

static QJsonObject rowToJson(const LayerRow &row)
{
    QJsonObject o;
    o["label"] = row.label;
    o["source"] = row.sourceName;
    o["isCamera"] = row.isCamera;
    return o;
}

static LayerRow rowFromJson(const QJsonObject &o, bool isCamera)
{
    LayerRow row;
    row.label = o["label"].toString();
    row.sourceName = o["source"].toString();
    row.isCamera = isCamera;
    return row;
}

LayersConfig LayersConfigStore::load()
{
    LayersConfig config = LayersConfig::defaults();

    QFile file(configPath());
    if (!file.open(QIODevice::ReadOnly))
        return config;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isObject())
        return config;

    QJsonObject root = doc.object();
    config.sceneName = root["scene"].toString();
    config.autoLive = root["autoLive"].toBool(false);

    if (root.contains("cameras") && root["cameras"].isArray()) {
        config.cameras.clear();
        for (const QJsonValue &v : root["cameras"].toArray())
            config.cameras.append(rowFromJson(v.toObject(), true));
    }
    if (root.contains("overlays") && root["overlays"].isArray()) {
        config.overlays.clear();
        for (const QJsonValue &v : root["overlays"].toArray())
            config.overlays.append(rowFromJson(v.toObject(), false));
    }
    return config;
}

void LayersConfigStore::save(const LayersConfig &config)
{
    QJsonObject root;
    root["scene"] = config.sceneName;
    root["autoLive"] = config.autoLive;

    QJsonArray cams;
    for (const LayerRow &row : config.cameras)
        cams.append(rowToJson(row));
    root["cameras"] = cams;

    QJsonArray overs;
    for (const LayerRow &row : config.overlays)
        overs.append(rowToJson(row));
    root["overlays"] = overs;

    QString path = configPath();
    QDir().mkpath(QFileInfo(path).absolutePath());

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
}
