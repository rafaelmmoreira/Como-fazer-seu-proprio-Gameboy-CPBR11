/*
 * File: Endurino.ino
 * Author: Rafael de Moura Moreira <rafaelmmoreira@gmail.com>
 * License: MIT License
 * 
 * Copyright (c) 2018 Rafael de Moura Moreira
 * 
 * Um jogo de carrinho para desviar dos obstáculos, compatível
 * com Arduino Uno e Mega com display LCD e alguns botões.
 * Idealizado para ser jogado na PQDB.
 * 
 * Essa versão é baseada em um dos jogos de exemplo do
 * repositório da PQDB, refatorado para se tornar mais
 * didático para uso no workshop na Campus Party Brasil 11,
 * "Como Fazer Seu Próprio Gameboy".
 * 
 * Os drivers da PQDB foram adaptados para se adequar
 * aos protótipos que serão usados no workshop. Quem
 * adquiriu/montou a própria PQDB deve baixar os drivers
 * atualizados no repositório do projeto.
 * <https://github.com/projetopqdb/PQDB-Arduino>
 * 
 * Este jogo subdivide cada linha do LCD em 2 linhas, de
 * modo que o carrinho pode ocupar 4 posições diferentes 
 * na vertical (e 16 na horizontal).
 * 
 * O código possui 3 funções principais em loop:
 * -> loop_Input() processa as entradas do usuário
 * -> loop_Update() atualiza as informações do jogo
 * -> loop_Render() desenha os elementos na tela
 */

#include "keypad.h"
#include "lcd.h"

// Definindo constantes para cada um dos desenhos.
// Isso vai facilitar o acesso à memória do LCD,
// pois não precisaremos decorar as posições de
// cada desenho.
#define CARRO1 1
#define CARRO2 2
#define CAIXA1 3
#define CAIXA2 4
#define CARROCAIXA1 5
#define CARROCAIXA2 6
#define PISTA 7

// Definindo os possíveis valores de cada posição no mapa
#define VAZIO 0
#define OBSTACULO 1

// key recebe a tecla que o usuário apertou.
// mov recebe key caso key seja um valor válido:
// 'U', 'D', 'L' ou 'R' (Up, Down, Left, Right)
char key;
char mov;

// Indica se houve Game Over (carrinho bateu numa caixa)
bool gameOver;

// A matriz "mapa" é a forma como "enxergamos" o jogo,
// com 4 linhas e 16 colunas.
// A matriz "tela" é a tradução da matriz mapa para o
// formato que o LCD "entende".
byte mapa[4][16];
char tela[2][16];

// linha e coluna armazenam a posição do carrinho.
int linha, coluna;

/*
 * A função inicializaJogo() é chamada na 1a execução
 * e sempre que houver Game Over.
 * Ela coloca o carrinho na posição inicial e limpa
 * a pista.
 */
void inicializaJogo()
{
   // Posicao inicial do carrinho
  linha = 1;
  coluna = 4;
  
  // Jogo nao comeca pausado e nem perdido
  gameOver = 0;

  // Pista comeca vazia
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 16; j++)
    {
      mapa[i][j] = VAZIO;
    }
  }
}

/*
 * A função loop_Input() armazena em key a última tecla
 * que o usuário pressionou. O teste de igualdade evita
 * que um toque longo conte como múltiplos toques.
 */
void loop_Input()
{
  for (int i = 0; i < 100; i++)
  {
    if (key != kpReadKey())
    {
      key = kpReadKey();
    }
  }
}

/*
 * A função loop_Update() testa o valor da tecla pressionada. 
 * Se for um movimento válido, ela altera a posição do carrinho
 * de acordo.
 * 
 */
void loop_Update()
{
  // Caso seja Game Over, reinicie o jogo.
  if (gameOver == 1)
  {
    inicializaJogo();
  }
  // Caso não seja Game Over, atualize as informações do jogo.
  else
  {
    // Dependendo da tecla, mova o carrinho
    if (key != mov){
      switch(key)
      {
        case 'U':
          mov = key;
          if (linha > 0)
            linha--;
          break;
        case 'D':
          mov = key;
          if (linha < 3)
            linha++;
          break;
        case 'L':
          mov = key;
          if (coluna > 0)
            coluna--;
          break;
        case 'R':
          mov = key;
          if (coluna < 15)
            coluna++;
          break;
        default: 
          mov = 0;          
      }
    }
    
    // Mova o mapa
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 15; j++)
      {
        mapa[i][j] = mapa[i][j+1];
      }
    }
      
    // Sorteia se/onde haverá obstáculo 
    // novo conforme a pista avança.
    for (int i = 0; i < 4; i++)
    {
      if (random(50) == 0)
        mapa[i][15] = OBSTACULO;
       else
        mapa[i][15] = VAZIO;
    }
  
    // Se a posição do carrinho coincide com
    // uma posição de obstáculo, game over.
    if (mapa[linha][coluna] == OBSTACULO)
    {
      gameOver = 1;
    }
      
    // Traduz as informações do mapa
    // para desenhinhos que possam ser
    // impressos na tela.
    for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 16; j++)
      {
        if (mapa[2*i][j] == OBSTACULO)
        {
          if ((2*i + 1 == linha) && (j == coluna))
          {
            tela[i][j] = CARROCAIXA2; 
          }
          else
          {
            tela[i][j] = CAIXA1;
          }
        }
        else if (mapa[2*i + 1][j] == OBSTACULO)
        {
          if ((2*i == linha) && (j == coluna))
          {
            tela[i][j] = CARROCAIXA1;
          }
          else
          {
            tela[i][j] = CAIXA2;
          }
        }
        else
        {
          if ((2*i == linha) && (j == coluna))
          {
            tela[i][j] = CARRO1;
          }
          else if ((2*i+1 == linha) && (j == coluna))
          {
            tela[i][j] = CARRO2; 
          }
          else
          {
            tela[i][j] = PISTA;
          }
        }
      }
    }
  }
}

/*
 * Essa é a função que vai efetivamente desenhar na tela.
 */
void loop_Render()
{
  // Move o cursor para a 1a posição do LCD.
  lcdCommand(0x80);
  // Limpa o LCD. 
  lcdCommand(0x01); 

  // Envia as informações do jogo para o LCD.
  for (int i = 0; i < 2; i++)
  {
    for (int j = 0; j < 16; j++)
    {
      lcdChar(tela[i][j]);
    }
    lcdCommand(0xC0); // pula linha
  } 
}

/*
 * O setup() é executado uma vez. Ele inicializa o hardware,
 * o que inclui salvar os nossos desenhos na memória do LCD.
 * Ele é a primeira função a ser executada no programa, que
 * em seguida entra no loop().
 */
void setup() {
  // Inicializando o hardware
  kpInit();
  lcdInit();

  // Criando os desenhos
  byte carro1[8] = {
  0b01010,
  0b11111,
  0b11111,
  0b01010,
  0b00000,
  0b00000,
  0b00000,
  0b00000
  };
  byte carro2[8] = {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b01010,
    0b11111,
    0b11111,
    0b01010
  };
  byte caixa1[8] = {
    0b11111,
    0b10001,
    0b10001,
    0b11111,
    0b00000,
    0b00000,
    0b00000,
    0b00000
  };
  byte caixa2[8] = {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b11111,
    0b10001,
    0b10001,
    0b11111
  };
  byte carrocaixa1[8] = {
    0b01010,
    0b11111,
    0b11111,
    0b01010,
    0b11111,
    0b10001,
    0b10001,
    0b11111
  };
  byte carrocaixa2[8] = {
    0b11111,
    0b10001,
    0b10001,
    0b11111,
    0b01010,
    0b11111,
    0b11111,
    0b01010
  };
  byte pista[8] = {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000
  };
  lcdCreateChar(CARRO1, carro1);
  lcdCreateChar(CARRO2, carro2);
  lcdCreateChar(CAIXA1, caixa1);
  lcdCreateChar(CAIXA2, caixa2);
  lcdCreateChar(CARROCAIXA1, carrocaixa1);
  lcdCreateChar(CARROCAIXA2, carrocaixa2);
  lcdCreateChar(PISTA, pista);

  inicializaJogo();
}

/*
 * Esse é o loop principal do jogo.
 * Ele constantemente lê e processa entradas,
 * atualiza as informações e redesenha a tela.
 */
void loop() {
  loop_Input();
  loop_Update();
  loop_Render();
}
