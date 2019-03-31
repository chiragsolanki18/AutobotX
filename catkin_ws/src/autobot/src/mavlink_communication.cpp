#include "mavlink_communication.h"
//#include <arduino.h>
#include "ros/ros.h"
#include <stdio.h>
char *uart_name = (char*)"/dev/ttyUSB0";
int baudrate = 57600;
std::string readings;
//Serial_Port serial;
Serial_Port serial_port(uart_name, baudrate);
mavlink_status_t mav_rec_status;
mavlink_message_t mav_rec_msg,m;
int mav_rec_chan = MAVLINK_COMM_0;
float vleft, vright;
mavlink_robot_sensor_readings_t mav_robot_sensor_readings;
mavlink_robot_position_change_t mav_robot_position_change;
mavlink_robot_distance_sensor_readings_t mav_robot_distance_sensor_readings;
mavlink_initial_status_t mav_initial_status;
mavlink_robot_dimensions_t mav_comm_robot_dimensions;
mavlink_left_wheel_pid_gains_t mav_comm_left_wheel_pid_gains;
mavlink_right_wheel_pid_gains_t mav_comm_right_wheel_pid_gains;
mavlink_desire_wheel_rpm_t mav_comm_desire_wheel_rpm;
mavlink_desire_cmd_val_t mav_comm_desire_cmd_val;

/*
void mav_send_robot_sensor_readings(uint32_t left_wheel_ticks, uint32_t right_wheel_ticks, float curr_heading)
{
  uint16_t x = 0;
  mavlink_message_t msg;
  x = mavlink_msg_robot_sensor_readings_pack(SYSTEM_ID, COMPONENT_ID, &msg, left_wheel_ticks, right_wheel_ticks,curr_heading);
  uint8_t send_buf[x];
  mavlink_msg_to_send_buffer(send_buf, &msg);
  int k = 0;
  for(k = 0; k < x; k++)
  {
     Serial.write(send_buf[k]);
  }
}
*/
void mav_send_robot_position_change(int delta_x, int delta_y, int delta_theta)
{
  int x = 0;

  mavlink_message_t msg;
  x = mavlink_msg_robot_position_change_pack(SYSTEM_ID, COMPONENT_ID, &msg, delta_x, delta_y, delta_theta);
  //int send_buf[x];
  //char send_bufc[x];
  //mavlink_msg_to_send_buffer(send_buf, &msg);
 //serial_port->initialize_defaults();
  int k = 0;
  //serial_port->start();
  serial_port.start();
  int j=serial_port.write_message(msg);
//  signal(SIGINT,quit_handler);
//int j=serial_port->write_message(msg);






/*  for(k = 0; k < x; k++)
  {

     send_bufc[k] = *(reinterpret_cast<char*>(send_buf[k]));
     send_bufc[k] = *(reinterpret_cast<int*>(send_buf[k]));
     //serial.write_message(msg);


  }*/

}

// String mav_send_robot_distance_sensor_readings(const int *ultrasonic_readings)
// { Sting check;
//   int x = 0;
//   mavlink_message_t msg;
//   x = mavlink_msg_robot_distance_sensor_readings_pack(SYSTEM_ID, COMPONENT_ID, &msg, ultrasonic_readings);
//   uint8_t send_buf[x];
//   mavlink_msg_to_send_buffer(send_buf, &msg);
//   int k = 0;
//   for(k = 0; k < x; k++)
//   {
//     check=check+'send_buf[k]';
//      //Serial.write(send_buf[k]);
//   }
//   return check;
// }

/*void mav_send_initial_status(float initial_heading, uint8_t ultrasonic_config, uint16_t update_freq, uint16_t encoder_ppr)
{
  uint16_t x = 0;
  mavlink_message_t msg;
  x = mavlink_msg_initial_status_pack(SYSTEM_ID, COMPONENT_ID, &msg, initial_heading, ultrasonic_config, update_freq, encoder_ppr);
  uint8_t send_buf[x];
  mavlink_msg_to_send_buffer(send_buf, &msg);
  int k = 0;
  for(k = 0; k < x; k++)
  {
     Serial.write(send_buf[k]);
  }
}*/
/*
void mav_decode_rec_byte(uint8_t rec_byte)
{
  if(mavlink_parse_char(mav_rec_chan, rec_byte, &mav_rec_msg, &mav_rec_status))
  {
    switch(mav_rec_msg.msgid)
    {
      case MAVLINK_MSG_ID_ROBOT_DIMENSIONS :
        mavlink_msg_robot_dimensions_decode(&mav_rec_msg, &mav_comm_robot_dimensions);
		motorcontrol.robot_wheel_radius = mav_comm_robot_dimensions.wheel_radius;
		motorcontrol.robot_distance_bw_wheels = mav_comm_robot_dimensions.distance_bw_wheels;
        break;

      case MAVLINK_MSG_ID_LEFT_WHEEL_PID_GAINS :
        mavlink_msg_left_wheel_pid_gains_decode(&mav_rec_msg, &mav_comm_left_wheel_pid_gains);
		motorcontrol.pid_leftmotor.Init(mav_comm_left_wheel_pid_gains.Kp, mav_comm_left_wheel_pid_gains.Ki, mav_comm_left_wheel_pid_gains.Kd);
        break;

      case MAVLINK_MSG_ID_RIGHT_WHEEL_PID_GAINS :
        mavlink_msg_right_wheel_pid_gains_decode(&mav_rec_msg, &mav_comm_right_wheel_pid_gains);
		motorcontrol.pid_rightmotor.Init(mav_comm_right_wheel_pid_gains.Kp, mav_comm_right_wheel_pid_gains.Ki, mav_comm_right_wheel_pid_gains.Kd);
        break;

      case MAVLINK_MSG_ID_DESIRE_WHEEL_RPM :
        mavlink_msg_desire_wheel_rpm_decode(&mav_rec_msg, &mav_comm_desire_wheel_rpm);
		motorcontrol.leftmotor.desirerpm = mav_comm_desire_wheel_rpm.left_wheel_rpm;
		motorcontrol.rightmotor.desirerpm = mav_comm_desire_wheel_rpm.right_wheel_rpm;
        break;

      case MAVLINK_MSG_ID_DESIRE_CMD_VAL :
        mavlink_msg_desire_cmd_val_decode(&mav_rec_msg, &mav_comm_desire_cmd_val);
		motorcontrol.cmd_val_v = mav_comm_desire_cmd_val.v;
		motorcontrol.cmd_val_w = mav_comm_desire_cmd_val.w;
		vleft = (2*motorcontrol.cmd_val_v - motorcontrol.robot_distance_bw_wheels*motorcontrol.cmd_val_w) / (2 * motorcontrol.robot_wheel_radius);  //in rad/s
		vright = (2*motorcontrol.cmd_val_v + motorcontrol.robot_distance_bw_wheels*motorcontrol.cmd_val_w) / (2 * motorcontrol.robot_wheel_radius);
		motorcontrol.leftmotor.desirerpm = vleft * (60.0/(2*pi));   //in rpm
		motorcontrol.rightmotor.desirerpm = vright * (60.0/(2*pi));
        break;

      default:
        break;
    }
  }
}
*/