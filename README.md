# 🔐 Sistema de Criptografía y Gestión de Archivos Criptográficos en C++

Un sistema interactivo desarrollado en **C++** para Windows que permite encriptar y desencriptar mensajes utilizando tres técnicas clásicas de la criptografía: **Cifrado César**, **Cifrado de Alberti** y el **Principio de Kerckhoffs (XOR con Base64)**. 

El programa incluye soporte completo para codificación UTF-8 (caracteres acentuados, `ñ`, `Ñ`), integración bidireccional con el **portapapeles de Windows** y un sistema de **persistencia en disco** con extensiones de archivo personalizadas.

---

## 🚀 Características Principales

* **3 Algoritmos de Cifrado Implementados:**
  * **Cifrado César:** Sustitución monoalfabética por desplazamiento circular dentro del rango de caracteres imprimibles ASCII (soporta enteros positivos y negativos).
  * **Cifrado de Alberti:** Sustitución polialfabética mediante clave variante (disco de cifrado).
  * **Principio de Kerckhoffs (XOR):** Cifrado simétrico de flujo a nivel de byte codificado en **Base64** para garantizar la portabilidad del texto.
* **Integración con el Portapapeles de Windows:**
  * Permite pegar texto directamente desde el portapapeles para encriptar o desencriptar.
  * Copia automáticamente cualquier resultado generado al portapapeles.
* **Persistencia y Manejo de Archivos:**
  * Generación y lectura de archivos con extensiones específicas según la técnica elegida: `.ces` (César), `.alb` (Alberti) y `.kck` (Kerckhoffs).
  * Encriptación directa de archivos de texto plano (`.txt`).
  * Desencriptación con opción de exportar el resultado restituido a un archivo `.txt`.
  * **Protecciones de Seguridad:** Validación de existencia de archivos, confirmación previa de sobrescritura, filtro contra archivos binarios y límite de tamaño configurado a **2 MB**.
  * **Navegación Fluida:** Submenús anidados con opción `0` para regresar al menú anterior en todo momento.
* **Soporte UTF-8 y Consola Acolorada:**
  * Configuración activa de páginas de código UTF-8 para evitar corrupción de tildes o caracteres especiales.
  * Interfaz de consola mejorada visualmente mediante secuencias de escape ANSI.

---

## 🧠 ¿Cómo funciona cada Cifrado?

### 1. Cifrado César (`.ces`)
Es una técnica de sustitución monoalfabética. Cada carácter del mensaje original se desplaza un número fijo de posiciones en el alfabeto/tabla.
* **Soporte de enteros negativos:** Si ingresás un valor negativo (ej: `-3`), el algoritmo calcula la posición equivalente hacia atrás en el rango de caracteres imprimibles (del 32 al 126), permitiendo cifrar en sentido inverso.

### 2. Cifrado de Alberti (`.alb`)
Inspirado en el disco de cifrado polialfabético ideado por Leon Battista Alberti. En lugar de usar un único desplazamiento fijo, utiliza una **palabra clave**. Cada letra de la clave determina el desplazamiento del carácter correspondiente en el mensaje (C_i = (T_i + K_i) mod Rango).

### 3. Principio de Kerckhoffs (`.kck`)
Basado en el axioma criptográfico de Auguste Kerckhoffs: *la seguridad de un criptosistema debe recaer exclusivamente en la clave y no en la privacidad del algoritmo*.
- **Mecanismo:** Realiza una operación **XOR (⊕)** bit a bit entre cada byte del texto y la clave secreta.
- **Codificación Base64:** Dado que la operación XOR a nivel de byte puede producir bytes nulos (`\0`) o no imprimibles, el resultado cifrado se convierte automáticamente a **Base64**. Esto permite copiar, guardar e intercambiar el texto cifrado sin pérdida de integridad.

---

## 📂 Formato de Archivos y Extensiones

| Técnica Criptográfica | Extensión de Salida Cifrada | Extensión de Salida Desencriptada |
| :--- | :---: | :---: |
| **Cifrado César** | `.ces` | `.txt` |
| **Cifrado de Alberti** | `.alb` | `.txt` |
| **Principio de Kerckhoffs** | `.kck` | `.txt` |

---

## 📋 Flujo de Uso del Programa

1. **Selección del Método:** Elegí la técnica criptográfica deseada (1 a 3).
2. **Selección de Acción:** Elegí entre **1. Encriptar**, **2. Desencriptar** o **0. Volver al menú anterior**.
3. **Origen de Datos:**
   * **Al Encriptar:** Podés escribir por teclado, pegar del portapapeles o cargar un archivo `.txt` (menor o igual a 2 MB).
   * **Al Desencriptar:** Podés pegar el texto cifrado del portapapeles o abrir un archivo con la extensión correspondiente (`.ces`, `.alb`, `.kck`).
4. **Clave / Parámetro:** Ingresá el desplazamiento numérico (positivo o negativo) o la clave según el algoritmo.
5. **Resultado:** El sistema mostrará la salida procesada en pantalla y la copiará automáticamente al portapapeles de Windows. Luego, te consultará si deseás guardar el resultado en un archivo (`.ces`/`.alb`/`.kck` si encriptaste, o `.txt` si desencriptaste).

---

## 👤 Autor

**J. Luis Rey**  
*Licenciatura en Ciberdefensa — FADENA (Facultad de Defensa Nacional)*

---

## 🛠️ Compilación (Windows)

Para compilar el código fuente con MinGW (`g++`):

```bash
g++ -O2 -std=c++11 criptografia.cpp -o CriptoTool.exe
