#define BLYNK_TEMPLATE_ID "TMPL6thFimnE5"
#define BLYNK_TEMPLATE_NAME "hk"
#define BLYNK_AUTH_TOKEN "8mkB30fMqg1vdaoX97w-r4RxNO0JZ-Wy"

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define RX 16
#define TX 17
#define LED 2

float Temp = 0.0, Humi = 0.0;
WidgetLED LED_ON_APP(V3);
int button;
String buff_rx;
SoftwareSerial espserial(RX, TX);
bool dataReceived = false;

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "duyhiepil";
char pass[] = "20012003";

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(9600);          // Khởi tạo Serial để theo dõi
  espserial.begin(38400);      // Khởi tạo giao tiếp UART
  espserial.listen();
  
  Serial.println("ESP kết nối đến STM32");
  
  Blynk.begin(auth, ssid, pass);  // Kết nối Blynk
}

BLYNK_WRITE(V2) {
  button = param.asInt();
  if (button == 1) {
    digitalWrite(LED, HIGH);
    LED_ON_APP.on();
  } else {
    digitalWrite(LED, LOW);
    LED_ON_APP.off();
  }
}

void loop() {
    Blynk.run();  // Giữ cho Blynk luôn hoạt động

  // Xử lý dữ liệu UART từ STM32
  if (espserial.available()) {
    buff_rx = espserial.readString();  // Đọc dữ liệu từ UART
    Serial.print("Dữ liệu nhận được từ STM32: ");  
    Serial.println(buff_rx);  // In ra dữ liệu đã nhận để kiểm tra

    // Loại bỏ các ký tự không mong muốn
    buff_rx.trim();  // Xóa các khoảng trắng và ký tự không hợp lệ ở đầu/cuối chuỗi
    buff_rx.replace("�", "");  // Xóa các ký tự không hợp lệ như `�`

    dataReceived = true;  // Đánh dấu là đã nhận được dữ liệu
  }

  // Xử lý dữ liệu nhiệt độ và độ ẩm khi có dữ liệu mới
  if (dataReceived) {
    int tempIndex = buff_rx.indexOf("Temp:");
    int humiIndex = buff_rx.indexOf("Hum:");

    if (tempIndex != -1 && humiIndex != -1) {
      // Tách giá trị nhiệt độ và độ ẩm
      Temp = buff_rx.substring(tempIndex + 5, buff_rx.indexOf('C', tempIndex)).toFloat();
      Humi = buff_rx.substring(humiIndex + 4, buff_rx.indexOf('%', humiIndex)).toFloat();

      // Gửi dữ liệu lên Blynk
      Blynk.virtualWrite(V0, Temp);
      Blynk.virtualWrite(V1, Humi);

      // Hiển thị ra Serial để kiểm tra
     Serial.print("Nhiệt độ: ");
      Serial.print(Temp);
      Serial.print("°C");  // Thêm đơn vị °C trên cùng một dòng
      Serial.println();  // Xuống dòng sau khi in xong nhiệt độ

      Serial.print("Độ ẩm: ");
      Serial.print(Humi);
      Serial.print("%");  // Thêm đơn vị % trên cùng một dòng
      Serial.println();  // Xuống dòng sau khi in xong độ ẩm
      // Reset dữ liệu
      buff_rx = "";
      dataReceived = false;  // Đặt lại cờ sau khi xử lý dữ liệu
    } else {
      Serial.println("Không tìm thấy dữ liệu nhiệt độ hoặc độ ẩm trong chuỗi.");
    }
  }
}
