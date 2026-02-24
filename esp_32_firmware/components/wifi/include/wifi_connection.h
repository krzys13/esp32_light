#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

bool wifi_connection_status(void);
// Publiczne API modułu WiFi
void wifi_init_sta(void);


#ifdef __cplusplus
}
#endif
