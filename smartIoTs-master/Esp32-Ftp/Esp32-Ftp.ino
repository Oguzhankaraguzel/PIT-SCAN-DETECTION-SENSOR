#include "esp_camera.h"
#include <WiFi.h>
#include "Arduino.h"
// Time
#include "time.h"
#include "lwip/err.h"
#include "lwip/apps/sntp.h"
// MicroSD
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"

// Edit ssid, password, capture_interval:
const char* ssid = "TurkTelekom_T9C3B";
const char* password = "mfJU4Pxk";

long capture_interval = 30000; // microseconds between captures
//

long current_millis;
long last_capture_millis = 0;
static esp_err_t cam_err;
static esp_err_t card_err;
char strftime_buf[64];
int file_number = 0;
bool internet_connected = false;
struct tm timeinfo;
time_t now;


// CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22



//#include <SPI.h>
//#include <ESP8266WiFi.h>
#include <WiFiClient.h>
//#include <ESP8266mDNS.h>
//#include <ESP8266WiFiMulti.h>
#include "FS.h"
#include "SD_MMC.h"

//ESP8266WiFiMulti WiFiMulti;

// change to your server
IPAddress server(52,174,29,83);

WiFiClient client;
WiFiClient dclient;
int status = WL_IDLE_STATUS;     // the Wifi radio's status

char outBuf[128];
char outCount;

char *FTPfilename;
camera_fb_t *fb;

#include "FS.h"
#include "SD.h"
#include "SPI.h"

void setup() {
  Serial.begin(115200);

  if (init_wifi()) { // Connected to WiFi
    internet_connected = true;
    Serial.println("Internet connected");
    init_time();
    time(&now);
    setenv("TZ", "GMT0BST,M3.5.0/01,M10.5.0/02", 1);
    tzset();
  }

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // camera init
  cam_err = esp_camera_init(&config);
  if (cam_err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", cam_err);
    return;
  }
  // SD camera init
  /*card_err = init_sdcard();
  if (card_err != ESP_OK) {
    Serial.printf("SD Card init failed with error 0x%x", card_err);
    return;
  }*/
}

bool init_wifi()
{
 int connAttempts = 0;
  Serial.println("\r\nConnecting to: " + String(ssid));
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
    if (connAttempts > 10) return false;
    connAttempts++;
  }
  return true;

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(2000);
  return true;

}

void init_time()
{
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "pool.ntp.org");
  sntp_init();
  // wait for time to be set
  time_t now = 0;
  timeinfo = { 0 };
  int retry = 0;
  const int retry_count = 10;
  while (timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
    Serial.printf("Waiting for system time to be set... (%d/%d)\n", retry, retry_count);
    delay(2000);
    time(&now);
    localtime_r(&now, &timeinfo);
  }
  Serial.println("Waiting for interval..");
}
/*
static esp_err_t init_sdcard()
{
  esp_err_t ret = ESP_FAIL;
  sdmmc_host_t host = SDMMC_HOST_DEFAULT();
  sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
    .format_if_mount_failed = false,
    .max_files = 1,
  };
  sdmmc_card_t *card;

  Serial.println("Mounting SD card...");
  ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);

  if (ret == ESP_OK) {
    Serial.println("SD card mount successfully!");
  }  else  {
    Serial.printf("Failed to mount SD card VFAT filesystem. Error: %s", esp_err_to_name(ret));
  }
}
*/
static esp_err_t save_photo_dated()
{
  Serial.println("Taking picture...");
  fb = esp_camera_fb_get();

   time(&now);
  localtime_r(&now, &timeinfo);
  strftime(strftime_buf, sizeof(strftime_buf), "%F_%H_%M_%S", &timeinfo);

  FTPfilename = (char*)malloc(21 + sizeof(strftime_buf));
  sprintf(FTPfilename, "capture_%s.jpeg", strftime_buf);

  Serial.println(FTPfilename);
  
  Serial.println("Connecting FTP...");
  doFTP();
  esp_camera_fb_return(fb);
}

void save_photo()
{
    save_photo_dated(); // filenames with date and time
}

void loop()
{
  current_millis = millis();
  if (current_millis - last_capture_millis > capture_interval) { // Take another picture
    last_capture_millis = millis();
    save_photo();
  }
}

byte doFTP()
{
  if (client.connect(server, 21)) {
    Serial.println(F("Command connected"));
  }
  else {
    Serial.println(F("Command connection failed"));
    return 0;
  }
  // send login name and password
  if (!eRcv()) return 0;
  client.println(F("USER ceyhuncoskun"));
  //if(!eRcv()) return 0;
  client.println(F("PASS ceyhunZENO19"));
  if (!eRcv()) return 0;
  client.println(F("SYST"));
  if (!eRcv()) return 0;
  client.println(F("Type I"));
  //if(!eRcv()) return 0;
  client.println(F("CWD CAM-2"));
  //if(!eRcv()) return 0;
  client.println(F("PASV"));
  if (!eRcv()) return 0;
  char *tStr = strtok(outBuf, "(,");
  int array_pasv[6];
  for ( int i = 0; i < 6; i++) {
    tStr = strtok(NULL, "(,");
    array_pasv[i] = atoi(tStr);
    if (tStr == NULL)
      Serial.println(F("Bad PASV Answer"));
  }

  unsigned int hiPort, loPort;
  hiPort = array_pasv[4] << 8;
  loPort = array_pasv[5] & 255;
  Serial.print(F("Data port: "));
  hiPort = hiPort | loPort;
  Serial.println(hiPort);
  if (dclient.connect(server, hiPort)) {
    Serial.println(F("Data connected"));
  }
  else {
    Serial.println(F("Data connection failed"));
    client.stop();
    return 0;
  }

  delay(1000);
  //if(!eRcv()) return 0;
  // setup to send file file fileName
  client.print(F("STOR "));
  client.println(FTPfilename);
  delay(1000);
  if (!eRcv()) {
    Serial.println("dataClient stopped");
    dclient.stop();
    return 0;
  }
  // send file contents
  dclient.write(fb->buf,fb->len);
  //dclient.write(HTTPfile);
  //dclient.write(clientBuf,clientCount);
  dclient.stop();
  Serial.println(F("Data disconnected"));
  client.println(F("MKD true"));
  if(!eRcv()) return 0;
  Serial.println("File Uploaded");
  client.println(F("QUIT"));
  //if(!eRcv()) return 0;
  //if(!eRcv()) return 0;
  client.stop();
  Serial.println(F("Command disconnected"));
  Serial.println("");
  Serial.println("Waiting for interval..");
  return 1;
}

byte eRcv()
{
  byte respCode;
  byte thisByte;
  //while(!client.available()) delay(1);
  delay(1000);
  respCode = client.peek();
  outCount = 0;
  while (client.available())
  {
    thisByte = client.read();
    Serial.write(thisByte);
    if (outCount < 127)
    {
      outBuf[outCount] = thisByte;
      outCount++;
      outBuf[outCount] = 0;
    }
  }
  if (respCode >= '4')
  {
    efail();
    return 0;
  }
  return 1;
}


void efail()
{
  byte thisByte = 0;
  client.println(F("QUIT"));
  while (!client.available()) delay(1);
  while (client.available())
  {
    thisByte = client.read();
    Serial.write(thisByte);
  }
  client.stop();
  Serial.println(F("Command disconnected"));
  Serial.println(F("SD closed"));
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
