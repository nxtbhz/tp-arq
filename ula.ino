int pinos[4] = {13, 12, 11, 10}; // pinos de saida para os 4 leds (F3 ate F0)

const int TAM_MEMORIA = 100; // tamanho maximo da memoria (4 regs + 96 instrucoes)
int memoria[TAM_MEMORIA];    // vetor que armazena registradores (0-3) e instrucoes (4-99)
int tamMemoria = 0;          // quantidade de instrucoes carregadas
bool memoriaCheia = false;

void setup()
{
  Serial.begin(9600);

  // configura os pinos dos leds como saida
  for (int i = 0; i < 4; i++)
  {
    pinMode(pinos[i], OUTPUT);
    digitalWrite(pinos[i], LOW);
  }

  memoria[0] = 4; // PC inicia apontando para a primeira instrucao

  Serial.println("ULA pronta. Envie XYZ e depois RUN.");
  dump();
}

void loop()
{
  if (Serial.available() <= 0)
  {
    return;
  }

  // le a entrada enviada pela serial
  String bloco = Serial.readString();
  processarEntrada(bloco);
}

// quebra o texto recebido em tokens separados por espaco, quebra de linha ou virgula
void processarEntrada(String entrada)
{
  entrada.toUpperCase();

  String token = "";
  for (int i = 0; i < entrada.length(); i++)
  {
    char c = entrada.charAt(i);

    if (c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == ';' || c == ',')
    {
      processarToken(token);
      token = "";
    }
    else
    {
      token += c;
    }
  }

  processarToken(token);
}

// interpreta cada comando ou instrucao recebida
void processarToken(String token)
{
  token.trim();
  if (token.length() == 0)
  {
    return;
  }

  // comando para executar o programa
  if (token == "RUN")
  {
    executar();
    return;
  }

  // comando para mostrar a memoria
  if (token == "DUMP")
  {
    dump();
    return;
  }

  // comando para limpar tudo
  if (token == "RESET")
  {
    limpar();
    dump();
    return;
  }

  // se for uma instrucao valida de 3 digitos hex
  if (instrucaoValida(token))
  {
    carregarInstrucao(token);
    return;
  }

  // se for um bloco continuo de instrucoes juntas (ex: C6BA3E)
  if (soHex(token) && token.length() % 3 == 0)
  {
    for (int i = 0; i < token.length(); i += 3)
    {
      if (tamMemoria >= TAM_MEMORIA - 4)
      {
        avisarMemoriaCheia();
        return;
      }

      String parte = token.substring(i, i + 3);
      carregarInstrucao(parte);
    }
    return;
  }

  if (token.length() == 3 && !instrucaoValida(token))
  {
    erro("Instrucao invalida (use 3 digitos hex, ex: C6B): " + token);
    return;
  }

  if (soHex(token) && token.length() % 3 != 0)
  {
    erro("Bloco hex invalido: quantidade de digitos nao e multiplo de 3: " + token);
    return;
  }

  erro("Comando invalido: " + token + " | Use: XYZ, RUN, DUMP ou RESET");
}

// salva a instrucao no vetor de memoria
void carregarInstrucao(String s)
{
  if (tamMemoria < TAM_MEMORIA - 4)
  {
    memoriaCheia = false;
    memoria[4 + tamMemoria] = parseInstrucao(s);
    tamMemoria++;
    memoria[0] = 4; // PC aponta para a primeira instrucao (indice 4)
    dump();
  }
  else
  {
    avisarMemoriaCheia();
  }
}

// mostra aviso se a memoria lotar
void avisarMemoriaCheia()
{
  if (!memoriaCheia)
  {
    erro("Memoria cheia. Apenas as " + String(TAM_MEMORIA - 4) + " primeiras instrucoes foram carregadas.");
    memoriaCheia = true;
  }
}

// imprime mensagem de erro
void erro(String mensagem)
{
  Serial.print("ERRO: ");
  Serial.println(mensagem);
}

// reseta os registradores, memoria e leds
void limpar()
{
  for (int i = 0; i < TAM_MEMORIA; i++)
  {
    memoria[i] = 0;
  }
  memoria[0] = 4; // PC aponta para o inicio das instrucoes
  tamMemoria = 0;
  memoriaCheia = false;
  escreverLeds(0);
}

// executa as instrucoes carregadas usando o PC
void executar()
{
  if (tamMemoria == 0)
  {
    erro("Nenhuma instrucao carregada.");
    return;
  }

  // permite rodar de novo se ja tiver chegado ao fim
  if (memoria[0] >= tamMemoria + 4 || memoria[0] < 4)
  {
    memoria[0] = 4;
  }

  while (memoria[0] < tamMemoria + 4)
  {
    int instr = memoria[memoria[0]];
    int x = (instr >> 8) & 0xF;
    int y = (instr >> 4) & 0xF;
    int op = instr & 0xF;

    // atualiza os registradores X, Y e o resultado W da ULA
    memoria[2] = x;
    memoria[3] = y;
    memoria[1] = ula(x, y, op);

    // atualiza o estado dos leds
    escreverLeds(memoria[1]);

    // incrementa o PC para a proxima instrucao
    memoria[0] = memoria[0] + 1;
    dump();
    delay(4000);
  }

  Serial.println("Programa concluido!");
}

// executa a operacao da ULA de 4 bits conforme a tabela da Figura 2
int ula(int x, int y, int s)
{
  int nx = (~x) & 0xF;
  int ny = (~y) & 0xF;

  if (s == 0x0)
    return nx; // nA
  else if (s == 0x1)
    return (~(x | y)) & 0xF; // AoBn
  else if (s == 0x2)
    return (nx & y) & 0xF; // nAeB
  else if (s == 0x3)
    return 0x0; // zeroL
  else if (s == 0x4)
    return (~(x & y)) & 0xF; // AeBn
  else if (s == 0x5)
    return ny; // nB
  else if (s == 0x6)
    return (x ^ y) & 0xF; // AxB
  else if (s == 0x7)
    return (x & ny) & 0xF; // AenB
  else if (s == 0x8)
    return (nx | y) & 0xF; // nAoB
  else if (s == 0x9)
    return (~(x ^ y)) & 0xF; // AxBn
  else if (s == 0xA)
    return y; // copiaB
  else if (s == 0xB)
    return (x & y) & 0xF; // AeB
  else if (s == 0xC)
    return 0xF; // umL
  else if (s == 0xD)
    return (x | ny) & 0xF; // AonB
  else if (s == 0xE)
    return (x | y) & 0xF; // AoB
  else if (s == 0xF)
    return x; // copiaA
  else
    return 0;
}

// aciona os 4 leds com o valor de W (pino 13 = bit mais significativo)
void escreverLeds(int valor)
{
  digitalWrite(13, ((valor >> 3) & 1) ? HIGH : LOW);
  digitalWrite(12, ((valor >> 2) & 1) ? HIGH : LOW);
  digitalWrite(11, ((valor >> 1) & 1) ? HIGH : LOW);
  digitalWrite(10, ((valor >> 0) & 1) ? HIGH : LOW);
}

// mostra o estado atual da memoria e dos registradores
void dump()
{
  Serial.println("--------------------------------");
  Serial.print("Memoria:       | ");
  for (int i = 0; i < tamMemoria; i++)
  {
    if (i + 4 == memoria[0])
    {
      Serial.print("->");
    }
    printInstrucao(memoria[i + 4]);
    Serial.print(" | ");
  }
  Serial.println();

  Serial.print("Registradores: | ");
  printHex(memoria[0]);
  Serial.print(" | ");
  printHex(memoria[1]);
  Serial.print(" | ");
  printHex(memoria[2]);
  Serial.print(" | ");
  printHex(memoria[3]);
  Serial.println(" |");
}

// verifica se o formato e valido (3 caracteres hex)
bool instrucaoValida(String s)
{
  if (s.length() != 3)
  {
    return false;
  }

  for (int i = 0; i < 3; i++)
  {
    if (!isHex(s.charAt(i)))
    {
      return false;
    }
  }

  return true;
}

// verifica se o caractere e hexadecimal
bool isHex(char c)
{
  return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
}

// verifica se a string inteira e hexadecimal
bool soHex(String s)
{
  if (s.length() == 0)
  {
    return false;
  }

  for (int i = 0; i < s.length(); i++)
  {
    if (!isHex(s.charAt(i)))
    {
      return false;
    }
  }

  return true;
}

// converte caractere hexadecimal para inteiro (0 a 15)
int hexParaInt(char c)
{
  if (c >= '0' && c <= '9')
  {
    return c - '0';
  }
  return 10 + (c - 'A');
}

// empacota a instrucao XYZ em um inteiro de 12 bits
int parseInstrucao(String s)
{
  int x = hexParaInt(s.charAt(0));
  int y = hexParaInt(s.charAt(1));
  int op = hexParaInt(s.charAt(2));
  return (x << 8) | (y << 4) | op;
}

// imprime um numero em hexadecimal (0 a F)
void printHex(int n)
{
  n = n & 0xF;
  if (n < 10)
  {
    Serial.print((char)('0' + n));
  }
  else
  {
    Serial.print((char)('A' + (n - 10)));
  }
}

// imprime a instrucao no formato de 3 digitos hex
void printInstrucao(int instr)
{
  int x = (instr >> 8) & 0xF;
  int y = (instr >> 4) & 0xF;
  int op = instr & 0xF;
  printHex(x);
  printHex(y);
  printHex(op);
}