/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/rbtree.cc
 * DESCRIPTION:     Red-Black Tree implementation
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Copies the color from the Source node to the Dest node without modifying the destination node's parent pointer.
 *
 * @param Destination
 *        Supplies a pointer to the balanced node for which to set the color.
 *
 * @param Source
 *        Supplies a pointer to the balanced node from which to copy the color.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
RTL::RedBlackTree::CopyNodeColor(OUT PRTL_BALANCED_NODE Destination,
                                 IN PRTL_BALANCED_NODE Source)
{
    /* Copy the color from the source node to the destination node */
    Destination->ParentValue = (Destination->ParentValue & ~RTL_BALANCED_NODE_COLOR_MASK) | GetNodeColor(Source);
}

/**
 * Extracts the color bit from the node's ParentValue.
 *
 * @param Node
 *        Supplies a pointer to the balanced node from which to extract the color.
 *
 * @return This routine returns the color of the given node, either NodeRed or NodeBlack.
 *
 * @since XT 1.0
 */
XTAPI
RTL_BALANCED_NODE_COLOR
RTL::RedBlackTree::GetNodeColor(IN PRTL_BALANCED_NODE Node)
{
    /* Return the node color */
    return (RTL_BALANCED_NODE_COLOR)(Node->ParentValue & RTL_BALANCED_NODE_COLOR_MASK);
}

/**
 * Retrieves the parent node pointer by masking out the reserved bits.
 *
 * @param Node
 *        Supplies a pointer to the balanced node from which to extract the parent pointer.
 *
 * @return This routine returns a pointer to the parent node of the given node.
 *
 * @since XT 1.0
 */
XTAPI
PRTL_BALANCED_NODE
RTL::RedBlackTree::GetParentNode(IN PRTL_BALANCED_NODE Node)
{
    /* Return the parent node pointer by masking out the reserved bits */
    return (PRTL_BALANCED_NODE)(Node->ParentValue & ~RTL_BALANCED_NODE_RESERVED_PARENT_MASK);
}

/**
 * Initializes a red-black tree by setting its root and minimum pointers to NULLPTR.
 *
 * @param Tree
 *        Supplies a pointer to the RTL_RB_TREE structure to initialize.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
RTL::RedBlackTree::InitializeTree(OUT PRTL_RB_TREE Tree)
{
    /* Initialize the tree */
    Tree->Root = NULLPTR;
    Tree->Min = NULLPTR;
}

/**
 * Inserts a new node into the red-black tree at the specified position and rebalances the tree.
 *
 * @param Tree
 *        Supplies a pointer to the RTL_RB_TREE structure into which the node is inserted.
 *
 * @param Parent
 *        Supplies a pointer to the parent node under which the new node will be inserted.
 *        If the tree is empty, this must be NULLPTR.
 *
 * @param Node
 *        Supplies a pointer to the uninitialized balanced node to insert into the tree.
 *
 * @param RightChild
 *        Specifies whether the new node should be inserted as the right child (TRUE)
 *        or the left child (FALSE) of the parent node.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
RTL::RedBlackTree::InsertNode(IN PRTL_RB_TREE Tree,
                              IN PRTL_BALANCED_NODE Parent,
                              IN PRTL_BALANCED_NODE Node,
                              IN BOOLEAN RightChild)
{
    PRTL_BALANCED_NODE CurrentNode, CurrentParent, GrandParent, Uncle;

    /* Initialize the new node with no children and default its color to red */
    Node->Left = NULLPTR;
    Node->Right = NULLPTR;
    Node->ParentValue = (ULONG_PTR)Parent | NodeRed;

    /* Check if the tree is not empty */
    if(Parent != NULLPTR)
    {
        /* Determine whether to attach the node as a right or left child */
        if(RightChild)
        {
            /* Attach the new node to the right side of the parent */
            Parent->Right = Node;
        }
        else
        {
            /* Attach the new node to the left side of the parent */
            Parent->Left = Node;

            /* Check if the new node is placed left of the current minimum */
            if(Parent == Tree->Min)
            {
                /* Update the cached minimum pointer for the tree */
                Tree->Min = Node;
            }
        }
    }
    else
    {
        /* The tree was empty, so the new node becomes both the root and the minimum */
        Tree->Root = Node;
        Tree->Min = Node;
    }

    /* Begin fixing up the red-black tree properties */
    CurrentNode = Node;
    while(CurrentNode != Tree->Root && GetNodeColor(GetParentNode(CurrentNode)) == NodeRed)
    {
        /* Retrieve the parent and grandparent nodes to determine rotation and recoloring cases */
        CurrentParent = GetParentNode(CurrentNode);
        GrandParent = GetParentNode(CurrentParent);

        /* Check if the parent is a left child of the grandparent */
        if(CurrentParent == GrandParent->Left)
        {
            /* Determine the uncle node */
            Uncle = GrandParent->Right;
            if(Uncle != NULLPTR && GetNodeColor(Uncle) == NodeRed)
            {
                /* Recolor parent, uncle, and grandparent */
                SetNodeColor(CurrentParent, NodeBlack);
                SetNodeColor(Uncle, NodeBlack);
                SetNodeColor(GrandParent, NodeRed);

                /* Move the current node pointer up to the grandparent */
                CurrentNode = GrandParent;
            }
            else
            {
                /* Check if current node is a right child */
                if(CurrentNode == CurrentParent->Right)
                {
                    /* Move the current pointer to the parent and perform a left rotation */
                    CurrentNode = CurrentParent;
                    RotateLeft(Tree, CurrentNode);

                    /* Update parent and grandparent pointers after the rotation */
                    CurrentParent = GetParentNode(CurrentNode);
                    GrandParent = GetParentNode(CurrentParent);
                }

                /* Recolor parent and grandparent */
                SetNodeColor(CurrentParent, NodeBlack);
                SetNodeColor(GrandParent, NodeRed);

                /* Perform a right rotation around the grandparent to restore tree balance */
                RotateRight(Tree, GrandParent);
            }
        }
        else
        {
            /* Determine the uncle node */
            Uncle = GrandParent->Left;
            if(Uncle != NULLPTR && GetNodeColor(Uncle) == NodeRed)
            {
                /* Recolor parent, uncle, and grandparent */
                SetNodeColor(CurrentParent, NodeBlack);
                SetNodeColor(Uncle, NodeBlack);
                SetNodeColor(GrandParent, NodeRed);

                /* Move the current node pointer up to the grandparent */
                CurrentNode = GrandParent;
            }
            else
            {
                /* Check if current node is a left child */
                if(CurrentNode == CurrentParent->Left)
                {
                    /* Move the current pointer to the parent and perform a right rotation */
                    CurrentNode = CurrentParent;
                    RotateRight(Tree, CurrentNode);

                    /* Update parent and grandparent pointers after the rotation */
                    CurrentParent = GetParentNode(CurrentNode);
                    GrandParent = GetParentNode(CurrentParent);
                }

                /* Recolor parent and grandparent */
                SetNodeColor(CurrentParent, NodeBlack);
                SetNodeColor(GrandParent, NodeRed);

                /* Perform a left rotation around the grandparent to restore tree balance */
                RotateLeft(Tree, GrandParent);
            }
        }
    }

    /* Ensure the root node is always colored black */
    SetNodeColor(Tree->Root, NodeBlack);
}

/**
 * Removes the specified node from the red-black tree and rebalances the tree to maintain its properties.
 *
 * @param Tree
 *        Supplies a pointer to the RTL_RB_TREE structure from which the node is removed.
 *
 * @param Node
 *        Supplies a pointer to the balanced node to remove from the tree.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
RTL::RedBlackTree::RemoveNode(IN PRTL_RB_TREE Tree,
                              IN PRTL_BALANCED_NODE Node)
{
    PRTL_BALANCED_NODE CurrentNode, NextMinNode, NodeToRemove, ParentNode, Replacement, ReplacementParent, SiblingNode;
    ULONG_PTR RemoveNodeColor;

    /* Initialize variables to track the node */
    NodeToRemove = Node;
    RemoveNodeColor = GetNodeColor(NodeToRemove);
    Replacement = NULLPTR;
    ReplacementParent = NULLPTR;

    /* Check if the node being removed is the cached minimum node */
    if(Tree->Min == Node)
    {
        /* Find the next minimum node */
        NextMinNode = Node->Right;
        if(NextMinNode != NULLPTR)
        {
            /* Traverse to the leftmost descendant of the right child */
            while(NextMinNode->Left != NULLPTR)
            {
                /* Move down to the left child */
                NextMinNode = NextMinNode->Left;
            }

            /* Update the tree's minimum pointer to the found successor */
            Tree->Min = NextMinNode;
        }
        else
        {
            /* The parent becomes the new minimum */
            Tree->Min = GetParentNode(Node);
        }
    }

    /* Check if the node to remove has no left child */
    if(Node->Left == NULLPTR)
    {
        /* Use the right child as the replacement node */
        Replacement = Node->Right;
        ReplacementParent = GetParentNode(Node);

        /* Check if the removed node was the root of the tree */
        if(ReplacementParent == NULLPTR)
        {
            /* The replacement node becomes the new root */
            Tree->Root = Replacement;
        }
        else if(Node == ReplacementParent->Left)
        {
            /* Link the replacement to the parent's left side */
            ReplacementParent->Left = Replacement;
        }
        else
        {
            /* Link the replacement to the parent's right side */
            ReplacementParent->Right = Replacement;
        }

        /* Check if replacement exists */
        if(Replacement != NULLPTR)
        {
            /* Link the replacement back to its new parent */
            SetParentNode(Replacement, ReplacementParent);
        }
    }
    else if(Node->Right == NULLPTR)
    {
        /* Use the left child as the replacement node */
        Replacement = Node->Left;
        ReplacementParent = GetParentNode(Node);

        /* Check if the removed node was the root of the tree */
        if(ReplacementParent == NULLPTR)
        {
            /* The replacement node becomes the new root */
            Tree->Root = Replacement;
        }
        else if(Node == ReplacementParent->Left)
        {
            /* Link the replacement to the parent's left side */
            ReplacementParent->Left = Replacement;
        }
        else
        {
            /* Link the replacement to the parent's right side */
            ReplacementParent->Right = Replacement;
        }

        /* Check if replacement exists */
        if(Replacement != NULLPTR)
        {
            /* Link the replacement back to its new parent */
            SetParentNode(Replacement, ReplacementParent);
        }
    }
    else
    {
        /* Find the in-order successor to replace it */
        NodeToRemove = Node->Right;
        while(NodeToRemove->Left != NULLPTR)
        {
            /* Traverse to the leftmost node in the right subtree */
            NodeToRemove = NodeToRemove->Left;
        }

        /* Cache the original color of the successor node */
        RemoveNodeColor = GetNodeColor(NodeToRemove);
        Replacement = NodeToRemove->Right;
        ReplacementParent = GetParentNode(NodeToRemove);

        /* Check fif the successor is not the direct right child of the node being removed */
        if(NodeToRemove != Node->Right)
        {
            /* Detach the successor from its original parent and link its right child */
            ReplacementParent->Left = Replacement;
            if(Replacement != NULLPTR)
            {
                /* Link the successor's right child to the successor's parent */
                SetParentNode(Replacement, ReplacementParent);
            }

            /* Attach the original node's right subtree to the successor */
            NodeToRemove->Right = Node->Right;
            SetParentNode(NodeToRemove->Right, NodeToRemove);
        }
        else
        {
            /* Set the replacement parent as the successor itself */
            ReplacementParent = NodeToRemove;
        }

        /* Retrieve the parent of the node being removed to link it to the successor */
        ParentNode = GetParentNode(Node);
        if(ParentNode == NULLPTR)
        {
            /* The successor becomes the new root */
            Tree->Root = NodeToRemove;
        }
        else if(Node == ParentNode->Left)
        {
            /* Link the successor to the left side of the original parent */
            ParentNode->Left = NodeToRemove;
        }
        else
        {
            /* Link the successor to the right side of the original parent */
            ParentNode->Right = NodeToRemove;
        }

        /* Update the parent pointer of the successor to its new parent */
        SetParentNode(NodeToRemove, ParentNode);
        NodeToRemove->Left = Node->Left;

        /* Update the parent pointer of the left child and copy the original node's color */
        SetParentNode(NodeToRemove->Left, NodeToRemove);
        CopyNodeColor(NodeToRemove, Node);
    }

    /* Check if the physically removed node was black */
    if(RemoveNodeColor == NodeBlack && Tree->Root != NULLPTR)
    {
        /* Fix up the tree properties */
        CurrentNode = Replacement;
        ParentNode = ReplacementParent;

        /* Traverse up the tree */
        while(CurrentNode != Tree->Root &&
              (CurrentNode == NULLPTR || GetNodeColor(CurrentNode) == NodeBlack))
        {
            /* Check if the current node is a left child */
            if(CurrentNode == ParentNode->Left)
            {
                /* Identify the sibling node on the right */
                SiblingNode = ParentNode->Right;

                /* Check if the sibling is red */
                if(GetNodeColor(SiblingNode) == NodeRed)
                {
                    /* Recolor the sibling to black */
                    SetNodeColor(SiblingNode, NodeBlack);
                    SetNodeColor(ParentNode, NodeRed);

                    /* Perform a left rotation around the parent */
                    RotateLeft(Tree, ParentNode);
                    SiblingNode = ParentNode->Right;
                }

                /* Check if both children of the sibling are black */
                if((SiblingNode->Left == NULLPTR || GetNodeColor(SiblingNode->Left) == NodeBlack) &&
                   (SiblingNode->Right == NULLPTR || GetNodeColor(SiblingNode->Right) == NodeBlack))
                {
                    /* Set the sibling color to red to restore black-height */
                    SetNodeColor(SiblingNode, NodeRed);

                    /* Move the current pointer up to the parent and continue checking */
                    CurrentNode = ParentNode;
                    ParentNode = GetParentNode(CurrentNode);
                }
                else
                {
                    /* Check if the sibling's right child is black */
                    if(SiblingNode->Right == NULLPTR || GetNodeColor(SiblingNode->Right) == NodeBlack)
                    {
                        /* Check if the left child needs updating */
                        if(SiblingNode->Left != NULLPTR)
                        {
                            /* Recolor the sibling's left child */
                            SetNodeColor(SiblingNode->Left, NodeBlack);
                        }

                        /* Set the sibling to red and perform the right rotation */
                        SetNodeColor(SiblingNode, NodeRed);
                        RotateRight(Tree, SiblingNode);
                        SiblingNode = ParentNode->Right;
                    }

                    /* Swap colors between sibling and parent and recolor right child to black */
                    CopyNodeColor(SiblingNode, ParentNode);
                    SetNodeColor(ParentNode, NodeBlack);

                    /* Check if the right child needs updating */
                    if(SiblingNode->Right != NULLPTR)
                    {
                        /* Recolor the sibling's right child */
                        SetNodeColor(SiblingNode->Right, NodeBlack);
                    }

                    /* Perform a left rotation around the parent to restore balance */
                    RotateLeft(Tree, ParentNode);
                    CurrentNode = Tree->Root;

                    /* The tree properties are restored, terminate the loop */
                    break;
                }
            }
            else
            {
                /* Process the case where the current node is a right child */
                SiblingNode = ParentNode->Left;

                /* Check if the sibling is red */
                if(GetNodeColor(SiblingNode) == NodeRed)
                {
                    /* Recolor the sibling to black */
                    SetNodeColor(SiblingNode, NodeBlack);
                    SetNodeColor(ParentNode, NodeRed);

                    /* Perform a right rotation around the parent */
                    RotateRight(Tree, ParentNode);
                    SiblingNode = ParentNode->Left;
                }

                /* Check if both children of the sibling are black */
                if((SiblingNode->Right == NULLPTR || GetNodeColor(SiblingNode->Right) == NodeBlack) &&
                   (SiblingNode->Left == NULLPTR || GetNodeColor(SiblingNode->Left) == NodeBlack))
                {
                    /* Set the sibling color to red to restore black-height */
                    SetNodeColor(SiblingNode, NodeRed);
                    CurrentNode = ParentNode;
                    ParentNode = GetParentNode(CurrentNode);
                }
                else
                {
                    /* Check if the sibling's left child is black */
                    if(SiblingNode->Left == NULLPTR || GetNodeColor(SiblingNode->Left) == NodeBlack)
                    {
                        /* Check if the right child needs updating */
                        if(SiblingNode->Right != NULLPTR)
                        {
                            /* Recolor the sibling's right child */
                            SetNodeColor(SiblingNode->Right, NodeBlack);
                        }

                        /* Set the sibling to red and perform the right rotation */
                        SetNodeColor(SiblingNode, NodeRed);
                        RotateLeft(Tree, SiblingNode);
                        SiblingNode = ParentNode->Left;
                    }

                    /* Swap colors between sibling and parent and recolor right child to black */
                    CopyNodeColor(SiblingNode, ParentNode);
                    SetNodeColor(ParentNode, NodeBlack);

                    /* Check if the left child needs updating */
                    if(SiblingNode->Left != NULLPTR)
                    {
                        /* Recolor the sibling's left child */
                        SetNodeColor(SiblingNode->Left, NodeBlack);
                    }

                    /* Perform a right rotation around the parent */
                    RotateRight(Tree, ParentNode);
                    CurrentNode = Tree->Root;

                    /* The tree properties are restored, terminate the loop */
                    break;
                }
            }
        }

        /* Check if the current node is not null */
        if(CurrentNode != NULLPTR)
        {
            /* Force the current node to be black to satisfy the red-black rules */
            SetNodeColor(CurrentNode, NodeBlack);
        }
    }
}

/**
 * Performs a left rotation around the specified node to maintain red-black tree balance.
 *
 * @param Tree
 *        Supplies a pointer to the RTL_RB_TREE structure containing the node.
 *
 * @param Node
 *        Supplies a pointer to the balanced node around which to perform the left rotation.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
RTL::RedBlackTree::RotateLeft(IN PRTL_RB_TREE Tree,
                              IN PRTL_BALANCED_NODE Node)
{
    PRTL_BALANCED_NODE ParentNode, RightNode;

    /* Establish the right child as the pivot node for the rotation */
    RightNode = Node->Right;
    Node->Right = RightNode->Left;

    /* Transfer the left subtree of the pivot to the right subtree of the node */
    if(RightNode->Left != NULLPTR)
    {
        /* Update the parent pointer */
        SetParentNode(RightNode->Left, Node);
    }

    /* Update the parent */
    ParentNode = GetParentNode(Node);
    SetParentNode(RightNode, ParentNode);

    /* Link the parent of the original node to the new pivot node */
    if(ParentNode == NULLPTR)
    {
        /* Set the pivot new root */
        Tree->Root = RightNode;
    }
    else if(Node == ParentNode->Left)
    {
        /* Update the left child pointer of the parent */
        ParentNode->Left = RightNode;
    }
    else
    {
        /* Update the right child pointer of the parent */
        ParentNode->Right = RightNode;
    }

    /* Place the original node as the left child of the pivot */
    RightNode->Left = Node;
    SetParentNode(Node, RightNode);
}

/**
 * Performs a right rotation around the specified node to maintain red-black tree balance.
 *
 * @param Tree
 *        Supplies a pointer to the RTL_RB_TREE structure containing the node.
 *
 * @param Node
 *        Supplies a pointer to the balanced node around which to perform the right rotation.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
RTL::RedBlackTree::RotateRight(IN PRTL_RB_TREE Tree,
                               IN PRTL_BALANCED_NODE Node)
{
    PRTL_BALANCED_NODE LeftNode, ParentNode;

    /* Establish the left child as the pivot node for the rotation */
    LeftNode = Node->Left;
    Node->Left = LeftNode->Right;

    /* Transfer the right subtree of the pivot to the left subtree of the node */
    if(LeftNode->Right != NULLPTR)
    {
        /* Update the parent pointer */
        SetParentNode(LeftNode->Right, Node);
    }

    /* Update the parent */
    ParentNode = GetParentNode(Node);
    SetParentNode(LeftNode, ParentNode);

    /* Link the parent of the original node to the new pivot node */
    if(ParentNode == NULLPTR)
    {
        /* Set the pivot new root */
        Tree->Root = LeftNode;
    }
    else if(Node == ParentNode->Right)
    {
        /* Update the right child pointer of the parent */
        ParentNode->Right = LeftNode;
    }
    else
    {
        /* Update the left child pointer of the parent */
        ParentNode->Left = LeftNode;
    }

    /* Place the original node as the right child of the pivot */
    LeftNode->Right = Node;
    SetParentNode(Node, LeftNode);
}

/**
 * Sets the node's color. Clears the color bit first, then applies the new color.
 *
 * @param Node
 *        Supplies a pointer to the balanced node for which to set the color.
 *
 * @param Color
 *        Specifies the new color to set for the node, either NodeRed or NodeBlack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
RTL::RedBlackTree::SetNodeColor(OUT PRTL_BALANCED_NODE Node,
                                IN RTL_BALANCED_NODE_COLOR Color)
{
    /* Set the node color */
    Node->ParentValue = (Node->ParentValue & ~RTL_BALANCED_NODE_COLOR_MASK) | Color;
}

/**
 * Sets the parent node pointer while preserving the current color bits.
 *
 * @param Node
 *        Supplies a pointer to the balanced node for which to set the parent pointer.
 *
 * @param Parent
 *        Supplies a pointer to the new parent node to be set for the given node.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
RTL::RedBlackTree::SetParentNode(OUT PRTL_BALANCED_NODE Node,
                                 IN PRTL_BALANCED_NODE Parent)
{
    /* Set the parent node pointer */
    Node->ParentValue = ((ULONG_PTR)Parent & ~RTL_BALANCED_NODE_RESERVED_PARENT_MASK) |
                        (Node->ParentValue & RTL_BALANCED_NODE_RESERVED_PARENT_MASK);
}
