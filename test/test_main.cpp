#include <Arduino.h>
#include <unity.h>
#include "testCAN.h"
#include "testClass.h"
#include "cbuffer.h"
#include "cnst.h"
//#include "gtest/gtest.h"

cbuffer_t buff;
#define TESTFNAME "/test_example.txt"
#define JSONFNAME "/unit_tests.json"

String STR_TO_TEST;

void setUp(void) {
    // set stuff up here
    STR_TO_TEST = "set stuff up";
      cbuffer_init(&buff);


}

void tearDown(void) {
    // clean stuff up here
    STR_TO_TEST = "clean stuff ";
     cbuffer_clear(&buff);
}



void test_CAN_init(void){

TEST_ASSERT_TRUE(CAN_init());

}
void test_CAN_HeartBeat(void){

TEST_ASSERT_TRUE(CAN_period(10000));

}
uint8_t test_CAN_resive(void){


}
bool test_CAN_send(void){

TEST_ASSERT_TRUE(CAN_send(15000));
}

bool test_CAN_AKL(void){



}




void test_string_concat(void) {
    String hello = "Hello, ";
    String world = "world!";
    TEST_ASSERT_EQUAL_STRING(STR_TO_TEST.c_str(), (hello + world).c_str());
}

void test_string_substring(void) {
    TEST_ASSERT_EQUAL_STRING("Hello", STR_TO_TEST.substring(0, 5).c_str());
}

void test_string_index_of(void) {
    TEST_ASSERT_EQUAL(7, STR_TO_TEST.indexOf('w'));
}

void test_string_equal_ignore_case(void) {
    TEST_ASSERT_TRUE(STR_TO_TEST.equalsIgnoreCase("HELLO, WORLD!"));
}

void test_string_to_upper_case(void) {
    STR_TO_TEST.toUpperCase();
    TEST_ASSERT_EQUAL_STRING("HELLO, WORLD!", STR_TO_TEST.c_str());
}

void test_string_replace(void) {
    STR_TO_TEST.replace('!', '?');
    TEST_ASSERT_EQUAL_STRING("Hello, world?", STR_TO_TEST.c_str());
}


void test_circular_buffer_empty_after_init() {
    cbuffer_t buffz;

    cbuffer_init(&buffz);

    TEST_ASSERT_TRUE(cbuffer_empty(&buffz));
}

void test_circular_buffer_not_empty_after_new_element_added() {
    cbuffer_t buff;
    cbuffer_init(&buff);

    cbuffer_add(&buff, 100);

    TEST_ASSERT_FALSE(cbuffer_empty(&buff));
}


void test_circular_buffer_reports_full_correctly() {
    cbuffer_t buff;
    cbuffer_init(&buff);
    for (uint8_t i = 0; i < BUFFER_SIZE; i++) {
        cbuffer_add(&buff, i);
    }

    TEST_ASSERT_TRUE(cbuffer_full(&buff));
}
void test_circular_buffer_read_element_succesful() {
    cbuffer_t buff;
    cbuffer_init(&buff);

    uint8_t value = 55;
    cbuffer_add(&buff, value);

    TEST_ASSERT_EQUAL(value, cbuffer_get(&buff));
}

void test_circular_buffer_cleaned_succesfully() {
    cbuffer_t buff;
    cbuffer_init(&buff);

    for (uint8_t i = 0; i < BUFFER_SIZE/2; i++) {
        cbuffer_add(&buff, i);
    }
    cbuffer_clear(&buff);

    TEST_ASSERT_TRUE(cbuffer_empty(&buff));
}


/*
void gpio_master_test()
{
    gpio_config_t slave_config = {
            .pin_bit_mask = 1 << MASTER_GPIO_PIN,
            .mode = GPIO_MODE_INPUT,
    };
    gpio_config(&slave_config);
    unity_wait_for_signal("output high level");
    TEST_ASSERT(gpio_get_level(MASTER_GPIO_PIN) == 1);
}

void gpio_slave_test()
{
    gpio_config_t master_config = {
            .pin_bit_mask = 1 << SLAVE_GPIO_PIN,
            .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&master_config);
    gpio_set_level(SLAVE_GPIO_PIN, 1);
    unity_send_signal("output high level");
}

TEST_CASE_MULTIPLE_DEVICES("gpio multiple devices test example", "[driver]", gpio_master_test, gpio_slave_test);


*/


void test_write(void)
{
    File file = FileFS.open("/test_example.txt", "w");
    uint8_t pbuf[] = "helloworld\0";
    bool exists;
    
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    file.write(pbuf, sizeof(pbuf));
    file.close();
    TEST_ASSERT_EQUAL(1, 1);

    exists = FileFS.exists(TESTFNAME);
    TEST_ASSERT_TRUE(exists==true);
    FileFS.remove(TESTFNAME);
}

void test_write_read(void)
{
    File file;
    uint8_t pwrite_buf[] = "helloworld\0";
    char pread_buf[255];
    int numbytes;
    
    file = FileFS.open("/test_example.txt", "w");
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    file.write(pwrite_buf, sizeof(pwrite_buf));
    file.close();

    file = FileFS.open(TESTFNAME,"r");
    numbytes = file.readBytesUntil('\n', pread_buf, sizeof(pread_buf));
    pread_buf[numbytes] = '\0';

    // Serial.println("write (" + String(pwrite_buf) + ")");
    // Serial.println("read (" + String(pread_buf) + ")");
    // Serial.println("len pwrite_buf= " + String(strlen(pwrite_buf)));
    // Serial.println("len pread_buf= " + String(strlen(pread_buf)));
    Serial.printf("\n");
    Serial.printf("wrote (%s)\n", pwrite_buf);
    Serial.printf("read (%s)\n", pread_buf);
    //Serial.printf("len pwrite_buf= %d\n", strlen(pwrite_buf));
    Serial.printf("len pread_buf=%d\n",strlen(pread_buf));

    //TEST_ASSERT_TRUE(String(pwrite_buf)==String(pread_buf));
    FileFS.remove(TESTFNAME);
}

/*
to upload door/data/test_FileFS_uploaded.txt,
first run:
dpio run -t uploadfs
then test_FileFS_uploaded.txt should appear on the esp8266 under /
Notes: 
you DO NOT need to set [platformio] data_dir. In fact, this setting seems to be ignored.
*/
void test_read_uploaded_file(void)
{
    File file;
    boolean found_flag = false;
    
    bool exists = FileFS.exists(JSONFNAME);
    TEST_ASSERT_TRUE(exists==true);

    file = FileFS.open(JSONFNAME,"r");
    Serial.printf("------------------\n");
    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        Serial.println(line);
        if (line.startsWith(String("\"wifi")))
        {
            found_flag |= true;
        }
    }
    TEST_ASSERT_TRUE(found_flag);
                                 
    Serial.printf("------------------\n");
    file.close();
}

void test_list_files(void)
{
    String str = "";

   // Dir dir = FileFS.openDir("/");
    File root = FileFS.open("/");
    File file = root.openNextFile();
    while (file) {
        Serial.printf("%s | %d\n",file.name(), file.size());
        // str += dir.fileName();
        // str += " / ";
        // str += dir.fileSize();
        // str += "\r\n";
        file = root.openNextFile();
    }
    Serial.print(str);
    TEST_ASSERT_TRUE(true);
}

void test_load_txt_into_buffer(void)
{
    File file;
    String txt;
    
    bool exists = FileFS.exists(JSONFNAME);
    TEST_ASSERT_TRUE(exists==true);

    file = FileFS.open(JSONFNAME,"r");
    Serial.printf("------------------\n");
    while (file.available()) {
        String line = file.readStringUntil('\n');
        txt = txt + line;
        Serial.println(line);
    }
    Serial.printf("------------------\n");
    Serial.println(txt);
    file.close();
}
#define TESTS_NUM 10
#define TESTS_DELAY 100

#define WIFI_SSID "NN"
#define WIFI_PASSWORD "clave"
#ifdef DEBUG_ESP_PORT
#define INFO_ESP_PORT DEBUG_ESP_PORT
#else
#define INFO_ESP_PORT Serial
#endif
#define INFO_TEST(...)                  \
	INFO_ESP_PORT.print("INFO TEST: "); \
	INFO_ESP_PORT.printf(__VA_ARGS__)
#include <WiFi.h>
WiFiClient wifi;


void test_connect_wifi()
{
	bool ret = true;
	char counter = 0;

	INFO_TEST("Connecting to WIFI...\n");
	WiFi.disconnect();
    WiFi.mode(WIFI_STA);
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		INFO_TEST("Trying reconnect to WIFI\n");
		if (counter == 5)
		{
			ret = false;
			break;
		}
		counter++;
	}
	TEST_ASSERT_TRUE(ret);
}









void setup()
{
    delay(2000); // service delay
    UNITY_BEGIN();
    //CAN test
    RUN_TEST(test_CAN_init);
    RUN_TEST(test_CAN_HeartBeat);
    RUN_TEST(test_CAN_send);
    // flash W/R test 
    RUN_TEST(test_string_concat);
    RUN_TEST(test_string_substring);
    RUN_TEST(test_string_index_of);
    RUN_TEST(test_string_equal_ignore_case);
    RUN_TEST(test_string_to_upper_case);
    RUN_TEST(test_string_replace);

    RUN_TEST(test_circular_buffer_empty_after_init);
    RUN_TEST(test_circular_buffer_not_empty_after_new_element_added);
    RUN_TEST(test_circular_buffer_reports_full_correctly);
    RUN_TEST(test_circular_buffer_read_element_succesful);
    RUN_TEST(test_circular_buffer_cleaned_succesfully);

    UNITY_END(); // stop unit testing
}

void loop()
{
}