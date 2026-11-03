// FADENA - Facultad de Defensa Nacional
// Licenciatura en Ciberdefensa
// Autor: J. Luis Rey
// Descripción: Sistema de Criptografía y archivos Criptográficos con Cifrado César, Alberti y Principio de Kerckhoffs.

// Fecha: 2026-11-02
// Version 1.31

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>
#include <windows.h>

using namespace std;

// Tamaño máximo permitido: 2 MB (2 * 1024 * 1024 bytes)
const streampos MAX_TAMANO_BYTES = 2 * 1024 * 1024;

// ============================================================================
// CÓDIGOS ANSI PARA COLORES EN CONSOLA
// ============================================================================
const string RESET    = "\033[0m";
const string VERDE    = "\033[1;32m";
const string ROJO     = "\033[1;31m";
const string AMARILLO = "\033[1;33m";
const string CYAN     = "\033[1;36m";
const string MAGENTA  = "\033[1;35m";

// ============================================================================
// FUNCIONES DE PORTAPAPELES DE WINDOWS
// ============================================================================

bool copiarTextoAPortapapeles(const string& texto) {
    if (!OpenClipboard(NULL)) return false;
    EmptyClipboard();

    HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, texto.size() + 1);
    if (!hGlob) {
        CloseClipboard();
        return false;
    }

    memcpy(GlobalLock(hGlob), texto.c_str(), texto.size() + 1);
    GlobalUnlock(hGlob);

    SetClipboardData(CF_TEXT, hGlob);
    CloseClipboard();
    return true;
}

string obtenerTextoPortapapeles() {
    if (!OpenClipboard(NULL)) {
        cout << ROJO << "[!] No se pudo abrir el portapapeles." << RESET << "\n";
        return "";
    }

    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == NULL) {
        cout << ROJO << "[!] El portapapeles no contiene texto valido." << RESET << "\n";
        CloseClipboard();
        return "";
    }

    char* pszText = static_cast<char*>(GlobalLock(hData));
    if (pszText == NULL) {
        CloseClipboard();
        return "";
    }

    string texto(pszText);
    GlobalUnlock(hData);
    CloseClipboard();

    return texto;
}

// ============================================================================
// GESTIÓN Y VALIDACIÓN DE ARCHIVOS
// ============================================================================

string asegurarExtension(const string& nombreArchivo, const string& extension) {
    if (nombreArchivo.length() >= extension.length()) {
        string sub = nombreArchivo.substr(nombreArchivo.length() - extension.length());
        if (sub == extension) return nombreArchivo;
    }
    return nombreArchivo + extension;
}

bool archivoExiste(const string& ruta) {
    ifstream f(ruta.c_str());
    return f.good();
}

bool esTextoPlano(const string& contenido) {
    for (char c : contenido) {
        if (c == '\0') return false;
    }
    return true;
}

string cargarArchivoTextoPlano() {
    while (true) {
        cout << CYAN << "Ingrese el nombre/ruta del archivo .txt a encriptar (o '0' / 'CANCELAR' para volver): " << RESET;
        string nombre;
        cin >> nombre;
        cin.ignore((numeric_limits<streamsize>::max)(), '\n');

        if (nombre == "CANCELAR" || nombre == "cancelar" || nombre == "0") {
            cout << AMARILLO << "[!] Operacion cancelada." << RESET << "\n";
            return "";
        }

        string rutaCompleta = asegurarExtension(nombre, ".txt");

        if (!archivoExiste(rutaCompleta)) {
            cout << ROJO << "[!] ERROR: El archivo '" << rutaCompleta << "' NO existe en la carpeta." << RESET << "\n";
            continue;
        }

        ifstream archivo(rutaCompleta.c_str(), ios::in | ios::binary | ios::ate);
        if (!archivo.is_open()) {
            cout << ROJO << "[!] No se pudo abrir el archivo especificado." << RESET << "\n";
            continue;
        }

        streampos tamano = archivo.tellg();
        if (tamano > MAX_TAMANO_BYTES) {
            cout << ROJO << "[!] ERROR: El archivo supera el limite maximo de 2 MB (" 
                 << (tamano / (1024 * 1024.0)) << " MB)." << RESET << "\n";
            archivo.close();
            continue;
        }

        archivo.seekg(0, ios::beg);
        string contenido((istreambuf_iterator<char>(archivo)), istreambuf_iterator<char>());
        archivo.close();

        if (!esTextoPlano(contenido)) {
            cout << ROJO << "[!] ERROR: El archivo no parece ser texto plano valido (contiene bytes binarios)." << RESET << "\n";
            continue;
        }

        cout << VERDE << "[+] Archivo '" << rutaCompleta << "' (" << tamano << " bytes) leido correctamente." << RESET << "\n";
        return contenido;
    }
}

bool guardarEnArchivo(const string& contenido, const string& extRequerida) {
    while (true) {
        cout << CYAN << "Ingrese el nombre del archivo para guardar (o '0' / 'CANCELAR' para salir): " << RESET;
        string nombre;
        cin >> nombre;
        cin.ignore((numeric_limits<streamsize>::max)(), '\n');

        if (nombre == "CANCELAR" || nombre == "cancelar" || nombre == "0") {
            cout << AMARILLO << "[!] Operacion de guardado cancelada." << RESET << "\n";
            return false;
        }

        string rutaCompleta = asegurarExtension(nombre, extRequerida);

        if (archivoExiste(rutaCompleta)) {
            cout << ROJO << "[!] ADVERTENCIA: El archivo '" << rutaCompleta << "' ya existe." << RESET << "\n";
            cout << AMARILLO << "¿Desea sobrescribirlo? (S/N): " << RESET;
            char opc;
            cin >> opc;
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');

            if (opc != 'S' && opc != 's') {
                cout << AMARILLO << "[i] Ingrese un nombre diferente..." << RESET << "\n";
                continue;
            }
        }

        ofstream archivo(rutaCompleta.c_str(), ios::out | ios::binary);
        if (!archivo.is_open()) {
            cout << ROJO << "[!] Error al crear o abrir el archivo. Reintente." << RESET << "\n";
            continue;
        }

        archivo << contenido;
        archivo.close();

        cout << VERDE << "[+] Archivo guardado exitosamente como: " << MAGENTA << rutaCompleta << RESET << "\n";
        return true;
    }
}

string cargarDesdeArchivoCifrado(const string& extRequerida) {
    while (true) {
        cout << CYAN << "Ingrese el nombre del archivo (" << extRequerida << ") a abrir (o '0' / 'CANCELAR' para volver): " << RESET;
        string nombre;
        cin >> nombre;
        cin.ignore((numeric_limits<streamsize>::max)(), '\n');

        if (nombre == "CANCELAR" || nombre == "cancelar" || nombre == "0") {
            cout << AMARILLO << "[!] Carga cancelada. Volviendo al menu..." << RESET << "\n";
            return "";
        }

        string rutaCompleta = asegurarExtension(nombre, extRequerida);

        if (!archivoExiste(rutaCompleta)) {
            cout << ROJO << "[!] ERROR: El archivo '" << rutaCompleta << "' NO existe." << RESET << "\n";
            continue;
        }

        ifstream archivo(rutaCompleta.c_str(), ios::in | ios::binary | ios::ate);
        if (!archivo.is_open()) {
            cout << ROJO << "[!] No se pudo abrir el archivo especificado." << RESET << "\n";
            continue;
        }

        streampos tamano = archivo.tellg();
        if (tamano > MAX_TAMANO_BYTES) {
            cout << ROJO << "[!] ERROR: El archivo supera el limite maximo de 2 MB." << RESET << "\n";
            archivo.close();
            continue;
        }

        archivo.seekg(0, ios::beg);
        string contenido((istreambuf_iterator<char>(archivo)), istreambuf_iterator<char>());
        archivo.close();

        cout << VERDE << "[+] Archivo '" << rutaCompleta << "' leido con exito." << RESET << "\n";
        return contenido;
    }
}

// ============================================================================
// FUNCIONES BASE64 PARA KERCKHOFFS
// ============================================================================
static const string BASE64_CHARS = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

string base64_encode(const string& in) {
    string out;
    int val = 0, valb = -6;
    for (unsigned char c : in) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back(BASE64_CHARS[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) out.push_back(BASE64_CHARS[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out.size() % 4) out.push_back('=');
    return out;
}

string base64_decode(const string& in) {
    string out;
    vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) T[BASE64_CHARS[i]] = i;

    int val = 0, valb = -8;
    for (unsigned char c : in) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

// ============================================================================
// ALGORITMOS DE CIFRADO
// ============================================================================

string cifrarCesar(const string& texto, int desplazamiento) {
    string resultado = texto;
    for (size_t i = 0; i < texto.length(); ++i) {
        unsigned char c = static_cast<unsigned char>(texto[i]);
        if (c >= 32 && c <= 126) {
            resultado[i] = static_cast<char>(32 + (c - 32 + desplazamiento % 95 + 95) % 95);
        }
    }
    return resultado;
}

string descifrarCesar(const string& texto, int desplazamiento) {
    return cifrarCesar(texto, -desplazamiento);
}

string cifrarAlberti(const string& texto, const string& clave) {
    if (clave.empty()) return texto;
    string resultado = texto;
    size_t lenClave = clave.length();

    for (size_t i = 0; i < texto.length(); ++i) {
        unsigned char c = static_cast<unsigned char>(texto[i]);
        unsigned char k = static_cast<unsigned char>(clave[i % lenClave]);

        if (c >= 32 && c <= 126) {
            int shift = (k % 95);
            resultado[i] = static_cast<char>(32 + (c - 32 + shift) % 95);
        }
    }
    return resultado;
}

string descifrarAlberti(const string& texto, const string& clave) {
    if (clave.empty()) return texto;
    string resultado = texto;
    size_t lenClave = clave.length();

    for (size_t i = 0; i < texto.length(); ++i) {
        unsigned char c = static_cast<unsigned char>(texto[i]);
        unsigned char k = static_cast<unsigned char>(clave[i % lenClave]);

        if (c >= 32 && c <= 126) {
            int shift = (k % 95);
            resultado[i] = static_cast<char>(32 + (c - 32 - shift + 95) % 95);
        }
    }
    return resultado;
}

string encriptarKerckhoffs(const string& texto, const string& claveSecreta) {
    if (claveSecreta.empty()) return texto;
    string cifradoRaw = texto;
    size_t lenClave = claveSecreta.length();

    for (size_t i = 0; i < texto.length(); ++i) {
        cifradoRaw[i] = texto[i] ^ claveSecreta[i % lenClave];
    }
    return base64_encode(cifradoRaw);
}

string desencriptarKerckhoffs(const string& textoBase64, const string& claveSecreta) {
    if (claveSecreta.empty()) return textoBase64;
    string cifradoRaw = base64_decode(textoBase64);
    string resultado = cifradoRaw;
    size_t lenClave = claveSecreta.length();

    for (size_t i = 0; i < cifradoRaw.length(); ++i) {
        resultado[i] = cifradoRaw[i] ^ claveSecreta[i % lenClave];
    }
    return resultado;
}

// ============================================================================
// FLUJO PRINCIPAL CON NAVEGACIÓN ANIDADA (VOLVER AL MENÚ ANTERIOR)
// ============================================================================
int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    while (true) { // MENÚ PRINCIPAL: Selección de Técnica
        cout << "\n" << CYAN << "==================================================" << RESET << "\n";
        cout << "   SISTEMA DE CRIPTOGRAFIA Y ARCHIVOS CRIPTOGRAFICOS   \n";
        cout << CYAN << "==================================================" << RESET << "\n";
        cout << "1. Cifrado Cesar (Extension: .ces)\n";
        cout << "2. Cifrado de Alberti (Extension: .alb)\n";
        cout << "3. Principio de Kerckhoffs (Extension: .kck)\n";
        cout << "4. Salir\n";
        cout << CYAN << "Seleccione una tecnica (1-4): " << RESET;

        int opcionMetodo;
        cin >> opcionMetodo;

        if (cin.fail() || opcionMetodo == 4) {
            cout << VERDE << "\nSaliendo del programa..." << RESET << "\n";
            break;
        }

        if (opcionMetodo < 1 || opcionMetodo > 3) {
            cout << ROJO << "[!] Opcion invalida. Reintente." << RESET << "\n";
            cin.clear();
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');
            continue;
        }

        string extRequerida = ".ces";
        if (opcionMetodo == 2) extRequerida = ".alb";
        if (opcionMetodo == 3) extRequerida = ".kck";

        while (true) { // SUBMENÚ 1: Encriptar / Desencriptar
            cout << "\n" << AMARILLO << "--- OPERACION ---" << RESET << "\n";
            cout << "1. Encriptar\n";
            cout << "2. Desencriptar\n";
            cout << "0. Volver al menu anterior\n";
            cout << CYAN << "Seleccione accion (0-2): " << RESET;

            int opcionAccion;
            cin >> opcionAccion;

            if (opcionAccion == 0) {
                cout << AMARILLO << "[i] Regresando a la seleccion de técnica..." << RESET << "\n";
                break; // Vuelve al Menú Principal
            }

            if (opcionAccion < 1 || opcionAccion > 2) {
                cout << ROJO << "[!] Accion invalida." << RESET << "\n";
                continue;
            }

            string textoEntrada = "";

            while (true) { // SUBMENÚ 2: Selección de Origen de Datos
                if (opcionAccion == 1) { // ENCRIPTAR
                    cout << "\n" << AMARILLO << "¿Como desea ingresar el texto a encriptar?" << RESET << "\n";
                    cout << "1. Escribir manualmente por teclado\n";
                    cout << "2. Pegar desde el portapapeles\n";
                    cout << "3. Cargar desde un archivo de texto plano (.txt <= 2MB)\n";
                    cout << "0. Volver al menu anterior\n";
                    cout << CYAN << "Seleccione opcion (0-3): " << RESET;

                    int opc;
                    cin >> opc;
                    cin.ignore((numeric_limits<streamsize>::max)(), '\n');

                    if (opc == 0) {
                        cout << AMARILLO << "[i] Regresando a la seleccion de operacion..." << RESET << "\n";
                        break; // Vuelve al Submenú 1 (Encriptar / Desencriptar)
                    } else if (opc == 2) {
                        textoEntrada = obtenerTextoPortapapeles();
                    } else if (opc == 3) {
                        textoEntrada = cargarArchivoTextoPlano();
                    } else if (opc == 1) {
                        cout << CYAN << "Ingrese el texto a encriptar: " << RESET;
                        getline(cin, textoEntrada);
                    } else {
                        cout << ROJO << "[!] Opcion invalida." << RESET << "\n";
                        continue;
                    }
                } else { // DESENCRIPTAR
                    cout << "\n" << AMARILLO << "¿De donde desea obtener el texto cifrado?" << RESET << "\n";
                    cout << "1. Pegar desde el portapapeles\n";
                    cout << "2. Leer desde un archivo cifrado (" << extRequerida << ")\n";
                    cout << "0. Volver al menu anterior\n";
                    cout << CYAN << "Seleccione opcion (0-2): " << RESET;

                    int opc;
                    cin >> opc;
                    cin.ignore((numeric_limits<streamsize>::max)(), '\n');

                    if (opc == 0) {
                        cout << AMARILLO << "[i] Regresando a la seleccion de operacion..." << RESET << "\n";
                        break; // Vuelve al Submenú 1 (Encriptar / Desencriptar)
                    } else if (opc == 2) {
                        textoEntrada = cargarDesdeArchivoCifrado(extRequerida);
                    } else if (opc == 1) {
                        textoEntrada = obtenerTextoPortapapeles();
                    } else {
                        cout << ROJO << "[!] Opcion invalida." << RESET << "\n";
                        continue;
                    }
                }

                // Si el usuario canceló la lectura del archivo o el texto está vacío, reintenta la entrada
                if (textoEntrada.empty()) {
                    continue;
                }

                string resultado = "";

                // Procesamiento según la técnica elegida
                switch (opcionMetodo) {
                    case 1: { // CÉSAR
                        cout << CYAN << "Ingrese el desplazamiento (numero entero): " << RESET;
                        int desp;
                        cin >> desp;
                        cin.ignore((numeric_limits<streamsize>::max)(), '\n');
                        resultado = (opcionAccion == 1) ? cifrarCesar(textoEntrada, desp) : descifrarCesar(textoEntrada, desp);
                        break;
                    }
                    case 2: { // ALBERTI
                        cout << CYAN << "Ingrese la clave polialfabetica: " << RESET;
                        string clave;
                        cin >> clave;
                        cin.ignore((numeric_limits<streamsize>::max)(), '\n');
                        resultado = (opcionAccion == 1) ? cifrarAlberti(textoEntrada, clave) : descifrarAlberti(textoEntrada, clave);
                        break;
                    }
                    case 3: { // KERCKHOFFS
                        cout << CYAN << "Ingrese la clave secreta: " << RESET;
                        string claveSecreta;
                        cin >> claveSecreta;
                        cin.ignore((numeric_limits<streamsize>::max)(), '\n');
                        resultado = (opcionAccion == 1) ? encriptarKerckhoffs(textoEntrada, claveSecreta) : desencriptarKerckhoffs(textoEntrada, claveSecreta);
                        break;
                    }
                }

                // Mostrar el resultado procesado
                cout << "\n" << VERDE << "==================================================" << RESET << "\n";
                cout << VERDE << (opcionAccion == 1 ? " RESULTADO ENCRIPTADO:" : " RESULTADO DESENCRIPTADO:") << RESET << "\n";
                cout << MAGENTA << resultado << RESET << "\n";
                cout << VERDE << "==================================================" << RESET << "\n";

                if (copiarTextoAPortapapeles(resultado)) {
                    cout << VERDE << "[+] Resultado copiado al portapapeles." << RESET << "\n";
                }

                // Guardado opcional en disco
                if (opcionAccion == 1) {
                    cout << "\n" << AMARILLO << "¿Desea guardar este resultado en un archivo (" << extRequerida << ")? (S/N): " << RESET;
                    char guardarOpc;
                    cin >> guardarOpc;
                    cin.ignore((numeric_limits<streamsize>::max)(), '\n');

                    if (guardarOpc == 'S' || guardarOpc == 's') {
                        guardarEnArchivo(resultado, extRequerida);
                    }
                } else {
                    cout << "\n" << AMARILLO << "¿Desea guardar el texto desencriptado en un archivo (.txt)? (S/N): " << RESET;
                    char guardarOpc;
                    cin >> guardarOpc;
                    cin.ignore((numeric_limits<streamsize>::max)(), '\n');

                    if (guardarOpc == 'S' || guardarOpc == 's') {
                        guardarEnArchivo(resultado, ".txt");
                    }
                }

                // Una vez completada la operación con éxito, rompe el bucle interno y vuelve a la elección de operación
                break;
            }
        }
    }

    return 0;
}