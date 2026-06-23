#ifndef VISUAL_NOVEL_H
#define VISUAL_NOVEL_H

#include "raylib.h"
#include <string>
#include <vector>

struct DialogNode {
    std::string speaker;
    std::string text;
    std::vector<std::string> choices;
    std::vector<int> karmaEffects;
    int speakerIdx; // 0 = Don Ciriaco (left), 1 = Rodrigo Paz (right)
};

class VisualNovel {
public:
    VisualNovel();
    ~VisualNovel();

    void Init();
    void LoadTextures();
    void UnloadTextures();

    void LoadScene(int sceneIndex);
    void Update();
    void Draw();

    bool IsSceneFinished() const;
    int GetKarmaDelta() const;

private:
    std::vector<DialogNode> currentNodes;
    int currentNodeIndex;
    int selectedChoice;
    bool sceneFinished;
    int karmaDelta;
    float textAlpha;

    Texture2D bgTexture;
    Texture2D ciriaco_tex;   // Don Ciriaco - left portrait
    Texture2D rodrigo_tex;   // Rodrigo Paz - right portrait
    bool texturesLoaded;

    void BuildIntro();
    void BuildEcoMiddle();
    void BuildEconMiddle();
    void BuildGoodEnding();
    void BuildBadEnding();

    // Helper: add a node easily
    void AddNode(int spkIdx, const char* spk, const char* txt);
    void AddChoice(int spkIdx, const char* spk, const char* txt,
                   std::initializer_list<const char*> ch,
                   std::initializer_list<int> karma);

    void DrawWrappedText(const char* text, int x, int y, int maxWidth, int fontSize, Color color);
};

#endif // VISUAL_NOVEL_H
