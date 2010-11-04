// 
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------


#ifndef _OE_WALL_CANVAS_H_
#define _OE_WALL_CANVAS_H_

#include <Display/ICanvas.h>
#include <Resources/IFontResource.h>
#include <Devices/IMouse.h>
#include <Renderers/IRenderer.h>
#include <Renderers/TextureLoader.h>
#include <Display/ILayout.h>

#include <Logging/Logger.h>
#include <Meta/OpenGL.h>
#include <Display/OrthogonalViewingVolume.h>
#include <Resources/IFontTextureResource.h>

#include <vector>

using namespace OpenEngine::Resources;
using namespace OpenEngine::Renderers;
using namespace OpenEngine::Devices;
using namespace std;

namespace OpenEngine {
namespace Display {

/**
 * Short description.
 *
 * @class WallCanvas WallCanvas.h s/MRISIM/Display/WallCanvas.h
 */
class WallRenderer;

class Item {
public:
    virtual Vector<2,float> GetSize() =0;
    virtual void Apply(WallRenderer *r) =0;
};
    
class ItemT : public Item {
public:
    ITextureResourcePtr tex;

    Vector<2,float> GetSize();

    void Apply(WallRenderer *r);
};


class WallItem : public Rect {
private:
    Vector<2,float> origin;
    //Vector<2,float> size;
public:
    Item *item;
    Vector<2,float> GetOrigin() { return origin; }
    Vector<2,float> GetSize() { return item->GetSize(); }
    void SetOrigin(Vector<2,float> o) { origin = o;}
    
};

class WallRenderer {
public:
    void RenderItemT(ItemT *item);

    void RenderWallItem(WallItem *wi);

};

class ItemGroup : public Item {
    vector<WallItem*> items;
public:
    void AddItem(WallItem *item) {
        items.push_back(item);
    }
    Vector<2,float> GetSize() {
        Vector<2,float> size;
        for(vector<WallItem*>::iterator itr = items.begin();
            itr != items.end();
            itr++) {
            WallItem* wi = *itr;
            Vector<2,float> span = wi->GetOrigin() + wi->GetSize();
            if (span[0] >= size[0])
                size[0] = span[0];
            if (span[1] >= size[1])
                size[1] = span[1];
        }
        return size;
    }
    void Apply(WallRenderer *r) {
        for(vector<WallItem*>::iterator itr = items.begin();
            itr != items.end();
            itr++) {
            r->RenderWallItem(*itr);
        }
    }
    
};



class WallCanvas : public ICanvas
                 , public IListener<MouseMovedEventArg>
                 , public IListener<MouseButtonEventArg> {
private:
    bool init;
    WallRenderer *wrenderer;
    IFontResourcePtr font;
    
    class RenderCanvasWrapper : public IRenderCanvas {
        WallCanvas *wc;
    public:
        RenderCanvasWrapper(WallCanvas *wc) : wc(wc) {
        }
        void Handle(InitializeEventArg arg)  {}
        void Handle(DeinitializeEventArg arg)  {}
        void Handle(ProcessEventArg arg)  {}
        void Handle(ResizeEventArg arg)  {}
        unsigned int GetHeight() const { return wc->GetHeight(); }
        unsigned int GetWidth() const { return wc->GetWidth(); }
        void SetHeight(unsigned int h) { wc->SetHeight(h); }
        void SetWidth(unsigned int w) { wc->SetWidth(w); }
        ITexture2DPtr GetTexture() {return wc->GetTexture();}
    };

    RenderCanvasWrapper *wrap;

    vector<WallItem*> items;
    WallItem* selectedItem;
    Vector<2,float> selectedOffset;
    IRenderer& renderer;
    TextureLoader& loader;
    ILayout* layout;
    Vector<4,float> backgroundColor;
    
    void RedoLayout() {
        if (layout) {
            RectType r = vector<Rect*>(items.begin(), items.end());
            // layout->LayoutItems(r, Vector<2,float>(GetWidth(), GetHeight()));
        }
    }

public:
    WallCanvas(ICanvasBackend* backend, IRenderer& renderer, TextureLoader& loader, IFontResourcePtr font, ILayout* l = NULL)
        : ICanvas(backend)
        , init(false)
        , font(font)
        , renderer(renderer)
        , loader(loader)
        , layout(l) {

        wrenderer = new WallRenderer();
        wrap = new RenderCanvasWrapper(this);
        backgroundColor = Vector<4,float>(0.5);
    }

    virtual ~WallCanvas() {
    
    }

    void SetBackgroundColor(Vector<4,float> bg) {
        backgroundColor = bg;
    }

    void AddTextureWithText(ITextureResourcePtr tex, string txt) {
        WallItem *wi = new WallItem();
        ItemT *it = new ItemT();

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
        RedoLayout();

    }

    void AddText(string txt, IFontResourcePtr fnt) {
        WallItem *wit = new WallItem();
        ItemT* it = new ItemT();
        Vector<2,int> size = fnt->TextDim(txt);
        IFontTextureResourcePtr txtt = fnt->CreateFontTexture(size[0]+2, size[1]+2);
        
        
        txtt->Clear(Vector<4,float>(0,0,0,0));
        
        fnt->RenderText(txt, txtt, 0, 0);
        
        loader.Load(txtt);

        wit->item = it;
        it->tex = txtt;
        items.push_back(wit);
        RedoLayout();
    }

    void AddText(string txt) {
        AddText(txt,font);
    }

    void Handle(Display::InitializeEventArg arg) {
        if (init) return;
        backend->Init(arg.canvas.GetWidth(), arg.canvas.GetHeight());
        ((IListener<Renderers::InitializeEventArg>&)renderer).Handle(Renderers::InitializeEventArg(*wrap));
        init = true;
        RedoLayout();
    }

    void Handle(Display::ProcessEventArg arg) {
        backend->Pre();

        renderer.SetBackgroundColor(backgroundColor);
        ((IListener<Renderers::ProcessEventArg>&)renderer).Handle(Renderers::ProcessEventArg(*wrap, arg.start, arg.approx));

        // Vector<4,float> bg = backgroundColor;
        // glClearColor(bg[0],bg[1],bg[2],bg[3]);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


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

            wrenderer->RenderWallItem(wi);

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


        backend->Post();
    }

    void Handle(Display::ResizeEventArg arg) {
        backend->SetDimensions(arg.canvas.GetWidth(), arg.canvas.GetHeight());
    }

    void Handle(Display::DeinitializeEventArg arg) { 
        if (!init) return;
        
        backend->Deinit();
        init = false;
    }

    void Handle(MouseMovedEventArg arg) {
        if (selectedItem) {
            Vector<2,int> pos = GetPosition();
            Vector<2,float> c(arg.x - pos[0],
                              arg.y - pos[1]);
            selectedItem->SetOrigin(c + selectedOffset);
        }
    }
    
    void Handle(MouseButtonEventArg arg) {
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

    unsigned int GetWidth() const {
        return backend->GetTexture()->GetWidth();
    };

    unsigned int GetHeight() const {
        return backend->GetTexture()->GetHeight();
    };


    void SetWidth(const unsigned int width) {
        backend->SetDimensions(width, backend->GetTexture()->GetHeight());
    }

    void SetHeight(const unsigned int height) {
        backend->SetDimensions(backend->GetTexture()->GetWidth(), height);
    }

    ITexture2DPtr GetTexture() {
        return backend->GetTexture();
    }
};


} // NS Display
} // NS OpenEngine

#endif // _OE_WALL_CANVAS_H_
