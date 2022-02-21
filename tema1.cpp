#include "lab_m1/tema1/tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/tema1/transform2D.h"
#include "lab_m1/tema1/object2D.h"
#include <math.h>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */

Tema1::Tema1() {
    // Setting up the Time
    srand(time(NULL));
    enemySpawnTime = Engine::GetElapsedTime();
    bulletLaunchingTime = 0.0f;
    interval = 0.5f;

    // Default Square Side
    squareSide = 1.0f;

    // Initialize the Number of Hits and the Score
    hits = 0;
    score = 0;

    // Initialize Logic Space Coordinates and Dimensions
    logicSpace.x = logicSpace.y = 0.0f;
    logicSpace.width = logicSpace.height = 2.0f;

    // Initialize Player Position -> Center of the Window
    player.x = logicSpace.x + logicSpace.width / 2.0f;
    player.y = logicSpace.y + logicSpace.height / 2.0f;

    // Initialize Player Radius
    player.radius = 0.154f / 2.0f;

    // Initialize Map Coordinates and Dimensions
    map.height = map.width = 6.0f;
    map.x = logicSpace.width / 2 - squareSide * map.width / 2.0f;
    map.y = logicSpace.height / 2 - squareSide * map.height / 2.0f;

    // Map Margins
    upperMargin = map.height / 2.0f - 0.195f;
    lowerMargin = -map.height / 2.0f + 0.195f;
    leftMargin = -map.width / 2.0f + 0.195f;
    rightMargin = map.width / 2.0f - 0.195f;

    // Initialize Health Bar Coordinates and Dimensions
    healthBar.x = logicSpace.x + logicSpace.width * 4.85 / 6.0f;
    healthBar.y = logicSpace.y + logicSpace.height * 5.0f / 6.0f;
    healthBar.height = 0.25f;
    healthBar.width = 1.2f;
}


Tema1::~Tema1() {
}


void Tema1::Init() {
    // Generate Obstacles
    CreateObstacles();

    // Generate Enemies
    for (int i = 0; i < 10; ++i) {
        CreateEnemy();
    }

    // Get Window Resolution
    glm::ivec2 resolution = window->GetResolution();

    // Set up the Camera
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float) resolution.x, 0, (float) resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    // Bottom Left Corner
    glm::vec3 corner = glm::vec3(0, 0, 0);

    // Start Score
    printf("Score: %u.\n", score);

    // Create meshes from data

    // Cream Circle -> Player
    Mesh *creamCircle = object2D::CreateCircle("creamCircle", glm::vec3(0, 0, 0), 0.2f,
                                               glm::vec3(0.86f, 0.64f, 0.3f));
    AddMeshToList(creamCircle);

    // Black Circle -> Player
    Mesh *blackCircle = object2D::CreateCircle("blackCircle", glm::vec3(0, 0, 0), 0.22f, glm::vec3(0, 0, 0));
    AddMeshToList(blackCircle);

    // Grey Square -> Map
    Mesh *greySquare = object2D::CreateSquare("greySquare", corner, squareSide, glm::vec3(0.75f, 0.75f, 0.75f), true);
    AddMeshToList(greySquare);

    // Green Square -> Obstacles
    Mesh *greenSquare = object2D::CreateSquare("greenSquare", corner, squareSide, glm::vec3(0.13f, 0.67f, 0.03f),
                                               true);
    AddMeshToList(greenSquare);

    // Magenta Square -> Enemies
    Mesh *magentaSquare = object2D::CreateSquare("magentaSquare", corner, 0.15f, glm::vec3(0.6f, 0.0f, 0.04f), true);
    AddMeshToList(magentaSquare);

    // Red Square -> Health Bar
    Mesh *redSquare = object2D::CreateSquare("redSquare", corner, squareSide, glm::vec3(0.8f, 0.0f, 0.0f), true);
    AddMeshToList(redSquare);

    // Contour of a Red Square -> Health Bar
    redSquare = object2D::CreateSquare("contourRedSquare", corner, squareSide, glm::vec3(0.8f, 0.0f, 0.0f), false);
    AddMeshToList(redSquare);

    // Black Square -> Bullets and Enemies
    Mesh *blackSquare = object2D::CreateSquare("blackSquare", corner, 0.1f, glm::vec3(0.0f, 0.0f, 0.0f), true);
    AddMeshToList(blackSquare);
}

// 2D visualization matrix
glm::mat3 Tema1::VisualizationTransf2D(const LogicSpace &logicSpace, const ViewportSpace &viewSpace) {
    float sx, sy, tx, ty;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    tx = viewSpace.x - sx * logicSpace.x;
    ty = viewSpace.y - sy * logicSpace.y;

    return glm::transpose(glm::mat3(
            sx, 0.0f, tx,
            0.0f, sy, ty,
            0.0f, 0.0f, 1.0f));
}

// Uniform 2D visualization matrix (same scale factor on x and y axes)
glm::mat3 Tema1::VisualizationTransf2DUnif(const LogicSpace &logicSpace, const ViewportSpace &viewSpace) {
    float sx, sy, tx, ty, smin;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;

    if (sx < sy) {
        smin = sx;
    } else {
        smin = sy;
    }

    tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
    ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

    return glm::transpose(glm::mat3(
            smin, 0.0f, tx,
            0.0f, smin, ty,
            0.0f, 0.0f, 1.0f));
}


void Tema1::SetViewportArea(const ViewportSpace &viewSpace, glm::vec3 colorColor, bool clear) {
    glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    glEnable(GL_SCISSOR_TEST);
    glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    GetSceneCamera()->SetOrthographic((float) viewSpace.x, (float) (viewSpace.x + viewSpace.width), (float) viewSpace.y,
                                      (float) (viewSpace.y + viewSpace.height), 0.1f, 400);
    GetSceneCamera()->Update();
}


void Tema1::FrameStart() {
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema1::Update(float deltaTimeSeconds) {
    // Get Window Resolution
    glm::ivec2 resolution = window->GetResolution();

    // Sets the screen area where to draw - the whole window
    viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
    SetViewportArea(viewSpace, glm::vec3(0, 0, 0), true);

    // Compute the 2D visualization matrix
    visMatrix = glm::mat3(1);
    visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);

    // Update Player Position -> Center of the Window
    player.x = logicSpace.x + logicSpace.width / 2;
    player.y = logicSpace.y + logicSpace.height / 2;

    // Update Health Bar Coordinates
    healthBar.x = logicSpace.x + logicSpace.width * 4.85 / 6;
    healthBar.y = logicSpace.y + logicSpace.height * 5 / 6;

    // Check if a new Enemy cand be generated
    GenerateEnemy();

    // Draw the Player
    DrawPlayer(visMatrix);

    // Draw the Heath Bar
    DrawHealthBar(visMatrix);

    // Draw the Bullets
    DrawBullets(visMatrix);

    glm::vec2 aabb_half_extents, aabb_center, difference, clamped, closest;

    // For each enemy
    for (int i = 0; i < enemies.size(); i++) {
        // Update its coordinates in order to make it follow the Player (use the angle between them)
        enemies[i].x += cos(enemies[i].angle) * enemies[i].speed;
        enemies[i].y += sin(enemies[i].angle) * enemies[i].speed;
        enemies[i].angle = atan2(player.y - enemies[i].y, player.x - enemies[i].x);

        // Enemy (Square) - Player (Circle) Collision Detection -> AABB - AABB collision

        // Player's Center
        glm::vec2 center(player.x, player.y);

        // AABB collision info (half-extents and center)
        aabb_half_extents = glm::vec2(enemies[i].width / 2.0f, enemies[i].height / 2.0f);
        aabb_center = glm::vec2(enemies[i].x + aabb_half_extents.x, enemies[i].y + aabb_half_extents.y);

        // Difference vector between both centers
        difference = center - aabb_center;
        clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);

        // Add clamped value to AABB_center to get the coordinates of the Enemy closest to the Player
        closest = aabb_center + clamped;

        // Retrieve the vector between the Player's center and the closest point AABB and check if length <= Player's Radius
        difference = closest - center;

        // If a collisison is detected then an Enemy hit the Player
        if (glm::length(difference) < player.radius) {
            // Delete the Enemy
            enemies.erase(enemies.begin() + i);

            // Increase the number of hits
            hits++;

            // Update the Health Bar
            DrawHealthBar(visMatrix);

            // If the Player has been hit 3 times then the Game is Over
            if (hits == 3) {
                printf("GAME OVER : %u.\n", score);
                exit(0);
            }
            continue;
        } else {
            // No collision detected: Draw the Enemy
            DrawEnemy(enemies[i], visMatrix);
        }

        /* Bullet(Square) - Enemy(Square) Collision Detection -> AABB - AABB collision
           
           AABB - AABB collision uses the Top Left and the Bottom Right Corners to detect collisions, so I convert from the Bottom Left 
           Corner definiton for squares to those ones when detecting collisions.
        */
        for (int j = 0; j < bullets.size(); j++) {
            // Check Collision on X-axis
            bool collisionX =
                    bullets[j].x + bullets[j].width >= enemies[i].x && enemies[i].x + enemies[i].width >= bullets[j].x;

            // Check Collision on Y-axis
            bool collisionY = bullets[j].y + 2 * bullets[j].height >= enemies[i].y &&
                              enemies[i].y + 2 * enemies[i].height >= bullets[j].y;

            // Collision only if they collide on both axes
            if (collisionX && collisionY) {
                // Delete the Bullet and the Enemy
                bullets.erase(bullets.begin() + j);
                enemies.erase(enemies.begin() + i);

                // Update the Score and display it
                score += 100;
                printf("Score: %u.\n", score);
                break;
            }
        }

    }

    // Draw the Map
    DrawMap(visMatrix);

    float displacement, distance, tolerance = 0.02f;

    /* Check collisons between Bulletsand Map Walls / Object
       
       The Tolerance and the Displacement have been computed by Try and Error considering that the bullet is oriented at some angle 
    */
    for (int i = 0; i < bullets.size(); i++) {
        // Collision with the Map's Upper Margin (x coordinate doesn't matter)
        displacement = cos(bullets[i].angle) < 0 ? bullets[i].height - tolerance : bullets[i].height + tolerance;

        // Distance between the Bullet and the Map's Upper Margin
        distance = abs(bullets[i].y - (map.y + map.height - displacement));
        if (distance <= displacement) {
            bullets.erase(bullets.begin() + i);
            continue;
        }

        // Collision with the Map's Lower Margin (x coordinate doesn't matter)
        displacement = cos(bullets[i].angle) > 0 ? bullets[i].height - tolerance : bullets[i].height + tolerance;

        // Distance between the Bullet and the Map's Lower Margin
        distance = abs(bullets[i].y - (map.y + displacement));
        if (distance <= displacement) {
            bullets.erase(bullets.begin() + i);
            continue;
        }

        // Collision with the Map's Left Margin (y coordinate doesn't matter)
        displacement = sin(bullets[i].angle) < 0 ? bullets[i].width - tolerance : bullets[i].width + tolerance;

        // Distance between the Bullet and the Map's Left Margin
        distance = abs(bullets[i].x - (map.x + displacement));
        if (distance <= displacement) {
            bullets.erase(bullets.begin() + i);
            continue;
        }

        // Collision with the Map's Right Margin (y coordinate doesn't matter)
        displacement = sin(bullets[i].angle) > 0 ? bullets[i].width - tolerance : bullets[i].width + tolerance;

        // Distance between the Bullet and the Map's Right Margin
        distance = abs(bullets[i].x - (map.x + map.width - displacement));
        if (distance <= displacement) {
            bullets.erase(bullets.begin() + i);
            continue;
        }

        tolerance = 0.1f;

        // Bullet - Object Collisions
        for (int j = 0; j < obstacles.size(); j++) {
            // Check if the Bullet collides with any Obstacle
            if (bullets[i].x >= obstacles[j]->x - tolerance &&
                bullets[i].x <= obstacles[j]->x + obstacles[j]->width + tolerance &&
                bullets[i].y >= obstacles[j]->y - tolerance &&
                bullets[i].y <= obstacles[j]->y + obstacles[j]->height + tolerance) {
                bullets.erase(bullets.begin() + i);
                break;
            }
        }
    }

}

void Tema1::FrameEnd() {
}

void Tema1::DrawPlayer(glm::mat3 visMatrix) {
    // Get Window Resolution
    glm::ivec2 resolution = window->GetResolution();

    // Angle between Player and The Mouse
    float angle = atan2(mouse_Y - resolution.y / 2, mouse_X - resolution.x / 2);

    // Draw the Player's Left Eye
    modelMatrix = visMatrix * transform2D::Translate(player.x, player.y);
    modelMatrix *= transform2D::Rotate(M_PI - angle);
    modelMatrix *= transform2D::Translate((-logicSpace.width) / 20, (-logicSpace.height) / 20);
    modelMatrix *= transform2D::Scale(0.2f, 0.2f);
    RenderMesh2D(meshes["creamCircle"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * transform2D::Translate(player.x, player.y);
    modelMatrix *= transform2D::Rotate(M_PI - angle);
    modelMatrix *= transform2D::Translate((-logicSpace.width) / 20, (-logicSpace.height) / 20);
    modelMatrix *= transform2D::Scale(0.23f, 0.23f);
    RenderMesh2D(meshes["blackCircle"], shaders["VertexColor"], modelMatrix);

    // Draw the Player's Right Eye
    modelMatrix = visMatrix * transform2D::Translate(player.x, player.y);
    modelMatrix *= transform2D::Rotate(M_PI - angle);
    modelMatrix *= transform2D::Translate((-logicSpace.width) / 20, (logicSpace.height) / 20);
    modelMatrix *= transform2D::Scale(0.2f, 0.2f);
    RenderMesh2D(meshes["creamCircle"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * transform2D::Translate(player.x, player.y);
    modelMatrix *= transform2D::Rotate(M_PI - angle);
    modelMatrix *= transform2D::Translate((-logicSpace.width) / 20, (logicSpace.height) / 20);
    modelMatrix *= transform2D::Scale(0.23f, 0.23f);
    RenderMesh2D(meshes["blackCircle"], shaders["VertexColor"], modelMatrix);

    // Draw the Player's Body
    modelMatrix = visMatrix * transform2D::Translate(player.x, player.y);
    modelMatrix *= transform2D::Scale(0.7f, 0.7f);
    RenderMesh2D(meshes["creamCircle"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * transform2D::Translate(player.x, player.y);
    modelMatrix *= transform2D::Scale(0.7f, 0.7f);
    RenderMesh2D(meshes["blackCircle"], shaders["VertexColor"], modelMatrix);
}


void Tema1::CreateObstacles() {
    // Dimensions for the Obstacles
    float heights[] = {1.5f, 0.25f, 0.6f, 0.9f, 0.7f, 0.9f};
    float widths[] = {0.25f, 1.2f, 0.6f, 1.3f, 0.7f, 0.9f};

    // Coordinates for the Obstacles
    float xs[] = {logicSpace.width / 2.0f - squareSide * 4.0f / 2.0f,
                  logicSpace.width / 2.0f - squareSide * 4.0f / 2.0f,
                  logicSpace.width / 2.0f - squareSide * 3.2f / 2.0f, logicSpace.width / 2.0f - squareSide / 2.0f,
                  logicSpace.width / 2.0f + squareSide * 2.5 / 2.0f,
                  logicSpace.width / 2.0f + squareSide * 1.9f / 2.0f};
    float ys[] = {logicSpace.height / 2.0f - squareSide * 4.0f / 2.0f,
                  logicSpace.height / 2.0f - squareSide * 4.0f / 2.0f,
                  logicSpace.height + squareSide / 2.0f, logicSpace.height, logicSpace.height - squareSide * 2 / 2.0f,
                  logicSpace.height - squareSide * 6.2f / 2.0f};

    Obstacle *obstacle;

    // Create each Obstacle and add it to the Obstacle Vector
    for (int i = 0; i < 6; i++) {
        obstacle = new Obstacle();
        obstacle->width = widths[i];
        obstacle->height = heights[i];
        obstacle->x = xs[i];
        obstacle->y = ys[i];
        obstacles.push_back(obstacle);
    }
}

void Tema1::DrawMap(glm::mat3 visMatrix) {
    // Draw the Obstacles
    for (int i = 0; i < obstacles.size(); i++) {
        modelMatrix = visMatrix * transform2D::Translate(obstacles[i]->x, obstacles[i]->y);
        modelMatrix *= transform2D::Scale(obstacles[i]->width, obstacles[i]->height);
        RenderMesh2D(meshes["greenSquare"], shaders["VertexColor"], modelMatrix);
    }

    // Draw the Map
    modelMatrix = visMatrix * transform2D::Translate(map.x, map.y);
    modelMatrix *= transform2D::Scale(map.width, map.height);
    RenderMesh2D(meshes["greySquare"], shaders["VertexColor"], modelMatrix);
}

void Tema1::DrawHealthBar(glm::mat3 visMatrix) {
    // Draw the Contour for the Health Bar 
    modelMatrix = visMatrix * transform2D::Translate(healthBar.x, healthBar.y);
    modelMatrix *= transform2D::Scale(healthBar.width, healthBar.height);
    RenderMesh2D(meshes["contourRedSquare"], shaders["VertexColor"], modelMatrix);

    // Compute the Length for the Actual Health Bar
    float displacement = (hits) * healthBar.width / 3 == healthBar.width ? 0 : (hits) * healthBar.width / 3;

    // Draw the Health Bar according to the current number of Lives the Player has left
    modelMatrix = visMatrix * transform2D::Translate(healthBar.x + displacement, healthBar.y);
    modelMatrix *= transform2D::Scale((3 - hits) * healthBar.width / 3, healthBar.height);
    RenderMesh2D(meshes["redSquare"], shaders["VertexColor"], modelMatrix);
}

void Tema1::DrawBullets(glm::mat3 visMatrix) {
    // For each Bullet
    for (int i = 0; i < bullets.size(); i++) {
        // Compute the Bullets's new coordinates according to it's angle and speed
        bullets[i].y += sin(bullets[i].angle) * M_PI / 180 * bullets[i].speed;
        bullets[i].x += cos(bullets[i].angle) * M_PI / 180 * bullets[i].speed;

        // Draw the Bullet
        modelMatrix = visMatrix * transform2D::Translate(bullets[i].x, bullets[i].y);
        modelMatrix *= transform2D::Rotate(bullets[i].angle);
        RenderMesh2D(meshes["blackSquare"], shaders["VertexColor"], modelMatrix);
    }
}

void Tema1::DrawEnemy(Enemy enemy, glm::mat3 visMatrix) {
    // Draw the Enemy's Left Eye
    modelMatrix = visMatrix * transform2D::Translate(enemy.x, enemy.y);
    modelMatrix *= transform2D::Rotate(enemy.angle);
    modelMatrix *= transform2D::Translate(0.14f + 0.025f / 2, 0.14f + 0.025f / 2);
    modelMatrix *= transform2D::Scale(0.5f, 0.5f);
    RenderMesh2D(meshes["magentaSquare"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * transform2D::Translate(enemy.x, enemy.y);
    modelMatrix *= transform2D::Rotate((float) enemy.angle);
    modelMatrix *= transform2D::Translate(0.14f, 0.14f);
    RenderMesh2D(meshes["blackSquare"], shaders["VertexColor"], modelMatrix);

    // Draw the Enemy's Right Eye
    modelMatrix = visMatrix * transform2D::Translate(enemy.x, enemy.y);
    modelMatrix *= transform2D::Rotate(enemy.angle);
    modelMatrix *= transform2D::Translate(0.14f + 0.025f / 2, -0.06f + 0.025f / 2);
    modelMatrix *= transform2D::Scale(0.5f, 0.5f);
    RenderMesh2D(meshes["magentaSquare"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * transform2D::Translate(enemy.x, enemy.y);
    modelMatrix *= transform2D::Rotate(enemy.angle);
    modelMatrix *= transform2D::Translate(0.14f, -0.06f);
    RenderMesh2D(meshes["blackSquare"], shaders["VertexColor"], modelMatrix);

    // Draw the Enemy's Body
    modelMatrix = visMatrix * transform2D::Translate(enemy.x, enemy.y);
    modelMatrix *= transform2D::Rotate(enemy.angle);
    modelMatrix *= transform2D::Translate(0.015f, 0.015f);
    RenderMesh2D(meshes["magentaSquare"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * transform2D::Translate(enemy.x, enemy.y);
    modelMatrix *= transform2D::Rotate(enemy.angle);
    modelMatrix *= transform2D::Scale(1.8f, 1.8f);
    RenderMesh2D(meshes["blackSquare"], shaders["VertexColor"], modelMatrix);
}

void Tema1::GenerateEnemy() {
    // Create an Enemy every time interval seconds have passed and update the Last Enemy Spawn Time 
    if (Engine::GetElapsedTime() - enemySpawnTime >= interval) {
        CreateEnemy();
        enemySpawnTime = Engine::GetElapsedTime();
    }
}

void Tema1::CreateEnemy() {
    Enemy enemy;

    // Random coordinates on the Map for The Enemy
    float angle = rand() % 360;
    enemy.x = sin(angle) * logicSpace.width + logicSpace.width / 2;
    enemy.y = cos(angle) * logicSpace.width + logicSpace.height / 2;

    // Check if the coordinates are outside of the View Area of the Window
    if ((enemy.x <= player.x - logicSpace.width / 2 || enemy.x >= player.x + logicSpace.width) &&
        (enemy.y <= player.y - logicSpace.height / 2 || enemy.y >= player.y + logicSpace.height)) {

        // Update the Enemy's info and add it to the Enemy Vector
        enemy.speed = 0.02f;
        enemy.width = 0.18f;
        enemy.height = 0.18f;
        enemy.angle = 0;
        enemies.push_back(enemy);
    }
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema1::OnInputUpdate(float deltaTime, int mods) {
    // The Player Wants to move Forward
    if (window->KeyHold(GLFW_KEY_W)) {
        // Possible Logic Space and Player Coordinates
        float newLogicSpaceY = logicSpace.y + 0.5 * deltaTime;
        float newPlayerY = newLogicSpaceY + logicSpace.height / 2;
        bool collision = false;

        // For each Obstacle
        for (int i = 0; i < obstacles.size(); i++) {
            // Check if the Player would collide with the Obstacle
            if (player.x >= obstacles[i]->x - player.radius * 2 &&
                player.x <= obstacles[i]->x + obstacles[i]->width + player.radius * 2 &&
                newPlayerY >= obstacles[i]->y - player.radius * 2 &&
                newPlayerY <= obstacles[i]->y + obstacles[i]->height + player.radius * 2) {
                collision = true;
                break;
            }
        }

        // If the Player won't collide with any object
        if (!collision) {
            // Check if the Player collides with the Map's Upper Margin and Update the Logic Space Y
            logicSpace.y = (newLogicSpaceY < upperMargin) ? newLogicSpaceY : logicSpace.y;
        }
    }

    // The Player Wants to move Backward
    if (window->KeyHold(GLFW_KEY_S)) {
        // Possible Logic Space and Player Coordinates
        float newLogicSpaceY = logicSpace.y - 0.5 * deltaTime;
        float newPlayerY = newLogicSpaceY + logicSpace.height / 2;
        bool collision = false;

        // For each Obstacle
        for (int i = 0; i < obstacles.size(); i++) {
            // Check if the Player would collide with the Obstacle
            if (player.x >= obstacles[i]->x - player.radius * 2 &&
                player.x <= obstacles[i]->x + obstacles[i]->width + player.radius * 2 &&
                newPlayerY >= obstacles[i]->y - player.radius * 2 &&
                newPlayerY <= obstacles[i]->y + obstacles[i]->height + player.radius * 2) {
                collision = true;
                break;
            }
        }

        // If the Player won't collide with any object
        if (!collision) {
            // Check if the Player collides with the Map's Lower Margin and Update the Logic Space Y
            logicSpace.y = (logicSpace.y - 0.5 * deltaTime > lowerMargin) ? newLogicSpaceY : logicSpace.y;
        }
    }

    // The Player Wants to move to the Left
    if (window->KeyHold(GLFW_KEY_A)) {
        // Possible Logic Space and Player Coordinates
        float newLogicSpaceX = logicSpace.x - 0.5 * deltaTime;
        float newPlayerX = newLogicSpaceX + logicSpace.width / 2;
        bool collision = false;

        // Check if the Player would collide with the Obstacle
        for (int j = 0; j < obstacles.size(); ++j) {
            if (newPlayerX >= obstacles[j]->x - player.radius * 2 &&
                newPlayerX <= obstacles[j]->x + obstacles[j]->width + player.radius * 2 &&
                player.y >= obstacles[j]->y - player.radius * 2 &&
                player.y <= obstacles[j]->y + obstacles[j]->height + player.radius * 2) {
                collision = true;
                break;
            }
        }

        // If the Player won't collide with any object
        if (!collision) {
            // Check if the Player collides with the Map's Left Margin and Update the Logic Space X
            logicSpace.x = (logicSpace.x - 0.5 * deltaTime > leftMargin) ? logicSpace.x - 0.5 * deltaTime
                                                                         : logicSpace.x;
        }
    }

    if (window->KeyHold(GLFW_KEY_D)) {
        // Possible Logic Space and Player Coordinates
        float newLogicSpaceX = logicSpace.x + 0.5 * deltaTime;
        float newPlayerX = newLogicSpaceX + logicSpace.width / 2;
        bool collision = false;

        // For each Obstacle
        for (int i = 0; i < obstacles.size(); i++) {
            // Check if the Player would collide with the Obstacle
            if (newPlayerX >= obstacles[i]->x - player.radius * 2 &&
                newPlayerX <= obstacles[i]->x + obstacles[i]->width + player.radius * 2 &&
                player.y >= obstacles[i]->y - player.radius * 2 &&
                player.y <= obstacles[i]->y + obstacles[i]->height + player.radius * 2) {
                collision = true;
                break;
            }
        }

        // If the Player won't collide with any object
        if (!collision) {
            // Check if the Player collides with the Map's Right Margin and Update the Logic Space X
            logicSpace.x = (logicSpace.x + 0.5 * deltaTime < rightMargin) ? logicSpace.x + 0.5 * deltaTime
                                                                          : logicSpace.x;
        }
    }
}


void Tema1::OnKeyPress(int key, int mods) {
    // Add key press event
}


void Tema1::OnKeyRelease(int key, int mods) {
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
    mouse_X = mouseX;
    mouse_Y = mouseY;

}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {
    // Check if the Player wants to Lauch a Bullet and the minimum Time between two Bullet launches has paased
    if (window->MouseHold(GLFW_MOUSE_BUTTON_LEFT) && Engine::GetElapsedTime() - bulletLaunchingTime >= interval) {
        Bullet bullet;

        // Get Window Resolution
        glm::ivec2 resolution = window->GetResolution();

        // Compute where the mouse is positioned and save the sign
        int sign_X = mouse_X - resolution.x / 2 <= 0 ? 1 : -1;

        // The Bullet's info
        bullet.angle = 2 * M_PI - atan2(mouse_Y - resolution.y / 2, mouse_X - resolution.x / 2);
        bullet.speed = 2.0f;
        bullet.x = player.x + sin((float) (bullet.angle)) * squareSide / 20.0f;
        bullet.y = player.y + sign_X * squareSide / 20.0f;
        bullet.width = squareSide / 20.0f;
        bullet.height = squareSide / 20.0f;

        // Add the Bullet to the Bullet Vector and Update the Last Time a Bullet has been launched
        bullets.push_back(bullet);
        bulletLaunchingTime = Engine::GetElapsedTime();
    }
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {
    // Add mouse button release event
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {
}


void Tema1::OnWindowResize(int width, int height) {
}
