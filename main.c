#include "ssd1306.h"


uint8_t buf[SSD1306_BUF_LEN];


void render_core1(){

	// Init I2C display
	i2c_init(I2C_INTERFACE, SSD1306_I2C_CLK * 1000);
	gpio_set_function(PICO_I2C_SDA_PIN, GPIO_FUNC_I2C);
	gpio_set_function(PICO_I2C_SCL_PIN, GPIO_FUNC_I2C);
	gpio_pull_up(PICO_I2C_SDA_PIN);
	gpio_pull_up(PICO_I2C_SCL_PIN);

	// Initialize SSD1306 OLED
	SSD1306_init();

	struct render_area frame_area = {
		start_col: 0,
		end_col : SSD1306_WIDTH - 1,
		start_page : 0,
		end_page : SSD1306_NUM_PAGES - 1
	};

	calc_render_area_buflen(&frame_area);

	// Zero the entire display
	memset(buf, 0, SSD1306_BUF_LEN);
	render(buf, &frame_area);

	// Flash screen
	SSD1306_send_cmd(SSD1306_SET_ALL_ON);
	sleep_ms(500);
	SSD1306_send_cmd(SSD1306_SET_ENTIRE_ON);
	
	// Rendering cycle
	while(1){
		// Update the OLED display
		render(buf, &frame_area);
		// Delay
		sleep_ms(25);
	}
}


int main() {
	stdio_init_all();

	// Init ADC joystick
	adc_init();
	adc_gpio_init(PICO_ADC_VRX_PIN);
	adc_gpio_init(PICO_ADC_VRY_PIN);
	
	gpio_init(JOY_SW_PIN);
	gpio_set_dir(JOY_SW_PIN, GPIO_IN);
	gpio_pull_up(JOY_SW_PIN);
	
	// Launch i2c init and rendering on core1
	multicore_launch_core1(render_core1);

	// Spawn position of the pixel
	uint8_t x = 64;
	uint8_t y = 32;

	uint8_t x_prev = x;
	uint8_t y_prev = y;

	bool drawline = true;


	// Movement
	while (1) {


		adc_select_input(ADC_X_PORT);
		uint16_t x_value = adc_read();
		adc_select_input(ADC_Y_PORT);
		uint16_t y_value = adc_read();


		// Move pixel based on joystick movement
		if (x_value > 2048 + JOYSTICK_DEADZONE) {
			if (x > 0) x--;
		} 
		else if (x_value < 2048 - JOYSTICK_DEADZONE) {
			if (x < SSD1306_WIDTH - 1) x++;
		}

		if (y_value > 2048 + JOYSTICK_DEADZONE) {
			if (y < SSD1306_HEIGHT - 1) y++;
		} 
		else if (y_value < 2048 - JOYSTICK_DEADZONE) {
			if (y > 0) y--;
		}     

		drawline = gpio_get(JOY_SW_PIN);

		// Draw pixel at new position
		SetPixel(buf, x, y, true);

		// Erase previous pixel if it's not equal
		if (x != x_prev || y != y_prev)
			SetPixel(buf, x_prev, y_prev, !drawline);	
		x_prev = x;
		y_prev = y;


		sleep_ms(40);
	}
}
