#include <stddef.h>

#include "node.h"

Node make_node(const int type, const int parent) {
    Node node;
    node.type = type;
    node.parent = parent;
    node.len = 0;
    node.data = NULL;
    return node;
}