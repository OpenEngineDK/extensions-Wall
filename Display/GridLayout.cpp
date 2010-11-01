#include "GridLayout.h"
#include <Logging/Logger.h>

namespace OpenEngine {
namespace Display {

    using namespace std;

    void GridLayout::LayoutItems(RectType items, Vector<2,float> size) {        
        float w;
        int cols = 0;
        for(RectType::iterator itr = items.begin(); itr != items.end(); itr++) {
            Rect *r = *itr;
            w += r->GetSize()[0];
            if (w > size[0])
                break;
            cols++;
        }
        if (cols == 0) {
            logger.warning << "Cant fit anything in the grid, aborting" << logger.end;
            return;
        }
        int rows = items.size() / cols;
        
        vector<float> colWidths(cols);      
        vector<float> rowHeights(rows);
        int colIdx = 0;
        
        for(RectType::iterator itr = items.begin(); itr != items.end(); itr++) {
            Rect *r = *itr;
            float iw = r->GetSize()[0];
            colWidths[colIdx % cols] = max(colWidths[colIdx % cols],iw);

            int rowIdx = colIdx / cols;            
            float ih = r->GetSize()[1];
            rowHeights[rowIdx] = max(rowHeights[rowIdx], ih);

            ++colIdx;
        }

        vector<float> colOffset(cols);
        vector<float> rowOffset(rows);
        colOffset[0] = 0;
        rowOffset[0] = 0;
        
        for (int i=1;i<=colWidths.size();i++) {
            colOffset[i] = colOffset[i-1] +  colWidths[i-1];            
        }
        for (int i=1;i<=rowHeights.size();i++) {
            rowOffset[i] = rowOffset[i-1] +  rowHeights[i-1];

        }

        


        int idx = 0;
        for (RectType::iterator itr = items.begin(); itr != items.end(); itr++) {
            Rect *r = *itr;
            int col = idx % cols;
            int row = idx / cols;
            
            Vector<2,float> pos(colOffset[col],rowOffset[row]);
            
            r->SetOrigin(pos);

            logger.warning << col << "," << row << " " << pos << logger.end;

            ++idx;
        }

            
    }

}
}
