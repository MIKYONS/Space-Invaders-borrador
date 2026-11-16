#pragma once

#include "Character.hpp"
#include "Enemy.hpp"
#include "Missile.hpp"
#include "Explosion.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>

// ====================================================================
// PODERES DE LA NAVE (los sueltan los enemigos al morir; el UFO siempre suelta uno)
//   TRIPLE : disparo en abanico de tres misiles          (10 s)
//   RAPIDO : cadencia de disparo mucho mayor              (8 s)
//   ESCUDO : campo de fuerza, absorbe los misiles         (6 s)
//   VIDA   : +1 vida (máximo 9)
//   BOMBA  : destruye la fila de enemigos más baja
// Los iconos están en Poderes.png (5 cuadros de 40x40, en ese orden).
// ====================================================================
enum TipoPoder { P_TRIPLE, P_RAPIDO, P_ESCUDO, P_VIDA, P_BOMBA, NUM_PODERES };

struct Poder {
    sf::Sprite sprite;
    TipoPoder tipo;

    Poder(sf::Texture& hoja, TipoPoder t, sf::Vector2f pos) : sprite(hoja), tipo(t) {
        sprite.setTextureRect(sf::IntRect((int)t * 40, 0, 40, 40));
        sprite.setOrigin(20.f, 20.f);
        sprite.setPosition(pos);
    }
};

struct EstadoPoderes {
    std::vector<Poder> cayendo;                         // poderes que están cayendo
    float tTriple = 0.f, tRapido = 0.f, tEscudo = 0.f;  // segundos que les quedan
    float tInvul = 0.f;                                 // invulnerabilidad corta tras un golpe
    sf::Clock reloj;

    bool triple() const { return tTriple > 0.f; }
    bool rapido() const { return tRapido > 0.f; }
    bool escudo() const { return tEscudo > 0.f; }

    void reiniciar() {
        cayendo.clear();
        tTriple = tRapido = tEscudo = tInvul = 0.f;
    }

    // Suelta un poder al azar con 'prob' % de probabilidad
    void soltar(sf::Texture& hoja, sf::Vector2f pos, int prob) {
        if (rand() % 100 >= prob)
            return;
        int r = rand() % 10;   // pesos: triple 3, rápido 3, escudo 2, vida 1, bomba 1
        TipoPoder t = r < 3 ? P_TRIPLE : r < 6 ? P_RAPIDO : r < 8 ? P_ESCUDO : r < 9 ? P_VIDA : P_BOMBA;
        cayendo.push_back(Poder(hoja, t, pos));
    }

    // Una vez por frame: baja los tiempos, mueve los poderes y devuelve los que la nave recogió
    std::vector<TipoPoder> actualizar(Character& ship) {
        float dt = std::min(reloj.restart().asSeconds(), 0.1f);   // las pausas no gastan los poderes
        tTriple = std::max(0.f, tTriple - dt);
        tRapido = std::max(0.f, tRapido - dt);
        tEscudo = std::max(0.f, tEscudo - dt);
        tInvul  = std::max(0.f, tInvul - dt);

        std::vector<TipoPoder> recogidos;
        for (size_t k = 0; k < cayendo.size(); ) {
            cayendo[k].sprite.move(0.f, 0.25f);             // cae por frame, como los misiles
            if (cayendo[k].sprite.getGlobalBounds().intersects(ship.getGlobalBounds())) {
                recogidos.push_back(cayendo[k].tipo);
                cayendo.erase(cayendo.begin() + k);
            }
            else if (cayendo[k].sprite.getPosition().y > 1000.f)
                cayendo.erase(cayendo.begin() + k);
            else
                k++;
        }
        return recogidos;
    }

    // Texto para el marcador: "TRIPLE 7s  RAPIDO 3s ..."
    std::string texto() const {
        std::string s;
        if (triple()) s += "TRIPLE " + std::to_string((int)std::ceil(tTriple)) + "s  ";
        if (rapido()) s += "RAPIDO " + std::to_string((int)std::ceil(tRapido)) + "s  ";
        if (escudo()) s += "ESCUDO " + std::to_string((int)std::ceil(tEscudo)) + "s  ";
        return s;
    }
};

// Efecto inmediato al recoger un poder
inline void aplicarPoder(TipoPoder tipo, EstadoPoderes& p, int& lives, int& score, int level,
                         Character* enemies[10][5], std::vector<Explosion>& expl, sf::Texture& texExp) {
    switch (tipo) {
    case P_TRIPLE: p.tTriple = 10.f; break;
    case P_RAPIDO: p.tRapido = 8.f;  break;
    case P_ESCUDO: p.tEscudo = 6.f;  break;
    case P_VIDA:
        if (lives < 9)
            lives++;
        break;
    case P_BOMBA: {
        // Busca la fila más baja que todavía tenga enemigos vivos y la destruye
        int fila = -1;
        for (int j = 4; j >= 0 && fila < 0; j--)
            for (int i = 0; i < 10; i++)
                if (dynamic_cast<Enemy*>(enemies[i][j])->isAlive()) {
                    fila = j;
                    break;
                }
        if (fila >= 0)
            for (int i = 0; i < 10; i++) {
                Enemy* en = dynamic_cast<Enemy*>(enemies[i][fila]);
                if (en->isAlive()) {
                    en->kill();
                    score += level * 10;
                    expl.emplace_back(texExp, 6, en->getPosition(), 50);
                }
            }
        break;
    }
    default: break;
    }
}

// Disparo de la nave: un misil recto, o tres en abanico con el poder TRIPLE
inline void dispararJugador(Character& ship, std::vector<Missile*>& missiles, sf::Texture& tex, bool triple) {
    const float PI = 3.14159265f;
    sf::Vector2f p(ship.getPosition().x + 47.f, ship.getPosition().y - 40.f);

    missiles.push_back(new Missile(p, tex, sf::Vector2f(0.3f, 0.3f)));

    if (triple) {
        for (int lado = -1; lado <= 1; lado += 2) {
            Missile* m = new Missile(p, tex, sf::Vector2f(0.3f, 0.3f));
            float vx = 0.17f * lado, vy = -0.5f;
            m->setVelocidad(vx, vy);
            m->setRotation(std::atan2(vx, -vy) * 180.f / PI);
            missiles.push_back(m);
        }
    }
}
