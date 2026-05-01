#include "input_handler.h"
#include "config.h"
#include "porthub.h"
#include <M5Unified.h>

// External variables
extern int startBtnState, resetBtnState, buttonPushState, stopBtnState;
extern int faderValue, angleValue, probeStartState, probeGoalState;
extern int OP180_1_State, OP180_2_State;
extern float accX, accY, accZ, gyroX, gyroY, gyroZ;
extern float startaccX, startaccY, startaccZ;
extern float cumForce, force;
extern int trialRunning;
extern PortHub porthub;
extern uint8_t HUB_ADDR[6];

void update_inputs() {
  M5.update();
  
  // Get device accel data
  auto imu_update = M5.Imu.update();
  auto imu_data = M5.Imu.getImuData();
  accX = imu_data.accel.x;
  accY = imu_data.accel.y;
  accZ = imu_data.accel.z;
  gyroX = imu_data.gyro.x;
  gyroY = imu_data.gyro.y;
  gyroZ = imu_data.gyro.z;

  startBtnState = M5.BtnA.isPressed() ? BUTTON_ON : BUTTON_OFF;
  resetBtnState = M5.BtnB.isPressed() ? BUTTON_ON : BUTTON_OFF;

  // Read from PbHub Module
  porthub.hub_d_wire_value_A(HUB_ADDR[3], 1);
  porthub.hub_d_wire_value_B(HUB_ADDR[3], 1);
  buttonPushState = porthub.hub_d_read_value_A(HUB_ADDR[0]);
  stopBtnState = porthub.hub_d_read_value_B(HUB_ADDR[0]);
  faderValue = porthub.hub_a_read_value(HUB_ADDR[5]);
  angleValue = porthub.hub_a_read_value(HUB_ADDR[4]);
  probeStartState = porthub.hub_d_read_value_A(HUB_ADDR[3]);
  probeGoalState = porthub.hub_d_read_value_B(HUB_ADDR[3]);
  OP180_1_State = porthub.hub_d_read_value_A(HUB_ADDR[1]);
  OP180_2_State = porthub.hub_d_read_value_A(HUB_ADDR[2]);

  // Collect "force" during trial
  if (trialRunning > 0) {
    force = abs(accX - startaccX) + abs(accY - startaccY) + abs(accZ - startaccZ);
    cumForce = cumForce + force;
  }
}
