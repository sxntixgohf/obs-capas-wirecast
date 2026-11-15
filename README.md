# OBS Capas estilo Wirecast — Dock para OBS Studio

Plugin nativo en C++ / Qt6 para OBS Studio en Windows.
No reemplaza el Preview / Program de OBS en Modo Estudio — lo complementa
con un dock de **capas estilo Wirecast**.

## El problema que resuelve

Santiago usa 2 y a veces 3 capturadoras de vídeo, sin switchera.
Antes duplicaba toda la escena principal por cada capturadora y solo
cambiaba la captura. Cada copia tenía su propio "ojo" del lower third,
así que al cambiar de escena y regresar, un lower third que no había
apagado se volvía a mandar por accidente.

Con este plugin:

- **1 sola escena "Principal"** con todo adentro.
- **Capa Cámaras (exclusiva):** solo una capturadora visible a la vez.
  Tocas una cámara en Preview y con GO pasa a Program. Con AutoLive
  prendido, sale directo al aire.
- **Capa Overlays (independiente):** lower thirds, logos, textos.
  Cada uno tiene su propio botón ON/OFF con luz roja. Su estado se
  guarda y **no se reinicia cuando cambias de cámara**.

## Nada fijo: tú eliges las fuentes

- **No hay nombres fijos en el código.**
- En el dock, cada fila tiene un desplegable con **todas las fuentes
  de tu escena actual en OBS**. Eliges la que quieras para esa fila.
- Puedes **agregar, quitar, renombrar y reordenar** filas cuando
  quieras: `+ Agregar cámara`, `+ Agregar overlay`, `✕` en cada fila.
- La configuración se guarda en un archivo JSON del plugin
  (`capas.json`) y se carga sola al abrir OBS. Si una fuente ya no
  existe, su fila aparece como "fuente no encontrada" para que la
  reasignes o la borres, sin romper el resto.
- También eliges **qué escena es la Principal** en un desplegable
  arriba del dock.

## Compilación automática (recomendado)

No necesitas Visual Studio en tu PC. Lee **`GUIA_GITHUB.md`**:

1. Crea un repositorio en GitHub llamado `obs-capas-wirecast`.
2. Sube todo el contenido de este ZIP.
3. Ve a la pestaña **Actions** y espera a que **"Compilar para Windows"**
   se ponga en verde (la primera vez tarda 15-25 minutos).
4. Descarga el Artifact **`obs-capas-wirecast-windows-x64`**.
5. Instálalo en OBS como se explica en la guía.

Si creas un tag `v1.0.0`, GitHub crea además un Release con el ZIP.

## Importante: versión de OBS

Esta versión (v3) se compila con el SDK de **OBS 30.2.3**,
la versión que usa Santiago. **No hay que actualizar OBS.**

El `buildspec.json` está fijado a OBS 30.2.3, con obs-deps
y Qt6 versión 2024-05-08, que son las dependencias oficiales
de OBS 30.2.3. Los hashes SHA256 se verificaron contra el
repositorio oficial de OBS:
- Código de OBS 30.2.3 (.zip Windows):
  `a75de0a777013ae5edca526e21b584176bf21606d4d5db23ba4b5a87cfde12e3`
- obs-deps 2024-05-08 (Windows x64):
  `773c87a0d173125ef2768aaca6f6de64a7d6053213cc9f7d3970a301152042d8`
- Qt6 2024-05-08 (Windows x64):
  `8f459af5115ce081ae24b108712327e113893f250e14a902b1bd188b43873ed1`

Si en el futuro actualizas OBS a la 31 o más reciente, el DLL
de la 30.2.3 puede dejar de cargar. En ese caso habría que
hacer una v4 con el SDK nuevo.

## Instalar el DLL

1. Cierra OBS por completo.
2. Descomprime el ZIP descargado de GitHub Actions / Release.
3. Copia su contenido a tu instalación de OBS o a
   `%APPDATA%\obs-studio\plugins\obs-capas-wirecast\`,
   según la estructura que traiga el ZIP (ver `GUIA_GITHUB.md`).
4. Abre OBS → **Docks → Capas Wirecast**.

## Compilar en tu PC (opcional)

Si prefieres compilarlo tú en Windows:

- Windows 10/11 64-bit, Visual Studio 2022, CMake 3.28+

```bat
cmake --preset windows-x64
cmake --build --preset windows-x64
cmake --install build_x64 --config RelWithDebInfo
```

La plantilla descarga sola OBS, obs-deps y Qt6 en la primera
configuración.

## Archivos

- `.github/workflows/windows.yml` — compilación automática solo Windows.
- `.github/scripts/Build-Windows.ps1` y `Package-Windows.ps1` —
  scripts oficiales de la plantilla para compilar y empaquetar.
- `CMakeLists.txt`, `CMakePresets.json`, `buildspec.json`, `cmake/` —
  proyecto basado en la plantilla oficial `obs-plugintemplate`.
- `src/plugin-main.cpp` — punto de entrada, registra el dock.
- `src/layers-dock.h` / `src/layers-dock.cpp` — el dock y la lógica
  de cámaras/overlays.
- `src/layers-config.h` / `src/layers-config.cpp` — configuración
  en JSON, sin fuentes fijas.
- `GUIA_GITHUB.md` — guía paso a paso en español.

## Licencia

GPLv2 o posterior, como exige OBS al enlazar con libobs.
Basado en `obs-plugintemplate` de OBS Project.
