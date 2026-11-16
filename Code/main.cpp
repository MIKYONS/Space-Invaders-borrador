#include "header.hpp"
#include "Character.hpp"
#include "User.hpp"
#include "Enemy.hpp"
#include "Missile.hpp"
#include "Menu.hpp"
#include "Explosion.hpp"
#include "UFO.hpp"
#include "Poderes.hpp"
#include "Patrones.hpp"

void delay(int x);
void ship_movement(Character& ship, RenderWindow& window);
void missile_movement(vector<Missile*>& missiles, vector<Missile*>& enemyM);
void draw_missiles(vector<Missile*>& missiles, vector<Missile*>& enemyM, RenderWindow& window);

int main() {
    srand((unsigned)time(nullptr));

    Menu m;

    bool next_level = true, indicator = true, dir = true, reset_check = false, no_test = true;
    int level = 1, lives = 5, score = 0, tChange1 = 1, tChange2 = 1, tChange3 = 1;

    Clock c, c2, c3;

    RenderWindow window(VideoMode(950, 950), "Space Invaders"); //Size of window

    Texture t1, t2, t3, t4, t5, t6, t7, t8; //Allows different textures for items

    //In-game music
    Music music;
    music.openFromFile("music.ogg");
    music.setLoop(true);
    music.play();
    music.setVolume(10.f);

    //Loads action sounds
    SoundBuffer buffer;
    SoundBuffer buffer2;
    SoundBuffer buffer3;
    buffer.loadFromFile("shoot.wav");
    buffer2.loadFromFile("invaderkilled.wav");
    buffer3.loadFromFile("explosion.wav");

    Sound missile_sound;
    Sound invader_killed;
    Sound ship_hit;
    
    // Fondo y hojas de sprites para explosiones
    sf::Texture texFondo, texExpShield, texExpShip, texExpBig;
    texFondo.loadFromFile("Fondo.png");
    texExpShield.loadFromFile("ExplosionShield.png");
    texExpShip.loadFromFile("ExplosionShip.png");
    texExpBig.loadFromFile("ExplosionBig.png");

    sf::Sprite spriteFondo(texFondo);
    spriteFondo.setScale(
        float(window.getSize().x) / texFondo.getSize().x,
        float(window.getSize().y) / texFondo.getSize().y
    );

    std::vector<Explosion> explosiones;

    missile_sound.setBuffer(buffer);
    invader_killed.setBuffer(buffer2);
    ship_hit.setBuffer(buffer3);

    invader_killed.setVolume(5.f);
    missile_sound.setVolume(5.f);
    ship_hit.setVolume(5.f);

    //Sets up different text in the game
    Text scoreT, livesT, levelT, scoreEnd, levelEnd, endMessage, testCase;
    Font font;
    font.loadFromFile("font.ttf");

    scoreT.setFont(font);
    livesT.setFont(font);
    levelT.setFont(font);
    scoreEnd.setFont(font);
    levelEnd.setFont(font);
    endMessage.setFont(font);
    testCase.setFont(font);

    scoreT.setCharacterSize(25);
    livesT.setCharacterSize(25);
    levelT.setCharacterSize(25);
    scoreEnd.setCharacterSize(25);
    levelEnd.setCharacterSize(25);
    endMessage.setCharacterSize(25);
    testCase.setCharacterSize(25);

    scoreT.setFillColor(Color::White);
    livesT.setFillColor(Color::White);
    levelT.setFillColor(Color::White);
    scoreEnd.setFillColor(Color::White);
    levelEnd.setFillColor(Color::White);
    endMessage.setFillColor(Color::White);
    testCase.setFillColor(Color::White);

    scoreT.setPosition(Vector2f(75.f, 0.f));
    livesT.setPosition(Vector2f(425.f, 0.f));
    levelT.setPosition(Vector2f(675.f, 0.f));
    scoreEnd.setPosition(Vector2f(125.f, 450.f));
    levelEnd.setPosition(Vector2f(500.f, 450.f));
    endMessage.setPosition(Vector2f(125.f, 500.f));
    testCase.setPosition(Vector2f(150.f, 0.f));

    endMessage.setString("Press enter to continue...");

    t1.loadFromFile("Ship.png"); //Loads image
    Character* ship = new User(Vector2f(425.f, 825.f), t1, Vector2f(0.1f, 0.1f)); //Polymorphism because user is a character

    Character* enemies[10][5] = { nullptr }; //Allows for matrix of enemies
    t2.loadFromFile("Alien.png");
    
    Texture t2b, t2c;
    t2b.loadFromFile("Alien2.png");
    t2c.loadFromFile("Alien3.png");

    Texture* tipo[5] = { &t2c, &t2b, &t2b, &t2, &t2 };   // fila 0: calamar, 1-2: pulpo, 3-4: cangrejo

    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 5; j++)
            enemies[i][j] = new Enemy(Vector2f(175.f + 60 * i, 125.f + 50 * j), *tipo[j], Vector2f(0.047f, 0.047f));

    t3.loadFromFile("Missile.png");
    vector<Missile*> missiles; //Keeps track of user missiles on the screen

    t4.loadFromFile("enemyMissile.png");
    vector<Missile*> enemyM; //Keeps track of enemy missiles on the screen

    //Helper variables for insertion into the vectors
    Missile* missile = nullptr;
    Missile* enemyMi = nullptr;

    //Each shield has its own vector to keep track of the texture cycle
    vector<Texture> shieldT1;
    vector<Texture> shieldT2;
    vector<Texture> shieldT3;

    t5.loadFromFile("Shield1.png");
    t6.loadFromFile("Shield2.png");
    t7.loadFromFile("Shield3.png");
    t8.loadFromFile("Shield4.png");

    //Inserts the cycle of textures
    shieldT1.push_back(t5);
    shieldT1.push_back(t6);
    shieldT1.push_back(t7);
    shieldT1.push_back(t8);
    shieldT2.push_back(t5);
    shieldT2.push_back(t6);
    shieldT2.push_back(t7);
    shieldT2.push_back(t8);
    shieldT3.push_back(t5);
    shieldT3.push_back(t6);
    shieldT3.push_back(t7);
    shieldT3.push_back(t8);

    //Declares shield sprites
    Sprite shieldS1(shieldT1.at(0));
    Sprite shieldS2(shieldT2.at(0));
    Sprite shieldS3(shieldT3.at(0));

    shieldS1.setPosition(Vector2f(125.f, 650.f));
    shieldS2.setPosition(Vector2f(425.f, 650.f));
    shieldS3.setPosition(Vector2f(725.f, 650.f));

    shieldS1.setScale(Vector2f(0.5f, 0.5f));
    shieldS2.setScale(Vector2f(0.5f, 0.5f));
    shieldS3.setScale(Vector2f(0.5f, 0.5f));
    
    // Cargar la textura del UFO especial y volver transparente su fondo blanco
    sf::Image imgUfo;
    Texture t_ufo;
    if (imgUfo.loadFromFile("UFO.png")) {
        imgUfo.createMaskFromColor(sf::Color::White);
        t_ufo.loadFromImage(imgUfo);
    }

    // Instanciar el UFO inicialmente inactivo fuera de la pantalla
    UFO* ufoEspecial = new UFO(Vector2f(-100.f, -100.f), t_ufo, Vector2f(0.06f, 0.06f)); 
    bool ufoSpawned = false; // Asegura que solo se genere una vez al llegar a 666 score

    // ---- Poderes de la nave y patrones de enemigos por nivel ----
    sf::Texture texPoderes;
    texPoderes.loadFromFile("Poderes.png");
    EstadoPoderes poderes;      // poderes que caen de los enemigos
    PatronEnemigo patron;       // movimiento de la formación (cambia con el nivel)
    SistemaAtaques ataques;     // disparos enemigos (más tipos y más seguidos cada nivel)

    // Los tres escudos en arreglos, para tratarlos con un ciclo
    Sprite* escudos[3]         = { &shieldS1, &shieldS2, &shieldS3 };
    vector<Texture>* ciclos[3] = { &shieldT1, &shieldT2, &shieldT3 };
    int* cambios[3]            = { &tChange1, &tChange2, &tChange3 };

    Text infoT;                 // patrón actual, ataques y poderes activos
    infoT.setFont(font);
    infoT.setCharacterSize(18);
    infoT.setFillColor(Color(200, 220, 255));
    infoT.setPosition(Vector2f(75.f, 32.f));

    CircleShape campo(60.f, 8); // octágono: campo de fuerza "pixelado"
    campo.setOrigin(60.f, 60.f);
    campo.setScale(1.0f, 0.62f);
    campo.setFillColor(Color(120, 210, 255, 45));
    campo.setOutlineColor(Color(120, 210, 255, 220));
    campo.setOutlineThickness(3.f);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }

        while (indicator && no_test)
            m.displayMenu(&window, score, lives, indicator, no_test); //Display menu until user enters a valid input

        if (no_test == false)
            goto test_cases;

        //Skips checking each enemy during rounds to let program run smoother
        if (reset_check)
            goto escape1;

        //If all ememies are dead, this sets that indicator
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                if (dynamic_cast <Enemy*> (enemies[i][j])->isAlive() == true)
                {
                    reset_check = false;
                    goto escape1;
                }
                else
                    reset_check = true;
            }
        }

    escape1:
        //When user dies or goes to next level
        if (reset_check)
        {
            //Resets position
            for (int i = 0; i < 10; i++)
                for (int j = 0; j < 5; j++)
                    enemies[i][j]->setPosition(Vector2f(175.f + 60 * i, 125.f + 50 * j));

            //Makes each enemy alive
            for (int i = 0; i < 10; i++)
                for (int j = 0; j < 5; j++)
                    dynamic_cast <Enemy*> (enemies[i][j])->reset();

            reset_check = false;
            patron.reiniciar();
            ataques.reiniciar();
        }

        //Updates statistics real-time
        scoreT.setString("Score: " + to_string(score));
        livesT.setString("Lives: " + to_string(lives));
        levelT.setString("Level: " + to_string(level));

        //Moves ship if it is within bounds of screen
        ship_movement(*ship, window);

        //Generates new missiles every 0.375 seconds or when user presses space, whichever is longest in time passed
        if (Keyboard::isKeyPressed(Keyboard::Space) && c.getElapsedTime().asSeconds() > (poderes.rapido() ? 0.15f : 0.375f)) {
            dispararJugador(*ship, missiles, t3, poderes.triple()); //Un misil, o tres con el poder TRIPLE
            c.restart();
            missile_sound.play();
        }

        //Generates new enemy missiles, which get faster each level
        //Ataques enemigos: cada nivel suma tipos nuevos y más atacantes (ver Patrones.hpp)
        ataques.actualizar(enemies, enemyM, t4, *ship, level);

        //If there are missiles, they move
        missile_movement(missiles, enemyM);

        //Movimiento de la formación: cada nivel usa un patrón distinto (ver Patrones.hpp)
        patron.actualizar(enemies, level);

        // Activar y actualizar el UFO especial al alcanzar 666 de score
        if (score >= 666 && !ufoSpawned) {
            ufoEspecial->spawn();
            ufoSpawned = true;
        }

        if (ufoEspecial->isAlive()) {
            ufoEspecial->update(1.0f + (level * 0.1f));
        }

        // Colisiones de misiles del jugador contra el UFO y los enemigos
        for (size_t k = 0; k < missiles.size(); ) {
            bool impacto = false;

            if (ufoEspecial->isAlive() && ufoEspecial->getGlobalBounds().intersects(missiles.at(k)->getGlobalBounds())) {
                sf::Vector2f posImpacto = ufoEspecial->getPosition();
                ufoEspecial->kill();
                invader_killed.play();
                explosiones.emplace_back(texExpBig, 10, posImpacto, 60);
                poderes.soltar(texPoderes, posImpacto, 100); // el UFO siempre suelta un poder
                score += 300;
                impacto = true;
            }

            for (int i = 0; i < 10 && !impacto; i++)
                for (int j = 0; j < 5 && !impacto; j++) {
                    Enemy* en = dynamic_cast <Enemy*> (enemies[i][j]);
                    if (en->isAlive() && en->getGlobalBounds().intersects(missiles.at(k)->getGlobalBounds())) {
                        en->kill();
                        invader_killed.play();
                        poderes.soltar(texPoderes, en->getPosition(), 14); // 14 % de soltar un poder
                        score += level * 10;
                        impacto = true;
                    }
                }

            if (impacto) {
                delete missiles.at(k);
                missiles.erase(missiles.begin() + k);
            }
            else
                k++;
        }
        limpiarFuera(missiles); //Quita los misiles que salen de la pantalla (también por los lados)

        // Colisiones de misiles enemigos contra la nave y los escudos
        for (size_t i = 0; i < enemyM.size(); ) {
            Missile* em = enemyM.at(i);
            bool consumido = false;

            // Si un misil enemigo golpea la nave
            if (lives > 0 && em->getGlobalBounds().intersects(ship->getGlobalBounds())) {
                consumido = true;
                sf::Vector2f centroNave(ship->getPosition().x + 52.f, ship->getPosition().y + 30.f);

                if (poderes.escudo()) {
                    // El campo de fuerza absorbe el misil
                    explosiones.emplace_back(texExpShield, 6, em->getPosition(), 40);
                }
                else if (poderes.tInvul <= 0.f) {
                    lives--;
                    ship_hit.play();
                    poderes.tInvul = 1.2f; // breve invulnerabilidad tras recibir un golpe

                    sf::Vector2f centro = centroNave;
                    if (lives > 0) {
                        // Explosión roja normal sobre la nave
                        explosiones.emplace_back(texExpShip, 6, centro, 55);
                    } else {
                        // Desintegración grande cuando matan la nave
                        explosionBloqueante(window, spriteFondo, texExpBig, 10, centro, 80, [&]() {
                            window.draw(scoreT);
                            window.draw(livesT);
                            window.draw(levelT);
                            window.draw(shieldS1);
                            window.draw(shieldS2);
                            window.draw(shieldS3);
                            for (int ei = 0; ei < 10; ei++)
                                for (int ej = 0; ej < 5; ej++)
                                    if ((dynamic_cast <Enemy*> (enemies[ei][ej]))->isAlive())
                                        window.draw(*enemies[ei][ej]);
                        });
                    }
                }
                // (con invulnerabilidad el misil se pierde sin hacer daño)
            }

            // Si golpea alguno de los escudos (explosión roja con amarillo)
            for (int s = 0; s < 3 && !consumido; s++) {
                if (*cambios[s] <= 3 && em->getGlobalBounds().intersects(escudos[s]->getGlobalBounds())) {
                    explosiones.emplace_back(texExpShield, 6, em->getPosition(), 50);
                    escudos[s]->setTexture(ciclos[s]->at(*cambios[s]));
                    (*cambios[s])++;
                    consumido = true;
                }
            }

            if (consumido) {
                delete em;
                enemyM.erase(enemyM.begin() + i);
            }
            else
                i++;
        }
        limpiarFuera(enemyM); //Quita los misiles que salen de la pantalla (también por los lados)

        // Colisiones de enemigos contra el límite inferior o los escudos
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 5; j++) {
                Enemy* en = dynamic_cast <Enemy*> (enemies[i][j]);
                if (!en->isAlive())
                    continue;

                //Si un enemigo llega a y = 900 el jugador pierde automáticamente
                if (en->getPosition().y + 50 >= 900)
                    lives = 0;

                //Si un enemigo toca un escudo, muere y el escudo recibe daño
                for (int s = 0; s < 3; s++) {
                    if (*cambios[s] <= 3 && en->getGlobalBounds().intersects(escudos[s]->getGlobalBounds())) {
                        en->kill();
                        escudos[s]->setTexture(ciclos[s]->at(*cambios[s]));
                        (*cambios[s])++;
                        explosiones.emplace_back(texExpShield, 6, en->getPosition(), 50);
                        break;
                    }
                }
            }
        }

        // Poderes: caen, la nave los recoge y se aplican
        {
            std::vector<TipoPoder> recogidos = poderes.actualizar(*ship);
            for (size_t k = 0; k < recogidos.size(); k++)
                aplicarPoder(recogidos[k], poderes, lives, score, level, enemies, explosiones, texExpShield);
        }

        // Actualizar animaciones de explosiones activas
        actualizarExplosiones(explosiones);

        // Dibujar toda la escena
        window.clear();
        window.draw(spriteFondo);

        window.draw(scoreT);
        window.draw(livesT);
        window.draw(levelT);

        window.draw(shieldS1);
        window.draw(shieldS2);
        window.draw(shieldS3);

        draw_missiles(missiles, enemyM, window);

        if (lives > 0) {
            //Parpadea mientras dura la invulnerabilidad tras un golpe
            if (poderes.tInvul <= 0.f || (int)(poderes.tInvul * 12) % 2 == 0)
                window.draw(*ship);

            if (poderes.escudo()) {
                campo.setPosition(ship->getPosition().x + 52.f, ship->getPosition().y + 25.f);
                window.draw(campo);
            }
        }

        for (int i = 0; i < 10; i++)
            for (int j = 0; j < 5; j++)
                if ((dynamic_cast <Enemy*> (enemies[i][j]))->isAlive())
                    window.draw(*enemies[i][j]);

        // Dibujar UFO especial si está activo
        if (ufoEspecial->isAlive())
            window.draw(*ufoEspecial);

        // Poderes que caen y marcador de patrón / ataques / poderes activos
        for (size_t k = 0; k < poderes.cayendo.size(); k++)
            window.draw(poderes.cayendo[k].sprite);

        infoT.setString("Patron: " + patron.nombre() + "  Ataques: " + to_string(SistemaAtaques::cantidadTipos(level)) + "   " + poderes.texto());
        window.draw(infoT);

        dibujarExplosiones(window, explosiones);

        window.display();

        //If all enemies are dead, go to next level
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 5; j++) {
                if ((dynamic_cast <Enemy*> (enemies[i][j]))->isAlive()) {
                    next_level = false;
                    goto escape4;
                }
            }
        }

    escape4:
        if (next_level || lives == 0) {
            reset_check = true;
            explosiones.clear();
            scoreEnd.setString("Score: " + to_string(score));
            levelEnd.setString("Level: " + to_string(level));

            //Displays statistics until user presses enter
            while (true) {
                window.clear();
                window.draw(spriteFondo);
                window.draw(levelEnd);
                window.draw(scoreEnd);
                window.draw(endMessage);
                window.display();
                if (Keyboard::isKeyPressed(Keyboard::Enter))
                    goto escape5;
            }

        escape5:
            ufoEspecial->kill();
            poderes.cayendo.clear();
            if (lives == 0) {
                indicator = true; //Displays menu again
                level = 1;
                ufoSpawned = false;
                poderes.reiniciar();
            }
            else
                level++;

            //Empties all missiles for next round
            while (!missiles.empty())
                missiles.pop_back();

            while (!enemyM.empty())
                enemyM.pop_back();

            //Resets shield cycle
            shieldS1.setTexture(shieldT1.at(0));
            shieldS2.setTexture(shieldT2.at(0));
            shieldS3.setTexture(shieldT3.at(0));
            tChange1 = 1;
            tChange2 = 1;
            tChange3 = 1;
        }

        next_level = true;

    test_cases:
        if (m.getUserChoice() == 2) {
            //Test case 1
            int testMissiles = 0, removeTM = 0, test = 1;
            testCase.setString("Test " + to_string(test) + ": Missile Deletion");
            test++;
            RectangleShape line(Vector2f(950, 5));
            line.setPosition(Vector2f(0, 200));

            while (true) {
                window.draw(line);
                window.draw(testCase);

                if (c2.getElapsedTime().asMilliseconds() > 1075 && testMissiles < 5) {
                    missile = new Missile(Vector2f(rand() % 901, 951), t3, Vector2f(0.3f, 0.3f));
                    missiles.push_back(missile);
                    missile_sound.play();
                    testMissiles++;
                    c2.restart();
                }

                if (!missiles.empty() && missiles.front()->getPosition().y < 155) {
                    missiles.erase(missiles.begin());
                    removeTM++;
                }

                if (removeTM >= 5)
                    break;

                draw_missiles(missiles, enemyM, window);
                missile_movement(missiles, enemyM);

                window.display();
                window.clear();
            }

            //Test case 2
            testCase.setString("Test " + to_string(test) + ": Ship Loses Life");
            test++;
            lives = 5;
            enemyMi = new Missile(Vector2f(ship->getPosition().x + 50, ship->getPosition().y - 800), t4, Vector2f(0.3f, 0.3f));
            enemyM.push_back(enemyMi);
            livesT.setPosition(Vector2f(425.f, 50.f));

            while (true) {
                window.draw(*ship);

                window.draw(testCase);
                window.draw(livesT);
                livesT.setString("Lives: " + to_string(lives));

                if (!enemyM.empty() && enemyM.at(0)->getPosition().y == ship->getPosition().y) {
                    enemyM.erase(enemyM.begin());
                    lives--;
                    ship_hit.play();
                    livesT.setString("Lives: " + to_string(lives));
                    window.clear();
                    window.draw(livesT);
                    window.draw(*ship);
                    window.draw(testCase);
                    window.display();
                    delay(1000);
                    break;
                }

                draw_missiles(missiles, enemyM, window);
                missile_movement(missiles, enemyM);

                window.display();
                window.clear();
            }
            livesT.setPosition(Vector2f(425.f, 0.f));

            //Test case 3
            testMissiles = 0, removeTM = 0;
            testCase.setString("Test " + to_string(test) + ": Shield Loses Lives");
            test++;

            while (true) {
                window.draw(shieldS2);
                window.draw(testCase);

                if (c2.getElapsedTime().asMilliseconds() > 1075 && testMissiles <= 2) {
                    enemyMi = new Missile(Vector2f(shieldS2.getPosition().x + 50, shieldS2.getPosition().y - 800), t4, Vector2f(0.3f, 0.3f));
                    enemyM.push_back(enemyMi);
                    testMissiles++;
                    c2.restart();
                }

                if (!enemyM.empty() && enemyM.at(0)->getPosition().y > shieldS2.getPosition().y && tChange2 <= 3) {
                    enemyM.erase(enemyM.begin());
                    shieldS2.setTexture(shieldT2.at(tChange2));
                    tChange2++;
                    removeTM++;
                }

                if (removeTM >= 3) {
                    window.clear();
                    window.draw(shieldS2);
                    window.draw(testCase);
                    window.display();
                    delay(1000);
                    break;
                }

                draw_missiles(missiles, enemyM, window);
                missile_movement(missiles, enemyM);

                window.display();
                window.clear();
            }
            shieldS2.setTexture(shieldT2.at(0));
            tChange2 = 1;

            //Test case 4
            testCase.setString("Test " + to_string(test) + ": Enemies move");
            test++;
            dir = true;

            while (true) {
                window.draw(testCase);

                for (int i = 0; i < 10; i++)
                    for (int j = 0; j < 5; j++)
                        if ((dynamic_cast <Enemy*> (enemies[i][j]))->isAlive())
                            window.draw(*enemies[i][j]);

                if (reset_check)
                    goto test_escape1;

                for (int i = 0; i < 10; i++)
                {
                    for (int j = 0; j < 5; j++)
                    {
                        if (dynamic_cast <Enemy*> (enemies[i][j])->isAlive() == true)
                        {
                            reset_check = false;
                            goto test_escape1;
                        }
                        else
                            reset_check = true;
                    }
                }

            test_escape1:
                if (reset_check)
                {
                    for (int i = 0; i < 10; i++)
                        for (int j = 0; j < 5; j++)
                            enemies[i][j]->setPosition(Vector2f(175.f + 60 * i, 125.f + 50 * j));

                    for (int i = 0; i < 10; i++)
                        for (int j = 0; j < 5; j++)
                            dynamic_cast <Enemy*> (enemies[i][j])->reset();

                    reset_check = false;
                }

                for (int i = 0; i < 10; i++)
                    for (int j = 0; j < 5; j++)
                        if ((dynamic_cast <Enemy*> (enemies[i][j]))->isAlive() && (enemies[i][j]->getPosition().x > 915)) {
                            dir = !dir;
                            for (int i = 0; i < 10; i++)
                                for (int j = 0; j < 5; j++)
                                    enemies[i][j]->move(Vector2f(0.f, (float)(9 + level)));

                            goto test_escape2;
                        }

            test_escape2:
                for (int i = 0; i < 10; i++)
                    for (int j = 0; j < 5; j++) {
                        if (dir)
                            enemies[i][j]->move(Vector2f((float)(0.1 + (level * 0.02)), 0.f));
                        else
                            enemies[i][j]->move(Vector2f((float)(-0.1 - (level * 0.02)), 0.f));
                    }

                for (int i = 0; i < 10; i++)
                    for (int j = 0; j < 5; j++)
                        if (enemies[i][j]->getPosition().x < 35)
                            goto test_escape3;

                window.display();
                window.clear();
            }

        test_escape3:
            reset_check = true;
            dir = true;

            //Test case 5
            testMissiles = 0, removeTM = 0;
            testCase.setString("Test " + to_string(test) + ": Enemies Shoot Randomly");

            while (true) {
                window.draw(testCase);

                for (int i = 0; i < 10; i++)
                    for (int j = 0; j < 5; j++)
                        if ((dynamic_cast <Enemy*> (enemies[i][j]))->isAlive())
                            window.draw(*enemies[i][j]);

                if (reset_check)
                    goto test_escape4;

                for (int i = 0; i < 10; i++)
                {
                    for (int j = 0; j < 5; j++)
                    {
                        if (dynamic_cast <Enemy*> (enemies[i][j])->isAlive() == true)
                        {
                            reset_check = false;
                            goto test_escape4;
                        }
                        else
                            reset_check = true;
                    }
                }

            test_escape4:
                if (reset_check)
                {
                    for (int i = 0; i < 10; i++)
                        for (int j = 0; j < 5; j++)
                            enemies[i][j]->setPosition(Vector2f(175.f + 60 * i, 125.f + 50 * j));

                    for (int i = 0; i < 10; i++)
                        for (int j = 0; j < 5; j++)
                            dynamic_cast <Enemy*> (enemies[i][j])->reset();

                    reset_check = false;
                }

                if (c2.getElapsedTime().asMilliseconds() > 1000 && testMissiles < 5) {
                    int x_coor = rand() % 10, y_coor = rand() % 5;
                    while (!(dynamic_cast <Enemy*> (enemies[x_coor][y_coor]))->isAlive())
                        x_coor = rand() % 10, y_coor = rand() % 5;
                    enemyMi = new Missile(Vector2f(enemies[x_coor][y_coor]->getPosition().x + 7, enemies[x_coor][y_coor]->getPosition().y), t4, Vector2f(0.3f, 0.3f));
                    enemyM.push_back(enemyMi);
                    c2.restart();
                    testMissiles++;
                }

                if (!enemyM.empty() && enemyM.front()->getPosition().y > 1050) {
                    enemyM.erase(enemyM.begin());
                    removeTM++;
                }

                if (removeTM >= 5)
                    break;

                draw_missiles(missiles, enemyM, window);
                missile_movement(missiles, enemyM);

                window.display();
                window.clear();
            }

            m.setUserChoice(0);
            no_test = true;
        }
    }

    return 0;
}

void delay(int x) {
    clock_t time = clock();
    while (clock() < time + x)
        ;
}

void ship_movement(Character& ship, RenderWindow& window) {
    if (ship.getPosition().x + 105 != window.getSize().x && (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D)))
        ship.move(0.5, 0);

    if (ship.getPosition().x != 0 && (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A)))
        ship.move(-0.5, 0);
}

void missile_movement(vector<Missile*>& missiles, vector<Missile*>& enemyM)
{
    for (size_t i = 0; i < missiles.size(); i++) {
        if (missiles.at(i)->tieneVelocidad())
            missiles.at(i)->move(missiles.at(i)->velocidad); //Misil en diagonal
        else
            missiles.at(i)->move(Vector2f(0.f, -0.5f));
    }

    for (size_t i = 0; i < enemyM.size(); i++) {
        if (enemyM.at(i)->tieneVelocidad())
            enemyM.at(i)->move(enemyM.at(i)->velocidad); //Misil en diagonal / apuntado
        else
            enemyM.at(i)->move(Vector2f(0.f, 0.5f));
    }
}

void draw_missiles(vector<Missile*>& missiles, vector<Missile*>& enemyM, RenderWindow& window)
{
    if (!missiles.empty()) {
        for (int i = 0; i < missiles.size(); i++)
            window.draw(*missiles.at(i));
    }

    if (!enemyM.empty()) {
        for (int i = 0; i < enemyM.size(); i++)
            window.draw(*enemyM.at(i));
    }
}
