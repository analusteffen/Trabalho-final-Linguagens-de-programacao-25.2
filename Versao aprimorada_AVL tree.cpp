#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;

// --- Estrutura do Nó da AVL ---
struct Node {
    double key;      // A temperatura
    int height;      // Altura do nó (para balanceamento)
    int size;        // TAMANHO da subárvore (O segredo para a Mediana O(log N))
    Node *left;
    Node *right;

    Node(double k) : key(k), height(1), size(1), left(nullptr), right(nullptr) {}
};

class SensorAVL {
private:
    Node* root;

    // --- Funções Auxiliares da AVL ---

    // Retorna altura segura (trata NULL)
    int height(Node *n) {
        return (n == nullptr) ? 0 : n->height;
    }

    // Retorna tamanho seguro (trata NULL)
    int getSize(Node *n) {
        return (n == nullptr) ? 0 : n->size;
    }

    // Atualiza altura e tamanho do nó baseado nos filhos
    void update(Node *n) {
        if (n) {
            n->height = 1 + max(height(n->left), height(n->right));
            n->size = 1 + getSize(n->left) + getSize(n->right);
        }
    }

    // Fator de balanceamento
    int getBalance(Node *n) {
        return (n == nullptr) ? 0 : height(n->left) - height(n->right);
    }

    // Rotação à Direita
    Node* rightRotate(Node *y) {
        Node *x = y->left;
        Node *T2 = x->right;

        // Rotação
        x->right = y;
        y->left = T2;

        // Atualizar alturas e tamanhos (ordem importa: filhos primeiro)
        update(y);
        update(x);

        return x; // Nova raiz
    }

    // Rotação à Esquerda
    Node* leftRotate(Node *x) {
        Node *y = x->right;
        Node *T2 = y->left;

        // Rotação
        y->left = x;
        x->right = T2;

        // Atualizar alturas e tamanhos
        update(x);
        update(y);

        return y; // Nova raiz
    }

    // Insere recursivamente e balanceia
    Node* insert(Node* node, double key) {
        // 1. Inserção normal de BST
        if (node == nullptr)
            return new Node(key);

        if (key < node->key)
            node->left = insert(node->left, key);
        else
            node->right = insert(node->right, key); // Duplicatas vão para a direita

        // 2. Atualizar dados do nó atual
        update(node);

        // 3. Balancear
        int balance = getBalance(node);

        // Caso Esquerda-Esquerda
        if (balance > 1 && key < node->left->key)
            return rightRotate(node);

        // Caso Direita-Direita
        if (balance < -1 && key >= node->right->key)
            return leftRotate(node);

        // Caso Esquerda-Direita
        if (balance > 1 && key >= node->left->key) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }

        // Caso Direita-Esquerda
        if (balance < -1 && key < node->right->key) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }

    // Encontra o nó com menor valor (para remoção)
    Node* minValueNode(Node* node) {
        Node* current = node;
        while (current->left != nullptr)
            current = current->left;
        return current;
    }

    // Remove recursivamente e balanceia
    Node* remove(Node* root, double key) {
        if (root == nullptr) return root;

        // Navegação
        if (key < root->key)
            root->left = remove(root->left, key);
        else if (key > root->key)
            root->right = remove(root->right, key);
        else {
            // Achou o nó. Casos: 0 ou 1 filho
            if ((root->left == nullptr) || (root->right == nullptr)) {
                Node *temp = root->left ? root->left : root->right;

                if (temp == nullptr) { // Sem filhos
                    temp = root;
                    root = nullptr;
                } else { // 1 filho
                    *root = *temp; // Copia conteúdo
                }
                delete temp;
            } else {
                // Caso: 2 filhos. Pega o sucessor (menor da direita)
                Node* temp = minValueNode(root->right);
                root->key = temp->key;
                // Remove o sucessor
                root->right = remove(root->right, temp->key);
            }
        }

        if (root == nullptr) return root;

        // Atualizar e Balancear
        update(root);
        int balance = getBalance(root);

        // Balanceamento (mesma lógica da inserção, verificando fator dos filhos)
        if (balance > 1 && getBalance(root->left) >= 0)
            return rightRotate(root);

        if (balance > 1 && getBalance(root->left) < 0) {
            root->left = leftRotate(root->left);
            return rightRotate(root);
        }

        if (balance < -1 && getBalance(root->right) <= 0)
            return leftRotate(root);

        if (balance < -1 && getBalance(root->right) > 0) {
            root->right = rightRotate(root->right);
            return leftRotate(root);
        }

        return root;
    }

    // Busca o k-ésimo menor elemento (Order Statistic) - O(log N)
    double findKthSmallest(Node* node, int k) {
        if (node == nullptr) return -1.0;

        int leftSize = getSize(node->left);

        if (k == leftSize + 1)
            return node->key; // Achamos! É o atual.
        else if (k <= leftSize)
            return findKthSmallest(node->left, k); // Está na esquerda
        else
            return findKthSmallest(node->right, k - (leftSize + 1)); // Está na direita (ajusta indice)
    }

    // Helpers de impressão e query
    void inOrder(Node *root) {
        if (root != nullptr) {
            inOrder(root->left);
            cout << root->key << " | ";
            inOrder(root->right);
        }
    }

    void rangeQueryRec(Node* node, double minVal, double maxVal) {
        if (node == nullptr) return;

        // Se o nó atual for maior que minVal, pode haver relevantes à esquerda
        if (minVal < node->key)
            rangeQueryRec(node->left, minVal, maxVal);

        // Se estiver dentro do range, imprime
        if (node->key >= minVal && node->key <= maxVal)
            cout << node->key << " ";

        // Se o nó atual for menor que maxVal, pode haver relevantes à direita
        if (maxVal > node->key)
            rangeQueryRec(node->right, minVal, maxVal);
    }

    // Helpers para min/max
    void getMinK(Node* node, int &k) {
        if (node == nullptr || k <= 0) return;
        getMinK(node->left, k);
        if (k > 0) {
            cout << node->key << " ";
            k--;
        }
        getMinK(node->right, k);
    }
    
    void getMaxK(Node* node, int &k) {
        if (node == nullptr || k <= 0) return;
        getMaxK(node->right, k); // Visita direita primeiro (maiores)
        if (k > 0) {
            cout << node->key << " ";
            k--;
        }
        getMaxK(node->left, k);
    }

public:
    SensorAVL() : root(nullptr) {}

    // --- MÉTODOS PÚBLICOS SOLICITADOS ---

    void insert(double value) {
        root = insert(root, value);
        // cout << "[AVL Insert] " << value << endl; // Comentado p/ performance
    }

    void remove(double value) {
        // Verifica se existe antes de tentar remover (opcional, mas bom pra log)
        root = remove(root, value);
        cout << "[Remove] Tentativa de remover " << value << endl;
    }

    void printSorted() {
        cout << "AVL Ordenada: ";
        inOrder(root);
        cout << endl;
    }

    void getMinMax(int k) {
        cout << "--- Extremos (" << k << ") ---" << endl;
        int count = k;
        cout << "Minimos: ";
        getMinK(root, count);
        cout << endl;

        count = k;
        cout << "Maximos: ";
        getMaxK(root, count);
        cout << endl;
    }

    void rangeQuery(double minVal, double maxVal) {
        cout << "--- Consulta Intervalo [" << minVal << " a " << maxVal << "] ---" << endl;
        cout << "Resultados: ";
        rangeQueryRec(root, minVal, maxVal);
        cout << endl;
    }

    double median() {
        int n = getSize(root);
        if (n == 0) return 0.0;

        if (n % 2 != 0) {
            // Ímpar: Pega o elemento exato do meio (n/2 + 1)
            return findKthSmallest(root, n / 2 + 1);
        } else {
            // Par: Média dos dois centrais
            double val1 = findKthSmallest(root, n / 2);
            double val2 = findKthSmallest(root, n / 2 + 1);
            return (val1 + val2) / 2.0;
        }
    }
};

// --- Teste Principal ---
int main() {
    SensorAVL avl;
    
    cout << "=== TESTE VERSAO APRIMORADA (AVL TREE) ===\n" << endl;

    // 1. Inserção (Cria desbalanceamento para testar rotações)
    avl.insert(10.0);
    avl.insert(20.0);
    avl.insert(30.0); // Rotação Esquerda deve ocorrer aqui
    avl.insert(40.0);
    avl.insert(50.0);
    avl.insert(25.0); // Inserção no meio

    avl.printSorted();

    // 2. Mediana O(log N)
    // Dados ordenados: 10, 20, 25, 30, 40, 50 (6 elementos)
    // Mediana esperada: (25 + 30) / 2 = 27.5
    cout << "Mediana (deve ser 27.5): " << avl.median() << endl;

    // 3. Range Query
    avl.rangeQuery(15.0, 35.0); // Esperado: 20, 25, 30

    // 4. Min / Max
    avl.getMinMax(2);

    // 5. Remoção
    avl.remove(30.0); // Remove um nó interno
    avl.printSorted();
    
    // Mediana após remoção: 10, 20, 25, 40, 50 (5 elementos) -> Meio é 25
    cout << "Nova Mediana (deve ser 25.0): " << avl.median() << endl;

    return 0;
}