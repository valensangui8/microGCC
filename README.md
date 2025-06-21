[![✗](https://img.shields.io/badge/Release-v1.1.0-ffb600.svg?style=for-the-badge)](https://github.com/agustin-golmar/Flex-Bison-Compiler/releases)

[![✗](https://github.com/agustin-golmar/Flex-Bison-Compiler/actions/workflows/pipeline.yaml/badge.svg?branch=production)](https://github.com/agustin-golmar/Flex-Bison-Compiler/actions/workflows/pipeline.yaml)

# Flex-Bison-Compiler

A base compiler example, developed with Flex and Bison.

* [Environment](#environment)
* [CI/CD](#cicd)
* [Recommended Extensions](#recommended-extensions)
* Installation
  * [Docker](doc/readme/Docker.md)
  * [Microsoft Windows](doc/readme/Windows.md)
  * [Ubuntu](doc/readme/Ubuntu.md)

## Environment

Set the following environment variables to control and configure the behaviour of the application:

|Name|Default|Description|
|-|:-:|-|
|`LOG_IGNORED_LEXEMES`|`true`|When `true`, logs all of the ignored lexemes found with Flex at DEBUGGING level. To remove those logs from the console output set it to `false`.|
|`LOGGING_LEVEL`|`INFORMATION`|The minimum level to log in the console output. From lower to higher, the available levels are: `ALL`, `DEBUGGING`, `INFORMATION`, `WARNING`, `ERROR` and `CRITICAL`.|

## CI/CD

To trigger an automatic integration on every push or PR (_Pull Request_), you must activate _GitHub Actions_ in the _Settings_ tab. Use the following configuration:

|Key|Value|
|-|-|
|Actions permissions|Allow all actions and reusable workflows|
|Artifact and log retention|`30 days`|
|Fork pull request workflows from outside collaborators|Require approval for all outside collaborators|
|Workflow permissions|Read repository contents and packages permissions|
|Allow GitHub Actions to create and approve pull requests|`false`|

After integration is done, change the badges of this `README.md` to point to the new repository.

## Recommended Extensions

* [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
* [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
* [Yash](https://marketplace.visualstudio.com/items?itemName=daohong-emilio.yash)

## Configurar el nombre del archivo ASM

El generador de código escribe el ensamblador en `src/test/asm/output`.
Por defecto el fichero se llama **`OUTPUT.asm`**, pero puedes definir un nombre personalizado mediante la variable de entorno:

```bash
export OUTPUT_FILE_NAME="mi_test.asm"
```

Cualquier valor asignado a `OUTPUT_FILE_NAME` será usado por el generador al crear el archivo.

---

## Ejecución de tests con los scripts de *asm*

En `script/ubuntu/asm/` encontrarás dos lanzadores:

| Script               | Propósito                                                                                    |
| -------------------- | -------------------------------------------------------------------------------------------- |
| `microGCC.sh`        | Compila un test C, genera ASM, ensambla y linkea usando la versión principal del compilador. |
| `microGCC_libasm.sh` | Igual que el anterior, pero enlaza además contra la librería `libasm.asm`.                   |

### Uso

```bash
./script/ubuntu/asm/microGCC.sh <nombre-test>
```

* **`<nombre-test>`** es el nombre del archivo de prueba ubicado en `src/test/c/accept/` **sin extensión**.
  Ejemplo: para `src/test/c/accept/02-extern-declaration` se invoca:

```bash
./script/ubuntu/asm/microGCC.sh 02-extern-declaration
```


Luego se puede correr el archivo ejecutable y ver lo que retorna con:
```bash
./src/test/asm/output/output ; echo $?
```
Aclaracion: estos dos comandos no hay que correrlos dentro del contenedor, pero si tiene que estar creado.


***Nota: si el contenedor de  docker no cuenta con permisos de escritura suficientes para crear el archivo .asm, el generador igualmente mostrará el código ensamblador por la salida estándar, junto con la tabla de símbolos formateada, de modo que nunca pierdas la información generada.***