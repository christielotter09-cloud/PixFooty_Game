#include "PixFootyEngine.h"
#include <ios>
DigitalIn buttonKick(A1);
DigitalIn buttonJump(A0);
PixFootyEngine::PixFootyEngine(){ user_goals = 0; dec_goals = 0;}    
bool _goal_scored = false;
Timer _goal_timer;
void PixFootyEngine::init(int player_position, int player_height, int player_width, int ball_size, int speed){
    //printf("Pong Engine: Init\n");
    _ball.init(ball_size,speed);
    _player.init(player_position, player_height, player_width);
    _player.load_kick_frames();  

}
int PixFootyEngine::update(UserInput input, N5110 &lcd) {
    check_goal(lcd); // first priority

    if (_goal_scored) {
        _dec.set_velocity({0, 0});
        _ball.set_velocity({0, 0});
        return user_goals;
    }

    _ball.update();
    _player.update(input);
    dec_logic();
    _dec.update_kick();
    _dec.update();

    Position2D player_velocity = _player.get_velocity();
    Position2D player_pos = _player.get_pos();

    check_wall_collision();
    player_on_player();
    check_dec_collision();
    check_player_collision();
    check_goal_collision();
    player_jump();

    if (buttonKick.read() && !_kick_animation_active) {
        _kick_animation_active = true;
        _player.reset_kick();
    }

    if (_kick_animation_active) {
        _player.update_kick();
        if (_player.get_current_kick_frame() >= 2) {
            _kick_animation_active = false;
        }
    }

    if (buttonJump.read() && (fabs(player_velocity.y) > 0)) {
        _player_jump = false;
    } else if (buttonJump.read() && !_player_jump) {
        _player_jump = true;
    }

    return user_goals;
}


void PixFootyEngine::draw(N5110 &lcd) {

    Position2D ball_pos = _ball.get_pos();
    int size = _ball.get_size();
    int speed = abs(_ball.get_velocity().x);  
    _map.draw(lcd);
    _dec.draw(lcd);
      // Score sprite based on user_goals
    if (user_goals >= 0 && user_goals <= 8 && _goal_scored == false) {
        lcd.drawSprite(19, 0, 28, 28, (int*)_map.player_score[user_goals]);
    }
    if (dec_goals >= 0 && dec_goals <= 8 && _goal_scored == false) {
        lcd.drawSprite(47, 0, 19, 20, (int*)_map.dec_score[dec_goals]);
    }
    _ball.draw(lcd);
   
     if (_kick_animation_active) {
        _player.draw_kick_frame(lcd);
    } else {
        _player.draw(lcd);
    }
   
     if (_kick_animation_active) {
        _player.draw_kick_frame(lcd);
    } else {
        _player.draw(lcd);
    }
 
    // Goal is scored, display "GOAL!" 
    if (_goal_scored) {
        lcd.printString(" GOAAAAAAAAAL! ", 0, 1);  
     printf("Goal scored. Timer: %d ms\n", _goal_timer.read_ms());  // check timer
        // Wait for 2 seconds
        if (_goal_timer.read_ms() > 2000) {
            _goal_scored = false;  
            _goal_timer.stop(); 
        }
    }
}

void PixFootyEngine::check_wall_collision() {
    //printf("Pong Engine: Check Wall Collision\n");
    // read current ball attributes
    Position2D ball_pos = _ball.get_pos();
    Position2D ball_velocity = _ball.get_velocity();
    int size = _ball.get_size();
    const float damping = 0.9;  // Damping factor
    float min_bounce_speed = 2.0f;
    int crossbar_x = 74;
    int crossbar_y = 19;

    int ball_left = ball_pos.x;
    int ball_right = ball_pos.x + size;
    int ball_top = ball_pos.y;
    int ball_bottom = ball_pos.y + size;

    // check if hit roof
    if (ball_top <= 1) {  
        ball_pos.y = 1; 
        ball_velocity.y = -ball_velocity.y;  // flip velocity
        //check if hit ground
    } else if (ball_bottom >= 37) {
        ball_pos.y = 37 - size;
        if (fabs(ball_velocity.y) > min_bounce_speed) {
            ball_velocity.y = -ball_velocity.y * damping;
            } else {
            ball_velocity.y = 0;  // stop bouncing
        }
    //check if hit right wall
    }
    if ((ball_right >= WIDTH) && (crossbar_y >= ball_bottom)) {
    ball_pos.x = WIDTH - size;
    ball_velocity.x = -ball_velocity.x * 0.5;
    }else if ((ball_left <= 1 ) && (crossbar_y >= ball_bottom)) {
        // hit left wall
        ball_pos.x = 1 + size ; 
        ball_velocity.x = -ball_velocity.x * 0.5 ;    // flip velcoity 
    }

    _ball.set_velocity(ball_velocity);
    _ball.set_pos(ball_pos);
}
void PixFootyEngine::player_jump(){
    Position2D player_pos = _player.get_pos();
    Position2D player_velocity = _player.get_velocity();
    if(_player_jump == true){
        player_velocity.y = -10;
        _player_jump = false;
    }
    
    _player.set_velocity(player_velocity);
    _player.set_pos(player_pos);

}
void PixFootyEngine::check_dec_collision(){
    Position2D ball_pos = _ball.get_pos();
    Position2D ball_velocity = _ball.get_velocity();
    Position2D dec_pos = _dec.get_pos();
    Position2D dec_velocity = _dec.get_velocity();
    int dec_height = 9;
    int dec_width = 10;
    int ball_size = _ball.get_size();
    
    int dec_left = dec_pos.x;
    int dec_right = dec_pos.x + dec_width;
    int dec_top = dec_pos.y;
    int dec_bottom = dec_pos.y + dec_height;
    
    int ball_left = ball_pos.x;
    int ball_right = ball_pos.x + ball_size;
    int ball_top = ball_pos.y;
    int ball_bottom = ball_pos.y + ball_size;
    
    float dec_damping = 0.2;

     // Regular player collision check
    bool x_dec_overlap = (ball_left < dec_right) && (ball_right > dec_left);
    bool y_dec_overlap = (ball_top < dec_bottom) && (ball_bottom > dec_top);
    
    // Handle regular dec collision if not already kicked
    if (x_dec_overlap && y_dec_overlap) {
        int overlap_left   = ball_right - dec_left;
        int overlap_right  = dec_right - ball_left;
        int overlap_top    = ball_bottom - dec_top;
        int overlap_bottom = dec_bottom - ball_top;
        
        // smallest overlap
        int min_overlap = std::min({overlap_left, overlap_right, overlap_top, overlap_bottom});
        
        if (min_overlap == overlap_top) {
            // Hit top of player 
            ball_pos.y = dec_top - ball_size;
            ball_velocity.y = -fabs(ball_velocity.y);
        } else if (min_overlap == overlap_bottom) {
            // Bottom of player 
            ball_pos.y = dec_bottom;
            ball_velocity.y = fabs(ball_velocity.y);
        } else if (min_overlap == overlap_left) {
            if (dec_velocity.x > 0) {
                ball_pos.x = dec_left - ball_size;
                ball_velocity.x = -fabs(ball_velocity.x) * dec_damping;
            } else if (dec_velocity.x < 0) {
                ball_pos.x = dec_left - ball_size;
                ball_velocity.x = -fabs(ball_velocity.x) - 2;
            } else {
                // Left side
                ball_pos.x = dec_left - ball_size;
                ball_velocity.x = -fabs(ball_velocity.x) * 0.6;
            }
        } else if (min_overlap == overlap_right) {
            if (dec_velocity.x > 0) {
                // Right side
                ball_pos.x = dec_right;
                ball_velocity.x = fabs(ball_velocity.x) + 2;
            } else if (dec_velocity.x < 0) {
                ball_pos.x = dec_right;
                ball_velocity.x = fabs(ball_velocity.x) * dec_damping;
            } else {
                ball_pos.x = dec_right;
                ball_velocity.x = fabs(ball_velocity.x) * 0.6;
            }
        }
        
        _ball.set_velocity(ball_velocity);
        _ball.set_pos(ball_pos);
    }
}
void PixFootyEngine::dec_logic() {
    Position2D ball_pos = _ball.get_pos();
    Position2D ball_velocity = _ball.get_velocity();
    Position2D dec_pos = _dec.get_pos();
    Position2D dec_velocity = _dec.get_velocity();
    int dec_height = 9;
    int dec_width = 10;
    int ball_size = _ball.get_size();

    int ball_left = ball_pos.x;
    int ball_right = ball_pos.x + ball_size;
    int ball_top = ball_pos.y;
    int ball_bottom = ball_pos.y + ball_size;

    int dec_left = dec_pos.x;
    int dec_right = dec_pos.x + dec_width;
    int dec_top = dec_pos.y;
    int dec_bottom = dec_pos.y + dec_height;

    const float prediction_frames = 5.0f;
    float future_ball_x = ball_pos.x + ball_velocity.x * prediction_frames;
    float future_ball_y = ball_pos.y + ball_velocity.y * prediction_frames;

    dec_velocity.x = 0;

    // Predict Ball Landing and Move 
    if (fabs(ball_velocity.x) > 0.1f) {
        if (future_ball_x > dec_pos.x + dec_width / 2) {
            dec_velocity.x = 3.0f;
        } else if (future_ball_x < dec_pos.x + dec_width / 2) {
            dec_velocity.x = -3.0f;
        }
    }
    if (ball_velocity.x == 0 && 
        ball_pos.x < dec_left - 5) {
        dec_velocity.x = -1.5f;  // move left
        } else if (ball_velocity.x == 0 && 
           ball_pos.x > dec_right + 5) {
        dec_velocity.x = 1.5f;   // move right
}


    //  Jump if ball coming above 
    bool ball_above_dec = (future_ball_y < dec_top - (dec_height / 2));
    bool ball_in_jump_zone = (future_ball_x > dec_left - 8) && (future_ball_x < dec_right + 8);

    if (fabs(ball_velocity.x) > 0 && ball_above_dec && ball_in_jump_zone) {
        if (dec_pos.y >= 28) {
            // Randomize Dec's jump, 70% chance 
            if (rand() % 100 < 70) {
                dec_velocity.y = -10.0f;
            }
        }
    }

    //  Kick if ball is very Close 
    bool ball_close = (ball_right > dec_pos.x - 5) && (ball_right < dec_pos.x) &&
                    (ball_bottom >= dec_top + dec_height/2) && (ball_top <= dec_bottom);

    if (ball_close && !_dec._dec_kick_animation_active) {
        // 65% chance to kick
        if (rand() % 100 < 65) {
            _dec.reset_kick();
            // variation in kick
            int kick_x = -6 + (rand() % 3 - 1);  // -7, -6, or -5
            int kick_y = -12 + (rand() % 3 - 1); // -13, -12, or -11
            _ball.set_velocity({kick_x, kick_y});
        }
        
    }
     // Return to Defend, ball far away
    if (fabs(ball_pos.x - dec_pos.x) > 30) {
        if (dec_pos.x > 60) {
            dec_velocity.x = -1.5f;
        } else if (dec_pos.x < 60) {
            dec_velocity.x = 1.5f;
        }
    }
    // Random reaction delay, 
    if (rand() % 100 < 5) {
        dec_velocity.x = 0; // 5% chance Dec hesitates randomly
    }
    _dec.set_velocity(dec_velocity);
    _dec.set_pos(dec_pos);
}

void PixFootyEngine::player_on_player() {
    Position2D player_pos = _player.get_pos();
    Position2D player_velocity = _player.get_velocity();
    int player_height = _player.get_height();
    int player_width = _player.get_width();
    Position2D dec_pos = _dec.get_pos();
    Position2D dec_velocity = _dec.get_velocity();
    int dec_height = 9;
    int dec_width = 10;

    int player_left = player_pos.x;
    int player_right = player_pos.x + player_width;
    int player_kick = player_right + 5;
    int player_top = player_pos.y;
    int player_bottom = player_pos.y + player_height;

    int dec_left = dec_pos.x;
    int dec_right = dec_pos.x + dec_width;
    int dec_top = dec_pos.y;
    int dec_bottom = dec_pos.y + dec_height;
    //Kick push
    int kick_push = 8.0f;
      // Kick push only if kick animation is active
    if (_kick_animation_active) {
        
        // Kick range
        int kick_left = player_right;
        int kick_right = player_right + 5;
        
        bool dec_in_kick_range = (dec_left <= kick_right - 1) && (dec_left >= kick_left);
        bool y_overlap = (player_top < dec_bottom) && (player_bottom > dec_top);
        
        // Apply kick push 
        if (dec_in_kick_range && y_overlap) {
            float kick_push = 8.0f;
            dec_pos.x += kick_push;
            dec_velocity.x = 5.0f;
            
            _dec.set_pos(dec_pos);
            _dec.set_velocity(dec_velocity);
        }
    }
    bool x_overlap = (player_left < dec_right) && (player_right > dec_left);
    bool y_overlap = (player_top < dec_bottom) && (player_bottom > dec_top);

    if (x_overlap && y_overlap) {
        const float push_strength = 1.0f;

        int overlap_left = player_right - dec_left;
        int overlap_right = dec_right - player_left;
        int min_overlap = std::min(overlap_left, overlap_right);

        if (overlap_left < overlap_right) {
            // Push player left, Dec right
            player_pos.x -= min_overlap / 2.0f;
            dec_pos.x += min_overlap / 2.0f;
        } else {
            // Player right, Dec left
            player_pos.x += min_overlap / 2.0f;
            dec_pos.x -= min_overlap / 2.0f;
        }

       
        if (player_velocity.x > 0 && dec_velocity.x == 0) {
            dec_pos.x += push_strength;
        } else if (player_velocity.x < 0 && dec_velocity.x == 0) {
            dec_pos.x -= push_strength;
        } else if (dec_velocity.x > 0 && player_velocity.x == 0) {
            player_pos.x += push_strength;
        } else if (dec_velocity.x < 0 && player_velocity.x == 0) {
            player_pos.x -= push_strength;
        } else if ((player_velocity.x > 0 && dec_velocity.x < 0) || 
                   (player_velocity.x < 0 && dec_velocity.x > 0)) {
            _player.set_velocity({0, player_velocity.y});
            _dec.set_velocity({0, dec_velocity.y});
        }

        _player.set_pos(player_pos);
        _dec.set_pos(dec_pos);
    }
}


void PixFootyEngine::check_player_collision() {
    Position2D ball_pos = _ball.get_pos();
    Position2D ball_velocity = _ball.get_velocity();
    Position2D player_pos = _player.get_pos();
    Position2D player_velocity = _player.get_velocity();
    int player_height = _player.get_height();
    int player_width = _player.get_width();
    int ball_size = _ball.get_size();
    Position2D dec_pos = _dec.get_pos();
    Position2D dec_velocity = _dec.get_velocity();
    int dec_height = 9;
    int dec_width = 10;
    
    
    int dec_left = dec_pos.x;
    
    
    int player_left = player_pos.x;
    int player_right = player_pos.x + player_width;
    int player_top = player_pos.y;
    int player_bottom = player_pos.y + player_height;
    
    int ball_left = ball_pos.x;
    int ball_right = ball_pos.x + ball_size;
    int ball_top = ball_pos.y;
    int ball_bottom = ball_pos.y + ball_size;
    
    float player_damping = 0.2;
    
    if (_kick_animation_active) {
        // Kick extends 5 pixels 
        int kick_left = player_right;
        int kick_right = player_right + 5;
        
        //Overlap check
        bool kick_x_overlap = (ball_left < kick_right) && (ball_right > kick_left);
        bool kick_y_overlap = (ball_top < player_bottom) && (ball_bottom > player_top);
        
        if (kick_x_overlap && kick_y_overlap) {
            // Calculate pos of ball in the kick
            float x_penetration = std::min(kick_right - ball_left, ball_right - kick_left);
            if (dec_left < kick_right + ball_size){
                ball_right = dec_left;
                ball_velocity.x = -10.0f; 
                ball_velocity.y = -20.0f;   
            }
            // Different kick behaviors 
            else if (x_penetration <= 1.0f) {
                // Roll ball along floor
                ball_velocity.x = 10.0f;
                ball_velocity.y = 0.0f;   // No vertical movement
                
            } else if (x_penetration <= 3.0f) {
                // Equal x and y velocity
                ball_velocity.x = 8.0f;
                ball_velocity.y = -10.0f;   
                
            } else {
                // More y velocity than x
                ball_velocity.x = 5.0f;
                ball_velocity.y = -18.0f; 
            }
             
            _ball.set_velocity(ball_velocity);

            ball_pos.x = kick_right + 1;
            _ball.set_pos(ball_pos);
           // Prevent multiple kicks 
            _ball_kicked = true;
 
            check_dec_collision();

            return;
        }
    }
    
    // Reset kick flaG
    if (!_kick_animation_active) {
        _ball_kicked = false;
    }
    
    // Regular player collision check
    bool x_overlap = (ball_left < player_right) && (ball_right > player_left);
    bool y_overlap = (ball_top < player_bottom) && (ball_bottom > player_top);
    
    if (x_overlap && y_overlap && !_ball_kicked) {
        // Collision happened, figure out direction
        int overlap_left   = ball_right - player_left;
        int overlap_right  = player_right - ball_left;
        int overlap_top    = ball_bottom - player_top;
        int overlap_bottom = player_bottom - ball_top;
        
        // smallest overlap 
        int min_overlap = std::min({overlap_left, overlap_right, overlap_top, overlap_bottom});
        
        if (min_overlap == overlap_top) {
            // Hit top of player 
            ball_pos.y = player_top - ball_size;
            ball_velocity.y = -fabs(ball_velocity.y);
        } else if (min_overlap == overlap_bottom) {
            // Bottom of player
            ball_pos.y = player_bottom;
            ball_velocity.y = fabs(ball_velocity.y);
        } else if (min_overlap == overlap_left) {
            if (player_velocity.x > 0) {
                ball_pos.x = player_left - ball_size;
                ball_velocity.x = -fabs(ball_velocity.x) * player_damping;
            } else if (player_velocity.x < 0) {
                ball_pos.x = player_left - ball_size;
                ball_velocity.x = -fabs(ball_velocity.x) - 2;
            } else {
                // left side 
                ball_pos.x = player_left - ball_size;
                ball_velocity.x = -fabs(ball_velocity.x) * 0.6;
            }
        } else if (min_overlap == overlap_right) {
            if (player_velocity.x > 0) {
                // Right side
                ball_pos.x = player_right;
                ball_velocity.x = fabs(ball_velocity.x) + 2;
            } else if (player_velocity.x < 0) {
                // 
                ball_pos.x = player_right;
                ball_velocity.x = fabs(ball_velocity.x) * player_damping;
            } else {
                ball_pos.x = player_right;
                ball_velocity.x = fabs(ball_velocity.x) * 0.6;
            }
        }
        
        _ball.set_velocity(ball_velocity);
        _ball.set_pos(ball_pos);
    }
}

void PixFootyEngine::check_goal_collision() {

    Position2D ball_pos = _ball.get_pos();
    Position2D ball_velocity = _ball.get_velocity();
    int size = _ball.get_size();
    float ball_center_x = ball_pos.x + size / 2.0f;
    float ball_center_y = ball_pos.y + size / 2.0f;

    int crossbar_x1 = 10;
    int crossbar_y = 19;

    int crossbar_x2 = 74;
    
    int ball_left = ball_pos.x;
    int ball_right = ball_pos.x + size;
    int ball_top = ball_pos.y;
    int ball_bottom = ball_pos.y + size;
    bool hit1 = (crossbar_x1 <= ball_right && crossbar_x1 > ball_left &&
            crossbar_y >= ball_top && crossbar_y < ball_bottom);
    //Crossbar and goal
    if (hit1) {
    // bALL centre from crossbar
    float dx1 = fabs(ball_center_x - crossbar_x2);
    float dy1 = fabs(ball_center_y - crossbar_y);
    if (dy1 > dx1) {
        ball_velocity.y = -ball_velocity.y *0.95;
    } else {
        ball_velocity.x = -ball_velocity.x * 0.95;
    }
    }
    //Top of goal
    bool top_of_goal1 = (crossbar_x1 >= ball_right && 1 <= ball_left &&
            crossbar_y >= ball_top && crossbar_y <= ball_bottom);
    if (top_of_goal1){
    ball_bottom = crossbar_y + 1;
    ball_velocity.y = -ball_velocity.y * 0.4;
    }

    bool hit2 = (crossbar_x2 >= ball_left && crossbar_x2 < ball_right &&
            crossbar_y >= ball_top && crossbar_y < ball_bottom);
    //Crossbar and goal
    if (hit2) {

    float dx2 = fabs(ball_center_x - crossbar_x2);
    float dy2 = fabs(ball_center_y - crossbar_y);

    if (dy2 > dx2) {
        ball_velocity.y = -ball_velocity.y *0.95;
    } else {
        ball_velocity.x = -ball_velocity.x * 0.95;
    }
    }
    //Top of goal
    bool top_of_goal2 = (crossbar_x2 <= ball_left && WIDTH >= ball_right &&
            crossbar_y >= ball_top && crossbar_y <= ball_bottom);
    if (top_of_goal2){
    ball_bottom = crossbar_y + 1;
    ball_velocity.y = -ball_velocity.y * 0.4;
    }
    _ball.set_velocity(ball_velocity);
    _ball.set_pos(ball_pos);
}

void PixFootyEngine::check_goal(N5110 &lcd) {
    //printf("Pong Engine: Check Goal\n");
    Position2D ball_pos = _ball.get_pos();
    Position2D player_pos = _player.get_pos();
    int height = _player.get_height();
    int width = _player.get_width();
    int goalline_x1 = 10;
    int goalline_x2 = 74;
    int crossbar_y = 19;
    int size = _ball.get_size();
    int ball_left = ball_pos.x;
    int ball_right = ball_pos.x + size;
    int ball_top = ball_pos.y;
    int ball_bottom = ball_pos.y + size;
   
    int speed = abs(_ball.get_velocity().x); 
    // check if ball position has gone off the left
    if (
        (ball_top > crossbar_y)&&
        (ball_left > goalline_x2)) {
        _goal_scored = true;
        user_goals++;
        _ball.set_velocity({0, 0});  
        _ball.init(size, 0);
        _player.init(24 - width, height, width);
        _dec.init(10, 9, 60, 28);
        _goal_timer.stop();
        _goal_timer.reset();
        _goal_timer.start();
    }
    if (
        (ball_top > crossbar_y)&&
        (ball_right < goalline_x1)) {
        _goal_scored = true;
        dec_goals++;
        _ball.set_velocity({0, 0});  
        _ball.init(size, 0);
        _player.init(24 - width, height, width);
        _dec.init(10, 9, 60, 28);
        _goal_timer.stop();
        _goal_timer.reset();
        _goal_timer.start();
    }
    }   

