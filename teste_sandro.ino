/**************************************************
|Sandro Miguel Weizenmann                         |
|Teste wifi                                       |
|Acendendo o Led do ESP32 por WiFi via página WEB |
**************************************************/

// Carrega biblioteca Wi-Fi
#include <WiFi.h>

// Substitua pelas suas credenciais de rede
const char* ssid     = "";     //Nome da rede
const char* password = "";     //Senha da rede

//Defina o número da porta do servidor da web como 80
WiFiServer server(80);

//Variável para armazenar a solicitação HTTP
String header;

//Variáveis ​​auxiliares para armazenar o estado de saída atual
String output22State = "off";

//Atribuir variáveis ​​de saída aos pinos GPIO
const int output22 = 2;            //saída do LED interno da placa ESP32ROOM

void setup() {
  Serial.begin(115200);            //taxa de transmissão
  //Inicialize as variáveis ​​de saída como saídas
  pinMode(output22, OUTPUT);
  //Defina as saídas para LOW
  digitalWrite(output22, LOW);

  //Conecte-se à rede Wi-Fi com SSID e senha
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Printando o endereço IP local e iniciando o servidor web
  Serial.println("");
  Serial.println("Conectado ao WiFi.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Ouça os clientes recebidos

  if (client) {                             // Se um novo cliente se conectar,
    Serial.println("New Client.");          // imprimir uma mensagem na porta serial
    String currentLine = "";                // faça uma String para armazenar dados recebidos do cliente
    while (client.connected()) {            // enquanto o cliente está conectado
      if (client.available()) {             // se houver bytes para ler do cliente,
        char c = client.read();             // ler um byte, então
        Serial.write(c);                    // printa a saída no monitor serial.
        header += c;
        if (c == '\n') {                    // se o byte for um caractere de nova linha
          // se a linha atual estiver em branco, você terá dois caracteres de nova linha seguidos.
          // como é o fim da solicitação HTTP do cliente, envie uma resposta:
          if (currentLine.length() == 0) {
            // Cabeçalhos HTTP sempre começam com um código de resposta (e.g. HTTP/1.1 200 OK)
            // e um tipo de conteúdo para que o cliente saiba o que está por vir e, em seguida, uma linha em branco:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // liga e desliga os GPIOs
            if (header.indexOf("GET /22/on") >= 0) {
              Serial.println("GPIO 22 on");
              output22State = "on";
              digitalWrite(output22, HIGH);
            } else if (header.indexOf("GET /22/off") >= 0) {
              Serial.println("GPIO 22 off");
              output22State = "off";
              digitalWrite(output22, LOW);
            } 
            
            // Exibir a página da web em HTML
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS para estilizar os botões liga / desliga 
            // Sinta-se à vontade para alterar os atributos de cor de fundo e tamanho da fonte para atender às suas preferências
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Cabeçalho da página da Web
            client.println("<body><h1>ESP32 - Acendendo Lampada Remotamente</h1>");
            client.println("<h2>Sandro Miguel Weizenmann</h2>");
            
            // Exibir estado atual e botões ON / OFF para GPIO 22 
            client.println("<p>GPIO 22 - State " + output22State + "</p>");
            // Se o output22State estiver desativado, ele exibirá o botão ON      
            if (output22State=="off") {
              client.println("<p><a href=\"/22/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/22/off\"><button class=\"button button2\">OFF</button></a></p>");
            }   
          
            client.println("</body></html>");
            
            // A resposta HTTP termina com outra linha em branco
            client.println();
            // Break out of the while loop
            break;
          } else { // se você recebeu uma nova linha, limpe a linha atual
            currentLine = "";
          }
        } else if (c != '\r') {  // se você tiver mais alguma coisa além de um caractere de retorno de transporte,
          currentLine += c;      // adicione-o ao final do currentLine
        }
      }
    }
    // Limpe a variável de cabeçalho
    header = "";
    // Fecha a conexão
    client.stop();
    Serial.println("Cliente desconectado.");
    Serial.println("");
  }
}
