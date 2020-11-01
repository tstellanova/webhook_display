
#include <JsonParserGeneratorRK.h>
#include <Adafruit_SSD1306_RK.h>
#include <Adafruit_GFX_RK.h>

// #include <SPI.h>
#include <Wire.h>

// -----------------------------------
// Controlling Displays over the Internet
// -----------------------------------
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)



static unsigned int _loop_count = 0;
static unsigned int _response_count = 0;
static JsonParserStatic<2048, 100> _json_parser;
// SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
SystemSleepConfiguration _sleep_config;

void setup()
{
    Particle.syncTime();
    Particle.function("render",renderCommand);
    Particle.function("startTZ", setTimeZone);
    Particle.subscribe("hook-response/purplest", purpleAirHandler, System.deviceID());
    
    display_setup();

    Time.zone(-7.0);
}


// TODO make this dynamic based on provided string
int setTimeZone(String args) {
    Time.zone(-8.0);
    Time.setDSTOffset(1.0);
    Time.beginDST();
    return 0;
}

// callback from purpleair webhook
void purpleAirHandler(const char *event, const char *data) {
	int responseIndex = 0;

    _response_count += 1;

	const char *slashOffset = strrchr(event, '/');
	if (slashOffset) {
		responseIndex = atoi(slashOffset + 1);
	}

	if (responseIndex == 0) {
		_json_parser.clear();
	}
	_json_parser.addString(data);
	
    if (_json_parser.parse()) {
        JsonReference sample = _json_parser.getReference().key("feeds").index(0);
        float pm_2_5 = sample.key("field2").valueFloat(); 
        float fahrenheit = sample.key("field6").valueFloat(); 
        // String raw_label = sample.key("created_at").valueString();
        // String label = raw_label.substring(raw_label.length() - 9);
        String temperature = String(fahrenheit,1);
        temperature.concat("F");
        time_t time = Time.now(); 
        String label = Time.format(time, "%r");
        label.concat(" | " + temperature);
        render_purple(label , pm_2_5);
    } 
    else {
        render_purple("error", 0.00);
    }
    

}

// display the air quality
void render_purple(String label, float pm_2_5) {
    display.clearDisplay();

    // time display
    display.setTextColor(WHITE);
    display.setCursor(4, 0);             
    display.setTextSize(1);            
    display.println(label);
    
    // 1,000 ppm is 1 ug/m^3
    // breakpoints for USA EPA AQI:
    // Good : < 12.1 ug/m^3
    // Moderate: < 35.5
    // Sensitive: < 55.5
    // Unhealthy: < 150.5
    // Very Unhealthy: < 250.5
    // Hazardous: >= 250.5 
    String rating = " ok";
    if (pm_2_5 > 12.0) {
        if (pm_2_5 > 35.4) {  rating = " PUFF"; }
        else { rating = " ICK"; }
    }
    
    // PM2.5 display
    display.setCursor(4, 18);
    display.setTextSize(4);
    String h1 = String(pm_2_5, 1);
    display.println(h1);
    display.setCursor(64, 48);
    display.setTextSize(2);
    display.println(rating);
    display.display();
    
    //ensure we write to the display
    delay(2000);
}

// remotely display a string
int renderCommand(String command) {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setCursor(4, 4);             
    display.setTextSize(1);            
    display.println(command);
    display.display();
    
    //ensure we write to the display
    delay(5000);
    return 0;
}

// configure the display driver
void display_setup() {
  Serial.begin(115200);
  Serial.println(F("-- begin--"));    
  delay(250);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32 // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    //for(;;); // Don't proceed
  }

  // Clear the buffer
  display.clearDisplay();
  display.display();
  delay(2000);

}



void loop() {
    static unsigned int local_resp_count = 0;
    String data = String(_loop_count);
    
    _loop_count++;
    local_resp_count = _response_count;

    Particle.publish("purplest", data, PRIVATE);
    
    //allow time for OTA response or firmware updates
    delay(60000);


    // sleep for a while before requesting another update
    _sleep_config.mode(SystemSleepMode::STOP).duration(60s);
    SystemSleepResult result = System.sleep(_sleep_config);

}






