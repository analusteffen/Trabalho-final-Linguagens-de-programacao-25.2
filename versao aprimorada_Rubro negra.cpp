#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// Constantes para cores
const bool RED = true;
const bool BLACK = false;

struct Node {
    double key;
    Node *left, *right;
    bool color; // true = Red, false = Black
    int size;   // Para cálculo de Mediana O(log N)

    Node(double k) : key(k), left(nullptr), right(nullptr), color(RED), size(1) {}
};

class SensorRedBlack {
private:
    Node* root;

    // --- Helpers de Propriedades ---
    bool isRed(Node* x) {
        if (x == nullptr) return false;
        return x->color == RED;
    }

    int size(Node* x) {
        if (x == nullptr) return 0;
        return x->size;
    }

    void updateSize(Node* x) {
        if (x != nullptr) {
            x->size = 1 + size(x->left) + size(x->right);
        }
    }

    // --- Rotações e Ajustes de Cores (A Mágica da LLRB) ---
    
    // Rotação à Esquerda (Usada quando temos link vermelho na direita)
    Node* rotateLeft(Node* h) {
        Node* x = h->right;
        h->right = x->left;
        x->left = h;
        x->color = h->color;
        h->color = RED;
        x->size = h->size;
        updateSize(h);
        return x;
    }

    // Rotação à Direita (Usada quando temos dois links vermelhos seguidos na esquerda)
    Node* rotateRight(Node* h) {
        Node* x = h->left;
        h->left = x->right;
        x->right = h;
        x->color = h->color;
        h->color = RED;
        x->size = h->size;
        updateSize(h);
        return x;
    }

    // Inversão de Cores (Quando ambos os filhos são vermelhos)
    void flipColors(Node* h) {
        h->color = !h->color;
        if (h->left) h->left->color = !h->left->color;
        if (h->right) h->right->color = !h->right->color;
    }

    // --- Inserção ---
    Node* insert(Node* h, double key) {
        if (h == nullptr) return new Node(key);

        if (key < h->key) h->left = insert(h->left, key);
        else h->right = insert(h->right, key); // Duplicatas vão p/ direita

        // A Lógica LLRB (Left-Leaning Red-Black)
        // 1. Se o filho direito é vermelho e o esquerdo é preto -> Rotaciona Esquerda
        if (isRed(h->right) && !isRed(h->left)) h = rotateLeft(h);
        
        // 2. Se o filho esquerdo e o neto esquerdo são vermelhos -> Rotaciona Direita
        if (isRed(h->left) && isRed(h->left->left)) h = rotateRight(h);
        
        // 3. Se ambos os filhos são vermelhos -> Inverte cores
        if (isRed(h->left) && isRed(h->right)) flipColors(h);

        updateSize(h);
        return h;
    }

    // --- Helpers de Busca e Remoção ---
    
    // Busca o K-ésimo menor (Para Mediana)
    double select(Node* x, int k) {
        if (x == nullptr) return -1.0;
        int t = size(x->left);
        if (t > k) return select(x->left, k);
        else if (t < k) return select(x->right, k - t - 1);
        else return x->key;
    }

    // Min/Max Helpers
    void getMinK(Node* node, int &k) {
        if (node == nullptr || k <= 0) return;
        getMinK(node->left, k);
        if (k > 0) { cout << node->key << " "; k--; }
        getMinK(node->right, k);
    }

    void getMaxK(Node* node, int &k) {
        if (node == nullptr || k <= 0) return;
        getMaxK(node->right, k);
        if (k > 0) { cout << node->key << " "; k--; }
        getMaxK(node->left, k);
    }

    void rangeQueryRec(Node* node, double minVal, double maxVal) {
        if (node == nullptr) return;
        if (minVal < node->key) rangeQueryRec(node->left, minVal, maxVal);
        if (node->key >= minVal && node->key <= maxVal) cout << node->key << " ";
        if (maxVal > node->key) rangeQueryRec(node->right, minVal, maxVal);
    }
    
    // Auxiliar para delete (Encontra o mínimo da subárvore direita)
    Node* minNode(Node* node) {
        Node* current = node;
        while (current->left != nullptr) current = current->left;
        return current;
    }
    
    // Remoção Simplificada (Standard BST Delete)
    // Nota: Implementar a remoção completa com rebalanceamento LLRB (moveRedLeft/Right) 
    // adicionaria cerca de 80 linhas de código complexo. Para fins acadêmicos de comparação
    // de inserção/leitura, a remoção BST padrão resolve, embora degrade levemente o balanceamento.
    Node* remove(Node* root, double key) {
        if (root == nullptr) return root;

        if (key < root->key) root->left = remove(root->left, key);
        else if (key > root->key) root->right = remove(root->right, key);
        else {
            if ((root->left == nullptr) || (root->right == nullptr)) {
                Node *temp = root->left ? root->left : root->right;
                if (temp == nullptr) { temp = root; root = nullptr; }
                else *root = *temp;
                delete temp;
            } else {
                Node* temp = minNode(root->right);
                root->key = temp->key;
                root->right = remove(root->right, temp->key);
            }
        }
        if (root != nullptr) updateSize(root);
        return root;
    }

    void inOrder(Node* x) {
        if (x == nullptr) return;
        inOrder(x->left);
        cout << x->key << " ";
        inOrder(x->right);
    }

public:
    SensorRedBlack() : root(nullptr) {}

    void insert(double value) {
        root = insert(root, value);
        root->color = BLACK; // A raiz é sempre preta
    }

    void remove(double value) {
        root = remove(root, value);
        if (root) root->color = BLACK;
        cout << "[Remove] " << value << endl;
    }

    void printSorted() {
        cout << "RB Tree Ordenada: ";
        inOrder(root);
        cout << endl;
    }

    void getMinMax(int k) {
        cout << "--- Extremos (" << k << ") ---" << endl;
        int c = k; cout << "Minimos: "; getMinK(root, c); cout << endl;
        c = k; cout << "Maximos: "; getMaxK(root, c); cout << endl;
    }

    void rangeQuery(double minVal, double maxVal) {
        cout << "--- Consulta Intervalo [" << minVal << " a " << maxVal << "] ---" << endl;
        cout << "Resultados: ";
        rangeQueryRec(root, minVal, maxVal);
        cout << endl;
    }

    double median() {
        if (root == nullptr) return 0.0;
        int n = size(root);
        if (n % 2 != 0) {
            return select(root, n / 2); // 0-indexed logic
        } else {
            return (select(root, n / 2 - 1) + select(root, n / 2)) / 2.0;
        }
    }
};

int main() {
    SensorRedBlack rb;
    
    cout << "=== TESTE VERSAO APRIMORADA (RED-BLACK TREE) ===\n" << endl;

    // 1. Inserção (Verifica auto-balanceamento)
    // Inserindo em ordem crescente (pior caso para BST, trivial para RB)
    rb.insert(10.0);
    rb.insert(20.0);
    rb.insert(30.0);
    rb.insert(40.0);
    rb.insert(50.0);
    rb.insert(25.0);

    rb.printSorted();

    // 2. Mediana O(log N)
    // Dados: 10, 20, 25, 30, 40, 50. Mediana esperada: (25+30)/2 = 27.5
    cout << "Mediana: " << rb.median() << endl;

    // 3. Range Query
    rb.rangeQuery(15.0, 35.0); // Esperado: 20, 25, 30

    // 4. Min/Max
    rb.getMinMax(2);

    // 5. Remoção
    rb.remove(25.0);
    rb.printSorted();
    cout << "Nova Mediana: " << rb.median() << endl;

    return 0;
}