// characteristics are defined in the modules that are responsible for them.

void BLE_Modules_Setup();

void BLESetup();

void BLE_ConnectionSetup();
void BLE_ConnectionCleanUp();
void BLE_ConnectionTick();

void BLETask(void *pvParameters);