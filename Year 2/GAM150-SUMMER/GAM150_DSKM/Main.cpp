// ---------------------------------------------------------------------------
// includes

//#include "framework.h"
#include "AEEngine.h"



// ---------------------------------------------------------------------------
// Libraries
//#pragma comment (lib, "Alpha_Engine.lib")



// ---------------------------------------------------------------------------
// main

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
  _In_opt_ HINSTANCE hPrevInstance,
  _In_ LPWSTR    lpCmdLine,
  _In_ int       nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  ///////////////////////
  // Variable declaration
  AEGfxVertexList* pMesh1 = 0; // Pointer to Mesh (Model) 
  AEGfxVertexList* pMesh2 = 0; // Pointer to Mesh (Model) 
  AEGfxTexture* pTex1; // Pointer to Texture (Image) 
  float obj1X = 0.0f, obj1Y = 0.0f; // Position variables for object 

  int gGameRunning = 1;

  // Variable declaration end
  ///////////////////////////


  /////////////////
  // Initialization

  //set background color 
  //AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);

  // Using custom window procedure
  AESysInit(hInstance, nCmdShow, 800, 600, 1, 60, true, NULL);


  // Changing the window title
  AESysSetWindowTitle("Tutorial for GAM150");

  // reset the system modules
  AESysReset();

  // Initialization end
  /////////////////////



  ////////////////////////////////
  // Creating the objects (Shapes)

    // Informing the library that we're about to start adding triangles
  AEGfxMeshStart();

  // This shape has 2 triangles
  AEGfxTriAdd(
    -30.0f, -30.0f, 0x00FF00FF, 0.0f, 1.0f,
    30.0f, -30.0f, 0x00FFFF00, 1.0f, 1.0f,
    -30.0f, 30.0f, 0x0000FFFF, 0.0f, 0.0f);

  AEGfxTriAdd(
    30.0f, -30.0f, 0x00FFFFFF, 1.0f, 1.0f,
    30.0f, 30.0f, 0x00FFFFFF, 1.0f, 0.0f,
    -30.0f, 30.0f, 0x00FFFFFF, 0.0f, 0.0f);

  // Saving the mesh (list of triangles) in pMesh2

  pMesh2 = AEGfxMeshEnd();
 // AE_ASSERT_MESG(pMesh2, "Failed to create mesh 2!!");


  // Informing the library that we're about to start adding triangles
  AEGfxMeshStart();

  // 1 triangle at a time
  // X, Y, Color, texU, texV
  AEGfxTriAdd(
    -25.5f, -25.5f, 0xFFFF0000, 0.0f, 0.0f,
    25.5f, 0.0f, 0xFFFF0000, 0.0f, 0.0f,
    -25.5f, 25.5f, 0xFFFF0000, 0.0f, 0.0f);

  // Saving the mesh (list of triangles) in pMesh1

  pMesh1 = AEGfxMeshEnd();
  //AE_ASSERT_MESG(pMesh1, "Failed to create mesh 1!!");


  // Creating the objects (Shapes) end
  ////////////////////////////////////



  ////////////////////////////
  // Loading textures (images)

  // Texture 1: From file 
  pTex1 = AEGfxTextureLoad("PlanetTexture.png"); 
  AE_ASSERT_MESG(pTex1, "Failed to create texture1!!"); 

  // Loading textures (images) end
  //////////////////////////////////

  //////////////////////////////////
  // Creating Fonts	

  // Creating Fonts end
  //////////////////////////////////


  // Game Loop
  while (gGameRunning)
  {
    // Informing the system about the loop's start
    AESysFrameStart();

    // No tint   - Object 1
    AEGfxSetTintColor(1.0f, 1.0f, 1.0f, 1.0f);

    //For pMesh2 – Object2
      // No tint  
    AEGfxSetTintColor(1.0f, 1.0f, 1.0f, 1.0f);

    //For pMesh2 – Object3
    // Add Blue tint  
    AEGfxSetTintColor(0.0f, 0.0f, 1.0f, 1.0f);

    // Drawing object 1
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    // Set position for object 1
    AEGfxSetPosition(0.0f, 0.0f);
    // No texture for object 1
    AEGfxTextureSet(NULL, 0, 0);
    // Drawing the mesh (list of triangles)
    AEGfxMeshDraw(pMesh1, AE_GFX_MDM_TRIANGLES);


    // Drawing object 2 - (first) - No tint
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    // Set position for object 2
    AEGfxSetPosition(100.0f, -60.0f);
    // No tint
    AEGfxSetTintColor(1.0f, 1.0f, 1.0f, 1.0f);
    // Set texture
    AEGfxTextureSet(pTex1, 0.0f, 0.0f);
    // Drawing the mesh (list of triangles)
    AEGfxMeshDraw(pMesh2, AE_GFX_MDM_TRIANGLES);


    // Drawing object 3 - (Second) - Blue tint
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    // Set position for object 3
    AEGfxSetPosition(100.0f, 60.0f);
    // Add Blue tint
    AEGfxSetTintColor(0.0f, 0.0f, 1.0f, 1.0f);
    // Set texture
    AEGfxTextureSet(pTex1, 0.0f, 0.0f);
    // Drawing the mesh (list of triangles)
    AEGfxMeshDraw(pMesh2, AE_GFX_MDM_TRIANGLES);

    // Handling Input
    AEInputUpdate();

    ///////////////////
    // Game loop update

        // Object 1 Control
    if (AEInputCheckCurr(AEVK_UP))
      obj1Y += 3.0f;
    else if (AEInputCheckCurr(AEVK_DOWN))
      obj1Y -= 3.0f;

    if (AEInputCheckCurr(AEVK_LEFT))
      obj1X -= 3.0f;
    else if (AEInputCheckCurr(AEVK_RIGHT))
      obj1X += 3.0f;

    AEGfxMeshFree(pMesh2);
    AEGfxTextureUnload(pTex1);
    // Game loop update end
    ///////////////////////


    //////////////////
    // Game loop draw

    // Set position for object 1 
    AEGfxSetPosition(obj1X, obj1Y);

    // Game loop draw end
    /////////////////////


    // Informing the system about the loop's end
    AESysFrameEnd();

    // check if forcing the application to quit
    if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
      gGameRunning = 0;
  }

  // free the system
  AESysExit();
}
