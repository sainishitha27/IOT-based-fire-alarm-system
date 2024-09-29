#include <ESP8266WiFi.h>
#include <ESP_Mail_Client.h>
#include <DHT.h>

#define WIFI_SSID "NFC1234"
#define WIFI_PASSWORD "B21EC009"
#define SMTP_server "smtp.gmail.com"
#define SMTP_Port 465
#define sender_email "fproject383@gmail.com"
#define sender_password "jopslclsfqpfpcfm"
#define Recipient_email "b21ec009@kitsw.ac.in"
#define Recipient_name ""
#define DHT_PIN D2 
#define D1 5
#define D2 4
#define D3 0

// Define D1 explicitly as pin 5
#define D1 5

// Digital pin connected to the relay
#define Relay_pin D3
#define WaterPump_pin Relay_pin


int Flame_sensor = D1;
int Flame_detected;

DHT dht(DHT_PIN, DHT11);

SMTPSession smtp;

void setup() {
  Serial.begin(115200);
  Serial.println();
    delay(100);

  pinMode(Relay_pin, OUTPUT);
  digitalWrite(Relay_pin, LOW); // Ensure relay is initially off
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  smtp.debug(1);
  dht.begin();
}

void loop() {
  // Example variable
  String str = "Fire Detection";
  // Example HTML content with the variable included
  String htmlContent = "<div style=\"color:#000000;\"><h1>" + str + "</h1><p>Detection of Fire </p></div>";

  Flame_detected = digitalRead(Flame_sensor);
  Serial.println(Flame_detected);
  delay(100);
  if (Flame_detected == 1) {
    Serial.println("Flame detected...! take action immediately.");
    digitalWrite(WaterPump_pin, LOW);
    // Read temperature
    float temperature = dht.readTemperature();
    // Modify HTML content to include temperature
    htmlContent += "<p>Temperature: " + String(temperature) + " °C</p>";
    // Call the function to send the email with the updated HTML content
    sendEmail(htmlContent);
  }
  else
  {
    Serial.println("No Fire detected. stay cool");
    digitalWrite(WaterPump_pin, HIGH);
  }

  // Add a delay or other logic as needed
  delay(10000);
  
  
  // Delay for 1 minute (adjust as needed)
}

void sendEmail(String htmlMsg) {
  Serial.println("Sending email...");

  // Create session
  ESP_Mail_Session session;
  session.server.host_name = SMTP_server;
  session.server.port = SMTP_Port;
  session.login.email = sender_email;
  session.login.password = sender_password;
  session.login.user_domain = "";

  // Create message
  SMTP_Message message;
  message.sender.name = "ESP8266";
  message.sender.email = sender_email;
  message.subject = "Detection of Flame";
  message.addRecipient(Recipient_name, Recipient_email);

  // Send HTML message
  message.html.content = htmlMsg.c_str();
  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  // Connect and send email
  if (!smtp.connect(&session)) {
    Serial.println("Error connecting to SMTP server");
    return;
  }

  if (!MailClient.sendMail(&smtp, &message)) {
    Serial.println("Error sending Email, " + smtp.errorReason());
  } else {
    Serial.println("Email sent successfully!");
  }
}
