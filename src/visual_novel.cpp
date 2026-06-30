#include "visual_novel.h"
#include "raylib.h"
#include <cstring>
#include <cstdio>

VisualNovel::VisualNovel() : texturesLoaded(false) { Init(); }
VisualNovel::~VisualNovel() {}

void VisualNovel::Init() {
    currentNodeIndex = 0; selectedChoice = 0;
    sceneFinished = false; karmaDelta = 0; textAlpha = 0.0f;
}

void VisualNovel::LoadTextures() {
    if (!texturesLoaded) {
        bgTexture    = ::LoadTexture("assets/bg_dialog.png");
        ciriaco_tex  = ::LoadTexture("assets/npc_guide.png");
        rodrigo_tex  = ::LoadTexture("assets/rodrigo_paz.png");
        texturesLoaded = true;
    }
}

void VisualNovel::UnloadTextures() {
    if (!texturesLoaded) return;
    ::UnloadTexture(bgTexture);
    ::UnloadTexture(ciriaco_tex);
    ::UnloadTexture(rodrigo_tex);
    texturesLoaded = false;
}

// ─── Story helpers ────────────────────────────────────────────────────────────
void VisualNovel::AddNode(int spkIdx, const char* spk, const char* txt) {
    DialogNode n;
    n.speaker = spk; n.text = txt; n.speakerIdx = spkIdx;
    currentNodes.push_back(n);
}

void VisualNovel::AddChoice(int spkIdx, const char* spk, const char* txt,
                             std::initializer_list<const char*> ch,
                             std::initializer_list<int> karma) {
    DialogNode n;
    n.speaker = spk; n.text = txt; n.speakerIdx = spkIdx;
    for (auto& c : ch)     n.choices.push_back(c);
    for (auto& k : karma)  n.karmaEffects.push_back(k);
    currentNodes.push_back(n);
}

void VisualNovel::LoadScene(int sceneIndex) {
    currentNodes.clear(); currentNodeIndex = 0;
    selectedChoice = 0; sceneFinished = false;
    karmaDelta = 0; textAlpha = 0.0f;
    switch (sceneIndex) {
        case 0: BuildIntro();       break;
        case 1: BuildEcoMiddle();   break;
        case 2: BuildEconMiddle();  break;
        case 3: BuildGoodEnding();  break;
        case 4: BuildBadEnding();   break;
    }
}

// ─── ESCENA 0: INTRO - El debate de Tariquía ──────────────────────────────────
// Don Ciriaco = 0, Rodrigo Paz = 1
void VisualNovel::BuildIntro() {
    AddNode(0,"Don Ciriaco",
        "Joven, bienvenido. Soy Don Ciriaco, guardian de estos valles. Siéntate, hay algo urgente que debes saber.");
    AddNode(0,"Don Ciriaco",
        "La Reserva Natural de Tariquia, nuestro pulmón verde. Hogar del oso de anteojos, el jaguar, el cóndor andino y más de 30 mil hectáreas de bosque virgen.");
    AddNode(1,"Rodrigo Paz",
        "Permítame. Soy Rodrigo Paz, Gobernador de Tarija. Don Ciriaco, le respeto mucho, pero debo ser directo con este joven.");
    AddNode(1,"Rodrigo Paz",
        "Bajo esa reserva hay gas suficiente para cambiar la historia económica de Bolivia. Regalías, empleo, desarrollo. ¿Acaso eso no vale?");
    AddNode(0,"Don Ciriaco",
        "¡Rodrigo! El dinero del gas dura 20 años. La reserva lleva miles de años. ¿Qué le dejamos a los que vienen después de nosotros?");
    AddNode(1,"Rodrigo Paz",
        "Con la tecnología moderna podemos perforar con impacto mínimo. No es el año 1970. Bolivia ya no puede darse el lujo de dejar esa riqueza bajo tierra.");
    AddNode(0,"Don Ciriaco",
        "Los comunarios de Tariquia ya dijeron NO. Sus aguas, su tierra, sus animales. Son su vida, Rodrigo. No solo un número en el PIB.");
    AddNode(1,"Rodrigo Paz",
        "Los comunarios serán compensados. Y con las regalías podemos construir escuelas, hospitales... ¿No quieren eso para sus hijos?");
    AddNode(0,"Don Ciriaco",
        "Joven... ha escuchado ambas posiciones. El gobernador promete progreso. Yo te pido que pienses en lo que no tiene precio. ¿Qué decides?");
    AddChoice(0,"Don Ciriaco",
        "El futuro está en tus manos ahora:",
        {"Rodrigo tiene razon. Bolivia necesita ese gas para progresar.",
         "Don Ciriaco tiene razon. Tariquia es irreemplazable."},
        {-2, 2});
}

// ─── ESCENA 1 (Eco): Defensa del bosque ───────────────────────────────────────
void VisualNovel::BuildEcoMiddle() {
    AddNode(0,"Don Ciriaco",
        "¡Lo lograste! Los blobs de contaminación retrocedieron. El bosque respira un poco más libre.");
    AddNode(0,"Don Ciriaco",
        "Pero el corazón de Tariquia, el Río San Telmo, aún está amenazado. Las maquinarias esperan en el borde de la reserva.");
    AddNode(0,"Don Ciriaco",
        "Un ingeniero propone perforar ALREDEDOR de la reserva para minimizar el daño. Dice que es una solución de compromiso.");
    AddChoice(0,"Don Ciriaco",
        "¿Qué responde usted?",
        {"Aceptar. Es un punto medio razonable entre economía y naturaleza.",
         "Rechazar. Tariquia se protege completa o no se protege. No hay medias tintas."},
        {-1, 1});
}

// ─── ESCENA 2 (Econ): La consecuencia del progreso ────────────────────────────
void VisualNovel::BuildEconMiddle() {
    AddNode(1,"Rodrigo Paz",
        "¡Lo lograste! Los Drill-Golems que despertó la maquinaria descontrolada han sido detenidos. Pero la situación se complica...");
    AddNode(0,"Don Ciriaco",
        "¡Rodrigo! ¿Ves lo que provoca la perforación sin control? Esas criaturas de aceite y metal nacieron del subsuelo revuelto. ¡El río ya tiene manchas negras!");
    AddNode(1,"Rodrigo Paz",
        "Es... inesperado. Pero las empresas tienen protocolos de remediación ambiental. Se puede contener. El contrato lo obliga.");
    AddNode(0,"Don Ciriaco",
        "Joven... mientras los Golems del petróleo siguen despertando, todavía puedes cambiar de posición. Los tribunales aún pueden suspender las licencias. ¿Qué decides?");
    AddChoice(1,"Rodrigo Paz",
        "El Gobernador te mira fijamente, esperando tu respuesta:",
        {"Apoyar a Rodrigo. El gas es el futuro de Bolivia, podemos manejar esto.",
         "Escuchar a Don Ciriaco. Me uno a la denuncia legal para proteger Tariquia."},
        {-1, 1});
}

// ─── ESCENA 3: BUEN FINAL ─────────────────────────────────────────────────────
void VisualNovel::BuildGoodEnding() {
    AddNode(0,"Don Ciriaco",
        "Joven guardian... la Reserva Natural de Tariquia... ESTA PROTEGIDA.");
    AddNode(0,"Don Ciriaco",
        "Tu decisión inspiró a otros. Los juzgados emitieron una medida cautelar. Las perforadoras se retiraron.");
    AddNode(1,"Rodrigo Paz",
        "Debo admitirlo... quizás me equivoqué al apresurarme. Bolivia encontrará otras fuentes de ingreso. La reserva es un patrimonio que no tiene precio.");
    AddNode(0,"Don Ciriaco",
        "El oso de anteojos tiene hogar. El cóndor vuela libre. El Río San Telmo sigue cantando. Y Tariquia... seguirá viva para las próximas generaciones.");
    AddChoice(0,"Don Ciriaco",
        "Gracias por ser un verdadero guardian de Tarija.",
        {"[ BUEN FINAL - Volver al Menu ]"},{0});
}

// ─── ESCENA 4: MAL FINAL ──────────────────────────────────────────────────────
void VisualNovel::BuildBadEnding() {
    AddNode(1,"Rodrigo Paz",
        "Las perforadoras entraron a Tariquia. Fui yo quien firmó el contrato. Esperaba que esto fuera el inicio del progreso... no calculé las consecuencias.");
    AddNode(0,"Don Ciriaco",
        "Los árboles cayeron. El oso de anteojos ya no se ve. Y los Drill-Golems del subsuelo siguen despertando donde la tierra fue herida. La naturaleza responde a su manera.");
    AddNode(1,"Rodrigo Paz",
        "...Las empresas se fueron antes de lo previsto. El campo era menos rentable de lo esperado. Las regalías fueron menores a las proyectadas. Solo quedaron los daños.");
    AddNode(0,"Don Ciriaco",
        "Tariquia era un regalo que no supimos cuidar. Joven... el dinero no alcanzó y el bosque ya no existe. Solo espero que la próxima generación aprenda de este error.");
    AddChoice(0,"Don Ciriaco",
        "Algunas pérdidas no tienen remedio.",
        {"[ MAL FINAL - Volver al Menu ]"},{0});
}

// ─── Update ───────────────────────────────────────────────────────────────────
void VisualNovel::Update() {
    if (sceneFinished) return;
    float dt = GetFrameTime();
    if (textAlpha < 1.0f) { textAlpha += dt * 2.5f; if (textAlpha > 1.0f) textAlpha = 1.0f; }

    if (currentNodeIndex < (int)currentNodes.size()) {
        const auto& node = currentNodes[currentNodeIndex];
        bool hasRealChoice = (node.choices.size() > 1);

        if (!hasRealChoice && node.choices.empty()) {
            // Pure narration: press enter
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                currentNodeIndex++; textAlpha = 0.0f;
            }
        } else if (hasRealChoice) {
            if (IsKeyPressed(KEY_DOWN))
                selectedChoice = (selectedChoice+1) % (int)node.choices.size();
            if (IsKeyPressed(KEY_UP))
                selectedChoice = (selectedChoice-1+(int)node.choices.size()) % (int)node.choices.size();
            if (IsKeyPressed(KEY_ENTER)) {
                karmaDelta += node.karmaEffects[selectedChoice];
                currentNodeIndex++; selectedChoice = 0; textAlpha = 0.0f;
            }
        } else {
            // Single choice ([TERMINAR...])
            if (IsKeyPressed(KEY_ENTER)) {
                currentNodeIndex++; textAlpha = 0.0f;
            }
        }
    }
    if (currentNodeIndex >= (int)currentNodes.size()) sceneFinished = true;
}

// ─── DrawWrappedText (FIXED) ──────────────────────────────────────────────────
void VisualNovel::DrawWrappedText(const char* text, int x, int y, int maxWidth, int fontSize, Color color) {
    char word[256];
    char line[512];
    line[0] = '\0';
    int curY = y;
    const char* p = text;

    while (*p) {
        // Skip spaces between words
        while (*p == ' ') p++;
        if (!*p) break;

        // Read one word
        int wi = 0;
        while (*p && *p != ' ' && wi < 254) word[wi++] = *p++;
        word[wi] = '\0';

        // Try appending word to current line
        char test[512];
        if (line[0] == '\0') {
            snprintf(test, sizeof(test), "%s", word);
        } else {
            snprintf(test, sizeof(test), "%s %s", line, word);
        }

        if (MeasureText(test, fontSize) <= maxWidth) {
            // Word fits: keep building this line
            snprintf(line, sizeof(line), "%s", test);
        } else {
            // Word doesn't fit: flush current line and start new one
            if (line[0] != '\0') {
                DrawText(line, x, curY, fontSize, color);
                curY += fontSize + 5;
            }
            // Start new line with this word
            snprintf(line, sizeof(line), "%s", word);
        }
    }
    // Draw remaining line
    if (line[0] != '\0') {
        DrawText(line, x, curY, fontSize, color);
    }
}

// ─── Draw ─────────────────────────────────────────────────────────────────────
void VisualNovel::Draw() {
    if (sceneFinished || currentNodeIndex >= (int)currentNodes.size()) return;
    const auto& node = currentNodes[currentNodeIndex];
    int spkIdx = node.speakerIdx;

    // Background
    if (texturesLoaded && bgTexture.id != 0)
        DrawTexturePro(bgTexture,{0,0,(float)bgTexture.width,(float)bgTexture.height},{0,0,800,450},{0,0},0,WHITE);
    else
        ClearBackground({30,15,5,255});

    // Dark overlay bottom
    DrawRectangle(0, 215, 800, 235, {0,0,0,195});

    // ── Portrait rendering ────────────────────────────────────────────────────
    // Left: Don Ciriaco (spkIdx==0 = active)
    {
        float alpha = (spkIdx == 0) ? 1.0f : 0.45f;
        Color tint = Fade(WHITE, alpha);
        float scale = (spkIdx == 0) ? 1.0f : 0.88f;
        float pw = 150 * scale, ph = 200 * scale;
        float py = 215 + (200 - ph); // align to bottom
        if (texturesLoaded && ciriaco_tex.id != 0)
            DrawTexturePro(ciriaco_tex,{0,0,(float)ciriaco_tex.width,(float)ciriaco_tex.height},{10,py,pw,ph},{0,0},0,tint);
        else { DrawRectangle(10,(int)py,(int)pw,(int)ph,Fade(BROWN,alpha)); }
    }

    // Right: Rodrigo Paz (spkIdx==1 = active)
    {
        float alpha = (spkIdx == 1) ? 1.0f : 0.45f;
        Color tint = Fade(WHITE, alpha);
        float scale = (spkIdx == 1) ? 1.0f : 0.88f;
        float pw = 150 * scale, ph = 200 * scale;
        float px = 800 - 10 - pw;
        float py = 215 + (200 - ph);
        if (texturesLoaded && rodrigo_tex.id != 0) {
            // Mirror Rodrigo so he faces left (toward Ciriaco)
            DrawTexturePro(rodrigo_tex,{0,0,-(float)rodrigo_tex.width,(float)rodrigo_tex.height},{px,py,pw,ph},{0,0},0,tint);
        } else { DrawRectangle((int)px,(int)py,(int)pw,(int)ph,Fade(DARKBLUE,alpha)); }
    }

    // ── Dialog box ────────────────────────────────────────────────────────────
    bool hasRealChoice = (node.choices.size() > 1);
    bool hasSingleChoice = (node.choices.size() == 1);
    int dialogH = (hasRealChoice) ? 90 : 110;
    int dialogY = 268;

    DrawRectangleRounded({170, (float)dialogY, 460, (float)dialogH}, 0.06f, 4, {8,4,2,220});
    DrawRectangleRoundedLines({170,(float)dialogY,460,(float)dialogH}, 0.06f, 4,
        spkIdx == 0 ? (Color){200,130,40,200} : (Color){40,80,200,200});

    // Speaker name tag
    Color nameBarColor = (spkIdx == 0) ? (Color){100,55,10,230} : (Color){20,40,130,230};
    Color nameBorderColor = (spkIdx == 0) ? (Color){220,160,60,255} : (Color){80,130,220,255};
    float nameX = (spkIdx == 0) ? 170 : 430;
    DrawRectangleRounded({nameX,(float)dialogY-26,200,26}, 0.3f, 4, nameBarColor);
    DrawRectangleRoundedLines({nameX,(float)dialogY-26,200,26}, 0.3f, 4, nameBorderColor);
    DrawText(node.speaker.c_str(), (int)nameX+10, dialogY-20, 18, {255,225,140,255});

    // Dialog text (FIXED word wrap)
    DrawWrappedText(node.text.c_str(), 182, dialogY+8, 436, 16, Fade(WHITE, textAlpha));

    // ── Choices ───────────────────────────────────────────────────────────────
    if (hasRealChoice) {
        int choiceStartY = dialogY + dialogH + 8;
        for (int i = 0; i < (int)node.choices.size(); i++) {
            bool sel = (i == selectedChoice);
            float oy = choiceStartY + i * 27;
            Rectangle opt = {170, oy, 460, 25};
            if (sel) {
                DrawRectangleRounded(opt, 0.2f, 4, {100,60,10,200});
                DrawRectangleRoundedLines(opt, 0.2f, 4, GOLD);
                DrawText(">", 175, (int)oy+4, 16, GOLD);
            }
            DrawText(node.choices[i].c_str(), 192, (int)oy+4, 15, sel ? GOLD : LIGHTGRAY);
        }
        DrawText("[Arriba/Abajo + Enter]", 615, 428, 11, {130,130,130,200});
    } else if (hasSingleChoice) {
        // Terminal choice
        DrawRectangleRounded({170,(float)(dialogY+dialogH+8),460,25},0.2f,4,{100,60,10,200});
        DrawText(node.choices[0].c_str(), 185, dialogY+dialogH+12, 15, GOLD);
        DrawText("[ ENTER ]", 610, dialogY+dialogH+12, 14, {200,200,100,200});
    } else {
        // Continue prompt
        if ((int)(GetTime()*2)%2==0)
            DrawText("[ ENTER / ESPACIO para continuar ]", 290, 428, 13, {180,180,180,200});
    }
}

bool VisualNovel::IsSceneFinished() const { return sceneFinished; }
int  VisualNovel::GetKarmaDelta() const { return karmaDelta; }
