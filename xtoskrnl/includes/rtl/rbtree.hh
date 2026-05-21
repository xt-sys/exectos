/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/rtl/rbtree.hh
 * DESCRIPTION:     Red-Black Tree implementation
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_RTL_RBTREE_HH
#define __XTOSKRNL_RTL_RBTREE_HH

#include <xtos.hh>


/* Runtime Library */
namespace RTL
{
    class RedBlackTree
    {
        public:
            STATIC XTAPI VOID InitializeTree(OUT PRTL_RB_TREE Tree);
            STATIC XTAPI VOID InsertNode(IN PRTL_RB_TREE Tree,
                                         IN PRTL_BALANCED_NODE Parent,
                                         IN PRTL_BALANCED_NODE Node,
                                         IN BOOLEAN RightChild);
            STATIC XTAPI VOID RemoveNode(IN PRTL_RB_TREE Tree,
                                         IN PRTL_BALANCED_NODE Node);

        private:
            STATIC XTAPI VOID CopyNodeColor(OUT PRTL_BALANCED_NODE Destination,
                                            IN PRTL_BALANCED_NODE Source);
            STATIC XTAPI RTL_BALANCED_NODE_COLOR GetNodeColor(IN PRTL_BALANCED_NODE Node);
            STATIC XTAPI PRTL_BALANCED_NODE GetParentNode(PRTL_BALANCED_NODE Node);
            STATIC XTAPI VOID RotateLeft(IN PRTL_RB_TREE Tree,
                                         IN PRTL_BALANCED_NODE Node);
            STATIC XTAPI VOID RotateRight(IN PRTL_RB_TREE Tree,
                                          IN PRTL_BALANCED_NODE Node);
            STATIC XTAPI VOID SetNodeColor(OUT PRTL_BALANCED_NODE Node,
                                           IN RTL_BALANCED_NODE_COLOR Color);
            STATIC XTAPI VOID SetParentNode(OUT PRTL_BALANCED_NODE Node,
                                            IN PRTL_BALANCED_NODE Parent);
    };
}

#endif /* __XTOSKRNL_RTL_RBTREE_HH */
