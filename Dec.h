#ifndef DEC_H
#define DEC_H

#include "mbed.h"
#include "N5110.h"
#include "Utils.h"

class Dec {
public:
Dec();
    void draw(N5110 &lcd);
    void update();
    void init(int height, int width, int x, int y);
    Position2D get_velocity();
    void set_velocity(Position2D v);
    Position2D get_pos();
    void set_pos(Position2D p);
    void reset_kick();
    void update_kick();
    void draw_kick_frame(N5110 &lcd);
    int get_current_kick_frame();
    bool _dec_kick_animation_active;

private:
    Position2D _velocity;
    int _x, _y, _height, _width;
    
    int _dec_kick_current_frame;

};

#endif