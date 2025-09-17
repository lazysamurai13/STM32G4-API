#include <stdint.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include <stdio.h>
#include "esp_camera.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "esp_timer.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_err.h"

// Replace with your own Wi-Fi credentials
#define WIFI_SSID "Amararaja_2G"
#define WIFI_PASSWORD "ARDA@2255"

#define PART_BOUNDARY "123456789000000000000987654321"
static const char *TAG = "camera_stream";
// static const char *TAG = "wifi_example";

#define CAM_PIN_PWDN    -1
#define CAM_PIN_RESET   -1
#define CAM_PIN_XCLK    10
#define CAM_PIN_SIOD    40
#define CAM_PIN_SIOC    39
#define CAM_PIN_D7      48
#define CAM_PIN_D6      11
#define CAM_PIN_D5      12
#define CAM_PIN_D4      14
#define CAM_PIN_D3      16
#define CAM_PIN_D2      18
#define CAM_PIN_D1      17
#define CAM_PIN_D0      15
#define CAM_PIN_VSYNC   38
#define CAM_PIN_HREF    47
#define CAM_PIN_PCLK    13

// Function prototypes
static void wifi_event_handler  (void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
void wifi_init_sta				(void);
static esp_err_t index_handler  (httpd_req_t *req);
static esp_err_t stream_handler (httpd_req_t *req);
esp_err_t start_handler         (httpd_req_t *req);
esp_err_t stop_handler          (httpd_req_t *req);
httpd_handle_t start_webserver  (void);

static int     stream_enabled  = 1;
static int64_t stream_end_time = 0;

static camera_config_t camera_config = {
	.pin_pwdn       = CAM_PIN_PWDN,
	.pin_reset      = CAM_PIN_RESET,
	.pin_xclk       = CAM_PIN_XCLK,
	.pin_sccb_sda   = CAM_PIN_SIOD,
	.pin_sccb_scl   = CAM_PIN_SIOC,
	.pin_d7         = CAM_PIN_D7,
	.pin_d6         = CAM_PIN_D6,
	.pin_d5         = CAM_PIN_D5,
	.pin_d4         = CAM_PIN_D4,
	.pin_d3         = CAM_PIN_D3,
	.pin_d2         = CAM_PIN_D2,
	.pin_d1         = CAM_PIN_D1,
	.pin_d0         = CAM_PIN_D0,
	.pin_vsync      = CAM_PIN_VSYNC,
	.pin_href       = CAM_PIN_HREF,
	.pin_pclk       = CAM_PIN_PCLK,
	.xclk_freq_hz   = 20000000,
	.ledc_timer     = LEDC_TIMER_0,
	.ledc_channel   = LEDC_CHANNEL_0,
	.pixel_format   = PIXFORMAT_JPEG,
	.frame_size     = FRAMESIZE_SVGA,
	.jpeg_quality   = 12,
	.fb_count       = 2,
	.fb_location    = CAMERA_FB_IN_PSRAM,
};

esp_err_t start_handler(httpd_req_t *req) {
	stream_enabled = 1;
	stream_end_time = esp_timer_get_time() + 2 * 60 * 1000000; // 2 minutes in microseconds
	httpd_resp_sendstr(req, "Stream started");
	return ESP_OK;
}

esp_err_t stop_handler(httpd_req_t *req) {
	stream_enabled = 0;
	stream_end_time = 0;
	httpd_resp_sendstr(req, "Stream stopped");
	return ESP_OK;
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
        ESP_LOGI(TAG, "Retrying connection...");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Connected, got IP: " IPSTR, IP2STR(&event->ip_info.ip));
    }
}

static esp_err_t index_handler(]httpd_req_t *req) {
	const char html[] =
		"<html><head><title>ESP32S3 Camera Stream</title></head><body>"
		"<h2>ESP32S3 MJPEG Stream</h2>"
		"<button onclick=\"fetch('/start')\">Start Stream</button>"
		"<button onclick=\"fetch('/stop')\">Stop Stream</button><br><br>"
		"<img src='/stream' style='width:100%;max-width:640px;'/><br>"
		"</body></html>";
	httpd_resp_set_type(req, "text/html");
	httpd_resp_send(req, html, strlen(html));
	return ESP_OK;
}

void wifi_init_sta(void) {
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
        },
    };
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL);
    esp_wifi_start();
}

static esp_err_t stream_handler(httpd_req_t *req) {
	if (!stream_enabled) {
		httpd_resp_send_404(req);
		return ESP_OK;
	}
	camera_fb_t *fb = NULL;
	char part_buf[128];
	static int64_t last_frame = 0;
	if (!last_frame) {
		last_frame = esp_timer_get_time();
	}
	httpd_resp_set_type(req, "multipart/x-mixed-replace;boundary=" PART_BOUNDARY);
	while (stream_enabled) {
		if (stream_end_time > 0 && esp_timer_get_time() > stream_end_time) {
			stream_enabled = 0;
			break;
		}
		fb = esp_camera_fb_get();
		if (!fb) {
			ESP_LOGE(TAG, "Camera capture failed");
			httpd_resp_send_500(req);
			break;
		}
		size_t hlen = snprintf((char *)part_buf, 128,
			"--%s\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n",
			PART_BOUNDARY, fb->len);
		if (httpd_resp_send_chunk(req, (const char *)part_buf, hlen) != ESP_OK ||
			httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len) != ESP_OK ||
			httpd_resp_send_chunk(req, "\r\n", 2) != ESP_OK) {
			esp_camera_fb_return(fb);
			break;
		}
		esp_camera_fb_return(fb);
		int64_t fr_end = esp_timer_get_time();
		int64_t frame_time = fr_end - last_frame;
		last_frame = fr_end;
		ESP_LOGI(TAG, "MJPEG: %uKB %ums", (fb->len/1024), (uint32_t)(frame_time/1000));
	}
	return ESP_OK;
}

httpd_handle_t start_webserver(void) {
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	httpd_handle_t server = NULL;
	if (httpd_start(&server, &config) == ESP_OK) {
		// MJPEG stream endpoint
		httpd_uri_t stream_uri = {
			.uri = "/stream",
			.method = HTTP_GET,
			.handler = stream_handler,
			.user_ctx = NULL
		};
		httpd_register_uri_handler(server, &stream_uri);

		// Simple HTML page endpoint
		httpd_uri_t index_uri = {
			.uri = "/",
			.method = HTTP_GET,
			.handler = index_handler,
			.user_ctx = NULL
		};
		httpd_register_uri_handler(server, &index_uri);

		// Start stream endpoint
		httpd_uri_t start_uri = {
			.uri = "/start",
			.method = HTTP_GET,
			.handler = start_handler,
			.user_ctx = NULL
		};
		httpd_register_uri_handler(server, &start_uri);

		// Stop stream endpoint
		httpd_uri_t stop_uri = {
			.uri = "/stop",
			.method = HTTP_GET,
			.handler = stop_handler,
			.user_ctx = NULL
		};
		httpd_register_uri_handler(server, &stop_uri);
	}
	return server;
}

void app_main(void) 
{
	nvs_flash_init();
    wifi_init_sta();
	// wifi_init_sta();

	esp_err_t err = esp_camera_init(&camera_config);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Camera init failed with error 0x%x", err);
		return;
	}
	start_webserver();
	ESP_LOGI(TAG, "Camera stream ready at /stream");
}