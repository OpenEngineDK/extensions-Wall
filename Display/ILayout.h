// 
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------


#ifndef _OE_I_LAYOUT_H_
#define _OE_I_LAYOUT_H_

#include <Math/Vector.h>
#include <vector>

namespace OpenEngine {
namespace Display {

    using namespace Math;


    class Rect {
    public:
        virtual Vector<2,float> GetOrigin() = 0;
        virtual Vector<2,float> GetSize() = 0;
        virtual void SetOrigin(Vector<2,float> o) = 0;
    };

    typedef std::vector<Rect*> RectType;

/**
 * Short description.
 *
 * @class ILayout ILayout.h ons/Wall/Display/ILayout.h
 */

class ILayout {
private:

public:
    virtual void LayoutItems(RectType items, Vector<2,float> size) = 0;
};

} // NS Display
} // NS OpenEngine

#endif // _OE_I_LAYOUT_H_
