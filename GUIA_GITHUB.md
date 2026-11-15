# Guía para Santiago: compilar el plugin en GitHub, sin instalar nada

No necesitas Visual Studio ni CMake en tu PC. GitHub compila el plugin
por ti, gratis, y te deja descargar el ZIP ya listo para OBS.

Tiempo total la primera vez: unos 10 minutos para subir el código,
y 15 a 25 minutos esperando la primera compilación (descarga OBS, Qt
y las dependencias). Las siguientes veces es más rápido.

## Antes de empezar

1. **No hay que actualizar OBS.** Esta versión (v3) está hecha
   para tu OBS **30.2.3**, tal como lo tienes. Quédate en esa
   versión mientras uses este DLL.
2. Crea una cuenta gratis en https://github.com si no tienes una.

## Paso 1. Crea el repositorio

1. Entra a https://github.com/new
2. **Repository name:** `obs-capas-wirecast`
3. Déjalo en **Public** (así GitHub Actions es gratis y sin límites
   raros para este proyecto).
4. **NO** marques "Add a README file" ni "Add .gitignore".
   Lo subiremos todo desde el ZIP.
5. Pulsa **Create repository**.

## Paso 2. Sube el código

1. Descomprime el ZIP `obs-capas-wirecast-v3.zip` en tu PC.
   Te quedará una carpeta `obs-capas-wirecast-v3` con archivos
   y carpetas dentro, incluyendo una carpeta oculta `.github`.
   **Importante:** si al descomprimir no ves la carpeta `.github`,
   actívala en el Explorador de Windows: Ver → Mostrar → Elementos
   ocultos. Sin esa carpeta no hay compilación automática.
2. En la página de tu repositorio nuevo en GitHub, pulsa el enlace
   **"uploading an existing file"**.
3. Arrastra **todo el contenido** de la carpeta
   `obs-capas-wirecast-v3` (no la carpeta en sí, sino lo que hay
   dentro: `src`, `cmake`, `.github`, `CMakeLists.txt`,
   `CMakePresets.json`, `buildspec.json`, `README.md`,
   `GUIA_GITHUB.md`, `LICENSE`, etc.).
4. Abajo, en **Commit changes**, escribe `Primera versión`
   y pulsa **Commit changes**.

> Otra forma, si sabes usar Git en tu PC:
> `git init`, `git add .`, `git commit -m "Primera versión"`,
> `git branch -M main`,
> `git remote add origin https://github.com/TU_USUARIO/obs-capas-wirecast.git`,
> `git push -u origin main`

## Paso 3. Espera la compilación

1. En tu repositorio, pulsa la pestaña **Actions** arriba.
2. Verás un trabajo llamado **"Compilar para Windows"** en marcha,
   con un círculo amarillo.
3. Espera a que se ponga en verde ✓. La primera vez tarda
   de 15 a 25 minutos. No cierres nada, GitHub trabaja solo.
4. Si se pone en rojo ✕, pulsa el trabajo, luego el paso en rojo
   y lee las últimas líneas. Copia ese error y mándamelo, lo
   corregimos juntos. Lo más común es un error de código, no de
   tu parte.

## Paso 4. Descarga el ZIP

1. Cuando esté en verde, pulsa el trabajo **"Compilar para Windows"**.
2. Baja hasta **Artifacts** y descarga
   **`obs-capas-wirecast-windows-x64`**.
3. Descomprime ese ZIP. Dentro verás una estructura lista para OBS,
   con el archivo `.dll` dentro de `obs-plugins/64bit/` o
   `bin/64bit/`, según lo empaquete la plantilla.

## Paso 5. Instala en OBS

1. **Cierra OBS por completo** antes de instalar.
2. Copia el contenido del ZIP descargado a la carpeta de OBS.
   Hay dos formas, usa la que te dé el ZIP:
   - Si el ZIP trae carpetas `obs-plugins` y `data`:
     cópialas dentro de tu carpeta de instalación de OBS
     (normalmente `C:\Program Files\obs-studio\`).
   - Si el ZIP trae la estructura de `%APPDATA%`:
     cópialo en `%APPDATA%\obs-studio\plugins\obs-capas-wirecast\`
     (escribe esa ruta en la barra del Explorador).
3. Abre OBS.
4. Menú **Docks → Capas Wirecast**. Si no aparece, revisa en
   **Ayuda → Archivos de registro → Ver registro actual**
   que diga `obs-capas-wirecast` cargado.
5. Configura una vez el dock: elige tu escena Principal,
   en **Cámaras** elige tus capturadoras en los desplegables,
   en **Overlays** elige tus lower thirds y logos.
   Todo con `+ Agregar` y `✕` cuando quieras cambiarlo.

## Para sacar una versión con Release (opcional, recomendado)

Cuando el plugin ya te funcione y quieras guardarlo como versión
oficial `v1.0.0`:

1. En GitHub, ve a **Releases → Draft a new release**.
2. **Choose a tag:** escribe `v1.0.0` y pulsa **Create new tag**.
3. **Release title:** `v1.0.0 — Capas Wirecast`.
4. Pulsa **Publish release**.
5. GitHub compilará de nuevo y adjuntará el ZIP al Release.
   Desde ahí lo descargas siempre que quieras, sin buscar en Actions.

Cada vez que quieras cambiar algo del código, solo sube los archivos
cambiados a GitHub y se compila solo otra vez.

## Si algo falla

- **"No veo la carpeta .github al subir":** activa archivos ocultos
  en el Explorador de Windows. Sin ella no se compila.
- **El trabajo se pone en rojo:** no es tu PC, es el código o la
  plantilla. Mándame captura de las últimas líneas del paso en rojo.
- **El dock no aparece en OBS:** cierra OBS, revisa que el DLL esté
  en la carpeta correcta y que tu OBS siga en la **30.2.3**.
  Este DLL está hecho para esa versión; si actualizas OBS a la
  31, puede dejar de cargar y habría que hacer una v4.
- **Una fuente sale como "(no encontrada)":** es normal si la
  borraste o la renombraste en OBS. En el dock, vuelve a elegirla
  en el desplegable o borra esa fila con `✕`.
