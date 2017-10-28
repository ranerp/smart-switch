#include <BLECentralRole.h>

// LED and button pin
#define LED_PIN     LED_BUILTIN
#define BUTTON_PIN  USER1_BUTTON

// create central instance
BLECentralRole bleCentral = BLECentralRole();

// create remote service with UUID compliant
//BLERemoteService remoteGenericAccessService = BLERemoteService("1800");
//BLERemoteService remoteGenericAttributeService = BLERemoteService("1801");
BLERemoteService remoteCustomService = BLERemoteService("0783b03e8535b5a07140a304d2495cb7");

// create remote characteristics with UUID and properties compliant those in led_switch example 
//BLERemoteCharCharacteristic remoteNameCharacteristics = BLERemoteCharCharacteristic("00002a0000001000800000805f9b34fb", BLERead);
//BLERemoteCharacteristic remotePrefferedConnectionParametersCharacteristics = BLERemoteCharacteristic("00002a0400001000800000805f9b34fb", BLERead);
BLERemoteCharacteristic remoteCustomCharacteristic = BLERemoteCharacteristic("0783b03e8535b5a07140a304d2495cb9", BLERead | BLEWrite);

bool buttonState;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  
  buttonState = digitalRead(BUTTON_PIN);
  // add service and characteristics
  //bleCentral.addRemoteAttribute(remoteGenericAccessService);
  //bleCentral.addRemoteAttribute(remoteGenericAttributeService);
  bleCentral.addRemoteAttribute(remoteCustomService);
  //bleCentral.addRemoteAttribute(remoteNameCharacteristics);
  //bleCentral.addRemoteAttribute(remotePrefferedConnectionParametersCharacteristics);
  bleCentral.addRemoteAttribute(remoteCustomCharacteristic);
  
  // assign event handlers for central events
  bleCentral.setEventHandler(BLEScanReceived, receiveAdvPck);  
  bleCentral.setEventHandler(BLEConnected, bleCentralConnectHandler);
  bleCentral.setEventHandler(BLEDisconnected, bleCentralDisconnectHandler);
  bleCentral.setEventHandler(BLERemoteServicesDiscovered, bleCentralRemoteServicesDiscoveredHandler);
 
  // set scan parameters
  // interval and window in 0.625 ms increments 
  bleCentral.setScanInterval(3200); // 2 sec
  bleCentral.setScanWindow(800);  // 0.5 sec

  // begin initialization and start scanning
  bleCentral.begin(); 
  
  Serial.println("Scanning...");
}

void loop() {
  if(bleCentral.connected()){
	
	//turn on BLE led
	digitalWrite(BLE_LED, HIGH);

  } else { // if we are not connected we are scanning hence blink BLE led
    digitalWrite(BLE_LED, LOW);
    delay(200);
    digitalWrite(BLE_LED, HIGH);
    delay(200);
  }
}

void receiveAdvPck(BLEPeripheralPeer& peer){
  char advertisedName[31];
  byte len;
  
  // search for a device that advertises "Grid Eye" name
  peer.getFieldInAdvPck(BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME, advertisedName, len);
  if(len == 0) // field not found
      peer.getFieldInAdvPck(BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, advertisedName, len);
  if(len != 0){ // the field was found
  Serial.println(advertisedName);
    if(!strcmp(advertisedName, "Grid Eye")) {
      Serial.println("Found grid eye");
      // Name found. Connect to the peripheral
      bleCentral.connect(peer);
    }
  } 
}

void bleCentralConnectHandler(BLEPeripheralPeer& peer) {
  // peer connected event handler  
  
  Serial.print("Connected event, peripheral: ");
  Serial.println(peer.address());
}

void bleCentralDisconnectHandler(BLEPeripheralPeer& peer) {
  // peer disconnected event handler
  Serial.print("Disconnected event, peripheral: ");
  Serial.println(peer.address());
}

void bleCentralRemoteServicesDiscoveredHandler(BLEPeripheralPeer& peer) {
  // peer remote services discovered event handler
  Serial.print("Remote services discovered event, peripheral: ");
  Serial.println(peer.address());
}
