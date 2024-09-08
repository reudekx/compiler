#include "token.h"

#include <stdlib.h>
#include <iostream>

namespace {

struct TrieNode {
    char ch;
    Token::Type type;
    TrieNode* right;
    TrieNode* child;
    
    constexpr TrieNode() : ch('\0'), type(Token::Type::IDENTIFIER), right(nullptr), child(nullptr) {}
    constexpr TrieNode(char ch, Token::Type type, TrieNode *right, TrieNode *child) : ch(ch), type(type), right(right), child(child) {}
};

struct Trie {
    TrieNode root[128];

    constexpr Trie() : root() {
        for (int i = 0; i < 128; i++) {
            root[i].ch = (char)i;
            root[i].type = Token::Type::IDENTIFIER;
        }
    }
};

constexpr int len(const char *string) {
    int index = 0;
    while (string[index] != '\0') {
        index++;
    }
    return index;
}

constexpr Trie* build_trie() {
    Trie* trie = new Trie;
    for (int i = 0; i < (int)Token::Type::Count; i++) {
        if (Token::String[i][0] == '#') {
            continue;
        }
        TrieNode* parent = &trie->root[Token::String[i][0]];
        TrieNode* cur = parent->child;
        TrieNode* prev = parent;
        for (int j = 1; j < len(Token::String[i]); j++) {
            char ch = Token::String[i][j];
            if (prev->type == Token::Type::IDENTIFIER) {
                prev->type = Token::Type::PART;
            }
            while (cur != nullptr) {
                if (ch == cur->ch) {
                    break; // break시 cur != nullptr
                }
                prev = cur;
                cur = cur->right;
            }
            if (cur == nullptr) { // ch에 대한 기존 노드를 못 찾은 경우
                TrieNode* next = new TrieNode(ch, Token::Type::IDENTIFIER, nullptr, nullptr);
                if (prev == parent) { // 아예 탐색이 이뤄지지 않은 경우
                    prev->child = next;
                }
                else { // 탐색은 했으나 찾지 못 한 경우
                    prev->right = next;
                }
                parent = next;
                prev = next;
            }
            else { // 노드를 찾은 경우
                parent = cur;
                prev = cur;
            }
            cur = parent->child;
        }
        prev->type = (Token::Type)i;
    }
    return trie;
}

const Trie* trie = build_trie();

void print_sub_trie(const TrieNode* node, int depth) {
    if (node == nullptr) return;

    // 현재 노드 출력
    for (int i = 0; i < depth; i++) std::cout << "  ";
    std::cout << "'" << node->ch << "' ";
    if (node->type != Token::Type::IDENTIFIER) {
        std::cout << "(" << Token::to_string(node->type) << ")";
    }
    std::cout << std::endl;

    // 자식 노드 출력
    print_sub_trie(node->child, depth + 1);

    // 형제 노드 출력
    print_sub_trie(node->right, depth);
}

void print_all_trie() {
    std::cout << "Trie structure:" << std::endl;
    for (int i = 0; i <128; i++) {
        print_sub_trie(&trie->root[i], 0);
    }
}

}

void Token::print_trie() {
    print_all_trie();
}

// 최초의 문자가 최소한 아스키 문자에 포함되는지의 여부는 외부에서 검사
// 또한 IDENTIFIER 반환 시 다음 호출 시 init 여부도 외부에서 제어
Token::Type Token::check_type(char ch, bool init) {
    static const TrieNode *cur = nullptr;
    static bool is_id = false;
    if (init) {
        is_id = false;
        cur = &trie->root[(int)ch];
        return cur->type;
    }
    else if (is_id) {
        return Token::Type::IDENTIFIER;
    }
    const TrieNode* next = cur->child;
    while (next != nullptr) {
        if (ch == next->ch) {
            break;
        }
        next = next->right;
    }
    if (next == nullptr) {
        is_id = true;
        return Token::Type::IDENTIFIER;
    }
    cur = next;
    return cur->type;
}