#ifndef OUTPUTS_H
#define OUTPUTS_H

// Initialization and time-based updates
void outputs_init(void);
void outputs_update(void);

// Output modes
void outputs_all_off(void);
void outputs_set_movement(void);
void outputs_set_door_opening(void);
void outputs_set_door_closing(void);
void outputs_set_obstacle(void);

// Partial output control
void outputs_stop_buzzer(void);

// Optional background jingle control
void outputs_start_jingle(void);
void outputs_stop_jingle(void);

#endif