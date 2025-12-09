#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <cmath>

// Função auxiliar para medição de tempo (evita repetição de código no main)
template <typename Func>
long long medirTempo(Func funcao) {
    auto inicio = std::chrono::steady_clock::now();
    funcao();
    auto fim = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(fim - inicio).count();
}

// 1. Implementação de Heap Binário (Min-Heap)
class MinHeapCustomizado {
private:
    std::vector<double> _dados;

    // Helpers de índice inline
    int pai(int i) const { return (i - 1) >> 1; } // Bitwise shift para dividir por 2
    int filhoEsq(int i) const { return (i << 1) + 1; }
    int filhoDir(int i) const { return (i << 1) + 2; }

    // "Promover" (Bubble Up)
    void promoverElemento(int idx) {
        while (idx > 0) {
            int p = pai(idx);
            if (_dados[p] <= _dados[idx]) break; // Propriedade do heap satisfeita
            std::swap(_dados[idx], _dados[p]);
            idx = p;
        }
    }

    // "Rebaixar" (Sink Down / Heapify)
    void rebaixarElemento(int idx) {
        int tam = _dados.size();
        while (true) {
            int menor = idx;
            int esq = filhoEsq(idx);
            int dir = filhoDir(idx);

            if (esq < tam && _dados[esq] < _dados[menor]) menor = esq;
            if (dir < tam && _dados[dir] < _dados[menor]) menor = dir;

            if (menor == idx) break;

            std::swap(_dados[idx], _dados[menor]);
            idx = menor;
        }
    }

public:
    void inserir(double valor) {
        _dados.push_back(valor);
        promoverElemento(_dados.size() - 1);
    }

    void remover(double valor) {
        // Busca linear substituída por std::find para legibilidade
        auto it = std::find(_dados.begin(), _dados.end(), valor);
        
        if (it == _dados.end()) return; // Não encontrou

        int idx = std::distance(_dados.begin(), it);
        
        // Troca com o último e remove
        _dados[idx] = _dados.back();
        _dados.pop_back();

        if (idx < _dados.size()) {
            rebaixarElemento(idx);
            promoverElemento(idx);
        }
    }

    double calcularMediana() {
        if (_dados.empty()) return 0.0;
        // Copia e ordena (estratégia ineficiente original mantida propositalmente)
        std::vector<double> temp = _dados;
        std::sort(temp.begin(), temp.end());
        
        size_t n = temp.size();
        bool par = (n % 2 == 0);
        return par ? (temp[n/2 - 1] + temp[n/2]) * 0.5 : temp[n/2];
    }

    std::vector<double> buscaIntervalo(double min, double max) {
        std::vector<double> resposta;
        for (const auto& val : _dados) {
            if (val >= min && val <= max) resposta.push_back(val);
        }
        return resposta;
    }
};

// 2. Árvore AVL
struct NoAVL {
    double valor;
    NoAVL *esq = nullptr, *dir = nullptr;
    int altura = 1;
    NoAVL(double v) : valor(v) {}
};

class ArvoreBalanceada {
private:
    NoAVL* raiz = nullptr;

    int alt(NoAVL* n) { return n ? n->altura : 0; }
    int fatorBal(NoAVL* n) { return n ? alt(n->esq) - alt(n->dir) : 0; }

    // Rotações com nomes mais descritivos
    NoAVL* rotacionarEsquerda(NoAVL* x) {
        NoAVL* y = x->dir;
        NoAVL* T2 = y->esq;
        y->esq = x;
        x->dir = T2;
        x->altura = std::max(alt(x->esq), alt(x->dir)) + 1;
        y->altura = std::max(alt(y->esq), alt(y->dir)) + 1;
        return y;
    }

    NoAVL* rotacionarDireita(NoAVL* y) {
        NoAVL* x = y->esq;
        NoAVL* T2 = x->dir;
        x->dir = y;
        y->esq = T2;
        y->altura = std::max(alt(y->esq), alt(y->dir)) + 1;
        x->altura = std::max(alt(x->esq), alt(x->dir)) + 1;
        return x;
    }

    NoAVL* inserirRec(NoAVL* no, double chave) {
        if (!no) return new NoAVL(chave);
        
        if (chave < no->valor) no->esq = inserirRec(no->esq, chave);
        else no->dir = inserirRec(no->dir, chave);

        no->altura = 1 + std::max(alt(no->esq), alt(no->dir));
        int bal = fatorBal(no);

        // Casos de desbalanceamento
        if (bal > 1) {
            if (chave < no->esq->valor) return rotacionarDireita(no);
            no->esq = rotacionarEsquerda(no->esq);
            return rotacionarDireita(no);
        }
        if (bal < -1) {
            if (chave >= no->dir->valor) return rotacionarEsquerda(no);
            no->dir = rotacionarDireita(no->dir);
            return rotacionarEsquerda(no);
        }
        return no;
    }

    NoAVL* minNo(NoAVL* no) {
        while (no && no->esq) no = no->esq;
        return no;
    }

    NoAVL* removerRec(NoAVL* no, double chave) {
        if (!no) return nullptr;

        if (chave < no->valor) no->esq = removerRec(no->esq, chave);
        else if (chave > no->valor) no->dir = removerRec(no->dir, chave);
        else {
            // Nó encontrado
            if (!no->esq || !no->dir) {
                NoAVL* temp = no->esq ? no->esq : no->dir;
                if (!temp) { temp = no; no = nullptr; }
                else *no = *temp;
                delete temp;
            } else {
                NoAVL* temp = minNo(no->dir);
                no->valor = temp->valor;
                no->dir = removerRec(no->dir, temp->valor);
            }
        }

        if (!no) return nullptr;

        no->altura = 1 + std::max(alt(no->esq), alt(no->dir));
        int bal = fatorBal(no);

        if (bal > 1 && fatorBal(no->esq) >= 0) return rotacionarDireita(no);
        if (bal > 1 && fatorBal(no->esq) < 0) {
            no->esq = rotacionarEsquerda(no->esq);
            return rotacionarDireita(no);
        }
        if (bal < -1 && fatorBal(no->dir) <= 0) return rotacionarEsquerda(no);
        if (bal < -1 && fatorBal(no->dir) > 0) {
            no->dir = rotacionarDireita(no->dir);
            return rotacionarEsquerda(no);
        }
        return no;
    }

    void emOrdem(NoAVL* no, std::vector<double>& lista) {
        if (!no) return;
        emOrdem(no->esq, lista);
        lista.push_back(no->valor);
        emOrdem(no->dir, lista);
    }

    void buscarFaixa(NoAVL* no, double min, double max, std::vector<double>& out) {
        if (!no) return;
        if (min < no->valor) buscarFaixa(no->esq, min, max, out);
        if (no->valor >= min && no->valor <= max) out.push_back(no->valor);
        if (max > no->valor) buscarFaixa(no->dir, min, max, out);
    }

public:
    void inserir(double v) { raiz = inserirRec(raiz, v); }
    void remover(double v) { raiz = removerRec(raiz, v); }
    
    std::vector<double> buscaIntervalo(double min, double max) {
        std::vector<double> res;
        buscarFaixa(raiz, min, max, res);
        return res;
    }

    double calcularMediana() {
        std::vector<double> ordenados;
        emOrdem(raiz, ordenados);
        if (ordenados.empty()) return 0.0;
        size_t n = ordenados.size();
        return (n % 2 != 0) ? ordenados[n/2] : (ordenados[n/2 - 1] + ordenados[n/2]) / 2.0;
    }
};

// 3. Vetor com Ordenação Preguiçosa (Lazy Insertion Sort)
class ListaOrdenadaManual {
private:
    std::vector<double> _container;
    bool _estaOrdenado = false;

    // Algoritmo Insertion Sort clássico
    void ordenarAgora() {
        int n = _container.size();
        for (int i = 1; i < n; ++i) {
            double pivo = _container[i];
            int j = i - 1;
            // Loop reescrito de forma mais compacta
            for (; j >= 0 && _container[j] > pivo; --j) {
                _container[j + 1] = _container[j];
            }
            _container[j + 1] = pivo;
        }
        _estaOrdenado = true;
    }

public:
    void inserir(double v) {
        _container.push_back(v);
        _estaOrdenado = false; // Invalida a ordenação
    }

    void remover(double v) {
        // Encontra primeiro elemento e remove
        auto it = std::find(_container.begin(), _container.end(), v);
        if (it != _container.end()) {
            _container.erase(it);
        }
    }

    double calcularMediana() {
        if (_container.empty()) return 0.0;
        if (!_estaOrdenado) ordenarAgora(); // Lazy sort

        size_t n = _container.size();
        return (n & 1) ? _container[n/2] : (_container[n/2 - 1] + _container[n/2]) * 0.5;
    }

    std::vector<double> buscaIntervalo(double min, double max) {
        std::vector<double> ret;
        for (double x : _container) {
            if (x >= min && x <= max) ret.push_back(x);
        }
        return ret;
    }
};

std::vector<double> carregarArquivo(const std::string& path) {
    std::vector<double> buffer;
    std::ifstream arq(path);
    if (!arq) {
        std::cerr << "[ERRO] Arquivo '" << path << "' nao encontrado.\n";
        return buffer;
    }
    std::string linha;
    while (std::getline(arq, linha)) {
        if (linha.empty()) continue;
        try { buffer.push_back(std::stod(linha)); } catch (...) {}
    }
    return buffer;
}

int main() {
    auto dadosBrutos = carregarArquivo("temperaturas.csv");
    if (dadosBrutos.empty()) {
        std::cout << "Por favor, crie o arquivo CSV antes de rodar.\n";
        return 1;
    }

    std::cout << ">>> Carregados " << dadosBrutos.size() << " registros.\n\n";

    // Instanciação das estruturas
    MinHeapCustomizado heap;
    ArvoreBalanceada avl;
    ListaOrdenadaManual lista;

    // --- TESTE 1: INSERÇÃO ---
    long tHeapIns = medirTempo([&]() {
        for (double v : dadosBrutos) heap.inserir(v);
    });
    long tAvlIns = medirTempo([&]() {
        for (double v : dadosBrutos) avl.inserir(v);
    });
    long tListIns = medirTempo([&]() {
        for (double v : dadosBrutos) lista.inserir(v);
    });

    // --- TESTE 2: MEDIANA ---
    long tHeapMed = medirTempo([&]() { heap.calcularMediana(); });
    long tAvlMed  = medirTempo([&]() { avl.calcularMediana(); });
    long tListMed = medirTempo([&]() { lista.calcularMediana(); });

    // --- TESTE 3: BUSCA POR INTERVALO ---
    double rangeA = 20.0, rangeB = 30.0;
    long tHeapBusca = medirTempo([&]() { heap.buscaIntervalo(rangeA, rangeB); });
    long tAvlBusca  = medirTempo([&]() { avl.buscaIntervalo(rangeA, rangeB); });
    long tListBusca = medirTempo([&]() { lista.buscaIntervalo(rangeA, rangeB); });

    // --- TESTE 4: REMOÇÃO (Amostra de 100 itens) ---
    std::vector<double> alvoRemocao;
    size_t qtdRemover = std::min((size_t)100, dadosBrutos.size());
    for(size_t i = 0; i < qtdRemover; i++) alvoRemocao.push_back(dadosBrutos[i]);

    long tHeapRem = medirTempo([&]() {
        for (double v : alvoRemocao) heap.remover(v);
    });
    long tAvlRem = medirTempo([&]() {
        for (double v : alvoRemocao) avl.remover(v);
    });
    long tListRem = medirTempo([&]() {
        for (double v : alvoRemocao) lista.remover(v);
    });

    // Exibição dos Resultados
    std::cout << "==============================================================\n";
    std::cout << "          RELATORIO DE DESEMPENHO (Microsegundos)             \n";
    std::cout << "==============================================================\n";
    std::cout << std::left << std::setw(18) << "Cenario" 
              << std::setw(12) << "MinHeap" 
              << std::setw(12) << "AVL Tree" 
              << std::setw(12) << "Vector" 
              << "Melhor" << std::endl;
    std::cout << "--------------------------------------------------------------\n";

    auto imprimirLinha = [](std::string nome, long t1, long t2, long t3) {
        std::string campeao;
        if (t1 <= t2 && t1 <= t3) campeao = "MinHeap";
        else if (t2 <= t1 && t2 <= t3) campeao = "AVL Tree";
        else campeao = "Vector";

        std::cout << std::left << std::setw(18) << nome 
                  << std::setw(12) << t1 
                  << std::setw(12) << t2 
                  << std::setw(12) << t3 
                  << campeao << std::endl;
    };

    imprimirLinha("Insercao", tHeapIns, tAvlIns, tListIns);
    imprimirLinha("Calc. Mediana", tHeapMed, tAvlMed, tListMed);
    imprimirLinha("Busca Faixa", tHeapBusca, tAvlBusca, tListBusca);
    imprimirLinha("Remocao (x100)", tHeapRem, tAvlRem, tListRem);

    std::cout << "\n[Analise]:\n";
    std::cout << "1. Vector eh instantaneo na insercao (append), mas sofre na mediana (ordena tudo).\n";
    std::cout << "2. AVL eh a estrutura mais estavel para buscas e remocoes.\n";
    std::cout << "3. Heap eh bom para inserir, mas ruim para buscas arbitras.\n";

    return 0;
}