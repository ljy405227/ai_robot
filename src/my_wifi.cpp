#include "my_wifi.hpp"


const char* ssid = "ljy";
const char* password = "147258369";
const char* apiKey = "sk-ndacpewnoqvheznlgzxnzcwqhjzggxcbkvjvcyqywzgkjrne";

bool Wifi_Init(void) {
    uint8_t wifi_get_time = 0;
    printf("正在连接到 WiFi 网络: %s,尝试密码：%s\n", ssid, password);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        vTaskDelay(pdMS_TO_TICKS(500));
        Serial.print(".");
        wifi_get_time++;
        if (wifi_get_time >= 6)
        {
            printf("WiFi连接失败");
            return false;
        }
        
    }
    Serial.println("\n✅ WiFi连接成功！");
    Serial.print("🔌 本机IP地址：");
    Serial.println(WiFi.localIP());  // 打印ESP32的局域网IP
    Serial.print("📶 WiFi信号强度：");
    Serial.print(WiFi.RSSI());       // 打印信号强度（单位dBm，数值越大信号越好）
    Serial.println(" dBm");
    return true;
}

void Wifi_Off(void) {
  if (WiFi.status() == WL_CONNECTED) {  // 先判断是否已连接，避免重复关闭
    WiFi.disconnect(true);  // true：彻底断开连接并清除保存的WiFi信息
    WiFi.mode(WIFI_OFF);    // 关闭WiFi模块，进入休眠状态（核心！真正关闭）
    Serial.println("✅ WiFi已彻底关闭，释放资源");
  } else {
    Serial.println("⚠️ WiFi未连接，无需关闭");
  }
}

void Wifi_On(void) {
    Wifi_Reconnect();
}

bool Wifi_Reconnect(void) {
  Serial.println("\n🔄 WiFi已断连，开始清理旧连接...");
  WiFi.disconnect(true);  // true=彻底断开并清除保存的WiFi信息
  WiFi.mode(WIFI_STA);    // 重置为STA客户端模式（避免模式混乱）
  delay(300);             // 短暂延时，保证模块重置完成

  Serial.println("🔄 开始WiFi重连...");
  if (Wifi_Init()) {
    Serial.println("✅ WiFi重连成功！");
    return true;
  } else {
    Serial.println("❌ WiFi重连失败，将在5秒后再次尝试");
    return false;
  }
}

String callAI(String userText) {
  HTTPClient http;
  http.begin("https://api.siliconflow.cn/v1/chat/completions");

  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", String("Bearer ") + apiKey);

  StaticJsonDocument<1024> doc;
  doc["model"] = "deepseek-ai/DeepSeek-OCR";
//   doc["model"] = "tencent/Hunyuan-MT-7B";
  // doc["model"] = "THUDM/GLM-Z1-9B-0414";

  JsonArray messages = doc.createNestedArray("messages");

  JsonObject sys = messages.createNestedObject();
  sys["role"] = "system";
  sys["content"] = "你是一个友好的中文语音助手";

  JsonObject user = messages.createNestedObject();
  user["role"] = "user";
  user["content"] = userText;

  String body;
  serializeJson(doc, body);

  int code = http.POST(body);
  if (code <= 0) {
    http.end();
    return "HTTP 请求失败";
  }

  String payload = http.getString();
  http.end();

  StaticJsonDocument<2048> resp;
  deserializeJson(resp, payload);

  return resp["choices"][0]["message"]["content"].as<String>();
}

