#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <random>
#include <chrono>

using std::cout;
using std::endl;
using std::mt19937;
using std::uniform_int_distribution;
using std::uniform_real_distribution;
using std::random_device;
using std::vector;

using namespace std::chrono;

using Matriz = vector<vector<float>>;
using Ruta = vector<int>;
using BoolVector = vector<bool>;

const int NUM_HORMIGAS = 5;
const int NUM_CIUDADES = 5;
const float ALPHA = 1.0f;
const float BETA = 5.0f;
const float EVAPORACION = 0.5f;
const float FEROMONA_INICIAL = 1.0f;
const int ITERACIONES = 100;

Matriz distancias = {
    {0, 3, 9, 2, 5}, 
    {3, 0, 4, 8, 2}, 
    {9, 4, 0, 8, 1}, 
    {2, 8, 8, 0, 4}, 
    {5, 2, 1, 4, 0}  
};

Matriz feromonas(NUM_CIUDADES, vector<float>(NUM_CIUDADES, FEROMONA_INICIAL));

void inicializarFeromonas() {
    for (auto& fila : feromonas) {
        std::fill(fila.begin(), fila.end(), FEROMONA_INICIAL);
    }
}

float calcularProbabilidad(int ciudad_actual, int ciudad_siguiente, const BoolVector& visitado) {
    if (visitado[ciudad_siguiente]) return 0.0f;
    return pow(feromonas[ciudad_actual][ciudad_siguiente], ALPHA) *
           pow(1.0f / distancias[ciudad_actual][ciudad_siguiente], BETA);
}

Ruta construirSolucion(float& costo_total, vector<float>& probabilidades) {
    Ruta ruta(NUM_CIUDADES);
    BoolVector visitado(NUM_CIUDADES, false);
    costo_total = 0.0f;

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, NUM_CIUDADES - 1);

    int ciudad_actual = dis(gen);
    ruta[0] = ciudad_actual;
    visitado[ciudad_actual] = true;

    for (int step = 1; step < NUM_CIUDADES; ++step) {
        vector<float> probabilidades_local(NUM_CIUDADES);
        float total_probabilidad = 0.0f;

        for (int j = 0; j < NUM_CIUDADES; ++j) {
            probabilidades_local[j] = calcularProbabilidad(ciudad_actual, j, visitado);
            total_probabilidad += probabilidades_local[j];
        }

        // Guardamos las probabilidades locales para imprimirlas más tarde
        probabilidades = probabilidades_local;

        uniform_real_distribution<> prob_dis(0, total_probabilidad);
        float prob_seleccion = prob_dis(gen);
        float suma_probabilidad = 0.0f;
        int siguiente_ciudad = -1;

        for (int j = 0; j < NUM_CIUDADES; ++j) {
            suma_probabilidad += probabilidades_local[j];
            if (suma_probabilidad >= prob_seleccion) {
                siguiente_ciudad = j;
                break;
            }
        }

        ruta[step] = siguiente_ciudad;
        visitado[siguiente_ciudad] = true;
        costo_total += distancias[ciudad_actual][siguiente_ciudad];
        ciudad_actual = siguiente_ciudad;
    }

    costo_total += distancias[ciudad_actual][ruta[0]];

    return ruta;
}

void actualizarFeromonas(const vector<Ruta>& rutas, const vector<float>& costos) {
    for (auto& fila : feromonas) {
        for (auto& valor : fila) {
            valor *= (1.0f - EVAPORACION);
        }
    }

    for (int i = 0; i < NUM_HORMIGAS; ++i) {
        float costo = costos[i];
        const Ruta& ruta = rutas[i];

        for (int j = 0; j < NUM_CIUDADES - 1; ++j) {
            int a = ruta[j];
            int b = ruta[j + 1];
            feromonas[a][b] += 1.0f / costo;
            feromonas[b][a] += 1.0f / costo;
        }

        int ultima_ciudad = ruta[NUM_CIUDADES - 1];
        int primera_ciudad = ruta[0];
        feromonas[ultima_ciudad][primera_ciudad] += 1.0f / costo;
        feromonas[primera_ciudad][ultima_ciudad] += 1.0f / costo;
    }
}

void imprimirInformacion(const vector<Ruta>& rutas, const vector<float>& costos, const vector<vector<float>>& probabilidades) {
    for (int i = 0; i < NUM_HORMIGAS; ++i) {
        cout << "Hormiga " << i << ": Ruta = ";
        for (int ciudad : rutas[i]) {
            cout << ciudad << " ";
        }
        cout << "| Costo = " << costos[i];

        // Imprimir probabilidades de la hormiga i
        cout << "| Probabilidades = ";
        for (float prob : probabilidades[i]) {
            cout << prob << " ";
        }
        cout << endl;
    }
}

int main() {
    auto inicio = high_resolution_clock::now(); // Iniciar el temporizador

    inicializarFeromonas();

    for (int iter = 0; iter < ITERACIONES; ++iter) {
        vector<Ruta> rutas(NUM_HORMIGAS);
        vector<float> costos(NUM_HORMIGAS);
        vector<vector<float>> probabilidades(NUM_HORMIGAS);

        for (int i = 0; i < NUM_HORMIGAS; ++i) {
            rutas[i] = construirSolucion(costos[i], probabilidades[i]);
        }

        actualizarFeromonas(rutas, costos);

        cout << "Iteración " << iter + 1 << endl;
        imprimirInformacion(rutas, costos, probabilidades);
    }

    cout << "Algoritmo ACO completado." << endl;

    auto fin = high_resolution_clock::now();
    auto duracion = duration_cast<milliseconds>(fin - inicio).count();
    cout << "Duración total de ejecución: " << duracion << " ms" << endl;

    return 0;
}
