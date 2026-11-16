/************************************************************************
* Programmers: Subham, Ryan, Danielle, Manjesh                          *
* Class: CptS 122                                                       *
* Programming Assignment: PA 9 - Space Invaders                         *
* Date: 4/17/2021                                                       *
* Description: This program emulates space invaders.                    *
************************************************************************/

#pragma once

#include "Character.hpp"

class Missile : public Character
{
public:
    // Si la velocidad es (0, 0) el misil usa la velocidad vertical de siempre
    // (hacia arriba si es de la nave, hacia abajo si es enemigo).
    // Con una velocidad propia puede ir en diagonal (abanico, apuntado, triple...).
    Vector2f velocidad;

    Missile(const Vector2f& pos, const Texture& texture, const Vector2f& scale) : Character(pos, texture, scale), velocidad(0.f, 0.f)
    {

    }

    void setVelocidad(float vx, float vy) { velocidad = Vector2f(vx, vy); }
    bool tieneVelocidad() const { return velocidad.x != 0.f || velocidad.y != 0.f; }
};
