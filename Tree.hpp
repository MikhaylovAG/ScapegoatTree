#ifndef TREE_HPP
#define TREE_HPP

#include <cstddef>
#include <vector>

class Tree {
public:
    [[nodiscard]] bool contains(int value) const noexcept;
    [[nodiscard]] bool insert(int value);
    [[nodiscard]] bool remove(int value);

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept;

    [[nodiscard]] std::vector<int> values() const noexcept;

    Tree() = default;
    ~Tree();

    Tree(Tree const&)            = delete;
    Tree& operator=(Tree const&) = delete;
    Tree(Tree&&)                 = delete;
    Tree& operator=(Tree&&)      = delete;

private:
    struct Node;

    void collectValues(Tree::Node* subroot_, std::vector<int>& res, int& i) const;
    [[nodiscard]] std::vector<int> subtreeValues(Node* subroot_) const;
    void updateSizes(Node* node, int delta);
    [[nodiscard]] Node* buildSubtree(std::vector<int>&, Node* parent, std::size_t l, std::size_t r);
    [[nodiscard]] Node* find(int value) const;
    void killGoat(Node* node);
    void killGoat2(Node* node);
    void rebuildSubtree(Node* scapegoat);
    void deleteSubtree(Node* node);

    Node* root_{};
    static constexpr float alpha = 0.8f;
};

#endif