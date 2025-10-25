#include "Dec.h"

// Dec character sprites
const int dec[10][9] = {
    {0, 0, 1, 1, 1, 1, 1, 0, 0}, 
    {0, 1, 0, 0, 0, 0, 0, 1, 0}, 
    {1, 0, 1, 1, 1, 1, 1, 0, 1}, 
    {1, 0, 1, 0, 0, 1, 0, 0, 1}, 
    {1, 0, 0, 0, 0, 0, 0, 0, 1}, 
    {1, 0, 0, 0, 0, 0, 0, 0, 1}, 
    {0, 1, 0, 0, 0, 0, 0, 1, 0}, 
    {0, 1, 1, 1, 1, 1, 1, 1, 0}, 
    {0, 1, 1, 0, 0, 0, 1, 1, 0}, 
    {1, 1, 1, 1, 0, 1, 1, 1, 1}
};

const int dec_kick_1[10][15] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0}, 
    {0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1}, 
    {0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1}, 
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1}, 
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1}, 
    {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0}, 
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1}
};

const int dec_kick_2[10][15] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0}, 
    {0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1}, 
    {0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1}, 
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1}, 
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1}, 
    {1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0}, 
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}, 
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1}
};
Dec::Dec(){
    _dec_kick_animation_active = false;
    _dec_kick_current_frame = 0;
    _x = 60;
    _y = 28;
    _height = 10;
    _width = 9;
    _velocity = {0, 0};
}
void Dec::init(int height, int width, int x, int y){
    _x = x;
    _y= y;
    _height = height;
    _width = width;
    
}
void Dec::update() {
    const float acc = 9.81f;  // gravity
    float dt = 0.25f;         // time step

    // Update velocity with acceleration
    _velocity.y = _velocity.y + acc * dt;  // v = v₀ + a*t
if (_velocity.y > -0.5f && _velocity.y <= 0.0f) {
    _velocity.y = 1;
}
    // Update position 
    _x += _velocity.x;
    _y += _velocity.y * dt + 0.5 * acc * dt * dt;  // y = y₀ + v₀*t + ½*a*t²
     if (_x < 1) {
        _x = 1;
    } else if (_x > WIDTH - _width) {
        _x = WIDTH - _width;
    }
    if (_y > 28) {
        _y = 28;
    } else if (_y < 0) {
        _y = 1;
    }
}

void Dec::reset_kick() {
    _dec_kick_animation_active = true;
    _dec_kick_current_frame = 0;
}

void Dec::update_kick() {
    if (_dec_kick_animation_active) {
        _dec_kick_current_frame++;
        if (_dec_kick_current_frame >= 2) {
            _dec_kick_animation_active = false; 
        }
    }
}

void Dec::draw(N5110 &lcd) {
    if (_dec_kick_animation_active) {
        draw_kick_frame(lcd);
    } else {
        lcd.drawSprite(_x, _y, 10, 9, (int*)dec);
    }
}

void Dec::draw_kick_frame(N5110 &lcd) {
    if (_dec_kick_current_frame == 0) {
        lcd.drawSprite(_x, _y, 10, 15, (int*)dec_kick_1);
    } else if (_dec_kick_current_frame == 1) {
        lcd.drawSprite(_x, _y, 10, 15, (int*)dec_kick_2);
    }
}

int Dec::get_current_kick_frame() {
    return _dec_kick_current_frame;
}
Position2D Dec::get_velocity() {
    return _velocity;
}

void Dec::set_velocity(Position2D v) {
    _velocity = v;
}

Position2D Dec::get_pos() {
    return Position2D{_x, _y};
}

void Dec::set_pos(Position2D p) {
    _x = p.x;
    _y = p.y;
}
