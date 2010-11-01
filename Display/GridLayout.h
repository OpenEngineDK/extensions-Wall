// 
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS)
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------


#ifndef _OE_GRID_LAYOUT_H_
#define _OE_GRID_LAYOUT_H_

#include <Display/ILayout.h>

namespace OpenEngine {
namespace Display {
/**
 * Short description.
 *
 * @class GridLayout GridLayout.h ons/Wall/Display/GridLayout.h
 */
class GridLayout : public ILayout {
private:

public:
    void LayoutItems(RectType items, Vector<2,float> size);
};

} // NS Display
} // NS OpenEngine

#endif // _OE_GRID_LAYOUT_H_
