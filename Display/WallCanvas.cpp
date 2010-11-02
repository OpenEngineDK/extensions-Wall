#include "WallCanvas.h"

namespace OpenEngine {
namespace Display {

void WallRenderer::RenderWallItem(WallItem *wi) {
        
        Item* it = wi->item;
        
        Vector<2,float> size = it->GetSize();
        
        glPushMatrix();
        
        Vector<2,float> o = wi->GetOrigin();
        
        glTranslatef(o[0],o[1],0);
        
        it->Apply(this);
        
        glPopMatrix();
}

void WallRenderer::RenderItemT(ItemT *item)  {
    glBindTexture(GL_TEXTURE_2D, item->tex->GetID());
    
    Vector<2,float> size(item->tex->GetWidth(),
                         item->tex->GetHeight());
    
    glBegin(GL_QUADS);
    glTexCoord2f(0,1);        glVertex2f(0,0);
    glTexCoord2f(1,1);        glVertex2f(size[0],0);
    glTexCoord2f(1,0);        glVertex2f(size[0],size[1]);
    glTexCoord2f(0,0);        glVertex2f(0,size[1]);        
    glEnd();
}

Vector<2,float> ItemT::GetSize() {
    return Vector<2,float>(tex->GetWidth(), tex->GetHeight());
}

void ItemT::Apply(WallRenderer *r) {
    r->RenderItemT(this);
}

}
}
