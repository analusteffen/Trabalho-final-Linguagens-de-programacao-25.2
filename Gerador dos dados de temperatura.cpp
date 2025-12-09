// Inclusão dos cabeçalhos necessários
#include <iostream>
#include <fstream>
#include <random>
#include <iomanip>

int main() {
    // Configurações e constantes do gerador
    constexpr auto CAMINHO_SAIDA = "temperaturas.csv";
    constexpr int NUM_AMOSTRAS = 1000;
    constexpr double MINIMO = -10.0;
    constexpr double MAXIMO = 45.0;

    // Inicializa o fluxo de saída para o arquivo
    std::ofstream fluxoSaida(CAMINHO_SAIDA);

    // Verifica se houve falha na abertura
    if (fluxoSaida.fail()) {
        std::cerr << "[Erro] Nao foi possivel criar ou abrir: " << CAMINHO_SAIDA << std::endl;
        return 1; // Retorna erro
    }

    // Configuração do motor de aleatoriedade (Mersenne Twister)
    std::random_device semente;
    std::mt19937 gerador(semente());
    // Define a faixa de valores float/double
    std::uniform_real_distribution<double> distribuicao(MINIMO, MAXIMO);

    // Configura a formatação numérica do arquivo (ponto fixo, 2 casas)
    fluxoSaida << std::fixed << std::setprecision(2);

    // Loop de geração e persistência dos dados
    int contador = 0;
    while (contador < NUM_AMOSTRAS) {
        double valorSorteado = distribuicao(gerador);
        fluxoSaida << valorSorteado << "\n";
        contador++;
    }

    // O fechamento explícito é opcional (o destrutor faria isso), mas é boa prática
    fluxoSaida.close();

    std::cout << "Sucesso! O arquivo '" << CAMINHO_SAIDA 
              << "' foi gerado com " << NUM_AMOSTRAS << " registros." << std::endl;

    return 0;
}