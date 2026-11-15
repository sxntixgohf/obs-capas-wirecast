#include <obs.h>
#include <obs-module.h>
#include <obs-frontend-api.h>

#include "layers-dock.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-capas-wirecast", "en-US")

static LayersDock *dockWidget = nullptr;

bool obs_module_load(void)
{
    QWidget *mainWindow = (QWidget *)obs_frontend_get_main_window();
    if (!mainWindow)
        return false;

    dockWidget = new LayersDock(mainWindow);

    // El dock aparece en OBS en: Docks -> Capas Wirecast
    // obs_frontend_add_dock_by_id() es la API oficial para docks.
    obs_frontend_add_dock_by_id("obs-capas-wirecast", "Capas Wirecast", dockWidget);

    blog(LOG_INFO, "[obs-capas-wirecast] Dock de capas cargado (Qt6, Frontend API)");
    return true;
}

void obs_module_unload(void)
{
    // OBS se encarga de destruir el dock con la ventana principal.
    dockWidget = nullptr;
}
