#pragma once

#include "Character.hpp"
#include "Enemy.hpp"
#include "Missile.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>

inline Enemy* comoEnemigo(Character* c) { return dynamic_cast<Enemy*>(c); }

// ====================================================================
// 1) MOVIMIENTO DE LA FORMACIÓN
//    Cada nivel usa un patrón distinto (y se repiten con más velocidad):
//      nivel 1: CLASICO     derecha-izquierda y baja al tocar el borde
//      nivel 2: OLA         vaivén con una onda vertical entre columnas
//      nivel 3: ACORDEON    la formación se abre y se cierra
//      nivel 4: ORBITA      recorre un ocho (figura 8)
//      nivel 5: SERPIENTE   cada fila se mueve desfasada de las demás
// ====================================================================
enum Movimiento { CLASICO, OLA, ACORDEON, ORBITA, SERPIENTE, NUM_MOVIMIENTOS };

struct PatronEnemigo {
    int   movimiento = CLASICO;
    float offX = 0.f, offY = 0.f;   // desplazamiento de toda la formación
    float fase = 0.f;               // avanza en cada frame (patrones ondulados)
    bool  derecha = true;           // solo para CLASICO

    void reiniciar() {
        offX = offY = fase = 0.f;
        derecha = true;
    }

    std::string nombre() const {
        static const char* n[] = { "CLASICO", "OLA", "ACORDEON", "ORBITA", "SERPIENTE" };
        return n[movimiento];
    }

    // Posición de un enemigo según el patrón (la casilla base es la misma de siempre)
    Vector2f posicion(int i, int j) const {
        float hx = 175.f + 60 * i, hy = 125.f + 50 * j;
        switch (movimiento) {
        case OLA:
            return Vector2f(hx + 30.f + 170.f * std::sin(fase),
                            hy + offY + 22.f * std::sin(4.f * fase - 0.7f * i));
        case ACORDEON: {
            float k = 1.f + 0.4f * std::sin(2.f * fase);
            return Vector2f(445.f + (hx - 445.f) * k,
                            225.f + offY + (hy - 225.f) * k);
        }
        case ORBITA:
            return Vector2f(hx + 30.f + 150.f * std::sin(fase),
                            hy + offY + 60.f * std::sin(2.f * fase));
        case SERPIENTE:
            return Vector2f(hx + 30.f + 160.f * std::sin(fase + 0.9f * j),
                            hy + offY + 12.f * std::sin(3.f * fase + 0.5f * i));
        default:
            return Vector2f(hx + offX, hy + offY);
        }
    }

    // Llamar una vez por frame (reemplaza al movimiento derecha/izquierda de antes)
    void actualizar(Character* enemies[10][5], int level) {
        movimiento = (level - 1) % NUM_MOVIMIENTOS;

        float v = 0.1f + level * 0.02f;               // misma velocidad base que el juego original

        if (movimiento == CLASICO) {
            offX += derecha ? v : -v;

            float minX = 1e9f, maxX = -1e9f;
            for (int i = 0; i < 10; i++)
                for (int j = 0; j < 5; j++)
                    if (comoEnemigo(enemies[i][j])->isAlive()) {
                        float x = 175.f + 60 * i + offX;
                        minX = std::min(minX, x);
                        maxX = std::max(maxX, x);
                    }

            if ((derecha && maxX > 915.f) || (!derecha && minX < 35.f)) {
                derecha = !derecha;
                offY += 9.f + level;                  // baja al tocar el borde
            }
        }
        else {
            fase += v / 140.f;                        // velocidad lateral parecida a la clásica
            offY += v * (9.f + level) / 340.f;        // bajada continua y lenta
        }

        for (int i = 0; i < 10; i++)
            for (int j = 0; j < 5; j++) {
                enemies[i][j]->setPosition(posicion(i, j));
                comoEnemigo(enemies[i][j])->animar(i);
            }
    }
};

// ====================================================================
// 2) ATAQUES ENEMIGOS
//    Cuántos tipos de ataque hay disponibles depende del nivel:
//      nivel 1: SIMPLE     un misil recto
//      nivel 2: RAFAGA     tres misiles seguidos desde el mismo enemigo
//      nivel 3: ABANICO    tres misiles en abanico
//      nivel 4: APUNTADO   un misil dirigido hacia la nave
//      nivel 5: LLUVIA     cuatro columnas disparan a la vez
//      nivel 7: BARRIDO    la fila de abajo dispara de lado a lado
//    Además, cada ataque ocurre más seguido y desde más enemigos a la vez.
// ====================================================================
enum TipoAtaque { A_SIMPLE, A_RAFAGA, A_ABANICO, A_APUNTADO, A_LLUVIA, A_BARRIDO };

struct DisparoProgramado {
    float cuando;      // segundos (en el reloj interno)
    int   i, j;        // enemigo que dispara
    int   clase;       // 0 recto, 1 abanico izquierda, 2 abanico derecha, 3 apuntado
};

struct SistemaAtaques {
    sf::Clock reloj;
    float proximo = 1.0f;
    std::vector<DisparoProgramado> cola;

    void reiniciar() {
        cola.clear();
        proximo = reloj.getElapsedTime().asSeconds() + 1.0f;
    }

    static int cantidadTipos(int level) {
        int n = 1;
        if (level >= 2) n++;
        if (level >= 3) n++;
        if (level >= 4) n++;
        if (level >= 5) n++;
        if (level >= 7) n++;
        return n;
    }

    // Enemigo vivo al azar
    static bool elegirVivo(Character* enemies[10][5], int& i, int& j) {
        std::vector<int> vivos;
        for (int a = 0; a < 10; a++)
            for (int b = 0; b < 5; b++)
                if (comoEnemigo(enemies[a][b])->isAlive())
                    vivos.push_back(a * 5 + b);
        if (vivos.empty())
            return false;
        int k = vivos[rand() % vivos.size()];
        i = k / 5;
        j = k % 5;
        return true;
    }

    // Enemigo vivo más bajo de una columna
    static bool masBajo(Character* enemies[10][5], int col, int& j) {
        for (int b = 4; b >= 0; b--)
            if (comoEnemigo(enemies[col][b])->isAlive()) {
                j = b;
                return true;
            }
        return false;
    }

    void programar(float ahora, float retraso, int i, int j, int clase) {
        DisparoProgramado d = { ahora + retraso, i, j, clase };
        cola.push_back(d);
    }

    // Crea el misil enemigo (recto, en abanico o apuntado)
    static void lanzar(Character* e, std::vector<Missile*>& enemyM, sf::Texture& tex, int clase, Vector2f objetivo) {
        Vector2f p = e->getPosition();
        Missile* m = new Missile(Vector2f(p.x - 4.f, p.y + 10.f), tex, Vector2f(0.3f, 0.3f));

        const float PI = 3.14159265f;
        const float vel = 0.5f;                      // misma velocidad que el misil de siempre
        float vx = 0.f, vy = vel;
        bool diagonal = true;

        if (clase == 1 || clase == 2) {
            float ang = (clase == 1 ? -18.f : 18.f) * PI / 180.f;
            vx = vel * std::sin(ang);
            vy = vel * std::cos(ang);
        }
        else if (clase == 3) {
            float dx = objetivo.x - p.x, dy = objetivo.y - p.y;
            float len = std::sqrt(dx * dx + dy * dy);
            if (len > 1.f && dy > 40.f) {
                vx = dx / len * vel;
                vy = dy / len * vel;
            }
            else
                diagonal = false;
        }
        else
            diagonal = false;

        if (diagonal) {
            m->setVelocidad(vx, vy);
            m->setRotation(std::atan2(-vx, vy) * 180.f / PI);
        }
        enemyM.push_back(m);
    }

    // Llamar una vez por frame (reemplaza al disparo aleatorio de antes)
    void actualizar(Character* enemies[10][5], std::vector<Missile*>& enemyM, sf::Texture& tex, Character& ship, int level) {
        float ahora = reloj.getElapsedTime().asSeconds();
        Vector2f objetivo(ship.getPosition().x + 52.f, ship.getPosition().y + 25.f);
        size_t tope = 10 + 2 * level;                // máximo de misiles enemigos en pantalla

        // 1) Disparos que ya les toca salir
        for (size_t k = 0; k < cola.size(); ) {
            if (cola[k].cuando <= ahora) {
                Character* e = enemies[cola[k].i][cola[k].j];
                if (comoEnemigo(e)->isAlive() && enemyM.size() < tope)
                    lanzar(e, enemyM, tex, cola[k].clase, objetivo);
                cola.erase(cola.begin() + k);
            }
            else
                k++;
        }

        // 2) ¿Toca un ataque nuevo?
        if (ahora < proximo)
            return;
        proximo = ahora + std::max(0.40f, 1.0f - 0.06f * level);

        int atacantes = 1 + level / 4;               // más enemigos atacando a la vez
        int tipos = cantidadTipos(level);

        for (int n = 0; n < atacantes; n++) {
            int ataque = rand() % tipos;             // 0..tipos-1, en el orden de TipoAtaque

            int i, j;
            switch (ataque) {
            case A_SIMPLE:
                if (elegirVivo(enemies, i, j))
                    programar(ahora, 0.f, i, j, 0);
                break;
            case A_RAFAGA:
                if (elegirVivo(enemies, i, j))
                    for (int r = 0; r < 3; r++)
                        programar(ahora, 0.14f * r, i, j, 0);
                break;
            case A_ABANICO:
                if (elegirVivo(enemies, i, j)) {
                    programar(ahora, 0.f, i, j, 1);
                    programar(ahora, 0.f, i, j, 0);
                    programar(ahora, 0.f, i, j, 2);
                }
                break;
            case A_APUNTADO:
                if (elegirVivo(enemies, i, j))
                    programar(ahora, 0.f, i, j, 3);
                break;
            case A_LLUVIA:
                for (int r = 0; r < 4; r++) {
                    int col = rand() % 10;
                    if (masBajo(enemies, col, j))
                        programar(ahora, 0.f, col, j, 0);
                }
                break;
            case A_BARRIDO: {
                bool haciaDerecha = rand() % 2 == 0;
                for (int c = 0; c < 10; c++) {
                    int col = haciaDerecha ? c : 9 - c;
                    if (masBajo(enemies, col, j))
                        programar(ahora, 0.09f * c, col, j, 0);
                }
                break;
            }
            }
        }
    }
};

// Elimina los misiles que ya salieron de la pantalla (arriba, abajo o por los lados)
inline void limpiarFuera(std::vector<Missile*>& v) {
    for (size_t k = 0; k < v.size(); ) {
        Vector2f p = v[k]->getPosition();
        if (p.y < -120.f || p.y > 1060.f || p.x < -60.f || p.x > 1010.f) {
            delete v[k];
            v.erase(v.begin() + k);
        }
        else
            k++;
    }
}
