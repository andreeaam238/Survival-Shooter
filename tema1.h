#pragma once

#include "components/simple_scene.h"


namespace m1 {
    class Tema1 : public gfxc::SimpleScene {
    public:
        struct ViewportSpace {
            ViewportSpace() : x(0), y(0), width(1), height(1) {}

            ViewportSpace(int x, int y, int width, int height)
                    : x(x), y(y), width(width), height(height) {}

            int x;
            int y;
            int width;
            int height;
        };

        struct LogicSpace {
            LogicSpace() : x(0), y(0), width(1), height(1) {}

            LogicSpace(float x, float y, float width, float height)
                    : x(x), y(y), width(width), height(height) {}

            float x;
            float y;
            float width;
            float height;
        };

        struct Player {
            float x;
            float y;
            float radius;
        };

        struct Bullet {
            float x;
            float y;
            float speed;
            float angle;
            float width;
            float height;
        };

        struct Obstacle {
            float x;
            float y;
            float width;
            float height;
        };

        struct Enemy {
            float x;
            float y;
            float speed;
            float width;
            float height;
            float angle;
        };

        struct HealthBar {
            float x;
            float y;
            float width;
            float height;
        };

        struct Map {
            float x;
            float y;
            float width;
            float height;
        };


    public:
        Tema1();

        ~Tema1();

        void Init() override;

    private:
        void FrameStart() override;

        void Update(float deltaTimeSeconds) override;

        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;

        void OnKeyPress(int key, int mods) override;

        void OnKeyRelease(int key, int mods) override;

        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;

        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;

        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;

        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;

        void OnWindowResize(int width, int height) override;

        // Sets the logic space and view space
        // logicSpace: { x, y, width, height }
        // viewSpace: { x, y, width, height }
        glm::mat3 VisualizationTransf2D(const LogicSpace &logicSpace, const ViewportSpace &viewSpace);

        glm::mat3 VisualizationTransf2DUnif(const LogicSpace &logicSpace, const ViewportSpace &viewSpace);

        void SetViewportArea(const ViewportSpace &viewSpace, glm::vec3 colorColor = glm::vec3(0), bool clear = true);

        void GenerateEnemy();

        void CreateObstacles();

        void CreateEnemy();

        void DrawPlayer(glm::mat3 visMatrix);

        void DrawMap(glm::mat3 visMatrix);

        void DrawHealthBar(glm::mat3 visMatrix);

        void DrawBullets(glm::mat3 visMatrix);

        void DrawEnemy(Enemy enemy, glm::mat3 visMatrix);

    protected:
        ViewportSpace viewSpace;
        LogicSpace logicSpace;

        glm::mat3 modelMatrix, visMatrix;;

        int mouse_X, mouse_Y;

        Player player;
        HealthBar healthBar;
        Map map;

        std::vector <Bullet> bullets;
        std::vector <Enemy> enemies;
        std::vector<Obstacle *> obstacles;

        float bulletLaunchingTime, enemySpawnTime, interval;

        int hits;
        unsigned int score;

        float upperMargin, lowerMargin, leftMargin, rightMargin;
        float squareSide;
    };

}   // namespace m1
