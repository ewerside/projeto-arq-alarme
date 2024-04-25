// C++ cod
/*
Amanda Fernandes Pereira - 202309924901
Ewerton Peixoto de Alencar Arrais - 202307164496
João Gabriel Tasca Barroso de Assis -202307623911
Miguel Veiga Gonçalves - 202305105727
Paula Luiza Rocha de Oliveira - 202309747511

se a entrada da porta 13 for HIGH o buzzer que 
esta na porta 12 recebera HIGH, isso so acontecera se o horario
estiver entre os horarios setados pelos dipswitchs(c1)
{A1 ate A5 "hr final",6 ate 2 "hr inicial", e nao pode passar de 24horas}, 
e isso so acontecera se o sistema receber HIGH na porta 11
*/ 

#define Sensor  2
#define Alarme  4
#define LED_verde  3
// definicao dos pinos para horario inicial
#define p2  45
#define p3  47
#define p4  49
#define p5  51
#define p6  53
// definicao dos pinos para horario final
#define f1 33
#define f2 35
#define f3 37
#define f4 39
#define f5 41
// definicao dos pinos para acionamento dos leds
#define LED_A1 8
#define LED_A2 9
#define LED_A3 10
#define LED_A4 11
#define LED_A5 12

String hora_lida = "--:--";
long int t1;
long int hora_inicial;
long int min_ant;

void setup() {
  Serial.begin(9600);
  pinMode(LED_verde, INPUT);
  pinMode(Sensor, INPUT);
  pinMode(Alarme, OUTPUT);
  
  pinMode(p2, INPUT);
  pinMode(p3, INPUT);
  pinMode(p4, INPUT);
  pinMode(p5, INPUT);
  pinMode(p6, INPUT);
  
  pinMode(f1, INPUT);
  pinMode(f2, INPUT);
  pinMode(f3, INPUT);
  pinMode(f4, INPUT);
  pinMode(f5, INPUT);
  
  pinMode(LED_A1, OUTPUT);
  pinMode(LED_A2, OUTPUT);
  pinMode(LED_A3, OUTPUT);
  pinMode(LED_A4, OUTPUT);
  pinMode(LED_A5, OUTPUT);  
  
  // O programa só vai começar quando o usuário digitar a hora.
  while (hora_lida == "--:--") {
    if (Serial.available() > 0) {
      hora_lida = Serial.readString(); //Formato HH:MM
      t1 = millis(); // Lê os ms na hora que a hora é lida do monitor para manter o relógio atualizado. 
      break;
    };
  };
  
  long int horas = hora_lida.substring(0, 2).toInt() * 60 * 60 * 1000;
  long int minutos = hora_lida.substring(3).toInt() * 60 * 1000;
  hora_inicial = horas + minutos;  
  
  /*
  Serial.print("Hora lida: "); Serial.println(hora_lida);
  Serial.print("Millis capturado (t1): "); Serial.println(t1);
  Serial.print("Horas em ms: "); Serial.println(horas);
  Serial.print("Minutos em ms: "); Serial.println(minutos);
  Serial.print("Hora inicial em ms: "); Serial.println(hora_inicial);
  */
}


void loop() {
  int PW = digitalRead(LED_verde); // Le a entrada do led verde
  int MOTION = digitalRead(Sensor); // Le a entrada do sensor

  int HORAIN = readDipSwitchesStart(); // Detectar horario inicial
  int HORAFN = readDipSwitchesEnd(); // Detectar horario final
  String relogio = updateCurrentTime(hora_inicial, t1);
  int HORA = relogio.substring(0, 2).toInt();
  int MIN = relogio.substring(3).toInt();

  
  
  
  if (PW == HIGH){
    if (MIN != min_ant && HORA < 24) { // Imprime o relógio na tela se o minuto mudar até às 23:59
      Serial.flush();
      Serial.print("Relogio: ");
      Serial.println(relogio);
      Serial.print("Hora Inicial: ");
      Serial.println(HORAIN);
      Serial.print("Hora Final: ");
      Serial.println(HORAFN);
      Serial.println("");
      min_ant = MIN;
    };
    if (HORAIN > 24 || HORAFN > 24){
      PiscaPisca();// LEDs azuis piscarao, avisando que HORAIN/HORAFN n sao possiveis
    } 
    else if (HORAIN <= HORA && HORA < HORAFN){
      int REMAINING = calculateRemainingTime(HORA, HORAFN);
      updateCountdownLEDs(REMAINING);
      if (MOTION == HIGH){
        TocarAlarme(1);// Alarme toca enquanto estiver movimento
      } 
      else {
        noTone(Alarme);
      }
    } else {
        digitalWrite(LED_A1, LOW);
        digitalWrite(LED_A2, LOW);
        digitalWrite(LED_A3, LOW);
        digitalWrite(LED_A4, LOW);
        digitalWrite(LED_A5, LOW); 
    }
  } else {
        digitalWrite(LED_A1, LOW);
        digitalWrite(LED_A2, LOW);
        digitalWrite(LED_A3, LOW);
        digitalWrite(LED_A4, LOW);
        digitalWrite(LED_A5, LOW); 
  }
}


// Toca o alarme se tiver algum movimento 
void TocarAlarme(int entrada) {
  while (entrada > 0) {
    tone(Alarme, 220);
    delay(100);
    entrada--;// Para o toque nao ficar infinito(tirar depois)
  }
}


// Funcao pra setar horario inicial
int readDipSwitchesStart() {
  int Pin2 = digitalRead(p2)== HIGH ? 1 : 0;
  int Pin3 = digitalRead(p3)== HIGH ? 1 : 0;
  int Pin4 = digitalRead(p4)== HIGH ? 1 : 0;
  int Pin5 = digitalRead(p5)== HIGH ? 1 : 0;
  int Pin6 = digitalRead(p6)== HIGH ? 1 : 0;
  // Concatena os valores dos pinos em uma string binária
  String binaryString = String(Pin2) +
                        String(Pin3) +
                        String(Pin4) +
                        String(Pin5) +
                        String(Pin6);
  // Converte a string binária em um número decimal
  int decimalNumberStart = 0;
  for (int i = 0; i < binaryString.length(); i++) {
    if (binaryString[i] == '1') {
      decimalNumberStart += 1 << (binaryString.length() - 1 - i);
    }
  }
  return decimalNumberStart;
}


// Funcao pra setar horario final
int readDipSwitchesEnd() {
  int PinA1 = digitalRead(f1)== HIGH ? 1 : 0;
  int PinA2 = digitalRead(f2)== HIGH ? 1 : 0;
  int PinA3 = digitalRead(f3)== HIGH ? 1 : 0;
  int PinA4 = digitalRead(f4)== HIGH ? 1 : 0;
  int PinA5 = digitalRead(f5)== HIGH ? 1 : 0;
  // Concatena os valores dos pinos em uma string binária
  String binaryString = String(PinA5) +
                        String(PinA4) +
                        String(PinA3) +
                        String(PinA2) +
                        String(PinA1);
  // Converte a string binária em um número decimal
  int decimalNumberEnd = 0;
  for (int i = 0; i < binaryString.length(); i++) {
    if (binaryString[i] == '1') {
      decimalNumberEnd += 1 << (binaryString.length() - 1 - i);
    }
  }
  return decimalNumberEnd;
}


// Funcao para exibir nos leds azuis o tempo restante
void updateCountdownLEDs(int horas){
  
  // Transforma em binario
  String binaryString = "";
  while (horas > 0) {
    binaryString = String(horas % 2) + binaryString;
    horas = horas / 2;
  }
  
  // Adicionar zeros à esquerda se necessário
  while (binaryString.length() < 5) {
    binaryString = "0" + binaryString;
  }

  // Calcular o complemento de 1
  String C1 = "";
  for (int i = 0; i < binaryString.length(); i++) {
    if (binaryString[i] == '0') {
      C1 += '1';
    } else {
      C1 += '0';
    }
  }
  // Acionar os LEDs
  digitalWrite(LED_A1, C1.charAt(0) == '1' ? HIGH : LOW);
  digitalWrite(LED_A2, C1.charAt(1) == '1' ? HIGH : LOW);
  digitalWrite(LED_A3, C1.charAt(2) == '1' ? HIGH : LOW);
  digitalWrite(LED_A4, C1.charAt(3) == '1' ? HIGH : LOW);
  digitalWrite(LED_A5, C1.charAt(4) == '1' ? HIGH : LOW);
}

//Funcao para calcular tempo restante
int calculateRemainingTime(int horas, int horafinal){
  int restante = horafinal - horas;
    return restante;
}


//Funcao para fazer os leds azuis piscar
void PiscaPisca(){
  digitalWrite(LED_A1, HIGH);
  digitalWrite(LED_A2, HIGH);
  digitalWrite(LED_A3, HIGH);
  digitalWrite(LED_A4, HIGH);
  digitalWrite(LED_A5, HIGH);
  delay(500);
  digitalWrite(LED_A1, LOW);
  digitalWrite(LED_A2, LOW);
  digitalWrite(LED_A3, LOW);
  digitalWrite(LED_A4, LOW);
  digitalWrite(LED_A5, LOW);
  delay(500);
}

// Para atualizar o relógio
String updateCurrentTime(long int hora_ini, long int t_ini) {
  unsigned long millis_atual = millis();  // Captura o millis atual. Pode ser multiplicado por algum número para "acelerar o tempo" e facilitar os testes
  long int tempo_decorrido = millis_atual - t_ini;  // Tempo decorrido desde o início
  long int total_ms = hora_ini + tempo_decorrido;  // Tempo total em milissegundos
  
  long int HORA = total_ms / 3600000;  // Total de horas
  long int MIN = (total_ms % 3600000) / 60000;  // Minutos restantes após subtrair as horas
  /*
  Serial.print("Millis atual: "); Serial.println(millis_atual);
  Serial.print("Millis decorridos: "); Serial.println(tempo_decorrido);
  Serial.print("Total millis: "); Serial.println(total_ms);
  Serial.print("Horas: "); Serial.println(HORA);
  Serial.print("Minutos: "); Serial.println(MIN);
  */
  return (HORA < 10 ? "0" : "") + String(HORA) + ":" + (MIN < 10 ? "0" : "") + String(MIN);  // Formatando a saída como "HH:MM"
};