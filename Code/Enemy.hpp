#pragma once

#include "Character.hpp"

class Enemy : public Character 
{
public:
	Enemy(const Vector2f& pos, const Texture& texture, const Vector2f& scale) : Character(pos, texture, scale) {
		alive = true;
		pasoAlterno = false;
		// Centrar el origen para que rote sobre su centro
		Vector2u size = texture.getSize();
		setOrigin(size.x / 2.f, size.y / 2.f);
	}
	bool isAlive();
	void kill();
	void reset();
	void animar(int desfase);

private:
	bool alive;
	bool pasoAlterno;
	Clock animClock;
};
