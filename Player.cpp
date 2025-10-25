#include "Player.h"

// nothing doing in the constructor and destructor
Player::Player() { }

void Player::init(int x,int height,int width) {
    printf("Player: Init\n");
    _x = 24 - width;  // x value on screen is fixed
    _y = 38 - height;  // y depends on height of screen and height of Player
    _height = height;
    _width = width;
    _speed = 3;  // default speed
    _score = 0;  // start score from zero
}
const int character [10][9]={
{0, 0, 1, 1, 1, 1, 1, 0, 0},
{0, 1, 0, 0, 0, 0, 0, 1, 0},
{1, 0, 1, 1, 0, 1, 1, 0, 1},
{1, 0, 0, 1, 0, 0, 1, 0, 1},
{1, 0, 0, 0, 0, 0, 0, 0, 1}, 
{1, 0, 0, 0, 0, 1, 1, 0, 1}, 
{0, 1, 0, 0, 0, 0, 0, 1, 0}, 
{0, 1, 1, 1, 1, 1, 1, 1, 0}, 
{0, 1, 1, 0, 0, 0, 1, 1, 0},
{1, 1, 1, 1, 0, 1, 1, 1, 1}, 
};
const int kick_1 [10][15]={
{0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
{0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0},
{0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
{0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0}, 
{0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0}, 
{0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, 
{0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}, 
{0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
{0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0}, 
};

const int kick_2 [10][15]={
{0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0}, 
{0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, 
{0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0}, 
{0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0}, 
{0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0}, 
{0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0}, 
{0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1}, 
{0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1}, 
{0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1}, 
{0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}, 
};
void Player::load_kick_frames(){
    _kick_frames[0]= (int*) kick_1;
    _kick_frames[1]= (int*) kick_2;

}
void Player::draw(N5110 &lcd) { 
    printf("Player: Draw\n");
    lcd.drawSprite(_x,_y,10,9,(int*)character); 
}

void Player::draw_kick_frame(N5110 &lcd) {
    if (_current_kick_frame < 2) {
        lcd.drawSprite(_x, _y, 10, 15, (int*)_kick_frames[_current_kick_frame]);
    }
}
void Player::reset_kick() {
    _current_kick_frame = 0;
    
}
void Player::update_kick() {
        _current_kick_frame++;
        
    }


int Player::get_current_kick_frame() {
    return _current_kick_frame;
}

void Player::update(UserInput input) {
    printf("Player: Update\n");

    // Update velocity based on input
    if (input.d == W) {
        _velocity.x = _speed;
    } else if (input.d == E) {
        _velocity.x = -_speed;
    } else {
        _velocity.x = 0;  // no movement
    }
    const float acc = 12.0f;  // gravity
    float dt = 0.25f;         // time step
// velocity with acceleration
    _velocity.y = _velocity.y + acc * dt;  // v = v₀ + a*t
if (_velocity.y > 0 && _y >= 28) {
    _velocity.y = 0;
}
if (_velocity.y > -1.0f && _velocity.y <= -0.5f){
    _velocity.y = 2;
}
 // Apply velocity to position
    _x += _velocity.x;
    _y += _velocity.y * dt + 0.5 * acc * dt * dt;
// Keep Player on screen horizontally
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
void Player::set_velocity(Position2D v) {
    _velocity = v;
}
void Player::set_pos(Position2D p) {
    _x = p.x;
    _y = p.y;
}

Position2D Player::get_velocity() {
    return _velocity;
}


void Player::add_score() { _score++; }

int Player::get_score() { return _score; }

Position2D Player::get_pos() { return {_x,_y}; }

int Player::get_height() { return _height; }

int Player::get_width() { return _width; }