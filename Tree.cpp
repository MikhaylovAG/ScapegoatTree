#include "tree/Tree.hpp"

#include <numeric>

struct Tree::Node {
    int value;
    int size{1};
    Node* parent{};
    Node* left{};
    Node* right{};

    explicit Node(int const value) : value{value} {}
    explicit Node(int const value, int size) : value{value}, size{size} {}
};

[[nodiscard]] bool Tree::contains(int value) const noexcept {
    Node* node{find(value)};

    return node != nullptr;
}

bool Tree::insert(int value) {
    if (root_ == nullptr) {
        root_ = new Node(value);
        return true;
    }

    Node* curr = root_;
    while (true) {
        if (curr->value != value) {
            Node*& next = curr->value < value ? curr->right : curr->left;
            if (next == nullptr) {
                Node* newNode   = new Node(value);
                next            = newNode;
                newNode->parent = curr;
                break;
            }

            curr = next;
        } else {
            return false;
        }
    }

    updateSizes(curr, 1);
    killGoat(curr);

    return true;
}

bool Tree::remove(int value) {
    Node* del = find(value);

    if (del == nullptr) {
        return false;
    }

    if (del->size == 1) {
        if (del->parent == nullptr) {
            root_ = nullptr;
        } else {
            Node* parent                                         = del->parent;
            (parent->left == del ? parent->left : parent->right) = nullptr;

            updateSizes(parent, -1);
            killGoat(parent);
        }

        delete del;

        return true;
    }

    if (del->left == nullptr || del->right == nullptr) {
        Node* child  = del->left != nullptr ? del->left : del->right;
        Node* parent = del->parent;
        if (parent != nullptr) {
            (parent->left == del ? parent->left : parent->right) = child;
        } else {
            root_ = child;
        }

        child->parent = parent;
        del->left     = nullptr;
        del->right    = nullptr;

        delete del;

        if (parent != nullptr) {
            updateSizes(parent, -1);
            killGoat2(parent);
        }

        return true;
    }

    Node* drop = del->right;
    while (drop->left != nullptr) {
        drop = drop->left;
    }

    del->value = drop->value;

    Node* dropParent = drop->parent;
    Node* dropChild  = drop->right;

    if (dropParent->left == drop) {
        dropParent->left = dropChild;
    } else {
        dropParent->right = dropChild;
    }

    if (dropChild != nullptr) {
        dropChild->parent = dropParent;
    }

    drop->left  = nullptr;
    drop->right = nullptr;
    delete drop;

    updateSizes(dropParent, -1);
    killGoat2(dropParent);

    return true;
}

[[nodiscard]] std::size_t Tree::size() const noexcept {
    return root_ == nullptr ? 0 : root_->size;
}
[[nodiscard]] bool Tree::empty() const noexcept {
    return root_ == nullptr;
}

[[nodiscard]] std::vector<int> Tree::values() const noexcept {
    return subtreeValues(root_);
}

std::vector<int> Tree::subtreeValues(Node* subroot_) const {
    if (subroot_ == nullptr) {
        return std::vector<int>{};
    }

    std::vector<int> res(subroot_->size);
    int i = 0;
    collectValues(subroot_, res, i);

    return res;
}

void Tree::collectValues(Tree::Node* subroot_, std::vector<int>& res, int& i) const {
    if (subroot_ != nullptr) {
        collectValues(subroot_->left, res, i);
        res[i++] = subroot_->value;
        collectValues(subroot_->right, res, i);
    }
}

void Tree::updateSizes(Node* node, int delta) {
    while (node != nullptr) {
        node->size += delta;
        node = node->parent;
    }
}

Tree::Node* Tree::buildSubtree(std::vector<int>& values, Node* parent, std::size_t l, std::size_t r) {
    if (r == l) {
        return nullptr;
    }

    int m          = std::midpoint(l, r);
    Node* myRoot   = new Node(values[m], r - l);
    myRoot->parent = parent;
    myRoot->left   = buildSubtree(values, myRoot, l, m);
    myRoot->right  = buildSubtree(values, myRoot, m + 1, r);
    return myRoot;
}

Tree::Node* Tree::find(int value) const {
    Node* curr = root_;
    while (curr != nullptr) {
        if (curr->value > value) {
            curr = curr->left;
        } else if (curr->value < value) {
            curr = curr->right;
        } else {
            return curr;
        }
    }

    return nullptr;
}

void Tree::killGoat(Node* node) {
    Node* scapegoat = nullptr;
    Node* curr      = node;

    while (curr->parent != nullptr) {
        if (static_cast<float>(curr->size) > alpha * static_cast<float>(curr->parent->size)) {
            scapegoat = curr->parent;
        }

        curr = curr->parent;
    }

    if (scapegoat != nullptr) {
        rebuildSubtree(scapegoat);
    }
}

void Tree::killGoat2(Node* node) {
    Node* scapegoat = nullptr;
    Node* curr      = node;

    while (curr != nullptr) {
        if (curr->left != nullptr && static_cast<float>(curr->left->size) > alpha * static_cast<float>(curr->size)) {
            scapegoat = curr;
        }

        if (curr->right != nullptr && static_cast<float>(curr->right->size) > alpha * static_cast<float>(curr->size)) {
            scapegoat = curr;
        }

        curr = curr->parent;
    }

    if (scapegoat != nullptr) {
        rebuildSubtree(scapegoat);
    }
}

void Tree::rebuildSubtree(Node* scapegoat) {
    if (scapegoat == nullptr)
        return;

    std::vector<int> values = subtreeValues(scapegoat);
    Node* parent            = scapegoat->parent;
    bool isLeft             = (parent != nullptr && parent->left == scapegoat);

    if (parent != nullptr) {
        if (parent->left == scapegoat) {
            parent->left = nullptr;
        } else {
            parent->right = nullptr;
        }
    } else {
        root_ = nullptr;
    }

    deleteSubtree(scapegoat);

    Node* newNode = buildSubtree(values, parent, 0, values.size());

    if (parent == nullptr) {
        root_ = newNode;
    } else if (isLeft) {
        parent->left = newNode;
    } else {
        parent->right = newNode;
    }
}

void Tree::deleteSubtree(Node* node) {
    if (node != nullptr) {
        deleteSubtree(node->left);
        deleteSubtree(node->right);
        delete node;
    }
}

Tree::~Tree() {
    deleteSubtree(root_);
}