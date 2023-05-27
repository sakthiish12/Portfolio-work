#ifndef MAIN_H
#define MAIN_H

void InitToolKit();

void UI_AsciiCharacterBrush();
void UI_ColorPallete();
void UI_CurrBrushInfo();
void UI_DefinePaintArea();
void UI_StationarySet();
void UI_LoadAndSave();
void UI_LayerMode();
void Painting(int brushType, int posX, int posY);
void Save();
void Load();
void ChangeMode(int mode);
void OnionPainting(int posX, int posY);


void MoveCanvasData(COORD start, COORD end, int displaceX, int displaceY);
void ProcessInput();
void Update();
void Rendering();



#endif
