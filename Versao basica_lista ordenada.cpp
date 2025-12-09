#include <iostream>
#include <vector>
#include <algorithm> // Necessário para lower_bound e upper_bound
#include <iomanip>   // Para formatar a saída (casas decimais)

using namespace std;

class SensorListaOrdenada {
private:
    vector<double> dados; // Estrutura linear (Array dinâmico)

public:
    // 1. insert(value): Insere mantendo a ordem (Insertion Sort logic)
    // Complexidade: O(N) - pois precisa deslocar elementos
    void insert(double value) {
        // Encontra a primeira posição onde o elemento não é menor que 'value'
        // Isso garante que inserimos na posição correta para manter a ordem.
        auto it = lower_bound(dados.begin(), dados.end(), value);
        
        // Insere o valor na posição encontrada, deslocando o resto para a direita
        dados.insert(it, value);
        
        cout << "[Insert] Inserido " << value << ". Total de leituras: " << dados.size() << endl;
    }

    // 2. remove(value): Remove uma leitura específica
    // Complexidade: O(N) - pois precisa deslocar elementos para tampar o buraco
    void remove(double value) {
        auto it = lower_bound(dados.begin(), dados.end(), value);
        
        // Verifica se o item realmente existe naquela posição
        if (it != dados.end() && *it == value) {
            dados.erase(it);
            cout << "[Remove] Removido " << value << endl;
        } else {
            cout << "[Remove] Valor " << value << " nao encontrado." << endl;
        }
    }

    // 3. printSorted(): Imprime todos em ordem
    // Complexidade: O(N)
    void printSorted() {
        cout << "Lista Ordenada: ";
        if (dados.empty()) {
            cout << "(vazia)" << endl;
            return;
        }
        for (double temp : dados) {
            cout << temp << " | ";
        }
        cout << endl;
    }

    // 4. min() e max(): Retorna os K menores/maiores
    // Complexidade: O(K) - Acesso direto, muito rápido
    void getMinMax(int k) {
        if (dados.empty()) return;
        int n = dados.size();
        
        // Ajusta K caso a lista seja menor que o pedido
        if (k > n) k = n;

        cout << "--- Extremos (" << k << " valores) ---" << endl;
        
        cout << "Minimos: ";
        for (int i = 0; i < k; i++) {
            cout << dados[i] << " ";
        }
        cout << endl;

        cout << "Maximos: ";
        for (int i = 0; i < k; i++) {
            cout << dados[n - 1 - i] << " "; // Acessa do fim para o começo
        }
        cout << endl;
    }

    // 5. rangeQuery(x, y): Retorna leituras entre X e Y
    // Complexidade: O(log N + K) - Busca binária + iteração
    void rangeQuery(double minVal, double maxVal) {
        cout << "--- Consulta Intervalo [" << minVal << " a " << maxVal << "] ---" << endl;
        
        // Busca binária para achar onde começa e onde termina
        auto itStart = lower_bound(dados.begin(), dados.end(), minVal);
        auto itEnd = upper_bound(dados.begin(), dados.end(), maxVal);

        if (itStart == dados.end() || itStart == itEnd) {
            cout << "Nenhuma leitura neste intervalo." << endl;
            return;
        }

        cout << "Resultados: ";
        for (auto it = itStart; it != itEnd; ++it) {
            cout << *it << " ";
        }
        cout << endl;
    }

    // 6. median(): Retorna a mediana
    // Complexidade: O(1) - Acesso imediato ao índice do meio
    double median() {
        if (dados.empty()) return 0.0;
        
        size_t n = dados.size();
        
        if (n % 2 != 0) {
            // Se for ímpar, é o elemento do meio exato
            return dados[n / 2];
        } else {
            // Se for par, é a média dos dois centrais
            return (dados[n / 2 - 1] + dados[n / 2]) / 2.0;
        }
    }
};

// --- Função Principal para Testar a Versão Básica ---
int main() {
    SensorListaOrdenada lista;

    cout << "=== TESTE VERSAO BASICA (LISTA ORDENADA) ===\n" << endl;

    // 1. Teste de Inserção (Inserindo fora de ordem propositalmente)
    lista.insert(25.5);
    lista.insert(30.0);
    lista.insert(10.5); // Menor valor, deve ir para o início
    lista.insert(22.0); // Valor intermediário
    lista.insert(30.0); // Valor repetido
    
    cout << endl;
    lista.printSorted();
    cout << endl;

    // 2. Teste de Min/Max
    lista.getMinMax(2); // Pegar os 2 menores e 2 maiores
    cout << endl;

    // 3. Teste da Mediana
    cout << "Mediana atual: " << lista.median() << endl;
    cout << endl;

    // 4. Teste de Range Query
    lista.rangeQuery(20.0, 28.0); // Deve retornar 22.0 e 25.5
    cout << endl;

    // 5. Teste de Remoção
    lista.remove(25.5);
    lista.printSorted();
    cout << "Nova Mediana: " << lista.median() << endl;

    return 0;
}