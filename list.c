// Author: Prince Emmanuel Nitafan
// Student #: 301 445 340 

#include "list.h"

static List emptyHeads[LIST_MAX_NUM_HEADS];
static List* currEmptyHead = NULL; // This will point to the next empty head in the pool

static Node emptyNodes[LIST_MAX_NUM_NODES];
static Node* currEmptyNode = NULL; // This will point to the next empty node in the pool

static int start = 0; // This will let List_create() know if it is the first call or not

////////////////////////////////////////////////////////////////////////////// done

List* List_create() {
    // this section only runs once for the first createList() call
    if (start == 0) {
        start = 1;
        // setting up the data structure for list heads
        for (int i = 0; i< LIST_MAX_NUM_HEADS-1; i++) {
            emptyHeads[i].l_next = &emptyHeads[i+1];
            emptyHeads[i+1].l_next = NULL;
        }
        currEmptyHead = &emptyHeads[0];

        // setting up the data structure for nodes
        for (int i = 0; i < LIST_MAX_NUM_NODES-1; i++) {
            emptyNodes[i].n_next = &emptyNodes[i+1];
            emptyNodes[i+1].n_next = NULL;
        }
        currEmptyNode = &emptyNodes[0];
    }

    // If there are no empty lists, return NULL
    if(currEmptyHead == NULL) {
        return NULL;
    }

    List* newList = currEmptyHead;

    // Move EmptyHead index to next empty head
    currEmptyHead = currEmptyHead->l_next;

    newList->head_node = NULL;
    newList->tail_node = NULL;
    newList->OOBvar = LIST_OOB_START;
    newList->item_count = 0;
    newList->l_next = NULL;

    return newList;
}

////////////////////////////////////////////////////////////////////////////// done

int List_count(List* pList){
    return pList->item_count;
}

////////////////////////////////////////////////////////////////////////////// done

void* List_first(List* pList) {
    if (List_count(pList) == 0) {
        return NULL;
    } else {
        pList->curr_node = pList->head_node;
        pList->OOBvar = 2;
        return pList->curr_node->item;
    }
}

////////////////////////////////////////////////////////////////////////////// done

void* List_last(List* pList) {
    if (List_count(pList) == 0){
        return NULL;
    } else {
        pList->curr_node = pList->tail_node;
        pList->OOBvar = 2;
        return pList->curr_node->item;
    }
}

////////////////////////////////////////////////////////////////////////////// done

void* List_next(List* pList) {
    // if there is no elements in the list
    if (List_count(pList) == 0) {
        pList->OOBvar = LIST_OOB_END;
        return NULL;
    }

    // if out of bounds from the start
    if (pList->OOBvar == LIST_OOB_START) {
        pList->OOBvar = 2;
        return pList->curr_node->item;
    }

    // if advanced past list
    if (pList->curr_node->n_next == NULL) {
        pList->OOBvar = LIST_OOB_END;
        return NULL;
    }

    pList->curr_node = pList->curr_node->n_next; 
    return pList->curr_node->item;
}

////////////////////////////////////////////////////////////////////////////// done

void* List_prev(List* pList) {
    // if there is no elements in the list
    if (List_count(pList) == 0) {
        pList->OOBvar = LIST_OOB_START;
        return NULL;
    }

    // if out of bounds from end
    if (pList->OOBvar == LIST_OOB_END) {
        pList->OOBvar = 2;
        return pList->curr_node->item;
    }

    // if backed up past list
    if (pList->curr_node->n_prev == NULL) {
        pList->OOBvar = LIST_OOB_START;
        return NULL;
    }

    pList->curr_node = pList->curr_node->n_prev; 
    return pList->curr_node->item;
}

////////////////////////////////////////////////////////////////////////////// done

void* List_curr(List* pList) {
    if (List_count(pList) == 0 || pList->OOBvar == LIST_OOB_START || pList->OOBvar == LIST_OOB_END) {
        return NULL;
    }

    return pList->curr_node->item;
}

////////////////////////////////////////////////////////////////////////////// done

int List_insert_after(List* pList, void* pItem) {
    // Check if there is an empty node available
    if (currEmptyNode != NULL) {

        Node* temp = currEmptyNode;
        temp->item = pItem;
        currEmptyNode = currEmptyNode->n_next;
        temp->n_next = NULL; temp->n_prev = NULL;

        // if pList is empty, new node becomes the head, tail, current
        if (List_count(pList) == 0) {
            pList->head_node = temp;
            pList->tail_node = temp;

        // if the current node is out of bounds, add new node at the start    
        } else if (pList->OOBvar == LIST_OOB_START) {
            pList->head_node->n_prev = temp;
            temp->n_next = pList->head_node;
            pList->head_node = temp;
 
        // if the current node is out of bounds or current node is at the end, add new node at the end    
        } else if (pList->OOBvar == LIST_OOB_END || pList->curr_node->n_next == NULL) {
            pList->tail_node->n_next = temp;
            temp->n_prev = pList->tail_node;
            pList->tail_node = temp;

        // if the new node will be in the middle of 2 nodes 
        } else {
            Node* lTemp = pList->curr_node;
            Node* rTemp = pList->curr_node->n_next; 
            lTemp->n_next = temp; temp->n_prev = lTemp;
            rTemp->n_prev = temp; temp->n_next = rTemp;
        }

        pList->curr_node = temp;
        pList->OOBvar = 2;

        pList->item_count +=1;

        // success
        return LIST_SUCCESS;
    }

    // fail
    return LIST_FAIL;
}

////////////////////////////////////////////////////////////////////////////// done

int List_insert_before(List* pList, void* pItem) {

    // Check if there's an empty node available
    if (currEmptyNode != NULL) { 

        Node* temp = currEmptyNode;
        temp->item = pItem;
        currEmptyNode = currEmptyNode->n_next;
        temp->n_next = NULL; temp->n_prev = NULL;

       // if pList is empty, new node becomes the head, tail, current node
        if (List_count(pList) == 0) {
            pList->head_node = temp;
            pList->tail_node = temp;

        // if the current node is out of bounds or current node is at the start, add new node at the start    
        } else if (pList->OOBvar == LIST_OOB_START || pList->curr_node->n_prev == NULL) {
            pList->head_node->n_prev = temp;
            temp->n_next = pList->head_node;
            pList->head_node = temp;

        // if the current node is out of bounds add new node at the end    
        } else if (pList->OOBvar == LIST_OOB_END) {
            pList->tail_node->n_next = temp;
            temp->n_prev = pList->tail_node;
            pList->tail_node = temp;

        // if the new node will be in the middle of 2 nodes 
        } else {
            Node* rTemp = pList->curr_node; 
            Node* lTemp = pList->curr_node->n_prev;
            rTemp->n_prev = temp; temp->n_next = rTemp;
            lTemp->n_next = temp; temp->n_prev = lTemp;
        }

        pList->curr_node = temp;
        pList->OOBvar = 2;

        pList->item_count +=1;

        // success
        return LIST_SUCCESS;
    }
    // fail
    return LIST_FAIL;
}

////////////////////////////////////////////////////////////////////////////// done

int List_append(List* pList, void* pItem) {
    //Check if there is an empty node available
    if (currEmptyNode != NULL) {

        Node* temp = currEmptyNode; 
        temp->item = pItem;
        currEmptyNode = currEmptyNode->n_next;
        temp->n_next = NULL; temp->n_prev = NULL;

        // if pList is empty, new node becomes the head, tail, current
        if (List_count(pList) == 0){
            pList->head_node = temp;
            pList->tail_node = temp;

        // regular append if pList is not empty
        } else {
            pList->tail_node->n_next = temp;
            temp->n_prev = pList->tail_node;
            pList->tail_node = temp;
        }

        pList->curr_node = temp;
        pList->OOBvar = 2;

        pList->item_count += 1;

        //success
        return LIST_SUCCESS;
        
    } 
    // fail
    return LIST_FAIL;
}

////////////////////////////////////////////////////////////////////////////// done

int List_prepend(List* pList, void* pItem) {
    if (currEmptyNode != NULL) {

        Node* temp = currEmptyNode;
        temp->item = pItem;
        currEmptyNode = currEmptyNode->n_next;
        temp->n_next = NULL; temp->n_prev = NULL;

        // if pList is empty, new node becomes the head, tail, current
        if (List_count(pList) == 0){
            pList->head_node = temp;
            pList->tail_node = temp;

        // regular prepend if list is not empty
        } else {
            pList->head_node->n_prev = temp;
            temp->n_next = pList->head_node;
            pList->head_node = temp;
        }

        pList->curr_node = temp;
        pList->OOBvar = 2;

        pList-> item_count +=1;

        // success
        return LIST_SUCCESS;
    } 
    // fail
    return LIST_FAIL;
}

////////////////////////////////////////////////////////////////////////////// Done

void* List_remove(List* pList) {
    
    // Check if the list is empty or out of bounds
    if (List_count(pList) == 0 || pList->OOBvar == LIST_OOB_START || pList->OOBvar == LIST_OOB_END) {
        return NULL;
    }

    void* retItem = pList->curr_node->item;
    Node* removeNode = pList->curr_node;

    // Check if the current node is the only node of the list
    if (List_count(pList) == 1) {
        pList->head_node = NULL;
        pList->tail_node = NULL;
        pList->curr_node = NULL;
        pList->OOBvar = LIST_OOB_END;

    // Check if the current node is the head
    } else if (pList->curr_node->n_prev == NULL && pList->curr_node->n_next != NULL) {
        pList->head_node = removeNode->n_next;
        pList->curr_node = pList->head_node;
        pList->head_node->n_prev = NULL;

    // Check if the current node is the tail
    } else if (pList->curr_node->n_next == NULL && pList->curr_node->n_prev != NULL) {
        pList->tail_node = removeNode->n_prev;
        pList->curr_node = pList->tail_node;
        pList->tail_node->n_next = NULL;
        pList->OOBvar = LIST_OOB_END;

    // if current node is between 2 nodes
    } else {
        Node* rTemp = removeNode->n_next;
        Node* lTemp = removeNode->n_prev;
        rTemp->n_prev = lTemp;
        lTemp->n_next = rTemp;
        pList->curr_node = rTemp;
    }

    removeNode->n_next = currEmptyNode;
    removeNode->n_prev = NULL;
    currEmptyNode = removeNode;

    pList->item_count -= 1;

    return retItem;
}

////////////////////////////////////////////////////////////////////////////// Done

void* List_trim(List* pList) {

    // Check if the list is empty
    if (List_count(pList) == 0) {
        return NULL;
    }

    void* retItem = pList->tail_node->item;
    Node* removeNode = pList->tail_node;

    // If there is only one node in the list, then will have zero nodes
    if (List_count(pList) == 1) {
        pList->head_node = NULL;
        pList->tail_node = NULL;
        pList->curr_node = NULL;
        pList->OOBvar = LIST_OOB_START;

    // regular tail node removal
    } else {
        pList->tail_node = removeNode->n_prev;
        pList->tail_node->n_next = NULL;
        pList->curr_node = pList->tail_node;
        pList->OOBvar = 2;
    }

    // removes the former tail node and return the node back to the empty Nodes pool
    removeNode->n_next = currEmptyNode;
    removeNode->n_prev = NULL;
    currEmptyNode = removeNode;

    pList->item_count -= 1;

    return retItem;
}

////////////////////////////////////////////////////////////////////////////// Done

void List_concat(List* pList1, List* pList2) {
    
    // if both lists are empty or just pList2 is empty
    if ((List_count(pList1) == 0 && List_count(pList2) == 0) || List_count(pList2) == 0) {
        pList2->head_node = NULL;
        pList2->tail_node = NULL;
        pList2->curr_node = NULL;
        pList2->OOBvar = 2;
        pList2->item_count = 0;

        pList2->l_next = currEmptyHead;
        currEmptyHead = pList2;

        return;

    // if the pList1 is empty while pList2 is not empty
    } else if (List_count(pList1) == 0 && List_count(pList2) > 0) {
        pList1->head_node = pList2->head_node;
        pList1->tail_node = pList2->tail_node;
        pList1->curr_node = pList1->head_node;
        pList1->OOBvar = LIST_OOB_START;
        pList1->item_count = pList2->item_count;

    // if pList1 is out of bounds on the right
    } else if (pList1->OOBvar == LIST_OOB_END) {
        pList1->tail_node->n_next = pList2->head_node;
        pList2->head_node->n_prev = pList1->tail_node;
        pList1->tail_node = pList2->tail_node;
        pList1->curr_node = pList1->tail_node;
        pList1->item_count += pList2->item_count;

    // regular concat
    } else {
        pList1->tail_node->n_next = pList2->head_node;
        pList2->head_node->n_prev = pList1->tail_node;
        pList1->tail_node = pList2->tail_node;
        pList1->item_count += pList2->item_count;
    }

    // removes the former pList2 and return pList2 back to the empty List heads pool
    pList2->head_node = NULL;
    pList2->tail_node = NULL;
    pList2->curr_node = NULL;
    pList2->OOBvar = 2;
    pList2->item_count = 0;

    pList2->l_next = currEmptyHead;
    currEmptyHead = pList2;
}

////////////////////////////////////////////////////////////////////////////// Done


void List_free(List* pList, FREE_FN pItemFreeFn) {

    // if the list is empty, then just return the list to the pool of empty heads
    if (List_count(pList) == 0){
        pList->OOBvar = 2;
        pList->l_next = currEmptyHead;
        currEmptyHead = pList;

    // if list is not empty, remove each node (and its item) and return them back to the empty node pool
    // and remove the list and return it back to the empty head pool
    } else {
        Node* removeNode = pList->head_node;
        Node* tempNode;
        while (removeNode != NULL) {

            tempNode = removeNode->n_next;

            // deletes the item of the node
            (*pItemFreeFn)(removeNode->item);

            // removes the node and returns it back to the empty nodes pool
            removeNode->n_next = currEmptyNode;
            removeNode->n_prev = NULL;
            removeNode->item = NULL;
            currEmptyNode = removeNode;

            removeNode = tempNode;
        }

        pList->head_node = NULL;
        pList->tail_node = NULL;
        pList->curr_node = NULL;
        pList->OOBvar = 2;
        pList->item_count = 0;

        pList->l_next = currEmptyHead;
        currEmptyHead = pList;
    }
}

////////////////////////////////////////////////////////////////////////////// Done

void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg) {
    // if the list is empty, then it is out of bounds end
    if (List_count(pList) == 0 || pList->OOBvar == LIST_OOB_END) {
        pList->OOBvar = LIST_OOB_END;
        return NULL;

    // if the list is not empty nor out of bounds end, then search from curr_node till tail node
    // if no match found, pList will be out of bounds end and return NULL
    } else {
        Node* temp = pList->curr_node;
        void* retItem = NULL;

        while (temp != NULL) {
            retItem = temp->item;

            if (pComparator(retItem, pComparisonArg) == 1) {
                pList->curr_node = temp;
                pList->OOBvar = 2;
                return retItem;
            }

            temp = temp->n_next;
        }

        // Since no match was found, setting the pList to out of bounds end, and returning NULL
        retItem = NULL;
        pList->curr_node = pList->tail_node;
        pList->OOBvar = LIST_OOB_END;

        return retItem;
    }
}

