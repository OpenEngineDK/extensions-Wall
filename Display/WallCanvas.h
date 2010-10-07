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

#include <Display/OpenGL/TextureCanvasBase.h>
#include <Display/ICanvas.h>
#include <Resources/IFontResource.h>
#include <Devices/IMouse.h>
#include <Renderers/TextureLoader.h>

#include <vector>

using namespace OpenEngine::Display::OpenGL;
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

class ItemT;
class WallItem;

class WallRenderer {
public:
    void RenderItemT(ItemT *);
    void RenderWallItem(WallItem *);
};

class Item {
public:
    virtual Vector<2,float> GetSize() =0;
    virtual void Apply(WallRenderer *r) =0;
};
    
class ItemT : public Item {
public:
    ITextureResourcePtr tex;
    Vector<2,float> GetSize() {
        return Vector<2,float>(tex->GetWidth(), tex->GetHeight());
    }
    void Apply(WallRenderer *r) {
        r->RenderItemT(this);
    }
};



class WallItem {
private:
    Vector<2,float> origin;
    //Vector<2,float> size;
public:
    Item *item;
    Vector<2,float> GetOrigin() { return origin; }
    Vector<2,float> GetSize() { return item->GetSize(); }
    void SetOrigin(Vector<2,float> o) { origin = o;}
    
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



class WallCanvas : public TextureCanvasBase
                 , public ICanvas
                 , public IListener<MouseMovedEventArg>
                 , public IListener<MouseButtonEventArg> {
private:
    bool init;
    WallRenderer *renderer;
    IFontResourcePtr font;



    vector<WallItem*> items;
    WallItem* selectedItem;
    Vector<2,float> selectedOffset;
    TextureLoader& loader;
public:
    WallCanvas(TextureLoader& loader, IFontResourcePtr font);
    virtual ~WallCanvas();

    void AddTextureWithText(ITextureResourcePtr tex, string txt);


    void Handle(Display::InitializeEventArg arg);
    void Handle(Display::ProcessEventArg arg);
    void Handle(Display::ResizeEventArg arg);
    void Handle(Display::DeinitializeEventArg arg);
    void Handle(MouseMovedEventArg arg);
    void Handle(MouseButtonEventArg arg);

    unsigned int GetWidth() const;
    unsigned int GetHeight() const;
    void SetWidth(const unsigned int width);
    void SetHeight(const unsigned int height);
    ITexture2DPtr GetTexture();
};


} // NS Display
} // NS OpenEngine

#endif // _OE_WALL_CANVAS_H_
