#include <iostream>
#include <vector>
#include <queue>      // Para priority_queue
#include <functional> // Para greater<double>
#include <algorithm>  // Para sort (auxiliar no print)

using namespace std;

class SensorHeap {
private:
    // HEAP 1: Armazena a metade MENOR dos dados. 
    // O topo é o MAIOR dessa metade (candidato à mediana).
    priority_queue<double> maxHeap; 

    // HEAP 2: Armazena a metade MAIOR dos dados.
    // O topo é o MENOR dessa metade (candidato à mediana).
    priority_queue<double, vector<double>, greater<double>> minHeap;

    // Função auxiliar para rebalancear os heaps após inserção/remoção
    void balanceHeaps() {
        // A regra é: maxHeap pode ter no máximo 1 elemento a mais que minHeap
        if (maxHeap.size() > minHeap.size() + 1) {
            minHeap.push(maxHeap.top());
            maxHeap.pop();
        } else if (minHeap.size() > maxHeap.size()) {
            maxHeap.push(minHeap.top());
            minHeap.pop();
        }
    }

    // Função auxiliar para remover item arbitrário (O ponto fraco do Heap)
    // C++ STL priority_queue não tem remove(valor), então precisamos reconstruir
    template <typename T>
    void removeFromQueue(T& pq, double value) {
        vector<double> temp;
        bool removed = false;
        
        // Esvazia a fila guardando elementos
        while (!pq.empty()) {
            double top = pq.top();
            pq.pop();
            
            // Se achou o valor e ainda não removeu, pula a reinserção
            if (top == value && !removed) {
                removed = true;
            } else {
                temp.push_back(top);
            }
        }
        
        // Reinsere tudo
        for (double v : temp) {
            pq.push(v);
        }
    }

public:
    // 1. insert(value): O(log N)
    // Muito rápido. Insere no heap correto e balanceia.
    void insert(double value) {
        if (maxHeap.empty() || value < maxHeap.top()) {
            maxHeap.push(value);
        } else {
            minHeap.push(value);
        }
        balanceHeaps();
    }

    // 2. median(): O(1) !!!
    // Esta é a grande vantagem desta estrutura. Acesso imediato.
    double median() {
        if (maxHeap.empty()) return 0.0;

        // Se tamanhos iguais, média dos topos
        if (maxHeap.size() == minHeap.size()) {
            return (maxHeap.top() + minHeap.top()) / 2.0;
        } else {
            // Se tamanhos diferentes, o maxHeap (que permitimos ter 1 a mais) tem a mediana
            return maxHeap.top();
        }
    }

    // 3. remove(value): O(N)
    // PONTO FRACO: Heaps não servem para remover itens do meio.
    // Temos que desmontar e montar de novo.
    void remove(double value) {
        // Tenta remover de ambos (não sabemos onde está)
        // Nota: isso é ineficiente comparado à Árvore AVL
        removeFromQueue(maxHeap, value);
        removeFromQueue(minHeap, value);
        balanceHeaps();
        cout << "[Remove] Processo de remocao executado para " << value << endl;
    }

    // 4. getMinMax(k): O(K log N) ou O(1) parcial
    // O Min global está no topo do minHeap (ou maxHeap se minHeap vazio)
    // O Max global está... perdido no fundo do minHeap ou no topo.
    // Heaps só garantem acesso rápido a UM extremo.
    void getMinMax(int k) {
        // Para implementar isso genericamente com 2 heaps focados em mediana,
        // precisamos copiar os dados, pois não temos acesso direto ao "Fundo" do heap.
        // Isso demonstra uma limitacao da estrutura para esse requisito especifico.
        
        vector<double> allData;
        
        // Cópia temporária (ineficiente, mas necessário para Heap estrito)
        priority_queue<double> tempMax = maxHeap;
        priority_queue<double, vector<double>, greater<double>> tempMin = minHeap;
        
        while(!tempMax.empty()) { allData.push_back(tempMax.top()); tempMax.pop(); }
        while(!tempMin.empty()) { allData.push_back(tempMin.top()); tempMin.pop(); }
        
        sort(allData.begin(), allData.end()); // O(N log N)
        
        int n = allData.size();
        k = min(k, n);
        
        cout << "--- Extremos (" << k << ") ---" << endl;
        cout << "Minimos: ";
        for(int i=0; i<k; i++) cout << allData[i] << " ";
        cout << "\nMaximos: ";
        for(int i=0; i<k; i++) cout << allData[n-1-i] << " ";
        cout << endl;
    }

    // 5. rangeQuery(x, y): O(N)
    // PONTO FRACO: Heap não ordena tudo, então precisamos verificar todos os itens.
    void rangeQuery(double minVal, double maxVal) {
        cout << "--- Consulta Intervalo [" << minVal << " a " << maxVal << "] ---" << endl;
        cout << "Resultados: ";
        
        // Helper lambda para checar um heap sem destruir
        auto checkHeap = [&](auto pq) {
            while (!pq.empty()) {
                double val = pq.top();
                pq.pop();
                if (val >= minVal && val <= maxVal) {
                    cout << val << " ";
                }
            }
        };

        checkHeap(maxHeap);
        checkHeap(minHeap);
        cout << endl;
    }

    // 6. printSorted(): O(N log N)
    // Heaps não mantém ordem total, apenas ordem de prioridade.
    // Para imprimir tudo ordenado, precisamos extrair tudo.
    void printSorted() {
        // Copia para não destruir a estrutura original
        priority_queue<double> tempMax = maxHeap;
        priority_queue<double, vector<double>, greater<double>> tempMin = minHeap;
        
        vector<double> sorted;
        while(!tempMax.empty()) { sorted.push_back(tempMax.top()); tempMax.pop(); }
        while(!tempMin.empty()) { sorted.push_back(tempMin.top()); tempMin.pop(); }
        
        sort(sorted.begin(), sorted.end());
        
        cout << "Heap (Flattened & Sorted): ";
        for(double v : sorted) cout << v << " | ";
        cout << endl;
    }
};

int main() {
    SensorHeap heaps;
    
    cout << "=== TESTE VERSAO APRIMORADA (TWO HEAPS) ===\n" << endl;

    // 1. Inserções
    heaps.insert(10.0);
    heaps.insert(50.0);
    heaps.insert(30.0);
    heaps.insert(20.0);
    heaps.insert(40.0); 
    // Estado atual ordenado logicamente: 10, 20, 30, 40, 50
    // Mediana esperada: 30
    
    // 2. Teste Mediana O(1)
    cout << "Mediana (Deve ser 30): " << heaps.median() << endl;
    
    heaps.insert(60.0);
    // Estado: 10, 20, 30, 40, 50, 60
    // Mediana: (30+40)/2 = 35
    cout << "Mediana Apos Insercao (Deve ser 35): " << heaps.median() << endl;

    // 3. Teste Min/Max
    heaps.getMinMax(2);

    // 4. Remoção (O(N) - Lento neste modelo)
    heaps.remove(30.0); 
    // Sobra: 10, 20, 40, 50, 60 -> Mediana 40
    cout << "Mediana Apos Remover 30 (Deve ser 40): " << heaps.median() << endl;

    // 5. Range Query
    heaps.rangeQuery(15.0, 45.0);

    return 0;
}