#include "WallCanvas.h"
#include <Logging/Logger.h>
#include <Meta/OpenGL.h>
#include <Display/OrthogonalViewingVolume.h>
#include <Resources/IFontTextureResource.h>

namespace OpenEngine {
namespace Display {

    WallCanvas::WallCanvas(TextureLoader& loader, IFontResourcePtr font) 
    : TextureCanvasBase()
    , init(false)
    , font(font)
    , loader(loader) {

    renderer = new WallRenderer();
}

WallCanvas::~WallCanvas() {
    
}

void WallCanvas::AddTextureWithText(ITextureResourcePtr tex, string txt) {
    WallItem *wi = new WallItem();
    ItemT *it = new ItemT();
    loader.Load(tex);
    it->tex = tex;    
    wi->item = it;

    WallItem *wit = new WallItem();
    it = new ItemT();
    Vector<2,int> size = font->TextDim(txt);
    IFontTextureResourcePtr txtt = font->CreateFontTexture(size[0]+2, size[1]+2);
    
    
    txtt->Clear(Vector<4,float>(0,0,0,0));

    font->RenderText(txt, txtt, 0, 0);

    loader.Load(txtt);

    wit->item = it;
    it->tex = txtt;

    WallItem *gitem = new WallItem();
    ItemGroup *gi = new ItemGroup();
    gitem->item = gi;
    gi->AddItem(wi);
    
    Vector<2,float> mid = wi->GetSize();
    
    mid[0] = mid[0]/2.0 - wit->GetSize()[0]/2.0;

    wit->SetOrigin(mid);
    gi->AddItem(wit);
    items.push_back(gitem);
    

}

void WallCanvas::Handle(Display::InitializeEventArg arg) {
    if (init) return;
    CreateTexture();
    SetTextureWidth(arg.canvas.GetWidth());
    SetTextureHeight(arg.canvas.GetHeight());
    SetupTexture();

    init = true;
}

void WallCanvas::Handle(ResizeEventArg arg) {
    SetTextureWidth(arg.canvas.GetWidth());
    SetTextureWidth(arg.canvas.GetWidth());
    SetupTexture();
}

void WallCanvas::Handle(Display::ProcessEventArg arg) {
    Vector<4,float> bg(0.7);
    glClearColor(bg[0],bg[1],bg[2],bg[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    Vector<4,int> d(0, 0, GetWidth(), GetHeight());
    
    //logger.info << d << logger.end;

    glViewport((GLsizei)d[0], (GLsizei)d[1], (GLsizei)d[2], (GLsizei)d[3]);
    OrthogonalViewingVolume volume(-1, 1, 0, GetWidth(), 0, GetHeight());    

    // Select The Projection Matrix
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    CHECK_FOR_GL_ERROR();

    // Reset The Projection Matrix
    glLoadIdentity();
    CHECK_FOR_GL_ERROR();

    // Setup OpenGL with the volumes projection matrix
    Matrix<4,4,float> projMatrix = volume.GetProjectionMatrix();
    float arr[16] = {0};
    projMatrix.ToArray(arr);
    glMultMatrixf(arr);
    CHECK_FOR_GL_ERROR();
        
    // Select the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    CHECK_FOR_GL_ERROR();
        
    // Reset the modelview matrix
    glLoadIdentity();
    CHECK_FOR_GL_ERROR();
        
    // Get the view matrix and apply it
    Matrix<4,4,float> matrix = volume.GetViewMatrix();
    float f[16] = {0};
    matrix.ToArray(f);
    glMultMatrixf(f);
    CHECK_FOR_GL_ERROR();
        
    GLboolean depth = glIsEnabled(GL_DEPTH_TEST);
    GLboolean lighting = glIsEnabled(GL_LIGHTING);
    GLboolean blending = glIsEnabled(GL_BLEND);
    GLboolean texture = glIsEnabled(GL_TEXTURE_2D);
    GLint texenv;
    glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &texenv);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    for (vector<WallItem*>::iterator itr = items.begin();
         itr != items.end();
         itr++) {
        WallItem* wi = *itr;

        renderer->RenderWallItem(wi);

    }


    glBindTexture(GL_TEXTURE_2D, 0);
 
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    CHECK_FOR_GL_ERROR();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    CHECK_FOR_GL_ERROR();
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, texenv);
    if (depth)    glEnable(GL_DEPTH_TEST);
    if (lighting) glEnable(GL_LIGHTING);
    if (blending) glEnable(GL_BLEND);
    if (!texture) glDisable(GL_TEXTURE_2D);



    CopyToTexture();
}

void WallCanvas::Handle(DeinitializeEventArg arg) { 
    if (!init) return;

    init = false;
}

unsigned int WallCanvas::GetWidth() const {
    return GetTextureWidth();
}

unsigned int WallCanvas::GetHeight() const {
    return GetTextureHeight();
}
    
void WallCanvas::SetWidth(const unsigned int width) {
    SetTextureWidth(width);
    if (init) SetupTexture();
}

void WallCanvas::SetHeight(const unsigned int height) {
    SetTextureHeight(height);
    if (init) SetupTexture();
}

ITexture2DPtr WallCanvas::GetTexture() {
    return tex;
}

void WallCanvas::Handle(MouseMovedEventArg arg) {
    if (selectedItem) {
        Vector<2,int> pos = GetPosition();
        Vector<2,float> c(arg.x - pos[0],
                          arg.y - pos[1]);
        selectedItem->SetOrigin(c + selectedOffset);
    }
}
void WallCanvas::Handle(MouseButtonEventArg arg) {
    if (arg.type == EVENT_PRESS) {
        int x = arg.state.x;
        int y = arg.state.y;

        Vector<2,int> pos = GetPosition();
        
        if ((x >= pos[0]) &&
            (y >= pos[1]) &&
            (x <= pos[0] + (int)GetWidth()) &&
            (y <= pos[1] + (int)GetHeight())) {
         
            // Local coordinate
            Vector<2,float> c(x - pos[0],
                              y - pos[1]);

            // Find item...
            for (vector<WallItem*>::reverse_iterator itr = items.rbegin();
                 itr != items.rend();
                 itr++) {
                WallItem* wi = *itr;
                Vector<2,float> origin = wi->GetOrigin();
                Vector<2,float> size = wi->GetSize();

                if ((c[0] >= origin[0]) &&
                    (c[1] >= origin[1]) &&
                    (c[0] <= origin[0] + size[0]) &&
                    (c[1] <= origin[1] + size[1])
                    ) {
                    selectedItem = wi;
                    selectedOffset = origin - c;
                    return;
                }                
            }
        }
    }
    selectedItem = NULL;
}
    

void WallRenderer::RenderWallItem(WallItem *wi) {

        Item* it = wi->item;

        Vector<2,float> size = it->GetSize();
        
        glPushMatrix();

        Vector<2,float> o = wi->GetOrigin();

        glTranslatef(o[0],o[1],0);

        it->Apply(this);
        
        glPopMatrix();


}
void WallRenderer::RenderItemT(ItemT *item) {
    glBindTexture(GL_TEXTURE_2D, item->tex->GetID());
    
    Vector<2,float> size(item->tex->GetWidth(),
                         item->tex->GetHeight());

        glBegin(GL_QUADS);
        glTexCoord2f(0,0);        glVertex2f(0,0);
        glTexCoord2f(1,0);        glVertex2f(size[0],0);
        glTexCoord2f(1,1);        glVertex2f(size[0],size[1]);
        glTexCoord2f(0,1);        glVertex2f(0,size[1]);        
        glEnd();
    
}


}
}
