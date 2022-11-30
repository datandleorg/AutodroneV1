unsigned long timer_channel_1, timer_channel_2, timer_channel_3, timer_channel_4, esc_timer, esc_loop_timer;
unsigned long loop_timer;

void setupPWM()
{
    DDRD |= B01111000;
    loop_timer = micros();
    battery_voltage = (1023 + 65) * 1.2317;
}

void sendPWM()
{

    while (micros() - loop_timer < 4000);
    loop_timer = micros();

    PORTD |= B01111000;
    timer_channel_1 = esc_1 + loop_timer;
    timer_channel_2 = esc_2 + loop_timer;
    timer_channel_3 = esc_3 + loop_timer;
    timer_channel_4 = esc_4 + loop_timer;

    read_IMU();

    while (PORTD >= 16)
    {
        esc_loop_timer = micros();
        if (timer_channel_1 <= esc_loop_timer)
            PORTD &= B11110111;
        if (timer_channel_2 <= esc_loop_timer)
            PORTD &= B11101111;
        if (timer_channel_3 <= esc_loop_timer)
            PORTD &= B11011111;
        if (timer_channel_4 <= esc_loop_timer)
            PORTD &= B10111111;
    }
}
