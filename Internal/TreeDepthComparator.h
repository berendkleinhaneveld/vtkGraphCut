//
//  TreeDepthComparator.h
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 18/07/16.
//
//

#ifndef TreeDepthComparator_h
#define TreeDepthComparator_h

class TreeDepthComparator
{
public:
    bool operator()(std::pair<int, NodeIndex> node1, std::pair<int, NodeIndex> node2) {
        return node1.second > node2.second;
    }
};

#endif /* TreeDepthComparator_h */
