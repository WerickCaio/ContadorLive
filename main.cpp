// =================================
// --- Especificações de projeto ---
// Projeto: ContadorLive
// Descrição: Objetivo do projeto será marcar um placar de modo que seja controlável com um controle remoto a adição e restart do placar a distância
// Projetista: Werick Caio
// Data: 07/02/24
//

// ===================================
// --- Inclusão das bibliotecas ---
#include <Arduino.h>
#include <SPI.h>
#include <LedControl.h>
#include <Drawing.h> // Arquivo com os caracteres

// =================================
// --- Mapeamento de Hardware ---
#define CLK (1 << PORTB5)  // 13 Essa definição serve para usar o clock do painel sem usar a função da biblioteca
#define DS (1 << PORTB2)   // 10 // Data select
#define MOSI (1 << PORTB3) // 11

#define LAT (1 << PORTD7) // 7
#define A (1 << PORTD2)   // 2
#define B (1 << PORTD3)   // 3
#define OE (1 << PORTD5)  //  // Vai passar a ser o 5, pois tem sinal de pwm

// #define DATAPIN   (1<<PORTD2)//  2 // (1<<PORTD2)
// #define G1  (1<<PORTD3)//  3 // (1<<PORTD3)
// #define B1  (1<<PORTD4)//  4 // (1<<PORTD4)
// #define R2  (1<<PORTD5)//  5 // (1<<PORTD5)
// #define G2  (1<<PORTD6)//  6 // (1<<PORTD6)
// #define B2  (1<<PORTD7)//  7 // (1<<PORTD7)

// =============================
// --- Protótipo das funções ---
void latchPanel();
void clearAll();
void ativaLinhaPar();
void ativaLinhaImpar();
void melquisedeque(); // Função de Toggle do estado do pino
void updatePanel();
void putPixel(uint8_t x, uint8_t y, char color);
void putLine();
void putPixelInTheFirstLine(uint8_t posicao);
void putPixelInLine(uint8_t x, uint8_t y);
void putAnyPixelFirstPanel(uint8_t x, uint8_t y);
void desativaTudo();
void putPixelMemory(int x, int y, char color);
void drawChar(int xi, int yi, bool ch[], char color);
//==============================
// --- Declaração de objetos ---

// // Declarações dos protótipos das funções
// void clearPanel();
// void ativaLinhaImpar();
// void ativaLinhaPar();
// void desativaTudo();
// void latch();
// void cleanSerial(); // limpa

// void dumpVariable();
// void mostraValorSerial();

// void putPixel(int x, int y);
// uint64_t rotate(uint64_t v, int n);
// void PutPixelUnico(int x, int y);
// void PutlineUnica(int linha);
// void myPutChar(bool ch[], int xi, int yi);
// // RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

// ==============================
// --- Definição de variáveis ---
// Primeiro de tudo, a variável referente aos primeiros 64 bits de informações está sendo declarada a seguir
// uint64_t Descarga1 = 0; // Uma variável de 64 bits que deve ser referente a primeira linha do display de 16x64
// Provavelmente terá que ser um vetor com 16 bixos desse, como:
uint64_t LedPanelRed[16];   // referente aos 1024 pontos de luz (Mas ainda apenas de uma cor)
uint64_t LedPanelGreeb[16]; // referente aos 1024 pontos de luz (Mas ainda apenas de uma cor)
uint64_t LedPanelBlue[16];  // referente aos 1024 pontos de luz (Mas ainda apenas de uma cor)
uint64_t LedPanelRGB[48];   // referente aos 3072 pontos de luz [ 0 - 1023 = Azul; 1024 - 2047 = Verde;

unsigned long tempodecorrido = 0;

// =============================
// --- Função de configuração ---
void setup()
{
  // put your setup code here, to run once:
  SPI.begin();
  Serial.begin(115200);
  digitalWrite(SS, HIGH);

  // Configuração das portas
  DDRD |= A | B | LAT; // | OE;
  DDRB |= CLK | MOSI | DS;

  // Estado inicial de pinos
  // PORTB &= ~OE; // Seta nível lógico baixo em Output Enable
  pinMode(5, OUTPUT);
  analogWrite(5, 125);

  // Temporário, mas deixará apenas uma das linhas ativas (Par selecionada)
  ativaLinhaPar();

  // // Função temporária para escrever um valor em LedPanelRGB[]
  //   uint64_t unidade = 1;
  //   uint64_t tempotu = 63;
  //   LedPanelRGB[32] = unidade<<tempotu; // Ou seja, a primeira linha do Azul deve ser colocada
  //   LedPanelRGB[33] = unidade<<tempotu; // Ou seja, a primeira linha do Azul deve ser colocada
  //   LedPanelRGB[16] = unidade<<tempotu; // Ou seja, a primeira linha do Azul deve ser colocada
  //   LedPanelRGB[17] = unidade<<tempotu; // Ou seja, a primeira linha do Azul deve ser colocada
  //   LedPanelRGB[19] = unidade<<tempotu; // Ou seja, a primeira linha do Azul deve ser colocada
  //   LedPanelRGB[0] = unidade<<tempotu; // Ou seja, a primeira linha do Azul deve ser colocada
  //   LedPanelRGB[1] = unidade<<tempotu; // Ou seja, a primeira linha do Azul deve ser colocada
  //   // LedPanelRGB[15] = ~0;

  // //   for (size_t i = 1; i < 15; i++)
  // // {
  // //   uint64_t unidade = 1;
  // //   uint64_t tempar = 63;
  // //   LedPanelRGB[i+16] |=  ((unidade << tempar) | 1);
  //   // LedPanelRGB[16] = 0xffffffff;
  // //   // uint32_t temp1 = LedPanelRGB[i]>>32;
  // //   // uint32_t temp2 = LedPanelRGB[i];
  // //   // Serial.println(temp1,HEX);
  // //   // Serial.println(temp2,HEX);
  // // }
  // uint32_t temp1 = LedPanelRGB[32]>>32;
  // uint32_t temp2 = LedPanelRGB[32];
  // Serial.println(temp1,BIN);
  // Serial.println(temp2,BIN);
  // temp1 = LedPanelRGB[16]>>32;
  //  temp2 = LedPanelRGB[16];
  // Serial.println(temp1,BIN);
  // Serial.println(temp2,BIN);
  //  temp1 = LedPanelRGB[0]>>32;
  //  temp2 = LedPanelRGB[0];
  // Serial.println(temp1,BIN);
  // Serial.println(temp2,BIN);
  // putPixelMemory(0, 0, 'r');
  // putPixelMemory(1, 1, 'r');
  // putPixelMemory(2, 2, 'r');
  // putPixelMemory(3, 3, 'r');
  // putPixelMemory(4, 4, 'r');

  // putPixelMemory(0, 0, 'g');
  // putPixelMemory(2, 2, 'b');
  // // putPixelMemory(0, 0, 'b');
  // drawChar(5, 5, Heart, 'r');
}
int x = 0;
// ============================
// --- Função de repetição ---
void loop()
{
  // tempodecorrido = millis();
  // putLine();
  // putLine();
  // putPixelInLine(10);
  // for(int i = 0; i < 4; i++)
  // {
  //   for
  // }

  // uint16_t tester = 1;
  // melquisedeque();
  // SPI.transfer16(tester);
  // melquisedeque();
  // latchPanel();
  // clearAll();
  // putAnyPixelFirstPanel(uni,  uni);
  // putAnyPixelFirstPanel(4,  4);
  // putAnyPixelFirstPanel(5,  5);
  updatePanel();
  // putAnyPixelFirstPanel(9,  9);
  // putAnyPixelFirstPanel(9,  9);
  // putAnyPixelFirstPanel(9,  9);
  // putAnyPixelFirstPanel(9,  9);
  // putAnyPixelFirstPanel(9,  9);
  // putAnyPixelFirstPanel(9,  9);
  // putAnyPixelFirstPanel(9,  9);
  // putAnyPixelFirstPanel(9,  9);
  // putAnyPixelFirstPanel(9,  9);
  // putAnyPixelFirstPanel(9,  9);
  // putAnyPixelFirstPanel(9,  9);
  // putAnyPixelFirstPanel(9,  9);
  if ( millis() - tempodecorrido  >= 2000)
  {
    drawChar(5, x, Heart, 'r');
    tempodecorrido = millis();
    clearAll();
    latchPanel();
    x++;
  }


  // uni++;
  // clearAll();
  // if (uni == 128)
  //   uni = 0;
  // delay(1000);
}

void clearAll()
{
  PORTD |= OE;
  uint16_t zeraAll = 0; // 0b0000000000000000
  for (byte allLines = 0; allLines < 96; allLines++)
  {
    melquisedeque();
    SPI.transfer16(zeraAll);
    melquisedeque();
  }
  latchPanel();
  PORTD &= ~OE;

}

void latchPanel()
{
  PORTD |= LAT;
  PORTD &= ~LAT;
}

void ativaLinhaImpar() // Seta B como 1 e A como 0
{
  PORTD |= B;
  PORTD &= ~A;
}
void ativaLinhaPar() // Seta A como 1 e B como 0
{
  PORTD |= A;
  PORTD &= ~B;
}

void melquisedeque() // Vai ficar alterando o valor de SS, para que o código rode de forma decente
{
  uint8_t state = PORTB & DS;
  if (state)
  {
    PORTB &= ~DS;
  }
  else
  {
    PORTB |= DS;
  }
}

void toggleOE() // Vai ficar alterando o valor de SS, para que o código rode de forma decente
{
  uint8_t state = PORTB & DS;
  if (state)
  {
    PORTB &= ~DS;
  }
  else
  {
    PORTB |= DS;
  }
}

void updatePanel()
{
  // Ou seja, deve haver manipulação dessas variáveis, para que a imagem seja formada no painel

  // uint8_t displayID = 0;
  //  i será referente as linhas pares e J referente às linhas ímpares
  //  A divisão de linhas por display será da seguinte maneira
  //  Display 1   Display 2   Display 3   Display 4
  //  0 a 15      16 a 31     32 a 47     48 a 63
  desativaTudo();
  // Nesse caso, o primeiro a ser executado é a última linha do 4 display
  //  Serial.println("Ativando o Impar");

  for (int cores = 2; cores >= 0; cores--) // quando for dois, o valor de adição é 2*16 + ValorAtual = azul
  {
    for (int displayID = 3; displayID >= 0; displayID--) // Isso é a indicação de 4 displays
    {
      for (int linhas = (15 + cores * 16); linhas >= (cores * 16); linhas -= 2) // Vai descarregar (0,2,4,6,8,10,12,14) 8 linhas de cada painel
      // Ou seja, essa função tem que ser executada 4 vezes para varrer uma das cores
      {
        // for (int i = (15 + (displayID * 16)) ; i >= (displayID*16) ; i--) //até que esse for acabe a primeira vez, ele terá varrido os últimos 16
        // {
        //   // int valorAtual = ((LedPanelComplete[linhas] >> i) & 0x00000001) ;// Após essa operação, só resta o bit atual

        //   // // Preciso que o valor atual seja descarregado em G1, de modo que não comprometa todo o porto
        //   // PORTD &= ~G1 & ~B1 & ~R1; // Isso significa zerar o valor do porto naquele ponto

        //   // PORTD |= valorAtual<<PORTD3;
        //   // passData();

        // }
        uint16_t slicedInfo = (LedPanelRGB[(linhas)] >> 16 * displayID); // Analizar para saber se esse shiftrealmente acontece
        // Serial.println(slicedInfo, BIN);
        // uint32_t temp1 = LedPanelRGB[linhas]>>32;
        // uint32_t temp2 = LedPanelRGB[linhas];
        // Serial.println(temp1,HEX);
        // Serial.println(temp2,HEX);
        // Serial.print("Cores: ");
        // Serial.println(cores);
        // Serial.print("displayID: ");
        // Serial.println(displayID);
        // Serial.print("linhas: ");
        // Serial.println(linhas);

        melquisedeque();
        SPI.transfer16(slicedInfo);
        melquisedeque();
        // latchPanel();
      }
      // Serial.println("Sai do for de linha");
    }

    // Serial.println("Sai do for de display");
    // delay(50);
  }
  // Serial.println("Sai do for geral");
  ativaLinhaPar();
  latchPanel();
  _delay_ms(1);
  // Serial.println("Sinal Enviado");

  desativaTudo();
  _delay_ms(1);

  // Serial.println("Ativando o Par");

  for (int cores = 2; cores >= 0; cores--) // quando for dois, o valor de adição é 2*16 + ValorAtual = azul
  {
    for (int displayID = 3; displayID >= 0; displayID--) // Isso é a indicação de 4 displays
    {
      for (int linhas = (14 + cores * 16); linhas >= (cores * 16); linhas -= 2) // Vai descarregar (0,2,4,6,8,10,12,14) 8 linhas de cada painel
      // Ou seja, essa função tem que ser executada 4 vezes para varrer uma das cores
      {
        // for (int i = (15 + (displayID * 16)) ; i >= (displayID*16) ; i--) //até que esse for acabe a primeira vez, ele terá varrido os últimos 16
        // {
        //   // int valorAtual = ((LedPanelComplete[linhas] >> i) & 0x00000001) ;// Após essa operação, só resta o bit atual

        //   // // Preciso que o valor atual seja descarregado em G1, de modo que não comprometa todo o porto
        //   // PORTD &= ~G1 & ~B1 & ~R1; // Isso significa zerar o valor do porto naquele ponto

        //   // PORTD |= valorAtual<<PORTD3;
        //   // passData();

        // }
        uint16_t slicedInfo = (LedPanelRGB[(linhas)] >> 16 * displayID); // Analizar para saber se esse shiftrealmente acontece
        // Serial.println(slicedInfo, BIN);
        // uint32_t temp1 = LedPanelRGB[linhas]>>32;
        // uint32_t temp2 = LedPanelRGB[linhas];
        // Serial.println(temp1,HEX);
        // Serial.println(temp2,HEX);
        // Serial.print("Cores: ");
        // Serial.println(cores);
        // Serial.print("displayID: ");
        // Serial.println(displayID);
        // Serial.print("linhas: ");
        // Serial.println(linhas);
        //
        melquisedeque();
        SPI.transfer16(slicedInfo);
        melquisedeque();
        // latchPanel();
      }
      // Serial.println("Sai do for de linha");
    }

    // Serial.println("Sai do for de display");
    // delay(50);
  }
  // Serial.println("Sai do for geral");
  ativaLinhaImpar();
  latchPanel();
  _delay_ms(1);
  desativaTudo();
  // Serial.println("Sinal Enviado 2");
  // Nesse caso, o primeiro a ser executado é a última linha do 4 display
  // ativaLinhaImpar();
  // for (int cores = 2; cores >= 0; cores--) // quando for dois, o valor de adição é 2*16 + ValorAtual = azul
  // {
  //   for (int displayID = 3; displayID >= 0; displayID--) // Isso é a indicação de 4 displays
  //   {
  //     for (int linhas = (1 + cores*16) ; linhas < (16 + cores*16) ; linhas+=2) // Vai descarregar (0,2,4,6,8,10,12,14) 8 linhas de cada painel
  //     // Ou seja, essa função tem que ser executada 4 vezes para varrer uma das cores
  //     {
  //       // for (int i = (15 + (displayID * 16)) ; i >= (displayID*16) ; i--) //até que esse for acabe a primeira vez, ele terá varrido os últimos 16
  //       // {
  //       //   // int valorAtual = ((LedPanelComplete[linhas] >> i) & 0x00000001) ;// Após essa operação, só resta o bit atual

  //       //   // // Preciso que o valor atual seja descarregado em G1, de modo que não comprometa todo o porto
  //       //   // PORTD &= ~G1 & ~B1 & ~R1; // Isso significa zerar o valor do porto naquele ponto

  //       //   // PORTD |= valorAtual<<PORTD3;
  //       //   // passData();

  //       // }
  //       uint16_t slicedInfo = (LedPanelRGB[linhas]>>16*displayID); // Analizar para saber se esse shiftrealmente acontece

  //       // uint32_t temp1 = LedPanelRGB[linhas]>>32;
  //       // uint32_t temp2 = LedPanelRGB[linhas];
  //       // Serial.println(temp1,HEX);
  //       // Serial.println(temp2,HEX);

  //       melquisedeque();
  //       SPI.transfer16(slicedInfo);
  //       melquisedeque();
  //       // latchPanel();
  //     }
  //     // Serial.println("Sai do for de linha");
  //   }
  //     // Serial.println("Sai do for de display");
  //   // delay(50);
  // }
  //     // Serial.println("Sai do for geral");
  // latchPanel();
}

void putLine()
{
  uint16_t temp = 0xffff;
  melquisedeque();
  SPI.transfer16(temp);
  melquisedeque();
  latchPanel();
}

void putPixelInTheFirstLine(uint8_t posicao)
{
  uint16_t temp = 0x0000;
  temp |= 1 << posicao;
  melquisedeque();
  SPI.transfer16(temp);
  melquisedeque();
  latchPanel();
}

void putPixelInLine(uint8_t x, uint8_t y)
{
  uint16_t skip = 0x0000;
  uint16_t temp = 0x0000;
  temp |= 1 << (x);
  melquisedeque();
  SPI.transfer16(temp);
  melquisedeque();
  for (uint8_t linhas = 0; linhas < y; linhas++)
  {
    melquisedeque();
    SPI.transfer16(skip);
    melquisedeque();
  }

  latchPanel();
}

void putAnyPixelFirstPanel(uint8_t x, uint8_t y)
{
  uint16_t skip = 0x0000;
  uint16_t temp = 0x0000;
  temp |= 1 << (x);

  if (y % 2 == 0)
  {
    ativaLinhaPar();
    melquisedeque();
    SPI.transfer16(temp);
    melquisedeque();
  }
  else
  {
    ativaLinhaImpar();
    melquisedeque();
    SPI.transfer16(temp);
    melquisedeque();
  }
  for (uint8_t linhas = 0; linhas < y - 1; linhas += 2)
  {
    melquisedeque();
    SPI.transfer16(skip);
    melquisedeque();
  }

  latchPanel();
}

// void putPixelMem(int x, int y, char)
// {
//   uint16_t temp = 0xffff;
//   melquisedeque();
//   SPI.transfer16(temp);
//   melquisedeque();
//   latchPanel();
// }
void putPixelMemory(int x, int y, char color)
// 0 <= x <= 64; 0 <= y <= 16
{
  uint64_t unidade = 1;
  uint64_t tempar = 63 - x;
  int multiplier = 0;
  switch (color)
  {
  case 'r':
    multiplier = 0;
    LedPanelRGB[y + (multiplier*16)] |= ((unidade << tempar));
    // uint32_t temp1 = LedPanelRGB[i]>>32;
    // uint32_t temp2 = LedPanelRGB[i];
    // Serial.println(temp1,HEX);
    // Serial.println(temp2,HEX);

    break;
  case 'g':
    /* code */
    multiplier = 1;
    LedPanelRGB[y + (multiplier*16)] |= ((unidade << tempar));
    break;

  case 'b':
    /* code */
    multiplier = 2;
    LedPanelRGB[y + (multiplier*16)] |= ((unidade << tempar));
    break;
  }
}

void drawChar(int xi, int yi, bool ch[], char color)
{
  uint64_t unidade = 1;
  uint64_t tempar = 63 - x;
  int multiplier = 0;
  switch (color)
  {
  case 'r':
    multiplier = 0;
    for (size_t x = 0; x < 5; x++)
  {
    for (size_t y = 0; y < 5; y++)
    {
      // delay(50);
      if (ch[5*x + y])
      {
        putPixelMemory(y + yi, x + xi, 'r');
      }
      // else{
      //    passData();     
      // }
    }
  }
    
    // LedPanelRGB[y + (multiplier*16)] |= ((unidade << tempar));
    // uint32_t temp1 = LedPanelRGB[i]>>32;
    // uint32_t temp2 = LedPanelRGB[i];
    // Serial.println(temp1,HEX);
    // Serial.println(temp2,HEX);

    break;
  case 'g':
    /* code */
    multiplier = 1;
    LedPanelRGB[yi + (multiplier*16)] |= ((unidade << tempar));
    break;

  case 'b':
    /* code */
    multiplier = 2;
    LedPanelRGB[yi + (multiplier*16)] |= ((unidade << tempar));
    break;
  }
}

// void myPutChar(bool ch[], int xi , int yi ){
  
//   for (size_t x = 0; x < 5; x++)
//   {
//     for (size_t y = 0; y < 4; y++)
//     {
//       // delay(50);
//       if (ch[4*x + y])
//       {
//         putPixelMemory(y + yi, x + xi);
//       }
//       else{
//          passData();     
//       }
//     }
//   }
// }
// --- FIM DE CÓDIGO ---
// =====================

// //==============================
// // --- Declaração de objetos ---

// // Declarações dos protótipos das funnções
// void clearPanel();
// void ligaVermelho();
// void ligaVerde();
// void ligaAzul();
// void ativaLinhaImpar();

// void desativaTudo();
// void latch();

// void cleanSerial();
// void dumpVariable();
// void mostraValorSerial();

// void putPixel(int x, int y);
// uint64_t rotate(uint64_t v, int n);
// void PutPixelUnico(int x, int y);
// void PutlineUnica(int linha);
// void myPutChar(bool ch[], int xi, int yi);
// // RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

// void loop()
// {
//   byte test = 0b10011001;
//   // bool varredura[1024];
//   // A variável em questão tem 64 bits, referentes as linhas do multiplexador
//   // for(int i = 0; i < 64; i++)
//   //   // Tenho que fazer com que esse valor altere apenas o registro de G1
//   //   PORTD |= (varredura >> i) ; // preenche a variável bool com 1

//   // // Descarrega a variável bool no shiftregister do painel
//   // for(int i = 0; i<1024; i++)
//   // {
//   //   (varredura >> i)
//   // }

//   // Pelo motivo da variável ter que ser zerada, terá que ter um laço de repetição.
//   // for (int zero = 0; zero < 5 ; zero++){
//   //   LedPanelComplete[zero] = 0xFFFFFFF1;
//   // }

// void clearPanel()
// {
//   digitalWrite(G1, LOW);
//     for (int i = 0; i<127; i++)
//     {
//       digitalWrite(CLK, HIGH);
//       delay(1);
//       digitalWrite(CLK, LOW);
//       delay(1);

//     }
//         digitalWrite(LAT, HIGH);
//         delay(10);
//         digitalWrite(LAT, LOW);
//         delay(10);
//         delay(10000);

//   digitalWrite(G1, HIGH);
// }

// void ligaVermelho()
// {
// // Configura vermelho como alto
//     PORTD |= R1;
//     PORTD &= ~G1 & ~B1;
// }

// void ligaVerde()
// {
// // Configura verde como alto
//     PORTD |= G1;
//     PORTD &= ~R1 & ~B1;
// }

// void ligaAzul()
// {
// // Configura vermelho como alto
//     PORTD |= B1;
//     PORTD &= ~G1 & ~R1;
// }

// void ativaLinhaImpar()
// {
//     PORTC |= A;
//     PORTC &= ~B;
// }

void desativaTudo()
{
  PORTC |= B;
  PORTC |= A;
}

// void cleanSerial()
// / {
// //   PORTD &= ~G1 & ~B1 &~R1 &~B2 &~G2 & R2;
//   for (int i = 0; i< 1024; i++)
//   {
//     passData();
//   }
//   latch();
// }

// void dumpVariableShifted() // Por enquanto ainda não foi feita alteração alguma de código
// {
//   //uint8_t displayID = 0;
//   // i será referente as linhas pares e J referente às linhas ímpares
//   // A divisão de linhas por display será da seguinte maneira
//   // Display 1   Display 2   Display 3   Display 4
//   // 0 a 15      16 a 31     32 a 47     48 a 63

//   //Nesse caso, o primeiro a ser executado é a última linha do 4 display
//   ativaLinhaPar();
//   for (int displayID = 3; displayID >= 0; displayID--)
//   {
//     for (int linhas = 0 ; linhas < 16 ; linhas+=2)
//     {
//       for (int i = (15 + (displayID * 16)) ; i >= (displayID*16) ; i--) //até que esse for acabe a primeira vez, ele terá varrido os últimos 16
//       {
//         int valorAtual = ((LedPanelComplete[linhas] >> i) & 0x00000001) ;// Após essa operação, só resta o bit atual

//         // Preciso que o valor atual seja descarregado em G1, de modo que não comprometa todo o porto
//         PORTD &= ~G1 & ~B1 & ~R1; // Isso significa zerar o valor do porto naquele ponto

//         PORTD |= valorAtual<<PORTD3;
//         passData();
//       }
//     }
//   }
//     latch();
//     // delay(50);

//   //Nesse caso, o primeiro a ser executado é a última linha do 4 display
//   ativaLinhaImpar();
//   for (int displayID = 3; displayID >= 0; displayID--)
//   {
//     for (int linhas = 1 ; linhas < 16 ; linhas+=2)
//     {
//       for (int i = (15 + (displayID * 16)) ; i >= (displayID*16) ; i--) //até que esse for acabe a primeira vez, ele terá varrido os últimos 16
//       {
//         int valorAtual = ((LedPanelComplete[linhas] >> i) & 0x00000001) ;// Após essa operação, só resta o bit atual

//         // Preciso que o valor atual seja descarregado em G1, de modo que não comprometa todo o porto
//         PORTD &= ~G1 & ~B1 & ~R1; // Isso significa zerar o valor do porto naquele ponto

//         PORTD |= valorAtual<<PORTD3;
//         passData();
//       }
//     }
//   }
//     latch();
//     // delay(50);

// }
// void mostraValorSerial() // função teste para descarregar o valor de LedPanelComplete na seral
// {
//   if ((millis() - tempoCorrido) > 5000)
//   {
//     for(int j = 0 ; j< 16; j++)
//       {
//         Serial.println();
//         Serial.print("Linha ");
//         Serial.print(j);
//         Serial.print(" : ");

//         for(int i = 0; i < 64; i++)
//         {
//           bool test = ((LedPanelComplete[j]>>i)&0x00000001);
//           Serial.print(test);
//           Serial.print(" ");
//         }

//         Serial.println();
//       }
//       tempoCorrido = millis();
//   }
// }

// void putPixel(int x, int y)
// {
//   LedPanelComplete[3] = 1 << 14;
//   Serial.println ("O valor 1 foi colocado nas posicoes: x:");
//   Serial.print(x);
//   Serial.print(" e ");
//   Serial.print(y);
//   Serial.println("");
//   // delay (500);

//   uint32_t hadouken = LedPanelComplete[3]>>32;
//   uint32_t hadouken2 = LedPanelComplete[3];

//   Serial.println(hadouken);
//   Serial.println(hadouken2);

//   // //Nesse caso, o primeiro a ser executado é a última linha do 4 display
//   // ativaLinhaPar();
//   // for (int displayID = 3; displayID >= 0; displayID--)
//   // {
//   //   for (int linhas = 0 ; linhas < 16 ; linhas+=2)
//   //   {
//   //     for (int i = (15 + (displayID * 16)) ; i >= (displayID*16) ; i--) //até que esse for acabe a primeira vez, ele terá varrido os últimos 16
//   //     {
//   //       int valorAtual = ((LedPanelComplete[linhas] >> i) & 0x00000001) ;// Após essa operação, só resta o bit atual

//   //       // Preciso que o valor atual seja descarregado em G1, de modo que não comprometa todo o porto
//   //       PORTD &= ~G1 & ~B1 & ~R1; // Isso significa zerar o valor do porto naquele ponto

//   //       PORTD |= valorAtual<<PORTD3;
//   //       passData();
//   //     }
//   //   }
//   // }
//   //   latch();
//   //   // delay(50);

//   // //Nesse caso, o primeiro a ser executado é a última linha do 4 display
//   // ativaLinhaImpar();
//   // for (int displayID = 3; displayID >= 0; displayID--)
//   // {
//   //   for (int linhas = 1 ; linhas < 16 ; linhas+=2)
//   //   {
//   //     for (int i = (15 + (displayID * 16)) ; i >= (displayID*16) ; i--) //até que esse for acabe a primeira vez, ele terá varrido os últimos 16
//   //     {
//   //       int valorAtual = ((LedPanelComplete[linhas] >> i) & 0x00000001) ;// Após essa operação, só resta o bit atual

//   //       // Preciso que o valor atual seja descarregado em G1, de modo que não comprometa todo o porto
//   //       PORTD &= ~G1 & ~B1 & ~R1; // Isso significa zerar o valor do porto naquele ponto

//   //       PORTD |= valorAtual<<PORTD3;
//   //       passData();
//   //     }
//   //   }
//   // }
//   //   latch();
//   //   // delay(50);

// }

// uint64_t rotate(uint64_t v, int n)
// {
//     int refeferencia = 0;
//     n = n & 63;
//     if (n)
//         v = (v >> n) | (v << (32-n));
//     return v;
//      }

// void PutPixelUnico(int x, int y)
// {
//   //uint8_t displayID = 0;
//   // i será referente as linhas pares e J referente às linhas ímpares
//   // A divisão de linhas por display será da seguinte maneira
//   // Display 1   Display 2   Display 3   Display 4
//   // 0 a 15      16 a 31     32 a 47     48 a 63

//   //Nesse caso, o primeiro a ser executado é a última linha do 4 display
//   ativaLinhaPar();
//   for (int displayID = 3; displayID >= 0; displayID--)
//   {
//     for (int linhas = 0 ; linhas < 16 ; linhas+=2)
//     {
//       for (int i = (15 + (displayID * 16)) ; i >= (displayID*16) ; i--) //até que esse for acabe a primeira vez, ele terá varrido os últimos 16
//       //  for (int i = 0 ; i < 16 ; i++) //até que esse for acabe a primeira vez, ele terá varrido os últimos 16
//       {
//         // int valorAtual = ((LedPanelComplete[linhas] >> i) & 0x00000001) ;// Após essa operação, só resta o bit atual

//         // Preciso que o valor atual seja descarregado em G1, de modo que não comprometa todo o porto
//         // PORTD &= ~G1 & ~B1 & ~R1; // Isso significa zerar o valor do porto naquele ponto

//         // PORTD |= valorAtual<<PORTD3;
//         int valorDisplay = (int)x / 16;
//         int valorId = x%16;
//         // Serial.println(valorDisplay);
//         // Serial.println(valorId);
//         // Serial.println(linhas);

//         if (displayID == valorDisplay && linhas == y && valorId == (i%16))
//         {
//           // Serial.println("Entrei no if");
//           PORTD |= G1;
//           // delay(1);
//         }
//         passData();
//         PORTD &= ~G1;
//       }
//     }
//   }
//     latch();
//     desativaTudo();
//     // delay(50);

//   ativaLinhaImpar();
//   for (int displayID = 3; displayID >= 0; displayID--)
//   {
//     for (int linhas = 1 ; linhas < 16 ; linhas+=2)
//     {
//       for (int i = (15 + (displayID * 16)) ; i >= (displayID*16) ; i--) //até que esse for acabe a primeira vez, ele terá varrido os últimos 16
//       //  for (int i = 0 ; i < 16 ; i++) //até que esse for acabe a primeira vez, ele terá varrido os últimos 16
//       {
//         // int valorAtual = ((LedPanelComplete[linhas] >> i) & 0x00000001) ;// Após essa operação, só resta o bit atual

//         // Preciso que o valor atual seja descarregado em G1, de modo que não comprometa todo o porto
//         // PORTD &= ~G1 & ~B1 & ~R1; // Isso significa zerar o valor do porto naquele ponto

//         // PORTD |= valorAtual<<PORTD3;
//         int valorDisplay = (int)x / 16;
//         int valorId = x%16;
//         // Serial.println(valorDisplay);
//         // Serial.println(valorId);
//         // Serial.println(linhas);

//         if (displayID == valorDisplay && linhas == y && valorId == (i%16))
//         {
//           // Serial.println("Entrei no if");
//           PORTD |= G1;
//           // delay(1);
//         }
//         passData();
//         PORTD &= ~G1;
//       }
//     }
//   }
//     latch();
//     desativaTudo();
// }

// void PutlineUnica(int linhachoosed)
// {
//   //uint8_t displayID = 0;
//   // i será referente as linhas pares e J referente às linhas ímpares
//   // A divisão de linhas por display será da seguinte maneira
//   // Display 1   Display 2   Display 3   Display 4
//   // 0 a 15      16 a 31     32 a 47     48 a 63

//   //Nesse caso, o primeiro a ser executado é a última linha do 4 display
//   ativaLinhaPar();
//   for (int displayID = 3; displayID >= 0; displayID--)
//   {
//     for (int linhas = 0 ; linhas < 16 ; linhas+=2)
//     {
//       for (int i = (15 + (displayID * 16)) ; i >= (displayID*16) ; i--) //até que esse for acabe a primeira vez, ele terá varrido os últimos 16
//       //  for (int i = 0 ; i < 16 ; i++) //até que esse for acabe a primeira vez, ele terá varrido os últimos 16
//       {
//         // int valorAtual = ((LedPanelComplete[linhas] >> i) & 0x00000001) ;// Após essa operação, só resta o bit atual

//         // Preciso que o valor atual seja descarregado em G1, de modo que não comprometa todo o porto
//         // PORTD &= ~G1 & ~B1 & ~R1; // Isso significa zerar o valor do porto naquele ponto

//         // PORTD |= valorAtual<<PORTD3;
//         // Serial.println(valorDisplay);
//         // Serial.println(valorId);
//         // Serial.println(linhas);

//         if (linhas == linhachoosed)
//         {
//           // Serial.println("Entrei no if");
//           PORTD |= G1;
//           passData();
//           // delay(1);
//         }
//         passData();
//         PORTD &= ~G1;
//       }
//     }
//   }
//     latch();
//     desativaTudo();
//     // delay(50);

//   ativaLinhaImpar();
//   for (int displayID = 3; displayID >= 0; displayID--)
//   {
//     for (int linhas = 1 ; linhas < 16 ; linhas+=2)
//     {
//       for (int i = (15 + (displayID * 16)) ; i >= (displayID*16) ; i--) //até que esse for acabe a primeira vez, ele terá varrido os últimos 16
//       //  for (int i = 0 ; i < 16 ; i++) //até que esse for acabe a primeira vez, ele terá varrido os últimos 16
//       {
//         // int valorAtual = ((LedPanelComplete[linhas] >> i) & 0x00000001) ;// Após essa operação, só resta o bit atual

//         // Preciso que o valor atual seja descarregado em G1, de modo que não comprometa todo o porto
//         // PORTD &= ~G1 & ~B1 & ~R1; // Isso significa zerar o valor do porto naquele ponto

//         // PORTD |= valorAtual<<PORTD3;
//         // Serial.println(valorDisplay);
//         // Serial.println(valorId);
//         // Serial.println(linhas);

//         if (linhas == linhachoosed)
//         {
//           // Serial.println("Entrei no if");
//           PORTD |= G1;
//           // delay(1);
//           passData();
//         }
//         passData();
//         PORTD &= ~G1;
//       }
//     }
//   }
//     latch();
//     desativaTudo();
// }

// void myPutChar(bool ch[], int xi , int yi ){

//   for (size_t x = 0; x < 5; x++)
//   {
//     for (size_t y = 0; y < 4; y++)
//     {
//       // delay(50);
//       if (ch[4*x + y])
//       {
//         PutPixelUnico(y + yi, x + xi);
//       }
//       else{
//          passData();
//       }
//     }

//   }

// }