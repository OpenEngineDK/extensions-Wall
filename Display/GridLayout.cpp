#include "GridLayout.h"
#include <Logging/Logger.h>

namespace OpenEngine {
namespace Display {

    using namespace std;

    GridLayout::GridLayout(float margin) : margin(margin) {}

    void GridLayout::LayoutItems(RectType items, Vector<2,float> size) {        
        float w = 0.0f;
        int cols = 0;
        for(RectType::iterator itr = items.begin(); itr != items.end(); itr++) {
            Rect *r = *itr;
            w += r->GetSize()[0] + margin;
            if (w > size[0])
                break;
            cols++;

        }
        if (cols == 0) {
            logger.warning << "Cant fit anything in the grid: " << size << logger.end;
            return;
        }
        int rows = ceil(items.size() / float(cols));

        logger.info << rows << ", " << cols << logger.end;
        
        vector<float> colWidths(cols);      
        vector<float> rowHeights(rows);
        int colIdx = 0;
        
        for(RectType::iterator itr = items.begin(); itr != items.end(); itr++) {
            Rect *r = *itr;
            float iw = r->GetSize()[0] + margin;
            colWidths[colIdx % cols] = max(colWidths[colIdx % cols],iw);

            int rowIdx = colIdx / cols;            
            float ih = r->GetSize()[1] + margin;
            rowHeights[rowIdx] = max(rowHeights[rowIdx], ih);

            ++colIdx;
        }

        vector<float> colOffset(cols);
        vector<float> rowOffset(rows);
        colOffset[0] = 0;
        rowOffset[0] = 0;
        
        for (unsigned int i=1;i<colWidths.size();i++) {
            colOffset.at(i) = colOffset.at(i-1) +  colWidths.at(i-1);            
        }
        for (unsigned int i=1;i<rowHeights.size();i++) {
            rowOffset.at(i) = rowOffset.at(i-1) +  rowHeights.at(i-1);
        }

        int idx = 0;
        for (RectType::iterator itr = items.begin(); itr != items.end(); itr++) {
            Rect *r = *itr;
            int col = idx % cols;
            int row = idx / cols;
            
            Vector<2,float> pos(colOffset[col],rowOffset[row]);
            
            if (pos[0] + r->GetSize()[0] >= size[0]) 
                pos[0] = size[0] - r->GetSize()[0];
            if (pos[1] + r->GetSize()[1] >= size[1]) 
                pos[1] = size[1] - r->GetSize()[1];
            

            r->SetOrigin(pos);

            logger.warning << col << "," << row << " " << pos << logger.end;

            ++idx;
        }

            
    }

}
}
