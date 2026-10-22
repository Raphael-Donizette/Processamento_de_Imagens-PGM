#include <cstdlib>
#include <iostream>
#include <fstream>
#include <clocale>

using namespace std;

typedef int tImagem[1024][1024];

string erro;

int carregaPGM(string nome, tImagem img, int *col, int *lin, int *tons)
{
    string tipo;
    ifstream arquivo(nome);
    if (!arquivo.is_open())
    {
        erro = "Erro: Arquivo n�o encontrado.";
        return 1;
    }

    arquivo >> tipo;
    if (tipo != "P2")
    {
        erro = "Erro: Arquivo n�o tem formato P2.";
        return 2;
    }

    arquivo >> *col >> *lin >> *tons;
    for (int i = 0; i < *lin; i++)
    {
        for (int j = 0; j < *col; j++)
        {
            arquivo >> img[i][j];
        }
    }
    arquivo.close();
    return 0;
}

int salvaPGM(string nome, tImagem img, int col, int lin, int tons)
{
    ofstream arquivo(nome);
    if (!arquivo.is_open())
    {
        erro = "Erro: Arquivo n�o encontrado.";
        return 1;
    }

    arquivo << "P2" << endl
            << col << " " << lin << endl
            << tons << endl;
    for (int i = 0; i < lin; i++)
    {
        for (int j = 0; j < col; j++)
        {
            arquivo << img[i][j] << " ";
        }
        arquivo << endl;
    }

    arquivo.close();
    return 0;
}

void ajustarBrilho(tImagem imgEntrada, tImagem imgSaida, int linhas, int colunas, int brilho)
{
    for (int i = 0; i < linhas; i++)
    {
        for (int j = 0; j < colunas; j++)
        {
            int novoValor = imgEntrada[i][j] + brilho;
            if (novoValor > 255) novoValor = 255;
            if (novoValor < 0)   novoValor = 0;
            imgSaida[i][j] = novoValor;
        }
    }
}

void binarizarLimiar(tImagem imgEntrada, tImagem imgSaida, int linhas, int colunas, int limiar)
{
    for (int i = 0; i < linhas; i++)
    {
        for (int j = 0; j < colunas; j++)
        {
            if (imgEntrada[i][j] >= limiar)
            {
                imgSaida[i][j] = 255;
            }
            else
            {
                imgSaida[i][j] = 0;
            }
        }
    }
}

// 1. ROTA��O PARA A DIREITA
void rotacionarDireita(tImagem imgEntrada, tImagem imgSaida, int &linhas, int &colunas)
{
    for (int i = 0; i < linhas; i++)
    {
        for (int j = 0; j < colunas; j++)
        {
            imgSaida[j][(linhas - 1) - i] = imgEntrada[i][j];
        }
    }
    int aux = linhas;
    linhas = colunas;
    colunas = aux;
}

// 2. ROTA��O PARA A ESQUERDA
void rotacionarEsquerda(tImagem imgEntrada, tImagem imgSaida, int &linhas, int &colunas)
{
    for (int i = 0; i < linhas; i++)
    {
        for (int j = 0; j < colunas; j++)
        {
            imgSaida[(colunas - 1) - j][i] = imgEntrada[i][j];
        }
    }
    int aux = linhas;
    linhas = colunas;
    colunas = aux;
}

// 3. ESPELHAMENTO HORIZONTAL
void espelharHorizontal(tImagem imgEntrada, tImagem imgSaida, int linhas, int colunas)
{
    for (int i = 0; i < linhas; i++)
    {
        for (int j = 0; j < colunas; j++)
        {
            imgSaida[i][(colunas - 1) - j] = imgEntrada[i][j];
        }
    }
}

// 4. ESPELHAMENTO VERTICAL
void espelharVertical(tImagem imgEntrada, tImagem imgSaida, int linhas, int colunas)
{
    for (int i = 0; i < linhas; i++)
    {
        for (int j = 0; j < colunas; j++)
        {
            imgSaida[(linhas - 1) - i][j] = imgEntrada[i][j];
        }
    }
}

void iconizar(tImagem imgEntrada, tImagem imgSaida, int linhas, int colunas)
{
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            int orig_i = i * linhas / 64;
            int orig_j = j * colunas / 64;
            imgSaida[i][j] = imgEntrada[orig_i][orig_j];
        }
    }
    cout << "Imagem iconizada com sucesso!" << endl;
}

void filtroPassaBaixa(tImagem imgEntrada, tImagem imgSaida, int linhas, int colunas)
{
    for (int i = 0; i < linhas; i++)
    {
        for (int j = 0; j < colunas; j++)
        {
            imgSaida[i][j] = imgEntrada[i][j];
        }
    }

    for (int i = 1; i < linhas - 1; i++)
    {
        for (int j = 1; j < colunas - 1; j++)
        {
            int soma = 0;
            soma += imgEntrada[i - 1][j - 1] + imgEntrada[i - 1][j] + imgEntrada[i - 1][j + 1];
            soma += imgEntrada[i][j - 1]     + imgEntrada[i][j]     + imgEntrada[i][j + 1];
            soma += imgEntrada[i + 1][j - 1] + imgEntrada[i + 1][j] + imgEntrada[i + 1][j + 1];

            imgSaida[i][j] = soma / 9;
        }
    }
    cout << "Filtro passa-baixa 3x3 aplicado com sucesso!" << endl;
}

void escurecerBorda(tImagem imgEntrada, tImagem imgSaida, int linhas, int colunas)
{
    for (int i = 0; i < linhas; i++)
    {
        for (int j = 0; j < colunas; j++)
        {
            int dist_esquerda = j;
            int dist_direita = (colunas - 1) - j;
            int dist_cima = i;
            int dist_baixo = (linhas - 1) - i;

            int menor_dist = dist_esquerda;
            if (dist_direita < menor_dist) menor_dist = dist_direita;
            if (dist_cima < menor_dist)    menor_dist = dist_cima;
            if (dist_baixo < menor_dist)   menor_dist = dist_baixo;

            int largura_borda = (linhas < colunas) ? linhas * 0.15 : colunas * 0.15;
            if (largura_borda < 1) largura_borda = 1;

            if (menor_dist < largura_borda)
            {
                double fator = (double)menor_dist / largura_borda;
                imgSaida[i][j] = imgEntrada[i][j] * fator;
            }
            else
            {
                imgSaida[i][j] = imgEntrada[i][j];
            }
        }
    }
    cout << "Bordas escurecidas com sucesso!" << endl;
}

void exibirMenu(tImagem imgEntrada, tImagem imgSaida, int &linhas, int &colunas, int &tons)
{
    int opcao, brilho;
    string arquivo_entrada, arquivo_saida;

    do
    {
        cout << "==============================================" << endl;
        cout << "      MENU DE PROCESSAMENTO DE IMAGENS" << endl;
        cout << "==============================================" << endl;
        cout << "0 - Sair do programa e Salvar" << endl;
        cout << "1 - Carregar arquivo PGM (at� 1024x1024)" << endl;
        cout << "2 - Ajustar brilho" << endl;
        cout << "3 - Binarizar por limiar" << endl;
        cout << "4 - Rotacionar imagem" << endl;
        cout << "5 - Negativar imagem" << endl;
        cout << "6 - Binarizar (Iconizar)" << endl;
        cout << "7 - Utilizar filtro passa-baixa 3x3" << endl;
        cout << "8 - Escurecer borda" << endl;
        cout << "Escolha uma das op��es acima: ";
        cin >> opcao;
        cout << endl;

        switch (opcao)
        {
        case 0:
            cout << " Entre com o nome da imagem de sa�da: ";
            cin >> arquivo_saida;
            arquivo_saida = arquivo_saida + ".pgm";

            if (salvaPGM(arquivo_saida, imgSaida, colunas, linhas, tons) != 0)
            {
                cout << "\n" + erro + "\n";
            }
            else
            {
                cout << "Imagem salva com sucesso! Encerrando..." << endl;
            }
            break;

        case 1:
            cout << "Entre com o nome da imagem de entrada: ";
            cin >> arquivo_entrada;
            cout << endl;
            arquivo_entrada = arquivo_entrada + ".pgm";

            if (carregaPGM(arquivo_entrada, imgEntrada, &colunas, &linhas, &tons) != 0)
            {
                cout << "\n" + erro + "\n";
            }
            else
            {
                cout << "Sucesso! Imagem carregada: " << colunas << "x" << linhas << endl;
                // Sincroniza a sa�da com a entrada inicial
                for (int i = 0; i < linhas; i++) {
                    for (int j = 0; j < colunas; j++) {
                        imgSaida[i][j] = imgEntrada[i][j];
                    }
                }
            }
            break;

        case 2:
            cout << "==============================================" << endl;
            cout << "              AJUSTE DE BRILHO" << endl;
            cout << "==============================================" << endl;
            cout << "Digite um valor negativo para ESCURECER ou positivo para CLAREAR: ";
            cin >> brilho;
            cout << endl;

            ajustarBrilho(imgEntrada, imgSaida, linhas, colunas, brilho);
            cout << "Brilho ajustado com sucesso!" << endl;

            for (int i = 0; i < linhas; i++) {
                for (int j = 0; j < colunas; j++) imgEntrada[i][j] = imgSaida[i][j];
            }
            break;

        case 3:
            cout << "==============================================" << endl;
            cout << "            BINARIZA��O POR LIMIAR" << endl;
            cout << "==============================================" << endl;
            cout << "Digite o valor do limiar (0 a 255): ";
            cin >> brilho;
            cout << endl;

            binarizarLimiar(imgEntrada, imgSaida, linhas, colunas, brilho);
            cout << "Imagem binarizada com sucesso!" << endl;

            for (int i = 0; i < linhas; i++) {
                for (int j = 0; j < colunas; j++) imgEntrada[i][j] = imgSaida[i][j];
            }
            break;

        case 4:
            int subOpcao;
            cout << "==============================================" << endl;
            cout << "                 ROTACIONAR" << endl;
            cout << "==============================================" << endl;
            cout << "1 - Rotacionar para a Esquerda" << endl;
            cout << "2 - Rotacionar para a Direita" << endl;
            cout << "3 - Espelhar Verticalmente (Ponta-cabe�a)" << endl;
            cout << "4 - Espelhar Horizontalmente (Inverter Lados)" << endl;
            cout << "Escolha o tipo de rota��o: ";
            cin >> subOpcao;
            cout << endl;

            switch (subOpcao)
            {
            case 1:
                rotacionarEsquerda(imgEntrada, imgSaida, linhas, colunas);
                cout << "Imagem rotacionada para a esquerda!" << endl;
                break;
            case 2:
                rotacionarDireita(imgEntrada, imgSaida, linhas, colunas);
                cout << "Imagem rotacionada para a direita!" << endl;
                break;
            case 3:
                espelharVertical(imgEntrada, imgSaida, linhas, colunas);
                cout << "Imagem espelhada verticalmente!" << endl;
                break;
            case 4:
                espelharHorizontal(imgEntrada, imgSaida, linhas, colunas);
                cout << "Imagem espelhada horizontalmente!" << endl;
                break;
            default:
                cout << "Op��o de rota��o inv�lida!" << endl;
                break;
            }

            for (int i = 0; i < linhas; i++) {
                for (int j = 0; j < colunas; j++) imgEntrada[i][j] = imgSaida[i][j];
            }
            break;

        case 5:
            for (int i = 0; i < linhas; i++)
            {
                for (int j = 0; j < colunas; j++)
                {
                    imgSaida[i][j] = tons - imgEntrada[i][j];
                }
            }
            cout << "Efeito negativo aplicado com sucesso!" << endl;

            for (int i = 0; i < linhas; i++) {
                for (int j = 0; j < colunas; j++) imgEntrada[i][j] = imgSaida[i][j];
            }
            break;

        case 6:
            iconizar(imgEntrada, imgSaida, linhas, colunas);
            linhas = 64;
            colunas = 64;

            for (int i = 0; i < linhas; i++) {
                for (int j = 0; j < colunas; j++) imgEntrada[i][j] = imgSaida[i][j];
            }
            break;

        case 7:
            filtroPassaBaixa(imgEntrada, imgSaida, linhas, colunas);

            for (int i = 0; i < linhas; i++) {
                for (int j = 0; j < colunas; j++) imgEntrada[i][j] = imgSaida[i][j];
            }
            break;

        case 8:
            cout << "==============================================" << endl;
            cout << "               ESCURECER BORDAS" << endl;
            cout << "==============================================" << endl;

            escurecerBorda(imgEntrada, imgSaida, linhas, colunas);

            for (int i = 0; i < linhas; i++) {
                for (int j = 0; j < colunas; j++) imgEntrada[i][j] = imgSaida[i][j];
            }
            break;

        default:
            if (opcao != 0)
                cout << "Op��o inv�lida! Tente novamente." << endl;
            break;
        }
    } while (opcao != 0);
}

int main(int argc, char **argv)
{
    setlocale(LC_ALL, "Portuguese");
    static tImagem img_entrada, img_saida;
    int colunas = 0, linhas = 0, tons = 0;

    exibirMenu(img_entrada, img_saida, linhas, colunas, tons);

    return 0;
}