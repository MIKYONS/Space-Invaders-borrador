#pragma once
#include "Character.hpp"

class UFO : public Character {
private:
    bool alive;
    float speed;
    Clock animClock;
    bool pasoAlterno;

public:
    UFO(const Vector2f& pos, const Texture& texture, const Vector2f& scale) 
        : Character(pos, texture, scale), alive(false), speed(0.12f), pasoAlterno(false) {
        
        // Centrar origen para rotaciones o pequeños efectos visuales
        Vector2u size = texture.getSize();
        setOrigin(size.x / 2.f, size.y / 2.f);
    }

    bool isAlive() const { return alive; }
    
    void spawn() {
        alive = true;
        // Aparece fuera de la pantalla por la izquierda, a la altura de la parte superior
        setPosition(Vector2f(-50.f, 60.f)); 
    }

    void kill() {
        alive = false;
        // Moverlo lejos para evitar colisiones fantasmas
        setPosition(Vector2f(-500.f, -500.f)); 
    }

    void update(float dt_factor) {
        if (!alive) return;

        // Movimiento de izquierda a derecha en la parte superior
        move(Vector2f(speed * dt_factor, 0.f));

        // Pequeño bamboleo/animación aérea
        if (animClock.getElapsedTime().asSeconds() >= 0.25f) {
            pasoAlterno = !pasoAlterno;
            setRotation(pasoAlterno ? 4.f : -4.f);
            animClock.restart();
        }

        // Si sale por el borde derecho de la pantalla (ancho 950), desaparece
        if (getPosition().x > 1000.f) {
            alive = false;
        }
    }
};
