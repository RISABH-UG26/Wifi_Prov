#include "WiFiProv.h"
#include "WiFi.h"
#include "nvs_flash.h"  // Include NVS Flash library

const char *pop = "abcd1234";  // Device PIN for provisioning
const char *service_name = "PROV_123";  // Device identifier for the ESP app
const char *service_key = NULL;  // No password for SoftAP mode
bool reset_prov = true;  // Reset previous credentials if any

void SysProvEvent(arduino_event_t *event) {
  switch (event->event_id) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.printf("\nConnected! IP: %s\n", IPAddress(event->event_info.got_ip.ip_info.ip.addr).toString().c_str());
      break;

    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("\nDisconnected. Reconnecting...");
      break;

    case ARDUINO_EVENT_PROV_START:
      Serial.println("\nProvisioning started. Use the app to connect.");
      break;

    case ARDUINO_EVENT_PROV_CRED_RECV:
      Serial.printf("\nWi-Fi Credentials:\nSSID: %s\nPassword: %s\n", 
                    event->event_info.prov_cred_recv.ssid,
                    event->event_info.prov_cred_recv.password);

      // Extract and print BSSID
      Serial.printf("BSSID: %02X:%02X:%02X:%02X:%02X:%02X\n",
                    event->event_info.prov_cred_recv.bssid[0],
                    event->event_info.prov_cred_recv.bssid[1],
                    event->event_info.prov_cred_recv.bssid[2],
                    event->event_info.prov_cred_recv.bssid[3],
                    event->event_info.prov_cred_recv.bssid[4],
                    event->event_info.prov_cred_recv.bssid[5]);
      break;

    case ARDUINO_EVENT_PROV_CRED_FAIL:
      Serial.println("\nProvisioning Failed. Reset and retry.");
      Serial.println(event->event_info.prov_fail_reason == NETWORK_PROV_WIFI_STA_AUTH_ERROR ? 
                     "Incorrect Wi-Fi password." : 
                     "Wi-Fi network not found. Try erasing NVS.");
      break;

    case ARDUINO_EVENT_PROV_CRED_SUCCESS:
      Serial.println("\nProvisioning Successful!");
      break;

    case ARDUINO_EVENT_PROV_END:
      Serial.println("\nProvisioning Completed.");
      break;

    default:
      break;
  }
}

void setup() {
  Serial.begin(115200);
  
  // Reset NVS and Wi-Fi credentials
  nvs_flash_init();
  nvs_flash_erase();
  WiFi.disconnect(true, true);
  delay(100);

  WiFi.mode(WIFI_MODE_STA);
  WiFi.onEvent(SysProvEvent);
  
  Serial.println("Starting BLE Provisioning...");
  
  uint8_t uuid[16] = {0xb4, 0xdf, 0x5a, 0x1c, 0x3f, 0x6b, 0xf4, 0xbf,
                      0xea, 0x4a, 0x82, 0x03, 0x04, 0x90, 0x1a, 0x02};
  
  WiFiProv.beginProvision(NETWORK_PROV_SCHEME_BLE, NETWORK_PROV_SCHEME_HANDLER_FREE_BLE, 
                          NETWORK_PROV_SECURITY_1, pop, service_name, 
                          service_key, uuid, reset_prov);
}


void loop() {
  delay(1000);  // Small delay to avoid unnecessary CPU usage
}
