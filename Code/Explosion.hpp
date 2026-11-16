#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>

// Explosión animada a partir de una hoja de sprites horizontal
// (todos los cuadros del mismo tamaño, uno junto a otro).
struct Explosion {
    sf::Sprite sprite;
    int frames, frameW, frameH, actual;
    sf::Time porFrame;
    sf::Clock reloj;
    bool terminada;

    // hoja: textura con los cuadros | numFrames: cuántos cuadros tiene
    // centro: dónde se centra la explosión | msPorFrame: duración de cada cuadro
    Explosion(sf::Texture& hoja, int numFrames, sf::Vector2f centro, int msPorFrame = 60)
        : sprite(hoja), frames(numFrames), actual(0),
          porFrame(sf::milliseconds(msPorFrame)), terminada(false)
    {
        frameW = hoja.getSize().x / numFrames;
        frameH = hoja.getSize().y;
        sprite.setTextureRect(sf::IntRect(0, 0, frameW, frameH));
        sprite.setOrigin(frameW / 2.f, frameH / 2.f);
        sprite.setPosition(centro);
    }

    void actualizar() {
        if (reloj.getElapsedTime() >= porFrame) {
            reloj.restart();
            actual++;
            if (actual >= frames) {
                terminada = true;
                return;
            }
            sprite.setTextureRect(sf::IntRect(actual * frameW, 0, frameW, frameH));
        }
    }
};

// Avanza las animaciones y elimina las que ya terminaron
inline void actualizarExplosiones(std::vector<Explosion>& v) {
    for (size_t i = 0; i < v.size(); ) {
        v[i].actualizar();
        if (v[i].terminada)
            v.erase(v.begin() + i);
        else
            i++;
    }
}

inline void dibujarExplosiones(sf::RenderWindow& w, std::vector<Explosion>& v) {
    for (size_t i = 0; i < v.size(); i++)
        w.draw(v[i].sprite);
}

// Explosión que detiene el juego unos instantes (para cuando destruyen la nave).
// 'escena' dibuja lo que debe seguir visible (escudos, enemigos...).
inline void explosionBloqueante(sf::RenderWindow& w, sf::Sprite& fondo, sf::Texture& hoja,
                                int frames, sf::Vector2f centro, int msPorFrame,
                                std::function<void()> escena = nullptr) {
    Explosion e(hoja, frames, centro, msPorFrame);
    while (!e.terminada && w.isOpen()) {
        e.actualizar();
        if (e.terminada)
            break;
        w.clear();
        w.draw(fondo);
        if (escena)
            escena();
        w.draw(e.sprite);
        w.display();
    }
}
