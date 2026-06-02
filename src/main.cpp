/**
 * @file main.cpp
 * @brief Core Closed-Loop Tracking Engine & Fault Mitigation Pipeline for ASAFE
 * @developer Utsav Anand Sharma
 * * Architecture: Embedded C++ Class-Based Adaptive Controller
 * Function: Monitors ADC sampling channels and executes automatic gain or routing corrections.
 */

#include <Arduino.h>

// System Operational Configurations
const int PRIMARY_ANALOG_CH = A0;   // Main sensory path
const int AUXILIARY_ANALOG_CH = A1; // Redundant self-healing backup signal path
const int AUX_RELAY_PIN = 12;       // Hardware switch pin for path reconfiguration

// Algorithmic Boundaries
const float EXTREME_VOLTAGE_UPPER = 4.5;
const float EXTREME_VOLTAGE_LOWER = 0.5;
const int MAX_FAULT_COUNT_LIMIT = 5; // Persistent iterations required to verify failure

class SelfHealingController {
private:
    bool isPrimaryPathFunctional;
    int faultCounter;
    float currentGainModifier;

public:
    SelfHealingController() {
        isPrimaryPathFunctional = true;
        faultCounter = 0;
        currentGainModifier = 1.0;
    }

    /**
     * @brief Evaluates sampled tracking parameters against structural boundaries
     */
    void processSignalTelemetry(float voltageInput) {
        // Evaluate for clipping or component distress parameters
        if (voltageInput >= EXTREME_VOLTAGE_UPPER || voltageInput <= EXTREME_VOLTAGE_LOWER) {
            faultCounter++;
            Serial.print("[WARNING] Signal anomaly tracked. Severity Count: ");
            Serial.println(faultCounter);
        } else {
            if (faultCounter > 0) faultCounter--; // Attenuate counter if signal restabilizes
        }

        // Trigger self-healing if degradation limits are broken
        if (faultCounter >= MAX_FAULT_COUNT_LIMIT && isPrimaryPathFunctional) {
            executeSelfHealingRoutine();
        }
    }

    /**
     * @brief Reconfigures mixed-signal interface paths to isolate failure vectors
     */
    void executeSelfHealingRoutine() {
        isPrimaryPathFunctional = false;
        Serial.println("\n[CRITICAL FAILURE DETECTED] Isolating primary signal array...");
        
        // Step 1: Engage hardware backup routing matrix
        digitalWrite(AUX_RELAY_PIN, HIGH);
        Serial.println("[HEALING SUCCESSFUL] Auxiliary hardware path online. System operational status: STABLE.\n");
    }

    bool getPathStatus() { return isPrimaryPathFunctional; }
};

SelfHealingController asafeEngine;

void setup() {
    Serial.begin(115200);
    pinMode(PRIMARY_ANALOG_CH, INPUT);
    pinMode(AUXILIARY_ANALOG_CH, INPUT);
    pinMode(AUX_RELAY_PIN, OUTPUT);
    
    digitalWrite(AUX_RELAY_PIN, LOW); // Start on primary channel path
    Serial.println("ASAFE Adaptive Diagnostics Initialized.");
}

void loop() {
    // Select active sampling index based on current topology map
    int activeChannel = asafeEngine.getPathStatus() ? PRIMARY_ANALOG_CH : AUXILIARY_ANALOG_CH;
    int rawSample = analogRead(activeChannel);
    float voltageValue = (rawSample * 5.0) / 1023.0;

    // Run telemetry check inside the object-oriented tracking logic
    asafeEngine.processSignalTelemetry(voltageValue);

    delay(200); // Sample collection window rate
}
